# Makefile of the library "audio"
# Paul Boersma, June 1, 2008

include ../makefile.defs

all: libaudio.a

OBJECTS = pa_unix_hostapis.o pa_linux_alsa.o \
	pa_front.o pa_unix_util.o pa_debugprint.o pa_cpuload.o \
	pa_allocation.o pa_process.o pa_converters.o pa_dither.o \
	pa_stream.o

all: libaudio.a
libaudio.a: $(OBJECTS)
	touch libaudio.a
	rm libaudio.a
	ar cq libaudio.a $(OBJECTS)
	$(RANLIB) libaudio.a

$(OBJECTS): *.h
