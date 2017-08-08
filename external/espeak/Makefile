# makefile for libespeak

include ../../makefile.defs

CPPFLAGS = -I ../../kar -I ../../sys -I ../../dwsys -I ../../stat

OBJECTS =  compiledict.o dictionary.o intonation.o klatt.o  \
	numbers.o phonemelist.o readclause.o setlengths.o  \
	sonic.o speak_lib.o synthdata.o synthesize.o  \
	synth_mbrola.o tr_languages.o translate.o voices.o wavegen.o \
	espeakdata_phons.o espeakdata_dicts.o espeakdata_voices.o espeakdata_variants.o \
	espeakdata_FileInMemory.o

.PHONY: all clean

all: libespeak.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libespeak.a

libespeak.a: $(OBJECTS)
	touch libespeak.a
	rm libespeak.a
	$(AR) cq libespeak.a $(OBJECTS)
	$(RANLIB) libespeak.a

$(OBJECTS):  *.h ../../kar/*.h ../../sys/*.h ../../dwsys/*.h ../../stat/*.h

