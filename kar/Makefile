# Makefile of the library "kar"
# Paul Boersma, 24 August 2013

include ../makefile.defs

OBJECTS = ipaSerifRegularPS.o ipaSerifRegular24.o longchar.o

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

$(OBJECTS): *.h
