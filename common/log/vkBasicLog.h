#ifndef __BASICLOG_H__
#define __BASICLOG_H__


#include <uv.h>


#include "../interfaces/vkILogBackend.h"


//namespace xmrig {
//    class Controller;
//}


class BasicLog final: public ILogBackend
{
public:
    BasicLog();

    void message(Level level, const char *fmt, va_list args) override;
    void text(const char *fmt, va_list args) override;

private:
    //bool isWritable() const;
    void print(va_list args);

    char m_buf[kBufferSize];
    char m_fmt[256];
};

#endif /* __BASICLOG_H__ */
