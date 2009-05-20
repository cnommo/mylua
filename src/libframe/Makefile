include ../Inc.mk
 
SFLAG += -shared -fPIC


LIBOBJ = mylib.so

all : $(LIBOBJ) 

mylib.so : mylib.o
	$(CXX) $(CFLAG) $(SFLAG) $(LIBPATH) -o $@ $< $(LIBS)

install :
	-cp $(LIBOBJ) $(EXTENPATH) -rf

.PHONY clean :
	-rm -rf *.o $(LIBOBJ)
