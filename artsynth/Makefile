# Makefile of the library "artsynth"
# Paul Boersma, 10 August 2018

include ../makefile.defs

CPPFLAGS = -I ../kar -I ../melder -I ../sys -I ../fon -I ../stat

OBJECTS = Speaker.o Articulation.o Artword.o \
     Art_Speaker.o Art_Speaker_to_VocalTract.o Artword_Speaker.o Artword_Speaker_Sound.o \
     Artword_Speaker_to_Sound.o Artword_to_Art.o \
     Delta.o Speaker_to_Delta.o Art_Speaker_Delta.o \
     ArtwordEditor.o praat_Artsynth.o manual_Artsynth.o

.PHONY: all clean

all: libartsynth.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libartsynth.a

libartsynth.a: $(OBJECTS)
	touch libartsynth.a
	rm libartsynth.a
	$(AR) cq libartsynth.a $(OBJECTS)
	$(RANLIB) libartsynth.a

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../fon/*.h ../stat/*.h

