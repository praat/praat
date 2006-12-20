# Makefile of the library "kar"
# Paul Boersma, December 15, 2006

include ../makefile.defs

OBJECTS = ipaSerifRegularPS.o ipaSerifRegular24.o longchar.o

all: libkar.a
libkar.a: $(OBJECTS)
	touch libkar.a
	rm libkar.a
	ar cq libkar.a $(OBJECTS)
	$(RANLIB) libkar.a

$(OBJECTS): *.h
