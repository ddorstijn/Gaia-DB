CC = clang
OS := $(shell uname)

src = $(wildcard src/*.c)
obj = $(src:.c=.o)

LDFLAGS = -Iinclude -Llib -llibdb181
CFLAGS =  #-shared

ifeq ($(OS), Linux)
CFLAGS += #-fPIC

default: linux
else
default: win32
endif

%.o: %.c
	$(CC) -c $(CFLAGS) $*.c -o $*.o $(LDFLAGS)

win32: $(obj)
	$(CC) $(CFLAGS) -o bin/libtest.dll $^ $(LDFLAGS)
	
linux: $(obj)
	$(CC) $(CFLAGS) -o bin/liboctree.so $^ $(LDFLAGS)

exe: $(obj)
	$(CC) -o bin/berkeley.exe $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default win32 clean
