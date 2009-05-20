include ../Inc.mk

LIBS += -lpthread

BINS = test

all : $(BINS) 

test : test.o
	$(CXX) $(CFLAG) $(LIBPATH) -o $@ $< $(LIBS)

install :
	-cp test $(BINPATH) -rf

.PHONY clean :
	-rm -rf *.o $(BINS)
