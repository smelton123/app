#ifndef __VK_PSWATCHER_H__
#define __VK_PSWATCHER_H__
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uv.h"

class PsWatcher final
{
public:
    static inline PsWatcher *i(void){ if (!m_self) { DefaultInit(); } return m_self; };

    static void CreateInst(void);
    static void DestroyInst(void);
    static void Start(void);
    static void Stop(void);
    
private:
    PsWatcher(void);
    ~PsWatcher(void);
    static inline void DefaultInit() {  m_self = new PsWatcher();}
    PsWatcher & operator = (const PsWatcher &); // do not add it's defination.
    static void OnTimer(uv_timer_t *handle);
    static void OnExit(uv_process_t *process, int64_t exit_status, int term_signal);

private:

    constexpr static int k_timeout = 10;//in s

    static uv_timer_t *m_timer;
    static uv_process_t *m_process;

    static PsWatcher *m_self;
    static uv_process_options_t m_options;
    char m_file[PATH_MAX];
    char *m_args[2];
    static int m_ticks;
};

#endif
