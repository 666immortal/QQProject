#ifndef _SERVER_FUNC_
#define _SERVER_FUNC_

#define MAXBUFLEN 300

#include "MsgParser.h"
#include "MsgPackager.h"

typedef struct
{
    int ID;
    int online;
}threadStatus;

void *serverClient(void* arg);
Status unPack(MsgContainer package); // 解包函数
Status pack(void); // 打包函数

Status clientRegister(userVerify info); // 客户注册函数
Status clientLogin(userVerify info); // 客户登陆函数
Status clientWannaList(userList list); // 客户求取列表函数
Status clientExit(void); // 客户退出函数

Status sendOK(int ID);  // 响应成功
Status sendError(int ID); // 响应失败
Status sendCmd(MsgEntity *ety, int ID); // 发送命令函数
Status sendDlg(MsgEntity *ety, int ID); // 发送对话函数

#endif