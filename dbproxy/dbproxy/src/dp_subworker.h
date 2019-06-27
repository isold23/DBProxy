#ifndef _DBPROXY_SUBWORKER_H_
#define _DBPROXY_SUBWORKER_H_

#include "include.h"
#include "net_epoll.h"
#include "dp_config.h"
#include "mysql_command.h"
#include "mysql_net_packet.h"
#include "mysql_net.h"

#define KEEPLIVE_TIME_OUT      120

namespace dbproxy
{

class CDPSubWorker : public sigslot::has_slots<>
{

public:
    CDPSubWorker() {}
    ~CDPSubWorker() {}
    
    int setListenFd(CNetSocket* apListenFd);
    int setConfig(CDPConfig* apDPConfig);
    int run();
    
private:
    int init();
    int dump();
    int timeoutWorker();
    void onDealErrorFd(int fd);
    void onDealAcceptEvent(int fd);
    void dealNetData(int fd, char* buffer, int length);
    //int dealDataComplete(int );
    int keeplive();
    int sendHandshakePacket(int fd);
    int sendOKPacket(int fd, int seq);
    int sendQueryPacket(std::string& sql);
    
public:
    CDPConfig* m_pDPConfig;
    CNetEpoll m_objDPNet;
    CNetSocket* m_pListenFd;
    CMySQLNetPack m_objNetPack;
    CMySQLNetList m_objMySQLNet;
    int m_iMaxFd;
    int m_iMaxFileSize;
    uint64_t m_iLastDumpTime;
    uint64_t m_iLastKeeplive;
    int m_iAuth;
};

} //endof namespace dbproxy
#endif //_DBPROXY_SUBWORKER_H_
