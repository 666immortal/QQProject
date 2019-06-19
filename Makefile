OBJS=server.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o SendFunc.o ServerFunc.o
CC=gcc
CFLAGS=-c

test_Parser:$(OBJS)
	$(CC) $(OBJS) -o server -lpthread

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf server *.o
