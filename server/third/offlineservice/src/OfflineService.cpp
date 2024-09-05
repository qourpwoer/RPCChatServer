#include "OfflineService.hpp"
#include "CommonConnectionPool.h"
#include "Logger.h"


void DeleteOffline(int id)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", id);
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    if (!sp->update(sql))
    {
        LOG_ERROR << "Mysql error!";
    }
}

void OfflineService::WriteOffline(google::protobuf::RpcController *controller,
                                  const ::ik_OfflineService::WriteOfflineRequest *request,
                                  google::protobuf::Empty *response,
                                  ::google::protobuf::Closure *done)
{
    int id = request->friendid();
    std::string msg = request->msg();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values('%d', '%s')", id, msg.c_str());
    if (!sp->update(sql))
    {
        LOG_ERROR << "mysql error!";
    }
    done->Run();
}
void OfflineService::ReadOffline(google::protobuf::RpcController *controller,
                                 const ::ik_OfflineService::ReadOfflineRequest *request,
                                 ::ik_OfflineService::ReadOfflineResponse *response,
                                 ::google::protobuf::Closure *done)
{
    int id = request->userid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid=%d", id);
    MYSQL_RES *res = sp->query(sql);
    if (!res)
    {
        LOG_ERROR << "Mysql error!";
    }
    else
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            auto chat_msg = response->add_chat_msgs();
            *chat_msg = row[0];
        }
    }
    DeleteOffline(id);
    done->Run();
}