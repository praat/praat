# Makefile of the library "EEG"
# Paul Boersma, 24 August 2013

include ../makefile.defs

CPPFLAGS = -I ../num -I ../kar -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon

OBJECTS = EEG.o EEGWindow.o ERPTier.o ERP.o ERPWindow.o \
   praat_EEG.o manual_EEG.o

.PHONY: all clean

all: libEEG.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libEEG.a

libEEG.a: $(OBJECTS)
	touch libEEG.a
	rm libEEG.a
	$(AR) cq libEEG.a $(OBJECTS)
	$(RANLIB) libEEG.a

$(OBJECTS): *.h ../num/NUM.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h
