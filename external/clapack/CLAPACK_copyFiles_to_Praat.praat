# CLAPACK_copyFiles_to_Praat.praat
# djmw 20200412
# 
# LAPACK sources copied form http://www.netlib.org/clapack/
#
# This file resides in <praatsource>/external/lapack
# it automatically copies the fortran files to the external blas and lapack directories
# and generates the corresponding Makefiles.
# HOWEVER, copy the make.inc from the LAPACK source directory both to the external 
# blas and lapack directories because this may contain platform dependent things!
#
# directories: external/clapack, external/clapack/lapack, external/clapack/blas
form Copy files from CLAPACK to Praat tree
	comment Also copy
	boolean BLAS 0
	boolean LAPACK 0
	boolean F2CLIBS 0
endform


writeInfo: ""
lapack_version$ = "3.1.1.1"
lapackbase$ = "/home/david/projects/CLAPACK-" + lapack_version$ + "/"
lapackpraat$ = "/home/david/projects/praat/external/clapack/"
thisScriptName$ = "CLAPACK_copyFiles_to_Praat.praat"

sources = 1
if sources > 0
	exclude_h$ = "--exclude=f2c.h "
	excludes$ = exclude_h$ + "--exclude=xerbla.c --exclude=xerbla_array.c  --exclude=dcabs1.c " +
	... "--exclude=dzasum.c  --exclude=dznrm2.c  --exclude=f2c.h "
	if bLAS
		todir$ = lapackpraat$ + "blas/"
		fromdir$ = lapackbase$ + "BLAS/SRC/"
		includes$ = fromdir$ + "d*.c " + fromdir$ + "idamax.c "
		runSystem_nocheck: "rsync " + excludes$ + includes$ + todir$
		fromdir$ = lapackbase$ + "INCLUDE/"
		includes$ = fromdir$ + "*.h "
		runSystem_nocheck: "rsync " + excludes$ + includes$ + lapackpraat$
	endif
	# We must add 
	# #include "clapack.h"
	# #include "clapack_missing_prototypes.h"
	# to the f2c.h file, once done we don't want to overwrite it.
	exclude_h$ = "--exclude=f2c.h "

	excludes$ = exclude_h$ + "--exclude=xerbla.c --exclude=xerbla_array.c  --exclude=dcabs1.c " +
		... "--exclude=dzasum.c  --exclude=dznrm2.c  --exclude=f2c.h "
	if lAPACK
		todir$ = lapackpraat$ + "lapack/"
		fromdir$ = lapackbase$ + "SRC/"
		includes$ = fromdir$ + "d*.c " + fromdir$ + "i*.c " + fromdir$ + "lsamen.c " 
		runSystem_nocheck: "rsync " + excludes$ +  includes$ + todir$
		fromdir$ = lapackbase$ + "INSTALL/"
		includes$ = fromdir$ + "ilaver.c " + fromdir$ + "lsame.c " + fromdir$ + "dlamch.c " +
			... fromdir$ + "slamch.c"
		runSystem_nocheck: "rsync -v " + excludes$ + includes$ + todir$
	endif
	excludes$ = "--exclude=s_stop.c --exclude=s_paus.c "
	if f2CLIBS
		todir$ = lapackpraat$ + "f2clib/"
		fromdir$ = lapackbase$ + "F2CLIBS/libf2c/"
		includes$ = fromdir$ + "d_*.c " + fromdir$ + "s_*.c " + fromdir$ + "pow_d*.c "
		runSystem_nocheck: "rsync -v " + excludes$  + includes$ + todir$
	endif
endif

endif

modified_blas$ = ""
modified_clapack$ = ""
# If we really want the c++ compiler
make_rule$ = ".SUFFIXES: .c .o" + newline$ +
	... ".c.o:" + newline$ +
	... tab$ + "$(CXX) $(CXXFLAGS) -I .. -I ../../../melder -c -o $@ $<" + newline$ + newline$

makerules$ = ""
if bLAS
	todir$ = lapackpraat$ + "blas/"
	@make_makefile: todir$, "libblas.a", "*.c", ".c"
	appendInfoLine: makefile$
	writeFile: todir$ + "Makefile", makefile$
	# copy the make.inc file from the LAPACK main directory
endif

if lAPACK
	todir$ = lapackpraat$ + "lapack/"
	@make_makefile: todir$, "liblapack.a", "*.c", ".c"
	appendInfoLine: makefile$
	writeFile: todir$ + "Makefile", makefile$
endif

if f2CLIBS
	todir$ = lapackpraat$ + "f2clib/"
	@make_makefile: todir$, "libf2c.a", "*.c", ".c"
	appendInfoLine: makefile$
	writeFile: todir$ + "Makefile", makefile$
endif

# now the c interface
# we have to redefine lapacke_xerbla.c 

xerbla$ = "" +
...	"/* lapacke_xerbla.c */" + newline$ +
...	"#include <stdio.h>" + newline$ +
...	"#include ""melder.h"""  + newline$ +
...	"" + newline$ +
...	"void xerbla_( const char *name, lapack_int info) {" + newline$ +
...	"	if( info < 0 )" + newline$ +
...	"		Melder_throw (U""Wrong parameter: "", -info, U"" in "", Melder_peek8to32 (name));" + newline$ +
...	"}"

d_sign$ = "" +
... "double d_sign (double *a, double *b) {" + newline$ +
... "double x = ( *a >= 0 ? *a : - *a );" + newline$ +
... "return ( *b >= 0 ? x : -x );" + newline$ +
... "}" + newline$


#The files *ggsvd* and *ggsvp* are deprecated and lapacke_config.h, once loaded, should 
#  not be overwritten because it has been modified.
#  The interface of *ggsvd* and *ggsvp* have been removed from lapack.h and lapacke.h
# Copies of the corrected versions also reside in external/lapack.
# 
#

procedure make_makefile:  .directory$, .lib$, .file_globber$, .extension$
	makefile$ = "# Makefile of the library " + .lib$ + newline$ +
	... "# David Weenink "+ newline$ +
	... "# Generated on " + date$ () + newline$ +
	... "# with the script """ + thisScriptName$ + """."+ newline$ +
	... "# For CLAPACK version " +  lapack_version$ + "." + newline$ + newline$ +
	... "include ../../../makefile.defs" + newline$ + newline$ +
	... make_rule$ +
	... "OBJECTS = "
	.dlist = Create Strings as file list: .lib$, .directory$  + .file_globber$
	@add_make_objects: .dlist, .extension$

	makefile$  = makefile$ + newline$ + newline$ +
	...".PHONY: all clean"  + newline$ + newline$ +
	... "all: " + .lib$ + newline$ + newline$ +
	... "clean:" + newline$ + 
	... tab$ + "$(RM) $(OBJECTS)" + newline$ + 
	... tab$ + "$(RM) " + .lib$ + newline$ + newline$ +
	... .lib$ +": $(OBJECTS)" + newline$ +
	... tab$ + "touch " + .lib$ + newline$ +
	... tab$ + "rm " + .lib$ + newline$ +
	... tab$ + "$(AR) cq " + .lib$ +" $(OBJECTS)" + newline$ +
	... tab$ + "$(RANLIB) " + .lib$  + newline$ + newline$ +
	... "$(OBJECTS): ../*.h ../../../melder/*.h" + newline$
endproc

procedure add_make_objects: .list, .extension$
	selectObject: .list
	.numberOfFiles = Get number of strings
	for .ifile to  .numberOfFiles
		.fileName$ = Get string: .ifile
		.noextension$ = .fileName$ - .extension$
		.end$ = if .ifile mod 3 == 0 and .ifile < .numberOfFiles then " \" + newline$ + tab$ else  " " endif
		makefile$ = makefile$ + .noextension$ + ".o" + .end$
	endfor
endproc

# kdevelop tips:
# To remove all complete external function definitions:
# Pattern "extern "
# Template "^(\s*%s.*;\s*$)
# Replacement text "//\1"
# After search Replacement text " "

