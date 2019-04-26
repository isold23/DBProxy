#include <iostream>
#include "include.h"
#include "debugtrace.h"
#include "dbmanager.h"

CDebugTrace* goDebugTrace = new CDebugTrace();

int main()
{
    char lszLogFileName[255];
    memset(lszLogFileName, 0, 255);
    CCommon::GetAppPath(lszLogFileName, 255);
    std::string lstrAppPath = lszLogFileName;
    std::string lstrApp = lstrAppPath.substr(0, lstrAppPath.find_last_of('/'));
    
    if(chdir(lstrApp.c_str())) {
        std::cout << "crs_trans error: chdir error." << lszLogFileName << std::endl;
        return 0;
    }
    unsigned int process_id = CCommon::GetProcessId();
    SET_TRACE_LEVEL(5);
    unsigned liOptions = (CDebugTrace::Timestamp | CDebugTrace::LogLevel & ~CDebugTrace::FileAndLine | CDebugTrace::AppendToFile);
    SET_TRACE_OPTIONS(GET_TRACE_OPTIONS() | liOptions);
    strcpy(strrchr(lszLogFileName, '/'), "//dbproxy_test_log");
    CCommon::CreatePath(lszLogFileName);
    char lszFileDate[50];
    memset(lszFileDate, 0, 50);
    sprintf(lszFileDate, "//dbproxy_test-%u", process_id);
    strcat(lszLogFileName, lszFileDate);
    SET_LOG_FILENAME(lszLogFileName);
    TRACE(3, "\n\n*******************DBPROXY VERSION:" <<
          "1.0.0.0" << "*******************");
    
    CDBManager man;
    man.init();
    man.connect();
    std::string sql = "select * from test.user;";
    MYSQL_RES* pRes = NULL;
    man.query(sql.c_str(), sql.length());
    pRes = mysql_store_result(man._connection);
    
    if(!pRes) {
        std::cout << "get result failed." << std::endl;
        return -1;
    }
    
    uint32_t num_fields = mysql_num_fields(pRes);
    uint32_t num_rows = mysql_num_rows(pRes);
    std::cout << "fields number: " << num_fields << " rows number: " << num_rows << std::endl;
    return 0;
}
