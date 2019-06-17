#include "Message.h"
#include <string.h>

Status setCommand(MsgEntity *cmd, CmdType doWhat, char *str, int flag)
{
    if(str == NULL)
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
    if(str == NULL)
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
    if(user == NULL || pwd == NULL)
    {
        return FAILURE;
    }


    return SUCCESSFUL;
}
Status configRegisterEty(MsgEntity *entity, char *user, char * pwd);