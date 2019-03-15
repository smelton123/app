#include <uv.h>
#include <stddef.h>
#include "interfaces/vkISignalListener.h"
#include "vkSignals.h"
#include "vkHandle.h"



static const int signums[Signals::kSignalsCount] = { SIGHUP, SIGINT, SIGTERM };


Signals::Signals(ISignalListener *listener)
    : m_listener(listener)
{
    for (size_t i = 0; i < kSignalsCount; ++i) {
        uv_signal_t *signal = new uv_signal_t;
        signal->data        = this;

        m_signals[i] = signal;

        uv_signal_init(uv_default_loop(), signal);
        uv_signal_start(signal, Signals::onSignal, signums[i]);
    }
}


Signals::~Signals()
{
    for (size_t i = 0; i < kSignalsCount; ++i) {
        Handle::close(m_signals[i]);
    }
}


void Signals::onSignal(uv_signal_t *handle, int signum)
{
    static_cast<Signals *>(handle->data)->m_listener->onSignal(signum);
}