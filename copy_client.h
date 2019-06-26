#ifndef _CLIENT_
#define _CLIENT_

#include "BasisMacro.h"
#include "MsgStruct.h"
#include "MsgParser.h"
#include "MsgPackager.h"
#include "SendFunc.h"

void *processRecv(void *arg);
int putStreamIntoFile(char *stream, int len);
Status readyForRecvFile(char *str);
void finishRecvFile();
Status recvFilefrom(MsgEntity *ety);

#endif