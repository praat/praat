Making clapack 3.1.1.1 ready for Praat

20200321 David Weenink

I Intro.

We downloaded the latest C-version, 3.1.1.1, of LAPACK from netlib as a compressed 
clapack-3.1.1.1.tgz file.
It contains C-versions of everything needed to run the LAPACK routines. All routines have
four different versions, i.e. for single precision (float), for double precision (double),
for single precision complex (complex) and for double precision complex. In general the names 
of the routines then start with s, d, c and z, respectively. For example, the routine for 
singular value decomposition has four varieties: sgesvd.c, cgesvd.c dgesvd.c and zgesvd.c.
Because all C-versions were machine translated with a program called f2c from the original FORTRAN
sources there is an additional collection of small helper files which is in F2CLIBS directory.
A heavy battery of testing code is also available, both for CLAPACK as well as for CBLAS.
After unpacking, the first level directories are TESTING, INSTALL, F2CLIBBS, BLAS, SRC and INCLUDE.
SRC contains the basic clapack sources, BLAS contains the C-version of BLAS.
The BLAS, F2CLIBS and TESTING contain subdirectories.

II what did we change?

The  Praat version of CLAPACK is located in external/clapack and has only two subdirectories
external/clapack/lapack and external/clapack/blas. In these directories all necessary files were
copied that are needed to use the double precision LAPACK and BLAS versions (mostly the file 
names that start with a 'd'.)

We made many syntactic changes in all files (most of them automatically and many by hand).

1. We renamed all source files from *.c to *.cpp because we want the c++ compiler to work on them.
2. We replaced all 'char*' to 'const char*'. In a couple of routines we therefore needed a const_cast<char *>.
	(dgesvd, dhseqr, dnormbr, dormlq, dormqr, dormrq, dormtr and dtrtri).
3. We placed the contents of all needed F2CLIBS code as static inline code in external/clapack/f2cP.h.
4. We removed all 'extern' function definitions from all source files.
5. The xerbla routine, which is used in clapack and cblas to notice exception was replaced by our own version
	which uses the praat exception mechanism.
6. The malloc(..) and free(.) calls in s_cat were replaced by Melder_malloc and Melder_free.
7. We freed the code from all kinds of typedef's that existed in the original f2c.h header file.
	Thanks to good regular expressions support in kdevelop we could replace 
	'doublereal' by 'double'
	'logical' by 'bool'
	'real' by 'float'
	'ftnlen' by 'integer'
	'logical (*L_fp)(...)' by 'bool (*select)(const char*, const char*[,const char*])
	'min/max by 'std::min/max'
	The rest of the typedef and defines were not needed.
8. The interface files "clapack.h" and "cblas.h" only contain the double interfaces. 
	The interface for the helper routines is in a separate header file external/clapack/clapackP.h.	
	
The praat interface to the clapack code is through dwsys/NUMlapack.h. In this file only the routines that are 
directly used in Praat have gotten a simpler C++-like interface. Pointers were removed as much as was possible.



III Some remarks.

-- Change 'doublereal' to 'double' is simple with regex

Pattern: doublereal
Template: \b%s\b
Replacement template: %s
Replace: double


-- Change 'ftnlen' to 'integer'
This could in principle be done as above but we did somewhat more as almost al uses were casts 
like (ftnlen)2 and (ftlen)3.
We changed patterns like (ftnlen)2 and (ftlen)3 to 2_integer and 3_integer, respectively.

Pattern: \(ftnlen\)2
Template: \b%s\b
Replacement template: %s
Replace: 2_integer


-- Change the 'real' to 'float' we first did:
This was more complicated because we only want to change real types and
not the word real in comments!
We first changed all 'real' to 'float' as above
Pattern: real
Template: \b%s\b
Replacement template: %s
Replace: float

Next we change back the 'float's in comments which always start with "/*" 
and end with "*/"

Pattern: float
Template: (^[/*][*].*\b)%s(\b.*[*/]$)
Replacement template: \1%s\2
Replace: real

Regular expressions are very powerfull. Nowadays computer are very fast. 
You can change a lot of files very easily and very fast. By using regular expressions 
you can very easily make a mess of hundreds of files. A 'git reset' is very usefull
for these cases.
