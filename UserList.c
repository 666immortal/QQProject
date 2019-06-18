#include "UserList.h"

Status initUser(userStruct *user)
{
    if(NULL == user)
    {
        printf("initUser error: pointer is null");
        return FAILURE;
    }

    user->index = 0;
    user->username = (char *)malloc(sizeof(char)*NAME_MAX_LEN);

    return SUCCESSFUL;
}

Status deleteUser(userStruct *user)
{
    if(NULL == user)
    {
        printf("deleteUser error: pointer is null");
        return FAILURE;
    }

    free(user->username);

    return SUCCESSFUL;
}

Status initUserList(userList *list)
{
    if(NULL == list)
    {
        printf("initUserList error: pointer is null");
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
        printf("deleteUserList error: pointer is null");
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
        printf("makeUserList error: pointer is null");
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
            strcpy(list->user[n].username, tmp);
            list->user[n].index = n;
            n++;
            k = 0;
        }
        
        i++;
    }

    tmp[k] = '\0';
    strcpy(list->user[n].username, tmp);
    list->user[n].index = n;
    n++;
    list->num = n;

    return SUCCESSFUL;
}

Status showUserList(userList list)
{
    int i;
    if(0 >= list.num)
    {
        printf("Nobody online!\n");
        return FAILURE;
    }

    for(i = 0; i < list.num; i++)
    {
        printf("No.%d: %s\n", list.user[i].index + 1, list.user[i].username);
    }

    return SUCCESSFUL;
}

Status listToString(userList list, char *str)
{
    if(list.num == 0)
    {
        printf("listToString error: nobody in list");
        return FAILURE;
    }
    if(NULL == str)
    {
        printf("listToString error: pointer is null");
        return FAILURE;
    }

    int i;

    strcpy(str, list.user[0].username);

    for(i = 1; i < list.num; i++)
    {
        strcat(str, " ");
        strcat(str, list.user[i].username);
    }

    return 0;
}

Status addUser(userList *list, char *name, int *index)
{
    if(list->num >= USER_MAX_NUM)
    {
        printf("addUser error: reach maximum user num\n");
        return FAILURE;
    }

    strcpy(list->user[list->num].username, name);
    list->user[list->num].index = list->num;
    
    if(index != NULL)
    {
        *index = list->num;
    }
    
    list->num++;

    return SUCCESSFUL;
}

Status removeUser(userList *list, int index)
{
    if(list->num <= 0)
    {
        printf("removeUser error: user list is empty\n");
        return FAILURE;
    }

    int i;
    for(i = index; i < list->num - 1; i++)
    {
        list->user[i] = list->user[i + 1];  
        list->user[i].index--;
    }

    list->num--;

    return SUCCESSFUL;
}