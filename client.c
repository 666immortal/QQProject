#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>

#define CLIENT_IPADDR "127.0.0.1"
#define SERVPORT 1012

userList cast;
FILE *fp;
MsgEntity *etyPoint; 
pthread_mutex_t fileLock;
pthread_mutex_t conLock;
pthread_cond_t threadCon;
pthread_cond_t readyCon;
pthread_cond_t readyLock;

void main()
{
  pthread_mutex_init(&fileLock, NULL);
  pthread_mutex_init(&conLock, NULL);
  pthread_mutex_init(&readyLock, NULL);
  pthread_cond_init(&threadCon, NULL);
  pthread_cond_init(&readyCon, NULL);
  initUserList(&cast);
  int sockfd,sendbytes;
  char buf[100];
  struct sockaddr_in serv_addr;

  if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
  {
      perror("create socket Error!");
      exit(1);
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVPORT);
  serv_addr.sin_addr.s_addr = inet_addr(CLIENT_IPADDR);
  bzero(&(serv_addr.sin_zero),8);
  if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))==-1)
  {
      perror("network connect Error!");
      exit(1);
  }


  printf("----------- 请先登陆/注册 -----------\n");
  printf("1.登陆\n2.注册\n");
  printf("-------------------------------\n");


  pthread_t tid;

  int res = pthread_create(&tid, NULL, processRecv, (void*)&sockfd);
  if(res != 0)
  {
    printf("create thread in main error!%d\n", res);
    exit(1);
  }

  int cycle = 1;
  while(cycle)
  {
    int type;
    int bagType;
    int object;
    char str[DETAILS_LEN];
    int flag;

    printf("----------- 请输入命令 -----------\n");
    printf("1.获取用户列表\n2.文件传输\n3.发送私信\n4.群发消息\n5.退出");
    printf("-------------------------------\n");

    scanf("%d", &type);
    switch (type)
    {
    case 1:  // 获取用户列表   
      bagType = COMMAND;
      object = CMD_GETLIST;
      strcpy(str, "");
      flag = SEND_FLAG;
      break;
    case 2:  // 文件传输
      printf("请输入文件的发送对象（输入用户序号）：\n");
      scanf("%d", &object);
      while (searchNameInUserList(&cast, object) == NULL)
      {
        printf("输入序号有误，请重新输入：\n");
        printf("---------- 用户列表 ------------\n");
        showUserList(cast);
        printf("----------------------------\n");
        scanf("%d", &object);
      }

      printf("请输入文件名：\n");
      scanf("%s", str);
      bagType = COMMAND;
      object = CMD_SEND_FILE;
      strcpy(str, "");
      flag = object;
      pthread_t tid;
      pthread_create(&tid, NULL, sendFileThread, (void*)object);
      break;
    case 3:  // 发送私信
      printf("请输入信息的发送对象（输入用户序号）：\n");
      scanf("%d", &flag);
      while (searchNameInUserList(&cast, flag) == NULL)
      {
        printf("输入序号有误，请重新输入：\n");
        printf("---------- 用户列表 ------------\n");
        showUserList(cast);
        printf("----------------------------\n");
        scanf("%d", &flag);
      }
      printf("请输入要发送的消息：\n");
      fgets(str, DETAILS_LEN, stdin);
      bagType = DIALOGUE;
      object = 0;
      break;
    case 4:  // 群发消息
      printf("请输入信息的发送对象（输入用户序号）：\n");
      scanf("%d", &object);
      while (searchNameInUserList(&cast, object) == NULL)
      {
        printf("输入序号有误，请重新输入：\n");
        printf("---------- 用户列表 ------------\n");
        showUserList(cast);
        printf("----------------------------\n");
        scanf("%d", &object);
      }
      printf("请输入要发送的消息：\n");
      fgets(str, DETAILS_LEN, stdin);
      bagType = DIALOGUE;
      flag = SEND_FLAG;
      break;
    case 5:  // 退出
      object = CMD_EXIT;
      strcpy(str, "");
      flag = SEND_FLAG;
      cycle = 0;
      break;    
    default:
      break;
    }

    MsgContainer tmp;
    tmp.type = type;
    tmp.content.object = object;
    strcpy(tmp.content.details, str);
    tmp.content.flag =  flag;

    showMsgContainer(tmp);

    if((sendbytes = send(sockfd, (void *)&tmp, sizeof(MsgContainer), 0)) == -1)
    {
      perror("send Error!");
      exit(1);
    }
  }

  pthread_mutex_destroy(&conLock);
  pthread_mutex_destroy(&fileLock);
  pthread_cond_destroy(&threadCon);
  deleteUserList(&cast);
}

void *processRecv(void *arg)
{
  int socket = *(int *)arg;
  int bytesNum;
  MsgContainer tmp;
  do
  {
    memset((char*)&tmp,0,sizeof(tmp));
    bytesNum = recv(socket,(void *)&tmp,sizeof(MsgContainer),0);
    if(bytesNum == -1)
    {
        perror("recv Error:");
    }
    else
    {          
      if(bytesNum <= 0)
      {
          printf("与服务器断开连接\n");
          break;
      }
      else
      {
        printf("--------------------------\n");
        printf("I receive a package:\n");
        showMsgContainer(tmp);
        printf("--------------------------\n");
        if(tmp.type == COMMAND)
        {
            switch (tmp.content.object)
            {
            case CMD_GETLIST:
              makeUserList(tmp.content.details, &cast);
              printf("---------- 更新用户列表 ------------\n");
              showUserList(cast);
              printf("----------           ------------\n");
              break;
            case CMD_SEND_FILE:  // 接收文件指令
              printf(" %s 向你发送文件 %s，按下回车键接收：", 
                                tmp.content.flag, tmp.content.details);
              pthread_t tid;
              etyPoint = (MsgEntity*)malloc(sizeof(MsgEntity));
              *etyPoint = tmp.content;
              pthread_create(&tid, NULL, recvFileThread, NULL);
              break;
            case CMD_TRANSFERING:
              etyPoint = (MsgEntity*)malloc(sizeof(MsgEntity));
              *etyPoint = tmp.content;
              pthread_mutex_lock(&conLock);
              pthread_cond_signal(&threadCon);
              pthread_mutex_unlock(&conLock);
              break;
            case CMD_READY:
              pthread_mutex_lock(&readyLock);
              pthread_cond_signal(&readyCon);
              pthread_mutex_unlock(&readyLock);
              break;
            default:
              break;
            }
        }
        else if(tmp.type == DIALOGUE)
        {
            if(tmp.content.object == 0)
            {
              char *name = searchNameInUserList(&cast, tmp.content.flag);
              printf("------------------------");
              printf(" %s 对全部人说: %s\n", name, tmp.content.details);
              printf("------------------------");
            }
            else
            {
              char *name = searchNameInUserList(&cast, tmp.content.object);
              printf("------------------------");
              printf("%s 对你说: %s\n", name, tmp.content.details);
              printf("------------------------");
            }
            
        }
      }
    }    
  }while (bytesNum > 0);  
}

void *recvFileThread(void *arg)
{
  int len = 0;
  int lenShouldBe = 0;
  char lastChar;
  // fp指针创建文件
  if(readyForRecvFile(etyPoint->details))
  {   
    clientSendReady(etyPoint->flag);
    free(etyPoint);
    do
    {
      // 因为要释放内存，所以提前把需要的值取出
      lenShouldBe = etyPoint->flag;
      lastChar = etyPoint->details[DETAILS_LEN - 1];
      pthread_mutex_lock(&conLock);
      pthread_cond_wait(&threadCon, &conLock);
      len = recvFilefrom(etyPoint);
      free(etyPoint);
      pthread_mutex_unlock(&conLock);
      if(len != lenShouldBe)
      {
        printf("recv file segment error: send and recv not match\n");
      }
    }while (len == DETAILS_LEN && lastChar != EOF);

    finishFileTask();
  }
}

void *sendFileThread(void *arg)
{
  int ID = *(int*)arg;

  if(readyForSendFile(etyPoint->details) == SUCCESSFUL)
  {
    pthread_mutex_lock(&readyLock);
    pthread_cond_wait(&readyCon, &readyLock);
    pthread_mutex_unlock(&readyLock);
    while(sendFileFor(ID) == DETAILS_LEN);    
    finishFileTask();
  }  
}

int putStreamIntoFile(char *stream, int len)
{
    int res;
    res = fwrite(stream, sizeof(char), len, fp);

    return res;
}

// 返回的就是读取到的长度
int getStreamFromFile(char *stream)
{
  int len = 0;
  len = fread(stream, sizeof(char), DETAILS_LEN, fp);

  return len;
}

Status readyForRecvFile(char *str)
{
  pthread_mutex_lock(&fileLock);
  fp = fopen(str, "wb");
  if(fp == NULL)
  {
    printf("Create File failure\n");
    return FAILURE;
  }

  return SUCCESSFUL;
}

Status readyForSendFile(char *str)
{
  pthread_mutex_lock(&fileLock);
  fp = fopen(str, "rb");
  if(fp == NULL)
  {
    printf("Open File failure\n");
    return FAILURE;
  }  

  return SUCCESSFUL;
}

void finishFileTask()
{
    fclose(fp);
    pthread_mutex_unlock(&fileLock);
}

int sendFileFor(int ID)
{
    int len;    
    MsgEntity tmp;
    tmp.object = CMD_TRANSFERING;
    len = getStreamFromFile(tmp.details);
    tmp.flag = len;
    sendCmd(&tmp, ID);  

    return len;
}

int recvFilefrom(MsgEntity *ety)
{
    int len = 0;
    if(NULL == ety)
    {
      return 0;
    }

    len = putStreamIntoFile(ety->details, ety->flag);

    return len;
}
