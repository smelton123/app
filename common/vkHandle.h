#ifndef __VKHANDLE_H__
#define __VKHANDLE_H__
#include <uv.h>

typedef struct uv_fs_event_s uv_fs_event_t;
typedef struct uv_getaddrinfo_s uv_getaddrinfo_t;
typedef struct uv_handle_s uv_handle_t;
typedef struct uv_signal_s uv_signal_t;
typedef struct uv_tcp_s uv_tcp_t;
typedef struct uv_timer_s uv_timer_t;

class Handle final
{
public:
    static void close(uv_fs_event_t *handle);
    static void close(uv_getaddrinfo_t *handle);
    static void close(uv_handle_t *handle);
    static void close(uv_signal_t *handle);
    static void close(uv_tcp_t *handle);
    static void close(uv_timer_t *handle);
};

#endif