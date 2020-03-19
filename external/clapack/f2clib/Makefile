# Makefile of the library libf2c.a
# David Weenink 
# Generated on Fri Mar 13 21:58:56 2020
# with the script "CLAPACK_copyFiles_to_Praat.praat".
# For CLAPACK version 3.1.1.1.

include ../../../makefile.defs

CPPFLAGS = -I ../../../melder -I ..

OBJECTS = d_abs.o d_acos.o d_asin.o \
	d_atan.o d_atn2.o d_cnjg.o \
	d_cos.o d_cosh.o d_dim.o \
	d_exp.o d_imag.o d_int.o \
	d_lg10.o d_log.o d_mod.o \
	d_nint.o d_prod.o d_sign.o \
	d_sin.o d_sinh.o d_sqrt.o \
	d_tan.o d_tanh.o derf_.o \
	derfc_.o dolio.o dtime_.o \
	pow_di.o s_cat.o \
	s_cmp.o s_copy.o s_rnge.o 

.PHONY: all clean

all: libf2c.a

clean:
	$(RM) $(OBJECTS)
	$(RM) libf2c.a

libf2c.a: $(OBJECTS)
	touch libf2c.a
	rm libf2c.a
	$(AR) cq libf2c.a $(OBJECTS)
	$(RANLIB) libf2c.a

$(OBJECTS): ../*.h ../../../melder/*.h
