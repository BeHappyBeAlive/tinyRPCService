#pragma once
#include <string>
#include "lockTinyLoggerQueue.h"
//日志级别 Info/错误
enum TinyLogLevel
{
    _LOG_INFO, //日志消息
    _LOG_ERR,  //日志错误
};

// tinyRPC框架所提供的日志类
class TinyRPCLogger
{
public:
    static TinyRPCLogger &getInstance();
    //日志存入线程安全的日志字符串队列 0 logInfo 1 logErr
    void putLogIntoQueue(std::string logStr, int logType);
    //设置日志级别
    //void setLogLevel(TinyLogLevel level);

private:
    //日志级别成员变量
    //TinyLogLevel m_logLevel;
    TinyRPCLogger();
    TinyRPCLogger(const TinyRPCLogger &) = delete;
    TinyRPCLogger(TinyRPCLogger &&) = delete;
    //线程安全的队列
    LockTinyLoggerQueue<std::string> m_lockLogQueue;
};

#define LOG_INFO(inputLogStrFormat, ...)                                   \
    do                                                                     \
    {                                                                      \
        TinyRPCLogger &logger = TinyRPCLogger::getInstance();              \
        char logFormatStrBuf[1024] = {0};                                  \
        snprintf(logFormatStrBuf, 1024, inputLogStrFormat, ##__VA_ARGS__); \
        logger.putLogIntoQueue(logFormatStrBuf, _LOG_INFO);                \
    } while (0);

#define LOG_ERR(inputLogStrFormat, ...)                                    \
    do                                                                     \
    {                                                                      \
        TinyRPCLogger &logger = TinyRPCLogger::getInstance();              \
        char logFormatStrBuf[1024] = {0};                                  \
        snprintf(logFormatStrBuf, 1024, inputLogStrFormat, ##__VA_ARGS__); \
        logger.putLogIntoQueue(logFormatStrBuf, _LOG_ERR);                 \
    } while (0);\
