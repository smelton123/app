#ifndef __VK_PSWATCHER_H__
#define __VK_PSWATCHER_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uv.h"

class PsWatcher final
{
public:
    static inline PsWatcher *getInstance(void){ if (!m_self) { DefaultInit(); } return m_self; };

    static void CreateInst(void);
    static void DestroyInst(void);
    void Start(void);
    void Stop(void);
    
private:
    PsWatcher(void);
    ~PsWatcher(void);
    static inline void DefaultInit() {  m_self = new PsWatcher();}
    PsWatcher & operator = (const PsWatcher &); // do not add it's defination.
    static void OnTimer(uv_timer_t *handle);
    static void OnExit(uv_process_t *process, int64_t exit_status, int term_signal);

private:

    //in seconds,every (k_timeout) seconds, check the cpu usage.
    constexpr static int k_timeout = 15;
    //in seconds,every (seconds*k_update_timeout) seconds, check upgrade.
    constexpr static int k_update_timeout = 60;

    static uv_timer_t *m_timer;
    static uv_process_t *m_process;

    static PsWatcher *m_self;
    static uv_process_options_t m_options;
    char m_file[PATH_MAX];
    char *m_args[2];
    static int m_ticks;
};

#endif
