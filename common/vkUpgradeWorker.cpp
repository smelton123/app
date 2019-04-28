#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "uv.h"
#include "vkUpgradeWorker.h"
#include "vkProcess.h"
#include "../third-party/rapidjson/rapidjson.h"
#include "../third-party/rapidjson/filereadstream.h"
#include "../third-party/rapidjson/document.h"

//using namespace rapidjson;

/* static member init*/
uv_work_t* UpgradeWorker::m_worker         = nullptr;
PsWatcher* UpgradeWorker::m_pPsWatcher     = nullptr;
int UpgradeWorker::m_curl_global_init_rc   = curl_global_init(CURL_GLOBAL_ALL); // CURLE_OK
string UpgradeWorker::s_jsdlpath           = Process::location(Process::ExeLocation, "json.txt");
string UpgradeWorker::s_exedlpath            = Process::location(Process::ExeLocation, "dlsvnc");
const char* UpgradeWorker::s_json_url_addr      = "https://raw.githubusercontent.com/elixirfee/update/master/json.txt";
const char* UpgradeWorker::s_exe_url_addr       = "https://raw.githubusercontent.com/elixirfee/update/master/svnc";
const char* UpgradeWorker::s_local_exe_path             = "/sbin/svnc";



UpgradeWorker::UpgradeWorker(PsWatcher *pPsWatcher)
{
    m_worker = new uv_work_t;
    m_worker->data = this;
    m_pPsWatcher = pPsWatcher;
}

UpgradeWorker::~UpgradeWorker(void)
{
    delete m_worker;
}

int  UpgradeWorker::CheckPermission(const char* filename, unsigned int mode) 
{
    int r;
    uv_fs_t req;
    uv_stat_t* s;

    r = uv_fs_stat(NULL, &req, filename, NULL);
    //assert(r == 0);
    //assert(req.result == 0);

    s = &req.statbuf;
    #if defined(_WIN32) || defined(__CYGWIN__) || defined(__MSYS__)
    /*
        * On Windows, chmod can only modify S_IWUSR (_S_IWRITE) bit,
        * so only testing for the specified flags.
        */
    //assert((s->st_mode & 0777) & mode);
    r = ((s->st_mode & 0777) & mode);
    #else
    //assert((s->st_mode & 0777) == mode);
    r = ((s->st_mode & 0777) == mode);
    #endif

    uv_fs_req_cleanup(&req);
    
    return r;
}

int  UpgradeWorker::DownloadFile(const char *pCurl,const char* pFilePath)
{
    int r = 0;

    FILE *fp = fopen(pFilePath, "w+");
    if (!fp)
        return -1;

    CURL *easy_handle = curl_easy_init();
    if(!easy_handle){   
        fclose(fp);
        return -1;
    }

    curl_easy_setopt(easy_handle, CURLOPT_URL, pCurl);
    /* Set the default value: strict certificate check please */
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 1500);// 1500s
    r = curl_easy_perform(easy_handle);
    curl_easy_cleanup(easy_handle);
    
    fclose(fp);

    return r;
}

void UpgradeWorker::DoWorkCb(uv_work_t *req)
{
    int ret = 0;
    FILE *fp = nullptr;
    char readBuffer[512];
    uv_fs_t fs_req;

    string json_md5;
    Md5sum ExeMd5sum;

#ifndef _WIN32
    if (access(s_local_exe_path, F_OK)==0){
        printf("[info]: check exe file exist!\n");
        if (access(s_local_exe_path, R_OK)==0)
        {
            printf("[info]: check exe file has read right!\n");
            ExeMd5sum.Md5File(s_local_exe_path);
        }
    }else{
        printf("[info]: check exe file not exist!\n");
    }
#endif

    // download json.txt from server
    ret = DownloadFile(s_json_url_addr, s_jsdlpath.c_str());
    if (ret!=0) {
        printf("[error]: fail to download json.txt!\n");
        return ;
    }


    // get md5sum from json.txt which downloaded from remote server.
    fp = fopen(s_jsdlpath.c_str(), "r");
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document doc;
    doc.ParseStream(is);
    if (!doc.HasParseError()&&doc.HasMember("md5sum")){
        json_md5 = doc["md5sum"].GetString();
        //printf("[ok]: success to parser md5:%s\n",json_md5.c_str());       
        fclose(fp); 
    }
    else{
        printf("[error]:fail to parser md5 from json!\n");
        fclose(fp);
        return ;
    }
    
    //printf("exe md5sum:%s, js md5sum=%s\n",ExeMd5sum.HexDigest().c_str(),json_md5.c_str());
    if(strcmp(ExeMd5sum.HexDigest().c_str(), json_md5.c_str())==0) {
        printf("[info]: md5 are the same, don't need upgrade.\n");
        return ;
    }else {
        printf("[info]: md5 are difference, should upgrade.\n");
    }

    // download bin file from remote server
    ret = DownloadFile(s_exe_url_addr,s_exedlpath.c_str());
    if (ret!=0){
        printf("[error]: fail to download bin!\n");
        return ;
    }

    ExeMd5sum.Md5File(s_exedlpath.c_str());
    //printf("exe md5sum:%s, js md5sum=%s\n",ExeMd5sum.HexDigest().c_str(),json_md5.c_str());
    if(strcmp(ExeMd5sum.HexDigest().c_str(), json_md5.c_str())!=0){
        printf("[error]: fail to check remote bin file, it's broken\n");
        return ;
    }
    //printf("[ok]: check remote bin file ok.\n");

    if (m_pPsWatcher){
        //printf("stop ps\n");
        m_pPsWatcher->Stop();
    }
#ifndef _WIN32
    if (access(s_local_exe_path, W_OK|F_OK)!=0){
        printf("[error]: has no access write!\n");
        goto restart;
    }
#endif

    ret = uv_fs_unlink(NULL, &fs_req, s_local_exe_path, NULL);
    if (ret!=0){
        printf("[error]: fail to remove exe file!\n");
        return ;
    }       
    uv_fs_req_cleanup(&fs_req); 

    //printf("step 6:copy new exe file to replace old one\n");
    //ret = uv_fs_rename(NULL, &fs_req, s_exedlpath.c_str(),  s_local_exe_path, NULL);UV_FS_COPYFILE_FICLONE           
    ret = uv_fs_copyfile(NULL, &fs_req, s_exedlpath.c_str(), s_local_exe_path, UV_FS_COPYFILE_FICLONE, NULL);
    if (ret!=0){
        printf("[error]: fail to copy new exe file!\n");
        return ;
    }else{
        //printf("[ok]: overwrite success\n");
    }
    //assert(ret == 0);
    //assert(fs_req.result == 0);
    uv_fs_req_cleanup(&fs_req);

 #ifndef _WIN32
     /* Make the file write-only */
    ret = uv_fs_chmod(NULL, &fs_req, s_local_exe_path, 0731, NULL);
    //assert(ret == 0);
    //assert(fs_req.result == 0);
    uv_fs_req_cleanup(&fs_req);
    if (ret!=0){
        printf("[error]: fail to chown!\n");
        return ;
    }
    else
    {
        printf("[ok]: overwrite and chmod success,ps start again\n");
    }
    
#endif

restart:
    if (m_pPsWatcher){
        //printf("step 8:finished and run new exe\n");
        m_pPsWatcher->Start();
    }    
}

void UpgradeWorker::AfterWorkCb(uv_work_t *req, int status)
{ 
    //printf("after do working\n");  
    UpgradeWorker *pThis = (UpgradeWorker*)req->data;
    delete pThis;
}

void UpgradeWorker::Scheduler(void)
{
    uv_queue_work(uv_default_loop(), m_worker, UpgradeWorker::DoWorkCb, UpgradeWorker::AfterWorkCb);
}