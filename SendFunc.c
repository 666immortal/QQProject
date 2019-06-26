#include "SendFunc.h"

// 用户登陆时，服务器回应Login表示登陆成功，服务器回应Register表示登陆失败
Status sendLogin(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_LOGIN;
    strcpy(tmp.details, "");
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

// 用户注册时，服务器回应Register表示注册成功，服务器回应Login表示注册失败
Status sendRegister(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_REGISTER;
    strcpy(tmp.details, "");
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

// ID为发送传文件请求的人
Status sendCmd(MsgEntity *ety, int ID)
{
    if(NULL == ety)
    {
        perror("sendCmd error: pointer is null\n");
        return FAILURE;
    }

    MsgContainer tmp;
    tmp.type = COMMAND;
    strcpy(tmp.content.details, ety->details);
    tmp.content.flag = ety->flag;
    tmp.content.object = ety->object;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer), 0) == -1)
    {
        perror("send Error!\n");
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
    strcpy(tmp.content.details, ety->details);
    tmp.content.flag = ety->flag;
    tmp.content.object = ety->object;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer), 0) == -1)
    {
        perror("send Error!\n");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status remindForReceive(MsgEntity *ety, int from)
{
    if(NULL == ety)
    {
        printf("forwardDlg error: pointer is null\n");
        return FAILURE;
    }

    MsgEntity tmp;
    tmp.object = CMD_SEND_FILE;
    for(int i = 0; i < DETAILS_LEN; i++)
    {
        tmp.details[i] = ety->details[i];
    }
    tmp.flag = from;

    sendCmd(&tmp, ety->flag);

    return SUCCESSFUL;
}

Status remindForEnding(MsgEntity *ety, int from)
{
    if(NULL == ety)
    {
        printf("forwardDlg error: pointer is null\n");
        return FAILURE;
    }

    MsgEntity tmp;
    tmp.object = CMD_END_TRANSFER;
    strcpy(tmp.details, "");
    tmp.flag = from;

    sendCmd(&tmp, ety->flag);

    return SUCCESSFUL;
}

Status forwardFile(MsgEntity *ety, int from)
{
    if(NULL == ety)
    {
        printf("forwardFile error: pointer is null\n");
        return FAILURE;
    }
    
    int sendFor = 0;
    unsigned short *point = &ety->flag;

    MsgEntity tmp;
    tmp.object = CMD_TRANSFERING;
    for(int i = 0; i < DETAILS_LEN; i++)
    {
        tmp.details[i] = ety->details[i];
    }

    sendFor = (int)point[1];
    tmp.flag = (int)point[0];

    sendCmd(&tmp, sendFor);

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
    if(configUserListEty(&tmp, list) == SUCCESSFUL)
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
    tmp.object = 0;
    tmp.flag = ID;
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