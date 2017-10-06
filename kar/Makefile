# Makefile of the library "kar"
# Paul Boersma, 5 October 2017

include ../makefile.defs

OBJECTS = ipaSerifRegularPS.o longchar.o

CPPFLAGS = -I ../sys

.PHONY: all clean

all: libkar.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libkar.a

libkar.a: $(OBJECTS)
	touch libkar.a
	rm libkar.a
	$(AR) cq libkar.a $(OBJECTS)
	$(RANLIB) libkar.a

$(OBJECTS): *.h ../sys/*.h
