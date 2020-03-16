#
#  Top Level Makefile for LAPACK
#  Version 3.4.1
#
# 20200405 Adapted by David Weenink: only the essentials

include ../../makefile.defs

.PHONY: all
all: blaslib lapacklib f2clib
	cp blas/libblas.a .
	cp lapack/liblapack.a .
	cp f2clib/libf2c.a .

.PHONY: blaslib
blaslib:
	$(MAKE) -C blas

.PHONY: lapacklib
lapacklib:
	$(MAKE) -C lapack
	
.PHONY: f2clib
f2clib:
	$(MAKE) -C f2clib


.PHONY: clean
clean:
	$(MAKE) -C blas clean
	$(MAKE) -C lapack clean
	$(MAKE) -C libf2c clean
	rm -f *.a
