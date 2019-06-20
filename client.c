#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include <pthread.h>

#include "MsgStruct.h"
#include "MsgParser.h"
#include "MsgPackager.h"

#define CLIENT_IPADDR "127.0.0.1"
#define SERVPORT 1012
#define MAXDATASIZE 100;

char *s_gets(char *st, int n);
void *processRecv(void *arg);

void main()
{
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
          break;
      }
      else
      {
        printf("--------------------------\n");
        printf("I receive a package:\n");
        showMsgContainer(tmp);
        printf("--------------------------\n");
      }
    }    
  }while (bytesNum > 0);  
}

char *s_gets(char *st, int n)
{
  char *ret_val;
  int  i = 0;

  ret_val = fgets(st, n, stdin);
  if(ret_val)
  {
    while(st[i] != '\n' && st[i] != '\0')
      i++;
    if(st[i] == '\n')
      st[i] = '\0';
    else
      while(getchar() != '\n')
        continue;
  }
  return ret_val;
}