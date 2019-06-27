#include "mysql_net.h"

namespace dbproxy {


    int CMySQLNet::init(std::string& h, uint16_t p, std::string& u, std::string& psw){
    
    host = h;
    port = p; 
    user = u;
    password = psw;
    int ret = connect();
    if(ret != 0) {
      TRACE(1, "connect failed.");
      return -1;
    }
    return 0;
    }

    int CMySQLNet::destroy() {
        return 0;
    }

    int CMySQLNet::connect() {
        if(m_ptrMySQL != NULL) {
            disconnect();
            return 0;
        }
        
        m_ptrMySQL = mysql_init(NULL);
        if(m_ptrMySQL == NULL) {
            TRACE(1, "mysql init faild.");
            return -1;
        }
 
        int ret = 0;
        uint32_t timeout = 10;  // 10s
        ret = mysql_options(m_ptrMySQL, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
        if (ret != 0) {
          TRACE(1, "set write timeout failed.");
        }

        ret = mysql_options(m_ptrMySQL, MYSQL_OPT_READ_TIMEOUT, &timeout);
        if (ret != 0) {
          TRACE(1, "set read timeout faild.");
        }

        ret = mysql_options(m_ptrMySQL, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
        if (ret != 0) {
          TRACE(1, "set connect timeout faild.");
        }

        bool ml_false = false;
        ret = mysql_options(m_ptrMySQL, MYSQL_OPT_RECONNECT, &ml_false);
        if (ret != 0) {
          TRACE(1, "set connection not reconnect failed.");
        }

        if (NULL == mysql_real_connect(m_ptrMySQL,
                                      host.c_str(),
                                      user.c_str(),
                                      password.c_str(),
                                      NULL,
                                      port,
                                      NULL,
                                      CLIENT_MULTI_STATEMENTS)) {
          TRACE(1, "connect to mysql failed. [host="<<host.c_str()<<" user="<<user.c_str()<<" password="<<password.c_str()<<" port="<<port<<"], error:"<<mysql_error(m_ptrMySQL));
          mysql_close(m_ptrMySQL);
          return -1;
        } else {
          TRACE(5, "connect mysql successful. host: "<<host.c_str());
        }
        fd = m_ptrMySQL->net.fd;
        TRACE(5, "fd: "<<fd)
        return 0;
    }
    int CMySQLNet::disconnect() {
        return 0;
    }
    int CMySQLNet::sendPacket(char* buffer, int len){
        char packet[1024];
        memset(packet, 0, sizeof(packet));
        packet[MYSQL_PACKET_HEADER_LEN] = COM_QUERY;
        int payload_len = len+1;
        memcpy(packet + 5, buffer, len);
        int body_len = payload_len + MYSQL_PACKET_HEADER_LEN;
        int3store(packet, static_cast<uint>(payload_len));
        packet[3] = 0;
        int ret = send(fd, packet, body_len, 0);
        if(ret < 0 || ret != body_len) {
          TRACE(1, "sendPacket failed. ret: "<<ret);
          return -1;
        }
        TRACE(5, "send succ. send succ len: "<<ret<<" sql len: "<<len);
        TRACE(5, "send succ. len: "<<ret<<" body_len: "<<body_len<<" fd: "<<fd);
        return 0;
    }
    int CMySQLNet::recvPacket(char* buffer, int& len){
         TRACE(5, "recv packet........................");
         char header[MYSQL_PACKET_HEADER_LEN];
         memset(header, 0, MYSQL_PACKET_HEADER_LEN);
         int ret = recv(fd, header, MYSQL_PACKET_HEADER_LEN, 0);
         if (ret != MYSQL_PACKET_HEADER_LEN) {
           TRACE(1, "recv header failed. ret: "<<ret<<" error: "<<errno);
           return -1;
         }

         TRACE(5, "recv len: "<<ret);
         uint32_t payload_len = uint3korr((uint8_t*)header);
         TRACE(5, "must recv len: "<<payload_len);
         ret = recv(fd, buffer, payload_len, 0);
         if(ret != payload_len) {
           TRACE(1, "recv payload data failed. ret: "<<ret<<" error: "<<errno);
           return -1;
         }
         len = payload_len;
         TRACE(5, "recv len: "<<ret);
        return 0;
    }

    int CMySQLNetList::init(CDPConfig* apDPConfig) {
      m_pDPConfig = apDPConfig;
      
      CMySQLNet* ptr = new CMySQLNet;
      int ret = ptr->init(m_pDPConfig->whost, m_pDPConfig->wport, m_pDPConfig->wuser, m_pDPConfig->wpassword);
      if(ret != 0) {
        return -1;
      }
      nets[1] = ptr;

      CMySQLNet* ptr1 = new CMySQLNet;
      ret = ptr1->init(m_pDPConfig->rhost, m_pDPConfig->rport, m_pDPConfig->ruser, m_pDPConfig->rpassword);
      if(ret != 0) {
        return -1;
      }
      nets[0] = ptr1;

      return 0;
    }

}


