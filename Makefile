OBJS=test.o MsgStruct.o MsgParser.o
CC=gcc
CFLAGS=-c -g

test:$(OBJS)
	$(CC) $(OBJS) -o test

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf test *.o
