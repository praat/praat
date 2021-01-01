# Makefile of the directory "external/opusfile/opus/celt"
# David Weenink, 1 Januari 2021

include ../../../../makefile.defs

CPPFLAGS = -I ../../melder -I ../../../vorbis -I ..

OBJECTS = bands.o celt.o celt_encoder.o celt_decoder.o \
cwrs.o entcode.o entdec.o entenc.o kiss_fft.o \
laplace.o mathops.o mdct.o modes.o pitch.o \
celt_lpc.o quant_bands.o rate.o vq.o


.PHONY: all clean

all: libcelt.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libcelt.a

libcelt.a: $(OBJECTS)
	touch libcelt.a
	rm libcelt.a
	$(AR) cq libcelt.a $(OBJECTS)
	$(RANLIB) libcelt.a

$(OBJECTS): *.h ../../../../melder/*.h
