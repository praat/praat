# Makefile for library "external/liblame.a"
# David Weenink 20231212, Paul Boersma 20231229

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = bitstream.o encoder.o fft.o gain_analysis.o mpglib_interface.o id3tag.o lame.o \
	newmdct.o psymodel.o quantize.o quantize_pvt.o set_get.o vbrquantize.o \
	reservoir.o tables.o takehiro.o util.o VbrTag.o \
	version.o presets.o vector_xmm_quantize_sub.o

.PHONY: all clean

all: liblame.a

clean:
	$(RM) $(OBJECTS)
	$(RM) liblame.a

liblame.a: $(OBJECTS)
	touch liblame.a
	rm liblame.a
	$(AR) cq liblame.a $(OBJECTS)
	$(RANLIB) liblame.a

$(OBJECTS): *.h

