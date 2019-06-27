#ifndef _MYSQL_FUNCTION_H_
#define _MYSQL_FUNCTION_H_

#include "include.h"

namespace dbproxy
{

#define net_new_transaction(net) ((net)->pkt_nr=0)

static inline int32_t sint3korr(const uint8_t* A)
{
    return
        ((int32_t)(((A[2]) & 128) ?
                   (((uint32_t) 255L << 24) |
                    (((uint32_t) A[2]) << 16) |
                    (((uint32_t) A[1]) << 8) |
                    ((uint32_t) A[0])) :
                   (((uint32_t) A[2]) << 16) |
                   (((uint32_t) A[1]) << 8) |
                   ((uint32_t) A[0])))
        ;
}

static inline uint32_t uint3korr(const uint8_t* A)
{
    return
        (uint32_t)(((uint32_t)(A[0])) +
                   (((uint32_t)(A[1])) << 8) +
                   (((uint32_t)(A[2])) << 16))
        ;
}

static inline uint64_t uint5korr(const uint8_t* A)
{
    return
        ((uint64_t)(((uint32_t)(A[0])) +
                    (((uint32_t)(A[1])) << 8) +
                    (((uint32_t)(A[2])) << 16) +
                    (((uint32_t)(A[3])) << 24)) +
         (((uint64_t)(A[4])) << 32))
        ;
}

static inline uint64_t uint6korr(const uint8_t* A)
{
    return
        ((uint64_t)(((uint32_t)(A[0]))          +
                    (((uint32_t)(A[1])) << 8)   +
                    (((uint32_t)(A[2])) << 16)  +
                    (((uint32_t)(A[3])) << 24)) +
         (((uint64_t)(A[4])) << 32) +
         (((uint64_t)(A[5])) << 40))
        ;
}

static inline void int3store(uint8_t* T, uint32_t A)
{
    *(T) = (uint8_t)(A);
    *(T + 1) = (uint8_t)(A >> 8);
    *(T + 2) = (uint8_t)(A >> 16);
}

static inline void int5store(uint8_t* T, uint64_t A)
{
    *(T) = (uint8_t)(A);
    *(T + 1) = (uint8_t)(A >> 8);
    *(T + 2) = (uint8_t)(A >> 16);
    *(T + 3) = (uint8_t)(A >> 24);
    *(T + 4) = (uint8_t)(A >> 32);
}

static inline void int6store(uint8_t* T, uint64_t A)
{
    *(T) = (uint8_t)(A);
    *(T + 1) = (uint8_t)(A >> 8);
    *(T + 2) = (uint8_t)(A >> 16);
    *(T + 3) = (uint8_t)(A >> 24);
    *(T + 4) = (uint8_t)(A >> 32);
    *(T + 5) = (uint8_t)(A >> 40);
}

static inline int16_t sint2korr(const uint8_t* A)
{
    return
        (int16_t)(((int16_t)(A[0])) +
                  ((int16_t)(A[1]) << 8))
        ;
}

static inline int32_t sint4korr(const uint8_t* A)
{
    return
        (int32_t)(((int32_t)(A[0])) +
                  (((int32_t)(A[1]) << 8)) +
                  (((int32_t)(A[2]) << 16)) +
                  (((int32_t)(A[3]) << 24)))
        ;
}

static inline uint16_t uint2korr(const uint8_t* A)
{
    return
        (uint16_t)(((uint16_t)(A[0])) +
                   ((uint16_t)(A[1]) << 8))
        ;
}

static inline uint32_t uint4korr(const uint8_t* A)
{
    return
        (uint32_t)(((uint32_t)(A[0])) +
                   (((uint32_t)(A[1])) << 8) +
                   (((uint32_t)(A[2])) << 16) +
                   (((uint32_t)(A[3])) << 24))
        ;
}

static inline uint64_t uint8korr(const uint8_t* A)
{
    return
        ((uint64_t)(((uint32_t)(A[0])) +
                    (((uint32_t)(A[1])) << 8) +
                    (((uint32_t)(A[2])) << 16) +
                    (((uint32_t)(A[3])) << 24)) +
         (((uint64_t)(((uint32_t)(A[4])) +
                      (((uint32_t)(A[5])) << 8) +
                      (((uint32_t)(A[6])) << 16) +
                      (((uint32_t)(A[7])) << 24))) <<
          32))
        ;
}

static inline int64_t  sint8korr(const uint8_t* A)
{
    return (int64_t) uint8korr(A);
}

static inline void int2store(uint8_t* T, uint16_t A)
{
    uint32_t def_temp = A ;
    *(T) = (uint8_t)(def_temp);
    *(T + 1) = (uint8_t)(def_temp >> 8);
}

static inline void int4store(uint8_t* T, uint32_t A)
{
    *(T) = (uint8_t)(A);
    *(T + 1) = (uint8_t)(A >> 8);
    *(T + 2) = (uint8_t)(A >> 16);
    *(T + 3) = (uint8_t)(A >> 24);
}

static inline void int8store(uint8_t* T, uint64_t A)
{
    uint32_t def_temp = (uint32_t) A,
             def_temp2 = (uint32_t)(A >> 32);
    int4store(T,  def_temp);
    int4store(T + 4, def_temp2);
}


static inline int16_t sint2korr(const char* pT)
{
    return sint2korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline uint16_t uint2korr(const char* pT)
{
    return uint2korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline uint32_t uint3korr(const char* pT)
{
    return uint3korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline int32_t  sint3korr(const char* pT)
{
    return sint3korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline uint32_t uint4korr(const char* pT)
{
    return uint4korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline int32_t     sint4korr(const char* pT)
{
    return sint4korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline uint64_t uint6korr(const char* pT)
{
    return uint6korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline uint64_t uint8korr(const char* pT)
{
    return uint8korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}

static inline int64_t  sint8korr(const char* pT)
{
    return sint8korr(static_cast<const uint8_t*>(static_cast<const void*>(pT)));
}


static inline void int2store(char* pT, uint16_t A)
{
    int2store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static inline void int3store(char* pT, uint32_t A)
{
    int3store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static inline void int4store(char* pT, uint32_t A)
{
    int4store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static inline void int5store(char* pT, uint64_t A)
{
    int5store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static inline void int6store(char* pT, uint64_t A)
{
    int6store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static inline void int8store(char* pT, uint64_t A)
{
    int8store(static_cast<uint8_t*>(static_cast<void*>(pT)), A);
}

static uint8_t* net_store_length(uint8_t* packet, uint64_t length)
{
    if(length < (uint64_t) 251LL) {
        *packet = (uint8_t) length;
        return packet + 1;
    }
    
    /* 251 is reserved for NULL */
    if(length < (uint64_t) 65536LL) {
        *packet++ = 252;
        int2store(packet, (uint32_t) length);
        return packet + 2;
    }
    
    if(length < (uint64_t) 16777216LL) {
        *packet++ = 253;
        int3store(packet, (uint32_t) length);
        return packet + 3;
    }
    
    *packet++ = 254;
    int8store(packet, length);
    return packet + 8;
}

static uint8_t* net_store_length_fast(uint8_t* packet, size_t length)
{
    if(length < 251) {
        *packet = (uint8_t) length;
        return packet + 1;
    }
    
    *packet++ = 252;
    int2store(packet, (uint32_t) length);
    return packet + 2;
}

//The following will only be used for short strings < 65K
static uint8_t* net_store_data(uint8_t* to, const uint8_t* from, size_t length)
{
    to = net_store_length_fast(to, length);
    memcpy(to, from, length);
    return to + length;
}

}// end of namespace dbproxy
#endif //_MYSQL_FUNCTION_H_
