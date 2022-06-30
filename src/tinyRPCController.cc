#include "tinyRPCController.h"

TinyRPCController::TinyRPCController()
    : m_rpcFailed(false),
      m_rpcErrText("")
{
}

void TinyRPCController::Reset()
{
    m_rpcFailed = false;
    m_rpcErrText = "";
}

bool TinyRPCController::Failed()
{
    return m_rpcFailed;
}

std::string TinyRPCController::ErrorText()
{
    return m_rpcErrText;
}

void TinyRPCController::StartCancel()
{
}

void TinyRPCController::SetFailed(const std::string &reason)
{
    m_rpcFailed = true;
    m_rpcErrText = reason;
}

bool TinyRPCController::IsCanceled()
{
    return false;
}

void TinyRPCController::NotifyOnCancel(google::protobuf::Closure *callback)
{
}