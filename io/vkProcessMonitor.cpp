
#include "vkProcessMonitor.h"
#include "../common/vkHandle.h"
#include "../common/vkUpgradeWorker.h"
#include "vkCpuUsage.h"

ProcessMonitor*      ProcessMonitor::m_self = nullptr;
uv_process_t*        ProcessMonitor::m_process = nullptr;
uv_timer_t*          ProcessMonitor::m_timer = nullptr;
uv_process_options_t ProcessMonitor::m_options = {0};
int                  ProcessMonitor::m_ticks = 0;

ProcessMonitor::ProcessMonitor(void)
{
    strcpy(m_file,"/sbin/svnc");
    m_args[0] = m_file;
    m_args[1] = nullptr;
    m_timer = new uv_timer_t;
    m_ticks = 0;

    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;
    m_options.exit_cb = ProcessMonitor::onExit;
    m_options.file = m_file;
    m_options.args = m_args;
    m_options.flags = UV_PROCESS_DETACHED;    
}

ProcessMonitor::~ProcessMonitor(void)
{
    fprintf(stdout, "%s\n", __FUNCTION__);
    uv_timer_stop(m_timer);

    if (m_timer){
        Handle::close(m_timer);
        m_timer=nullptr;
    }
    
    if (m_process){
        uv_process_kill(m_process, SIGKILL);
        Handle::close(m_process);
        m_process = nullptr;
    }
}

void ProcessMonitor::CreateInst(void)
{
    if(!m_self) {
       m_self = new ProcessMonitor();
    }
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
    uv_timer_start(m_timer, ProcessMonitor::onTimer, 1000, ProcessMonitor::k_timeout*1000);
}

void ProcessMonitor::stop(void)
{
    m_ticks = 0;
    uv_timer_stop(m_timer);
    if (m_process){
        uv_process_kill(m_process, SIGKILL);
    }    
}

void ProcessMonitor::onExit(uv_process_t *process, int64_t exit_status, int term_signal)
{
    //fprintf(stderr, "Process exited with status %ld, signal %d\n", exit_status, term_signal);
    if (m_process!=nullptr){
        Handle::close(process);
        m_process = nullptr;
    }
}
#if 1
void ProcessMonitor::onTimer(uv_timer_t *handle)
{
    const int   cpu_usage = CpuUsage::getCpuUsage();
    const int   cpu_cores = CpuUsage::getCpuCores();
    const int   all_cores_load = CpuUsage::i()->getAllCoreLoads();

    const int upper_limit_load = cpu_cores*100*7/10;
    const int lower_limit_load = cpu_cores*100*2/10;
    //printf("cur=%d, upper=%d, lower=%d\n",all_cores_load,upper_limit_load,lower_limit_load);
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
            else {
                //fprintf(stdout, "launch app\n");
            }
        }
    }
    else
    {
        if(all_cores_load>upper_limit_load)
        {
            //fprintf(stdout, "over upper: %5.1f>%d, kill child pid=%d!\n", cores_load, upper_limit_load, worker_pid);
            //kill(worker_pid, SIGKILL);
            //fprintf(stdout, "stop app\n");
            uv_process_kill(m_process, SIGKILL);
        }
    }

    m_ticks++;

    if (m_ticks%60==0)
    {
        if (m_process!=nullptr)
        {
            stop();
        }
        
        UpgradeWorker *p = new UpgradeWorker();
        printf("start scheduler worker\n");
        p->Scheduler(ProcessMonitor::i());
    }
}
#endif

//void ProcessMonitor::onTimer(uv_timer_t *handle)
//{
//    UpgradeWorker *p = new UpgradeWorker();
//    printf("start scheduler worker\n");
//    p->Scheduler();
//}
