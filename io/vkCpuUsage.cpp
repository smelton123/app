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
cpu_counters_t CpuUsage::m_pre_counter = {0};
cpu_counters_t CpuUsage::m_cur_counter = {0};

uv_mutex_t CpuUsage::m_mutex = {0};

CpuUsage::CpuUsage(void)    
{
    int num = sysconf(_SC_NPROCESSORS_ONLN);
    assert(m_self==nullptr);

    m_pre_cpu = new_cpu_t(num);
    m_cur_cpu = new_cpu_t(num);

    read_stat(m_pre_cpu);
    read_stat(m_cur_cpu);

    read_cpu_counters(&m_pre_counter);
    read_cpu_counters(&m_cur_counter);

    uv_mutex_init(&m_mutex);

    m_timer = new uv_timer_t;
    uv_timer_init(uv_default_loop(), m_timer);    
    m_timer->data = this;
    m_self = this;

    uv_timer_start(m_timer, CpuUsage::onTimer, 500, 5000);
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

void CpuUsage::onTimer(uv_timer_t *handle)
{
    float cpu_usage = 0;
    float cores_load = 0;

    read_stat(m_cur_cpu);
    read_cpu_counters(&m_cur_counter);

    cores_load = cal_cpu_load(m_pre_cpu, m_cur_cpu);
    cpu_usage = CpuUsage::cpu_usage(&m_pre_counter,&m_cur_counter);
    cpu_copy(m_pre_cpu,m_cur_cpu);
    m_pre_counter=m_cur_counter;

    uv_mutex_lock(&m_mutex);
    m_all_cores_load = cores_load;
    m_cpu_usage = cpu_usage;
    uv_mutex_unlock(&m_mutex);
    //printf("%3.1f,  %3.1f,  %3.1f\n",cores_load, cpu_usage, cores_load/8);
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

/**
 * @brief
 *
 * @param[in] before 
 * @param[in] after  
 */
float CpuUsage::cal_cpu_load(const cpu_t *before, const cpu_t *after)
{
  cputime_t diff;
  int num = before->num;
  get_diff(&before->times[num], &after->times[num], &diff);
  uint64_t total  = get_total(&diff);
  uint64_t load   = get_load(&diff);
  float cpu_cores_load = 0;
  if (total == 0) {
    total = 1;
  }
  float usage = (float) load / total * 100; 

  //printf("%5.1f%% \n", usage);
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
      cpu_cores_load += usage;
      //printf("%5.1f%%", (float) load / total * 100);
    }
  }
  return cpu_cores_load;
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


int CpuUsage::read_cpu_counters(struct cpu_counters *cpu_cnt)
{
  FILE *f = NULL;
  char buf[256];
  char *rest = NULL, *token, *str;
  int ntok = 0;
  int err = 0;

  f = fopen("/proc/stat", "r");
  if (!f) {
    fprintf(stderr, "error: can't read the /proc/stat\n");
    return -1;
  }

  /* the cpu counters resides in the first line */
  if (!fgets(buf, 256, f)) {
    fprintf(stderr, "error: invalid cpu counters in /proc/stat \n");
    err = -1;
    goto out;
  }

  str = buf;
  memset(cpu_cnt, 0, sizeof(*cpu_cnt));
  while ((token = strtok_r(str, " ", &rest)) != NULL) {
    ++ntok;
    str = rest;
    /* skip the fist token */
    if (ntok == 1)
      continue;
    if (ntok < 5)
      cpu_cnt->work_jiffies += atoll(token);
    cpu_cnt->total_jiffies += atoll(token);
  }

out:
  fclose(f);
  return err;
}

float CpuUsage::cpu_usage(const struct cpu_counters *cpu_cnt_start, const struct cpu_counters *cpu_cnt_end)
{
  return ((float)(cpu_cnt_end->work_jiffies - cpu_cnt_start->work_jiffies) /
    (float)(cpu_cnt_end->total_jiffies - cpu_cnt_start->total_jiffies)) * 100;
}

