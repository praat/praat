# Makefile of the 'main' routines.
# Paul Boersma, March 27, 2001

include ../makefile.defs
CFLAGS = -I ../sys

main_*.o: ../sys/praat.h

# The following is only for Windows.

praat_win.o: praat_win.rc
	windres -o praat_win.o praat_win.rc
