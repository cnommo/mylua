ALLDIRS = mainframe \
		  message \
		  log \
		  tools

path = $(PWD)

all :
	@for i in $(ALLDIRS) ; \
	do \
		cd $(path)/$$i; echo "make... $$i"; make; \
	done

install : 
	@for i in $(ALLDIRS) ; \
	do \
		cd $(path)/$$i; echo "make install... $$i"; make install; \
	done



.PHONY clean :
	@for i in $(ALLDIRS) ; \
	do \
		cd $(path)/$$i; echo "make clean ... $$i"; make clean; \
	done

