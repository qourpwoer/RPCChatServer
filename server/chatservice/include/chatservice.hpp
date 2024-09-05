#pragma once
#include "ChatService.pb.h"
#include "unordered_map"
#include "redis.hpp"
#include "FriendService.pb.h"
#include "GroupService.pb.h"
#include "OfflineService.pb.h"

class ChatService : public ik_ChatService::ChatServiceRpc
{
public:
    ChatService();
    void OneChat(::google::protobuf::RpcController *controller,
                 const ::ik_ChatService::OneChatRequest *request,
                 google::protobuf::Empty *response,
                 ::google::protobuf::Closure *done);
    void GroupChat(::google::protobuf::RpcController *controller,
                   const ::ik_ChatService::GroupChatRequest *request,
                   google::protobuf::Empty *response,
                   ::google::protobuf::Closure *done);
    int establish_connection(string host);
    ~ChatService();
private:
    unordered_map<std::string, int> channel_map_;
    Redis redis_client_;
    ik_FriendService::FriendServiceRpc_Stub friend_stub_;
    ik_GroupService::GroupServiceRpc_Stub group_stub_;
    ik_OfflineService::OfflineServiceRpc_Stub offline_stub_;
};