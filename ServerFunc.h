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
Status unPack(MsgContainer package);
Status pack(void);

Status clientRegister(userVerify info);
Status clientLogin(userVerify info);
Status clientWannaList(userList list);
Status clientExit(void);

Status sendOK(void);
Status sendError(void);

#endif