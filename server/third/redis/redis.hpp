#ifndef REDIS_H
#define REDIS_H

#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    //初始化redis连接信息
    Redis();

    //释放连接
    ~Redis();

    //初始化get_channel、set_channel连接
    bool connect();

    //判断某个账号是否在线
    bool is_exist(int id);

    //根据账户得到它上线的服务器
    string get_host(int id);

    //设置账户及其登录的服务器
    bool set_host(int id, string host);

    //删除用户的信息
    bool del_host(int id);
private:
    redisContext *get_channel_;
    redisContext *set_channel_;

    string ip_;
    int port_;
   
};
#endif
