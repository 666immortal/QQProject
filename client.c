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
      perror("connect Error!");
      exit(1);
  }

  pthread_t tid;

  int res = pthread_create(&tid, NULL, processRecv, (void*)&sockfd);
  if(res != 0)
  {
    printf("error!%d\n", res);
    exit(1);
  }
  
  while(1)
  {
    int type;
    int type_type;
    char str[200];
    int flag;

    printf("Please input data to send to server:\n");
    printf("类型：\n");
    scanf("%d", &type);
    printf("类型的类型：\n");
    scanf("%d", &type_type);
    printf("内容: \n");
    //s_gets(str, DETAILS_LEN);
    scanf("%s", str);
    printf("标志: \n");
    scanf("%d", &flag);

    MsgContainer tmp;
    tmp.type = type;
    tmp.content.object = type_type;
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
              showUserList(cast);
              break;
            case CMD_SEND_FILE:  // 接收文件指令
              etyPoint = &tmp.content;
              pthread_t tid;
              pthread_create(&tid, NULL, recvFileThread, NULL);
              break;
            case CMD_TRANSFERING:
              etyPoint = &tmp.content;
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
  // fp指针创建文件
  if(readyForRecvFile(etyPoint->details))
  {
    clientSendReady(etyPoint->flag);

    do
    {
      pthread_mutex_lock(&conLock);
      pthread_cond_wait(&threadCon, &conLock);
      len = recvFilefrom(etyPoint);
      pthread_mutex_unlock(&conLock);
      if(len != etyPoint->flag)
      {
        printf("recv file segment error: send and recv not match\n");
      }
    }while (len == DETAILS_LEN && etyPoint->details[DETAILS_LEN - 1] != EOF);

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

    while(sendFileFor(ID) == DETAILS_LEN);

    pthread_mutex_unlock(&readyLock);
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
    printf("Create File failure\n");
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
