#ifndef __VKICONSOLELISTENER_H__
#define __VKICONSOLELISTENER_H__

class IConsoleListener
{
public:
    virtual ~IConsoleListener() {}

    virtual void onConsoleCommand(char command) = 0;
};


#endif