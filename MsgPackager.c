#include "MsgPackager.h"

Status setCommand(MsgEntity *cmd, CmdType doWhat, char *str, int flag)
{
    if(str == NULL || cmd == NULL)
    {
        printf("setCommand error: pointer is null\n");
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
        printf("setDialogue error: pointer is null\n");
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
        printf("configLoginEty error: pointer is null\n");
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    sprintf(jointStr, "%s#%s", user, pwd);
    entity->object = CMD_LOGIN;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configRegisterEty(MsgEntity *entity, char *user, char * pwd)
{
    if(user == NULL || pwd == NULL || entity == NULL)
    {
        printf("configRegisterEty error: pointer is null\n");
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    sprintf(jointStr, "%s#%s", user, pwd);
    entity->object = CMD_REGISTER;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configUserListEty(MsgEntity *entity, userList *list)
{
    if(list == NULL || entity == NULL)
    {
        printf("configUserListEty error: pointer is null\n");
        return FAILURE;
    }

    char jointStr[DETAILS_LEN];
    listToString(*list, jointStr);
    entity->object = CMD_GETLIST;
    entity->flag = list->num;
    strcpy(entity->details, jointStr);

    return SUCCESSFUL;
}

Status configExitEty(MsgEntity *entity)
{
    if(entity == NULL)
    {
        printf("configExitEty error: pointer is null\n");
        return FAILURE;
    }

    entity->object = CMD_EXIT;
    entity->flag = SEND_FLAG;
    strcpy(entity->details, "");

    return SUCCESSFUL;
}
