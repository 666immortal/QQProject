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

void *serverClient(void* arg); // 线程服务函数
Status unPack(MsgContainer package); // 解包函数
Status pack(void); // 打包函数

Status clientRegister(userVerify info); // 客户注册处理函数
Status clientLogin(userVerify info); // 客户登陆处理函数
Status clientWannaList(int ID); // 客户求取列表处理函数
Status clientExit(int ID); // 客户退出处理函数

// 用户登陆时，服务器回应Login表示登陆成功，服务器回应Register表示登陆失败
Status sendLogin(int ID);
// 用户注册时，服务器回应Register表示注册成功，服务器回应Login表示注册失败
Status sendRegister(int ID);

// 群发列表，通常用来更新列表
Status broadcastList(void);
// 群发信息
Status broadcastDlg(void);

Status sendCmd(MsgEntity *ety, int ID); // 发送命令函数
Status sendDlg(MsgEntity *ety, int ID); // 发送对话函数

#endif