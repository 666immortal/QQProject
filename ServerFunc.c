#include "ServerFunc.h"
#include <pthread.h>

//#define FOLDER_PATH "./fileBox/"

// 外部变量，在线用户列表
extern userList onlineUserList;
// __thread local 线程局部变量，每个线程都拥有一份对变量的拷贝
static __thread threadStatus state;
// 文件互斥锁，互斥访问文件
pthread_mutex_t file_mutex;
// 用户列表互斥锁，互斥修改列表，即使只是读取也需要互斥，防止读取的过程中被修改
pthread_mutex_t list_mutex;

#define ONLINE  1  // 上线状态
#define OFFLINE 0  // 下线状态

static void setThreadID(int ID);  // 设置线程（用户）ID
static void setThreadState(int onlineOrNot); // 设置线程（用户）状态
static int getThreadID(void); // 获取线程（用户）ID
static int getThreadState(void); // 获取线程（用户）状态

// 线程服务函数（接收数据包，处理数据包）
void *serverClient(void* arg)
{
    // 初始化文件互斥锁
    pthread_mutex_init(&file_mutex, NULL);
    // 初始化用户列表互斥锁
    pthread_mutex_init(&list_mutex, NULL);

    // 初始时为未登录状态
    state.online = OFFLINE;
    int theClient = *(int*)arg;
    int bytesNum;
    char buf[MAXBUFLEN];
    // 设置好进程的ID号
    setThreadID(theClient);

    do
    {
        memset(buf,0,sizeof(buf)); //将内存内容全部初始化为0
        // 获取数据包
        bytesNum = recv(theClient,(void *)buf,sizeof(MsgContainer),0);

        //解析数据包
        if(bytesNum == -1)
        {
            // 若数据量为-1，接收数据发生错误
            perror("recv Error:");
            if(getThreadState() == OFFLINE)
            {
                printf("The user had been offline\n");
                break;
            }

            pthread_mutex_lock(&list_mutex);
            removeUser(&onlineUserList, getThreadID());
            //showUserList(onlineUserList);
            pthread_mutex_unlock(&list_mutex);
            if(onlineUserList.num > 0)
            {
                pthread_mutex_lock(&list_mutex);
                broadcastList(&onlineUserList);
                pthread_mutex_unlock(&list_mutex);
            } 
        }
        else
        {
            // 若数据量为零，意为连接中断            
            if(bytesNum <= 0)
            {
                if(getThreadState() == OFFLINE)
                {
                    printf("The user had been offline\n");
                    break;
                }

                pthread_mutex_lock(&list_mutex);
                removeUser(&onlineUserList, getThreadID());
                //showUserList(onlineUserList);
                pthread_mutex_unlock(&list_mutex);
                if(onlineUserList.num > 0)
                {
                    pthread_mutex_lock(&list_mutex);
                    broadcastList(&onlineUserList);
                    pthread_mutex_unlock(&list_mutex);
                }                
            }
            else
            {
                // 解析数据包并执行操作
                //showMsgContainer(*(MsgContainer*)buf);
                unPack(*(MsgContainer*)buf);
            }
        }    
    }while(bytesNum>0);

    // 关闭线程
    close(theClient);
}

Status unPack(MsgContainer package)
{
    int res = FAILURE; // 返回值

    if(package.type == COMMAND) // 如果为命令包
    {
        int type;  // 存放命令类型
        Stnparser info;  // 存放命令内容

        // 解析命令包，获取命令内容
        printf("I receive a package\n");
        analysisCmdEty(&package.content,&type, &info);
        printf("parser a package successful\n");
        switch (type)
        {
        case CMD_REGISTER:  // 用户发来注册命令
            if(getThreadState() == ONLINE) // 如果已经上线，判断为错误包，丢弃
            {
                printf("User has been online\n");
                sendLogin(getThreadID());  // 发送注册失败信息到客户端
                res = FAILURE;
                break;
            }

            if(clientRegister(info.nameAndPwd) == SUCCESSFUL) // 调用用户注册函数
            {
                pthread_mutex_lock(&list_mutex);  // 给用户列表上锁
                // 向用户列表中添加用户
                res = addUser(&onlineUserList, info.nameAndPwd.username, getThreadID()); 
                //showUserList(onlineUserList);               
                pthread_mutex_unlock(&list_mutex); // 给用户列表解锁
                if(res == FAILURE) // 发生错误，退出
                    break;
                setThreadState(ONLINE); // 注册成功之后，马上登陆
                sendRegister(getThreadID()); // 发送用户注册成功消息
                pthread_mutex_lock(&list_mutex);  // 给用户列表上锁
                res = broadcastList(&onlineUserList); // 群发新的名单
                pthread_mutex_unlock(&list_mutex); // 给用户列表解锁
            }
            else
            {
                printf("Register failure\n");
                sendLogin(getThreadID());  // 发送注册失败消息
                res = FAILURE;
            }                            
            break;
        case CMD_LOGIN:
            if(getThreadState() == ONLINE)  // 如果用户已经上线，判断为错误包，丢弃
            {
                printf("User has been online\n");
                sendRegister(getThreadID());  // 发送登陆失败信息到客户端
                res = FAILURE;
                break;
            }

            if(clientLogin(info.nameAndPwd) == SUCCESSFUL) // 登陆成功
            {
                pthread_mutex_lock(&list_mutex); // 用户列表上锁
                // 向用户列表中添加用户
                res = addUser(&onlineUserList, info.nameAndPwd.username, getThreadID());
                //showUserList(onlineUserList);
                pthread_mutex_unlock(&list_mutex); // 用户列表解锁
                if(res == FAILURE) // 添加用户失败，退出
                    break;
                setThreadState(ONLINE); // 登陆成功，设置标记位
                sendLogin(getThreadID()); // 发送登陆成功消息
                pthread_mutex_lock(&list_mutex); // 群发之前，先给列表上锁
                res = broadcastList(&onlineUserList); // 群发新的名单
                pthread_mutex_unlock(&list_mutex); // 群发结束，解锁列表
            }
            else // 登陆失败，查找不到此用户
            {
                printf("Can't match\n");
                sendRegister(getThreadID()); // 用户名或密码不对，发送错误
                res = FAILURE;
            }                
            break;
        case CMD_GETLIST:
            if(getThreadState() == OFFLINE)
            {
                printf("Please login first\n");
                res = FAILURE;
                break;
            }
            res = clientWannaList(getThreadID());
            //onlineUserList = info.list;
            break;
        case CMD_EXIT:
            if(getThreadState() == OFFLINE)
            {
                printf("Recv exit cmd but the user had been offline\n");
                res = FAILURE;
                break;
            }

            pthread_mutex_lock(&list_mutex);
            res = removeUser(&onlineUserList, getThreadID());
            //showUserList(onlineUserList);
            pthread_mutex_unlock(&list_mutex);
            if(res == SUCCESSFUL && onlineUserList.num > 0)
            {
                pthread_mutex_lock(&list_mutex);
                res = broadcastList(&onlineUserList);
                pthread_mutex_unlock(&list_mutex);
            }                
            setThreadState(OFFLINE);                
            break;
        case CMD_SEND_FILE:
            if(getThreadState() == OFFLINE)
            {
                printf("Please login first\n");
                res = FAILURE;
                break;
            }
            remindForReceive(&package.content, getThreadID());
            break;
        case CMD_TRANSFERING:
            if(getThreadState() == OFFLINE)
            {
                printf("Please login first\n");
                res = FAILURE;
                break;
            }
            forwardFile(&package.content, getThreadID());
            break;
        case CMD_END_TRANSFER:
            if(getThreadState() == OFFLINE)
            {
                printf("Please login first\n");
                res = FAILURE;
                break;
            }
            remindForEnding(&package.content, getThreadID());
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
        // BOARDCAST表示群发消息
        if(package.content.object == BOARDCAST)
        {
            pthread_mutex_lock(&list_mutex);
            res = broadcastDlg(&onlineUserList, &package.content, getThreadID());
            pthread_mutex_unlock(&list_mutex);
        }
        else
        {
            res = forwardDlg(&package.content, getThreadID());
        }
        
    }
    
    return res;
}

Status clientRegister(userVerify info)
{
    if(strlen(info.username) >= NAME_MAX_LEN || strlen(info.password) >= PWD_MAX_LEN)
    {
        printf("clientRegister error: name or password is too long\n");
        return FAILURE;
    }

    if(searchInFile(info) == SUCCESSFUL)
    {
        printf("clientRegister had been registerd before\n");
        return FAILURE;
    }

    pthread_mutex_lock(&file_mutex);
    FILE *fp;
    if((fp = fopen("userdata", "a")) == NULL)
    {
        printf("clientRegister opens file error\n");
        return FAILURE;
    }

    fprintf(fp, "%s#%s\n", info.username, info.password);

    if(fclose(fp) != 0)
    {
        printf("clientRegiser errors in closing file\n");
        return FAILURE;
    }
    pthread_mutex_unlock(&file_mutex);

    return SUCCESSFUL;
}

Status searchInFile(userVerify info)
{
    pthread_mutex_lock(&file_mutex);
    FILE *fp;
    char tmp[NAME_MAX_LEN+PWD_MAX_LEN+1];
    char tmpName[NAME_MAX_LEN];
    char tmpPwd[PWD_MAX_LEN];
    int res = FAILURE;

    if((fp = fopen("userdata", "r+")) == NULL)
    {
        printf("searchInFile opens file error\n");
        return FAILURE;
    }

    rewind(fp);

    while (fscanf(fp, "%s", tmp) == 1)
    {        
        int i = 0;
        while (tmp[i] != '#')
        {
            tmpName[i] = tmp[i];
            i++;
        }

        tmpName[i] = '\0';
        i++;
        strcpy(tmpPwd, &tmp[i]);   
        
        if(strcmp(info.username, tmpName) == 0)
            if(strcmp(info.password, tmpPwd) == 0)
            {
                res = SUCCESSFUL;
                break;
            }              
    }
    
    if(fclose(fp) != 0)
    {
        printf("searchInFile errors in closing file\n");
        return FAILURE;
    }
    pthread_mutex_unlock(&file_mutex);

    return res;
}

Status clientLogin(userVerify info)
{
    return searchInFile(info);
}

Status clientWannaList(int ID)
{
    MsgEntity tmp;
    int res;
    pthread_mutex_lock(&list_mutex);
    res = configUserListEty(&tmp, &onlineUserList);
    pthread_mutex_unlock(&list_mutex);

    if(res == SUCCESSFUL)
    {
        res = sendCmd(&tmp, ID);
    }
    else
    {
        printf("clinentWannaList error: config eneity failure\n");
        res = FAILURE;
    }
    
    return res;
}

static void setThreadID(int ID)
{
    state.ID = ID;
}

static void setThreadState(int onlineOrNot)
{
    state.online = onlineOrNot;
}

static int getThreadID(void)
{
    return state.ID;
}

static int getThreadState(void)
{
    return state.online;
}