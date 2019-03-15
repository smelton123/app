#ifndef __VKSIGNAL_H__
#define __VKSIGNAL_H__

class ISignalListener
{
public:
    virtual ~ISignalListener() = default;

    virtual void onSignal(int signum) = 0;
};

#endif