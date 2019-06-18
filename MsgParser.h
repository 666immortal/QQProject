#ifndef _MSG_PARSER_
#define _MSG_PARSER_

#include "MsgStruct.h"
#include "UserList.h"
#include "UserVerify.h"

// 联合体，在处理命令时可以区分验证信息还是列表
typedef union 
{
    userVerify nameAndPwd;
    userList list;
}Stnparser;

// 解析登陆&注册命令内容
Status analysisLogRegCmd(const char *str, userVerify *getNamePwd);
// 解析用户列表内容
Status analysisUserListCmd(const char *str, userList *list);
// 开始解析一个命令消息
Status analysisCmdEty(MsgEntity *entity, int *Msgtype, Stnparser *container);
// 结束解析一个命令消息，记得要配对使用，否则会造成内存泄漏
Status endAnalysis(int Msgtype, Stnparser *container);


#endif