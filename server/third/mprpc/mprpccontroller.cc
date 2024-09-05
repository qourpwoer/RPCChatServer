#include "mprpccontroller.h"
#include <string>

MprpcController::MprpcController()
{
    m_failed = false;
    m_errText = "";
}

void MprpcController::Reset()
{
}

bool MprpcController::Failed() const
{
    return false;
}

void MprpcController::SetFailed(const std::string &reason)
{
    m_failed = true;
    m_errText = reason;
}
std::string MprpcController::ErrorText() const
{
    return m_errText;
}

// 目前未实现具体的功能
// 目前未实现的功能
void MprpcController::StartCancel() {}
bool MprpcController::IsCanceled() const { return false; }
void MprpcController::NotifyOnCancel(google::protobuf::Closure *callback) {}