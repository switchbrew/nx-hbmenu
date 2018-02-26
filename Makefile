nx:
	$(MAKE) -f Makefile.nx

pc:
	$(MAKE) -f Makefile.pc

all: nx pc

clean:
	$(MAKE) -f Makefile.pc clean
	$(MAKE) -f Makefile.nx clean