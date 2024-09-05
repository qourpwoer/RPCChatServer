#pragma once
#include "FriendService.pb.h"


class FriendService : public ik_FriendService::FriendServiceRpc
{
public:
    void GetFriendList(google::protobuf::RpcController *controller,
                               const ::ik_FriendService::FriendListRequest *request,
                               ::ik_FriendService::FriendListResponse *response,
                               ::google::protobuf::Closure *done);
    void GetUserInfo(google::protobuf::RpcController *controller,
                             const ::ik_FriendService::UserInfoRequest *request,
                             ::ik_FriendService::UserInfoResponse *response,
                             ::google::protobuf::Closure *done);
    void AddFriend(google::protobuf::RpcController *controller,
                           const ::ik_FriendService::AddFriendRequest *request,
                           ::ik_FriendService::AddFriendResponse *response,
                           ::google::protobuf::Closure *done);
    void DeleteFriend(google::protobuf::RpcController *controller,
                              const ::ik_FriendService::DeleteFriendRequest *request,
                              ::ik_FriendService::DeleteFriendResponse *response,
                              ::google::protobuf::Closure *done);
};