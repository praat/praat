# makefile for library dwtools.
# David Weenink 20030703


include ../makefile.defs

CFLAGS = -I ../sys -I ../fon -I ../dwtools -I ../GSL

all: libdwsys.a

clean:
	rm *.o

OBJECTS = Collection_extensions.o Command.o \
	NUM2.o NUMlapack.o NUMmachar.o \
	NUMf2c.o NUMcblas.o NUMclapack.o NUMfft_f.o NUMfft_d.o NUMsort2.o \
	regularExp.o Sequence.o SimpleVector.o Simple_extensions.o \
	SVD.o

$(OBJECTS): *.h ../sys/*.h


libdwsys.a: $(OBJECTS) NUMmachar.o
	touch libdwsys.a
	rm libdwsys.a
	ar cq libdwsys.a $(OBJECTS)
	$(RANLIB) libdwsys.a
	
