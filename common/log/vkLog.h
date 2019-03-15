#ifndef __VKLOG_HEADER__
#define __VKLOG_HEADER__

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include <uv.h>
#include "../interfaces/vkILogBackend.h"

class Log final
{
public:
    static Log *i(void);
    static inline void add(ILogBackend *backend) { i()->m_backends.push_back(backend); }    
    static void initInst(void);
    static void destroyInst(void);

    void message(ILogBackend::Level level, const char* fmt, ...);
    void text(const char* fmt, ...);

    static const char *colorByLevel(ILogBackend::Level level, bool isColors = true);
    static const char *endl(bool isColors = true);

    static bool colors;

private:
    Log(void);
    static void defaultInit(void);
    ~Log(void);

private:
    static Log *m_self;
    std::vector<ILogBackend *> m_backends;
    uv_mutex_t m_mutex;
};

#define RED_BOLD(x)     "\x1B[1;31m" x "\x1B[0m"
#define RED(x)          "\x1B[0;31m" x "\x1B[0m"
#define GREEN_BOLD(x)   "\x1B[1;32m" x "\x1B[0m"
#define GREEN(x)        "\x1B[0;32m" x "\x1B[0m"
#define YELLOW(x)       "\x1B[0;33m" x "\x1B[0m"
#define YELLOW_BOLD(x)  "\x1B[1;33m" x "\x1B[0m"
#define MAGENTA_BOLD(x) "\x1B[1;35m" x "\x1B[0m"
#define MAGENTA(x)      "\x1B[0;35m" x "\x1B[0m"
#define CYAN_BOLD(x)    "\x1B[1;36m" x "\x1B[0m"
#define CYAN(x)         "\x1B[0;36m" x "\x1B[0m"
#define WHITE_BOLD(x)   "\x1B[1;37m" x "\x1B[0m"
#define WHITE(x)        "\x1B[0;37m" x "\x1B[0m"
#define GRAY(x)         "\x1B[1;30m" x "\x1B[0m"

#endif