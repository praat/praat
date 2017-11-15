# Makefile of the library "contrib/ola"
# Ola So"der 19 January 2008
# Paul Boersma, 2 June 2017

include ../../makefile.defs

CPPFLAGS = -I ../../kar -I ../../sys -I ../../FFNet -I ../../dwtools -I ../../fon -I ../../dwsys -I ../../stat -I ../../external/gsl -D_DEBUG -D_REENTRANT

OBJECTS = KNN.o \
   KNN_threads.o Pattern_to_Categories_cluster.o KNN_prune.o FeatureWeights.o praat_contrib_Ola_KNN.o manual_KNN.o

.PHONY: all clean

all: libOla.a  

clean:
	$(RM) $(OBJECTS)
	$(RM) libOla.a

libOla.a: $(OBJECTS)
	touch libOla.a
	rm libOla.a
	$(AR) cq libOla.a $(OBJECTS)
	$(RANLIB) libOla.a

$(OBJECTS): *.h ../../kar/*.h ../../sys/*.h ../../FFNet/*.h ../../dwtools/*.h ../../fon/*.h ../../dwsys/*.h ../../stat/*.h ../../external/gsl/*.h
