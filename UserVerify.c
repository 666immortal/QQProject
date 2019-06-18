#include "UserVerify.h"

Status initUserVerify(userVerify *user)
{
    if(NULL == user)
    {
        printf("initUserVerify error: pointer is null");
        return FAILURE;
    }

    user->username.length = 0;
    user->password.length = 0;
    user->username.str = (char *)malloc(sizeof(char)*NAME_MAX_LEN);
    user->password.str = (char *)malloc(sizeof(char)*PWD_MAX_LEN);

    return SUCCESSFUL;
}

Status deleteUserVerify(userVerify *user)
{
    if(NULL == user)
    {
        printf("deleteUserVerify error: pointer is null");
        return FAILURE;
    }
    free(user->username.str);
    free(user->password.str);

    return SUCCESSFUL;
}

void showUserVerify(userVerify user)
{
    printf("name: %d, passwords: %d\n", user.username, user.password);
}