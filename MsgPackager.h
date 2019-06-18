#ifndef _MSG_PACKAGER_
#define _MSG_PACKAGER_

#include "MsgStruct.h"
#include "UserList.h"

// 将命令配置在实体中
Status setCommand(MsgEntity *cmd, CmdType doWhat, char *str, int flag);
// 将消息配置在实体中
Status setDialogue(MsgEntity *dlg, int forWho, char *str, MsgFlag flag);
// 配置登陆实体，客户端才用得到，服务器负责解析
Status configLoginEty(MsgEntity *entity, char *user, char * pwd);
// 配置注册实体，客户端才用得到，服务器负责解析
Status configRegisterEty(MsgEntity *entity, char *user, char * pwd);
// 配置在线用户列表实体，服务器才用得到，客户端负责解析，客户不能太多
Status configUserListEty(MsgEntity *entity, userList *list);
// 配置退出实体，客户端才用得到，服务器负责解析
Status configExitEty(MsgEntity *entity);

#endif