#include "groupservice.hpp"
#include "CommonConnectionPool.h"
#include "Logger.h"
#include <memory>
#include <string>
#include <mysql/mysql.h>

void GroupService::AddGroup(google::protobuf::RpcController *controller,
                            const ::ik_GroupService::AddGroupRequest *request,
                            ::ik_GroupService::AddGroupResponse *response,
                            ::google::protobuf::Closure *done)
{
    int userid = request->userid();
    int groupid = request->groupid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values('%d', '%d', '%s')", groupid, userid, "normal");
    if (sp->update(sql))
    {
        response->set_is_success(true);
    }
    else
    {
        response->set_is_success(false);
        response->set_err_msg("Add group Failed!");
    }
    done->Run();
}
void GroupService::CreateGroup(google::protobuf::RpcController *controller,
                               const ::ik_GroupService::CreateGroupRequest *request,
                               ::ik_GroupService::CreateGroupResponse *response,
                               ::google::protobuf::Closure *done)
{
    int userid = request->userid();
    std::string groupname = request->groupname();
    std::string groupdesc = request->groupdesc();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "insert into allgroup(groupname, groupdesc) values('%s', '%s')", groupname.c_str(), groupdesc.c_str());
    if (sp->update(sql))
    {
        int groupid = sp->get_insert_id();
        memset(sql, 0, sizeof(sql));
        sprintf(sql, "insert into groupuser(groupid, userid, grouprole) values('%d', '%d', '%s')", groupid, userid, "creator");
        if (sp->update(sql))
        {
            response->set_is_success(true);
            response->set_id(groupid);
        }
        else
        {
            response->set_is_success(false);
            response->set_err_msg("Add group creator Failed!");
        }
    }
    else
    {
        response->set_is_success(false);
        response->set_err_msg("Create group Failed!");
    }
    done->Run();
}
void GroupService::QuitGroup(google::protobuf::RpcController *controller,
                             const ::ik_GroupService::QuitGroupRequest *request,
                             ::ik_GroupService::QuitGroupResponse *response,
                             ::google::protobuf::Closure *done)
{
    int userid = request->userid();
    int groupid = request->groupid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "delete from groupuser where groupid=%d and userid=%d", groupid, userid);
    if (sp->update(sql))
    {
        response->set_is_success(true);
    }
    else
    {
        response->set_is_success(false);
        response->set_err_msg("QuitGroup Failed!");
    }
    done->Run();
}
void GroupService::GetGroupUsers(google::protobuf::RpcController *controller,
                                 const ::ik_GroupService::GetGroupUsersRequest *request,
                                 ::ik_GroupService::GetGroupUsersResponse *response,
                                 ::google::protobuf::Closure *done)
{
    int groupid = request->groupid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,b.grouprole from user a,groupuser b where a.id=b.userid and b.groupid=%d", groupid);
    MYSQL_RES *res = sp->query(sql);
    if (res)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            int id = atoi(row[0]);
            std::string name = row[1];
            std::string grouprole = row[2];
            ik_GroupService::UserInfo *user = response->add_users();
            user->set_id(id);
            user->set_name(name);
            user->set_grouprole(grouprole);
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
void GroupService::GetGroups(google::protobuf::RpcController *controller,
               const ::ik_GroupService::GetGroupsRequest *request,
               ::ik_GroupService::GetGroupsResponse *response,
               ::google::protobuf::Closure *done)
{
    int userid = request->userid();
    // 获取单例连接池
    ConnectionPool *cp = ConnectionPool::getConnectionPool();
    // 从连接池中获取连接
    std::shared_ptr<Connection> sp = cp->getConnection();
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.groupname,a.groupdesc from Allgroup a, groupuser b where a.id=b.groupid and b.userid=%d", userid);
    MYSQL_RES *res = sp->query(sql);
    if (res)
    {
        MYSQL_ROW row;
        while ((row = mysql_fetch_row(res)) != nullptr)
        {
            int id = atoi(row[0]);
            std::string groupname = row[1];
            std::string groupdesc = row[2];
            ik_GroupService::GroupInfo *group = response->add_groups();
            group->set_id(id);
            group->set_groupname(groupname);
            group->set_groupdesc(groupdesc);
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