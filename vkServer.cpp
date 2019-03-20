#include "uv.h"
#include "vkServer.h"

Server::Server(const BindHost &host) :
    m_version(0),
    m_port(host.port()),
    m_host(host.host())
{
    uv_tcp_init(uv_default_loop(), &m_server);
    m_server.data = this;

    uv_tcp_nodelay(&m_server, 1);

    if (host.isIPv6() 
        && uv_ip6_addr(m_host.data(), m_port, &m_addr6) == 0)
    {
        m_version = 6;
        return;
    }

    if (uv_ip4_addr(m_host.data(), m_port, &m_addr) == 0)
    {
        m_version = 4;
    }
}

bool Server::bind(void)
{
    if (!m_version) {
        return false;
    }

    const sockaddr *addr = (m_version == 6) ? (reinterpret_cast<const sockaddr*>(&m_addr6)) : (reinterpret_cast<const sockaddr*>(&m_addr));
    uv_tcp_bind(&m_server, addr, m_version == 6 ? UV_TCP_IPV6ONLY : 0);

    const int r = uv_listen(reinterpret_cast<uv_stream_t*>(&m_server), 511, Server::onConnection);
    if (r) {
        //LOG_ERR("[%s:%u] listen error: \"%s\"", m_host.data(), m_port, uv_strerror(r));
        return false;
    }

    return true;
}


void Server::create(uv_stream_t *server, int status)
{
    if (status < 0) {
        //LOG_ERR("[%s:%u] new connection error: \"%s\"", m_host.data(), m_port, uv_strerror(status));
        return;
    }

    User *user = new User(m_version == 6, m_port);
    if (!user) {
        return;
    }

    if (!user->accept(server)) {
        delete user;
        return;
    }

    //ConnectionEvent::start(miner, m_port);
}


void Server::onConnection(uv_stream_t *server, int status)
{
    static_cast<Server*>(server->data)->create(server, status);
}