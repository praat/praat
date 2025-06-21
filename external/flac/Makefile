# Makefile of the library "external/flac"
# Erez Volk, March 2007
# Paul Boersma, 21 June 2025

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = \
	flac_bitmath.o \
	flac_bitreader.o \
	flac_bitwriter.o \
	flac_cpu.o \
	flac_crc.o \
	flac_fixed.o \
	flac_float.o \
	flac_format.o \
	flac_lpc.o \
	flac_md5.o \
	flac_memory.o \
	flac_metadata_iterators.o \
	flac_metadata_object.o \
	flac_stream_decoder.o \
	flac_stream_encoder.o \
	flac_stream_encoder_framing.o \
	flac_window.o \
	flac_share_win_utf8_io.o

.PHONY: all clean

all: libflac.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libflac.a

libflac.a: $(OBJECTS)
	touch libflac.a
	rm libflac.a
	$(AR) cq libflac.a $(OBJECTS)
	$(RANLIB) libflac.a

$(OBJECTS): *.h ../../melder/*.h
