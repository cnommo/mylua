include ../Inc.mk
 
SFLAG += -shared -fPIC

LIBS += -lrt

LIBOBJ = log.so

$(LIBOBJ) : log.o
	$(CXX) $(CFLAG) $(SFLAG) $(LIBPATH) -o $@ $< $(LIBS)

install :
	-cp $(LIBOBJ) $(EXTENPATH) -rf

.PHONY clean :
	-rm -rf *.o $(LIBOBJ)
