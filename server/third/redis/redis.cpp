#include "redis.hpp"
#include "mprpcapplication.h"
#include "mprpcconfig.h"

#include <iostream>

//初始化redis连接信息
Redis::Redis()
{
    MprpcConfig configure = MprpcApplication::GetConfig();
    ip_ = configure.Load("redisip");
    port_ = atoi(configure.Load("redisport").c_str());
}

//释放连接
Redis::~Redis()
{
    redisFree(get_channel_);
    redisFree(set_channel_);
}

//初始化get_channel、set_channel连接
bool Redis::connect()
{
    get_channel_ = redisConnect(ip_.c_str(), port_);
    if (get_channel_ == nullptr)
    {
        return false;
    }

    set_channel_ = redisConnect(ip_.c_str(), port_);
    if (set_channel_ == nullptr)
    {
        return false;
    }

    return true;
}

//判断某个账号是否在线
bool Redis::is_exist(int id)
{
    if (get_host(id) == "")
    {
        return false;
    }
    else
    {
        return true;
    }
}

//根据账户得到它上线的服务器
string Redis::get_host(int id)
{
    redisReply *reply = (redisReply *)redisCommand(get_channel_, "get %d", id, sizeof(id));
    if (reply == nullptr)
    {
        return "";
    }
    if(!reply->str)
    {
        return ""; 
    }
    string result = reply->str;
    freeReplyObject(reply);

    return result;
}

//设置账户及其登录的服务器
bool Redis::set_host(int id, string host)
{
    redisReply *reply = (redisReply *)redisCommand(set_channel_, "set %d %s", id, host.c_str());
    if (reply == nullptr)
    {
        return false;
    }
    return true;
}

//删除用户的信息
bool Redis::del_host(int id)
{
    redisReply *reply = (redisReply *)redisCommand(set_channel_, "del %d",id);
    if (reply == nullptr)
    {
        return false;
    }
    return true;
}
