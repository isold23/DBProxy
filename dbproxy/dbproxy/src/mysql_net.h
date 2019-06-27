#ifndef _MYSQL_NET_H_
#define _MYSQL_NET_H_

#include "mysql.h"
#include "mysql_net_packet.h"
#include "dp_config.h"

namespace dbproxy
{



class CClientInfo {
public:
    CClientInfo() {}
    CClientInfo(const CClientInfo& info) {
        m_iAuth = info.m_iAuth;
    }
    ~CClientInfo()  {}

    CClientInfo& operator=(const CClientInfo& info) {
        if(&info == this)
            return *this;
        m_iAuth = info.m_iAuth;
        return *this;
    }
public:
    int m_iAuth;
};

class CClientList {
public:
    CClientList() {}
    ~CClientList() {}

    CClientInfo get(int fd) {
        return cl[fd];
    }

    int set(int fd, CClientInfo& info) {
        cl[fd] = info;
        return 0;
    }

public:
    std::map<int, CClientInfo> cl;
};

class CMySQLNet {
public:
    CMySQLNet():m_ptrMySQL(NULL) {}
    ~CMySQLNet() {}

    int init(std::string& h, uint16_t p, std::string& u, std::string& psw);
    int destroy();
    int connect();
    int disconnect();
    int sendPacket(char* buffer, int len);
    int recvPacket(char* buffer, int& len);

    int print(){
        TRACE(5, "net host: "<<host.c_str()<<" port: "<<port);
        return 0;
    }

public:
    int seq;

private:
    int fd;
    MYSQL* m_ptrMySQL;
    std::string host;
    uint16_t port;
    std::string user;
    std::string password;

};

/*


 */
class CMySQLNetList {
public:
    CMySQLNetList() {}
    ~CMySQLNetList() {}
    int init(CDPConfig* apDPConfig);
    CMySQLNet* operator[](int key) {
        return nets[key];
    }

public:
    CDPConfig* m_pDPConfig;
    std::map<int, CMySQLNet*> nets;
};

} //end of namespace dbproxy

#endif //_MYSQL_NET_H_
