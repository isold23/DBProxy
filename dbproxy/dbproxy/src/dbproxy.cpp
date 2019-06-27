

#include <iostream>
#include "include.h"
#include "debugtrace.h"

#include "dp_config.h"
#include "dp_worker.h"

CDebugTrace* goDebugTrace = NULL;

void help_info()
{
    std::cout << "DB Proxy Server Help Infomation:" << std::endl;
    std::cout << "./dns -f config_file_name" << std::endl;
    std::cout << "./dns -v (dbproxy server version information)" << std::endl;
    std::cout << "./dns -h(dbproxy server help infomation)" << std::endl;
    std::cout << "Copyright (C) Beijing Sogou Information Technology Co.,Ltd 2016-2018" << std::endl;
};


int main(int argc, char* argv[])
{
    if(argc > 1 && strcmp(argv[1], "-v") == 0) {
        std::cout << "SinaShow DNS Version Infomation:" << std::endl;
        std::cout << "Dispatch Node Server Version: " << g_strServerVersion.c_str() << std::endl;
        std::cout << "Copyright (C) Beijing Sina Information Technology Co.,Ltd 2003-2009" << std::endl;
        return 0;
    }
    
    if(argc > 1 && strcmp(argv[1], "-h") == 0) {
        help_info();
        return 0;
    }
    
    if(argc > 1 && strcmp(argv[1], "-f") != 0) {
        help_info();
        return 0;
    }
    
    std::string lstrConfigName = "";
    
    if(argc > 1 && strcmp(argv[1], "-f") == 0) {
        lstrConfigName = argv[2];
        
        if(lstrConfigName.empty()) {
            help_info();
            return 0;
        }
    }
    
    char lszLogFileName[255];
    memset(lszLogFileName, 0, 255);
    CCommon::GetAppPath(lszLogFileName, 255);
    std::string lstrAppPath = lszLogFileName;
    std::string lstrApp = lstrAppPath.substr(0, lstrAppPath.find_last_of('/'));
    
    if(chdir(lstrApp.c_str())) {
        std::cout << "crs_trans error: chdir error." << lszLogFileName << std::endl;
        return 0;
    }
    
    goDebugTrace = new CDebugTrace;
    unsigned int process_id = CCommon::GetProcessId();
    SET_TRACE_LEVEL(5);
    unsigned liOptions = (CDebugTrace::Timestamp | CDebugTrace::LogLevel & ~CDebugTrace::FileAndLine | CDebugTrace::AppendToFile);
    SET_TRACE_OPTIONS(GET_TRACE_OPTIONS() | liOptions);
    strcpy(strrchr(lszLogFileName, '/'), "//dbproxy_log");
    CCommon::CreatePath(lszLogFileName);
    char lszFileDate[50];
    memset(lszFileDate, 0, 50);
    sprintf(lszFileDate, "//dbproxy-%u", process_id);
    strcat(lszLogFileName, lszFileDate);
    SET_LOG_FILENAME(lszLogFileName);
    TRACE(3, "\n\n*******************DBPROXY VERSION:" <<
          g_strServerVersion.c_str() << "*******************");
    TRACE(3, "Config file name: " << lstrConfigName.c_str());
    dbproxy::CDPConfig loDNSConfig;
    loDNSConfig.set_conf_file_name(lstrConfigName.c_str());
    bool bRet = loDNSConfig.load();
    
    if(!bRet) {
        TRACE(1, "ERROR: load config file failed.");
        return 0;
    }
    
    dbproxy::CDPWorker worker;
    worker.setConfig(&loDNSConfig);
    worker.run();
    return 0;
}
