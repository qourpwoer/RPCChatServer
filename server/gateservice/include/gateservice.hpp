#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include <mutex>

#include "TcpConnection.h"
#include "zookeeperutil.h"
#include "GateServer.pb.h"
#include "redis.hpp"

#include "UserService.pb.h"
#include "FriendService.pb.h"
#include "GroupService.pb.h"
#include "ChatService.pb.h"
#include "OfflineService.pb.h"

using namespace std;

using MsgHandler = function<void(const TcpConnectionPtr &conn, string &recv_str, Timestamp time)>;
// 网关服务器业务类
class GateService
{
public: // 获取单例对象的接口函数
	static GateService *instance()
	{
		static GateService service;
		return &service;
	}

public:
	// 获取请求处理器
	MsgHandler get_handler(const string &msg_type);
	// 用户断开连接异常处理
	void client_close_exception(const TcpConnectionPtr &conn);
	// 服务器异常，业务重置
	void reset();
	// 登录
	void login(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 注册
	void regist(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 注销业务
	void logout(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 添加好友业务
	void add_friend(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 删除好友
	void delete_friend(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 获取用户信息
	void get_userInfo(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 获取好友列表
	void get_friendList(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 读取离线消息
	void read_offlineMsg(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 创建 群组
	void create_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 加入群组
	void add_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 退出群
	void quit_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 获取群组信息
	void get_groups(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 获得群用户信息
	void get_groupUsers(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 一对一聊天
	void one_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 群聊
	void group_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

	// 转发聊天消息
	void convey_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time);

private:
	GateService();
	GateService(const GateService &) = delete;
	GateService &operator=(GateService &) = delete;

	mutex mutex_;
	unordered_map<string, MsgHandler> msg_handler_map_;		   // 存储事件及事件对应的回调函数
	unordered_map<int, TcpConnectionPtr> user_connection_map_; // 存储登录用户，及对应tcpptr信息
	Redis redis_client_;									   // 连接redis服务器

	ik_UserService::UserServiceRpc_Stub user_stub_;
	ik_FriendService::FriendServiceRpc_Stub friend_stub_;
	ik_GroupService::GroupServiceRpc_Stub group_stub_;
	ik_ChatService::ChatServiceRpc_Stub chat_stub_;
	ik_OfflineService::OfflineServiceRpc_Stub offline_stub_;
};
