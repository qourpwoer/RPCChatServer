#pragma once
#include "GroupService.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "redis.hpp"

class GroupService : public ik_GroupService::GroupServiceRpc
{
    void AddGroup(google::protobuf::RpcController *controller,
                  const ::ik_GroupService::AddGroupRequest *request,
                  ::ik_GroupService::AddGroupResponse *response,
                  ::google::protobuf::Closure *done);
    void CreateGroup(google::protobuf::RpcController *controller,
                     const ::ik_GroupService::CreateGroupRequest *request,
                     ::ik_GroupService::CreateGroupResponse *response,
                     ::google::protobuf::Closure *done);
    void QuitGroup(google::protobuf::RpcController *controller,
                   const ::ik_GroupService::QuitGroupRequest *request,
                   ::ik_GroupService::QuitGroupResponse *response,
                   ::google::protobuf::Closure *done);
    void GetGroupUsers(google::protobuf::RpcController *controller,
                       const ::ik_GroupService::GetGroupUsersRequest *request,
                       ::ik_GroupService::GetGroupUsersResponse *response,
                       ::google::protobuf::Closure *done);
    void GetGroups(google::protobuf::RpcController *controller,
                   const ::ik_GroupService::GetGroupsRequest *request,
                   ::ik_GroupService::GetGroupsResponse *response,
                   ::google::protobuf::Closure *done);
};