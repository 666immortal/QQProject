#include "MsgParser.h"

Status analysisLogRegCmd(const char *str, userVerify *getNamePwd)
{
    if(NULL == str)
    {
        printf("analysisLogRegCmd error: pointer is null");
        return FAILURE;
    }

    sscanf(str, "%s %s", getNamePwd->username, getNamePwd->password);
    
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
    int type, n;

    switch (type = entity->object)
    {
    case CMD_LOGIN:
    case CMD_REGISTER:
        initUserVerify(&container->nameAndPwd);
        analysisLogRegCmd(entity->details, &container->nameAndPwd);
        res = SUCCESSFUL;
        break;
    case CMD_GETLIST:
        initUserList(&container->list);
        analysisUserListCmd(entity->details, &container->list);
        break;
    case CMD_EXIT:
        printf("exit");
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

Status endAnalysis(CmdType *Msgtype, Stnparser *container)
{
    if(NULL == container)
    {
        printf("endAnalysis error: pointer is null");
        return FAILURE;
    }

    if(Msgtype == CMD_LOGIN || Msgtype == CMD_REGISTER)
    {
        deleteUserVerify(&container->nameAndPwd);
    }
    else if(Msgtype == CMD_GETLIST)
    {
        deleteUserList(&container->list);
    }
    
    return SUCCESSFUL;
}