SRC:=src
TEST:=test
BLD:=build
OBJ:=$(BLD)/objects
TEST_OBJ:=$(OBJ)/test
DIRS:=$(BLD) $(OBJ) $(TEST_OBJ)

SCT_VERSION:=0.0.0

OBJECTS:=sll
TEST_OBJS:=$(OBJECTS)
SRC_HDR:=$(OBJECTS)
TEST_HDR:=test

CFLAGS+=-Wall -Wextra -std=gnu11
CFLAGS+=-DSCT_VERSION=\"$(SCT_VERSION)\"

TEST_CFLAGS:=-I$(SRC)
TEST_CFLAGS+=-Og -g3 -Wno-unused-variable

ifeq ($(DEBUG), 1)
	INFO:=$(info $(origin DEBUG): DEBUG=1)
	CFLAGS+=-Og -g3
	CFLAGS+=-D_SCT_DEBUG
	VFLAGS+=-v --leak-check=full --track-origins=yes --show-leak-kinds=all
endif
