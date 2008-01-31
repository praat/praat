# makefile for library dwsys.
# David Weenink 20061222
# djmw 20080122 Latest modification.

include ../makefile.defs

CFLAGS = -I ../sys -I ../fon -I ../dwtools -I ../GSL -I ../dwsys -I ../kar

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

$(OBJECTS): *.h ../sys/*.h


libdwsys.a: $(OBJECTS) NUMmachar.o
	touch libdwsys.a
	rm libdwsys.a
	ar cq libdwsys.a $(OBJECTS)
	$(RANLIB) libdwsys.a
	
