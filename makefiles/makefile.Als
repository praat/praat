# File: makefile.Als
# System-independent makefile of the application "ALS".

# System-dependent definitions of CC and LIBS:
include makefile.defs

all:
	cd ipa; make
	cd sys; make
	cd lex; make
	cd sift; make
	cd ewn; make
	cd main; make main_Als.o
	$(CC) -o ldb main/main_Als.o sift/libsift.a ewn/libewn.a \
		lex/liblex.a sys/libsys.a ipa/libipa.a $(LIBS)
