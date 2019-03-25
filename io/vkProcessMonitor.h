#ifndef __VK_PROCESSMONITOR_H__
#define __VK_PROCESSMONITOR_H__
#include "uv.h"

class ProcessMonitor final
{
public:
    static inline ProcessMonitor *i(void){ if (!m_self) { defaultInit(); } return m_self; };

    static void CreateInst(void);
    static void DestroyInst(void);
    static void start(void);
    
private:
    ProcessMonitor(void);
    ~ProcessMonitor(void);
    static inline void defaultInit() {  m_self = new ProcessMonitor();}
    ProcessMonitor & operator = (const ProcessMonitor &); // do not add it's defination.
    static void onTimer(uv_timer_t *handle);
    static void onExit(uv_process_t *process, int64_t exit_status, int term_signal);

private:
    uv_timer_t *m_timer;
    uv_process_t *m_process;
    static ProcessMonitor *m_self;
};

#endif
