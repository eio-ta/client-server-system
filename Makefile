all : cassini saturnd

CC ?= gcc
CCFLAGS ?= -Wall -c

cassini: cassini.o timing-text-io.o stringBIS.o commandline.o read-pipes.o write-pipes.o utils.o
	$(CC) -o cassini cassini.o timing-text-io.o stringBIS.o commandline.o read-pipes.o write-pipes.o utils.o

saturnd: saturnd.o timing-text-io.o stringBIS.o commandline.o read-pipes.o write-pipes.o utils.o
	$(CC) -o saturnd saturnd.o timing-text-io.o stringBIS.o commandline.o read-pipes.o write-pipes.o utils.o

cassini.o: src/cassini.c 
	$(CC) $(CCFLAGS) src/cassini.c -Iinclude

saturnd.o: src/saturnd.c
	$(CC) $(CCFLAGS) src/saturnd.c -Iinclude

timing-text-io.o: src/timing-text-io.c 
	$(CC) $(CCFLAGS) src/timing-text-io.c -Iinclude

stringBIS.o: src/stringBIS.c 
	$(CC) $(CCFLAGS) src/stringBIS.c -Iinclude

commandline.o: src/commandline.c
	$(CC) $(CCFLAGS) src/commandline.c -Iinclude

read-pipes.o: src/read-pipes.c
	$(CC) $(CCFLAGS) src/read-pipes.c -Iinclude

write-pipes.o: src/write-pipes.c
	$(CC) $(CCFLAGS) src/write-pipes.c -Iinclude

utils.o: src/utils.c
	$(CC) $(CCFLAGS) src/utils.c -Iinclude

distclean:
	rm -f *.o cassini saturnd