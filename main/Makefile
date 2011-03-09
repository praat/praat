# Makefile of the 'main' routines.
# Paul Boersma, 10 March 2011

include ../makefile.defs
CFLAGS = $(DEFS) -I ../num -I ../sys -I ../fon

CPPFLAGS = $(CFLAGS)

.PHONY: clean

main_*.o: ../sys/praat.h ../sys/praat_version.h

clean:
	$(RM) main_*.o
	$(RM) praat_win.o

# The following is only for Windows.

praat_win.o: praat_win.rc
	windres -o praat_win.o praat_win.rc
