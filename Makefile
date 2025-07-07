# File: Makefile

# Makefile for Praat
# Paul Boersma & David Weenink, 7 July 2025

# System-dependent definitions of CC, LIBS, ICON and MAIN_ICON should be in
# makefile.defs, which has to be copied and renamed
# from a suitable makefile.defs.XXX file in the makefiles directory,
# Perhaps that file requires some editing.
include makefile.defs

.PHONY: all clean install

# Makes the Praat executable in the source directory.
all: all-external all-self
	$(LINK) -o $(EXECUTABLE) main/main_Praat.o $(MAIN_ICON) fon/libfon.a \
		artsynth/libartsynth.a FFNet/libFFNet.a \
		gram/libgram.a EEG/libEEG.a \
		LPC/libLPC.a dwtools/libdwtools.a sensors/libsensors.a \
		foned/libfoned.a fon/libfon.a stat/libstat.a \
		dwsys/libdwsys.a sys/libsys.a melder/libmelder.a kar/libkar.a \
		external/espeak/libespeak.a \
		external/portaudio/libportaudio.a \
		external/flac/libflac.a external/lame/liblame.a external/mp3/libmp3.a \
		external/glpk/libglpk.a \
		external/clapack/libclapack.a \
		external/gsl/libgsl.a \
		external/num/libnum.a \
		external/vorbis/libvorbis.a \
		external/opusfile/libopusfile.a \
		$(LIBS)

all-external:
	$(MAKE) -C external/clapack
	$(MAKE) -C external/gsl
	$(MAKE) -C external/glpk
	$(MAKE) -C external/lame
	$(MAKE) -C external/mp3
	$(MAKE) -C external/num
	$(MAKE) -C external/flac
	$(MAKE) -C external/portaudio
	$(MAKE) -C external/espeak
	$(MAKE) -C external/vorbis
	$(MAKE) -C external/opusfile

all-self:
	$(MAKE) -C kar
	$(MAKE) -C melder
	$(MAKE) -C sys
	$(MAKE) -C dwsys
	$(MAKE) -C stat
	$(MAKE) -C fon
	$(MAKE) -C foned
	$(MAKE) -C dwtools
	$(MAKE) -C LPC
	$(MAKE) -C EEG
	$(MAKE) -C sensors
	$(MAKE) -C gram
	$(MAKE) -C FFNet
	$(MAKE) -C artsynth
	$(MAKE) -C main main_Praat.o $(ICON)

clean: clean-external clean-self
	$(RM) praat

clean-external:
	$(MAKE) -C external/clapack clean
	$(MAKE) -C external/gsl clean
	$(MAKE) -C external/glpk clean
	$(MAKE) -C external/lame clean
	$(MAKE) -C external/mp3 clean
	$(MAKE) -C external/num clean
	$(MAKE) -C external/flac clean
	$(MAKE) -C external/portaudio clean
	$(MAKE) -C external/espeak clean
	$(MAKE) -C external/vorbis clean
	$(MAKE) -C external/opusfile clean

clean-self:
	$(MAKE) -C kar clean
	$(MAKE) -C melder clean
	$(MAKE) -C sys clean
	$(MAKE) -C dwsys clean
	$(MAKE) -C stat clean
	$(MAKE) -C fon clean
	$(MAKE) -C foned clean
	$(MAKE) -C dwtools clean
	$(MAKE) -C LPC clean
	$(MAKE) -C EEG clean
	$(MAKE) -C sensors clean
	$(MAKE) -C gram clean
	$(MAKE) -C FFNet clean
	$(MAKE) -C artsynth clean
	$(MAKE) -C main clean

install:
	$(INSTALL)
