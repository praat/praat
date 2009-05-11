# makefile for library "dwsys".
# David Weenink 20061222
# djmw 20080130 Latest modification.
# djmw 20090607 Added -O0 to remove optimization for the numerical routines (especially NUMcblas.c)

include ../makefile.defs

CFLAGS = -I ../stat -I ../num -I ../sys -I ../GSL -I ../kar -O0

all: libdwsys.a

clean:
	rm *.o

OBJECTS = Collection_extensions.o Command.o \
	Eigen.o Index.o \
	NUM2.o NUMhuber.o NUMlapack.o NUMmachar.o \
	NUMf2c.o NUMcblas.o NUMclapack.o NUMfft_d.o NUMsort2.o \
	Permutation.o Permutation_and_Index.o \
	regularExp.o SimpleVector.o Simple_extensions.o \
	SVD.o

$(OBJECTS): *.h ../stat/*.h ../num/NUM.h ../sys/*.h ../GSL/*.h ../dwsys/*.h ../kar/*.h


libdwsys.a: $(OBJECTS) NUMmachar.o
	touch libdwsys.a
	rm libdwsys.a
	ar cq libdwsys.a $(OBJECTS)
	$(RANLIB) libdwsys.a
	
