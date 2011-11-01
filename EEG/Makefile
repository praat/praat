# Makefile of the library "EEG"
# Paul Boersma, 1 November 2011

include ../makefile.defs

CPPFLAGS = -I ../num -I ../kar -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon

OBJECTS = EEG.o EEGWindow.o \
   praat_EEG.o

.PHONY: all clean

all: libEEG.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libEEG.a

libEEG.a: $(OBJECTS)
	touch libEEG.a
	rm libEEG.a
	ar cq libEEG.a $(OBJECTS)
	$(RANLIB) libEEG.a

$(OBJECTS): *.h ../num/NUM.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h
