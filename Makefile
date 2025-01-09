EXE_NAME := timesplit2

CC := gcc
COMPILE_DEBUG := 1

OBJ_DIR := ./obj
SRC_DIR := ./src

CPPFLAGS := \
	-I$(SRC_DIR) \
	-D_POSIX_C_SOURCE=200809L \
	-D_XOPEN_SOURCE=500 \
	-DDEBUG=1

LDFLAGS := -lpthread `pkg-config --cflags --libs sndfile`
CCFLAGS := -std=c89 -ansi -Wall -Wextra -Werror


ifdef COMPILE_DEBUG
	CCFLAGS += -ggdb -Wno-unused-variable -Wno-unused-parameter \
			   -Wno-unused-but-set-variable -Wno-unused-function
else
	CCFLAGS += -O3
endif

SRC := $(shell find $(SRC_DIR) -name "*.c")
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

ifeq ($(PREFIX),)
	PREFIX := $(HOME)/.local
endif

all: $(OBJ)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(EXE_NAME) $^

clean:
	rm -fr out-audio $(OBJ_DIR) $(EXE_NAME)

install: all
	install -m 0755 $(EXE_NAME) $(PREFIX)/bin

compile_commands.json:
	@bear -- make

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	@mkdir -p $(dir $@) || true
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) $(CCFLAGS) $(CPPFLAGS) $(LDFLAGS) -c -o $@ $<

.PHONY: all clean install compile_commands.json
