# Makefile of the library "stat"
# Paul Boersma, April 9, 2008

include ../makefile.defs
CFLAGS = -I ../num -I ../audio -I ../kar -I ../sys -I ../dwsys -I ../dwtools -I ../fon -I ../stat

OBJECTS = Table.o TableEditor.o Regression.o manual_statistics.o praat_Stat.o \
   TableOfReal.o Distributions.o Distributions_and_Strings.o PairDistribution.o \
   LogisticRegression.o

all: libstat.a
libstat.a: $(OBJECTS)
	touch libstat.a
	rm libstat.a
	ar cq libstat.a $(OBJECTS)
	$(RANLIB) libstat.a

$(OBJECTS): *.h ../num/NUM.h ../audio/*.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../dwtools/*.h ../fon/*.h
