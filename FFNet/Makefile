# Makefile of the library "FFNet"
# David Weenink, 22 February 2010

include ../makefile.defs

CPPFLAGS = -I ../num -I ../dwtools -I ../fon -I ../sys -I ../dwsys -I ../stat

OBJECTS = FFNet.o \
	FFNet_Eigen.o FFNet_Matrix.o FFNet_Pattern.o \
	FFNet_Activation_Categories.o FFNet_Pattern_Activation.o \
	FFNet_Pattern_Categories.o \
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

$(OBJECTS): *.h ../num/NUM.h ../sys/*.h ../dwtools/*.h ../fon/*.h ../dwsys/*.h ../stat/*.h
