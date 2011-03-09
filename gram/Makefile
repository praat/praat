# Makefile of the library "gram"
# Paul Boersma, 10 March 2011

include ../makefile.defs

CFLAGS = $(DEFS) -I ../num -I ../audio -I ../kar -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon

CPPFLAGS = $(CFLAGS)

OBJECTS = Network.o \
   OTGrammar.o OTGrammarEditor.o manual_gram.o praat_gram.o OTMulti.o OTMultiEditor.o \
   OTGrammar_ex_metrics.o OTGrammar_ex_NoCoda.o OTGrammar_ex_NPA.o OTGrammar_ex_tongueRoot.o

.PHONY: all clean

all: libgram.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libgram.a

libgram.a: $(OBJECTS)
	touch libgram.a
	rm libgram.a
	ar cq libgram.a $(OBJECTS)
	$(RANLIB) libgram.a

$(OBJECTS): *.h ../num/NUM.h ../audio/*.h ../kar/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h
