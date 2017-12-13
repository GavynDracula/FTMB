
all: backup.c
	gcc -g -Wall -o backup backup.c packet_counter.c -lpcap -lpthread

clean:
	rm -rf *.o backup
