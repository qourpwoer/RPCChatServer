#include "userservice.hpp"
#include "CommonConnectionPool.h"
#include "Logger.h"
#include <memory>
#include <string>
#include <mysql/mysql.h>

UserService::UserService()
{
    if (!redis_client_.connect())
    {
        LOG_ERROR << "redis is can not be connected!";
    }
}

void UserService::Login(google::protobuf::RpcController *controller,
                        const ik_UserService::LoginRequest *request,
                        ik_UserService::LoginResponse *response,
                        google::protobuf::Closure *done)
{
    // 框架给业务上报了请求参数LoginRequest, 应用获取相应数据做本地业务
    uint32_t req_id = request->id();
    std::string req_pwd = request->password();

    // 判断是否已经登录
    if (redis_client_.is_exist(req_id))
    {
        response->set_is_success(false);
        response->set_err_msg("This account is logined!");
        done->Run();
        return;
    }

    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id=%d", req_id);
    MYSQL_RES *res = sp->query(sql);
    if (!res)
    {
        response->set_is_success(false);
        response->set_err_msg("No return from mysql!");
    }
    else
    {

        MYSQL_ROW row = mysql_fetch_row(res);
        if (row)
        {
            uint32_t id = atoi(row[0]);
            std::string name = row[1];
            std::string pwd = row[2];
            if (id == req_id && pwd == req_pwd)
            {
                response->set_is_success(true);
                response->set_name(name);
                response->set_err_msg("");
                response->set_id(id);
            }
            else
            {
                response->set_is_success(false);
                response->set_err_msg("id or name is wrong");
            }
        }
        else
        {
            response->set_is_success(false);
            response->set_err_msg("id or name is wrong");
        }
    }
    // 执行回调操作  执行响应对象数据的序列化和网络发送
    done->Run();
}
void UserService::Register(google::protobuf::RpcController *controller,
                           const ik_UserService::RegisterRequest *request,
                           ik_UserService::RegisterResponse *response,
                           google::protobuf::Closure *done)
{
    string name = request->name();
    string pwd = request->password();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password) values('%s', '%s')", name.c_str(), pwd.c_str());
    if (sp->update(sql))
    {
        response->set_id(sp->get_insert_id());
        response->set_is_success(true);
    }
    else
    {
        response->set_id(-1);
        response->set_is_success(false);
    }
    done->Run();
}
void UserService::LoginOut(google::protobuf::RpcController *controller,
                           const ik_UserService::LoginOutRequest *request,
                           google::protobuf::Empty *response,
                           google::protobuf::Closure *done)
{
    int id = request->id();
    if (redis_client_.is_exist(id))
    {
        redis_client_.del_host(id);
    }
    done->Run();
}
