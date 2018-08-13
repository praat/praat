# Makefile of the library "stat"
# Paul Boersma, 10 August 2018

include ../makefile.defs

CPPFLAGS = -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../dwtools -I ../fon -I ../stat

OBJECTS = Table.o TableEditor.o Regression.o manual_statistics.o \
   praat_TableOfReal.o praat_Stat.o \
   TableOfReal.o Distributions.o Distributions_and_Strings.o PairDistribution.o \
   LogisticRegression.o

.PHONY: all clean

all: libstat.a

libstat.a: $(OBJECTS)
	touch libstat.a
	rm libstat.a
	$(AR) cq libstat.a $(OBJECTS)
	$(RANLIB) libstat.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libstat.a

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../dwtools/*.h ../fon/*.h ../stat/*.h
