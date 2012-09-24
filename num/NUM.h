#ifndef _NUM_h_
#define _NUM_h_
/* NUM.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* "NUM" = "NUMerics" */
/* More mathematical and numerical things than there are in <math.h>. */

/********** Inherit all the ANSI routines from math.h **********/

/* On the sgi, math.h declares some bessel functions. */
/* The following statements suppress these declarations */
/* so that the compiler will give no warnings */
/* when you redeclare y0 etc. in your code. */
#ifdef sgi
	#define y0 sgi_y0
	#define y1 sgi_y1
	#define yn sgi_yn
	#define j0 sgi_j0
	#define j1 sgi_j1
	#define jn sgi_jn
#endif
#include <math.h>
#ifdef sgi
	#undef y0
	#undef y1
	#undef yn
	#undef j0
	#undef j1
	#undef jn
#endif
#include <stdio.h>
#include <wchar.h>
#include "abcio.h"
#define NUMlog2(x)  (log (x) * NUMlog2e)

void NUMinit (void);

double NUMpow (double base, double exponent);   /* Zero for non-positive base. */
void NUMshift (double *x, double xfrom, double xto);
void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto);

/********** Constants **********
 * Forty-digit constants computed by e.g.:
 *    bc -l
 *       scale=42
 *       print e(1)
 * Then rounding away the last two digits.
 */
//      print e(1)
#define NUMe  2.7182818284590452353602874713526624977572
//      print 1/l(2)
#define NUMlog2e  1.4426950408889634073599246810018921374266
//      print l(10)/l(2)
#define NUMlog2_10  3.3219280948873623478703194294893901758648
//      print 1/l(10)
#define NUMlog10e  0.4342944819032518276511289189166050822944
//      print l(2)/l(10)
#define NUMlog10_2  0.3010299956639811952137388947244930267682
//      print l(2)
#define NUMln2  0.6931471805599453094172321214581765680755
//      print l(10)
#define NUMln10  2.3025850929940456840179914546843642076011
//      print a(1)*8
#define NUM2pi  6.2831853071795864769252867665590057683943
//      print a(1)*4
#define NUMpi  3.1415926535897932384626433832795028841972
//      print a(1)*2
#define NUMpi_2  1.5707963267948966192313216916397514420986
//      print a(1)
#define NUMpi_4  0.7853981633974483096156608458198757210493
//      print 0.25/a(1)
#define NUM1_pi  0.3183098861837906715377675267450287240689
//      print 0.5/a(1)
#define NUM2_pi  0.6366197723675813430755350534900574481378
//      print sqrt(a(1)*4)
#define NUMsqrtpi  1.7724538509055160272981674833411451827975
//      print sqrt(a(1)*8)
#define NUMsqrt2pi  2.5066282746310005024157652848110452530070
//      print 1/sqrt(a(1)*8)
#define NUM1_sqrt2pi  0.3989422804014326779399460599343818684759
//      print 1/sqrt(a(1))
#define NUM2_sqrtpi  1.1283791670955125738961589031215451716881
//      print l(a(1)*4)
#define NUMlnpi  1.1447298858494001741434273513530587116473
//      print sqrt(2)
#define NUMsqrt2  1.4142135623730950488016887242096980785697
//      print sqrt(0.5)
#define NUMsqrt1_2  0.7071067811865475244008443621048490392848
//      print sqrt(3)
#define NUMsqrt3  1.7320508075688772935274463415058723669428
//      print sqrt(5)
#define NUMsqrt5  2.2360679774997896964091736687312762354406
//      print sqrt(6)
#define NUMsqrt6  2.4494897427831780981972840747058913919659
//      print sqrt(7)
#define NUMsqrt7  2.6457513110645905905016157536392604257102
//      print sqrt(8)
#define NUMsqrt8  2.8284271247461900976033774484193961571393
//      print sqrt(10)
#define NUMsqrt10  3.1622776601683793319988935444327185337196
//      print sqrt(5)/2-0.5
#define NUM_goldenSection  0.6180339887498948482045868343656381177203
// The Euler-Mascheroni constant cannot be computed by bc.
// Instead we use the 40 digits computed by Johann von Soldner in 1809.
#define NUM_euler  0.5772156649015328606065120900824024310422
#define NUMundefined  HUGE_VAL
#define NUMdefined(x)  ((x) != NUMundefined)

/********** Arrays with one index (NUMarrays.c) **********/

void * NUMvector (long elementSize, long lo, long hi);
/*
	Function:
		create a vector [lo...hi] with all values initialized to 0.
		Queue an error message and return NULL if anything went wrong.
	Preconditions:
		hi >= lo;
*/

void NUMvector_free (long elementSize, void *v, long lo);
/*
	Function:
		destroy a vector v that was created with NUMvector.
	Preconditions:
		lo must have the same values as with the creation of the vector.
*/

void * NUMvector_copy (long elementSize, void *v, long lo, long hi);
/*
	Function:
		copy (part of) a vector v, which need not have been created with NUMvector, to a new one.
		Queue an error message and return NULL if anything went wrong.
	Preconditions:
		if v != NULL, the values v [lo..hi] must exist.
*/

void NUMvector_copyElements (long elementSize, void *v, void *to, long lo, long hi);
/*
	copy the vector elements v [lo..hi] to those of a vector 'to'.
	These vectors need not have been created by NUMvector.
*/

bool NUMvector_equal (long elementSize, void *v1, void *v2, long lo, long hi);
/*
	return 1 if the vector elements v1 [lo..hi] are equal
	to the corresponding elements of the vector v2; otherwise, return 0.
	The vectors need not have been created by NUMvector.
*/

void NUMvector_append (long elementSize, void **v, long lo, long *hi);
void NUMvector_insert (long elementSize, void **v, long lo, long *hi, long position);
/*
	add one element to the vector *v.
	The new element is initialized to zero.
	On success, *v points to the new vector, and *hi is incremented by 1.
	On failure, *v and *hi are not changed.
*/

/********** Arrays with two indices (NUMarrays.c) **********/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2);
/*
	Function:
		create a matrix [row1...row2] [col1...col2] with all values initialized to 0.
		Queue an error message and return NULL if anything went wrong.
	Preconditions:
		row2 >= row1;
		col2 >= col1;
*/

void NUMmatrix_free (long elementSize, void *m, long row1, long col1);
/*
	Function:
		destroy a matrix m created with NUM...matrix.
	Preconditions:
		if m != NULL: row1 and row2
		must have the same value as with the creation of the matrix.
*/

void * NUMmatrix_copy (long elementSize, void *m, long row1, long row2, long col1, long col2);
/*
	Function:
		copy (part of) a matrix m, wich does not have to be created with NUMmatrix, to a new one.
		Queue an error message and return NULL if anything went wrong.
	Preconditions:
		if m != NULL: the values m [rowmin..rowmax] [colmin..colmax] must exist.
*/

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2);
/*
	copy the matrix elements m [r1..r2] [c1..c2] to those of a matrix 'to'.
	These matrices need not have been created by NUMmatrix.
*/

bool NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2);
/*
	return 1 if the matrix elements m1 [r1..r2] [c1..c2] are equal
	to the corresponding elements of the matrix m2; otherwise, return 0.
	The matrices need not have been created by NUM...matrix.
*/

long NUM_getTotalNumberOfArrays (void);   /* For debugging. */

/********** Special functions (NUM.c) **********/

double NUMlnGamma (double x);
double NUMbeta (double z, double w);
double NUMbesselI (long n, double x);   /* Precondition: n >= 0 */
double NUMbessel_i0_f (double x);
double NUMbessel_i1_f (double x);
double NUMbesselK (long n, double x);   /* Preconditions: n >= 0 && x > 0.0 */
double NUMbessel_k0_f (double x);
double NUMbessel_k1_f (double x);
double NUMbesselK_f (long n, double x);
double NUMsigmoid (double x);   /* Correct also for large positive or negative x. */
double NUMinvSigmoid (double x);
double NUMerfcc (double x);
double NUMgaussP (double z);
double NUMgaussQ (double z);
double NUMincompleteGammaP (double a, double x);
double NUMincompleteGammaQ (double a, double x);
double NUMchiSquareP (double chiSquare, double degreesOfFreedom);
double NUMchiSquareQ (double chiSquare, double degreesOfFreedom);
double NUMcombinations (long n, long k);
double NUMincompleteBeta (double a, double b, double x);   // incomplete beta function Ix(a,b). Preconditions: a, b > 0; 0 <= x <= 1
double NUMbinomialP (double p, double k, double n);
double NUMbinomialQ (double p, double k, double n);
double NUMinvBinomialP (double p, double k, double n);
double NUMinvBinomialQ (double p, double k, double n);

/********** Auditory modelling (NUMear.c) **********/

double NUMhertzToBark (double hertz);
double NUMbarkToHertz (double bark);
double NUMphonToDifferenceLimens (double phon);
double NUMdifferenceLimensToPhon (double ndli);
double NUMsoundPressureToPhon (double soundPressure, double bark);
double NUMhertzToMel (double hertz);
double NUMmelToHertz (double mel);
double NUMhertzToSemitones (double hertz);
double NUMsemitonesToHertz (double semitones);
double NUMerb (double f);
double NUMhertzToErb (double hertz);
double NUMerbToHertz (double erb);

/********** Sorting (NUMsort.c) **********/

void NUMsort_d (long n, double ra []);   /* Heap sort. */
void NUMsort_i (long n, int ra []);
void NUMsort_l (long n, long ra []);
void NUMsort_str (long n, wchar_t *a []);
void NUMsort_p (long n, void *a [], int (*compare) (const void *, const void *));

double NUMquantile (long n, double a [], double factor);
/*
	An estimate of the quantile 'factor' (between 0 and 1) of the distribution
	from which the set 'a [1..n]' is a sorted array of random samples.
	For instance, if 'factor' is 0.5, this function returns an estimate of
	the median of the distribution underlying the sorted set a [].
	If your array has not been sorted, first sort it with NUMsort (n, a).
*/

/********** Interpolation and optimization (NUM.c) **********/

// Special values for interpolationDepth:
#define NUM_VALUE_INTERPOLATE_NEAREST  0
#define NUM_VALUE_INTERPOLATE_LINEAR  1
#define NUM_VALUE_INTERPOLATE_CUBIC  2
// Higher values than 2 yield a true sinc interpolation. Here are some examples:
#define NUM_VALUE_INTERPOLATE_SINC70  70
#define NUM_VALUE_INTERPOLATE_SINC700  700
double NUM_interpolate_sinc (double y [], long nx, double x, long interpolationDepth);

#define NUM_PEAK_INTERPOLATE_NONE  0
#define NUM_PEAK_INTERPOLATE_PARABOLIC  1
#define NUM_PEAK_INTERPOLATE_CUBIC  2
#define NUM_PEAK_INTERPOLATE_SINC70  3
#define NUM_PEAK_INTERPOLATE_SINC700  4

double NUMimproveExtremum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real, int isMaximum);
double NUMimproveMaximum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);
double NUMimproveMinimum (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);

void NUM_viterbi (
	long numberOfFrames, long maxnCandidates,
	long (*getNumberOfCandidates) (long iframe, void *closure),
	double (*getLocalCost) (long iframe, long icand, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, void *closure),
	void (*putResult) (long iframe, long place, void *closure),
	void *closure);

void NUM_viterbi_multi (
	long nframe, long ncand, int ntrack,
	double (*getLocalCost) (long iframe, long icand, int itrack, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, int itrack, void *closure),
	void (*putResult) (long iframe, long place, int itrack, void *closure),
	void *closure);

/********** Metrics (NUM.c) **********/

int NUMrotationsPointInPolygon
	(double x0, double y0, long n, double x [], double y []);
/*
	Returns the number of times that the closed polygon
	(x [1], y [1]), (x [2], y [2]),..., (x [n], y [n]), (x [1], y [1]) encloses the point (x0, y0).
	The result is positive if the polygon encloses the point in the
	anti-clockwise direction, and negative if the direction is clockwise.
	The result is 0 if the point is outside the polygon.
	If the point is on the polygon, the result is unpredictable.
*/

/********** Random numbers (NUMrandom.c) **********/

void NUMrandomRestart (unsigned long seed);
/*
	Not needed for starting the random generator for the first time;
	that will be done on the basis of the current time.
*/

double NUMrandomFraction (void);

double NUMrandomUniform (double lowest, double highest);

long NUMrandomInteger (long lowest, long highest);

double NUMrandomGauss (double mean, double standardDeviation);
	
double NUMrandomPoisson (double mean);

void NUMfbtoa (double formant, double bandwidth, double dt, double *a1, double *a2);
void NUMfilterSecondOrderSection_a (double x [], long n, double a1, double a2);
void NUMfilterSecondOrderSection_fb (double x [], long n, double dt, double formant, double bandwidth);
double NUMftopreemphasis (double f, double dt);
void NUMpreemphasize_a (double x [], long n, double preemphasis);
void NUMdeemphasize_a (double x [], long n, double preemphasis);
void NUMpreemphasize_f (double x [], long n, double dt, double frequency);
void NUMdeemphasize_f (double x [], long n, double dt, double frequency);
void NUMautoscale (double x [], long n, double scale);

/* The following ANSI-C power trick generates the declarations of 156 functions. */
#define FUNCTION(type,storage)  \
	void NUMvector_writeText_##storage (const type *v, long lo, long hi, MelderFile file, const wchar_t *name); \
	void NUMvector_writeBinary_##storage (const type *v, long lo, long hi, FILE *f); \
	void NUMvector_writeCache_##storage (const type *v, long lo, long hi, CACHE *f); \
	type * NUMvector_readText_##storage (long lo, long hi, MelderReadText text, const char *name); \
	type * NUMvector_readBinary_##storage (long lo, long hi, FILE *f); \
	type * NUMvector_readCache_##storage (long lo, long hi, CACHE *f); \
	void NUMmatrix_writeText_##storage (type **v, long r1, long r2, long c1, long c2, MelderFile file, const wchar_t *name); \
	void NUMmatrix_writeBinary_##storage (type **v, long r1, long r2, long c1, long c2, FILE *f); \
	void NUMmatrix_writeCache_##storage (type **v, long r1, long r2, long c1, long c2, CACHE *f); \
	type ** NUMmatrix_readText_##storage (long r1, long r2, long c1, long c2, MelderReadText text, const char *name); \
	type ** NUMmatrix_readBinary_##storage (long r1, long r2, long c1, long c2, FILE *f); \
	type ** NUMmatrix_readCache_##storage (long r1, long r2, long c1, long c2, CACHE *f);
FUNCTION (signed char, i1)
FUNCTION (int, i2)
FUNCTION (long, i4)
FUNCTION (unsigned char, u1)
FUNCTION (unsigned int, u2)
FUNCTION (unsigned long, u4)
FUNCTION (double, r4)
FUNCTION (double, r8)
FUNCTION (fcomplex, c8)
FUNCTION (dcomplex, c16)
#undef FUNCTION

/*
void NUMvector_writeBinary_r8 (const double *v, long lo, long hi, FILE *f);   // etc
	write the vector elements v [lo..hi] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUM...vector.
double * NUMvector_readText_r8 (long lo, long hi, MelderReadString *text, const char *name);   // etc
	create and read a vector as text.
	Throw an error message if anything went wrong.
	Every element is supposed to be on the beginning of a line.
double * NUMvector_readBinary_r8 (long lo, long hi, FILE *f);   // etc
	create and read a vector as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
void NUMvector_writeText_r8 (const double *v, long lo, long hi, MelderFile file, const wchar_t *name);   // etc
	write the vector elements v [lo..hi] as text to the open file,
	each element on its own line, preceded by "name [index]: ".
	Throw an error message if anything went wrong.
	The vectors need not have been created by NUMvector.
void NUMmatrix_writeText_r8 (double **m, long r1, long r2, long c1, long c2, MelderFile file, const wchar_t *name);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as text to the open file.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
void NUMmatrix_writeBinary_r8 (double **m, long r1, long r2, long c1, long c2, FILE *f);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as machine-independent
	binary data to the stream f.
	Throw an error message if anything went wrong.
	The matrices need not have been created by NUMmatrix.
double ** NUMmatrix_readText_r8 (long r1, long r2, long c1, long c2, MelderReadString *text, const char *name);   // etc
	create and read a matrix as text.
	Throw an error message if anything went wrong.
double ** NUMmatrix_readBinary_r8 (long r1, long r2, long c1, long c2, FILE *f);   // etc
	create and read a matrix as machine-independent binary data from the stream f.
	Throw an error message if anything went wrong.
*/

typedef struct structNUMlinprog *NUMlinprog;
void NUMlinprog_delete (NUMlinprog me);
NUMlinprog NUMlinprog_new (bool maximize);
void NUMlinprog_addVariable (NUMlinprog me, double lowerBound, double upperBound, double coeff);
void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound);
void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient);
void NUMlinprog_run (NUMlinprog me);
double NUMlinprog_getPrimalValue (NUMlinprog me, long ivar);

template <class T>
T* NUMvector (long from, long to) {
	T* result = static_cast <T*> (NUMvector (sizeof (T), from, to));
	return result;
}

template <class T>
void NUMvector_free (T* ptr, long from) {
	NUMvector_free (sizeof (T), ptr, from);
}

template <class T>
T* NUMvector_copy (T* ptr, long lo, long hi) {
	T* result = static_cast <T*> (NUMvector_copy (sizeof (T), ptr, lo, hi));
	return result;
}

template <class T>
bool NUMvector_equal (T* v1, T* v2, long lo, long hi) {
	return NUMvector_equal (sizeof (T), v1, v2, lo, hi);
}

template <class T>
void NUMvector_copyElements (T* vfrom, T* vto, long lo, long hi) {
	NUMvector_copyElements (sizeof (T), vfrom, vto, lo, hi);
}

template <class T>
void NUMvector_append (T** v, long lo, long *hi) {
	NUMvector_append (sizeof (T), (void**) v, lo, hi);
}

template <class T>
void NUMvector_insert (T** v, long lo, long *hi, long position) {
	NUMvector_insert (sizeof (T), (void**) v, lo, hi, position);
}

template <class T>
class autoNUMvector {
	T* d_ptr;
	long d_from;
public:
	autoNUMvector<T> (long from, long to) : d_from (from) {
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
	autoNUMvector (T *ptr, long from) : d_ptr (ptr), d_from (from) {
	}
	autoNUMvector () : d_ptr (NULL), d_from (1) {
	}
	~autoNUMvector<T> () {
		if (d_ptr) NUMvector_free (sizeof (T), d_ptr, d_from);
	}
	T& operator[] (long i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long from, long to) {
		if (d_ptr) {
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = NULL;
		}
		d_from = from;
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
};

template <class T>
T** NUMmatrix (long row1, long row2, long col1, long col2) {
	T** result = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2));
	return result;
}

template <class T>
void NUMmatrix_free (T** ptr, long row1, long col1) {
	NUMmatrix_free (sizeof (T), ptr, row1, col1);
}

template <class T>
T** NUMmatrix_copy (T** ptr, long row1, long row2, long col1, long col2) {
	T** result = static_cast <T**> (NUMmatrix_copy (sizeof (T), ptr, row1, row2, col1, col2));
	return result;
}

template <class T>
bool NUMmatrix_equal (T** m1, T** m2, long row1, long row2, long col1, long col2) {
	return NUMmatrix_equal (sizeof (T), m1, m2, row1, row2, col1, col2);
}

template <class T>
void NUMmatrix_copyElements (T** mfrom, T** mto, long row1, long row2, long col1, long col2) {
	NUMmatrix_copyElements (sizeof (T), mfrom, mto, row1, row2, col1, col2);
}

template <class T>
class autoNUMmatrix {
	T** d_ptr;
	long d_row1, d_col1;
public:
	autoNUMmatrix (long row1, long row2, long col1, long col2) : d_row1 (row1), d_col1 (col1) {
		d_ptr = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2));
	}
	autoNUMmatrix (T **ptr, long row1, long col1) : d_ptr (ptr), d_row1 (row1), d_col1 (col1) {
	}
	autoNUMmatrix () : d_ptr (NULL), d_row1 (0), d_col1 (0) {
	}
	~autoNUMmatrix () {
		if (d_ptr) NUMmatrix_free (sizeof (T), d_ptr, d_row1, d_col1);
	}
	T*& operator[] (long row) {
		return d_ptr [row];
	}
	T** peek () const {
		return d_ptr;
	}
	T** transfer () {
		T** temp = d_ptr;
		d_ptr = NULL;
		return temp;
	}
	void reset (long row1, long row2, long col1, long col2) {
		if (d_ptr) {
			NUMmatrix_free (sizeof (T), d_ptr, d_row1, d_col1);
			d_ptr = NULL;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = static_cast <T**> (NUMmatrix (sizeof (T), row1, row2, col1, col2));
	}
};

template <class T>
class autodatavector {
	T* d_ptr;
	long d_from, d_to;
public:
	autodatavector<T> (long from, long to) : d_from (from), d_to (to) {
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
	autodatavector (T *ptr, long from, long to) : d_ptr (ptr), d_from (from), d_to (to) {
	}
	autodatavector () : d_ptr (NULL), d_from (1), d_to (0) {
	}
	~autodatavector<T> () {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				Melder_free (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
		}
	}
	T& operator[] (long i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = NULL;   // make the pointer non-automatic again
		return temp;
	}
	void reset (long from, long to) {
		if (d_ptr) {
			for (long i = d_from; i <= d_to; i ++)
				Melder_free (d_ptr [i]);
			NUMvector_free (sizeof (T), d_ptr, d_from);
			d_ptr = NULL;
		}
		d_from = from;   // this assignment is safe, because d_ptr is NULL
		d_to = to;
		d_ptr = static_cast <T*> (NUMvector (sizeof (T), from, to));
	}
};

typedef autodatavector <wchar_t *> autostringvector;
typedef autodatavector <char *> autostring8vector;

/* End of file NUM.h */
#endif
