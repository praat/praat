# File: makefile

# Makefile for Praat.
# Paul Boersma, July 7, 2003

# System-dependent definitions of CC and LIBS.
# A suitable makefile.defs.XXX file has been copied from the makefiles directory,
# and renamed to makefile.defs. Perhaps that file requires some editing.
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
	cd main; make main_Praat.o
	$(CC) -o praat main/main_Praat.o fon/libfon.a \
		LPC/libLPC.a FFNet/libFFNet.a dwtools/libdwtools.a \
		artsynth/libartsynth.a fon/libfon.a dwsys/libdwsys.a \
		sys/libsys.a GSL/libgsl.a ipa/libipa.a $(LIBS)

# For Windows, the last two lines may include the icon resource:
#	cd main; make main_Praat.o praat_win.o
#	$(CC) -o praat main/main_Praat.o main/praat_win.o fon/libfon.a \
#		LPC/libLPC.a FFNet/libFFNet.a dwtools/libdwtools.a \
#		artsynth/libartsynth.a fon/libfon.a dwsys/libdwsys.a \
#		sys/libsys.a GSL/libgsl.a ipa/libipa.a $(LIBS)
