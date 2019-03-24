#ifndef DEF_H_
#define DEF_H_

#ifdef _DEBUG_
#define ERR(fmt, ...) fprintf(stderr, "\033[31m[%-15.15s:%4u] " fmt "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define DBG(fmt, ...) fprintf(stderr, "\033[33m[%-15.15s:%4u] " fmt "\033[0m\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(fmt, ...) fprintf(stderr, "[%-15.15s:%4u] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define PRT(fmt, ...) fprintf(stderror, fmt, ##__VA_ARGS__)
#else
#define ERR(fmt, ...)
#define DBG(fmt, ...)
#define LOG(fmt, ...)
#define PRT(fmt, ...)
#endif

#define TRUE  1 
#define FALSE 0 

typedef enum result_t {
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

int get_cpu_usage(float *total_usage, float *sum_usage);

#endif /* DEF_H_ */
