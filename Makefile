CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = $(HOME)/bulk/builds/TEST_BUILD
OBJ_DIR = $(HOME)/bulk/builds/budgetTUI

SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -f $(OBJS) $(TARGET)
