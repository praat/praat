Making Clapack 3.1.1.1 ready for Praat

20200321, 20200417 David Weenink

I Intro.

We downloaded the latest C version, 3.1.1.1, of LAPACK from netlib as a compressed
clapack-3.1.1.1.tgz file.
It contains C versions of everything needed to run the LAPACK routines. All routines have
four different versions, i.e. for single precision (float), for double precision (double),
for single-precision complex (complex) and for double-precision complex. In general, the names
of the routines then start with s, d, c and z, respectively. For example, the routine for 
singular value decomposition has four varieties: sgesvd.c, cgesvd.c dgesvd.c and zgesvd.c.
Because all C versions were machine-translated with a program called f2c from the original Fortran
sources, there is an additional collection of small helper files which is in the F2CLIBS directory.
A heavy battery of testing code is also available, both for Clapack and for Cblas.
After unpacking, the first-level directories are TESTING, INSTALL, F2CLIBBS, BLAS, SRC and INCLUDE.
SRC contains the basic Clapack sources; BLAS contains the C version of Blas.
BLAS, F2CLIBS and TESTING contain subdirectories.

II what did we change?

The Praat version of Clapack is located in external/clapack and has only two subdirectories:
external/clapack/lapack and external/clapack/blas. Into these directories all necessary files were
copied that are needed to use the double precision LAPACK and Blas versions (mostly the file 
names that start with a 'd'.)

We made many syntactic changes in all files (most of them automatically, but many by hand).

1. We renamed all source files from *.c to *.cpp because we want the C++ compiler to work on them.
2. We replaced all 'char*' with 'const char*'. In a couple of routines we therefore needed a const_cast<char *>.
	(dgesvd, dhseqr, dnormbr, dormlq, dormqr, dormrq, dormtr and dtrtri).
3. We placed the contents of all needed F2CLIBS code as static inline code in external/clapack/f2cP.h.
4. We removed all 'extern' function definitions from all source files.
5. The xerbla routine, which is used in Clapack and Cblas to notice exceptions was replaced by our own version
	which uses the Praat exception mechanism.
6. The malloc(...) and free(...) calls in s_cat were replaced by Melder_malloc and Melder_free.
7. We freed the code from all kinds of typedefs that existed in the original f2c.h header file.
	Thanks to good regular expressions support in kdevelop we could replace 
	'doublereal' by 'double'
	'logical' by 'bool'
	'real' by 'float'
	'ftnlen' by 'integer'
	'logical (*L_fp)(...)' by 'bool (*select)(const char*, const char*[,const char*])
	'min/max by 'std::min/max'
	The rest of the typedefs and defines were not needed.
8. The interface files "clapack.h" and "cblas.h" only contain the 'double' interfaces.
	The interface for the helper routines is in the separate header file external/clapack/clapackP.h.
	
The Praat interface to the Clapack code is through dwsys/NUMlapack.h. In this file, only the routines that are
directly used in Praat have gotten a simpler C++-like interface. Pointers were removed as much as was possible.



III Some remarks.

-- Change 'doublereal' to 'double' is simple with regex

Pattern: doublereal
Template: \b%s\b
Replacement template: %s
Replace: double


-- Change 'ftnlen' to 'integer'
This could in principle be done as above but we did somewhat more as almost all uses were casts
like (ftnlen)2 and (ftlen)3.
We changed patterns like (ftnlen)2 and (ftlen)3 to 2_integer and 3_integer, respectively.

Pattern: \(ftnlen\)2
Template: \b%s\b
Replacement template: %s
Replace: 2_integer


-- Change the 'real' to 'float'; we first did:
This was more complicated because we only want to change real types and
not the word 'real' in comments!
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

Regular expressions are very powerful. Nowadays computers are very fast.
You can change a lot of files very easily and very fast. By using regular expressions 
you can very easily make a mess of hundreds of files. A 'git reset' is very useful
for these cases.

+++++ Upgrading to clapack version 3.2.1 ++++++
To test this version we first had to add the -fno-stack-protector to the CFLAGS. Without this flag the testing routines throw "stack smashing errors" and the testing cannot be completed. Adding this flag made all tests succeed.

This stack smashing error occured in dlaqr0.c where an "array" dimensioned as char jbcmpz[1] was used as follows:

	char jbcmpz[1];
	....
	if (*wantz) {
	    *(unsigned char *)jbcmpz[1] = 'V'; // illegal write to memory into one byte of the array
	} else {
	    *(unsigned char *)jbcmpz[1] = 'N'; // illegal write to memory
	}
	nwr = ilaenv_(&c__13, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);
	
Clearly jbcmpz[1] = .. is outside the boundary. This string is passed as the third argument of the ilaenv_ routine where a strlen is executed on it. So 'jbcmpz' is not properly closed either.
To correct these bugs we have to dimension jbcmpz[3] and always close the string with a null byte. 

	char jbcmpz[3];
	....
	if (*wantz) {
	    *(unsigned char *)jbcmpz[1] = 'V';
	} else {
	    *(unsigned char *)jbcmpz[1] = 'N';
	}
	jbcmpz [2] = '\0';
	nwr = ilaenv_(&c__13, "DLAQR0", jbcmpz, n, ilo, ihi, lwork);

There are more occasions in clapack that don't properly close the third argument passed to ilaenv. In routines dhseqr, dgesvd, dormbr, dormqr, dormlq, dormql, dormqr, dormrq, dormtr, and dormrz, character arrays of dimension 2 are passed without proper closing the string with a null byte. We corrected this too.

Two other bugs were found by Paul Boersma: 
The return value in dlamc3_ has to be declared volatile, otherwise the whole routine is optimized away and a wrong result may occur.
double dlamc3_(double *a, double *b) {
	volatile double ret_val;
	ret_val = *a + *b;
	return ret_val;
}
The same of course for the single and complex routines.
