#include "libnet.h"
#include "dbmanager.h"

CDBManager::CDBManager()
{
    _connection = NULL;
}

CDBManager::~CDBManager()
{
    if(NULL != _connection)
        mysql_close(_connection) ;
        
    _connection = NULL;
}

int CDBManager::init(void)
{
    _current = 1;
    _dbuser = "root";
    _dbpasswd = "binlog";
    _charset = "gbk";
    _reconn_sec = 5;
    
    for(size_t i = 0; i < 1; ++i)
        _iplist.push_back("127.0.0.1");
        
    
    _dbport = 8766;
    //_dbport = 3306;
    
    if(_dbport <= 0) {
        LOG_FATAL("parse host port failed");
        return -1;
    }
    
    connect();
    return 0;
}

void CDBManager::display()
{
    printf("reconn_sec: %d\n", _reconn_sec);
    printf("dbhost: %s\n", _dbhost.c_str());
    printf("dbport: %d\n", _dbport);
    printf("dbuser: %s\n", _dbuser.c_str());
    printf("charset: %s\n", _charset.c_str());
    
    for(size_t i = 0; i < _iplist.size(); ++i)
        printf("ip: %s\n", _iplist[i].c_str());
}

void CDBManager::connect()
{
    if(_connection != NULL) {
        return ;
    }
    
    mysql_init(&_mysql);
    uint32_t retry = 0;
    //timeout 10s
    uint32_t timeout = 10;
    int ret = 0;
    ret = mysql_options(&_mysql, MYSQL_OPT_WRITE_TIMEOUT, &timeout);
    
    if(ret != 0) {
        LOG_WARNING("set write timeout failed.");
    }
    
    ret = mysql_options(&_mysql, MYSQL_OPT_READ_TIMEOUT, &timeout);
    
    if(ret != 0) {
        LOG_WARNING("set read timeout faild.");
    }
    
    ret = mysql_options(&_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &timeout);
    
    if(ret != 0) {
        LOG_WARNING("set connect timeout faild.");
    }
    
    bool ml_false = false;
    ret = mysql_options(&_mysql, MYSQL_OPT_RECONNECT, &ml_false);
    
    if(ret != 0) {
        LOG_WARNING("set connection not reconnect failed.");
    }
    
    while(true) {
        if(retry++ > 0) {
            LOG_FATAL("query db fail. retry:%u", retry);
        }
        
        sleep(1);
        _current = (_current + 1) % _iplist.size();
        _cur_host = _iplist[_current];
        LOG_NOTICE("get host by name start.");
        std::string str_ip = "";
        GetIpByHostName(_cur_host, str_ip);
        
        if(str_ip.empty()) {
            LOG_NOTICE("gethostbyname error. host: %s", _cur_host);
            return;
        }
        
        LOG_NOTICE("get host by name end. ip: %s", str_ip.c_str());
        _connection = mysql_real_connect(&_mysql, str_ip.c_str(), _dbuser.c_str(),
                                         _dbpasswd.c_str(), NULL, _dbport, NULL, CLIENT_MULTI_STATEMENTS);
                                         
        if(_connection == NULL) {
            LOG_WARNING("conn db failed.[ip:%s][port:%d][user:%s][passwd:%s] error:[%s]", str_ip.c_str(), _dbport, _dbuser.c_str(), _dbpasswd.c_str(), mysql_error(&_mysql));
        } else {
            std::string sql("set names ");
            sql += _charset;
            
            if(0 != mysql_query(_connection, sql.c_str())) {
                LOG_WARNING("set names failed.[charset:%s]", _charset.c_str());
                _connection = NULL;
            } else {
                LOG_NOTICE("conn db success.[ip:%s][port:%d][user:%s][passwd:%s]",
                           _iplist[_current].c_str(), _dbport, _dbuser.c_str(), _dbpasswd.c_str());
                break;
            }
        }
    }
    
    return;
}

int CDBManager::query_ex(const char* sql, uint64_t sql_len, MYSQL_RES** result)
{
    if(sql == NULL || result == NULL) {
        LOG_FATAL("param is NULL");
        return -1;
    }
    
    uint32_t retry = 0;
    
    while(true) {
        if(retry++ >= 5) {
            LOG_FATAL("query db fail. retry:%u.[sql:%s]", retry, sql);
            return -1;
        }
        
        connect();
        
        if(mysql_real_query(_connection, sql, sql_len) != 0) {
            LOG_WARNING("db query failed.[retry:%d][errno:%d][error:%s][sql:%s]",
                        retry, mysql_errno(_connection), mysql_error(_connection), sql);
            mysql_close(_connection);
            _connection = NULL;
        } else {
            *result = mysql_store_result(_connection);
            
            if(*result != NULL)
                break;
                
            LOG_WARNING("db store result failed.[sql:%s][errno:%d][error:%s]",
                        sql, mysql_errno(_connection), mysql_error(_connection));
            mysql_close(_connection);
            _connection = NULL;
        }
    }
    
    //int rownum = (int) mysql_num_rows(*result);
    LOG_TRACE("sql exec succ.[record number: %d][sql:%s]", (int) mysql_num_rows(*result), sql);
    return 0;
}

int CDBManager::query(const char* sql, uint64_t sql_len)
{
    if(sql == NULL) {
        LOG_FATAL("sql is NULL");
        return -1;
    }
    
    uint32_t retry = 0;
    
    while(true) {
        if(retry++ >= 5) {
            LOG_FATAL("query db fail. retry:%u.[sql:%s]", retry, sql);
            return -1;
        }
        
        connect();
        
        if(mysql_real_query(_connection, sql, sql_len) != 0) {
            LOG_WARNING("db query failed.[retry:%d][errno:%d][error:%s][sql:%s]",
                        retry, mysql_errno(_connection), mysql_error(_connection), sql);
            mysql_close(_connection);
            _connection = NULL;
            continue;
        } else {
            break;
        }
    }
    
    LOG_TRACE("sql exec succ.[sql:%s]", sql);
    return 0;
}


