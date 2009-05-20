include ../Inc.mk
 
SFLAG += -shared -fPIC


LIBOBJ = tools.so

all : $(LIBOBJ) 

tools.so : tools.o base64.o hex.o
	$(CXX) $(CFLAG) $(SFLAG) $(LIBPATH) -o $@ $^ $(LIBS)

install :
	-cp $(LIBOBJ) $(EXTENPATH) -rf

.PHONY clean :
	-rm -rf *.o $(LIBOBJ)
