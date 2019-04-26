#include "dp_config.h"

namespace dbproxy
{

bool CDPConfig::parse_value(const char* key, const char* value)
{
    if(!strcmp(key, "host")) {
        m_iHost = ntohl(inet_addr(value));
        return true;
    }
    
    if(!strcmp(key, "port")) {
        m_iPort = (int)strtol(value, NULL, 0);
        return true;
    }
    
    if(!strcmp(key, "max_bind_times")) {
        m_iMaxBindTimes = (int)strtol(value, NULL, 0);
        return true;
    }
    
    if(!strcmp(key, "max_processor")) {
        m_iMaxSubWorker = (int)strtol(value, NULL, 0);
        return true;
    }
    
    if(!strcmp(key, "max_file_size")) {
        m_iMaxFileSize = (int)strtol(value, NULL, 0);
        return true;
    }
    
    return true;
}

void CDPConfig::print()
{
}


} //end of namespace dbproxy
