#ifndef _NUM_h_
#define _NUM_h_
/* NUM.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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

/*
 * pb 2002/06/01
 * pb 2002/10/31 NUMlog2 instead of log2
 * pb 2003/06/23 removed NUMbesselJ and NUMbesselY
 * pb 2004/10/16 replaced struct Type with struct structType
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
#ifndef _abcio_h_
	#include "abcio.h"
#endif
#define NUMlog2(x)  (log (x) * NUMlog2e)
/*
 * CodeWarrior used to have an incorrect pow function, in which the base cannot be 0.
 */
#ifdef _WIN32xxxxx
	double pow_WIN32_CW11 (double base, double exponent);
	#define pow(base,exponent)   pow_WIN32_CW11 (base, exponent)
#endif

/********** Constants **********/

#define NUMe  2.7182818284590452354
#define NUMlog2e  1.4426950408889634074
#define NUMlog10e  0.43429448190325182765
#define NUMln2	  0.69314718055994530942
#define NUMln10  2.30258509299404568402
#define NUMpi  3.14159265358979323846
#define NUMpi_2  1.57079632679489661923
#define NUMpi_4  0.78539816339744830962
#define NUM1_pi  0.31830988618379067154
#define NUM2_pi  0.63661977236758134308
#define NUM2_sqrtpi  1.12837916709551257390
#define NUMsqrt2  1.41421356237309504880
#define NUMsqrt1_2  0.70710678118654752440
#define NUM_goldenSection  0.618033988749895
#define NUMundefined  HUGE_VAL
#define NUMdefined(x)  ((x) != NUMundefined)

/********** Arrays with one index (NUMarrays.c) **********/

void * NUMvector (long elementSize, long lo, long hi);
/*
	Function:
		create a vector [lo...hi] with all values initialized to 0.
		Queue an error message and return NULL if something went wrong.
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
		Queue an error message and return NULL if something went wrong.
	Preconditions:
		if v != NULL, the values v [lo..hi] must exist.
*/

void NUMvector_copyElements (long elementSize, void *v, void *to, long lo, long hi);
/*
	copy the vector elements v [lo..hi] to those of a vector 'to'.
	These vectors need not have been created by NUMvector.
*/

int NUMvector_equal (long elementSize, void *v1, void *v2, long lo, long hi);
/*
	return 1 if the vector elements v1 [lo..hi] are equal
	to the corresponding elements of the vector v2; otherwise, return 0.
	The vectors need not have been created by NUMvector.
*/

/********** Arrays with two indices (NUMarrays.c) **********/

void * NUMmatrix (long elementSize, long row1, long row2, long col1, long col2);
/*
	Function:
		create a matrix [row1...row2] [col1...col2] with all values initialized to 0.
		Queue an error message and return NULL if something went wrong.
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

void * NUMmatrix_copy (long elementSize, void * m, long row1, long row2, long col1, long col2);
/*
	Function:
		copy (part of) a matrix m, wich does not have to be created with NUMmatrix, to a new one.
		Queue an error message and return NULL if something went wrong.
	Preconditions:
		if m != NULL: the values m [rowmin..rowmax] [colmin..colmax] must exist.
*/

void NUMmatrix_copyElements (long elementSize, void *m, void *to, long row1, long row2, long col1, long col2);
/*
	copy the matrix elements m [r1..r2] [c1..c2] to those of a matrix 'to'.
	These matrices need not have been created by NUMmatrix.
*/

int NUMmatrix_equal (long elementSize, void *m1, void *m2, long row1, long row2, long col1, long col2);
/*
	return 1 if the matrix elements m1 [r1..r2] [c1..c2] are equal
	to the corresponding elements of the matrix m2; otherwise, return 0.
	The matrices need not have been created by NUM...matrix.
*/

/* The following ANSI-C power trick generates the declarations of 130 functions. */
#define FUNCTION(t,type)  \
	type * NUM##t##vector (long lo, long hi); \
	void NUM##t##vector_free (type *v, long lo); \
	type * NUM##t##vector_copy (const type *v, long lo, long hi); \
	void NUM##t##vector_copyElements (const type *v, type *to, long lo, long hi); \
	int NUM##t##vector_equal (const type *v1, const type *v2, long lo, long hi); \
	type ** NUM##t##matrix (long row1, long row2, long col1, long col2); \
	void NUM##t##matrix_free (type **m, long row1, long col1); \
	type ** NUM##t##matrix_copy (type **m, long row1, long row2, long col1, long col2); \
	void NUM##t##matrix_copyElements (type **m, type **to, long row1, long row2, long col1, long col2); \
	int NUM##t##matrix_equal (type **m1, type **m2, long row1, long row2, long col1, long col2);
FUNCTION (b, signed char)
FUNCTION (s, short)
FUNCTION (i, int)
FUNCTION (l, long)
FUNCTION (ub, unsigned char)
FUNCTION (us, unsigned short)
FUNCTION (ui, unsigned int)
FUNCTION (ul, unsigned long)
FUNCTION (f, float)
FUNCTION (d, double)
FUNCTION (fc, fcomplex)
FUNCTION (dc, dcomplex)
FUNCTION (c, char)
#undef FUNCTION

#define NUMstructvector(Type,lo,hi)  \
	NUMvector (sizeof (struct struct##Type), lo, hi)
#define NUMstructvector_free(Type,v,lo)  \
	NUMvector_free (sizeof (struct struct##Type), v, lo)
#define NUMstructvector_copy(Type,v,lo,hi)  \
	NUMvector_copy (sizeof (struct struct##Type), v, lo, hi)
#define NUMstructvector_copyElements(Type,v,to,lo,hi)  \
	NUMvector_copyElements (sizeof (struct struct##Type), v, to, lo, hi)
#define NUMstructvector_equal(Type,v1,v2,lo,hi)  \
	NUMvector_equal (sizeof (struct struct##Type), v1, v2, lo, hi)

#define NUMpvector(lo,hi)  \
	NUMvector (sizeof (void *), lo, hi)
#define NUMpvector_free(v,lo)  \
	NUMvector_free (sizeof (void *), v, lo)
#define NUMpvector_copy(v,lo,hi)  \
	NUMvector_copy (sizeof (void *), v, lo, hi)
#define NUMpvector_copyElements(v,to,lo,hi)  \
	NUMvector_copyElements (sizeof (void *), v, to, lo, hi)
#define NUMpvector_equal(v1,v2,lo,hi)  \
	NUMvector_equal (sizeof (void *), v1, v2, lo, hi)

#define NUMstructmatrix(Type,row1,row2,col1,col2)  \
	NUMmatrix (sizeof (struct struct##Type), row1, row2, col1, col2)
#define NUMstructmatrix_free(Type,m,row1,col1)  \
	NUMmatrix_free (sizeof (struct struct##Type), m, row1, col1)
#define NUMstructmatrix_copy(Type,m,row1,row2,col1,col2)  \
	NUMmatrix_copy (sizeof (struct struct##Type), m, row1, row2, col1, col2)
#define NUMstructmatrix_copyElements(Type,m,to,row1,row2,col1,col2)  \
	NUMmatrix_copyElements (sizeof (struct struct##Type), m, to, row1, row2, col1, col2)
#define NUMstructmatrix_equal(Type,m1,m2,row1,row2,col1,col2)  \
	NUMmatrix_equal (sizeof (struct struct##Type), m1, m2, row1, row2, col1, col2)

#define NUMpmatrix(row1,row2,col1,col2)  \
	NUMmatrix (sizeof (void *), row1, row2, col1, col2)
#define NUMpmatrix_free(m,row1,col1)  \
	NUMmatrix_free (sizeof (void *), m, row1, col1)
#define NUMpmatrix_copy(m,row1,row2,col1,col2)  \
	NUMmatrix_copy (sizeof (void *), m, row1, row2, col1, col2)
#define NUMpmatrix_copyElements(m,to,row1,row2,col1,col2)  \
	NUMmatrix_copyElements (sizeof (void *), m, to, row1, row2, col1, col2)
#define NUMpmatrix_equal(m1,m2,row1,row2,col1,col2)  \
	NUMmatrix_equal (sizeof (void *), m1, m2, row1, row2, col1, col2)

long NUM_getTotalNumberOfArrays (void);   /* For debugging. */

/********** Special functions (NUMfunctions.c) **********/

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
double NUMerfcc (double x);
double NUMgaussP (double z);
double NUMgaussQ (double z);
double NUMincompleteGammaP (double a, double x);
double NUMincompleteGammaQ (double a, double x);
double NUMchiSquareP (double chiSquare, long degreesOfFreedom);
double NUMchiSquareQ (double chiSquare, long degreesOfFreedom);
double NUMcombinations (long n, long k);
double NUMincompleteBeta (double a, double b, double x);
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

#define NUMsort NUMsort_f
void NUMsort_f (long n, float ra []);   /* Heap sort. */
void NUMsort_d (long n, double ra []);
void NUMsort_i (long n, int ra []);
void NUMsort_l (long n, long ra []);
void NUMsort_str (long n, char *a []);
void NUMsort_p (long n, void *a [], int (*compare) (const void *, const void *));

double NUMquantile_f (long n, float  a [], double factor);
double NUMquantile_d (long n, double a [], double factor);
#define NUMquantile NUMquantile_f
/*
	An estimate of the quantile 'factor' (between 0 and 1) of the distribution
	from which the set 'a [1..n]' is a sorted array of random samples.
	For instance, if 'factor' is 0.5, this function returns an estimate of
	the median of the distribution underlying the sorted set a [].
	If your array has not been sorted, first sort it with NUMsort (n, a).
*/

/********** Interpolation and optimization (NUM_interpol.c) **********/

double NUM_interpolate_sinc_f (float  y [], long nx, double x, long maxDepth);
double NUM_interpolate_sinc_d (double y [], long nx, double x, long maxDepth);
#define NUM_interpolate_sinc NUM_interpolate_sinc_f

#define NUM_PEAK_INTERPOLATE_NONE  0
#define NUM_PEAK_INTERPOLATE_PARABOLIC  1
#define NUM_PEAK_INTERPOLATE_CUBIC  2
#define NUM_PEAK_INTERPOLATE_SINC70  3
#define NUM_PEAK_INTERPOLATE_SINC700  4

double NUMimproveExtremum_f (float *y, long nx, long ixmid, int interpolation, double *ixmid_real, int isMaximum);
double NUMimproveMaximum_f (float *y, long nx, long ixmid, int interpolation, double *ixmid_real);
double NUMimproveMinimum_f (float *y, long nx, long ixmid, int interpolation, double *ixmid_real);
double NUMimproveExtremum_d (double *y, long nx, long ixmid, int interpolation, double *ixmid_real, int isMaximum);
double NUMimproveMaximum_d (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);
double NUMimproveMinimum_d (double *y, long nx, long ixmid, int interpolation, double *ixmid_real);
#define NUMimproveMaximum NUMimproveMaximum_f
#define NUMimproveMinimum NUMimproveMinimum_f

int NUM_viterbi (
	long numberOfFrames, long maxnCandidates,
	long (*getNumberOfCandidates) (long iframe, void *closure),
	double (*getLocalCost) (long iframe, long icand, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, void *closure),
	void (*putResult) (long iframe, long place, void *closure),
	void *closure);

int NUM_viterbi_multi (
	long nframe, long ncand, int ntrack,
	double (*getLocalCost) (long iframe, long icand, int itrack, void *closure),
	double (*getTransitionCost) (long iframe, long icand1, long icand2, int itrack, void *closure),
	void (*putResult) (long iframe, long place, int itrack, void *closure),
	void *closure);

/********** Metrics (NUM_metrics.c) **********/

int NUMrotationsPointInPolygon
	(double x0, double y0, long n, float x [], float y []);
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
void NUMfilterSecondOrderSection_a (float x [], long n, double a1, double a2);
void NUMfilterSecondOrderSection_fb (float x [], long n, double dt, double formant, double bandwidth);
double NUMftopreemphasis (double f, double dt);
void NUMpreemphasize_a (float x [], long n, double preemphasis);
void NUMdeemphasize_a (float x [], long n, double preemphasis);
void NUMpreemphasize_f (float x [], long n, double dt, double frequency);
void NUMdeemphasize_f (float x [], long n, double dt, double frequency);
void NUMautoscale (float x [], long n, double scale);

/* The following ANSI-C power trick generates the declarations of 156 functions. */
#define FUNCTION(t,type)  \
	int NUM##t##vector_writeAscii (const type *v, long lo, long hi, FILE *f, const char *name); \
	int NUM##t##vector_writeBinary (const type *v, long lo, long hi, FILE *f); \
	int NUM##t##vector_writeCache (const type *v, long lo, long hi, CACHE *f); \
	type * NUM##t##vector_readAscii (long lo, long hi, FILE *f, const char *name); \
	type * NUM##t##vector_readBinary (long lo, long hi, FILE *f); \
	type * NUM##t##vector_readCache (long lo, long hi, CACHE *f); \
	int NUM##t##matrix_writeAscii (type **v, long r1, long r2, long c1, long c2, FILE *f, const char *name); \
	int NUM##t##matrix_writeBinary (type **v, long r1, long r2, long c1, long c2, FILE *f); \
	int NUM##t##matrix_writeCache (type **v, long r1, long r2, long c1, long c2, CACHE *f); \
	type ** NUM##t##matrix_readAscii (long r1, long r2, long c1, long c2, FILE *f, const char *name); \
	type ** NUM##t##matrix_readBinary (long r1, long r2, long c1, long c2, FILE *f); \
	type ** NUM##t##matrix_readCache (long r1, long r2, long c1, long c2, CACHE *f);
FUNCTION (b, signed char)
FUNCTION (s, short)
FUNCTION (i, int)
FUNCTION (l, long)
FUNCTION (ub, unsigned char)
FUNCTION (us, unsigned short)
FUNCTION (ui, unsigned int)
FUNCTION (ul, unsigned long)
FUNCTION (f, float)
FUNCTION (d, double)
FUNCTION (fc, fcomplex)
FUNCTION (dc, dcomplex)
FUNCTION (c, char)
#undef FUNCTION

/*
int NUMfvector_writeBinary (const float *v, long lo, long hi, FILE *f);   // etc
	write the vector elements v [lo..hi] as machine-independent
	binary data to the stream f.
	Return 0 if something went wrong, else return 1.
	The vectors need not have been created by NUM...vector.
float * NUMfvector_readAscii (long lo, long hi, FILE *f, const char *name);   // etc
	create and read a vector as text from the stream f.
	Queue an error message and return NULL if something went wrong.
	Every element is supposed to be on the beginning of a line.
float * NUMfvector_readBinary (long lo, long hi, FILE *f);   // etc
	create and read a vector as machine-independent binary data from the stream f.
	Queue an error message and return NULL if something went wrong.
int NUMfvector_writeAscii (const float *v, long lo, long hi, FILE *f, const char *name);   // etc
	write the vector elements v [lo..hi] as text to the stream f,
	each element on its own line, preceded by "name [index]: ".
	Return 0 if something went wrong, else return 1.
	The vectors need not have been created by NUMvector.
int NUMfmatrix_writeAscii (float **m, long r1, long r2, long c1, long c2, FILE *f, const char *name);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as text to the stream f.
	Return 0 if something went wrong, else return 1.
	The matrices need not have been created by NUMmatrix.
int NUMfmatrix_writeBinary (float **m, long r1, long r2, long c1, long c2, FILE *f);   // etc
	write the matrix elements m [r1..r2] [c1..c2] as machine-independent
	binary data to the stream f.
	Return 0 if something went wrong, else return 1.
	The matrices need not have been created by NUMmatrix.
float ** NUMfmatrix_readAscii (long r1, long r2, long c1, long c2, FILE *f, const char *name);   // etc
	create and read a matrix as text from the stream f.
	Give an error message and return NULL if something went wrong.
float ** NUMfmatrix_readBinary (long r1, long r2, long c1, long c2, FILE *f);   // etc
	create and read a matrix as machine-independent binary data from the stream f.
	Give an error message and return NULL if something went wrong.
*/

/* End of file NUM.h */
#endif
