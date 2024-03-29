#include "MsgParser.h"

Status analysisLogRegCmd(const char *str, userVerify *getNamePwd)
{
    if(NULL == str)
    {
        printf("analysisLogRegCmd error: pointer is null\n");
        return FAILURE;
    }

    int i = 0, k = 0;
    char tmp[NAME_MAX_LEN];

    while (str[i] != '\0')
    {
        if(str[i] != '#')
        {
            tmp[k] = str[i];
            k++;
        }
        else
        {
            tmp[k] = '\0';
            strcpy(getNamePwd->username, tmp);
            k = 0;            
        }    
        i++;    
    }
    tmp[k] = '\0';
    strcpy(getNamePwd->password, tmp);
    
    return SUCCESSFUL;
}

Status analysisUserListCmd(const char *str, userList *list)
{
    if(NULL == str || NULL == list)
    {
        printf("analysisUserListCmd error: pointer is null\n");
        return FAILURE;
    }

    makeUserList(str, list);   

    return SUCCESSFUL;
}

Status analysisCmdEty(MsgEntity *entity, int *Msgtype, Stnparser *container)
{
    Status res = FAILURE;
    int type;
    
    switch (type = entity->object)
    {
    case CMD_LOGIN:
    case CMD_REGISTER:
        res = analysisLogRegCmd(entity->details, &container->nameAndPwd);        
        break;
    case CMD_GETLIST:
        initUserList(&container->list);
        res  = analysisUserListCmd(entity->details, &container->list);        
        break;
    case CMD_SEND_FILE:
        printf("parser: File Process\n");
        res = SUCCESSFUL;
        break;
    case CMD_TRANSFERING:
        printf("parser: transfering...\n");
        res = SUCCESSFUL;
        break;
    case CMD_EXIT:
        printf("parser: exit\n");
        res = SUCCESSFUL;
        break;
    case CMD_END_TRANSFER:
        printf("transfer end...\n");
        res = SUCCESSFUL;
        break;
    default:
        printf("analysisCmdEty error: error type\n");
        break;
    }
    
    if(Msgtype != NULL)
    {
        *Msgtype = type;
    }

    return res;
}

Status endAnalysis(int Msgtype, Stnparser *container)
{
    if(NULL == container)
    {
        printf("endAnalysis error: pointer is null\n");
        return FAILURE;
    }
    else if(Msgtype == CMD_GETLIST)
    {
        deleteUserList(&container->list);
    }
    
    return SUCCESSFUL;
}