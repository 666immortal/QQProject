#ifndef _MSG_ENTITY_
#define _MSG_ENTITY_

#define SUCCESSFUL 0
#define FAILURE    1

typedef int Status;

typedef enum
{
    SEND = 0,
    RECV = 1
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
    char   details[200];
    int flag;
}MsgEntity;

typedef struct 
{
    MsgType type;
    MsgEntity content;
}MsgContainer;

// 将命令配置在实体中
Status setCommand(MsgEntity *cmd, CmdType doWhat, char *str, int flag);
// 将消息配置在实体中
Status setDialogue(MsgEntity *dlg, int forWho, char *str, MsgFlag flag);
// 配置登陆实体
Status configLoginEty(MsgEntity *entity, char *user, char * pwd);
// 配置注册实体
Status configRegisterEty(MsgEntity *entity, char *user, char * pwd);


Status clientLogin(MsgEntity *entity, char *user, char * pwd);
Status clientRegister(MsgEntity *entity, char *user, char *pwd);


#endif
