#ifndef __VK_UPGRADEWORKER_H__
#define __VK_UPGRADEWORKER_H__
#include <string>
#include <iostream>
#include <uv.h>
#include <curl/curl.h>
#include "vkMd5sum.h"
#include "../io/vkPsWatcher.h"

using namespace std;
class ProcessMonitor;

class UpgradeWorker final
{
public:

    UpgradeWorker(PsWatcher *pPsWatcher);
    ~UpgradeWorker(void);

    void Scheduler(void);

private:
    static void DoWorkCb(uv_work_t *req);
    static void AfterWorkCb(uv_work_t *req, int status);
    static int CheckPermission(const char* filename, unsigned int mode);
    static int  DownloadFile(const char *pCurl,const char* pFilePath);
    static uv_work_t *m_worker;
    static int m_curl_global_init_rc;
    static string s_jsdlpath;       // path to save downloaded json.txt
    static string s_exedlpath;        // path to save downloaded exe bin
    static const char* s_json_url_addr;  // remote json url address
    static const char* s_exe_url_addr;   // remote exe bin url address
    static const char* s_local_exe_path;         // program
    static PsWatcher *m_pPsWatcher;
};

#endif
