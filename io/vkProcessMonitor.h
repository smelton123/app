#ifndef __VK_PROCESSMONITOR_H__
#define __VK_PROCESSMONITOR_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uv.h"

class ProcessMonitor final
{
public:
    static inline ProcessMonitor *i(void){ if (!m_self) { defaultInit(); } return m_self; };

    static void CreateInst(void);
    static void DestroyInst(void);
    static void start(void);
    static void stop(void);
    
private:
    ProcessMonitor(void);
    ~ProcessMonitor(void);
    static inline void defaultInit() {  m_self = new ProcessMonitor();}
    ProcessMonitor & operator = (const ProcessMonitor &); // do not add it's defination.
    static void onTimer(uv_timer_t *handle);
    static void onExit(uv_process_t *process, int64_t exit_status, int term_signal);

private:

    constexpr static int k_timeout = 10;//in s

    static uv_timer_t *m_timer;
    static uv_process_t *m_process;

    static ProcessMonitor *m_self;
    static uv_process_options_t m_options;
    char m_file[PATH_MAX];
    char *m_args[2];
    static int m_ticks;
};

#endif
