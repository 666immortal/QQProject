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
            else
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
    int res = FAILURE;

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
                    res = addUser(&onlineUserList, info.nameAndPwd.username, getThreadID());
                    pthread_mutex_unlock(&list_mutex);
                    if(res == FAILURE)
                        break;
                    setThreadState(ONLINE);
                    sendRegister(getThreadID);
                    res = broadcastList();
                }
                else
                {
                    perror("Register failure\n");
                    sendLogin(getThreadID());
                    res = FAILURE;
                }                
            }
            else
            {
                perror("User has been online\n");
                sendLogin(getThreadID());
                res = FAILURE;
            }
            break;
        case CMD_LOGIN:
            if(getThreadState() == OFFLINE)
            {
                if(clientLogin(info.nameAndPwd) == SUCCESSFUL)
                {
                    pthread_mutex_lock(&list_mutex);
                    res = addUser(&onlineUserList, info.nameAndPwd.username, getThreadID());
                    pthread_mutex_unlock(&list_mutex);
                    if(res == FAILURE)
                        break;
                    setThreadState(ONLINE);
                    sendLogin(getThreadID());
                    res = broadcastList();
                }
                else
                {
                    perror("Can't match\n");
                    sendRegister(getThreadID());
                    res = FAILURE;
                }                
            }
            else
            {
                perror("User has been online\n");
                sendRegister(getThreadID());
                res = FAILURE;
            }
            break;
        case CMD_GETLIST:
            res = clientWannaList(getThreadID());
            //onlineUserList = info.list;
            break;
        case CMD_EXIT:
            if(getThreadState() == ONLINE)
            {                
                pthread_mutex_lock(&list_mutex);
                res = removeUser(&onlineUserList, getThreadID());
                pthread_mutex_unlock(&list_mutex);
                if(res == SUCCESSFUL)
                    res = broadcastList();
                if(clientExit(getThreadID()) == FAILURE)
                {
                    perror("Client exit error\n");
                    res = FAILURE;
                }                
            }
            else
            {
                perror("The user had been offline\n");
                sendError(getThreadID());
                res = FAILURE;
            }            
            break;
        default:
            res = FAILURE;
            printf("error command\n");
            break;
        }

        endAnalysis(type, &info);
    }
    else if (package.type == DIALOGUE)
    {

    }
    
    return res;
}

Status clientRegister(userVerify info)
{
    pthread_mutex_lock(&file_mutex);
    FILE *fp;
    if(fp = fopen("userdata.txt", "a") == NULL)
    {
        perror("clientRegister opens file error\n");
        return FAILURE;
    }
    
    if(strlen(info.username) >= NAME_MAX_LEN || strlen(info.password) >= PWD_MAX_LEN)
    {
        perror("clientRegister error: name or password is too long\n");
        return FAILURE;
    }

    fprintf(fp, "%s\t%s\n", info.username, info.password);

    if(fclose(fp) != 0)
    {
        perror("clientRegiser errors in closing file\n");
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
        perror("clientLogin opens file error\n");
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
        perror("clientLogin errors in closing file\n");
        return FAILURE;
    }
    pthread_mutex_unlock(&file_mutex);

    if(res == FAILURE)
    {
        perror("clientLogin: match failure, Please register first\n");
    }

    return res;
}

Status clientWannaList(int ID)
{
    MsgEntity tmp;
    if(configUserListEty(&tmp, &onlineUserList) == SUCCESSFUL)
    {
        sendCmd(&tmp, ID);
    }
    else
    {
        perror("clinentWannaList error: config eneity failure\n");
        return FAILURE;
    }
    
    return SUCCESSFUL;
}

Status clientExit(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_EXIT;
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

// 用户登陆时，服务器回应Login表示登陆成功，服务器回应Register表示登陆失败
Status sendLogin(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_LOGIN;
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

// 用户注册时，服务器回应Register表示注册成功，服务器回应Login表示注册失败
Status sendRegister(int ID)
{
    MsgEntity tmp;
    tmp.object = CMD_REGISTER;
    tmp.flag = SEND_FLAG;

    return sendCmd(&tmp, ID);
}

Status sendCmd(MsgEntity *ety, int ID)
{
    if(NULL == ety)
    {
        perror("sendCmd error: pointer is null\n");
        return FAILURE;
    }

    MsgContainer tmp;
    tmp.type = COMMAND;
    tmp.content = *ety;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer)) == -1)
    {
        perror("send Error!");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status sendDlg(MsgEntity *ety, int ID)
{
    if(NULL == ety)
    {
        perror("sendCmd error: pointer is null\n");
        return FAILURE;
    }

    MsgContainer tmp;
    tmp.type = DIALOGUE;
    tmp.content = *ety;

    if(send(ID, (void *)&tmp, sizeof(MsgContainer)) == -1)
    {
        perror("send Error!");
        return FAILURE;
    }

    return SUCCESSFUL;
}

Status broadcastList(void)
{
    int i;
    for(i = 0; i < onlineUserList.num; i++)
    {
        if(clientWannaList(onlineUserList.user[i].threadID) == FAILURE)
        {
            return FAILURE;
        }
    }

    return SUCCESSFUL;
}

Status broadcastDlg(void);

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