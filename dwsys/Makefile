# makefile for library "dwsys".
# David Weenink 2020319
# Paul Boersma 2018-08-10

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/clapack -I ../kar

all: libdwsys.a

OBJECTS = ChebyshevSeries.o Collection_extensions.o Command.o \
	DoublyLinkedList.o Eigen.o \
	FileInMemory.o FileInMemorySet.o FileInMemoryManager.o \
	FunctionSeries.o \
	Graphics_extensions.o Index.o \
	LegendreSeries.o \
	MAT_numerics.o \
	NMF.o NUM2.o NUMhuber.o NUMmachar.o \
	NUMcomplex.o NUMfft_d.o NUMsort2.o \
	NUMmathlib.o NUMstring.o \
	Permutation.o Permutation_and_Index.o \
	Polynomial.o \
	Roots.o \
	Spline.o SVD.o

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

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../fon/*.h ../external/gsl/*.h ../dwsys/*.h ../kar/*.h

