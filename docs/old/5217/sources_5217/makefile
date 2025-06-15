# File: makefile

# Makefile for Praat.
# Paul Boersma, 20 February 2010

# System-dependent definitions of CC, LIBS, ICON and MAIN_ICON should be in
# makefile.defs, which has to be copied and renamed
# from a suitable makefile.defs.XXX file in the makefiles directory,
# Perhaps that file requires some editing.
include makefile.defs

# Makes the Praat executable in the source directory.
all:
	$(MAKE) -C GSL
	$(MAKE) -C num
	$(MAKE) -C num/glpk
	$(MAKE) -C kar
	$(MAKE) -C audio
	$(MAKE) -C audio/FLAC
	$(MAKE) -C audio/mp3
	$(MAKE) -C sys
	$(MAKE) -C stat
	$(MAKE) -C fon
	$(MAKE) -C dwsys
	$(MAKE) -C dwtools
	$(MAKE) -C LPC
	$(MAKE) -C gram
	$(MAKE) -C FFNet
	$(MAKE) -C artsynth
	$(MAKE) -C contrib/ola
	$(MAKE) -C main main_Praat.o $(ICON)
	$(CC) -o praat main/main_Praat.o $(MAIN_ICON) fon/libfon.a contrib/ola/libOla.a \
		LPC/libLPC.a FFNet/libFFNet.a gram/libgram.a dwtools/libdwtools.a \
		artsynth/libartsynth.a fon/libfon.a stat/libstat.a dwsys/libdwsys.a \
		sys/libsys.a num/libnum.a GSL/libgsl.a num/glpk/libglpk.a kar/libkar.a \
		audio/libaudio.a audio/FLAC/libFLAC.a audio/mp3/libmp3.a \
		$(LIBS)

clean:
	$(MAKE) -C GSL clean
	$(MAKE) -C num clean
	$(MAKE) -C num/glpk clean
	$(MAKE) -C kar clean
	$(MAKE) -C audio clean
	$(MAKE) -C audio/FLAC clean
	$(MAKE) -C audio/mp3 clean
	$(MAKE) -C sys clean
	$(MAKE) -C stat clean
	$(MAKE) -C fon clean
	$(MAKE) -C dwsys clean
	$(MAKE) -C dwtools clean
	$(MAKE) -C LPC clean
	$(MAKE) -C gram clean
	$(MAKE) -C FFNet clean
	$(MAKE) -C artsynth clean
	$(MAKE) -C contrib/ola clean
	# $(MAKE) -C main clean
	$(RM) praat
