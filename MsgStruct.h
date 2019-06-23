#ifndef _MSG_STRUCT_
#define _MSG_STRUCT_

#include "BasisMacro.h"

// 群发消息标志
#define BOARDCAST 0
// 发送接收标志
typedef enum
{
    SEND_FLAG = 0, // 发送标志
    RECV_FLAG = 1  // 接收标志
}MsgFlag;

// 消息类型
typedef enum
{
    COMMAND = 0, // 命令
    DIALOGUE = 1 // 对话
}MsgType;

// 命令类型
typedef enum 
{
    CMD_REGISTER = -1, // 注册
    CMD_LOGIN = -2, // 登陆
    CMD_GETLIST = -3, // 获取列表
    CMD_EXIT = -4, // 退出
    CMD_SEND_FILE = -5,
    CMD_TRANSFERING = -6,
    CMD_READY = -7
}CmdType;

// 消息实体
typedef struct 
{
    int object; // 对象
    char   details[DETAILS_LEN]; // 具体内容
    int flag; // 发送接收标志
}MsgEntity;

// 消息类
typedef struct 
{
    MsgType type; // 消息类型
    MsgEntity content; // 消息实体
}MsgContainer;

void showMsgEntity(MsgEntity ety);
void showMsgContainer(MsgContainer ctn);

#endif
