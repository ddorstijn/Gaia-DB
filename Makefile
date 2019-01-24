CC = gcc
OS := $(shell uname)

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

test_src = $(wildcard src/test/*.c)
test_obj = $(test_src:.c=.o)

DEBUG ?= 0

CFLAGS = 
LDFLAGS = 

ifeq ($(DEBUG), 1)
CFLAGS += -g -O0
else
CFLAGS += -O3
endif

ifeq ($(OS), Linux)
LDFLAGS += -ldb
default: linux
else
CFLAGS += -Iinclude -Llib
LDFLAGS += -llibdb181
default: win32
endif

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o 

win32: CFLAGS += -shared -DBUILD_GAIADB_DLL
win32: $(obj)
	$(CC) $(CFLAGS) -o bin/libgaiadb.dll $^ $(LDFLAGS)

linux: CFLAGS += -shared -fPIC
linux: $(obj)
	$(CC) $(CFLAGS) -o bin/libgaiadb.so $^ $(LDFLAGS)

test: $(obj) $(test_obj)
	$(CC) $(CFLAGS) -o bin/gaiadb $^ $(LDFLAGS)

clean:
	rm -f $(obj) $(test_obj) 

.PHONY: default linux win32 test clean
