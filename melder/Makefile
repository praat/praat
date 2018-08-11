# Makefile of the library "melder"
# Paul Boersma, 11 August 2018

include ../makefile.defs

CPPFLAGS = -I ../melder

OBJECTS = melder.o complex.o melder_alloc.o \
	melder_ftoa.o melder_console.o melder_textencoding.o melder_atof.o melder_files.o \
	melder_vector.o melder_sort.o melder_debug.o melder_strings.o melder_token.o \
	melder_info.o melder_error.o melder_time.o \
	melder_audio.o melder_audiofiles.o melder_quantity.o melder_readtext.o melder_writetext.o \
	abcio.o MelderGui.o melder_sysenv.o regularExp.o \
	tensor.o \
	NUMmath.o \
	NUMspecfunc.o NUMear.o NUMinterpol.o NUMmetrics.o NUMrandom.o NUMfilter.o NUMlinprog.o \
	xxStr.o

.PHONY: all clean

all: libmelder.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libmelder.a

libmelder.a: $(OBJECTS)
	touch libmelder.a
	rm libmelder.a
	$(AR) cq libmelder.a $(OBJECTS)
	$(RANLIB) libmelder.a

$(OBJECTS): ../kar/*.h ../sys/*.h ../dwsys/*.h
