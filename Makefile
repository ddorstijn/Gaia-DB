CXX = clang++
OS := $(shell uname)

src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

CFLAGS = -g -O0 #-shared
LDFLAGS = 

ifeq ($(OS), Linux)
CFLAGS += #-fPIC
LDFLAGS := -ldb
default: linux
else
LDFLAGS = -Iinclude -Llib -llibdb181
default: win32
endif

%.o: %.c
	$(CXX) -c -g -O0 $(CFLAGS) $*.c -o $*.o 

win32: $(obj)
	$(CXX) $(CFLAGS) -o bin/libtest.dll $^ $(LDFLAGS)
	
linux: $(obj)
	$(CXX) $(CFLAGS) -o bin/liboctree.so $^ $(LDFLAGS)

exe: $(obj)
	$(CXX) $(CFLAGS) -g -O0  -o bin/berkeley $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default win32 clean
