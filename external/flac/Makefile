# Makefile of the library "external/flac"
# Erez Volk, March 2007
# pb 20100108: include sys/melder.h
# Paul Boersma, 24 August 2013

include ../../makefile.defs

CPPFLAGS = -DFLAC__NOASM -I ../../melder

# EREZ ?
# On non-Intel, also define -DFLAC__CPU_UNKNOWN
# On Intel, define -DFLAC__CPU_IA32
# On PowerPC, define -DFLAC__CPU_PPC
# How/when are these relevant?

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
	flac_window.o

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
