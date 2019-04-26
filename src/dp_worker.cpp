#include "dp_worker.h"

namespace dbproxy
{

int CDPWorker::setConfig(CDPConfig* apDPConfig)
{
    if(apDPConfig != NULL) {
        m_pDPConfig = apDPConfig;
        return 0;
    }
    
    TRACE(1, "DBProxy config is null.");
    return -1;
}

int CDPWorker::run()
{
    try {
        bool lbBind = false;
        unsigned short lusBindPort = m_pDPConfig->m_iPort;
        struct sockaddr_in addr;
        addr.sin_addr.s_addr = htonl(m_pDPConfig->m_iHost);
        
        for(int i = 0; i < m_pDPConfig->m_iMaxBindTimes; ++i) {
            bool bRet = m_objListenFd.CreateSocket(inet_ntoa(addr.sin_addr), lusBindPort);
            
            if(bRet) {
                lbBind = true;
                break;
            }
        }
        
        if(!lbBind) {
            TRACE(1, "CDPWorker::Run bind failed. ");
            return -1;
        }
        
        bool bRet = m_objListenFd.SetNoBlock();
        
        if(!bRet) {
            m_objListenFd.Close();
            TRACE(1, "CDPWorker::Run set noblock failed.");
            return -1;
        }
        
        bRet = m_objListenFd.Listen();
        
        if(!bRet) {
            m_objListenFd.Close();
            TRACE(1, "CDPWorker::Run listen failed.");
            return -1;
        }
        
        m_objListenFd.moNetStat = COMMON_TCP_LISTEN;
        m_objListenFd.mbListenSocket = true;
        
        for(int i = 0; i < m_pDPConfig->m_iMaxSubWorker; ++i) {
            pid_t nPid;
            
            if((nPid = fork()) < 0) {
                TRACE(1, "CDPWorker::Run fork failed. ");
                kill(0, SIGKILL);
                return -1;
            } else if(nPid == 0) {
                subProcess(this);
            } else {
                TRACE(3, "CDPWorker::Run Child PID: " << nPid << " PID: " << getpid());
            }
        }
        
        while(true) {
            //TimeOutWork();
            pid_t pt;
            int status = 0;
            pt = waitpid(-1, &status, WNOHANG);
            
            if(-1 == pt) {
                if(errno != ECHILD) {
                    TRACE(1, "CDPWorker::Run wait_pid failed. errno = " << errno);
                }
            } else if(pt > 0) {
                pid_t nPid;
                
                if((nPid = fork()) < 0) {
                    TRACE(1, "CDPWorker::Run fork failed.");
                    kill(0, SIGKILL);
                } else if(nPid == 0) {
                    subProcess(this);
                } else {
                    TRACE(1, "CDPWorker::Run Child PID: " << nPid << " PID: " << getpid());
                }
            }
            
            usleep(1000 * 100);
        }
    } catch(...) {
        TRACE(1, "CDPWorker::Run exception.");
        return -1;
    }
    
    return -1;
}

void CDPWorker::subProcess(void* parameter)
{
    if(parameter == NULL)
        return;
        
    CDPWorker* p = (CDPWorker*)parameter;
    goDebugTrace = new CDebugTrace;
    char lszLogFileName[255];
    memset(lszLogFileName, 0, 255);
    CCommon::GetAppPath(lszLogFileName, 255);
    unsigned int process_id = CCommon::GetProcessId();
    SET_TRACE_LEVEL(5);
    unsigned liOptions = (CDebugTrace::Timestamp | CDebugTrace::LogLevel & ~CDebugTrace::FileAndLine | CDebugTrace::AppendToFile);
    SET_TRACE_OPTIONS(GET_TRACE_OPTIONS() | liOptions);
    strcpy(strrchr(lszLogFileName, '/'), "//dbproxy_log");
    CCommon::CreatePath(lszLogFileName);
    char lszFileDate[50];
    memset(lszFileDate, 0, 50);
    sprintf(lszFileDate, "//dbproxy-%u", process_id);
    strcat(lszLogFileName, lszFileDate);
    SET_LOG_FILENAME(lszLogFileName);
    TRACE(3, "\n\n*******************DBPROXY VERSION:" << g_strServerVersion.c_str() << "*******************");
    CDPConfig loDNSConfig;
    loDNSConfig.set_conf_file_name(p->m_pDPConfig->get_conf_file_name().c_str());
    TRACE(3, "Config file name: " << loDNSConfig.get_conf_file_name().c_str());
    bool bRet = loDNSConfig.load();
    
    if(!bRet) {
        TRACE(1, "ERROR: load config failed.");
        exit(0);
    }
    
    CDPSubWorker dns_child_worker;
    dns_child_worker.setConfig(&loDNSConfig);
    int ret = dns_child_worker.setListenFd(&p->m_objListenFd);
    
    if(ret != 0) {
        TRACE(1, "set listen fd failed.");
        exit(0);
    }
    
    dns_child_worker.run();
    exit(0);
}
} //end of namespace dbproxy
