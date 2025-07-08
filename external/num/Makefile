# Makefile of the library "external/num"
# Paul Boersma, 8 July 2025

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = median_of_ninthers.o

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

$(OBJECTS): *.h ../../melder/*.h
