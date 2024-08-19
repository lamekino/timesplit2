EXE_NAME := timesplit2

CC := gcc

OBJ_DIR := ./obj
SRC_DIR := ./src

LDFLAGS := \
	-I$(SRC_DIR) \
	-lpthread \
	`pkg-config --cflags --libs sndfile`

CCFLAGS = \
	-std=c89 -ansi -ggdb -Wall -Wextra -Werror -Wno-unused-variable \
	-Wno-unused-parameter -Wno-unused-but-set-variable -Wno-unused-function \
	-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500 -DDEBUG=1

ifeq ($(DEBUG),)
	CCFLAGS = \
		-std=c89 -ansi -O3 -Wall -Wextra -Werror -Wno-unused-function \
		-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=500
endif

SRC := $(shell find $(SRC_DIR) -name "*.c")
OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

ifeq ($(PREFIX),)
	PREFIX := $(HOME)/.local
endif

all: $(EXE_NAME)

clean:
	rm -fr out-audio $(OBJ_DIR) $(EXE_NAME)

install: all
	install -m 0755 $(EXE_NAME) $(PREFIX)/bin

compile_commands.json:
	@bear -- make

$(EXE_NAME): $(OBJ)
	@mkdir out-audio || true
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	@mkdir -p $(dir $@) || true
	$(CC) $(CCFLAGS) $(LDFLAGS) -c -o $@ $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@) || true
	$(CC) $(CCFLAGS) $(LDFLAGS) -c -o $@ $<

.PHONY: all clean install compile_commands.json
