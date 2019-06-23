#ifndef _SEND_FUNC_
#define _SEND_FUNC_

#include "MsgStruct.h"
#include "UserList.h"

// 用户登陆时，服务器回应Login表示登陆成功，服务器回应Register表示登陆失败
Status sendLogin(int ID);
// 用户注册时，服务器回应Register表示注册成功，服务器回应Login表示注册失败
Status sendRegister(int ID);

// 转发消息
Status forwardDlg(MsgEntity *ety, int from);
// 群发列表，通常用来更新列表
Status broadcastList(userList *list);
// 群发信息
Status broadcastDlg(userList *list, MsgEntity *ety, int ID);

Status sendCmd(MsgEntity *ety, int ID); // 发送命令函数
Status sendDlg(MsgEntity *ety, int ID); // 发送对话函数

Status remindForReceive(MsgEntity *ety, int from);
Status forwardFile(MsgEntity *ety);
Status endTransferFile(MsgEntity *ety, int from);

Status clientSendReady(int ID);
Status serverSendReady(int ID);

#endif