CC = gcc
CFLAGS = -Wall -g -pthread
LDFLAGS = -lrt
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
INCLUDE_DIR = include

SOURCES = $(SRC_DIR)/procA.c $(SRC_DIR)/procB.c $(INCLUDE_DIR)/inc.c
OBJECTS = $(BUILD_DIR)/procA.o $(BUILD_DIR)/procB.o $(BUILD_DIR)/inc.o
TARGET_A = $(BIN_DIR)/procA
TARGET_B = $(BIN_DIR)/procB

all: $(TARGET_A) $(TARGET_B)

$(TARGET_A): $(BUILD_DIR)/procA.o $(BUILD_DIR)/inc.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(TARGET_B): $(BUILD_DIR)/procB.o $(BUILD_DIR)/inc.o
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/procA.o: $(SRC_DIR)/procA.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(BUILD_DIR)/procB.o: $(SRC_DIR)/procB.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)

$(BUILD_DIR)/inc.o: $(INCLUDE_DIR)/inc.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INCLUDE_DIR)


clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
