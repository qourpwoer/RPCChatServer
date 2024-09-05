#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "chatservice.hpp"

int main(int argc, char **argv)
{

    // 调用框架的初始化操作
    MprpcApplication::Init(argc, argv);

    // 把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new ChatService());


    // 启动一个rpc服务节点 Run以后进程进入阻塞状态
    provider.Run();
    // 启动rpc服务发布节点

    return 0;
}
