CC = gcc
CFLAGS = -Wall -pthread

SRCS = main.c thread_func.c
OBJS = $(SRCS:.c=.o)
HEADERS = thread_func.h
TARGET = main_prog

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)