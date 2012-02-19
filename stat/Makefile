# Makefile of the library "stat"
# Paul Boersma, 14 January 2012

include ../makefile.defs

CPPFLAGS = -I ../num -I ../kar -I ../sys -I ../dwsys -I ../dwtools -I ../fon -I ../stat

OBJECTS = Table.o TableEditor.o Regression.o manual_statistics.o praat_Stat.o \
   TableOfReal.o Distributions.o Distributions_and_Strings.o PairDistribution.o \
   LogisticRegression.o

.PHONY: all clean

all: libstat.a

libstat.a: $(OBJECTS)
	touch libstat.a
	rm libstat.a
	ar cq libstat.a $(OBJECTS)
	$(RANLIB) libstat.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libstat.a

$(OBJECTS): *.h ../num/NUM.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../dwtools/*.h ../fon/*.h
