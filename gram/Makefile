# Makefile of the library "gram"
# Paul Boersma 2018-08-10

include ../makefile.defs

CPPFLAGS = -I ../kar -I ../melder -I ../sys -I ../dwsys -I ../stat -I ../dwtools -I ../fon

OBJECTS = Network.o \
   OTGrammar.o OTGrammarEditor.o manual_gram.o praat_gram.o OTMulti.o OTMultiEditor.o \
   OTGrammar_ex_metrics.o OTGrammar_ex_NoCoda.o OTGrammar_ex_NPA.o OTGrammar_ex_tongueRoot.o \
   OTMulti_ex_metrics.o Layer.o Net.o \
   NoulliGrid.o NoulliGridEditor.o

.PHONY: all clean

all: libgram.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libgram.a

libgram.a: $(OBJECTS)
	touch libgram.a
	rm libgram.a
	$(AR) cq libgram.a $(OBJECTS)
	$(RANLIB) libgram.a

$(OBJECTS): *.h ../kar/*.h ../melder/*.h ../sys/*.h ../dwsys/*.h ../stat/*.h ../dwtools/*.h ../fon/*.h
