/********************************************************************
author: isold.wang@gmail.com
purpose: tcp socket
*********************************************************************/
#include "tcp_socket.h"

CNetSocket::CNetSocket()
{
    miSocket = -1;
    memset(mszResendBuffer, 0, SEND_CATCH_LEN);
    miResendLength = 0;
    memset(mszRecvCache, 0, RECV_CATCH_LEN);
    miRecvCacheLength = 0;
    mbCanSend = true;
    moNetStat = COMMON_TCP_CLOSED;
    mbListenSocket = false;
    mbClientSocket = false;
    m_pNetPack = NULL;
    mp_net_io = NULL;
}
CNetSocket::~CNetSocket()
{
    Close();
}
bool CNetSocket::CreateSocket(void)
{
    miSocket = socket(AF_INET, SOCK_STREAM, 0);
    int err = errno;
    
    if(miSocket < 0) {
        TRACE(1, "create socket failed. errno = " << err);
        return false;
    }
    
    return true;
}
bool CNetSocket::CreateSocket(const char* ip, const short port)
{
    miSocket = socket(AF_INET, SOCK_STREAM, 0);
    int err = errno;
    
    if(miSocket < 0) {
        TRACE(1, "CNetSocket::CreateSocket socket() ʧ�ܡ�errno = " << err);
        return false;
    }
    
    //���õ�ַ����
    int iReuseAddr = 1;
    setsockopt(miSocket, SOL_SOCKET, SO_REUSEADDR, (void*)(& (iReuseAddr))
               , sizeof(iReuseAddr));
    struct sockaddr_in stAddr;
    stAddr.sin_family = AF_INET;
    
    if(ip) {
        stAddr.sin_addr.s_addr = inet_addr(ip);
    } else {
        stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    stAddr.sin_port = htons(port);
    socklen_t loinlen = sizeof(sockaddr_in);
    int ret = 0;
    ret = bind(miSocket, (struct sockaddr*)&stAddr, loinlen);
    
    if(ret < 0) {
        err = errno;
        TRACE(1, "CNetSocket::CreateSocket bind() ʧ�ܡ�port = " << port <<
              " ip = " << ip << " errno = " << err);
        close(miSocket);
        return false;
    }
    
    return true;
}
bool CNetSocket::Close(int flag /* = 2 */)
{
    //������ͻ�����
    memset(mszResendBuffer, 0, SEND_CATCH_LEN);
    miResendLength = 0;
    //������Ͷ���
    CAutoLock send_lock(moSendSection);
    STRU_NET_DATA_INFO loNetDataInfo;
    
    while(moSendList.PopFront(loNetDataInfo)) {
        delete loNetDataInfo.buffer;
        loNetDataInfo.buffer = NULL;
        loNetDataInfo.length = 0;
    }
    
    //������ջ�����
    memset(mszRecvCache, 0,  RECV_CATCH_LEN);
    miRecvCacheLength = 0;
    //������ն���
    CAutoLock recv_lock(moRecvSection);
    
    while(moRecvList.PopFront(loNetDataInfo)) {
        delete loNetDataInfo.buffer;
        loNetDataInfo.buffer = NULL;
        loNetDataInfo.length = 0;
    }
    
    if(miSocket != -1) {
        shutdown(miSocket, flag);
        close(miSocket);
        TRACE(1, "CNetSocket::Close �ر�SOCKET: " << miSocket);
        moNetStat = COMMON_TCP_CLOSED;
        miSocket = -1;
    }
    
    return true;
}

void CNetSocket::CloseWR(int flag /* = 2 */)
{
    shutdown(miSocket, flag);
    close(miSocket);
    
    if(mp_net_io) {
        mp_net_io->release();
        mp_net_io = NULL;
    }
}

bool CNetSocket::Listen()
{
    if(listen(miSocket, 1024) < 0) {
        int err = errno;
        TRACE(1, "CNetSocket::Listen ʧ�ܡ�errno = " << err);
        return false;
    }
    
    moNetStat = COMMON_TCP_LISTEN;
    return true;
}
bool CNetSocket::ConnectServer(const char* ip, const short port)
{
    struct sockaddr_in stAddr;
    stAddr.sin_family = AF_INET;
    
    if(ip) {
        stAddr.sin_addr.s_addr = inet_addr(ip);
    } else {
        stAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    
    stAddr.sin_port = htons(port);
    socklen_t loinlen = sizeof(sockaddr_in);
    
    if(connect(miSocket, (const struct sockaddr*)&stAddr, loinlen) < 0) {
        int err = errno;
        
        if(errno != EINPROGRESS) {
            TRACE(1, "CNetSocket::ConnectServer ʧ�ܡ� errno = " << err <<
                  " ip = " << ip << " port = " << port);
            // �����������Ӵ�����ر�����
            return false;
        } else {
            // ������ģʽ����δ���
            TRACE(1, "CNetSocket::ConnectServer ������ģʽ����δ��ɡ�");
            moNetStat = COMMON_TCP_CONNECTING;
            return true;
        }
    }
    
    moNetStat = COMMON_TCP_CONNECTED;
    mp_net_io = m_pNetPack->create_clt_io(miSocket);
    mp_net_io->open();
    return true;
}
int CNetSocket::Accept(uint32_t& ip, uint16_t& port)
{
    int err = 0;
    sockaddr_in loSockAddr;
    int nRet = 0;
    socklen_t loinlen = sizeof(sockaddr_in);
    nRet = accept(miSocket, (struct sockaddr*)&loSockAddr, &loinlen);
    
    if(-1 == nRet) {
        err = errno;
        
        if(err == EAGAIN || err == EWOULDBLOCK) {
            TRACE(3, "CNetSocket::Accept accept not ready.");
        } else {
            TRACE(1, "CNetSocket::Accept ����errno = " << err);
        }
        
        return 0;
    }
    
    ip = ntohl(loSockAddr.sin_addr.s_addr);
    port = ntohs(loSockAddr.sin_port);
    TRACE(3, "CNetSocket::Accept IP = " << inet_ntoa(loSockAddr.sin_addr) << " PORT = " << port << " fd = " << nRet);
    return nRet;
}
bool CNetSocket::SetNoBlock()
{
    int iFlags = 1; // nonblock reusaddr
    ioctl(miSocket, FIONBIO, &iFlags);
    iFlags = fcntl(miSocket, F_GETFL, 0);
    int nRet = fcntl(miSocket, F_SETFL, iFlags | O_NONBLOCK | O_NDELAY);
    
    if(nRet == -1) {
        TRACE(1, "CNetSocket::SetNoBlock ���÷���������ʧ�ܡ�socket : " << miSocket);
        return false;
    }
    
    int iBufLen = 2 * DEF_SOCKET_CATCH_LEN;
    
    //����TCP�Ľ��ջ���
    if(setsockopt(miSocket, SOL_SOCKET, SO_RCVBUF, (char*)&iBufLen, sizeof(iBufLen)) == -1) {
        TRACE(1, "CNetSocket::SetNoBlock ���ý��ջ�����ʧ�ܡ�socket : " << miSocket);
        return false;
    }
    
    //����TCP�ķ��ͻ���
    if(setsockopt(miSocket, SOL_SOCKET, SO_SNDBUF, (char*)&iBufLen, sizeof(iBufLen)) == -1) {
        TRACE(1, "CNetSocket::SetNoBlock ���÷��ͻ�����ʧ�ܡ�socket : " << miSocket);
        return false;
    }
    
    //���ô��
    SetTcpSockKeepAlive(1, 120, 30, 3);
    return true;
}
int CNetSocket::SetTcpSockKeepAlive(int nKeepAlive, int nKeepIdle, int nKeepInterval, int nKeepCnt)
{
    if(setsockopt(miSocket, SOL_SOCKET, SO_KEEPALIVE, (void*)&nKeepAlive, sizeof(nKeepAlive))) {
        TRACE(1, "CNetSocket::SetTcpSockKeepAlive SO_KEEPALIVE ʧ�ܡ�");
        return -1;
    }
    
    if(nKeepAlive) {
        if(nKeepIdle > 0) {
            if(setsockopt(miSocket, SOL_TCP, TCP_KEEPIDLE, (void*)&nKeepIdle, sizeof(nKeepIdle))) {
                TRACE(1, "CNetSocket::SetTcpSockKeepAlive TCP_KEEPIDLE ʧ�ܡ�");
                return -2;
            }
        }
        
        if(nKeepInterval > 0) {
            if(setsockopt(miSocket, SOL_TCP, TCP_KEEPINTVL, (void*)&nKeepInterval, sizeof(nKeepInterval))) {
                TRACE(1, "CNetSocket::SetTcpSockKeepAlive TCP_KEEPINTVL ʧ�ܡ�");
                return -3;
            }
        }
        
        if(nKeepCnt > 0) {
            if(setsockopt(miSocket, SOL_TCP, TCP_KEEPCNT, (void*)&nKeepCnt, sizeof(nKeepCnt))) {
                TRACE(1, "CNetSocket::SetTcpSockKeepAlive TCP_KEEPCNT ʧ�ܡ�");
                return -4;
            }
        }
    }
    
    return 0;
}
int CNetSocket::SendData(const char* buffer, const int length)
{
    ASSERT(NULL != m_pNetPack);
    char lszSendBuffer[m_pNetPack->_max_pack_size];
    memset(lszSendBuffer, 0, m_pNetPack->_max_pack_size);
    uint32_t liSendLen = m_pNetPack->_max_pack_size;
    int nRet = m_pNetPack->Pack(buffer, length, lszSendBuffer, liSendLen);
    
    if(nRet < 0) {
        TRACE(1, "CNetSocket::SendData pack error. fd = " << miSocket << " ret = " << nRet);
        return -1;
    }
    
    int ssize = 0;
    memcpy(&ssize, lszSendBuffer, 4);
    TRACE(5, "send size : " << ssize);
    CAutoLock lock(moSendSection);
   // 
    if(((liSendLen + miResendLength) <= DEF_SOCKET_CATCH_LEN) && moSendList.IsEmpty()) {
        memcpy(mszResendBuffer + miResendLength, lszSendBuffer, liSendLen);
        miResendLength += liSendLen;
    } else {
        STRU_NET_DATA_INFO struNetDataInfo;
        struNetDataInfo.buffer =  new char[liSendLen];
        memset(struNetDataInfo.buffer, 0, liSendLen);
        memcpy(struNetDataInfo.buffer, lszSendBuffer, liSendLen);
        struNetDataInfo.length = liSendLen;
        moSendList.PushBack(struNetDataInfo);
    }
    
    return 1;
}
/************************************************************************
����ֵ��
1   ���������������ȴ��´��¼�֪ͨ��  ��Ҫ����socket������
0   ������������
-1  �������ӳ������⣬��Ҫ�Ͽ�������
************************************************************************/
int CNetSocket::SendCacheData()
{
    if(miResendLength != 0) {
        int nRet = 0;
        nRet = _SendData(mszResendBuffer, miResendLength);
        TRACE(5, "send data length : " << nRet);
        
        if(nRet == 0) {
            mbCanSend = false;
            return 1;
        }
        
        if(nRet < 0) {
            TRACE(1, "CNetSocket::SendCacheData nRet = " << nRet << " miResendLength = " << miResendLength);
            return -1;
        }
        
        if((nRet < miResendLength) && (nRet > 0)) {
            miResendLength -= nRet;
            ASSERT(miResendLength > 0);
            memmove(mszResendBuffer, mszResendBuffer + nRet, miResendLength + 1);
            mbCanSend = false;
            return 1;
        }
        
        if(nRet > miResendLength) {
            mbCanSend = false;
            TRACE(1, "CNetSocket::SendCacheData nRet = " << nRet << " miResendLength = " << miResendLength);
            return -1;
        }
    }
    
    miResendLength = 0;
    memset(mszResendBuffer, 0, SEND_CATCH_LEN);
    return 0;
}

/************************************************************************
����ֵ��
-1  �������ӳ������⣬��Ҫ�Ͽ�������
0    ���������˳�
1     ���������������ȴ��´��¼�֪ͨ��  ��Ҫ����socket������
2   û�����ݿ��Է���
3   �������Ӳ����Է��ͣ�ֱ�ӷ���
************************************************************************/
int CNetSocket::SendData()
{
    if(!mbCanSend) {
        return 3;
    }
    
    CAutoLock lock(moSendSection);
    
    if(0 == miResendLength && moSendList.IsEmpty()) {
        return 2;
    }
    
    int nSendCacheRtn = SendCacheData();
    
    if(0 != nSendCacheRtn) {
        return nSendCacheRtn;
    }
    
    STRU_NET_DATA_INFO loNetDataInfo;
    
    //������������ݷ�����Ͼ���while
    while(moSendList.TryPop(loNetDataInfo)) {
        if(miResendLength + loNetDataInfo.length <= DEF_SOCKET_CATCH_LEN) {
            STRU_NET_DATA_INFO info;
            moSendList.PopFront(info);
            ASSERT(info.length == loNetDataInfo.length);
            ASSERT(info.buffer == loNetDataInfo.buffer);
            memcpy(mszResendBuffer + miResendLength, loNetDataInfo.buffer, loNetDataInfo.length);
            miResendLength += loNetDataInfo.length;
            delete [] loNetDataInfo.buffer;
            loNetDataInfo.buffer = NULL;
            loNetDataInfo.length = 0;
        } else {
            int liReSendCache = SendCacheData();
            
            if(0 != liReSendCache) {
                TRACE(1, "CNetSocket::SendData ��������������socket ����1��fd : " << miSocket);
                return liReSendCache;
            }
        }
    }
    
    int iRtn = SendCacheData();
    
    if(0 != iRtn) {
        TRACE(1, "CNetSocket::SendData ��������������socket ����2��fd : " << miSocket);
        return iRtn;
    }
    
    return 0;
}

/************************************************************************
����ֵ��
>0 ���͵��ֽ���
0    ���������������Ҫ�ȴ��´�֪ͨ
-1  �������ӳ����쳣
************************************************************************/
int CNetSocket::_SendData(const char* buffer, const int length)
{
    int nRet = mp_net_io->write(miSocket, buffer, length);
    int err = errno;
    int size = 0;
    //memcpy(&size, buffer, 4);
    TRACE(5, "size : " << size);
    
    if(nRet > 0) {
        return nRet;
    } else if(nRet < 0) {
        if((err == EAGAIN) || (err == EWOULDBLOCK) || (EINTR == err)) {
            return 0;
        } else {
            TRACE(1, "CNetSocket::_SendData ��������ʧ�ܡ�errno = " << err);
            return -1;
        }
    } else if(nRet == 0) {
        TRACE(1, "CNetSocket::_SendData ��������ʧ�ܡ�errno = " << err);
        return -1;
    }
    
    return 0;
}
bool CNetSocket::RecvData(char* buffer, int& length)
{
    ASSERT(buffer != NULL);
    CAutoLock lock(moRecvSection);
    
    if(moRecvList.IsEmpty())
        return false;
        
    STRU_NET_DATA_INFO loDataInfo;
    moRecvList.PopFront(loDataInfo);
    memcpy(buffer, loDataInfo.buffer, loDataInfo.length);
    length = loDataInfo.length;
    delete [] loDataInfo.buffer;
    loDataInfo.buffer = NULL;
    return true;
}
bool CNetSocket::RecvData()
{
    char lszBuffer[DEF_SOCKET_CATCH_LEN];
    int liLength = DEF_SOCKET_CATCH_LEN;
    int err = 0;
    CAutoLock lock(moRecvSection);
    
    while(moNetStat == COMMON_TCP_ESTABLISHED || moNetStat == COMMON_TCP_CONNECTED) {
        memset(lszBuffer, 0, DEF_SOCKET_CATCH_LEN);
        liLength = DEF_SOCKET_CATCH_LEN;
        int nRet = mp_net_io->read(miSocket, lszBuffer, liLength);
        err = errno;
        
        if(nRet > 0) {
            ASSERT((miRecvCacheLength + nRet) <= (RECV_CATCH_LEN - 1));
            memcpy(mszRecvCache + miRecvCacheLength, lszBuffer, nRet);
            miRecvCacheLength += nRet;
            
            while(miRecvCacheLength > m_pNetPack->_min_pack_size) {
                char lszRecvBuffer[m_pNetPack->_max_pack_size];
                memset(lszRecvBuffer, 0, m_pNetPack->_max_pack_size);
                uint32_t liRecvBufferLen = m_pNetPack->_max_pack_size;
                uint32_t liRecvDataLen = 0;
                int ret = m_pNetPack->Unpack((const char*)mszRecvCache, miRecvCacheLength, lszRecvBuffer, liRecvBufferLen, liRecvDataLen);
                
                if(ret > 0) {
                    STRU_NET_DATA_INFO loNetDataInfo;
                    char* lpBuffer = new char[liRecvBufferLen + 1];
                    memset(lpBuffer, 0, liRecvBufferLen + 1);
                    memcpy(lpBuffer, lszRecvBuffer, liRecvBufferLen);
                    loNetDataInfo.length = liRecvBufferLen;
                    loNetDataInfo.buffer = lpBuffer;
                    moRecvList.PushBack(loNetDataInfo);
                    miRecvCacheLength -= liRecvDataLen;
                    assert(miRecvCacheLength >= 0);
                    
                    if(miRecvCacheLength > 0) {
                        memmove(mszRecvCache, mszRecvCache + liRecvDataLen, miRecvCacheLength + 1);
                    } else {
                        memset(mszRecvCache, 0, RECV_CATCH_LEN);
                    }
                } else if(ret == 0) {
                    break;
                } else {
                    TRACE(1, "CNetSocket::RecvData unpack failed, ret : " << ret);
                    return false;
                }
            }
            
            if(nRet >= DEF_SOCKET_CATCH_LEN) {
                continue;
            } else {
                return true;
            }
        } else if(nRet == 0) {
            TRACE(1, "CNetSocket::RecvData recv errno : " << err << " fd = " << miSocket);
            return false;
        } else if((nRet < 0) && (errno != ECONNRESET)) {
            return true;
        }
    }
    
    TRACE(1, "CNetSocket::RecvData return recv errno : " << err << " fd = " << miSocket);
    return false;
}



















