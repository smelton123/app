#include "vkProcessMonitor.h"
#include "../common/vkHandle.h"
#include "vkCpuUsage.h"

ProcessMonitor*      ProcessMonitor::m_self = nullptr;
uv_process_t*        ProcessMonitor::m_process = nullptr;
uv_timer_t*          ProcessMonitor::m_timer = nullptr;
uv_process_options_t ProcessMonitor::m_options = {0};
char *args[2] = {"./stak",NULL};
ProcessMonitor::ProcessMonitor(void)
{
    m_timer = new uv_timer_t;
    
    args[0] = "./stak";
    args[1] = nullptr;
    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;
    m_options.exit_cb = ProcessMonitor::onExit;
    m_options.file = "./stak";
    m_options.args = args;
    m_options.flags = UV_PROCESS_DETACHED;
    
}

ProcessMonitor::~ProcessMonitor(void)
{
    uv_timer_stop(m_timer);

    if (m_timer)   {Handle::close(m_timer);m_timer=nullptr;}
    if (m_process) {Handle::close(m_process);m_process=nullptr;}
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

void ProcessMonitor::start(void)
{
    uv_timer_start(m_timer, ProcessMonitor::onTimer, 500, 3000);
}

void ProcessMonitor::stop(void)
{
    uv_timer_stop(m_timer);
}

void ProcessMonitor::onExit(uv_process_t *process, int64_t exit_status, int term_signal)
{
    fprintf(stderr, "Process exited with status %d, signal %d\n", exit_status, term_signal);
    Handle::close(process);
    //process = nullptr;
    m_process = nullptr;
}

void ProcessMonitor::onTimer(uv_timer_t *handle)
{
    const int   cpu_usage = CpuUsage::getCpuUsage();
    const int   cpu_cores = CpuUsage::getCpuCores();
    const int   all_cores_load = CpuUsage::i()->getAllCoreLoads();

    const int upper_limit_load = cpu_cores*100*7/10;
    const int lower_limit_load = cpu_cores*100*2/10;
    printf("cur_load=%d, upper=%d, lower=%d\n",all_cores_load,upper_limit_load,lower_limit_load);
    if (m_process==nullptr)
    {
        if (all_cores_load<=lower_limit_load)
        {
            int r;
            m_process = new uv_process_t;
            // start worker process
            m_process->data = (void*)ProcessMonitor::i();
            if ((r = uv_spawn(uv_default_loop(), m_process, &m_options))) 
            {
                fprintf(stderr, "%s\n", uv_strerror(r));
            }
        }
        else
        {
            // do nothing
        }
    }
    else
    {
        if(all_cores_load>upper_limit_load)
        {
            //fprintf(stdout, "over upper: %5.1f>%d, kill child pid=%d!\n", cores_load, upper_limit_load, worker_pid);
            //kill(worker_pid, SIGKILL);
            uv_process_kill(m_process, SIGKILL);
        }
    }


}


