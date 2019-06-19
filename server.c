#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include <pthread.h>

#include "ServerFunc.h"
#include "UserList.h"

#define SERVER_IPADDR "127.0.0.1"
#define SERVPORT 1012 /*server listening port*/
#define BACKLOG 10 /*allow total number of connection*/

userList onlineUserList;

int main()
{
  int sockfd,client_fd; /*sock_fd: listening Socket;sock_fd: Data Transfer Socket*/
  struct sockaddr_in my_addr; /*local PC address info*/
  struct sockaddr_in remote_addr; /*remote PC address info */
  
  initUserList(&onlineUserList);

  if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1)
  {
    perror("create Socket Error!");
    exit(1);
  }
  
  bzero(&my_addr,sizeof(my_addr));
  my_addr.sin_family = AF_INET;  /*there is a quescstion here:as I have two network card*/
  my_addr.sin_port = htons(SERVPORT);
  my_addr.sin_addr.s_addr = inet_addr(SERVER_IPADDR);/*IP Address  Also has a quesstion*/
  // bzero(&(my_addr.sin_zero),8);

  int i_bind_return = bind(sockfd,(struct sockaddr *)&my_addr,sizeof(my_addr));
  printf("bind func return: %d\n", i_bind_return);
   
  if ( i_bind_return == -1 )
  {
     perror("bind error");
     exit(1);
  }

  if(listen(sockfd,BACKLOG) == -1)
  {
      perror("listen Error!");
      exit(1);
  }

  while(1)
  {
    // printf("waiting for remote connection \n");
    int sin_size = sizeof(struct sockaddr_in);
    if((client_fd = accept(sockfd,(struct sockaddr *)&remote_addr,&sin_size))==-1)
    {
      perror("accept Error");
      continue;
    }
    // char *s_remote_addr = inet_ntoa(remote_addr.sin_addr);
    printf("received a connection from %s\n",inet_ntoa(remote_addr.sin_addr));

    pthread_t tid;

    int res = pthread_create(&tid, NULL, serverClient, (void*)&client_fd);
      
    if(res != 0)
    {
      printf("error!%d\n", res);
      exit(1);
    }
  }

  deleteUserList(&onlineUserList);
}