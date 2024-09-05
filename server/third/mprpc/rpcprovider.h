#pragma once
#include <google/protobuf/service.h>
#include <memory>
#include "TcpServer.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "TcpConnection.h"
#include "Buffer.h"
#include "Timestamp.h"
#include <google/protobuf/descriptor.h>
#include <unordered_map>

// 框架提供的专门发布rpc服务的网络对象类
class RpcProvider
{
public:
    // 这里是框架提供给外部使用的，可以发布rpc
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程调用服务
    void Run();

private:
    // 组合EventLoop
    EventLoop m_eventLoop;

    struct ServiceInfo
    {
        google::protobuf::Service *m_service; //保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> m_serviceMap;
    // 新的socket连接回调
    void OnConnection(const TcpConnectionPtr &);
    // 已建立连接用户的读写事件回调
    void OnMessage(const  TcpConnectionPtr &,  Buffer *,  Timestamp);
    // Closure的回调操作，用于序列化rpc的响应和网络发送
    void SendRpcResponse(const TcpConnectionPtr&, google::protobuf::Message*);
};