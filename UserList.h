#ifndef _USER_LIST_
#define _USER_LIST_

#include "BasisMacro.h"

typedef struct
{
    char *str;
    int length;
}string;

typedef struct
{
    string username;
    int index;
}userStruct;

typedef struct
{
    userStruct user[USER_MAX_NUM];
    int num;
}userList;

Status initUser(userStruct *user);
Status deleteUser(userStruct *user);
Status initUserList(userList *list);
Status deleteUserList(userList *list);
Status makeUserList(const char *str, userList *list);


#endif