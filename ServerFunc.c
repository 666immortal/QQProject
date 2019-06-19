#include "ServerFunc.h"
#include <pthread.h>

// 外部变量，在线用户列表
extern userList onlineUserList;
// __thread local 线程局部变量，每个线程都拥有一份对变量的拷贝
static __thread threadStatus state;
// 文件互斥锁，互斥访问文件
pthread_mutex_t file_mutex;
// 列表互斥锁，互斥修改列表，即使只是读取也需要互斥，防止读取的过程中被修改
pthread_mutex_t list_mutex;

#define ONLINE  1  // 上线状态
#define OFFLINE 0  // 下线状态

void setThreadID(int ID);  // 设置线程（用户）ID
void setThreadState(int onlineOrNot); // 设置线程（用户）状态
int getThreadID(void); // 获取线程（用户）ID
int getThreadState(void); // 获取线程（用户）状态

// 线程服务函数（接收数据包，处理数据包）
void *serverClient(void* arg)
{
    // 初始化文件互斥锁
    pthread_mutex_init(&file_mutex, NULL);
    // 初始化列表互斥锁
    pthread_mutex_init(&list_mutex, NULL);

    state.online = FAILURE;
    state.ID = -1;
    int theClient = *(int*)arg;
    int bytesNum;
    char buf[MAXBUFLEN];

    /* if(send(client_fd,"Hello,you are connected!\n",26,0) == -1)
    {
        perror("send Error!");
    }*/
    // 设置好进程的ID号
    setThreadID(theClient);

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
                    pthread_mutex_lock(&list_mutex);
                    addUser(&onlineUserList, info.nameAndPwd.username, getThreadID());
                    pthread_mutex_unlock(&list_mutex);
                    setThreadState(ONLINE);
                    sendOK(getThreadID());
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
                else
                {
                    printf("Register failure\n");
                    sendError(getThreadID());
                }                
            }
            else
            {
                printf("User has been online\n");
                sendError(getThreadID());
            }
            break;
        case CMD_LOGIN:
            if(getThreadState() == OFFLINE)
            {
                if(clientLogin(info.nameAndPwd) == SUCCESSFUL)
                {
                    pthread_mutex_lock(&list_mutex);
                    addUser(&onlineUserList, info.nameAndPwd.username, getThreadID());
                    pthread_mutex_unlock(&list_mutex);
                    setThreadState(ONLINE);
                    sendOK(getThreadID());
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
                else
                {
                    printf("Can't match\n");
                    sendError(getThreadID());
                }                
            }
            else
            {
                printf("User has been online\n");
                sendError(getThreadID());
            }
            break;
        case CMD_GETLIST:
            onlineUserList = info.list;
            break;
        case CMD_EXIT:
            if(getThreadState() == ONLINE)
            {
                if(clientExit() == SUCCESSFUL)
                {
                    pthread_mutex_lock(&list_mutex);
                    removeUser(&onlineUserList, getThreadID());
                    pthread_mutex_unlock(&list_mutex);
                    // 更新其他线程的ID，因为注销这个用户之后，其他用户的ID号随之更改
                    // 待补充。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。。
                    // 更新所有用户列表
                    // 待补充。。。。。。。。。。。。。。。。通过群发命令包更新
                }
            }
            else
            {
                printf("The user had been offline\n");
                sendError(getThreadID());
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
    pthread_mutex_lock(&file_mutex);
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
    pthread_mutex_unlock(&file_mutex);

    return SUCCESSFUL;
}

Status clientLogin(userVerify info)
{
    pthread_mutex_lock(&file_mutex);
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
    pthread_mutex_unlock(&file_mutex);

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

Status sendCmd(MsgEntity *ety, int ID);
Status sendDlg(MsgEntity *ety, int ID);

Status sendOK(int ID);
Status sendError(int ID);

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