# makefile for library "sensors"
# David Weenink 20230817

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/clapack -I ../kar -I ../dwtools

<<<<<<< HEAD
OBJECTS = EMA.o praat_sensors_init.o Sensor.o
=======
all: libsensors.a

OBJECTS = EMA.o praat_Sensors_init.o Sensor.o
>>>>>>> 5fb533860cd336f9097174359e9d603c666ded5a

.PHONY: all clean

all: libsensors.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libsensors.a

libsensors.a: $(OBJECTS)
	touch libsensors.a
	rm libsensors.a
	$(AR) cq libsensors.a $(OBJECTS)
	$(RANLIB) libsensors.a

$(OBJECTS): *.h ../melder/*.h ../stat/*.h ../sys/*.h ../fon/*.h ../external/gsl/*.h ../dwsys/*.h ../kar/*.h

