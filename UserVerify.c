#include "UserVerify.h"

Status setUserVerify(userVerify *user, char *name, char *pwd)
{
    if(NULL == user)
    {
        printf("setUserVerify error: pointer is null\n");
        return FAILURE;
    }

    strcpy(user->username, name);
    strcpy(user->password, pwd);
    
    return SUCCESSFUL;
}

void showUserVerify(userVerify user)
{
    printf("name: %s, passwords: %s\n", user.username, user.password);
}