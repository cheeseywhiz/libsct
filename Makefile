include config.mk

OBJECTS:=$(addprefix $(OBJ)/,$(addsuffix .o,$(OBJECTS)))
TEST_OBJS:=$(addprefix $(TEST_OBJ)/,$(addsuffix .o,$(TEST_OBJS)))
SRC_HDR:=$(addprefix $(SRC)/,$(addsuffix .h,$(SRC_HDR)))
TEST_HDR:=$(addprefix $(TEST)/,$(addsuffix .h,$(TEST_HDR)))

AUTOLINK+=-Wl,-rpath=$(BLD),-rpath-link=$(BLD)

ifeq ($(DEBUG), 1)
	VALGRIND:=-
else
	VALGRIND:=
endif

HAS_VALGRIND:=$(shell command -v valgrind 2>/dev/null)

ifdef HAS_VALGRIND
	VALGRIND+=valgrind $(VFLAGS)
else
	INFO:=$(info NOT FOUND: valgrind)
endif

INFO:=$(info $(origin CC): CC=$(CC))

.PHONY: all
all: dirs $(BLD)/libsct.so

$(DIRS):
	mkdir -p $@

.PHONY: dirs
dirs: $(DIRS)

.PHONY: clean
clean:
	rm -rf $(shell cat .gitignore)

$(OBJ)/%.o: $(SRC)/%.c $(SRC_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BLD)/libsct.so: $(OBJECTS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(TEST_OBJ)/%.o: $(TEST)/%.c $(TEST_HDR) $(SRC_HDR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BLD)/libtest.so: CFLAGS+=$(TEST_CFLAGS)
$(BLD)/libtest.so: $(TEST_OBJS)
	$(CC) $(CFLAGS) -shared -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(BLD)/test: $(TEST)/test.c $(BLD)/libsct.so $(BLD)/libtest.so
	$(CC) $(CFLAGS) $(TEST_CFLAGS) $(AUTOLINK) -o $@ $< $(LDFLAGS) -L$(BLD) $(LDLIBS) -ltest -lsct

.PHONY: valgrind
valgrind:
	$(VALGRIND) $(V_CMD)

.PHONY: testsct
testsct: V_CMD:=$(BLD)/test
testsct: $(BLD)/test valgrind

.PHONY: test
test: dirs testsct
