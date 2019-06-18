#ifndef _USER_LIST_
#define _USER_LIST_

#include "BasisMacro.h"

// 用户结构体
typedef struct
{
    string username;
    int index;
}userStruct;

// 在线用户列表
typedef struct
{
    userStruct user[USER_MAX_NUM];
    int num;
}userList;

Status initUser(userStruct *user); // 初始化一个用户类
Status deleteUser(userStruct *user); // 删除一个用户类
Status initUserList(userList *list); // 初始化用户列表
Status deleteUserList(userList *list); // 删除用户列表
Status makeUserList(const char *str, userList *list); // 根据消息更新用户列表
Status showUserList(userList list); // 显示用户列表
Status listToString(userList list, char *str); // 将列表连接成串，以空格分隔

#endif