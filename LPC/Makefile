# Makefile of the library "LPC"
# David Weenink and Paul Boersma 2017-08-08

include ../makefile.defs

CPPFLAGS = -I ../kar -I ../dwtools -I ../fon -I ../sys -I ../dwsys -I ../stat

OBJECTS = Cepstrum.o Cepstrumc.o Cepstrum_and_Spectrum.o \
	Cepstrogram.o \
	Formant_extensions.o LineSpectralFrequencies.o \
	LPC.o LPC_and_Cepstrumc.o LPC_and_Formant.o LPC_and_LFCC.o \
	LPC_and_LineSpectralFrequencies.o LPC_and_Polynomial.o \
	LPC_to_Spectrum.o  LPC_to_Spectrogram.o \
	LPC_and_Tube.o \
	Sound_and_LPC.o  Sound_and_LPC_robust.o \
	Sound_and_Cepstrum.o Tube.o \
	VocalTractTier.o \
	praat_LPC_init.o manual_LPC.o

.PHONY: all clean

all: libLPC.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libLPC.a

libLPC.a: $(OBJECTS)
	touch libLPC.a
	rm libLPC.a
	$(AR) cq libLPC.a $(OBJECTS)
	$(RANLIB) libLPC.a

$(OBJECTS): *.h ../kar/*.h ../dwtools/*.h ../fon/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h


