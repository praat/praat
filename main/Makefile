# Makefile of the 'main' routines.
# Paul Boersma, 24 September 2024

include ../makefile.defs

CPPFLAGS = -I ../kar -I ../melder -I ../sys -I ../fon

.PHONY: clean

main_Praat.o: ../sys/praat.h ./main_Praat.h ../sys/Graphics.h

clean:
	$(RM) main_*.o
	$(RM) praat_win.o

# The following is only for Windows.

praat_win.o: praat_win.rc
	$(WINDRES) -o praat_win.o praat_win.rc
