#include "vkUser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int64_t nextId = 0;
char User::m_sendBuf[2048] = { 0 };
Storage<User> User::m_storage;

User::User(bool ipv6, uint16_t port) :
    m_ipv6(ipv6),
    //m_nicehash(true),
    //m_ip(),
    //m_routeId(-1),
    m_id(++nextId),
    //m_loginId(0),
    m_recvBufPos(0),
    //m_mapperId(-1),
    //m_state(WaitLoginState),
    //m_tls(nullptr),
    m_localPort(port),
    //m_customDiff(0),
    //m_diff(0),
    m_expire(uv_now(uv_default_loop()) + kLoginTimeout),
    m_rx(0),
    //m_timestamp(currentMSecsSinceEpoch()),
    m_tx(0),
    m_fixedByte(0)
{
    m_key = m_storage.add(this);

    //Uuid::create(m_rpcId, sizeof(m_rpcId));

    m_socket.data = m_storage.ptr(m_key);
    uv_tcp_init(uv_default_loop(), &m_socket);

    m_recvBuf.base = m_buf;
    m_recvBuf.len  = sizeof(m_buf);

    //Counters::connections++;
}

User::~User(void)
{
    m_socket.data = nullptr;
    //Counters::connections--;
}

bool User::accept(uv_stream_t *server)
{
    const int rt = uv_accept(server, reinterpret_cast<uv_stream_t*>(&m_socket));
    if (rt < 0) {
        //LOG_ERR("[miner] accept error: \"%s\"", uv_strerror(rt));
        return false;
    }

    sockaddr_storage addr = { 0 };
    int size = sizeof(addr);

    uv_tcp_getpeername(&m_socket, reinterpret_cast<sockaddr*>(&addr), &size);

    if (m_ipv6) {
        uv_ip6_name(reinterpret_cast<sockaddr_in6*>(&addr), m_ip, 45);
    } else {
        uv_ip4_name(reinterpret_cast<sockaddr_in*>(&addr), m_ip, 16);
    }

    uv_read_start(reinterpret_cast<uv_stream_t*>(&m_socket), User::onAllocBuffer, User::onRead);

    return true;
}

void User::shutdown(bool had_error)
{
    //if (m_state == ClosingState) {
    //    return;
    //}

    //setState(ClosingState);
    uv_read_stop(reinterpret_cast<uv_stream_t*>(&m_socket));

    uv_shutdown(new uv_shutdown_t, reinterpret_cast<uv_stream_t*>(&m_socket), [](uv_shutdown_t* req, int status) {

        if (uv_is_closing(reinterpret_cast<uv_handle_t*>(req->handle)) == 0) {
            uv_close(reinterpret_cast<uv_handle_t*>(req->handle), [](uv_handle_t *handle) {
                User *miner = getUser(handle->data);
                if (!miner) {
                    return;
                }

                //CloseEvent::start(miner);
                m_storage.remove(handle->data);
            });
        }

        delete req;
    });
}


void User::onAllocBuffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    auto user = getUser(handle->data);
    if (!user) {
        return;
    }

    buf->base = &user->m_recvBuf.base[user->m_recvBufPos];
    buf->len  = user->m_recvBuf.len - user->m_recvBufPos;
}


void User::onRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    User *user = getUser(stream->data);
    if (!user) {
        return;
    }

    if (nread < 0 || (size_t) nread > (sizeof(m_buf) - 8 - user->m_recvBufPos)) {
        return user->shutdown(nread != UV_EOF);;
    }

    user->m_rx += nread;
    user->m_recvBufPos += nread;

    user->readTLS(static_cast<int>(nread));
}

void User::readTLS(int nread)
{
    read();
}

void User::read()
{
    char* end;
    char* start = m_recvBuf.base;
    size_t remaining = m_recvBufPos;

    while ((end = static_cast<char*>(memchr(start, '\n', remaining))) != nullptr) {
        end++;
        size_t len = end - start;
        //parse(start, len);

        remaining -= len;
        start = end;
    }

    if (remaining == 0) {
        m_recvBufPos = 0;
        return;
    }

    if (start == m_recvBuf.base) {
        return;
    }

    memcpy(m_recvBuf.base, start, remaining);
    m_recvBufPos = remaining;
}