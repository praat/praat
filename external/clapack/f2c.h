#ifndef _f2c_h_
#define _f2c_h_
/* f2c.h  --  Fortran to C header file */

/* 20100320 David Weenink, reduced header file */

#include "melder.h"

//typedef intptr_t integer;
//typedef unsigned long int uinteger;
//typedef char *address;
//typedef short int shortint;
typedef float real;
//typedef double doublereal;
//typedef struct { real r, i; } complex;
//typedef struct { double r, i; } doublecomplex;
typedef integer logical;

//#ifdef INTEGER_STAR_8	/* Adjust for integer*8. */
//typedef long long longint;		/* system-dependent */
//typedef unsigned long long ulongint;	/* system-dependent */
//#define qbit_clear(a,b)	((a) & ~((ulongint)1 << (b)))
//#define qbit_set(a,b)	((a) |  ((ulongint)1 << (b)))
//#endif

#define TRUE_ (1)
#define FALSE_ (0)

/* Extern is for use with -E */
//#ifndef Extern
//#define Extern extern
//#endif

/* I/O stuff */

//typedef long int flag;
typedef integer ftnlen;
//typedef long int ftnlen;
//typedef long int ftnint;


//#define abs(x) ((x) >= 0 ? (x) : -(x))
//#define dabs(x) (double)abs(x)
//#define min(a,b) (std::min(a,b))
//#define max(a,b) (std::max(a,b))
//#define dmin(a,b) (double)min(a,b)
//#define dmax(a,b) (double)max(a,b)
//#define bit_test(a,b)	((a) >> (b) & 1)
//#define bit_clear(a,b)	((a) & ~((uinteger)1 << (b)))
//#define bit_set(a,b)	((a) |  ((uinteger)1 << (b)))

/* procedure parameter types for -A and -C++ */

//#define F2C_proc_par_types 1

//typedef int /* Unknown procedure type */ (*U_fp)(...);
//typedef shortint (*J_fp)(...);
//typedef integer (*I_fp)(...);
//typedef float (*R_fp)(...);
//typedef double (*D_fp)(...), (*E_fp)(...);
//typedef /* Complex */ void (*C_fp)(...);
//typedef /* Double Complex */ void (*Z_fp)(...);
typedef logical (*L_fp)(...);
//typedef shortlogical (*K_fp)(...);
//typedef /* Character */ void (*H_fp)(...);
//typedef /* Subroutine */ int (*S_fp)(...);

#endif /* _f2c_h_ */
