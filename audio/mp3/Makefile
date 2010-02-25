# Makefile of the library "audio/mp3"
# Erez Volk, 24 May 2007
# Paul Boersma, 22 February 2010

include ../../makefile.defs

CFLAGS = -I ../../sys

OBJECTS = mp3.o \
	mad_bit.o \
	mad_decoder.o \
	mad_fixed.o \
	mad_frame.o \
	mad_huffman.o \
	mad_layer12.o \
	mad_layer3.o \
	mad_stream.o \
	mad_synth.o \
	mad_timer.o \
	mad_version.o

.PHONY: all clean

all: libmp3.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libmp3.a

libmp3.a: $(OBJECTS)
	touch libmp3.a
	rm libmp3.a
	ar cq libmp3.a $(OBJECTS)
	$(RANLIB) libmp3.a

$(OBJECTS): *.h ../../sys/*.h
