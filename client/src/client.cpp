#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_map>
#include <functional>
using namespace std;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>

#include "UserService.pb.h"
#include "GateServer.pb.h"
#include "FriendService.pb.h"
#include "GroupService.pb.h"
#include "ChatService.pb.h"
#include "OfflineService.pb.h"

class User
{
public:
    User(int id = -1, string name = "", string pwd = "", string state = "offline") : id(id), name(name), password(pwd), state(state) {}

    void setId(int id) { this->id = id; };
    void setName(string name) { this->name = name; };
    void setPwd(string pwd) { this->password = pwd; };
    void setState(string state) { this->state = state; };

    int getId() { return this->id; };
    string getName() { return this->name; };
    string getPwd() { return this->password; };
    string getState() { return this->state; };

protected:
    int id;
    string name;
    string password;
    string state;
};

// 记录当前系统登录的用户信息
User g_currentUser;

// 控制主菜单页面程序
bool isMainMenuRunning = false;

// 用于读写线程之间的通信
sem_t rwsem;
// 记录登录状态
atomic_bool g_isLoginSucess{false};

// 接收线程
void readTaskHandler(int clientfd);
// 获取系统时间(聊天信息需要添加时间信息)
string getCurrentTime();
// 主聊天页面程序
void mainMenu(int);
// 显示当前登录成功用户的基本信息
void showCurrentUserData();

// 聊天客户端程序实现，main线程用作发送线程，子线程用作接收线程
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建client端的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == clientfd)
    {
        cerr << "socket create error!" << endl;
        exit(-1);
    }

    // 填写client需要连接的server信息ip+port;
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // client和server进行连接
    if (-1 == connect(clientfd, (sockaddr *)&server, sizeof(sockaddr_in)))
    {
        cerr << "connect server error" << endl;
        close(clientfd);
        exit(-1);
    }
    // 初始化读写线程通信用的信号量
    sem_init(&rwsem, 0, 0);

    // 连接服务器成功， 启动接收子线程
    std::thread readTask(readTaskHandler, clientfd);
    readTask.detach();

    // main线程用于接收用户输入，负责发送数据
    for (;;)
    {
        // 显示首页面菜单 登录、注册、退出
        cout << "========================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "========================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;
        cin.get(); // 读掉缓冲区残留的回车

        switch (choice)
        {
        case 1: // login业务
        {
            int id = 0;
            char pwd[50] = {0};
            cout << "userid:";
            cin >> id;
            cin.get(); // 读掉缓冲区残留的回车
            cout << "userpassword:";
            cin.getline(pwd, 50); // 该函数不会因为读到空格而结束，读到换行才结束

            ik_UserService::LoginRequest login_req;
            login_req.set_id(id);
            login_req.set_password(pwd);

            ik_Gate::GateRequest request;
            request.set_type("Login");
            request.set_request_msg(login_req.SerializePartialAsString());
            string req = request.SerializeAsString();
            g_isLoginSucess = false;

            int len = send(clientfd, req.c_str(), strlen(req.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "send login msg error:" << req << endl;
            }

            sem_wait(&rwsem); // 等待信号量，由子线程处理完登录的响应消息后，通知这里
            if (g_isLoginSucess)
            {
                // 进入聊天主菜单页面
                isMainMenuRunning = true;
                mainMenu(clientfd);
            }
        }
        break;
        case 2: // register业务
        {
            char name[50] = {0};
            char pwd[50] = {0};
            cout << "username:";
            cin.getline(name, 50);
            cout << "userpassword:";
            cin.getline(pwd, 50);

            ik_UserService::RegisterRequest reg_req;
            reg_req.set_name(name);
            reg_req.set_password(pwd);
            ik_Gate::GateRequest request;
            request.set_type("Register");
            request.set_request_msg(reg_req.SerializeAsString());
            string req = request.SerializeAsString();
            int len = send(clientfd, req.c_str(), strlen(req.c_str()) + 1, 0);
            if (len == -1)
            {
                cerr << "send reg msg error:" << req << endl;
            }

            sem_wait(&rwsem); // 等待信号量，子线程处理完消息会通知
        }
        break;
        case 3: // quit业务
            close(clientfd);
            sem_destroy(&rwsem);
            exit(0);
        default:
            cerr << "invalid input!" << endl;
            break;
        }
    }

    return 0;
}

// 处理注册的相应逻辑
void doRegResponse(ik_UserService::RegisterResponse &response)
{
    if (false == response.is_success())
    {
        cerr << "name is already exist, register error!" << endl;
    }
    else
    {
        cout << "name register success, userid is " << response.id()
             << ", do not forget it!" << endl;
    }
}

// 处理登录的响应逻辑
void doLoginResponse(ik_UserService::LoginResponse &response, int clientfd)
{
    if (false == response.is_success())
    {
        cerr << response.err_msg() << endl;
        g_isLoginSucess = false;
        return;
    }
    else
    {
        g_currentUser.setId(response.id());
        g_currentUser.setName(response.name());
        showCurrentUserData();
        // 获取当前用户的离线消息 个人聊天消息或者群组消息
        ik_OfflineService::ReadOfflineRequest readoff_request;
        readoff_request.set_userid(g_currentUser.getId());
        ik_Gate::GateRequest request;
        request.set_type("ReadOfflineMsg");
        request.set_request_msg(readoff_request.SerializeAsString());
        string buffer = request.SerializeAsString();
        int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
        if (-1 == len)
        {
            cerr << "send offlinemsg msg error ->" << buffer << endl;
        }
    }

    g_isLoginSucess = true;
}
// 子线程 - 接收线程
void readTaskHandler(int clientfd)
{
    for (;;)
    {
        char buffer[1024] = {0};
        int len = recv(clientfd, buffer, 1024, 0);
        if (-1 == len || 0 == len)
        {
            close(clientfd);
            exit(-1);
        }

        ik_Gate::GateResponse response;
        response.ParseFromString(buffer);
        string type = response.type();
        if (type == "Login")
        {
            ik_UserService::LoginResponse login_res;
            login_res.ParseFromString(response.response_msg());
            doLoginResponse(login_res, clientfd);
            sem_post(&rwsem);
            continue;
        }
        if (type == "Register")
        {
            ik_UserService::RegisterResponse register_res;
            register_res.ParseFromString(response.response_msg());
            doRegResponse(register_res);
            sem_post(&rwsem);
            continue;
        }
        if (type == "AddFriend")
        {
            ik_FriendService::AddFriendResponse add_res;
            add_res.ParseFromString(response.response_msg());
            if (add_res.is_success())
            {
                cout << "添加好友成功！新的好友名字:" << add_res.name() << endl;
            }
            else
            {
                // 添加失败,name字段作为理由出现
                cout << add_res.name() << endl;
            }
            continue;
        }
        if (type == "DeleteFriend")
        {
            ik_FriendService::DeleteFriendResponse del_res;
            del_res.ParseFromString(response.response_msg());
            if (del_res.is_success())
            {
                cout << "删除好友成功！好友名字:" << del_res.name() << endl;
            }
            else
            {
                // 添加失败,name字段作为理由出现
                cout << del_res.name() << endl;
            }
            continue;
        }
        if (type == "GetUserInfo")
        {
            ik_FriendService::UserInfoResponse user_res;
            user_res.ParseFromString(response.response_msg());
            if (user_res.id() != -1)
            {
                cout << "查询用户,用户id:" << user_res.id() << " 名字:" << user_res.name() << endl;
            }
            else
            {
                cout << "该用户不存在!" << endl;
            }
            continue;
        }
        if (type == "GetFriendList")
        {
            ik_FriendService::FriendListResponse frilist_res;
            frilist_res.ParseFromString(response.response_msg());
            cout << "----------------------friend list---------------------" << endl;
            int size = frilist_res.friends_size();
            for (int i = 0; i < size; ++i)
            {
                ik_FriendService::FriendInfo fri = frilist_res.friends(i);
                int id = fri.id();
                string name = fri.name();
                cout << "id:" << id << ", "
                     << "name:" << name << endl;
            }
            continue;
        }
        if (type == "GetGroups")
        {
            ik_GroupService::GetGroupsResponse getgrp_res;
            getgrp_res.ParseFromString(response.response_msg());
            cout << "----------------------group list----------------------" << endl;
            int size = getgrp_res.groups_size();
            for (int i = 0; i < size; ++i)
            {
                ik_GroupService::GroupInfo grp = getgrp_res.groups(i);
                cout << grp.id() << " " << grp.groupname() << " " << grp.groupdesc() << endl;
            }
            continue;
        }
        if (type == "CreateGroup")
        {
            ik_GroupService::CreateGroupResponse group_res;
            group_res.ParseFromString(response.response_msg());
            if (group_res.is_success())
            {
                cout << "Create group success! group id:" << group_res.id() << endl;
            }
            else
            {
                cout << "Create group failed!" << endl;
            }
            continue;
        }
        if (type == "AddGroup")
        {
            ik_GroupService::AddGroupResponse addgrp_res;
            addgrp_res.ParseFromString(response.response_msg());
            if (addgrp_res.is_success())
            {
                cout << "Add group success!" << endl;
            }
            else
            {
                cout << "Add group failed!" << endl;
            }
            continue;
        }
        if (type == "QuitGroup")
        {
            ik_GroupService::QuitGroupResponse quitgrp_res;
            quitgrp_res.ParseFromString(response.response_msg());
            if (quitgrp_res.is_success())
            {
                cout << "Quit group success!" << endl;
            }
            else
            {
                cout << "Quit group failed!" << endl;
            }
            continue;
        }
        if (type == "GetGroupUsers")
        {
            ik_GroupService::GetGroupUsersResponse getgrpusers_res;
            getgrpusers_res.ParseFromString(response.response_msg());
            cout << "----------------------group users list----------------------" << endl;
            int size = getgrpusers_res.users_size();
            for (int i = 0; i < size; ++i)
            {
                ik_GroupService::UserInfo user = getgrpusers_res.users(i);
                cout << user.id() << " " << user.name() << " " << user.grouprole() << endl;
            }
            continue;
        }
        if (type == "ConveyChat")
        {
            ik_ChatService::ChatMsg chat_msg;
            chat_msg.ParseFromString(response.response_msg());
            if (chat_msg.is_group())
            {
                cout << "群消息[" << chat_msg.id() << "]" << chat_msg.time() << "[" << chat_msg.id() << "]" << chat_msg.username()
                     << " said: " << chat_msg.msg() << endl;
            }
            else
            {
                cout << chat_msg.time() << " [" << chat_msg.id() << "]" << chat_msg.username()
                     << " said: " << chat_msg.msg() << endl;
            }
            continue;
        }
        if (type == "ReadOfflineMsg")
        {
            ik_OfflineService::ReadOfflineResponse readoff_response;
            readoff_response.ParseFromString(response.response_msg());
            int size = readoff_response.chat_msgs_size();
            for (int i = 0; i < size; ++i)
            {
                ik_ChatService::ChatMsg chat_msg;
                chat_msg.ParseFromString(readoff_response.chat_msgs(i));
                if (chat_msg.is_group())
                {
                    cout << "群消息[" << chat_msg.id() << "]" << chat_msg.time() << "[" << chat_msg.id() << "]" << chat_msg.username()
                         << " said: " << chat_msg.msg() << endl;
                }
                else
                {
                    cout << chat_msg.time() << " [" << chat_msg.id() << "]" << chat_msg.username()
                         << " said: " << chat_msg.msg() << endl;
                }
            }
            continue;
        }
    }
}
// 显示当前登录成功用户的基本信息
void showCurrentUserData()
{
    cout << "======================login user======================" << endl;
    cout << "current login user => id:" << g_currentUser.getId() << " name:" << g_currentUser.getName() << endl;
    cout << "======================================================" << endl;
}

// "help" command hanler
void help(int fd = 0, string str = "");
// "chat" command handler
void chat(int, string);
// "addfriend" command handler;
void addfriend(int, string);
// "deletefriend" command handler;
void deletefriend(int, string);
// "userinfo" command handler;
void userinfo(int, string);
// "addgroup" command handler;
void addgroup(int, string);
// "groupchat" command handler;
void groupchat(int, string);
// "creategroup" command handler
void creategroup(int, string);
// "loginout" command handler
void loginout(int, string);
// "grouplist" command handler
void grouplist(int, string);
// "friendlist" command handler
void friendlist(int, string);
// "groupusers" command handler
void groupusers(int, string);
// "quitgroup" command handler
void quitgroup(int, string);
// 系统支持的客户端命令列表
unordered_map<string, string> commandMap = {
    {"help", "显示所有支持的命令,格式help"},
    {"chat", "一对一聊天,格式chat:friendid:message"},
    {"addfriend", "添加好友,格式addfriendid:friendid"},
    {"creategroup", "创建群组,格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组,格式addgroup:groupid"},
    {"groupchat", "群聊,格式groupchat:groupid:message"},
    {"loginout", "注销登录,格式loginout"},
    {"grouplist", "查看已经加入的群组列表,格式grouplist"},
    {"groupusers", "查看群组成员, 格式groupuser:groupid"},
    {"quitgroup", "退出群组,格式quitgroup:groupid"},
    {"friendlist", "查看好友列表,格式friendlist"},
    {"deletefriend", "删除指定好友,格式deletefriend:friendid"},
    {"userinfo", "查看指定用户id的信息,格式userinfo:id"}};

// 注册系统支持的客户端命令处理
unordered_map<string, function<void(int, string)>> commandHandlerMap = {
    {"help", help},
    {"chat", chat},
    {"addfriend", addfriend},
    {"creategroup", creategroup},
    {"addgroup", addgroup},
    {"groupchat", groupchat},
    {"loginout", loginout},
    {"grouplist", grouplist},
    {"friendlist", friendlist},
    {"deletefriend", deletefriend},
    {"userinfo", userinfo},
    {"groupusers", groupusers},
    {"quitgroup", quitgroup}};

// 主聊天页面程序
void mainMenu(int clientfd)
{
    help();

    char buffer[1024] = {0};
    while (isMainMenuRunning)
    {
        cin.getline(buffer, 1024);
        string commandbuf(buffer);
        string command; // 存储命令
        int idx = commandbuf.find(":");
        if (-1 == idx)
        {
            command = commandbuf;
        }
        else
        {
            command = commandbuf.substr(0, idx);
        }
        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end())
        {
            cerr << "invalid input command!" << endl;
            continue;
        }

        // 调用响应命令的事件处理回调，mainMenu对修改封闭，添加新功能不需要修改函数
        it->second(clientfd, commandbuf.substr(idx + 1, commandbuf.size() - idx)); // 调用命令处理方法
    }
}

// "help" command hanler
void help(int, string)
{
    cout << "show command list >>> " << endl;
    for (auto &p : commandMap)
    {
        cout << p.first << " : " << p.second << endl;
    }
    cout << endl;
}

// "chat" command handler
void chat(int clientfd, string str)
{
    int idx = str.find(":"); // friendid:message
    if (-1 == idx)
    {
        cerr << "chat command invalid!" << endl;
        return;
    }

    int friendid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    ik_ChatService::OneChatRequest onechat_req;
    onechat_req.set_id(g_currentUser.getId());
    onechat_req.set_toid(friendid);
    onechat_req.set_msg(message);
    onechat_req.set_time(getCurrentTime());
    ik_Gate::GateRequest request;
    request.set_type("OneChat");
    request.set_request_msg(onechat_req.SerializeAsString());

    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send chat msg error ->" << buffer << endl;
    }
}
// "addfriend" command handler;
void addfriend(int clientfd, string str)
{
    int friendid = atoi(str.c_str());
    ik_FriendService::AddFriendRequest add_req;
    add_req.set_myid(g_currentUser.getId());
    add_req.set_friendid(friendid);
    ik_Gate::GateRequest request;
    request.set_type("AddFriend");
    request.set_request_msg(add_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send addfriend msg error ->" << buffer << endl;
    }
}
void deletefriend(int clientfd, string str)
{
    int friendid = atoi(str.c_str());
    ik_FriendService::DeleteFriendRequest del_req;
    del_req.set_myid(g_currentUser.getId());
    del_req.set_friendid(friendid);
    ik_Gate::GateRequest request;
    request.set_type("DeleteFriend");
    request.set_request_msg(del_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send deletefriend msg error ->" << buffer << endl;
    }
}
// "userinfo" command handler
void userinfo(int clientfd, string str)
{
    int id = atoi(str.c_str());
    ik_FriendService::UserInfoRequest user_req;
    user_req.set_id(id);
    ik_Gate::GateRequest request;
    request.set_type("GetUserInfo");
    request.set_request_msg(user_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send userinfo msg error ->" << buffer << endl;
    }
}
// "creategroup" command handler
void creategroup(int clientfd, string str)
{
    int idx = str.find(":");
    if (-1 == idx)
    {
        cerr << "creategroup command invalid!" << endl;
        return;
    }

    string groupname = str.substr(0, idx);
    string groupdesc = str.substr(idx + 1, str.size() - idx);

    ik_GroupService::CreateGroupRequest cregrp_req;
    cregrp_req.set_userid(g_currentUser.getId());
    cregrp_req.set_groupname(groupname);
    cregrp_req.set_groupdesc(groupdesc);
    ik_Gate::GateRequest request;
    request.set_type("CreateGroup");
    request.set_request_msg(cregrp_req.SerializeAsString());
    string buffer = request.SerializeAsString();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send creategroup msg error -> " << buffer << endl;
    }
}
// "addgroup" command handler;
void addgroup(int clientfd, string str)
{
    int groupid = atoi(str.c_str());
    ik_GroupService::AddGroupRequest addgrp_req;
    addgrp_req.set_userid(g_currentUser.getId());
    addgrp_req.set_groupid(groupid);
    ik_Gate::GateRequest request;
    request.set_type("AddGroup");
    request.set_request_msg(addgrp_req.SerializeAsString());

    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send addgroup msg error ->" << buffer << endl;
    }
}
// "groupchat" command handler;
void groupchat(int clientfd, string str)
{
    int idx = str.find(":");
    if (-1 == idx)
    {
        cerr << "groupchat command invalid!" << endl;
        return;
    }

    int groupid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);
    ik_ChatService::GroupChatRequest grpchat_req;
    grpchat_req.set_userid(g_currentUser.getId());
    grpchat_req.set_groupid(groupid);
    grpchat_req.set_msg(message);
    grpchat_req.set_time(getCurrentTime());
    ik_Gate::GateRequest request;
    request.set_type("GroupChat");
    request.set_request_msg(grpchat_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (-1 == len)
    {
        cerr << "send groupchat msg errot -> " << buffer << endl;
    }
}
// "loginout" command handler;
void loginout(int clientfd, string str)
{
    ik_UserService::LoginOutRequest loginout_request;
    loginout_request.set_id(g_currentUser.getId());
    ik_Gate::GateRequest request;
    request.set_type("Loginout");
    request.set_request_msg(loginout_request.SerializeAsString());
    string buffer = request.SerializeAsString();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send loginout msg error -> " << buffer << endl;
    }
    else
    {
        isMainMenuRunning = false;
    }
}

void grouplist(int clientfd, string)
{
    ik_GroupService::GetGroupsRequest getgrp_req;
    getgrp_req.set_userid(g_currentUser.getId());
    ik_Gate::GateRequest request;
    request.set_type("GetGroups");
    request.set_request_msg(getgrp_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send grouplist msg error ->" << buffer << endl;
    }
}

void groupusers(int clientfd, string str)
{
    int groupid = atoi(str.c_str());
    ik_GroupService::GetGroupUsersRequest getgrpusr_req;
    getgrpusr_req.set_groupid(groupid);
    ik_Gate::GateRequest request;
    request.set_type("GetGroupUsers");
    request.set_request_msg(getgrpusr_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send grouplist msg error ->" << buffer << endl;
    }
}
void quitgroup(int clientfd, string str)
{
    int groupid = atoi(str.c_str());
    ik_GroupService::QuitGroupRequest quitgrp_req;
    quitgrp_req.set_groupid(groupid);
    ik_Gate::GateRequest request;
    request.set_type("GetGroupUsers");
    request.set_request_msg(quitgrp_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send grouplist msg error ->" << buffer << endl;
    }
}

void friendlist(int clientfd, string)
{
    ik_FriendService::FriendListRequest frilist_req;
    frilist_req.set_id(g_currentUser.getId());
    ik_Gate::GateRequest request;
    request.set_type("GetFriendList");
    request.set_request_msg(frilist_req.SerializeAsString());
    string buffer = request.SerializeAsString();
    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()), 0);
    if (-1 == len)
    {
        cerr << "send friendlist msg error ->" << buffer << endl;
    }
}

// 获取系统时间（聊天信息需要添加时间信息）
string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm *ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d",
            (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday,
            (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return string(date);
}