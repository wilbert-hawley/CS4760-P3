CC = gcc
CLFAGS = -Wall -g
MONITOR = monitor
MONITOR_C = monitor.c
MONITOR_O = monitor.o

all: $(MONITOR)

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

$(MONITOR): $(MONITOR_O)
	$(CC) $(CLFAGS) $(MONITOR_O) -o $(MONITOR)

.PHONY: clean
clean:
	/bin/rm -f $(MONITOR) *.o
