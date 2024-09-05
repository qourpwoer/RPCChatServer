#pragma once
#include "UserService.pb.h"
#include "redis.hpp"

class UserService : public ik_UserService::UserServiceRpc
{
public:
    UserService();
    void Login(google::protobuf::RpcController *controller,
               const ik_UserService::LoginRequest *request,
               ik_UserService::LoginResponse *response,
               google::protobuf::Closure *done);
    void Register(google::protobuf::RpcController *controller,
                 const ik_UserService::RegisterRequest *request,
                 ik_UserService::RegisterResponse *response,
                 google::protobuf::Closure *done);
    void LoginOut(google::protobuf::RpcController *controller,
                  const ik_UserService::LoginOutRequest *request,
                  google::protobuf::Empty *response,
                  google::protobuf::Closure *done);          
private:
    Redis redis_client_; //连接redis服务器
};