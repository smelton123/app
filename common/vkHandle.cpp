#include <uv.h>


#include "vkHandle.h"


void Handle::close(uv_fs_event_t *handle)
{
    if (handle) {
        uv_fs_event_stop(handle);
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}


void Handle::close(uv_getaddrinfo_t *handle)
{
    if (handle) {
        uv_cancel(reinterpret_cast<uv_req_t *>(handle));
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}


void Handle::close(uv_handle_t *handle)
{
    uv_close(handle, [](uv_handle_t *handle) { delete handle; });
}


void Handle::close(uv_signal_t *handle)
{
    if (handle) {
        uv_signal_stop(handle);
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}


void Handle::close(uv_tcp_t *handle)
{
    if (handle) {
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}


void Handle::close(uv_timer_s *handle)
{
    if (handle) {
        uv_timer_stop(handle);
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}

void Handle::close(uv_process_s *handle)
{
    if (handle) {
        close(reinterpret_cast<uv_handle_t *>(handle));
    }
}