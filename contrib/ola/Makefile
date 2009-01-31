# Makefile of the library "Ola"
# Ola Söder 20080109


include ../../makefile.defs

CFLAGS =  -I ../../dwtools -I ../../fon -I ../../sys -I ../../dwsys -I ../../stat -I ../../num -I ../../GSL -D_DEBUG -D_REENTRANT

all: libOla.a  

clean:
	rm *.o *.a

OBJECTS = KNN.o \
   KNN_threads.o Pattern_to_Categories_cluster.o KNN_prune.o FeatureWeights.o praat_contrib_Ola_KNN.o manual_KNN.o

$(OBJECTS): *.h ../../sys/*.h ../../dwtools/*.h ../../fon/*.h ../../dwsys/*.h ../../stat/*.h ../../num/*.h ../../GSL/*.h

libOla.a: $(OBJECTS)
	touch libOla.a
	rm libOla.a
	ar cq libOla.a $(OBJECTS)
	$(RANLIB) libOla.a
