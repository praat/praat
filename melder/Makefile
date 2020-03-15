# Makefile of the library "melder"
# Paul Boersma, 14 March 2020

include ../makefile.defs

CPPFLAGS = -I ../melder

OBJECTS = melder.o complex.o melder_alloc.o melder_colour.o \
	melder_ftoa.o melder_console.o melder_textencoding.o melder_atof.o melder_files.o \
	melder_sort.o melder_debug.o MelderFile.o melder_strings.o \
	melder_search.o \
	melder_info.o melder_error.o melder_warning.o melder_fatal.o melder_progress.o \
	melder_play.o melder_help.o melder_time.o \
	melder_audio.o melder_audiofiles.o melder_quantity.o MelderReadText.o melder_tensorio.o \
	abcio.o melder_sysenv.o regularExp.o \
	NUMmath.o \
	NUMspecfunc.o NUMear.o NUMinterpol.o NUMmetrics.o NUMrandom.o NUMfilter.o NUMlinprog.o \
	NUM.o STR.o VEC.o MAT.o STRVEC.o

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
