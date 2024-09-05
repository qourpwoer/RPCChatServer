#include "gateserver.hpp"
#include "gateservice.hpp"
#include "GateServer.pb.h"
#include <functional>
#include <string>
using namespace placeholders;
using namespace std;
//初始化聊天服务器对象
GateServer::GateServer(EventLoop *loop,
		const InetAddress& listenAddr,
		const string& nameArg)
		:_server(loop, listenAddr, nameArg)
		,_loop(loop)
{
	//注册链接回调
	_server.setConnectionCallback(std::bind(&GateServer::onConnection, this, _1));

	//注册消息回调
	_server.setMessageCallback(std::bind(&GateServer::onMessage, this, _1, _2, _3));

	//设置线程数量
	_server.setThreadNum(3);

}
//启动服务
void GateServer::start()
{
	_server.start();
}

// 上报连接相关信息的回调函数
void GateServer::onConnection(const TcpConnectionPtr& conn)
{
	// 客户端断开链接
	if(!conn->connected())
	{
		GateService::instance()->client_close_exception(conn);
		conn->shutdown();
	}
}

//上报读写事件相关信息的回调函数
void GateServer::onMessage(const TcpConnectionPtr& conn, 
						Buffer* buffer,
						Timestamp time)
{
    // 反序列化
	string recv_buf = buffer->retrieveAllAsString();
	ik_Gate::GateRequest request;
	request.ParseFromString(recv_buf);

	// 获取对应的处理器并执行
	auto msg_handler = GateService::instance()->get_handler(request.type());
	string str = request.request_msg();
	msg_handler(conn, str, time);
}