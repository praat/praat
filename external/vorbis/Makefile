# Makefile of the library "external/vorbis"
# David Weenink, 21 December 2020

include ../../makefile.defs

CPPFLAGS = -I ../../melder

OBJECTS = ogg_bitwise.o ogg_framing.o vorbis_floor0.o vorbis_info.o vorbis_mdct.o vorbisfile.o vorbis_floor0.o \
	vorbis_registry.o vorbis_window.o


.PHONY: all clean

all: libvorbis.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libvorbis.a

libvorbis.a: $(OBJECTS)
	touch libvorbis.a
	rm libvorbis.a
	$(AR) cq libvorbis.a $(OBJECTS)
	$(RANLIB) libvorbis.a

$(OBJECTS): *.h ../../melder/*.h
