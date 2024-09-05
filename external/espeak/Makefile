# makefile for libespeak
# djmw 20230912
# ppgb 20240105  compatible with 32-bit compilers (split off Russian)
# ppgb 20240828  compatible with low-resource compilers (split off Faroese)

include ../../makefile.defs

CPPFLAGS = -I ../../kar -I ../../melder -I ../../sys -I ../../dwsys -I ../../stat

OBJECTS = case.o categories.o common.o compiledict.o \
	dictionary.o \
	encoding.o error.o espeak_api.o \
	intonation.o klatt.o langopts.o mnemonics.o \
	numbers.o phonemelist.o proplist.o readclause.o \
	setlengths.o soundicon.o speech.o \
	ssml.o synthdata.o synthesize.o \
	tr_languages.o translate.o translateword.o voices.o wavegen.o \
	espeak_praat_FileInMemorySet_addLanguages.o espeak_praat_FileInMemorySet_addVoices.o \
	espeak_praat_FileInMemorySet_addRussianDict.o espeak_praat_FileInMemorySet_addFaroeseDict.o \
	espeak_praat_FileInMemorySet_addOtherDicts.o espeak_praat_FileInMemorySet_addPhon.o \
	espeak_praat.o

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

