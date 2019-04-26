#ifndef _DBPROXY_PACKET_H_
#define _DBPROXY_PACKET_H_

namespace dbproxy
{

class CDPPacket
{
public:
    CDPPacket() {}
    ~CDPPacket() {}
    int dealData(const char* apIn, const uint32 aiIn, char* apOut, uint32& aiOut);
public:
    sigslot::signal3<int, int, CBasePack*> DealDataComplete;
};

} //end of namespace dbproxy
#endif //_DBPROXY_PACKET_H_
