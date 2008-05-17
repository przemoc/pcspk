CC = gcc
OBJS = popts.o

default : popts.o

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean : 
	$(RM) $(OBJS)
