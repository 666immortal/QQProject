OBJS=test_UserList.o MsgStruct.o MsgParser.o UserList.o MsgPackager.o UserVerify.o
CC=gcc
CFLAGS=-c -g

test_Parser:$(OBJS)
	$(CC) $(OBJS) -o test_UserList

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf test_UserList *.o
