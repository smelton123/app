#include <string.h>
#include <assert.h>
#include "vkBindHost.h"


BindHost::BindHost(const char *host, uint16_t port, int version) :
    m_tls(false),
    m_version(version),
    m_port(port),
    m_host(host)
{
}

BindHost::BindHost(const char *addr) :
    m_tls(false),
    m_version(0),
    m_port(0)
{
    if (!addr || strlen(addr) < 5) {
        return;
    }

    if (addr[0] == '[') {
        parseIPv6(addr);
        return;
    }

    parseIPv4(addr);
}

void BindHost::parseIPv4(const char *addr)
{
    const char *port = strchr(addr, ':');
    if (!port) {
        return;
    }

    m_version = 4;
    const size_t size = port++ - addr + 1;
    char *host = new char[size]();
    memcpy(host, addr, size - 1);

    m_host = host;
    m_port = static_cast<uint16_t>(strtol(port, nullptr, 10));
}


void BindHost::parseIPv6(const char *addr)
{
    const char *end = strchr(addr, ']');
    if (!end) {
        return;
    }

    const char *port = strchr(end, ':');
    if (!port) {
        return;
    }

    m_version = 6;
    const size_t size = end - addr;
    char *host = new char[size]();
    memcpy(host, addr + 1, size - 1);

    m_host = host;
    m_port = static_cast<uint16_t>(strtol(port + 1, nullptr, 10));
}

bool BindHost::parseHost(const char *host)
{
    assert(host != nullptr && strlen(host) >= 2);
    m_version = 0;

    if (host == nullptr || strlen(host) < 2) {
        return false;
    }

    if (host[0] == '[') {
        const char *end = strchr(host, ']');
        if (!end) {
            return false;
        }

        const size_t size = end - host;
        char *buf         = new char[size]();
        memcpy(buf, host + 1, size - 1);

        m_version = 6;
        m_host    = buf;
    }
    else {
        m_version = strchr(host, ':') != nullptr ? 6 : 4;
        m_host    = host;
    }

    return m_version > 0;
}
