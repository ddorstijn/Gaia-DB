CXX = g++
OS := $(shell uname)

src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

CFLAGS = -O3 
LDFLAGS = 

ifeq ($(OS), Linux)
LDFLAGS := -ldb
default: linux
else
CFLAGS += -Iinclude -Llib
LDFLAGS = -llibdb181
default: exe
endif

%.o: %.cpp
	$(CXX) -c $(CFLAGS) $*.cpp -o $*.o 

win32: $(obj)
	$(CXX) $(CFLAGS) -o bin/libgaiadb.dll $^ $(LDFLAGS)
	
linux: $(obj)
	$(CXX) $(CFLAGS) -o bin/libgaiadb.so $^ $(LDFLAGS)

exe: $(obj)
	$(CXX) $(CFLAGS) -o bin/gaiadb.exe $^ $(LDFLAGS)

clean:
	rm -f $(obj) win32

.PHONY: default win32 clean
