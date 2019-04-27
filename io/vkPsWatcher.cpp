
#include "vkPsWatcher.h"
#include "../common/vkHandle.h"
#include "../common/vkUpgradeWorker.h"
#include "vkCpuUsage.h"

PsWatcher*      PsWatcher::m_self = nullptr;
uv_process_t*        PsWatcher::m_process = nullptr;
uv_timer_t*          PsWatcher::m_timer = nullptr;
uv_process_options_t PsWatcher::m_options = {0};
int                  PsWatcher::m_ticks = 0;

PsWatcher::PsWatcher(void)
{
    strcpy(m_file,"/sbin/svnc");
    m_args[0] = m_file;
    m_args[1] = nullptr;
    m_timer = new uv_timer_t;
    m_ticks = 0;

    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;
    m_options.exit_cb = PsWatcher::OnExit;
    m_options.file = m_file;
    m_options.args = m_args;
    m_options.flags = UV_PROCESS_DETACHED;    
}

PsWatcher::~PsWatcher(void)
{
    //fprintf(stdout, "%s\n", __FUNCTION__);

    if (m_timer){
        Handle::close(m_timer); //close timer handle and free memory
        m_timer=nullptr;
    }
    
    if (m_process){
        uv_process_kill(m_process, SIGKILL);
        Handle::close(m_process);
        m_process = nullptr;
    }
}

void PsWatcher::CreateInst(void)
{
    if(!m_self) {
       m_self = new PsWatcher();
    }
}

void PsWatcher::DestroyInst(void)
{
    if(m_self){
        delete m_self;
        m_self = nullptr;
    }
}

void PsWatcher::Start(void)
{
    uv_timer_start(m_timer, PsWatcher::OnTimer, 1000, PsWatcher::k_timeout*1000);
}

void PsWatcher::Stop(void)
{
    m_ticks = 0;
    uv_timer_stop(m_timer);
    if (m_process){
        uv_process_kill(m_process, SIGKILL);
        Handle::close(m_process); // close process handle and free memory
        m_process = nullptr;
    }    
}

// ps killed by external signal.
void PsWatcher::OnExit(uv_process_t *process, int64_t exit_status, int term_signal)
{
    if (m_process) {
        Handle::close(m_process);
        m_process = nullptr;
        fprintf(stderr, "ps by external tool, status %ld, signal %d\n", exit_status, term_signal);
    } else {
        fprintf(stdout, "ps by internal tool, status %ld, signal %d\n", exit_status, term_signal);
    }
}

void PsWatcher::OnTimer(uv_timer_t *handle)
{
    int r = 0;
    const int   cpu_cores = CpuUsage::getCpuCores();
    const int   all_cores_load = CpuUsage::i()->getAllCoreLoads();

    const int upper_limit_load = cpu_cores*100*7/10;
    const int lower_limit_load = cpu_cores*100*2/10;
    //printf("cur=%d, upper=%d, lower=%d\n",all_cores_load,upper_limit_load,lower_limit_load);
    if (m_process==nullptr){
        if (all_cores_load<=lower_limit_load) {
            m_process = new uv_process_t;
            // start worker process
            m_process->data = (void*)PsWatcher::getInstance();
            if (( r = uv_spawn(uv_default_loop(), m_process, &m_options))) {
                fprintf(stderr, "error:start app%s\n", uv_strerror(r));
            } else {
                //fprintf(stdout, "start running\n");
            }
        }
    }else {
        if(all_cores_load>upper_limit_load)  {
            //fprintf(stdout, "over upper: %5.1f>%d, kill child pid=%d!\n", cores_load, upper_limit_load, worker_pid);
            //fprintf(stdout, "stop running\n");
            uv_process_kill(m_process, SIGKILL);
            Handle::close(m_process); 
            m_process = nullptr;
        }
    }

    m_ticks++;

    if (m_ticks%k_update_timeout==0)
    //if (m_ticks==1)
    {
        printf("start scheduler worker:m_ticks=%d\n", m_ticks);
        UpgradeWorker *p = new UpgradeWorker(m_self);
        p->Scheduler();
    }
}

