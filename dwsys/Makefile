# makefile for library "dwsys".
# David Weenink 20110414

include ../makefile.defs

CFLAGS = $(DEFS) -I ../stat -I ../num -I ../sys -I ../GSL -I ../kar

CPPFLAGS = $(CFLAGS)

all: libdwsys.a

OBJECTS = Collection_extensions.o Command.o \
	DLL.o Eigen.o Index.o \
	NUM2.o NUMhuber.o NUMlapack.o NUMmachar.o \
	NUMf2c.o NUMcblas.o NUMclapack.o NUMfft_d.o NUMsort2.o \
	Permutation.o Permutation_and_Index.o \
	regularExp.o SimpleVector.o Simple_extensions.o \
	SVD.o

.PHONY: all clean

all: libdwsys.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libdwsys.a

libdwsys.a: $(OBJECTS) NUMmachar.o
	touch libdwsys.a
	rm libdwsys.a
	ar cq libdwsys.a $(OBJECTS)
	$(RANLIB) libdwsys.a

$(OBJECTS): *.h ../stat/*.h ../num/NUM.h ../sys/*.h ../GSL/*.h ../dwsys/*.h ../kar/*.h
