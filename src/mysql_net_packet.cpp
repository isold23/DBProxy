#include "mysql_net_packet.h"

namespace dbproxy
{

int CMySQLNetPack::Pack(const char* in_buffer, const uint32_t in_length,
                        char* out_buffer, uint32_t& out_length)
{
    try {
        send_pack_length = in_length;
        CStandardSerialize loSerialize(out_buffer, out_length, CStandardSerialize::STORE);
        memset(out_buffer, 0, out_length);
        
        if(Serialize(loSerialize) == -1)
            return -1;
        else {
            memcpy(out_buffer + _min_pack_size, in_buffer, in_length);
            out_length = _min_pack_size + in_length;
        }
        
        return 1;
    } catch(...) {
        return -1;
    }
}
int CMySQLNetPack::Unpack(const char* in_buffer, const uint32_t in_length,
                          char* out_buffer, uint32_t& out_buffer_length, uint32_t& out_data_length)
{
    try {
        CStandardSerialize loSerialize((char*)in_buffer, in_length, CStandardSerialize::LOAD);
        
        if(Serialize(loSerialize) == -1) {
            return -1;
        } else {
            if(CheckPack()) {
                if(in_length >= _min_pack_size + recv_pack_length) {
                    memcpy(out_buffer, in_buffer + _min_pack_size, recv_pack_length);
                    out_buffer_length = recv_pack_length;
                    out_data_length = recv_pack_length + _min_pack_size;
                } else {
                    return 0;
                }
            } else {
                return -1;
            }
        }
        
        return 1;
    } catch(...) {
        TRACE(1, "version3 unpacke exception.");
        return -1;
    }
}
int CMySQLNetPack::Serialize(CStandardSerialize& aoStandardSerialize)
{
    try {
        if(aoStandardSerialize.mbyType ==  CStandardSerialize::STORE) {
            uint8_t buf[4];
            memset(buf, 0, 4);
            int3store(buf, static_cast<uint32_t>(send_pack_length));
            buf[3] = send_seq;
            aoStandardSerialize.Serialize(buf, 4);
        } else if(aoStandardSerialize.mbyType == CStandardSerialize::LOAD) {
            uint8_t buf[4];
            memset(buf, 0, 4);
            int len = 4;
            aoStandardSerialize.Serialize(buf, 4);
            //recv_pack_length = ntohl(recv_pack_length);
            recv_pack_length = uint3korr(buf);
            recv_seq = buf[3];
            TRACE(3, "recv req: "<<recv_seq);
        }
        
        return 1;
    } catch(...) {
        TRACE(1, "version3 serialize exception.");
        return -1;
    }
}

}//end of namespace dbproxy
