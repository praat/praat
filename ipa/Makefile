# Makefile of the library "ipa"
# Paul Boersma, September 23, 2000

include ../makefile.defs
all: libipa.a

# The objects:

OBJECTS = ipaSerifRegularPS.o ipaSerifRegular24.o

# The archive:

libipa.a: $(OBJECTS)
	touch libipa.a
	rm libipa.a
	ar cq libipa.a $(OBJECTS)
	$(RANLIB) libipa.a
