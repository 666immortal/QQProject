CLI_OBJS=client.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o SendFunc.o
SER_OBJS=server.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o SendFunc.o ServerFunc.o
CC=gcc
CFLAGS=-c

all:server client

client:$(CLI_OBJS)
	$(CC) $(CLI_OBJS) -o client -lpthread

server:$(SER_OBJS)
	$(CC) $(SER_OBJS) -o server -lpthread

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf server client *.o
