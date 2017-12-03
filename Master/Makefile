
all: master.c packet_counter.c
	gcc -g -Wall -o master master.c packet_counter.c -lpcap -lpthread

clean:
	rm -rf *.o master
