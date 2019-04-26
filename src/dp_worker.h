#ifndef _DBPROXY_WORKER_H_
#define _DBPROXY_WORKER_H_

#include "tcp_socket.h"
#include "dp_config.h"
#include "dp_subworker.h"

namespace dbproxy
{

class CDPWorker
{
public:
    CDPWorker() {}
    ~CDPWorker() {}
    int setConfig(CDPConfig* apDPConfig);
    int run();
    
    static void subProcess(void* parameter);
    
private:
    int dump();
    int timeoutProcess();
public:
    CDPConfig* m_pDPConfig;
    CNetSocket m_objListenFd;
    
};

} //end of namespace dbproxy

#endif //_DBPROXY_WORKER_H_
