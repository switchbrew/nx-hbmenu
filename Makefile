all:
	make -f Makefile.nx
	make -f Makefile.pc

clean:
	make -f Makefile.pc clean
	make -f Makefile.nx clean
