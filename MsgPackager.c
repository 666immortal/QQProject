#include "MsgPackager.h"

Status setCommand(MsgEntity *cmd, CmdType doWhat, char *str, int flag)
{
    if(str == NULL || cmd == NULL)
    {
        return FAILURE;
    }

    cmd->object = doWhat;
    strcpy(cmd->details, str);
    cmd->flag = flag;

    return SUCCESSFUL;
}

Status setDialogue(MsgEntity *dlg, int forWho, char *str, MsgFlag flag)
{
    if(str == NULL || dlg == NULL)
    {
        return FAILURE;
    }

    dlg->object = forWho;
    strcpy(dlg->details, str);
    dlg->flag = flag;

    return SUCCESSFUL;
}

Status configLoginEty(MsgEntity *entity, char *user, char * pwd)
{
    if(user == NULL || pwd == NULL || entity == NULL)
    {
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    sprintf(jointStr, "%s %s", user, pwd);
    entity->object = CMD_LOGIN;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configRegisterEty(MsgEntity *entity, char *user, char * pwd)
{
    if(user == NULL || pwd == NULL || entity == NULL)
    {
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    sprintf(jointStr, "%s %s", user, pwd);
    entity->object = CMD_REGISTER;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configUserListEty(MsgEntity *entity, char **list, int num)
{
    if(list == NULL || entity == NULL)
    {
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    int i;

    strcpy(jointStr, list[0]);
    for(i = 1; i < num; i++)
    {
        strcat(jointStr, list[i]);
    }

    entity->object = CMD_GETLIST;
    entity->flag = num;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configExitEty(MsgEntity *entity)
{
    if(entity == NULL)
    {
        return FAILURE;
    }

    entity->object = CMD_EXIT;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, "");

    return SUCCESSFUL;
}