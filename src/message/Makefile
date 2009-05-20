include ../Inc.mk
 
SFLAG += -shared -fPIC


LIBOBJ = message.so

$(LIBOBJ) : message.o
	$(CXX) $(CFLAG) $(SFLAG) $(LIBPATH) -o $@ $< $(LIBS)

install :
	-cp $(LIBOBJ) $(EXTENPATH) -rf

.PHONY clean :
	-rm -rf *.o $(LIBOBJ)
