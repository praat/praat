# Makefile of the 'main' routines.
# Paul Boersma, March 9, 2005

include ../makefile.defs
CFLAGS = -I ../sys -I ../fon

main_*.o: ../sys/praat.h ../fon/praat_version.h

# The following is only for Windows.

praat_win.o: praat_win.rc
	windres -o praat_win.o praat_win.rc
