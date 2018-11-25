SRC:=src
TEST:=test
BLD:=build
OBJ:=$(BLD)/objects
TEST_OBJ:=$(OBJ)/test
DIRS:=$(BLD) $(OBJ) $(TEST_OBJ)

SCT_VERSION:=0.0.0

OBJECTS:=exception sll arr slice hash
TEST_OBJS:=sll arr hash
SRC_HDR:=$(OBJECTS) sct_internal sct
TEST_HDR:=test

CFLAGS+=-Wall -Wextra -std=gnu11 -fPIC
CFLAGS+=-DSCT_VERSION=\"$(SCT_VERSION)\"
# CFLAGS+=-D_SCT_SUPPRESS_EXCEPTIONS

TEST_CFLAGS:=-I$(SRC)
TEST_CFLAGS+=-Og -g3 -Wno-unused-variable

ifeq ($(DEBUG), 1)
	INFO:=$(info $(origin DEBUG): DEBUG=1)
	CFLAGS+=-Og -g3
	CFLAGS+=-D_SCT_DEBUG
	VFLAGS+=-v --leak-check=full --track-origins=yes --show-leak-kinds=all
endif
