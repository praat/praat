#
#  Top Level Makefile for CLAPACK
#
# 20200320 David Weenink

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
