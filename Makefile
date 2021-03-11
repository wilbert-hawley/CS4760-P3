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
	$(CC) -lm -o $@ $(MONITOR_O) -L . -lmonitor 

producer: producer.o
	$(CC) -o $@ producer.o

consumer: consumer.o
	$(CC) -o $@ consumer.o

.c.o:
	$(CC) -c $(CFLAGS) $<

$(LIBMONITOR): $(LIB_O)
	ar rc $(LIBMONITOR) $(LIB_O)

libs: $(LIBMONITOR)

.PHONY: clean
clean:
	/bin/rm -f $(MONITOR) producer consumer  *.o *.a logfile
