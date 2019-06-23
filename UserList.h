#ifndef _USER_LIST_
#define _USER_LIST_

#include "BasisMacro.h"

// 用户结构体
typedef struct
{
    string username;
    int threadID;
}userStruct;

// 在线用户列表
typedef struct
{
    userStruct user[USER_MAX_NUM];
    int num;
}userList;

Status initUser(userStruct *user); // 初始化一个用户类
Status deleteUser(userStruct *user); // 删除一个用户类（指释放内存）
Status initUserList(userList *list); // 初始化用户列表
Status deleteUserList(userList *list); // 删除用户列表
Status makeUserList(const char *str, userList *list); // 根据消息更新用户列表
Status showUserList(userList list); // 显示用户列表
Status listToString(userList list, char *str); // 将列表连接成串，以空格分隔
// 添加删除用户列表都应该互斥访问，
// 添加删除用户之后应该刷新各客户端的用户列表
// 函数返回添加后的索引值
Status addUser(userList *list, char *name, int ID);
// 从列表中移除用户之后，记得释放user指针
Status removeUser(userList *list, int index);
char* searchNameInUserList(userList *list, int ID);

#endif