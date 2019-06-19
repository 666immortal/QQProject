#ifndef _USER_VERIFY_
#define _USER_VERIFY_

#include "BasisMacro.h"

// 验证信息，包括用户名和密码
typedef struct
{
    char username[NAME_MAX_LEN];
    char password[PWD_MAX_LEN];
}userVerify;

Status setUserVerify(userVerify *user, char *name, char *pwd);
void showUserVerify(userVerify user);

#endif
