# Makefile of the library "num"
# Paul Boersma, April 12, 2008

include ../makefile.defs
CFLAGS = -I ../GSL -I glpk -I ../sys -I ../dwsys

OBJECTS = NUM.o NUMarrays.o NUMrandom.o NUMsort.o NUMear.o NUMlinprog.o

all: libnum.a
libnum.a: $(OBJECTS)
	touch libnum.a
	rm libnum.a
	ar cq libnum.a $(OBJECTS)
	$(RANLIB) libnum.a

$(OBJECTS): *.h ../GSL/*.h glpk/*.h ../sys/*.h ../dwsys/*.h
