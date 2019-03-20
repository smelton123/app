#ifndef __SERVER_H__
#include <string>
#include <uv.h>
#include "vkBindHost.h"
#include "common/vkUser.h"

using namespace std;

class Server final
{
public:
    Server(const BindHost &host);
    bool bind(void);

private:
    static void onConnection(uv_stream_t *server, int status);
    void create(uv_stream_t *server, int status);
    int m_version;
    sockaddr_in m_addr;
    sockaddr_in6 m_addr6;
    uint16_t m_port;
    uv_tcp_t m_server;
    string m_host;
};

#endif