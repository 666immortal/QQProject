#ifndef _CLIENT_
#define _CLIENT_

#include "BasisMacro.h"
#include "MsgStruct.h"
#include "MsgParser.h"
#include "MsgPackager.h"
#include "SendFunc.h"

void *processRecv(void *arg);
void *recvFileThread(void *arg);
void *sendFileThread(void *arg);
int putStreamIntoFile(char *stream, int len);
int getStreamFromFile(char *stream);
Status readyForRecvFile(char *str);
Status readyForSendFile(char *str);
void finishFileTask();
Status sendFileFor(int ID);
Status recvFilefrom(MsgEntity *ety);

#endif