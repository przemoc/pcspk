CC = gcc
OBJS = str_en.o alloc.o error.o

default : pbase.o

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

str_en.o : str_en.c str_config.h
	$(CC) $(CFLAGS) -c -o $@ $< -include str_config.h

pbase.o :	$(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -r -o $@

clean : 
	$(RM) $(OBJS) pbase.o
