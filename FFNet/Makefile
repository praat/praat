# Makefile of the library "FFNet"
# David Weenink, 2 June 2017

include ../makefile.defs

CPPFLAGS = -I ../num -I ../kar -I ../dwtools -I ../gram -I ../fon -I ../sys -I ../dwsys -I ../stat

OBJECTS = FFNet.o \
	FFNet_Eigen.o FFNet_Matrix.o FFNet_PatternList.o \
	FFNet_ActivationList_Categories.o FFNet_PatternList_ActivationList.o \
	FFNet_PatternList_Categories.o RBM_extensions.o \
	praat_FFNet_init.o manual_FFNet.o

.PHONY: all clean

all: libFFNet.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libFFNet.a

libFFNet.a: $(OBJECTS)
	touch libFFNet.a
	rm libFFNet.a
	$(AR) cq libFFNet.a $(OBJECTS)
	$(RANLIB) libFFNet.a

$(OBJECTS): *.h ../sys/*.h ../dwtools/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h
