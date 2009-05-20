CC = gcc
CXX = g++

CFLAG = -Wall -g -O2
SFLAG = 

LUAPATH=../../dep/lua-5.1.4
INCPATH = -I$(LUAPATH)/include
LIBPATH = -L$(LUAPATH)/lib
LIBS = -llua -lm -ldl

BINPATH = ../../bin
EXTENPATH = ../../bin/extensions

%.o : %.c
	$(CC) $(CFLAG) $(SFLAG) $()$(INCPATH) -c -o $@ $<

%.o : %.cpp
	$(CXX) $(CFLAG) $(SFLAG) $(INCPATH) -c -o $@ $< 
