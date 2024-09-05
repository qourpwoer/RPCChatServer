#include "friendservice.hpp"
#include "Logger.h"
#include "CommonConnectionPool.h"

void FriendService::GetFriendList(google::protobuf::RpcController *controller,
                                  const ::ik_FriendService::FriendListRequest *request,
                                  ::ik_FriendService::FriendListResponse *response,
                                  ::google::protobuf::Closure *done)
{
    int id = request->id();
    char sql[1024] = {0};
    sprintf(sql, "select a.id, a.name from user a inner join friend b on b.friendid = a.id where b.userid=%d", id);
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    MYSQL_RES *res = sp->query(sql);
    if (res)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            int id = atoi(row[0]);
            std::string name = row[1];
            ik_FriendService::FriendInfo *user = response->add_friends();
            user->set_id(id);
            user->set_name(name);
        }
        response->set_is_success(true);
    }
    else
    {
        response->set_is_success(false);
        LOG_ERROR << "No return from mysql!";
    }
    done->Run();
}
void FriendService::GetUserInfo(google::protobuf::RpcController *controller,
                                const ::ik_FriendService::UserInfoRequest *request,
                                ::ik_FriendService::UserInfoResponse *response,
                                ::google::protobuf::Closure *done)
{
    int id = request->id();
    char sql[1024] = {0};
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    sprintf(sql, "select * from user where id=%d", id);
    MYSQL_RES *res = sp->query(sql);
    if (res)
    {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (row)
        {
            response->set_id(id);
            string name = row[1];
            response->set_name(name);
        }
        else
        {
            response->set_id(-1);
            response->set_name("查找的id不存在!");
        }
    }
    else
    {
        LOG_ERROR << "No return from mysql!";
    }
    done->Run();
}
void FriendService::AddFriend(google::protobuf::RpcController *controller,
                              const ::ik_FriendService::AddFriendRequest *request,
                              ::ik_FriendService::AddFriendResponse *response,
                              ::google::protobuf::Closure *done)
{
    int myid = request->myid();
    int friendid = request->friendid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into friend(userid, friendid) values('%d', '%d')", myid, friendid);
    if (sp->update(sql))
    {
        response->set_is_success(true);
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "select name from user where id=%d", friendid);
        MYSQL_RES *res = sp->query(sql);
        MYSQL_ROW row = mysql_fetch_row(res);
        std::string name = row[0];
        response->set_name(name);
    }
    else
    {
        response->set_is_success(false);
        response->set_name("添加失败,请确认输入的id是否存在或已经是您的好友!");
    }
    done->Run();
}
void FriendService::DeleteFriend(google::protobuf::RpcController *controller,
                                 const ::ik_FriendService::DeleteFriendRequest *request,
                                 ::ik_FriendService::DeleteFriendResponse *response,
                                 ::google::protobuf::Closure *done)
{
    int myid = request->myid();
    int friendid = request->friendid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "delete from friend where userid=%d and friendid=%d", myid, friendid);
    if (sp->update(sql))
    {
        response->set_is_success(true);
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "select name from user where id=%d", friendid);
        MYSQL_RES *res = sp->query(sql);
        MYSQL_ROW row = mysql_fetch_row(res);
        std::string name = row[0];
        response->set_name(name);
    }
    else
    {
        response->set_is_success(false);
        response->set_name("删除失败,该id可能并不是您的好友或不存在!");
    }
    done->Run();
}