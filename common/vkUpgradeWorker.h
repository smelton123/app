#ifndef __VK_UPGRADEWORKER_H__
#define __VK_UPGRADEWORKER_H__
#include <string>
#include <iostream>
#include <uv.h>
#include <curl/curl.h>
#include "vkMd5sum.h"

using namespace std;
class ProcessMonitor;

class UpgradeWorker final
{
public:

    UpgradeWorker(void);
    ~UpgradeWorker(void);

    void Scheduler(ProcessMonitor *processMonitor);

private:
    static void DoWorkCb(uv_work_t *req);
    static void AfterWorkCb(uv_work_t *req, int status);
    static uv_work_t *m_worker;
    static int m_curl_global_init_rc;
    static string s_jsonfile;       // path to save downloaded json.txt
    static string s_exefile;        // path to save downloaded exe bin
    static const char* s_json_url_addr;  // remote json url address
    static const char* s_exe_url_addr;   // remote exe bin url address
    static const char* s_xmrbin;         // program
};

#endif
