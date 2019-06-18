#ifndef _MSG_STRUCT_
#define _MSG_STRUCT_

#include "BasisMacro.h"

typedef enum
{
    SEND_FLAG = 0,
    RECV_FLAG = 1
}MsgFlag;

typedef enum
{
    COMMAND = 0,
    DIALOGUE = 1
}MsgType;

typedef enum 
{
    CMD_REGISTER = -1,
    CMD_LOGIN = -2,
    CMD_GETLIST = -3,
    CMD_EXIT = -4
}CmdType;

typedef struct 
{
    int object;
    char   details[DETAILS_LEN];
    int flag;
}MsgEntity;

typedef struct 
{
    MsgType type;
    MsgEntity content;
}MsgContainer;


#endif
