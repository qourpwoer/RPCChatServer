#include "chatservice.hpp"
#include "redis.hpp"
#include "Logger.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "GateServer.pb.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

ChatService::ChatService() : friend_stub_(new MprpcChannel()), group_stub_(new MprpcChannel()), offline_stub_(new MprpcChannel())
{
    if (!redis_client_.connect())
    {
        LOG_ERROR << "redis is can not be connected!";
    }
}

//关闭其中的套接字描述符
ChatService::~ChatService()
{
    for (auto &v : channel_map_)
    {
        close(v.second);
    }
}

// 根据host建立连接
int ChatService::establish_connection(string host)
{
    // 解析ip和port
    int index = host.find(":");
    string ip = host.substr(0, index);
    int port = atoi(host.substr(index + 1, host.size() - index).c_str());

    // 创建socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        // 打印错误日志
        LOG_ERROR << "Create socket fd error!";

        close(client_fd);
        return -1;
    }

    // 配置服务器信息
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        // 打印错误日志
        LOG_ERROR << "Establish connection error!";

        close(client_fd);
        return -1;
    }
    return client_fd;
}

void ChatService::OneChat(::google::protobuf::RpcController *controller,
                          const ::ik_ChatService::OneChatRequest *request,
                          google::protobuf::Empty *response,
                          ::google::protobuf::Closure *done)
{
    int id = request->id();
    int toid = request->toid();
    string time = request->time();
    std::string msg = request->msg();

    ik_ChatService::ChatMsg chat_msg;
    ik_FriendService::UserInfoRequest user_request;
    user_request.set_id(id);
    ik_FriendService::UserInfoResponse user_response;
    MprpcController mprpccontroller;
    friend_stub_.GetUserInfo(&mprpccontroller, &user_request, &user_response, nullptr);
    if (mprpccontroller.Failed())
    {
        LOG_ERROR << "Call FriendService error!";
    }
    chat_msg.set_id(id);
    chat_msg.set_toid(toid);
    chat_msg.set_is_group(false);
    chat_msg.set_msg(msg);
    chat_msg.set_username(user_response.name());
    chat_msg.set_time(time);

    if (redis_client_.is_exist(toid))
    {
        std::string host = redis_client_.get_host(toid);
        // 在线，找到gate转发
        int connect_fd = -1;
        if (channel_map_.find(host) != channel_map_.end())
        {
            connect_fd = channel_map_[host];
        }
        else
        {
            connect_fd = establish_connection(host);
            channel_map_.insert({host, connect_fd});
        }

        ik_Gate::GateRequest request;
        request.set_type("ConveyChat");
        request.set_request_msg(chat_msg.SerializeAsString());
        string send_str = request.SerializeAsString();
        if (-1 == send(connect_fd, send_str.c_str(), strlen(send_str.c_str()), 0))
        {
            if (channel_map_.find(host) != channel_map_.end())
            {
                channel_map_.erase(host);
            }
            LOG_ERROR << "Send error!";
        }
    }
    else
    {
        // 不在线，写入离线消息
        ik_OfflineService::WriteOfflineRequest writeoff_request;
        writeoff_request.set_friendid(toid);
        writeoff_request.set_msg(chat_msg.SerializeAsString());
        MprpcController mprpccontroller;
        google::protobuf::Empty empty;
        offline_stub_.WriteOffline(&mprpccontroller, &writeoff_request, &empty, nullptr);
        if (mprpccontroller.Failed())
        {
            LOG_ERROR << "Call OfflineService error!";
        }
    }
    done->Run();
}
void ChatService::GroupChat(::google::protobuf::RpcController *controller,
                            const ::ik_ChatService::GroupChatRequest *request,
                            google::protobuf::Empty *response,
                            ::google::protobuf::Closure *done)
{
    int groupid = request->groupid();
    int userid = request->userid();
    string time = request->time();
    std::string msg = request->msg();
    ik_GroupService::GetGroupUsersRequest grpuser_request;
    grpuser_request.set_groupid(groupid);
    MprpcController mprpccontroller;
    ik_GroupService::GetGroupUsersResponse grpuser_response;
    group_stub_.GetGroupUsers(&mprpccontroller, &grpuser_request, &grpuser_response, nullptr);
    if (mprpccontroller.Failed())
    {
        LOG_ERROR << "Call GetGroupUser error!";
    }
    for (int i = 0; i < grpuser_response.users_size(); ++i)
    {
        ik_GroupService::UserInfo user = grpuser_response.users(i);
        int toid = user.id();
        string name = user.name();
        ik_ChatService::ChatMsg chat_msg;
        chat_msg.set_id(userid);
        chat_msg.set_toid(toid);
        chat_msg.set_username(name);
        chat_msg.set_is_group(true);
        chat_msg.set_groupid(groupid);
        chat_msg.set_msg(msg);
        chat_msg.set_time(time);
        if (redis_client_.is_exist(toid))
        {
            // 在线，找到gate转发
            string host = redis_client_.get_host(toid);
            int connect_fd = -1;
            if (channel_map_.find(host) != channel_map_.end())
            {
                connect_fd = channel_map_[host];
            }
            else
            {
                connect_fd = establish_connection(host);
                channel_map_.insert({host, connect_fd});
            }
            ik_Gate::GateRequest request;
            request.set_type("ConveyChat");
            request.set_request_msg(chat_msg.SerializeAsString());
            string send_str = request.SerializeAsString();
            if (-1 == send(connect_fd, send_str.c_str(), strlen(send_str.c_str()), 0))
            {
                if (channel_map_.find(host) != channel_map_.end())
                {
                    channel_map_.erase(host);
                }
                LOG_ERROR << "Send error!";
            }
        }
        else
        {
            // 不在线，写入离线消息
            ik_OfflineService::WriteOfflineRequest writeoff_request;
            writeoff_request.set_friendid(toid);
            writeoff_request.set_msg(chat_msg.SerializeAsString());
            MprpcController mprpccontroller;
            google::protobuf::Empty empty;
            offline_stub_.WriteOffline(&mprpccontroller, &writeoff_request, &empty, nullptr);
            if (mprpccontroller.Failed())
            {
                LOG_ERROR << "Call OfflineService error!";
            }
        }
    }
    done->Run();
}
