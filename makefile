CC = gcc
CFLAGS = -Wall -pthread

SRCS = procA.c 
OBJS = $(SRCS:.c=.o)
HEADERS = 
TARGET = main_prog
BINDIR = bin

$(BINDIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(BINDIR)/$(TARGET)