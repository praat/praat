#
#  Top Level Makefile for LAPACK
#  Version 3.4.1
#
# 20200405 Adapted by David Weenink: only the essentials

include ../../makefile.defs

.PHONY: all
all: blaslib lapacklib
	cp blas/libblas.a .
	cp lapack/liblapack.a .

.PHONY: blaslib
blaslib:
	$(MAKE) -C blas

.PHONY: lapacklib
lapacklib:
	$(MAKE) -C lapack


.PHONY: clean
clean:
	$(MAKE) -C blas clean
	$(MAKE) -C lapack clean
	rm -f *.a
