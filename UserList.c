#include "UserList.h"

Status initUser(userStruct *user)
{
    if(NULL == user)
    {
        printf("initUser error: pointer is null\n");
        return FAILURE;
    }

    user->threadID = 0;
    user->username = (char *)malloc(sizeof(char)*NAME_MAX_LEN);

    return SUCCESSFUL;
}

Status deleteUser(userStruct *user)
{
    if(NULL == user)
    {
        printf("deleteUser error: pointer is null\n");
        return FAILURE;
    }

    free(user->username);

    return SUCCESSFUL;
}

Status initUserList(userList *list)
{
    if(NULL == list)
    {
        printf("initUserList error: pointer is null\n");
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
        printf("deleteUserList error: pointer is null\n");
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
        printf("makeUserList error: pointer is null\n");
        return FAILURE;
    }

    int i = 0, k = 0, n = 0, ctr = 0;
    char tmp[NAME_MAX_LEN];

    while ('\0' != str[i])
    {
        if('#' != str[i])
        {
            tmp[k] = str[i];
            k++;      
        }
        else
        {
            if(0 == ctr)
            {
                tmp[k] = '\0';
                strcpy(list->user[n].username, tmp);
                ctr = 1;
            }
            else
            {
                tmp[k] = '\0';
                list->user[n].threadID = atoi(tmp);
                n++;
                ctr = 0;
            }           
            k = 0;
        }       
        i++;
    }

    tmp[k] = '\0';
    list->user[n].threadID = atoi(tmp);
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
        printf("ID.%d: %s\n", list.user[i].threadID, list.user[i].username);
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
    char putID[20];

    sprintf(putID, "%d", list.user[0].threadID);

    strcpy(str, list.user[0].username);
    strcat(str, "#");
    strcat(str, putID);

    for(i = 1; i < list.num; i++)
    {
        strcat(str, "#");
        strcat(str, list.user[i].username);
        strcat(str, "#");
        sprintf(putID, "%d", list.user[i].threadID);
        strcat(str, putID);
    }

    return 0;
}

Status addUser(userList *list, char *name, int ID)
{
    if(list->num >= USER_MAX_NUM)
    {
        printf("addUser error: reach maximum user num\n");
        return FAILURE;
    }

    strcpy(list->user[list->num].username, name);
    list->user[list->num].threadID = ID;
    
    list->num++;

    return SUCCESSFUL; 
}

Status removeUser(userList *list, int ID)
{
    if(list->num <= 0)
    {
        printf("removeUser error: user list is empty\n");
        return FAILURE;
    }

    int i;
    for(i = 0; i < list->num; i++)
    {
        if(list->user->threadID == ID)
            break;
    }

    if(i >= list->num)
    {
        printf("No such user in list\n");
        return FAILURE;
    }

    for(; i < list->num - 1; i++)
    {
        list->user[i] = list->user[i + 1];  
    }

    list->num--;

    return SUCCESSFUL;
}