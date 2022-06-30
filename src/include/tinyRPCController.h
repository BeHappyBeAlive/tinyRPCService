#pragma once

#include <google/protobuf/service.h>

/*
    rpc服务框架中的控制类  对rpc中出现的流程的状态 如失败、退出等记录在此类中，进行展示控制
*/
class TinyRPCController : public google::protobuf::RpcController
{
public:
    TinyRPCController();
    
    void Reset();

    bool Failed();

    std::string ErrorText();

    void StartCancel();

    void SetFailed(const std::string &reason);

    bool IsCanceled();

    void NotifyOnCancel(google::protobuf::Closure *callback);

private:
    // rpc执行过程中的状态，如果失败则为true，否则为false
    bool m_rpcFailed;
    // rpc执行过程中的错误文本，如果上述状态为true则将错误信息设置在此成员变量中
    std::string m_rpcErrText;
};