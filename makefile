# File: makefile

# Makefile for Praat.
# Paul Boersma, 11 August 2008

# System-dependent definitions of CC, LIBS, ICON and MAIN_ICON should be in
# makefile.defs, which has to be copied and renamed
# from a suitable makefile.defs.XXX file in the makefiles directory,
# Perhaps that file requires some editing.
include makefile.defs

# Makes the Praat executable in the source directory.
all:
	cd GSL; make
	cd num; make
	cd num/glpk; make
	cd kar; make
	cd audio; make
	cd mp3; make
	cd FLAC; make
	cd sys; make
	cd stat; make
	cd fon; make
	cd dwsys; make
	cd dwtools; make
	cd LPC; make
	cd FFNet; make
	cd artsynth; make
	cd contrib/ola; make
	cd main; make main_Praat.o $(ICON)
	$(CC) -o praat main/main_Praat.o $(MAIN_ICON) fon/libfon.a \
		LPC/libLPC.a FFNet/libFFNet.a dwtools/libdwtools.a \
		artsynth/libartsynth.a fon/libfon.a stat/libstat.a dwsys/libdwsys.a \
		sys/libsys.a num/libnum.a GSL/libgsl.a num/glpk/libglpk.a kar/libkar.a \
		audio/libaudio.a FLAC/libFLAC.a mp3/libmp3.a contrib/ola/libOla.a \
		$(LIBS)
