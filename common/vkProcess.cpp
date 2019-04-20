#include <uv.h>
#include "vkProcess.h"

static size_t location(Process::Location location, char *buf, size_t max)
{
    size_t size = max;
    if (location == Process::ExeLocation) {
        return uv_exepath(buf, &size) < 0 ? 0 : size;
    }

    if (location == Process::CwdLocation) {
        return uv_cwd(buf, &size) < 0 ? 0 : size;
    }

    return 0;
}


Process::Process(int argc, char **argv) //:
    //m_arguments(argc, argv)
{
    //srand(static_cast<unsigned int>(static_cast<uintptr_t>(time(nullptr)) ^ reinterpret_cast<uintptr_t>(this)));
}


Process::~Process()
{
}


string Process::location(Location location, const char *fileName)
{
    constexpr const size_t max = 520;

    char *buf   = new char[max]();
    size_t size = ::location(location, buf, max);

    if (size == 0) {
        delete [] buf;

        return string();
    }

    if (fileName == nullptr) {
        return buf;
    }

    if (location == ExeLocation) {
        char *p = strrchr(buf, kDirSeparator);

        if (p == nullptr) {
            delete [] buf;

            return string();
        }

        size = static_cast<size_t>(p - buf);
    }

    if ((size + strlen(fileName) + 2) >= max) {
        delete [] buf;

        return string();
    }

    buf[size] = kDirSeparator;
    strcpy(buf + size + 1, fileName);

    return buf;
}
