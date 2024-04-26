# Makefile of the library "kar"
# Paul Boersma, 26 April 2024

include ../makefile.defs

OBJECTS = ipaSerifRegularPS.o longchar.o wctype_portable.o

CPPFLAGS = -I ../melder

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

$(OBJECTS): *.h ../melder/*.h
