# makefile for library "dwsys".
# David Weenink 20180828
# Paul Boersma 2018-08-10

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../external/gsl -I ../kar

all: libdwsys.a

OBJECTS = Collection_extensions.o Command.o \
	DoublyLinkedList.o Eigen.o \
	FileInMemory.o FileInMemorySet.o FileInMemoryManager.o\
	Graphics_extensions.o Index.o \
	MAT_numerics.o \
	NUM2.o NUMhuber.o NUMlapack.o NUMmachar.o \
	NUMf2c.o NUMcblas.o NUMclapack.o NUMcomplex.o NUMfft_d.o NUMsort2.o \
	NUMmathlib.o NUMstring.o \
	Permutation.o Permutation_and_Index.o \
	SimpleVector.o \
	SVD.o

.PHONY: all clean

all: libdwsys.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libdwsys.a

libdwsys.a: $(OBJECTS) NUMmachar.o
	touch libdwsys.a
	rm libdwsys.a
	$(AR) cq libdwsys.a $(OBJECTS)
	$(RANLIB) libdwsys.a

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../external/gsl/*.h ../dwsys/*.h ../kar/*.h

