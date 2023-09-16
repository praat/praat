<<<<<<< HEAD
# makefile for library "Sensors".
# David Weenink 20230923
=======
# makefile for library "sensors"
# David Weenink 20230817
>>>>>>> ae15782530c800273532a33ab6de8f5020e7ce91

include ../makefile.defs

CPPFLAGS = -I ../melder -I ../stat -I ../sys -I ../fon -I ../external/gsl -I ../external/clapack -I ../kar -I ../dwtools

OBJECTS = EMA.o praat_sensors_init.o Sensor.o

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

