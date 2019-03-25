#include "vkProcessMonitor.h"
#include "../common/vkHandle.h"

ProcessMonitor* ProcessMonitor::m_self = nullptr;

ProcessMonitor::ProcessMonitor(void)
{
    m_timer = new uv_timer_t;
    m_process = new uv_process_t;
    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;
}

ProcessMonitor::~ProcessMonitor(void)
{
    uv_timer_stop(m_timer);
    delete m_timer;
    delete m_process;
    Handle::close(m_timer);
}

void ProcessMonitor::CreateInst(void)
{
    if(!m_self)
       m_self = new ProcessMonitor();
}

void ProcessMonitor::DestroyInst(void)
{
    if(m_self){
        delete m_self;
        m_self = nullptr;
    }
}

void ProcessMonitor::onExit(uv_process_t *process, int64_t exit_status, int term_signal)
{
    fprintf(stdout, "Process exited with status %PRId64, signal %d\n", exit_status, term_signal);
    Handle::close(process);
}

void ProcessMonitor::onTimer(uv_timer_t *handle)
{

}


