#include "tinyLogger.h"
#include <time.h>
#include <iostream>
#include <utility>

TinyRPCLogger &TinyRPCLogger::getInstance()
{
    static TinyRPCLogger logger;
    return logger;
}
//日志存入线程安全的日志字符串队列
void TinyRPCLogger::putLogIntoQueue(std::string logStr, int logType)
{
    //将日志信息写入到缓存队列当中
    m_lockLogQueue.Push(logStr, logType);
}
//设置日志级别
// void TinyRPCLogger::setLogLevel(TinyLogLevel level)
// {
//     //m_logLevel = level;
// }

TinyRPCLogger::TinyRPCLogger()
{
    //启动写日志线程
    std::thread writeLoggerTask([&]()
    {
        for (;;)
        {
            //日志文件写在log/yyyy-MM-dd-log.txt中
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);
            char fileNameBuf[128];
            sprintf(fileNameBuf, "../log/%d-%d-%d-log.txt", nowtm->tm_year + 1900, nowtm->tm_mon + 1, nowtm->tm_mday);
            FILE *pf = fopen(fileNameBuf, "a+");
            if (pf == nullptr)
            {
                std::cout<<"logger open file : "<<fileNameBuf<<" failed!"<<std::endl;
                exit(EXIT_FAILURE);
            }
            std::pair<std::string,int> pair = m_lockLogQueue.POP();
            std::string LogStr = pair.first;
            int curLogType = pair.second;
            char timeBuf[128]={0};
            sprintf(timeBuf,"%d:%d:%d => [%s]",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec,
                /*(m_logLevel == _LOG_INFO?"LOG_INFO":"LOG_ERR"*/(curLogType == 0?"LOG_INFO":"LOG_ERR")
            );
            LogStr.insert(0,timeBuf);
            LogStr.append("\n");
            fputs(LogStr.c_str(),pf);
            fclose(pf);
        } });
    //设置分离线程 相当于守护线程
    writeLoggerTask.detach();
}