#ifndef __VK_PROCESS_HEADER__
#define __VK_PROCESS_HEADER__
#include <uv.h>
#include <time.h>
#include <string>
#include <cstring>
#include "vkProcess.h"

using namespace std;

class Process final
{
public:
    enum Location {
        ExeLocation,
        CwdLocation
    };

#   ifdef WIN32
    constexpr const static char kDirSeparator = '\\';
#   else
    constexpr const static char kDirSeparator = '/';
#   endif

    Process(int argc, char **argv);
    ~Process();

    static string location(Location location, const char *fileName = nullptr);

    //inline const Arguments &arguments() const { return m_arguments; }

private:
    //Arguments m_arguments;
};

#endif