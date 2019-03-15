#ifndef  __VKCONSOLE_H__
#define  __VKCONSOLE_H__

#include <uv.h>
#include "interfaces/vkIConsoleListener.h"
class Console final
{
public:
    Console(IConsoleListener *listener);

private:
    static void onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf);
    static void onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);

    char m_buf[1];
    IConsoleListener *m_listener;
    uv_tty_t m_tty;
};

#endif