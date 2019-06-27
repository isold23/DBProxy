#ifndef _TIME_H_
#define _TIME_H_

#include <string>
#include <iostream>
#include <iomanip>

#include "include.h"

class CTimeBase
{
public:
    static uint64_t get_current_time(void);
    static uint64_t escape_time(uint64_t last_time);
    
    //2009-12-02 12:02:30:234
    static std::string get_now(void);
    static std::string conver_timestamp(uint32_t timestamp);
};

#endif //_TIME_H_




