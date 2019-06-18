#include "UserVerify.h"

Status initUserVerify(userVerify *user)
{
    if(NULL == user)
    {
        printf("initUserVerify error: pointer is null");
        return FAILURE;
    }

    user->username = (char *)malloc(sizeof(char)*NAME_MAX_LEN);
    user->password = (char *)malloc(sizeof(char)*PWD_MAX_LEN);

    return SUCCESSFUL;
}

Status deleteUserVerify(userVerify *user)
{
    if(NULL == user)
    {
        printf("deleteUserVerify error: pointer is null");
        return FAILURE;
    }
    free(user->username);
    free(user->password);

    return SUCCESSFUL;
}

void showUserVerify(userVerify user)
{
    printf("name: %s, passwords: %s\n", user.username, user.password);
}