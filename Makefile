CC = gcc
CLFAGS = -Wall -g
MONITOR = monitor
MONITOR_C = monitor.c
MONITOR_O = monitor.o
LIBMONITOR = libmonitor.a
LIB_H = libmonitor.h
LIB_C = libmonitor.c
LIB_O = libmonitor.o

.SUFFIXES: .c .o

all: $(MONITOR) producer consumer $(LIBMONITOR)

$(MONITOR): $(MONITOR_O) $(LIBMONITOR)
	$(CC) -lm -o $@ $(MONITOR_O) -L . -lmonitor -lpthread -lrt 

producer: producer.o $(LIBMONITOR)
	$(CC) -lm -o $@ producer.o -L . -lmonitor

consumer: consumer.o $(LIBMONITOR)
	$(CC) -lm -o $@ consumer.o -L . -lmonitor

.c.o:
	$(CC) -c $(CFLAGS) $<

$(LIBMONITOR): $(LIB_O)
	ar rc $(LIBMONITOR) $(LIB_O)

libs: $(LIBMONITOR)

.PHONY: clean
clean:
	/bin/rm -f $(MONITOR) producer consumer  *.o *.a logfile
