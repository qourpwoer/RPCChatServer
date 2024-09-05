#include "gateservice.hpp"
#include "Logger.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

using namespace placeholders;

GateService::GateService() : user_stub_(new MprpcChannel()),
                             friend_stub_(new MprpcChannel()),
                             group_stub_(new MprpcChannel()),
                             chat_stub_(new MprpcChannel()),
                             offline_stub_(new MprpcChannel())
{
    msg_handler_map_.insert({"Login", bind(&GateService::login, this, _1, _2, _3)});
    msg_handler_map_.insert({"Register", bind(&GateService::regist, this, _1, _2, _3)});
    msg_handler_map_.insert({"LogOut", bind(&GateService::logout, this, _1, _2, _3)});
    msg_handler_map_.insert({"AddFriend", bind(&GateService::add_friend, this, _1, _2, _3)});
    msg_handler_map_.insert({"DelteFriend", bind(&GateService::delete_friend, this, _1, _2, _3)});
    msg_handler_map_.insert({"GetUserInfo", bind(&GateService::get_userInfo, this, _1, _2, _3)});
    msg_handler_map_.insert({"GetFriendList", bind(&GateService::get_friendList, this, _1, _2, _3)});
    msg_handler_map_.insert({"ReadOfflineMsg", bind(&GateService::read_offlineMsg, this, _1, _2, _3)});
    msg_handler_map_.insert({"CreateGroup", bind(&GateService::create_group, this, _1, _2, _3)});
    msg_handler_map_.insert({"QuitGroup", bind(&GateService::quit_group, this, _1, _2, _3)});
    msg_handler_map_.insert({"AddGroup", bind(&GateService::add_group, this, _1, _2, _3)});
    msg_handler_map_.insert({"GetGroups", bind(&GateService::get_groups, this, _1, _2, _3)});
    msg_handler_map_.insert({"GetGroupUsers", bind(&GateService::get_groupUsers, this, _1, _2, _3)});
    msg_handler_map_.insert({"OneChat", bind(&GateService::one_chat, this, _1, _2, _3)});
    msg_handler_map_.insert({"GroupChat", bind(&GateService::group_chat, this, _1, _2, _3)});
    msg_handler_map_.insert({"ConveyChat", bind(&GateService::convey_chat, this, _1, _2, _3)});
    if (!redis_client_.connect())
    {
        LOG_ERROR << "redis is can not be connected!";
    }
}

// 获取请求处理器
MsgHandler GateService::get_handler(const string &msg_type)
{
    auto it = msg_handler_map_.find(msg_type);
    if (it == msg_handler_map_.end())
    {
        LOG_WARN << "msg_type: " << msg_type << "Not Found handler!";
        return [=](const TcpConnectionPtr &conn, string &recv_buf, Timestamp time) {};
    }
    return it->second;
}

// 服务器异常，业务重置
void GateService::reset()
{
    for (auto &v : user_connection_map_)
    {
        if (redis_client_.is_exist(v.first))
        {
            redis_client_.del_host(v.first);
        }
    }
}

void GateService::client_close_exception(const TcpConnectionPtr &conn)
{
    for (auto &v : user_connection_map_)
    {
        if (v.second == conn)
        {
            redis_client_.del_host(v.first);
            user_connection_map_.erase(v.first);
            break;
        }
    }
}
// 登录
void GateService::login(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_UserService::LoginRequest login_request;
    login_request.ParseFromString(recv_buf);

    // 执行
    ik_UserService::LoginResponse login_response;
    MprpcController controller;
    user_stub_.Login(&controller, &login_request, &login_response, nullptr);

    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
        // 序列化并返回结果
        ik_Gate::GateResponse response;
        response.set_type("Error");
        response.set_response_msg("Call Login in UserService error!");
        string send_str = response.SerializeAsString();
        conn->send(send_str);
    }
    else
    {
        // 在redis上记录此用户
        if (login_response.is_success())
        {
            MprpcConfig config = MprpcApplication::GetInstance().GetConfig();
            string ip = config.Load("rpcserverip");
            uint16_t port = atoi(config.Load("rpcserverport").c_str());
            redis_client_.set_host(login_request.id(), ip + ":" + to_string(port));

            // 添加此用户到userMap
            {
                lock_guard<mutex> lock(mutex_);
                user_connection_map_.insert({login_request.id(), conn});
            }
        }
        // 序列化并返回结果
        ik_Gate::GateResponse response;
        response.set_type("Login");
        response.set_response_msg(login_response.SerializeAsString());
        string send_str = response.SerializeAsString();
        conn->send(send_str);
    }
}
// 注册
void GateService::regist(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_UserService::RegisterRequest reg_request;
    reg_request.ParseFromString(recv_buf);

    // 执行
    ik_UserService::RegisterResponse reg_response;
    MprpcController controller;
    user_stub_.Register(&controller, &reg_request, &reg_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
        // 序列化并返回结果
        ik_Gate::GateResponse response;
        response.set_type("Error");
        response.set_response_msg("Call Login in UserService error!");
        string send_str = response.SerializeAsString();
        conn->send(send_str);
    }
    else
    {
        ik_Gate::GateResponse response;
        response.set_type("Register");
        response.set_response_msg(reg_response.SerializeAsString());
        string send_str = response.SerializeAsString();
        conn->send(send_str);
    }
}

// 注销业务
void GateService::logout(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_UserService::LoginOutRequest loginout_request;
    loginout_request.ParseFromString(recv_buf);

    // 执行
    MprpcController controller;
    google::protobuf::Empty empty;
    user_stub_.LoginOut(&controller, &loginout_request, &empty, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    if (user_connection_map_.find(loginout_request.id()) != user_connection_map_.end())
    {
        user_connection_map_.erase(loginout_request.id());
    }
}

// 添加好友业务
void GateService::add_friend(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_FriendService::AddFriendRequest add_request;
    add_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_FriendService::AddFriendResponse add_response;
    friend_stub_.AddFriend(&controller, &add_request, &add_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        ik_Gate::GateResponse response;
        response.set_type("AddFriend");
        response.set_response_msg(add_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 删除好友
void GateService::delete_friend(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_FriendService::DeleteFriendRequest del_request;
    del_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_FriendService::DeleteFriendResponse del_response;
    friend_stub_.DeleteFriend(&controller, &del_request, &del_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        ik_Gate::GateResponse response;
        response.set_type("DeleteFriend");
        response.set_response_msg(del_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 获取用户信息
void GateService::get_userInfo(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_FriendService::UserInfoRequest user_request;
    user_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_FriendService::UserInfoResponse user_response;
    friend_stub_.GetUserInfo(&controller, &user_request, &user_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        ik_Gate::GateResponse response;
        response.set_type("GetUserInfo");
        response.set_response_msg(user_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 获取好友列表
void GateService::get_friendList(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_FriendService::FriendListRequest frilist_request;
    frilist_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_FriendService::FriendListResponse frilist_response;
    friend_stub_.GetFriendList(&controller, &frilist_request, &frilist_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        if (frilist_response.is_success())
        {
            ik_Gate::GateResponse response;
            response.set_type("GetFriendList");
            response.set_response_msg(frilist_response.SerializeAsString());
            string send = response.SerializeAsString();
            conn->send(send);
        }
        else
        {
            LOG_ERROR << "Get Friendlist failed!";
        }
    }
}

// 创建群组
void GateService::create_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_GroupService::CreateGroupRequest cregrp_request;
    cregrp_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_GroupService::CreateGroupResponse cregrp_response;
    group_stub_.CreateGroup(&controller, &cregrp_request, &cregrp_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        if (!cregrp_response.is_success())
        {
            LOG_ERROR << cregrp_response.err_msg();
        }
        ik_Gate::GateResponse response;
        response.set_type("CreateGroup");
        response.set_response_msg(cregrp_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 加入群组
void GateService::add_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_GroupService::AddGroupRequest addgrp_request;
    addgrp_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_GroupService::AddGroupResponse addgrp_response;
    group_stub_.AddGroup(&controller, &addgrp_request, &addgrp_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        if (!addgrp_response.is_success())
        {
            LOG_ERROR << addgrp_response.err_msg();
        }
        ik_Gate::GateResponse response;
        response.set_type("AddGroup");
        response.set_response_msg(addgrp_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 退出群
void GateService::quit_group(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_GroupService::QuitGroupRequest qitgrp_request;
    qitgrp_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_GroupService::QuitGroupResponse qitgrp_response;
    group_stub_.QuitGroup(&controller, &qitgrp_request, &qitgrp_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        if (!qitgrp_response.is_success())
        {
            LOG_ERROR << qitgrp_response.err_msg();
        }
        ik_Gate::GateResponse response;
        response.set_type("QuitGroup");
        response.set_response_msg(qitgrp_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}
void GateService::get_groups(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_GroupService::GetGroupsRequest getgrp_request;
    getgrp_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_GroupService::GetGroupsResponse getgrp_response;
    group_stub_.GetGroups(&controller, &getgrp_request, &getgrp_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << "Call GroupService error!";
    }
    ik_Gate::GateResponse response;
    response.set_type("GetGroups");
    response.set_response_msg(getgrp_response.SerializeAsString());
    string send = response.SerializeAsString();
    conn->send(send);
}

// 获得群用户信息
void GateService::get_groupUsers(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_GroupService::GetGroupUsersRequest getgrpuser_request;
    getgrpuser_request.ParseFromString(recv_buf);
    // 执行
    MprpcController controller;
    ik_GroupService::GetGroupUsersResponse getgrpuser_response;
    group_stub_.GetGroupUsers(&controller, &getgrpuser_request, &getgrpuser_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << controller.ErrorText();
    }
    else
    {
        if (!getgrpuser_response.is_success())
        {
            LOG_ERROR << "GetGroupUsers error!";
        }
        ik_Gate::GateResponse response;
        response.set_type("GetGroupUsers");
        response.set_response_msg(getgrpuser_response.SerializeAsString());
        string send = response.SerializeAsString();
        conn->send(send);
    }
}

// 读取离线消息
void GateService::read_offlineMsg(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    ik_OfflineService::ReadOfflineRequest readoff_request;
    readoff_request.ParseFromString(recv_buf);
    ik_OfflineService::ReadOfflineResponse readoff_response;
    MprpcController controller;
    offline_stub_.ReadOffline(&controller, &readoff_request, &readoff_response, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << "Call OfflineService error!";
    }
    ik_Gate::GateResponse response;
    response.set_type("ReadOfflineMsg");
    response.set_response_msg(readoff_response.SerializeAsString());
    string send = response.SerializeAsString();
    conn->send(send);
}

// 一对一聊天业务
void GateService::one_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    // 反序列化
    ik_ChatService::OneChatRequest onechat_request;
    onechat_request.ParseFromString(recv_buf);
    int toid = onechat_request.toid();
    if (user_connection_map_.find(toid) != user_connection_map_.end())
    {
        ik_FriendService::UserInfoRequest user_request;
        user_request.set_id(onechat_request.id());
        ik_FriendService::UserInfoResponse user_response;
        MprpcController controller;
        friend_stub_.GetUserInfo(&controller, &user_request, &user_response, nullptr);
        ik_ChatService::ChatMsg chat_msg;
        chat_msg.set_id(onechat_request.id());
        chat_msg.set_toid(onechat_request.toid());
        chat_msg.set_is_group(false);
        chat_msg.set_msg(onechat_request.msg());
        if (controller.Failed())
        {
            LOG_ERROR << "Call FriendService error!";
        }
        else
        {
            chat_msg.set_username(user_response.name());
        }
        ik_Gate::GateResponse response;
        response.set_type("ConveyChat");
        response.set_response_msg(chat_msg.SerializeAsString());
        string send = response.SerializeAsString();
        user_connection_map_[toid]->send(send);
    }
    else
    {
        MprpcController controller;
        google::protobuf::Empty empty;
        chat_stub_.OneChat(&controller, &onechat_request, &empty, nullptr);
        if (controller.Failed())
        {
            LOG_ERROR << "OneChat Error!";
        }
    }
}

// 群聊
void GateService::group_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    ik_ChatService::GroupChatRequest grpchat_request;
    grpchat_request.ParseFromString(recv_buf);
    MprpcController controller;
    google::protobuf::Empty empty;
    chat_stub_.GroupChat(&controller, &grpchat_request, &empty, nullptr);
    if (controller.Failed())
    {
        LOG_ERROR << "GroupChat Error!";
    }
}

// 转发聊天信息
void GateService::convey_chat(const TcpConnectionPtr &conn, string &recv_buf, Timestamp time)
{
    ik_ChatService::ChatMsg chat_msg;
    chat_msg.ParseFromString(recv_buf);
    int toid = chat_msg.toid();
    if (user_connection_map_.find(toid) != user_connection_map_.end())
    {
        // 仍在线，直接转发
        ik_Gate::GateRequest request;
        request.set_type("ConveyChat");
        request.set_request_msg(chat_msg.SerializeAsString());
        user_connection_map_[toid]->send(request.SerializeAsString());
    }
    else
    {
        // 不在线，写入离线消息
        ik_OfflineService::WriteOfflineRequest writeoff_request;
        writeoff_request.set_friendid(toid);
        writeoff_request.set_msg(chat_msg.SerializeAsString());
        MprpcController controller;
        google::protobuf::Empty empty;
        offline_stub_.WriteOffline(&controller, &writeoff_request, &empty, nullptr);
        if (controller.Failed())
        {
            LOG_ERROR << "Call OfflineService error!";
        }
    }
}
