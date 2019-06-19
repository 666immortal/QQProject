#include "SendFunc.h"


// 用户登陆时，服务器回应Login表示登陆成功，服务器回应Register表示登陆失败
Status sendLogin(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_LOGIN;
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

// 用户注册时，服务器回应Register表示注册成功，服务器回应Login表示注册失败
Status sendRegister(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_REGISTER;
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

Status sendCmd(MsgEntity *ety, int ID)
{
    if(NULL == ety)
    {
        perror("sendCmd error: pointer is null\n");
        return FAILURE;
    }

    MsgContainer tmp;
    tmp.type = COMMAND;
    tmp.content = *ety;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer)) == -1)
    {
        perror("send Error!");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status sendDlg(MsgEntity *ety, int ID)
{
    if(NULL == ety)
    {
        perror("sendCmd error: pointer is null\n");
        return FAILURE;
    }

    MsgContainer tmp;
    tmp.type = DIALOGUE;
    tmp.content = *ety;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer)) == -1)
    {
        perror("send Error!");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status forwardDlg(MsgEntity *ety, int from)
{
    if(NULL == ety)
    {
        printf("forwardDlg error: pointer is null\n");
        return FAILURE;
    }

    MsgEntity tmp;
    tmp.object = from;
    strcpy(tmp.details, ety->details);
    tmp.flag = RECV_FLAG;

    sendDlg(&tmp, ety->object);

    return SUCCESSFUL;
}

Status broadcastList(userList *list)
{
    int i;
    MsgEntity tmp;
    if(configUserListEty(&tmp, &list) == SUCCESSFUL)
    {
        for(i = 0; i < list->num; i++)
        {
            if(sendCmd(&tmp, list->user[i].threadID) == FAILURE)
            {
                printf("broadcastList error: send msg failure\n");
                return FAILURE;
            }
        }
    }
    else
    {
        perror("broadcastList error: config eneity failure\n");
        return FAILURE;
    }
    
    return SUCCESSFUL;
}

Status broadcastDlg(userList *list, MsgEntity *ety, int ID)
{
    MsgEntity tmp;
    tmp.object = ID;
    tmp.flag = SEND_FLAG;
    strcpy(tmp.details, ety->details);

    int i;
    for(i = 0; i < list->num; i++)
    {
        if(list->user[i].threadID == ID)
            continue;
        if(sendDlg(&tmp, list->user[i].threadID) == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESSFUL;
}