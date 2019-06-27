/********************************************************************
    created:    2011/03/03
    created:    3:3:2011   10:49
    file base:  UdpSocket
    file ext:   h
    author:     isold.wang@gmail.com

    purpose:    UDPЭ��򵥷�װ��
*********************************************************************/
#ifndef _UDP_SOCKET_H_
#define _UDP_SOCKET_H_

#include "include.h"
#include "net_pack.h"

#define DEF_UDP_PACK_BUF_LEN 1500

class CUdpSocket
{
public:
    CUdpSocket();
    ~CUdpSocket();
    
    //�����ֽ���
    bool CreateSocket(const char* ip, const short port);
    bool CreateSocket(void);
    bool Close(int flag = 2);
    //����socket����
    bool SetNoBlock(void);
    //�������ݼ���Ĭ��Э��ͷ
    bool SendData(const char* buffer, const uint32_t length,
                  const uint32_t dst_ip, const uint16_t dst_port);
    //�������ݰ�����Ĭ��Э��ͷ
    bool RecvData(char* buffer, uint32_t& length,
                  uint32_t& src_ip, uint16_t& src_port);
                  
    //��������δ����Ĭ��Э��ͷ
    bool SendDataWithoutHeader(const char* buffer, const uint32_t length,
                               const uint32_t dst_ip, const uint16_t dst_port);
    //�������ݰ�δ����Ĭ��Э��ͷ
    bool RecvDataWithoutHeader(char* buffer, uint& length,
                               uint32_t& src_ip, uint16_t& src_port);
                               
    //Dump��־
    void Dump(void);
    void TimeWorkFunction(void);
    
private:
    int miSocket;
};


#endif //_UDP_SOCKET_H





