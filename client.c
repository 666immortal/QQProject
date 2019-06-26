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

#define CLIENT_IPADDR "172.21.128.67"
#define SERVPORT 1012

userList cast;
FILE *fp;
pthread_t t_processRecvThread;

void main()
{
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

  int select = 0;
  char username[NAME_MAX_LEN];
  char password[PWD_MAX_LEN];

while (1)
{
  printf("----------- 请先登陆/注册 -----------\n");
  printf("1.登陆\n2.注册\n");
  printf("-------------------------------\n");
  scanf("%d", &select);
  printf("请输入用户名：\n");
  scanf("%s", username);
  printf("请输入密码：\n");
  scanf("%s", password);
  MsgEntity tmp;
  if(select == 1)
  {    
    configLoginEty(&tmp, username, password);
  }
  else if(select == 2)
  {
    configRegisterEty(&tmp, username, password);
  }
  else
  {
    printf("输入有误，请重新输入\n");
    continue;
  }
  sendCmd(&tmp, sockfd);

  int bytesNum;
  MsgContainer tmpbag;
  memset((char*)&tmpbag,0,sizeof(tmpbag));
  do{
    bytesNum = recv(sockfd,(void *)&tmpbag,sizeof(MsgContainer),0);
  }while (bytesNum <= 0);

  showMsgContainer(tmpbag);
  if(tmpbag.type == COMMAND && (tmpbag.content.object) == (select - 3))
  {
    printf("成功\n");
    break;
  }
  else
  {
    printf("失败，请重试\n");
  }
}

  int res = pthread_create(&t_processRecvThread, NULL, processRecv, (void*)&sockfd);
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
    printf("1.获取用户列表\n2.文件传输\n3.发送私信\n4.群发消息\n5.退出\n");
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
      scanf("%d", &flag);
      while (searchNameInUserList(&cast, flag) == NULL)
      {
        printf("输入序号有误，请重新输入：\n");
        printf("---------- 用户列表 ------------\n");
        showUserList(cast);
        printf("----------------------------\n"); 
        scanf("%d", &flag);
      }

      printf("请输入文件名：\n");    
      scanf("%s", str);
      sendFileProcess(sockfd, flag, str);
      break;
    case 3:  // 发送私信
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
      printf("请输入要发送的消息(请不要添加空格)：\n");      
      scanf("%s", str);
      bagType = DIALOGUE;
      flag = SEND_FLAG;
      break;
    case 4:  // 群发消息
      printf("请输入要发送的消息(请不要添加空格)：\n");   
      scanf("%s", str); 
      bagType = DIALOGUE;
      object = 0;
      flag = sockfd;
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

    if(type != 2)
    {
      MsgContainer tmp;
      tmp.type = bagType;
      tmp.content.object = object;
      strcpy(tmp.content.details, str);
      tmp.content.flag =  flag;
      // printf("I send a package as follow:\n");
      // printf("----------------------\n");
      // showMsgContainer(tmp);
      // printf("----------------------\n");

      if((sendbytes = send(sockfd, (void *)&tmp, sizeof(MsgContainer), 0)) == -1)
      {
        perror("send Error!");
        exit(1);
      }
      //printf("send successful\n");
      }    
  }

  if(fp != NULL)
  {
    fclose(fp);
  }

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
        if(tmp.type == COMMAND)
        {
            char who[NAME_MAX_LEN];
            switch (tmp.content.object)
            {
            case CMD_GETLIST:
              makeUserList(tmp.content.details, &cast);
              printf("---------- 更新用户列表 ------------\n");
              showUserList(cast);
              printf("---------------------------------\n");
              break;
            case CMD_SEND_FILE:
              strcpy(who, searchNameInUserList(&cast, tmp.content.flag));              
              printf(" %s 向你发送文件 %s，为您将文件转存为copy_%s：\n", 
                            who, tmp.content.details, tmp.content.details);
              readyForRecvFile(tmp.content.details);
              break;
            case CMD_TRANSFERING:
              recvFilefrom(&tmp.content);
              break;
            case CMD_END_TRANSFER:
              finishRecvFile();
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
            printf("------------------------\n");
            printf(" %s 对全部人说: %s\n", name, tmp.content.details);
            printf("------------------------\n");
          }
          else
          {
            char *name = searchNameInUserList(&cast, tmp.content.object);
            printf("------------------------\n");
            printf("%s 对你说: %s\n", name, tmp.content.details);
            printf("------------------------\n");
          }
        }
      }
    }    
  }while (bytesNum > 0);  
}

Status sendFileProcess(const int socdID, const int sendForwho, const char *fileName)
{
  int sendBytes = 0;
  MsgContainer tmp;
  char buf[DETAILS_LEN];
  int len = 0;

  tmp.type = COMMAND;
  configSendFileCmd(&tmp.content, sendForwho, fileName);
  if((sendBytes = send(socdID, (void *)&tmp, sizeof(MsgContainer), 0)) == -1)
  {
    perror("send Error!");
    return FAILURE;
  }

  printf("文件正在发送中，请稍后...\n");

  fp = fopen(fileName, "rb");
  if(fp == NULL)
  {
    printf("Open File failure\n");
    return FAILURE;
  }

  do
  {
    memset(buf, 0, DETAILS_LEN);
    len = fread(buf, sizeof(char), DETAILS_LEN, fp);

    if(len > 0)
    {
      unsigned short tmplen = (unsigned short)len;
      unsigned short sendFor = (unsigned short)sendForwho;
      int tmpp;
      unsigned short *point = &tmpp;
      point[0] = len;
      point[1] = sendFor;
      tmp.type = COMMAND;
      tmp.content.object = CMD_TRANSFERING;
      tmp.content.flag = tmpp;

      for(int i = 0; i < len; i++)
      {
        tmp.content.details[i] = buf[i];
      }

      if((sendBytes = send(socdID, (void *)&tmp, sizeof(MsgContainer), 0)) == -1)
      {
        perror("send Error!");
        return FAILURE;
      }
    }   
      
  } while (feof(fp) == 0);
  
  tmp.type = COMMAND;
  configEndTransferCmd(&tmp.content, sendForwho);
  if((sendBytes = send(socdID, (void *)&tmp, sizeof(MsgContainer), 0)) == -1)
  {
    perror("send Error!");
    return FAILURE;
  }

  fclose(fp);
  printf("发送文件成功\n");

  return SUCCESSFUL;
}

int putStreamIntoFile(char *stream, int len)
{
    int res;
    res = fwrite(stream, sizeof(char), len, fp);

    if(feof(fp) != 0)
    {
      fclose(fp);
    }

    return res;
}

Status readyForRecvFile(char *str)
{
  char newName[FILE_NAME_MAX_LEN];
  strcpy(newName, "copy_");
  strcat(newName, str);
  fp = fopen(newName, "wb");
  if(fp == NULL)
  {
    printf("Create File failure\n");
    return FAILURE;
  }

  return SUCCESSFUL;
}

void finishRecvFile()
{
  if(fp != NULL)
    fclose(fp);
  printf("成功接收到文件\n");
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
