# Makefile of the library libblas.a
# David Weenink 
# Generated on Thu Mar 12 12:19:06 2020
# with the script "CLAPACK_copyFiles_to_Praat.praat".
# For CLAPACK version 3.1.1.1.

include ../../../makefile.defs

CPPFLAGS = -I ..

OBJECTS = dasum.o daxpy.o dcopy.o \
	ddot.o dgbmv.o dgemm.o \
	dgemv.o dger.o dnrm2.o \
	drot.o drotg.o drotm.o \
	drotmg.o dsbmv.o dscal.o \
	dsdot.o dspmv.o dspr.o \
	dspr2.o dswap.o dsymm.o \
	dsymv.o dsyr.o dsyr2.o \
	dsyr2k.o dsyrk.o dtbmv.o \
	dtbsv.o dtpmv.o dtpsv.o \
	dtrmm.o dtrmv.o dtrsm.o \
	dtrsv.o idamax.o 

.PHONY: all clean

all: libblas.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libblas.a

libblas.a: $(OBJECTS)
	touch libblas.a
	rm libblas.a
	$(AR) cq libblas.a $(OBJECTS)
	$(RANLIB) libblas.a

$(OBJECTS): ../*.h ../../../melder/*.h
