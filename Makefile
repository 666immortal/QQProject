CLI_OBJS=client.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o SendFunc.o
SER_OBJS=server.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o SendFunc.o ServerFunc.o
CC=gcc
CFLAGS=-c

all:server client

server:$(SER_OBJS)
	$(CC) $(SER_OBJS) -o server -lpthread

client:$(CLI_OBJS)
	$(CC) $(CLI_OBJS) -o client -lpthread

%.o:%.c
	$(CC) $(CFLAGS) -std=c99 $< -o $@

clean:
	rm -rf *.o
