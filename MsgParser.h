#ifndef _MSG_PARSER_
#define _MSG_PARSER_

#include "MsgStruct.h"


Status analysisLoginCmd(const char *str, char *user, char *pwd);
Status analysisRegisterCmd(const char *str, char *user, char *pwd);
// 使用前记得先用malloc为list申请好内存
Status analysisUserListCmd(const char *str, char *list[], int *num);

// 解析命令实体，用法需要特别注意，interestType需要真实地址传入，
// interest1和interest2传进去是NULL，传出来申请了内存空间，后面记得free
Status analysisCmdEty(MsgEntity *entity, int *interestType, void *interest1, void *interest2);

Status clientLogin(MsgEntity *entity, char *user, char * pwd);
Status clientRegister(MsgEntity *entity, char *user, char *pwd);


#endif