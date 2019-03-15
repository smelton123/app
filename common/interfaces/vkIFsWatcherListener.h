#ifndef  __VKFSWATCHERLISTENER_H__
#define  __VKFSWATCHERLISTENER_H__

#include <string>
using namespace std;

class IFsWatcherListener
{
public:
    virtual ~IFsWatcherListener() = default;

    virtual void onFileChanged(const string &fileName) = 0;
};

#endif