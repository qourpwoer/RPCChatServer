#include "gateserver.hpp"
#include "gateservice.hpp"
#include "mprpcapplication.h"
#include "mprpcconfig.h"

#include <iostream>
#include <signal.h>
using namespace std;

//处理服务器ctrl+c结束后，重置user的状态信息
void resetHandler(int)
{
	GateService::instance()->reset();
	exit(0);
}
int main(int argc, char **argv)
{
	MprpcApplication::Init(argc, argv);
	MprpcConfig config = MprpcApplication::GetInstance().GetConfig();
	string ip = config.Load("rpcserverip");
	uint16_t port = atoi(config.Load("rpcserverport").c_str());
	cout << ip << ":" << port << endl;
	signal(SIGINT, resetHandler);

	EventLoop loop;
	InetAddress addr(port, ip.c_str());
	GateServer server(&loop, addr, "GateServer");

	server.start();
	loop.loop();
	return 0;
}