#include <uv.h>
#include "vkFsWatcher.h"
#include "../common/vkHandle.h"


FsWatcher::FsWatcher(const string &path, IFsWatcherListener *listener) :
    m_listener(listener),
    m_path(path)
{
    m_fsEvent = new uv_fs_event_t;
    uv_fs_event_init(uv_default_loop(), m_fsEvent);

    m_timer = new uv_timer_t;
    uv_timer_init(uv_default_loop(), m_timer);

    m_fsEvent->data = m_timer->data = this;

    start();
}


FsWatcher::~FsWatcher()
{
    Handle::close(m_timer);
    Handle::close(m_fsEvent);
    delete m_fsEvent;
    delete m_timer;
}


void FsWatcher::onTimer(uv_timer_t *handle)
{
    static_cast<FsWatcher *>(handle->data)->reload();
}


void FsWatcher::onFsEvent(uv_fs_event_t *handle, const char *filename, int, int)
{
    if (!filename) {
        return;
    }

    static_cast<FsWatcher *>(handle->data)->queueUpdate();
}


void FsWatcher::queueUpdate()
{
    uv_timer_stop(m_timer);
    uv_timer_start(m_timer, FsWatcher::onTimer, kDelay, 0);
}


void FsWatcher::reload()
{
    m_listener->onFileChanged(m_path);

#   ifndef _WIN32
    uv_fs_event_stop(m_fsEvent);
    start();
#   endif
}


void FsWatcher::start()
{
    uv_fs_event_start(m_fsEvent, FsWatcher::onFsEvent, m_path.c_str(), 0);
}