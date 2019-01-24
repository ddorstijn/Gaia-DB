CXX = g++
OS := $(shell uname)

src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

test_src = $(wildcard src/test/*.cpp)
test_obj = $(test_src:.cpp=.o)

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

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $*.cpp -o $*.o 

win32: $(obj)
	$(CXX) $(CFLAGS) -o bin/libgaiadb.dll $^ $(LDFLAGS)

linux: CFLAGS += -shared
linux: $(obj)
	$(CXX) $(CFLAGS) -o bin/libgaiadb.so $^ $(LDFLAGS)

test: $(obj) $(test_obj)
	$(CXX) $(CFLAGS) -o bin/gaiadb $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default win32 clean
