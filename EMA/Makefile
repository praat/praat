# makefile for library "EMA".
# David Weenink 20230817

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/clapack -I ../kar -I ../dwtools

all: libEMA.a

OBJECTS = EMA.o praat_EMA_init.o Sensor.o

.PHONY: all clean

all: libEMA.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libEMA.a

libEMA.a: $(OBJECTS)
	touch libEMA.a
	rm libEMA.a
	$(AR) cq libEMA.a $(OBJECTS)
	$(RANLIB) libEMA.a

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../fon/*.h ../external/gsl/*.h ../dwsys/*.h ../kar/*.h

