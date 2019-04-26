#ifndef _DBMANAGER_H
#define _DBMANAGER_H

#include <string>
#include <vector>
#include "mysql.h"

class CDBManager
{
public:
    CDBManager();
    ~CDBManager();
    int init();
    void connect();
    int query_ex(const char* sql, uint64_t sql_len, MYSQL_RES** result);
    int query(const char* sql, uint64_t sql_len);
    void display();
    
private:
    CDBManager(const CDBManager&);
    CDBManager& operator= (const CDBManager&);
    
private:
    std::string _cur_host;
    std::string _dbhost;
    uint32_t _dbport;
    uint32_t _reconn_sec;
    std::string _dbuser;
    std::string _dbpasswd;
    std::string _charset;
    std::vector<std::string> _iplist;
    MYSQL _mysql;
    int _current;
public:
    MYSQL* _connection;
};
#endif
