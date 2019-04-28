#ifndef __VKSIGNALS_H__
#define __VKSIGNALS_H__
#include <uv.h>
#include "interfaces/vkISignalListener.h"


class Signals final
{
public:
    constexpr static const size_t kSignalsCount = 4;

    Signals(ISignalListener *listener);
    ~Signals();

private:
    void close(int signum);

    static void onSignal(uv_signal_t *handle, int signum);

    ISignalListener *m_listener;
    uv_signal_t *m_signals[kSignalsCount];
};


#endif

