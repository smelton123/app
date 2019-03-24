#ifndef __VK_CPUUSAGE_H__
#define __VK_CPUUSAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <uv.h>

#ifndef TRUE
#define TRUE  1 
#endif

#ifndef FALSE
#define FALSE 0 
#endif

typedef enum result_t 
{
  SUCCESS = 0, 
  FAILURE = -1, 
} result_t;

typedef struct cputime_t 
{
  uint64_t user;       /** */
  uint64_t nice;       /** */
  uint64_t system;     /** */
  uint64_t idle;       /** */
  uint64_t iowait;     /** */
  uint64_t irq;        /** */
  uint64_t softirq;    /** */
  uint64_t steal;      /** */
  uint64_t guest;      /** */
  uint64_t guest_nice; /** */
} cputime_t;

typedef struct cpu_t 
{
  int num;            /** number of CPU */
  cputime_t *times;   /** */
} cpu_t;

class CpuUsage final
{
public:
    static inline CpuUsage *i(void){ if (!m_self) { defaultInit(); } return m_self; };

    static void CreateInst(void);
    static void DestroyInst(void);
    int getAllCoreLoads(void) ;
    static inline int getCpuCores(void)  {return m_cpu_cores;}
    static inline int getCpuUsage(void)  {return m_cpu_usage;}
    //bool isRunning(void);
    constexpr static const size_t LINE_BUFFER_SIZE = 1024;

private:
    CpuUsage(void);
    ~CpuUsage(void);
    static inline void defaultInit() {  m_self = new CpuUsage();}
    CpuUsage & operator = (const CpuUsage &); // do not add it's defination.
    static void onTimer(uv_timer_t *handle);

    // cpu related function.
    static void *xmalloc(size_t size);
    static cpu_t *new_cpu_t(int num);
    static void delete_cpu_t(cpu_t *cpu);
    static uint64_t get_total(cputime_t *time);
    static uint64_t get_load(cputime_t *time);
    static void get_diff(cputime_t *before, cputime_t *after, cputime_t *diff);
    static result_t read_stat(cpu_t *cpu);
    static void cpu_copy(cpu_t *des, const cpu_t *src);
    static void cal_cpu_load(cpu_t *before, cpu_t *after, float *total_usage, float *sum_of_core_load);

private:
    static CpuUsage *m_self;        // single instance.
    static int m_cpu_cores;         // number of  cpu core.
    static int m_all_cores_load;    // for 2 cores,max=200%, 4 cores, max=400%;...
    static int m_cpu_usage;         // range:0%~100%
    static cpu_t *m_pre_cpu;
    static cpu_t *m_cur_cpu;
    static uv_mutex_t m_mutex;
    uv_timer_t *m_timer;
};



#endif
