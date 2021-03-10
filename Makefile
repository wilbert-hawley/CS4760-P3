CC = gcc
OLFAGS = -Wall -g
MONITOR = monitor
MONITOR_C = monitor.c
MONITOR_O = monitor.o
LIBMONITOR = libmonitor.a
LIB_H = libmonitor.h
LIB_C = libmonitor.h
LIB_O = libmonitor.o

all: $(MONITOR)

$(MONITOR): $(MONITOR_O) $(LIBMONITOR)
	$(CC) -lm -o $(MONITOR) $(MONITOR_O) -L. 

$(MONITOR_O): $(MONITOR_C) 
	$(CC) $(OFLAGS) -c $(MONITOR_C) -o $(MONITOR_O)

$(LIB_O): $(LIB_C) $(LIB_H)
	$(CC) $(OFLAGS) -c $(LIB_C) -o $(LIB_O)

$(LIBMONITOR): $(LIB_O)
	ar rcs $(LIBMONITOR) $(LIB_O)

libs: $(LIBMONITOR)

# %.o: %.c 
# #        $(CC) $(OFLAGS) -c $*.c -o $*.o

.PHONY: clean
clean:
	/bin/rm -f $(MONITOR) *.o *.a
