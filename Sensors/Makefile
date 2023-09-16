# makefile for library "Sensors".
# David Weenink 20230817

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/clapack -I ../kar -I ../dwtools

all: libSensors.a

OBJECTS = EMA.o praat_Sensors_init.o Sensor.o

.PHONY: all clean

all: libSensors.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libSensors.a

libSensors.a: $(OBJECTS)
	touch libSensors.a
	rm libSensors.a
	$(AR) cq libSensors.a $(OBJECTS)
	$(RANLIB) libSensors.a

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../fon/*.h ../external/gsl/*.h ../dwsys/*.h ../kar/*.h

