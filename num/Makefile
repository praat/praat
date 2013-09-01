# Makefile of the library "num"
# Paul Boersma, 24 August 2013

include ../makefile.defs

CPPFLAGS = -I ../external/gsl -I ../external/glpk -I ../sys -I ../dwsys

OBJECTS = NUM.o NUMarrays.o NUMrandom.o NUMsort.o NUMear.o NUMlinprog.o

.PHONY: all clean

all: libnum.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libnum.a

libnum.a: $(OBJECTS)
	touch libnum.a
	rm libnum.a
	$(AR) cq libnum.a $(OBJECTS)
	$(RANLIB) libnum.a

$(OBJECTS): *.h ../external/gsl/*.h ../external/glpk/*.h ../sys/*.h ../dwsys/*.h
