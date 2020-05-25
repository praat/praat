# Makefile of the library "external/clapack"
# David Weenink, 13 March 2020
# Paul Boersma, 24 May 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = blas.o \
	lapack.o lapack_dg.o lapack_dlaq.o \
	lapack_dlar.o lapack_ds.o lapack_dt.o

.PHONY: all clean

all: libclapack.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libclapack.a

libclapack.a: $(OBJECTS)
	touch libclapack.a
	rm libclapack.a
	$(AR) cq libclapack.a $(OBJECTS)
	$(RANLIB) libclapack.a

$(OBJECTS): *.h ../../melder/*.h
