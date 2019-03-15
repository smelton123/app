#include "vkLog.h"

Log *Log::m_self = nullptr;
bool Log::colors = true;


static const char *color[5] = 
{
    "\x1B[0;31m",  /* ERR     */
    "\x1B[0;33m",  /* WARNING */
    "\x1B[1;37m",  /* NOTICE  */
    "",            /* INFO    */
#   ifdef WIN32
    "\x1B[1;30m"   /* DEBUG   */
#   else
    "\x1B[90m"     /* DEBUG   */
#   endif
};


void Log::message(ILogBackend::Level level, const char* fmt, ...)
{
    uv_mutex_lock(&m_mutex);

    va_list args;
    va_list copy;
    va_start(args, fmt);

    for (ILogBackend *backend : m_backends) {
        va_copy(copy, args);
        backend->message(level, fmt, copy);
        va_end(copy);
    }

    va_end(args);

    uv_mutex_unlock(&m_mutex);
}


void Log::text(const char* fmt, ...)
{
    uv_mutex_lock(&m_mutex);

    va_list args;
    va_list copy;
    va_start(args, fmt);

    for (ILogBackend *backend : m_backends) {
        va_copy(copy, args);
        backend->text(fmt, copy);
        va_end(copy);
    }

    va_end(args);

    uv_mutex_unlock(&m_mutex);
}


const char *Log::colorByLevel(ILogBackend::Level level, bool isColors)
{
    if (!isColors) {
        return "";
    }

    return color[level];
}


const char *Log::endl(bool isColors)
{
#   ifdef _WIN32
    return isColors ? "\x1B[0m\r\n" : "\r\n";
#   else
    return isColors ? "\x1B[0m\n" : "\n";
#   endif
}

Log::~Log()
{
    for (auto backend : m_backends) {
        delete backend;
    }
}

Log::Log(void)
{
    assert(m_self==nullptr);
    uv_mutex_init(&m_mutex);
    m_self = this;
}

void Log::destroyInst(void)
{
    if (m_self)
        delete m_self;
    m_self = nullptr;
}

void Log::initInst(void)
{
    if (!m_self)
        defaultInit();
}

void Log::defaultInit(void)
{
    m_self = new Log;
}

Log* Log::i(void)
{
    if (!m_self)
        defaultInit();
    return m_self;
}
