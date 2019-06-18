#ifndef _USER_VERIFY_
#define _USER_VERIFY_

#include "BasisMacro.h"

// 验证信息，包括用户名和密码
typedef struct
{
    string username;
    string password;
}userVerify;

Status initUserVerify(userVerify *user);
Status deleteUserVerify(userVerify *user);
void showUserVerify(userVerify user);

#endif
