# Makefile of the 'main' routines.
# Paul Boersma, April 9, 2008

include ../makefile.defs
CFLAGS = -I ../num -I ../sys -I ../fon

main_*.o: ../sys/praat.h ../fon/praat_version.h

# The following is only for Windows.

praat_win.o: praat_win.rc
	windres -o praat_win.o praat_win.rc
