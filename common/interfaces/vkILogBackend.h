#ifndef __CILOGBACKEND_HEADER__
#define __CILOGBACKEND_HEADER__

#include <stdarg.h>
#include <stddef.h>

class ILogBackend
{
public:
    enum Level
    {
        ERR,
        WARNING,
        NOTICES,
        INFO,
        DEBUG
    };
    constexpr static const size_t kBufferSize = 1024;
    virtual ~ILogBackend() {}
    virtual void message(Level level, const char* fmt, va_list args) = 0;
    virtual void text(const char* fmt, va_list args)                 = 0;
};

#endif