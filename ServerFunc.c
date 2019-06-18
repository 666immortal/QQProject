#include "ServerFunc.h"

extern userList onlineUserList;

static threadStatus state;

#define ONLINE  1
#define OFFLINE 0

void setThreadID(int ID);
void setThreadState(int onlineOrNot);
int getThreadID(void);
int getThreadState(void);

void *serverClient(void* arg)
{
    state.online = FAILURE;
    state.ID = -1;
    int theClient = *(int*)arg;
    int bytesNum;
    char buf[MAXBUFLEN];

    /* if(send(client_fd,"Hello,you are connected!\n",26,0) == -1)
    {
        perror("send Error!");
    }*/

    do
    {
        memset(buf,0,sizeof(buf)); //将内存内容全部初始化为0
        bytesNum = recv(theClient,(void *)buf,sizeof(MsgContainer),0);
        if(bytesNum == -1)
        {
        perror("recv Error:");
        }
        else
        {            
            if(bytesNum <= 0)
            {
                break;
            }
            else if(bytesNum == sizeof(MsgContainer))
            {
                MsgContainer package;
                package = *(MsgContainer*)buf;
            }
        }    
    }while(bytesNum>0);

    close(theClient);
}

Status unPack(MsgContainer package)
{
    if(package.type == COMMAND)
    {
        int type;
        int index;
        Stnparser info;

        analysisCmdEty(&package.content,&type, &info);
        switch (type)
        {
        case CMD_REGISTER:
            if(getThreadState() == OFFLINE)
            {
                if(clientRegister(info.nameAndPwd) == SUCCESSFUL)
                {
                    addUser(&onlineUserList, info.nameAndPwd.username, &index);
                    setThreadID(index);
                    setThreadState(ONLINE);
                    sendOK();
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
                else
                {
                    printf("Register failure\n");
                    sendError();
                }                
            }
            else
            {
                printf("User has been online\n");
                sendError();
            }
            break;
        case CMD_LOGIN:
            if(getThreadState() == OFFLINE)
            {
                if(clientLogin(info.nameAndPwd) == SUCCESSFUL)
                {
                    addUser(&onlineUserList, info.nameAndPwd.username, &index);
                    setThreadID(index);
                    setThreadState(ONLINE);
                    sendOK();
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
                else
                {
                    printf("Can't match\n");
                    sendError();
                }                
            }
            else
            {
                printf("User has been online\n");
                sendError();
            }
            break;
        case CMD_GETLIST:
            /* code */
            break;
        case CMD_EXIT:
            if(getThreadState() == ONLINE)
            {
                if(clientExit() == SUCCESSFUL)
                {
                    removeUser(&onlineUserList, getThreadID());
                    // 更新其他线程的ID，因为注销这个用户之后，其他用户的ID号随之更改
                    // 待补充。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
            }
            else
            {
                printf("The user had been offline\n");
                sendError();
            }            
            break;
        default:
            break;
        }

        endAnalysis(type, &info);
    }
    else if (package.type == DIALOGUE)
    {

    }
    
}

Status clientRegister(userVerify info)
{
    FILE *fp;
    if(fp = fopen("userdata.txt", "a") == NULL)
    {
        printf("clientRegister opens file error\n");
        return FAILURE;
    }
    
    if(strlen(info.username) >= NAME_MAX_LEN || strlen(info.password) >= PWD_MAX_LEN)
    {
        printf("clientRegister error: name or password is too long\n");
        return FAILURE;
    }

    fprintf(fp, "%s\t%s\n", info.username, info.password);

    if(fclose(fp) != 0)
    {
        printf("clientRegiser errors in closing file\n");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status clientLogin(userVerify info)
{
    FILE *fp;
    char tmpName[NAME_MAX_LEN];
    char tmpPwd[PWD_MAX_LEN];
    int res = FAILURE;

    if(fp = fopen("userdata.txt", "r") == NULL)
    {
        printf("clientLogin opens file error\n");
        return FAILURE;
    }

    while (fscanf(fp, "%s\t%s\n", tmpName, tmpPwd) == 1)
    {
        if(strcmp(info.username, tmpName) == 0)
            if(strcmp(info.password, tmpPwd) == 0)
                res = SUCCESSFUL;
    }
    
    if(fclose(fp) != 0)
    {
        printf("clientLogin errors in closing file\n");
        return FAILURE;
    }

    if(res == FAILURE)
    {
        printf("clientLogin: match failure, Please register first\n");
    }

    return res;
}

Status clientWannaList(userList list)
{

}

Status clientExit(void);

Status sendOK(void);
Status sendError(void);

void setThreadID(int ID)
{
    state.ID = ID;
}

void setThreadState(int onlineOrNot)
{
    state.online = onlineOrNot;
}

int getThreadID(void)
{
    return state.ID;
}

int getThreadState(void)
{
    return state.online;
}