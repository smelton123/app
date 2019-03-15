#ifndef __FSWATCHER_H__
#define __FSWATCHER_H__
#include "../common/interfaces/vkIFsWatcherListener.h"

class FsWatcher final
{
public:
    FsWatcher(const string &path, IFsWatcherListener *listener);
    ~FsWatcher();

private:
    constexpr static int kDelay = 500;//in ms

    static void onFsEvent(uv_fs_event_t *handle, const char *filename, int events, int status);
    static void onTimer(uv_timer_t *handle);

    void queueUpdate();
    void reload();
    void start();

    IFsWatcherListener *m_listener;
    string m_path;
    uv_fs_event_t *m_fsEvent;
    uv_timer_t *m_timer;
};

#endif