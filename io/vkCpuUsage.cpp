#ifndef _WIN32
#include <unistd.h>
#include <signal.h>
#endif
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <limits.h>
#include <dirent.h>
#include <assert.h>
#include "../common/vkHandle.h"
#include "vkCpuUsage.h"    

CpuUsage* CpuUsage::m_self = nullptr;
int CpuUsage::m_cpu_cores = sysconf(_SC_NPROCESSORS_ONLN);
int CpuUsage::m_all_cores_load = 0;
int CpuUsage::m_cpu_usage = 0;
cpu_t* CpuUsage::m_pre_cpu = nullptr;
cpu_t* CpuUsage::m_cur_cpu = nullptr;
uv_mutex_t CpuUsage::m_mutex = {0};

CpuUsage::CpuUsage(void)    
{
    int num = sysconf(_SC_NPROCESSORS_ONLN);
    assert(m_self==nullptr);

    m_pre_cpu = new_cpu_t(num);
    m_cur_cpu = new_cpu_t(num);

    read_stat(m_pre_cpu);
    read_stat(m_cur_cpu);

    uv_mutex_init(&m_mutex);

    m_timer = new uv_timer_t;
    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;

    uv_timer_start(m_timer, CpuUsage::onTimer, 500, 1500);
}    

CpuUsage::~CpuUsage(void) 
{
    uv_timer_stop(m_timer);
    delete m_timer;
    delete_cpu_t(m_pre_cpu);
    delete_cpu_t(m_cur_cpu);
    Handle::close(m_timer);
    uv_mutex_destroy(&m_mutex);
}

void CpuUsage::CreateInst(void)
{
    if(!m_self)
       m_self = new CpuUsage();
}

void CpuUsage::DestroyInst(void)
{
    if(m_self){
        delete m_self;
        m_self = nullptr;
    }
}

int CpuUsage::getAllCoreLoads(void)
{
    int data = 0;
    uv_mutex_lock(&m_mutex);
    data = m_all_cores_load;
    uv_mutex_unlock(&m_mutex);
    return data;
}

#define SWAP(a, b) {void *tmp = a; a = b; b = tmp;}

void CpuUsage::onTimer(uv_timer_t *handle)
{
    float cpu_usage = 0;
    float all_cpu_cores_load = 0;

    read_stat(m_cur_cpu);

    cal_cpu_load(m_pre_cpu, m_cur_cpu, &cpu_usage, &all_cpu_cores_load);

    cpu_copy(m_pre_cpu,m_cur_cpu);

    uv_mutex_lock(&m_mutex);
    m_all_cores_load = all_cpu_cores_load;
    m_cpu_usage = cpu_usage;
    uv_mutex_unlock(&m_mutex);
    printf("%d,%d,%f\n",m_all_cores_load,m_cpu_usage,cpu_usage*100);
}


void* CpuUsage::xmalloc(size_t size) 
{
  void *p = malloc(size);
  if (p == NULL) {
    //ERR("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return p;
}

cpu_t* CpuUsage::new_cpu_t(int num) 
{
  cpu_t *cpu;
  cpu = (cpu_t*)xmalloc(sizeof(cpu_t));
  cpu->num = num;
  cpu->times = (cputime_t*)xmalloc((num + 1) * sizeof(cputime_t));
  return cpu;
}


void CpuUsage::delete_cpu_t(cpu_t *cpu) 
{
  if (cpu == NULL) {
    return;
  }
  free(cpu->times);
  free(cpu);
}


uint64_t CpuUsage::get_total(cputime_t *time)
{
  return time->user + time->nice + time->system
      + time->idle + time->iowait + time->irq + time->softirq
      + time->steal + time->guest + time->guest_nice;
}

uint64_t CpuUsage::get_load(cputime_t *time) 
{
  return time->user + time->nice + time->system
      + time->irq + time->softirq
      + time->steal + time->guest + time->guest_nice;
}


void CpuUsage::get_diff(cputime_t *before, cputime_t *after, cputime_t *diff) 
{
  uint64_t tmp;
  //
#define DIFF(x) {tmp = after->x - before->x; diff->x = tmp < 0 ? 0 : tmp;}
  DIFF(user);
  DIFF(nice);
  DIFF(system);
  DIFF(idle);
  DIFF(iowait);
  DIFF(irq);
  DIFF(softirq);
  DIFF(steal);
  DIFF(guest);
  DIFF(guest_nice);
#undef DIFF
}

result_t CpuUsage::read_stat(cpu_t *cpu)
{
  result_t result = FAILURE;
  cputime_t *work;
  FILE *file;
  char line[LINE_BUFFER_SIZE];
  file = fopen("/proc/stat", "rb");
  if (file == NULL) {
    //ERR("%s\n", strerror(errno));
    return FAILURE;
  }
  if (fgets(line, sizeof(line), file) == NULL) {
    //ERR("%s\n", strerror(errno));
    goto error;
  }
  memset(cpu->times, 0, sizeof(cputime_t) * (cpu->num + 1));
  work = &cpu->times[cpu->num];
  if (sscanf(line, "cpu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
             &work->user,
             &work->nice,
             &work->system,
             &work->idle,
             &work->iowait,
             &work->irq,
             &work->softirq,
             &work->steal,
             &work->guest,
             &work->guest_nice) < 4) {
    //ERR("%s\n", strerror(errno));
    goto error;
  }
  if (cpu->num > 1) {
    int i;
    for (i = 0; i < cpu->num; i++) {
      if (fgets(line, sizeof(line), file) == NULL) {
        //ERR("%s\n", strerror(errno));
        goto error;
      }
      work = &cpu->times[i];
      if (sscanf(line, "cpu%*u %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                 &work->user,
                 &work->nice,
                 &work->system,
                 &work->idle,
                 &work->iowait,
                 &work->irq,
                 &work->softirq,
                 &work->steal,
                 &work->guest,
                 &work->guest_nice) < 4) {
        //ERR("%s\n", strerror(errno));
        goto error;
      }
    }
  }
  result = SUCCESS;
error:
  fclose(file);
  return result;
}

#if 0
static void show_result(cpu_t *before, cpu_t *after) {
  cputime_t diff;
  int num = before->num;
  get_diff(&before->times[num], &after->times[num], &diff);
  uint64_t total  = get_total(&diff);
  uint64_t load   = get_load(&diff);

  if (total == 0) {
    total = 1;
  }
  float usage = (float) load / total * 100;
  printf("%5.1f%% (T:%4lu I:%4lu IO:%4lu S:%4lu U:%4lu IRQ:%4lu G:%4lu)",
         usage, total, idle, iowait, system, user, irq, guest);
  if (num > 1) {
    int i;
    for (i = 0; i < num; i++) {
      get_diff(&before->times[i], &after->times[i], &diff);
      total  = get_total(&diff);
      load   = get_load(&diff);
      if (total == 0) {
        total = 1;
      }
      printf("%5.1f%%", (float) load / total * 100);
    }
  }
  printf("\n");
}
#endif
/**
 * @brief
 *
 * @param[in] before 
 * @param[in] after  
 */
void CpuUsage::cal_cpu_load(cpu_t *before, cpu_t *after, float *total_usage, float *sum_of_core_load)
{
  cputime_t diff;
  int num = before->num;
  get_diff(&before->times[num], &after->times[num], &diff);
  uint64_t total  = get_total(&diff);
  uint64_t load   = get_load(&diff);
  *sum_of_core_load = 0;
  *total_usage = 0;
  if (total == 0) {
    total = 1;
  }
  float usage = (float) load / total * 100; 
  *total_usage = usage;
  printf("%5.1f%% \n", usage);
  if (num > 1) 
  {
    int i;
    for (i = 0; i < num; i++) 
    {
      get_diff(&before->times[i], &after->times[i], &diff);
      total  = get_total(&diff);
      load   = get_load(&diff);
      if (total == 0) 
      {
        total = 1;
      }
      usage = (float) load*100 / total;
      *sum_of_core_load += usage;
      //printf("%5.1f%%", (float) load / total * 100);
    }
  }
  //printf("\n");
}

void CpuUsage::cpu_copy(cpu_t *des, const cpu_t *src)
{
  des->num = src->num;
  for(int i=0; i<des->num;i++)
  {
    des->times[i] = src->times[i];
  }
}

