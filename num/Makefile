# Makefile of the library "num"
# Paul Boersma, 22 February 2010

include ../makefile.defs
CFLAGS = -I ../GSL -I glpk -I ../sys -I ../dwsys

OBJECTS = NUM.o NUMarrays.o NUMrandom.o NUMsort.o NUMear.o NUMlinprog.o

.PHONY: all clean

all: libnum.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libnum.a

libnum.a: $(OBJECTS)
	touch libnum.a
	rm libnum.a
	ar cq libnum.a $(OBJECTS)
	$(RANLIB) libnum.a

$(OBJECTS): *.h ../GSL/*.h glpk/*.h ../sys/*.h ../dwsys/*.h
