CC=gcc

include ../../config.mk

all: latency

latency:
	$(CC) $(CFLAGS) -I../../include -L../.. -Wl,-rpath,../.. -o latency latency.c -lpthread_workqueue -lpthread -lrt

macosx:
	$(CC) $(CFLAGS) -I../../include -L../.. -R../.. -o latencym latency.c -lpthread

check: latency
	LD_LIBRARY_PATH=../..:/usr/sfw/lib/amd64 ./latency

clean:
	rm -f latency
	rm -f latencym
