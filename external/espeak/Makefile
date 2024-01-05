# makefile for libespeak
# djmw 20230912
# ppgb 20240105  compatible with 32-bit compilers (split off __ru from create_espeak_ng_FileInMemorySet)

include ../../makefile.defs

CPPFLAGS = -I ../../kar -I ../../melder -I ../../dwtools -I ../../sys -I ../../dwsys -I ../../stat

OBJECTS = case.o categories.o common.o compiledata.o compiledict.o \
	dictionary.o \
	encoding.o error.o espeak_api.o \
	intonation.o klatt.o langopts.o mnemonics.o \
	numbers.o phonemelist.o proplist.o readclause.o \
	setlengths.o soundicon.o speech.o \
	ssml.o synthdata.o synthesize.o \
	synth_mbrola.o tr_languages.o translate.o translateword.o voices.o wavegen.o \
	create_espeak_ng_FileInMemoryManager.o \
	create_espeak_ng_FileInMemorySet__ru.o create_espeak_ng_FileInMemorySet.o \
	espeak_io.o

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

$(OBJECTS):  *.h ../../kar/*.h ../../melder/*.h ../../sys/*.h ../../dwsys/*.h ../../stat/*.h

