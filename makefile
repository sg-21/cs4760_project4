CC = gcc
CFLAGS = -g -Wall -lm

OSS = oss
USER = user

all: oss user

oss: oss.o
	$(CC) $(CFLAGS) -o $(OSS) oss.o

oss.o: oss.c oss.h
	$(CC) $(CFLAGS) -c oss.c

user: user.o
	$(CC) $(CFLAGS) -o $(USER) user.o

user.o: user.c
	$(CC) $(CFLAGS) -c user.c

clean:
	rm *.o oss user ossLog.txt
