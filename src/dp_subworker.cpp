#include <pthread.h>
#include "dp_subworker.h"

namespace dbproxy
{

int CDPSubWorker::setListenFd(CNetSocket* apListenFd)
{
    if(apListenFd != NULL) {
        m_pListenFd = apListenFd;
        return 0;
    }
    
    return -1;
}
int CDPSubWorker::setConfig(CDPConfig* apDPConfig)
{
    if(apDPConfig == NULL) {
        return -1;
    }
    
    m_pDPConfig = apDPConfig;
    m_iMaxFd = m_pDPConfig->m_iMaxFileSize - 100;
    //set max open file for sub process
    CCommon::SetOpenFiles(m_pDPConfig->m_iMaxFileSize);
    return 0;
}
int CDPSubWorker::init()
{
    m_objDPNet.SetPack(&m_objNetPack);
    bool bRet = m_objDPNet.Init(m_iMaxFd);
    
    if(!bRet) {
        TRACE(1, "CDPSubWorker::init EPOLL init failed.");
        return -1;
    }
    
    bRet = m_objDPNet.Addfd(m_pListenFd);
    
    if(!bRet) {
        TRACE(1, "CDPSubWorker::init add fd failed. ");
        m_objDPNet.Destroy();
        return -1;
    }
    
    m_objDPNet.mbHasListenFd = true;
    m_objDPNet.RecvFrom.connect(this, &dbproxy::CDPSubWorker::dealNetData);
    m_objDPNet.OnErrorNotice.connect(this, &dbproxy::CDPSubWorker::onDealErrorFd);
    m_objDPNet.OnAcceptNotice.connect(this, &dbproxy::CDPSubWorker::onDealAcceptEvent);
    return 0;
}
int CDPSubWorker::run()
{
    int liParentId = getppid();
    
    if(1 == liParentId) {
        TRACE(1, "CDPSubWorker::Run sub processor failed. ID = " << liParentId);
        exit(0);
    }
    
    int bDnsServerInitRet = init();
    
    if(bDnsServerInitRet != 0) {
        TRACE(1, "CDPSubWorker::Run DBProxy Server init failed.");
        exit(0);
    }
    
    for(;;) {
        int liCurrentParentId = getppid();
        
        if((liParentId != liCurrentParentId) || (1 == liCurrentParentId)) {
            TRACE(1, "CDPSubWorker::Run current parent ID = " << liParentId << " CurrentId = " << liCurrentParentId);
            break;
        }
        
        int liCurrentConnectSize = m_objDPNet.GetConnectedSize();
        
        if(liCurrentConnectSize > m_iMaxFd) {
            if(m_objDPNet.mbHasListenFd) {
                TRACE(1, "CDNSChildWorker::Run ????????????????????????: " << liCurrentConnectSize << " ??????????: " << m_iMaxFd);
                m_objDPNet.Delfd(m_pListenFd);
                m_objDPNet.mbHasListenFd = false;
            }
        }
        
        if(liCurrentConnectSize < m_iMaxFd) {
            if(!m_objDPNet.mbHasListenFd) {
                TRACE(1, "CDNSChildWorker::Run С??????????????????????: " << liCurrentConnectSize << " ??????????: " << m_iMaxFd);
                m_objDPNet.Addfd(m_pListenFd);
                m_objDPNet.mbHasListenFd = true;
            }
        }
        
        int nRet = m_objDPNet.CheckEpollEvent(-1);
        m_objDPNet.ProcessEpollEvent(nRet);
        //TimeOutWork();
        usleep(1000 * 1000);
    }
    
    return 0;
}

int CDPSubWorker::dump()
{
    return 0;
}
int CDPSubWorker::timeoutWorker()
{
    return 0;
}

int  CDPSubWorker::sendHandshakePacket(int fd)
{
    char data[21];
    memset(data, '1', 21);
    int data_len = 21;
    char* buff = new char[1 + SERVER_VERSION_LENGTH + data_len + 64];
    char* end = buff;
    *end++ = PROTOCOL_VERSION;
    char szServerVersion[SERVER_VERSION_LENGTH];
    memset(szServerVersion, 0, SERVER_VERSION_LENGTH);
    std::string strVer = "5.7.15-9-log";
    memcpy(szServerVersion, strVer.c_str(), strVer.length());
    memcpy(end, szServerVersion, strVer.length());
    end += strVer.length();
    end += 1;
    //thread id
    int pid = getpid();
    TRACE(5, "thread id : "<<pid);
    int4store((uint8_t*)end, pid);
    end += 4;
    end = (char*)memcpy(end, data, AUTH_PLUGIN_DATA_PART_1_LENGTH);
    end += AUTH_PLUGIN_DATA_PART_1_LENGTH;
    *end++ = 0;
    //
    unsigned int iCap = CLIENT_BASIC_FLAGS|CLIENT_DEPRECATE_EOF|CLIENT_SESSION_TRACK;
    TRACE(5, "cap : "<<iCap);
    int2store(end, static_cast<uint16_t>(iCap));
    /* write server characteristics: up to 16 bytes allowed */
    end[2] = 8;
    uint16_t iServerStatus = SERVER_STATUS_AUTOCOMMIT;
    int2store(end + 3, iServerStatus);
    int2store(end + 5, static_cast<uint16_t>(iCap >> 16));

    uint16_t a = uint2korr(end+5);
    TRACE(5, "a: "<<a);
    end[7] = data_len;
    memset(end + 8, 0, 10);
    end += 18;
    /* write scramble tail */
    end = (char*) memcpy(end, data + AUTH_PLUGIN_DATA_PART_1_LENGTH,
                         data_len - AUTH_PLUGIN_DATA_PART_1_LENGTH);
    end += data_len - AUTH_PLUGIN_DATA_PART_1_LENGTH;
    std::string strPluginName = "mysql_native_password";
    memcpy(end, strPluginName.c_str(), strPluginName.length());
    end += strPluginName.length();
    end += 1;
    
    m_objNetPack.send_seq = 0;
    m_objDPNet.SendData(fd, buff, end - buff);
    delete [] buff;
    buff = NULL;
    m_iAuth = 0;
    return 0;
}
void CDPSubWorker::onDealErrorFd(int fd)
{
    TRACE(1, " fd error : " << fd);
}
void CDPSubWorker::onDealAcceptEvent(int fd)
{
    TRACE(3, " Accept event : " << fd);
    int ret = 0;
    ret = sendHandshakePacket(fd);
    
    if(ret != 0) {
        TRACE(1, "send handshake packet failed.");
        //net resource clear!!!!!!
    }
}

int CDPSubWorker::sendOKPacket(int fd, int seq)
{
    uint8_t buff[MYSQL_ERRMSG_SIZE + 10];
    uint8_t* pos, *start;
    start = buff;
    buff[0] = 0;
    uint64_t affected_rows = 0;
    pos = net_store_length(buff + 1, affected_rows);
    uint64_t id = 0;
    pos = net_store_length(pos, id);
    /* server status */
    int server_status = SERVER_STATUS_AUTOCOMMIT;
    int2store(pos, server_status);
    pos += 2;
    /* warning count: we can only return up to 65535 warnings in two bytes. */
    int statement_warn_count = 0;
    //uint tmp = min(statement_warn_count, 65535U);
    uint32_t tmp = 0;
    int2store(pos, tmp);
    pos += 2;
    pos += 1;
    m_objNetPack.send_seq = seq;
    m_objDPNet.SendData(fd, (char*)start, pos - start);
    return 0;
}
void CDPSubWorker::dealNetData(int fd, char* buffer, int length)
{
    if(m_iAuth == 0) {
        TRACE(3, "auth packet............. length:"<<length);
        short flag = 0;
        memcpy(&flag, buffer, m_objNetPack.recv_seq+1);
        TRACE(3, "flag : "<<flag);
        sendOKPacket(fd, 2);
        m_iAuth = 1;
        return;
    }
    
    char cmd;
    cmd = buffer[0];
    
    switch(cmd) {
    case COM_SLEEP: {
            TRACE(3, "COM_SLEEP");
            break;
        }
        
    case COM_QUIT: {
            TRACE(3, "COM_QUIT");
            break;
        }
        
    case COM_INIT_DB: {
            TRACE(3, "COM_INIT_DB");
            break;
        }
        
    case COM_QUERY: {
            TRACE(3, "COM_QUERY");
            std::string query = buffer+1;
            TRACE(3, "query: "<<query.c_str());
            sendOKPacket(fd, m_objNetPack.recv_seq+1);
            break;
        }
        
    case COM_FIELD_LIST: {
            TRACE(3, "COM_FIELD_LIST");
            break;
        }
        
    case COM_CREATE_DB: {
            TRACE(3, "COM_CREATE_DB");
            break;
        }
        
    case COM_DROP_DB: {
            TRACE(3, "COM_DROP_DB");
            break;
        }
        
    case COM_REFRESH: {
            TRACE(3, "COM_REFRESH");
            break;
        }
        
    case COM_SHUTDOWN: {
            TRACE(3, "COM_SHUTDOWN");
            break;
        }
        
    case COM_STATISTICS: {
            TRACE(3, "COM_STATISTICS");
            break;
        }
        
    case COM_PROCESS_INFO: {
            TRACE(3, "COM_PROCESS_INFO");
            break;
        }
        
    case COM_CONNECT: {
            TRACE(3, "COM_CONNECT");
            break;
        }
        
    case COM_PROCESS_KILL: {
            TRACE(3, "COM_PROCESS_KILL");
            break;
        }
        
    case COM_DEBUG: {
            TRACE(3, "COM_DEBUG");
            break;
        }
        
    case COM_PING: {
            TRACE(3, "COM_PING");
            break;
        }
        
    case COM_TIME: {
            TRACE(3, "COM_TIME");
            break;
        }
        
    case COM_DELAYED_INSERT: {
            TRACE(3, "COM_DELAYED_INSERT");
            break;
        }
        
    case COM_CHANGE_USER: {
            TRACE(3, "COM_CHANGE_USER");
            break;
        }
        
    case COM_BINLOG_DUMP: {
            TRACE(3, "COM_BINLOG_DUMP");
            break;
        }
        
    case COM_TABLE_DUMP: {
            TRACE(3, "COM_TABLE_DUMP");
            break;
        }
        
    case COM_CONNECT_OUT: {
            TRACE(3, "COM_CONNECT_OUT");
            break;
        }
        
    case COM_REGISTER_SLAVE: {
            TRACE(3, "COM_REGISTER_SLAVE");
            break;
        }
        
    case COM_STMT_PREPARE: {
            TRACE(3, "COM_STMT_PREPARE");
            break;
        }
        
    case COM_STMT_EXECUTE: {
            TRACE(3, "COM_STMT_EXECUTE");
            break;
        }
        
    case COM_STMT_SEND_LONG_DATA: {
            TRACE(3, "COM_STMT_SEND_LONG_DATA");
            break;
        }
        
    case COM_STMT_CLOSE: {
            TRACE(3, "COM_STMT_CLOSE");
            break;
        }
        
    case COM_STMT_RESET: {
            TRACE(3, "COM_STMT_RESET");
            break;
        }
        
    case COM_SET_OPTION: {
            TRACE(3, "COM_SET_OPTION");
            break;
        }
        
    case COM_STMT_FETCH: {
            TRACE(3, "COM_STMT_FETCH");
            break;
        }
        
    case COM_DAEMON: {
            TRACE(3, "COM_DAEMON");
            break;
        }
        
    case COM_BINLOG_DUMP_GTID: {
            TRACE(3, "COM_BINLOG_DUMP_GTID");
            break;
        }
        
    case COM_END: {
            TRACE(3, "COM_END");
            break;
        }
        
    default: {
            TRACE(2, "unknown cmd: " << cmd);
            break;
        }
    }
}
int CDPSubWorker::keeplive()
{
    return 0;
}

} //end of namespace dbproxy
