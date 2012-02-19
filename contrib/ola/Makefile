# Makefile of the library "contrib/ola"
# Ola So"der 19 January 2008
# Paul Boersma, 14 January 2012

include ../../makefile.defs

CPPFLAGS = -I ../../dwtools -I ../../fon -I ../../sys -I ../../dwsys -I ../../stat -I ../../num -I ../../external/gsl -D_DEBUG -D_REENTRANT

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
	ar cq libOla.a $(OBJECTS)
	$(RANLIB) libOla.a

$(OBJECTS): *.h ../../sys/*.h ../../dwtools/*.h ../../fon/*.h ../../dwsys/*.h ../../stat/*.h ../../num/*.h ../../external/gsl/*.h
