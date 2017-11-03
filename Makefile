# File: Makefile

# Makefile for Praat.
# Paul Boersma, 22 October 2016

# System-dependent definitions of CC, LIBS, ICON and MAIN_ICON should be in
# makefile.defs, which has to be copied and renamed
# from a suitable makefile.defs.XXX file in the makefiles directory,
# Perhaps that file requires some editing.
include makefile.defs

.PHONY: all clean install

# Makes the Praat executable in the source directory.
all:
	$(MAKE) -C external/gsl
	$(MAKE) -C external/glpk
	$(MAKE) -C external/mp3
	$(MAKE) -C external/flac
	$(MAKE) -C external/portaudio
	$(MAKE) -C external/espeak
	$(MAKE) -C kar
	$(MAKE) -C num
	$(MAKE) -C sys
	$(MAKE) -C dwsys
	$(MAKE) -C stat
	$(MAKE) -C fon
	$(MAKE) -C dwtools
	$(MAKE) -C LPC
	$(MAKE) -C EEG
	$(MAKE) -C gram
	$(MAKE) -C FFNet
	$(MAKE) -C artsynth
	$(MAKE) -C contrib/ola
	$(MAKE) -C main main_Praat.o $(ICON)
	$(LINK) -o $(EXECUTABLE) main/main_Praat.o $(MAIN_ICON) fon/libfon.a \
		contrib/ola/libOla.a artsynth/libartsynth.a \
		FFNet/libFFNet.a gram/libgram.a EEG/libEEG.a \
		LPC/libLPC.a dwtools/libdwtools.a \
		fon/libfon.a stat/libstat.a dwsys/libdwsys.a \
		sys/libsys.a num/libnum.a kar/libkar.a \
		external/espeak/libespeak.a \
		external/portaudio/libportaudio.a \
		external/flac/libflac.a external/mp3/libmp3.a \
		external/glpk/libglpk.a external/gsl/libgsl.a \
		$(LIBS)

clean:
	$(MAKE) -C external/gsl clean
	$(MAKE) -C external/glpk clean
	$(MAKE) -C external/mp3 clean
	$(MAKE) -C external/flac clean
	$(MAKE) -C external/portaudio clean
	$(MAKE) -C external/espeak clean
	$(MAKE) -C kar clean
	$(MAKE) -C num clean
	$(MAKE) -C sys clean
	$(MAKE) -C dwsys clean
	$(MAKE) -C stat clean
	$(MAKE) -C fon clean
	$(MAKE) -C dwtools clean
	$(MAKE) -C LPC clean
	$(MAKE) -C EEG clean
	$(MAKE) -C gram clean
	$(MAKE) -C FFNet clean
	$(MAKE) -C artsynth clean
	$(MAKE) -C contrib/ola clean
	$(MAKE) -C main clean
	$(RM) praat

install:
	$(INSTALL)
