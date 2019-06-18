#include "UserList.h"

Status initUser(userStruct *user)
{
    if(NULL == user)
    {
        return FAILURE;
    }

    user->index = 0;
    user->username.length = 0;
    user->username.str = (char *)malloc(sizeof(char)*NAME_MAX_LEN);

    return SUCCESSFUL;
}

Status deleteUser(userStruct *user)
{
    if(NULL == user)
    {
        return FAILURE;
    }

    free(user->username.str);

    return SUCCESSFUL;
}

Status initUserList(userList *list)
{
    if(NULL == list)
    {
        return FAILURE;
    }

    int i;
    list->num = 0;
    for(i = 0; i < USER_MAX_NUM; i++)
    {
       initUser(&list->user[i]);
    }
    
    return SUCCESSFUL;
}

Status deleteUserList(userList *list)
{
    if(NULL == list)
    {
        return FAILURE;
    }

    int i;
    for(i = 0; i < USER_MAX_NUM; i++)
    {
        deleteUser(&list->user[i]);
    }

    return SUCCESSFUL;
}

Status makeUserList(const char *str, userList *list)
{
    if(NULL == str)
    {
        return FAILURE;
    }

    int i = 0, k = 0, n = 0;
    char tmp[NAME_MAX_LEN];

    while ('\0' != str[i])
    {
        if(' ' != str[i])
        {
            tmp[k] = str[i];
            k++;      
        }
        else
        {
            tmp[k] = '\0';
            list->user->username.length = k;
            strcpy(list->user->username.str, tmp);
            list->num++;
            k = 0;
        }
        
        i++;
    }

    tmp[k] = '\0';
    list->user->username.length = k;
    strcpy(list->user->username.str, tmp);
    list->num++;

    return SUCCESSFUL;
}