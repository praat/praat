# Makefile of the library "external/ogg"
# David Weenink, 21 December 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = bitwise.o framing.o

.PHONY: all clean

all: libogg.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libogg.a

libogg.a: $(OBJECTS)
	touch libogg.a
	rm libogg.a
	$(AR) cq libogg.a $(OBJECTS)
	$(RANLIB) libogg.a

$(OBJECTS): *.h ../../melder/*.h
