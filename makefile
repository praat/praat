# File: makefile

# Makefile for Praat.
# Paul Boersma, May 19, 2005

# System-dependent definitions of CC, LIBS, ICON and MAIN_ICON should be in
# makefile.defs, which has to be copied and renamed
# from a suitable makefile.defs.XXX file in the makefiles directory,
# Perhaps that file requires some editing.
include makefile.defs

# Makes the Praat executable in the source directory.
all:
	cd ipa; make
	cd GSL; make
	cd sys; make
	cd fon; make
	cd dwsys; make
	cd dwtools; make
	cd LPC; make
	cd FFNet; make
	cd artsynth; make
	cd main; make main_Praat.o $(ICON)
	$(CC) -o praat main/main_Praat.o $(MAIN_ICON) fon/libfon.a \
		LPC/libLPC.a FFNet/libFFNet.a dwtools/libdwtools.a \
		artsynth/libartsynth.a fon/libfon.a dwsys/libdwsys.a \
		sys/libsys.a GSL/libgsl.a ipa/libipa.a $(LIBS)
