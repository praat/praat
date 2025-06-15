# Makefile of the library "audio"
# Paul Boersma, 22 February 2010

include ../makefile.defs

OBJECTS = pa_unix_hostapis.o pa_linux_alsa.o \
	pa_front.o pa_unix_util.o pa_debugprint.o pa_cpuload.o \
	pa_allocation.o pa_process.o pa_converters.o pa_dither.o \
	pa_stream.o

.PHONY: all clean

all: libaudio.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libaudio.a

libaudio.a: $(OBJECTS)
	touch libaudio.a
	rm libaudio.a
	ar cq libaudio.a $(OBJECTS)
	$(RANLIB) libaudio.a

$(OBJECTS): *.h
