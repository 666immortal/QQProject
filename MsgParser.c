#include "MsgParser.h"

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
        list->user[i].length = 0;
        list->user[i].str = (string)malloc(sizeof(char)*NAME_MAX_LEN);
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
        free(list->user[i].str);
    }

    return SUCCESSFUL;
}

Status makeOnlineList(const char *str, userList *list)
{
    if(NULL == str)
    {
        return FAILURE;
    }

    int i = 0, k = 0, n = 0;
    char tmp[20];

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
            list->user[list->num].length = k;
            strcpy(list->user[list->num].str, tmp);
            list->num++;
            k = 0;
        }
        
        i++;
    }


    return SUCCESSFUL;
}

Status analysisLoginCmd(const char *str, char *user, char *pwd)
{
    if(NULL == str)
    {
        return FAILURE;
    }

    sscanf(str, "%s %s", user, pwd);

    return SUCCESSFUL;
}

Status analysisRegisterCmd(const char *str, char *user, char *pwd)
{
    if(NULL == str)
    {
        return FAILURE;
    }

    sscanf(str, "%s %s", user, pwd);
    
    return SUCCESSFUL;
}

Status analysisUserListtCmd(const char *str, char *list[], int *num)
{
    if(NULL == str)
    {
        return FAILURE;
    }

    int i = 0, k = 0, n = 0;
    char tmp[20];

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
            strcpy(list[n], tmp);
            n++;
            k = 0;
        }
        
        i++;
    }

    tmp[k] = '\0';
    strcpy(list[n], tmp);
    n++;
    *num = n;

    return SUCCESSFUL;
}

Status analysisCmdEty(MsgEntity *entity, int *interestType, void *interest1, void *interest2)
{
    Status res = FAILURE;
    int type, n;

    switch (type = entity->object)
    {
    case CMD_LOGIN:
        interest1 = (void *)malloc(sizeof(char)*NAME_MAX_LEN);
        interest2 = (void *)malloc(sizeof(char)*PWD_MAX_LEN);
        analysisLoginCmd(entity->details, (char*)interest1, (char*)interest2);
        res = SUCCESSFUL;
        break;
    case CMD_REGISTER:
        interest1 = (void *)malloc(sizeof(char)*NAME_MAX_LEN);
        interest2 = (void *)malloc(sizeof(char)*PWD_MAX_LEN);
        analysisRegisterCmd(entity->details, (char*)interest1, (char*)interest2);
        res = SUCCESSFUL;
        break;
    case CMD_GETLIST:
        interest1 = malloc(sizeof(char*)*ONLINE_MAX_NUM);
        for(n = 0; n < ONLINE_MAX_NUM; n++)
        {
            *((char **)interest1 + n) = (char *)malloc(sizeof(char)*NAME_MAX_LEN);
        }

        interest2 = (void *)malloc(sizeof(int));
        analysisOnlineListCmd(entity->details, (char **)interest1, (int *)interest2);
        break;
    case CMD_EXIT:
        printf("exit");
        break;
    default:
        break;
    }
    
    if(interestType != NULL)
    {
        *interestType = type;
    }

    return res;
}