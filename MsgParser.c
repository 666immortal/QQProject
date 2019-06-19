#include "MsgParser.h"

Status analysisLogRegCmd(const char *str, userVerify *getNamePwd)
{
    if(NULL == str)
    {
        printf("analysisLogRegCmd error: pointer is null");
        return FAILURE;
    }

    int i = 0, k = 0;
    char tmp[NAME_MAX_LEN];

    while (str[i] != '\0')
    {
        if(str[i] != ' ')
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
        printf("analysisUserListCmd error: pointer is null");
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
        analysisLogRegCmd(entity->details, &container->nameAndPwd);
        res = SUCCESSFUL;
        break;
    case CMD_GETLIST:
        initUserList(&container->list);
        analysisUserListCmd(entity->details, &container->list);
        res = SUCCESSFUL;
        break;
    case CMD_EXIT:
        printf("exit");
        res = SUCCESSFUL;
        break;
    default:
        printf("analysisCmdEty error: error type");
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
        printf("endAnalysis error: pointer is null");
        return FAILURE;
    }
    else if(Msgtype == CMD_GETLIST)
    {
        deleteUserList(&container->list);
    }
    
    return SUCCESSFUL;
}