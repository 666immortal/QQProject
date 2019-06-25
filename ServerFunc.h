#ifndef _SERVER_FUNC_
#define _SERVER_FUNC_

#define MAXBUFLEN 300

#include "MsgParser.h"
#include "MsgPackager.h"
#include "SendFunc.h"

typedef struct
{
    int ID;
    int online;
}threadStatus;

void *serverClient(void* arg); // 线程服务函数
Status unPack(MsgContainer package); // 解包函数
Status pack(void); // 打包函数

Status searchInFile(userVerify info); // 在文件中查找用户
Status clientRegister(userVerify info); // 客户注册处理函数
Status clientLogin(userVerify info); // 客户登陆处理函数
Status clientWannaList(int ID); // 客户求取列表处理函数

#endif