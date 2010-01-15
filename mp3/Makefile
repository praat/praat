# Makefile for mp3 wrapper library.
# Erez Volk 20070524

include ../makefile.defs
CFLAGS = -I ../sys

all: libmp3.a

clean:
	rm *.o

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

TEST = test.o

$(OBJECTS): *.h ../sys/*.h

test: $(TEST) libmp3.a

libmp3.a: $(OBJECTS)
	touch libmp3.a
	rm libmp3.a
	ar cq libmp3.a $(OBJECTS)
	$(RANLIB) libmp3.a

