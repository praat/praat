# Makefile of the library "kar"
# Paul Boersma, 10 August 2018

include ../makefile.defs

OBJECTS = ipaSerifRegularPS.o longchar.o

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
