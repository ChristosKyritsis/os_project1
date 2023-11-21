CC = gcc
CFLAGS = -Wall -g -pthread
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

SOURCES = $(SRC_DIR)/procA.c $(SRC_DIR)/procB.c $(INCLUDE_DIR)/inc.c
OBJECTS = $(BUILD_DIR)/procA.o $(BUILD_DIR)/procB.o $(BUILD_DIR)/inc.o
TARGET = $(BIN_DIR)/my_program

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/procA.o: $(SRC_DIR)/procA.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(BUILD_DIR)/procB.o: $(SRC_DIR)/procB.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

run: $(TARGET)
	./$(TARGET) $(ARGS)


.PHONY: all clean
