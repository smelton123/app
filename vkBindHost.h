#ifndef _VKBINDHOST_H__
#define _VKBINDHOST_H__

#include <stdint.h>
#include <string>

using namespace std;

class BindHost final
{
public:
    constexpr static uint16_t kDefaultPort = 3333;
    inline BindHost() :
        m_tls(false),
        m_version(0),
        m_port(0)
    {}


    BindHost(const char *addr);
    BindHost(const char *host, uint16_t port, int version);
    //BindHost(const rapidjson::Value &object);

    //rapidjson::Value toJSON(rapidjson::Document &doc) const;

    inline bool isIPv6() const      { return m_version == 6; }
    inline bool isTLS() const       { return m_tls; }
    inline bool isValid() const     { return m_version && !m_host.empty() && m_port > 0; }
    inline const char *host() const { return m_host.data(); }
    inline uint16_t port() const    { return m_port; }
    inline void setTLS(bool enable) { m_tls = enable; }

private:
    bool parseHost(const char *host);
    void parseIPv4(const char *addr);
    void parseIPv6(const char *addr);

private:
    bool m_tls;
    int m_version;
    uint16_t m_port;
    string m_host;

};



#endif

