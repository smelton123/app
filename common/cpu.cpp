#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include "cpu.h"


#define LINE_BUFFER_SIZE 1024
//#define SWAP(a, b) {void *tmp = a; a = b; b = tmp;}

static void *xmalloc(size_t size);
static cpu_t *new_cpu_t(int num);
static void delete_cpu_t(cpu_t *cpu);
static uint64_t get_total(cputime_t *time);
static uint64_t get_load(cputime_t *time);
static void get_diff(cputime_t *before, cputime_t *after, cputime_t *diff);
static result_t read_stat(cpu_t *cpu);
static void cal_cpu_load(cpu_t *before, cpu_t *after, float *total_usage, float *sum_of_core_load);


static void *xmalloc(size_t size) 
{
  void *p = malloc(size);
  if (p == NULL) {
    ERR("%s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return p;
}

static cpu_t *new_cpu_t(int num) 
{
  cpu_t *cpu;
  cpu = (cpu_t*)xmalloc(sizeof(cpu_t));
  cpu->num = num;
  cpu->times = (cputime_t*)xmalloc((num + 1) * sizeof(cputime_t));
  return cpu;
}


static void delete_cpu_t(cpu_t *cpu) 
{
  if (cpu == NULL) {
    return;
  }
  free(cpu->times);
  free(cpu);
}


static uint64_t get_total(cputime_t *time)
{
  return time->user + time->nice + time->system
      + time->idle + time->iowait + time->irq + time->softirq
      + time->steal + time->guest + time->guest_nice;
}

static uint64_t get_load(cputime_t *time) 
{
  return time->user + time->nice + time->system
      + time->irq + time->softirq
      + time->steal + time->guest + time->guest_nice;
}


static void get_diff(cputime_t *before, cputime_t *after, cputime_t *diff) 
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

static result_t read_stat(cpu_t *cpu)
{
  result_t result = FAILURE;
  cputime_t *work;
  FILE *file;
  char line[LINE_BUFFER_SIZE];
  file = fopen("/proc/stat", "rb");
  if (file == NULL) {
    ERR("%s\n", strerror(errno));
    return FAILURE;
  }
  if (fgets(line, sizeof(line), file) == NULL) {
    ERR("%s\n", strerror(errno));
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
    ERR("%s\n", strerror(errno));
    goto error;
  }
  if (cpu->num > 1) {
    int i;
    for (i = 0; i < cpu->num; i++) {
      if (fgets(line, sizeof(line), file) == NULL) {
        ERR("%s\n", strerror(errno));
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
        ERR("%s\n", strerror(errno));
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
static void cal_cpu_load(cpu_t *before, cpu_t *after, float *total_usage, float *sum_of_core_load)
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
  //printf("%5.1f%% ", usage);
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
      usage = (float) load / total * 100;
      *sum_of_core_load += usage;
      //printf("%5.1f%%", (float) load / total * 100);
    }
  }
  //printf("\n");
}

/*
*
*@param[out] total_usage: total usage of all cpu, in percent(0~100%)
*@param[out] sum_usage: sum usage of all core, in percent(0~100%*(num of cpus))
*/
int get_cpu_usage(float *total_usage, float *sum_usage)
{
    int result = -1;
    cpu_t *after;
    cpu_t *before;
    
    int num = sysconf(_SC_NPROCESSORS_ONLN);
    after = new_cpu_t(num);
    before = new_cpu_t(num);

    if (read_stat(before) != SUCCESS) {
        goto error;
    }
  
    usleep(500*1000);
    
    if (read_stat(after) != SUCCESS) {
        goto error;
    }
    
    cal_cpu_load(before, after, total_usage, sum_usage);
    result = 0;
error:
    delete_cpu_t(before);
    delete_cpu_t(after);
    return result;
}


