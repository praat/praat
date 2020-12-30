# Makefile of the library "external/opus"
# David Weenink, 29 December 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder -I ../vorbis

OBJECTS = opusfile.o opusfile_info.o opusfile_internal.o opusfile_stream.o


.PHONY: all clean

all: libopusfile.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libopusfile.a

libopusfile.a: $(OBJECTS)
	touch libopusfile.a
	rm libopusfile.a
	$(AR) cq libopusfile.a $(OBJECTS)
	$(RANLIB) libopusfile.a

$(OBJECTS): *.h ../../melder/*.h
