# Makefile of the library liblapack.a
# David Weenink 
# Generated on Fri Mar 13 11:53:58 2020
# with the script "CLAPACK_copyFiles_to_Praat.praat".
# For CLAPACK version 3.1.1.1.

include ../../../makefile.defs

CPPFLAGS = -I ../../../melder -I ..

OBJECTS = lapack.o

.PHONY: all clean

all: liblapack.a

clean:
	$(RM) $(OBJECTS)
	$(RM) liblapack.a

liblapack.a: $(OBJECTS)
	touch liblapack.a
	rm liblapack.a
	$(AR) cq liblapack.a $(OBJECTS)
	$(RANLIB) liblapack.a

$(OBJECTS): ../*.h ../../../melder/*.h
