#ifndef _NUM2_h_
#define _NUM2_h_
/* NUM2.h
 *
 * Copyright (C) 1997-2005 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020815 GPL header
 djmw 20050406 latest modification.
*/

#ifndef _NUM_h_
	#include "NUM.h"
#endif
#include "limits.h"

#ifndef _regularExp_h_
	#include "regularExp.h"
#endif
/* machine precision */
#define NUMeps 2.2e-16

int NUMstrcmp (const char *s1, const char *s2);
/*
	Compares strings, accepts NUL-strings (NULL < 'a')
	return s1 == NULL ? (s2 == NULL ? 0 : - 1) : 
		(s2 == NULL ? 1 : strcmp (s1, s2)); 
*/

int NUMstring_containsPrintableCharacter (char *s);

double *NUMstring_to_numbers (const char *s, long *numbers_found);
/* return array with the number of numbers found */

int NUMstrings_equal (char **s1, char **s2, long lo, long hi);
int NUMstrings_copyElements (char **from, char**to, long lo, long hi);
void NUMstrings_free (char **s, long lo, long hi);
int NUMstrings_setSequentialNumbering (char **s, long lo, long hi, 
	char *precursor, long number, long increment);
/* 
	Set s[lo]   = precursor<number>
	    s[lo+1] = precursor<number+1>
		...
		s[hi]   = precursor<number+hi-lo>
*/

char **NUMstrings_copy (char **from, long lo, long hi);

char *strstr_regexp (const char *string, const char *search_regexp);
/*
	Returns a pointer to the first occurrence in 'string' of the 
	regular expression 'searchRE'. It returns a null pointer if
	no match is found.
*/

char **strs_replace (char **from, long lo, long hi, const char *search, 
	const char *replace, int maximumNumberOfReplaces, long *nmatches, 
	long *nstringmatches, int use_regexp);
/*
	Searches and replaces in string array of strings.
	If use_regexp != 0, 'search' and 'replace' will be interpreted 
	as regular expressions. Else these strings are taken literally.
	
	'maximumNumberOfReplaces' is the maximum number of replaces in EACH string
	in the array of strings (you can replace ALL occurrences by making this 
	number <= 0).
	The totalnumber of matches found is returned in 'nmatches'.
	The number of strings with at least one match is returned in
	'nstringmatches'.
*/

char *str_replace_literal (char *string, const char *search, 
	const char *replace, long maximumNumberOfReplaces, long *nmatches);
/*
	Search and replace in 'string'.
	The maximum number of replaces is limited by 'maximumNumberOfReplaces'.
*/

char *str_replace_regexp (char *string, regexp *search_compiled, 
	const char *replace_regexp, long maximumNumberOfReplaces, long *nmatches);
/*
	Searches and replaces 'maximumNumberOfReplaces' times in 'string' on 
	the basis of regular expressions (RE). 
	If maximumNumberOfReplaces <= 0 the interpreted 'replaceRE' replaces 
	ALL occurrences.
	'search_regexp' is an efficient representation of the search RE and 
	is the result of the compileRE-function which must be called first before
	calling this routine.
	The number of matches found is returned in 'nmatches'.
*/
	
void NUMlvector_extrema (long v[], long lo, long hi, double *min, double *max);
void NUMfvector_extrema (float v[], long lo, long hi, double *min, double *max);
void NUMivector_extrema (int v[], long lo, long hi, double *min, double *max);
void NUMdvector_extrema (double v[], long lo, long hi, double *min, 
	double *max);
void NUMsvector_extrema (short v[], long lo, long hi, double *min, double *max);
/*  Function:
		compute minimum and maximum values of array v[lo..hi].
	Precondition:
		lo and hi must be valid indices in the array.
*/

void NUMdmatrix_printMatlabForm (double **m, long nr, long nc, char *name);
/*
	Print a matrix in a form that can be used as input for octave/matlab.
	                      1 2 3
	Let A be the matrix:  4 5 6
				          7 8 9
	The output from NUMdmatrix_printAsOctaveForm (A, 3, 3, "M") will be
	M=[1, 2, 3;
	4, 5, 6;
	7, 8, 9];
*/

double **NUMdmatrix_transpose (double **m, long nr, long nc);
/*
	Transpose a nr x nc matrix.
*/

void NUMdmatrix_extrema (double **x, long rb, long re, long cb, long ce, 
	double *min, double *max);
void NUMfmatrix_extrema (float **x, long rb, long re, long cb, long ce, 
	double *min, double *max);
void NUMlmatrix_extrema (long **x, long rb, long re, long cb, long ce, 
	double *min, double *max);
void NUMimatrix_extrema (int **x, long rb, long re, long cb, long ce, 
	double *min, double *max);
void NUMsmatrix_extrema (short **x, long rb, long re, long cb, long ce, 
	double *min, double *max);


void NUMlvector_clip (long v[], long lo, long hi, double min, double max);
void NUMfvector_clip (float v[], long lo, long hi, double min, double max);
void NUMivector_clip (int v[], long lo, long hi, double min, double max);
void NUMdvector_clip (double v[], long lo, long hi, double min, double max);
void NUMsvector_clip (short v[], long lo, long hi, double min, double max);
/* 
	Clip array values.
	c[i] = c[i] < min ? min : (c[i] > max ? max : c[i])
*/

int NUMdmatrix_hasInfinities (double **m, long rb, long re, long cb, long ce);

void NUMfvector_moment2 (float v[], long lo, long hi, double *mean, 
	double *variance);

float NUMfvector_normalize1 (float v[], long n);
float NUMfvector_normalize2 (float v[], long n);
float NUMfvector_getNorm1 (const float v[], long n);
float NUMfvector_getNorm2 (const float v[], long n);
double NUMdvector_normalize1 (double v[], long n);
double NUMdvector_normalize2 (double v[], long n);
double NUMdvector_getNorm1 (const double v[], long n);
double NUMdvector_getNorm2 (const double v[], long n);

void  NUMcentreRows_d(double **a, long rb, long re, long cb, long ce);
/*
	a[i][j] -= a[i][.]
*/
void NUMcentreColumns_d (double **a, long rb, long re, long cb, long ce, 
	double *centres);
/*
	a[i][j] -= a[.][j]
	if centres != NULL the means are returned in centres[1..re-rb+1]
*/

void NUMdoubleCentre_d (double **a, long rb, long re, long cb, long ce);
/*
	Function: Make the average value of each column and each row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/

void NUMnormalizeRows_d (double **a, long nr, long nc, double norm);

void NUMnormalizeColumns_d (double **a, long nr, long nc, double norm);
/*
	Scale a[.][j] such that sqrt (Sum(a[i][j]^2, i=1..nPoints)) = norm.
*/

void NUMnormalize_d (double **a, long nr, long nc, double norm);
/*
	Scale all elements of the matrix [1..nr][1..nc] such that
	(sqrt(Sum( a[i][j]^2, i=1..nr, j=1..nc)) becomes equal to norm.	
*/

void NUMstandardizeColumns (double **a, long rb, long re, long cb, long ce);
/* a[i][j] = (a[i][j] - average[j]) / sigma[j] */
void NUMstandardizeRows (double **a, long rb, long re, long cb, long ce);
/* a[i][j] = (a[i][j] - average[i]) / sigma[i] */

void NUMaverageColumns (double **a, long rb, long re, long cb, long ce);
/* a[i][j] = average[j]) */

void NUMcolumn_avevar (double **a, long nr, long nc, long icol, 
	double *average, double *variance);
/*
	Get mean and variance of a column.
	When average and/or variance are NULL, the corresponding output is 
	NOT given.   
 */

void NUMcolumn2_avevar (double **a, long nr, long nc, long icol1, long icol2,
	double *average1, double *variance1, double *average2, double *variance2,
	double *covariance);
/*
	Get mean and variance of two columns.
	When average and/or variance are NULL, the corresponding output is 
	NOT given.   
 */

int NUMmad_f (float *x, long n, double *location, int wantlocation,
	double *mad, float *work);
int NUMmad_d (double *x, long n, double *location, int wantlocation,
	double *mad, double *work);
/*
	Computes the median absolute deviation, i.e., the median of the
	absolute deviations from the median, and adjust by a factor for
	asymptotically normal consistency.
	You either GIVE the median location (if wantlocation = 0) or it
	will be calculated (if wantlocation = 1);
	
	work is a working array (1..n) that can be used for efficiency reasons.
	If work == NULL, the routine allocates (and destroys) its own memory.
 */

int NUMstatistics_huber_f (float *x, long n, double *location, int wantlocation,
	double *scale, int wantscale, double k, double tol, float *work);
int NUMstatistics_huber_d (double *x, long n, double *location, int wantlocation,
	double *scale, int wantscale, double k, double tol, double *work);
/*
	Finds the Huber M-estimator for location with scale specified,
	scale with location specified, or both if neither is specified.
	k Winsorizes at `k' standard deviations.

	work is a working array (1..n) that can be used for efficiency reasons.
	If work == NULL, the routine allocates (and destroys) its own memory.
*/

void NUMmonotoneRegression_d (const double x[], long n, double xs[]);
/*
	Find numbers xs[1..n] that have a monotone relationship with 
	the numbers in x[1..n].
	The xs[i] will be ascending. 
*/

void NUMsort2_fl (long n, float a[], long b[]);
void NUMsort2_ff (long n, float a[], float b[]);
void NUMsort2_dl (long n, double a[], long b[]);	
void NUMsort2_dd (long n, double a[], double b[]);
void NUMsort2_ll (long n, long a[], long b[]);
/*
	Sorts (inplace) an array a[1..n] into ascending order using the Heapsort algorithm,
	while making the corresponding rearrangement of the companion 
	array b[1..n]. A characteristic of heapsort is that it does not conserve 
	the order of equals: e.g., the array 3,1,1,2 will be sorted as 1,1,2,3. 
	It may occur that a_sorted[1] = a_presorted[2] and a_sorted[2] = a_presorted[1]
	no g 
*/

void NUMindexx_f (const float a[], long n, long indx[]);
void NUMindexx_d (const double a[], long n, long indx[]);
void NUMindexx_s (char *a[], long n, long indx[]);
/*
	Indexes the array a[1..n], i.e., outputs the array indx[1..n] such that
	a[ indx[i] ] is in ascending order for i=1..n;
	No preservation of order amoung equals (see NUMsort2_...)
*/

void NUMrank_f (long n, float a []);
void NUMrank_d (long n, double a []);
/*
	Replace content of array by rank number, including midranking of ties.
	E.g. The elements {10, 20.1, 20.1, 20.1, 20.1, 30} in array a will be replaced
    by {1, 3.5, 3.5, 3.5, 3.5, 4}, respectively.
*/
int NUMrankColumns_f (float **m, long rb, long re, long cb, long ce);
int NUMrankColumns_d (double **m, long rb, long re, long cb, long ce);



void NUMhunt_f (float xx[], long n, float x, long *jlo);
void NUMhunt_d (double xx[], long n, double x, long *jlo);
/*
	Given an array xx[1..n], and given a value x, returns a value jlo such that
	x is between xx[jlo] and xx[jlo+1]. xx[1..n] must be monotonic, either
	increasing or decreasing. jlo=0 and jlo=n are out of range indicators.
	jlo on input is taken as the initial guess.
*/

void NUMlocate_f (float *xx, long n, float x, long *index);
void NUMlocate_d (double *xx, long n, double x, long *index);
/*
	Given an array xx[1..n], and given a value x, returns a value index
	such that xx[index] < x <=  xx[index+1].
	Preconditions: xx must be monotonic
	Error: index = 0 or index = n (out of range)
*/

int NUMjacobi (float **a, long n, float d[], float **v, long *nrot);
/*
	This version deviates from the NR version. 
	HERE: v[1..n][1..n] is a matrix whose ROWS
	(and not the columns) contain, on output, the normalized eigenvectors 
	of `a'.
	Computes all eigenvalues and eigenvectors of a real symmetric 
	matrix a[1..n][1..n].
	On output, elements of `a' above the diagonal are destroyed. 
	d[1..n] returns the eigenvalues of `a'. 
	`nrot' returns the number of Jacobi rotations that were required.
 */
 
void NUMtred2_f (float **a, long n, float d[], float e[]);
void NUMtred2_d (double **a, long n, double d[], double e[]);
/*
	Householder reduction of a real, symmetric matrix a[1..n][1..n]. On output,
	a is replaced by the orthogonal matrix Q effecting the transformation.
	d[1..n] returns the diagonal elements of the tridiagonal matrix, and e[1..n]
	the off-diagonal elements, with e[1] = 0. 
*/

int NUMtqli_f (float d[], float e[], long n, float **z);
int NUMtqli_d (double d[], double e[], long n, double **z);
/*
	QL algorithm with implicit shifts, to determine the (sorted) eigenvalues 
	and eigenvectors of a real, symmetric, tridiagonal matrix, or of a real,
	symmetric matrix previously reduced by NUMtred2 . 
	On input d[1..n] contains the diagonal elements of the tridiagonal matrix.
	On output, it returns the eigenvalues. 
	The vector e[1..n] inputs the subdiagonal elements of the tridiagonal
	matrix, with e[1] arbitrary.
	On output e is destroyed. 
	If the eigenvectors of a tridiagonal matrix are desired,
	the matrix z[1..n][1..n] is input as the identity matrix. 
	If the eigenvectors of a matrix that has been reduced by NUMtred2 are
	required, then z is input as the matrix output by NUMtred2. The k-th 
	column of z returns the normalized eigenvector corresponding to d[k].
	Returns 0 in case of too many rotations.
*/

int NUMgaussj_d (double **a, long n, double **b, long m);

int NUMgaussj (float **a, long n);
/* 
	Calculate inverse of square matrix a[1..n][1..n] (in-place).
	Method: Gauss-Jordan elimination with full pivoting.
	Error message in case of singular matrix.
*/

int NUMsvdcmp (double **a, long m, long n, double w[], double **v);
/* 
	Given a matrix a[1..m][1..n], this routine computes its singular 
	value decomposition, A = U.W.V'. The matrix U replaces a on output. 
	The diagonal matrix of singular values W is output as vector w[1..n]. 
	The matrix V (not the transpose V') is output as v[1..n][1..n].
	Possible errors: no memory or more than 30 iterations.
 */
 
int NUMsvbksb (double **u, double w[], double **v, long m, long n, double b[], double x[]);
/*
	Solves A.X=B for a vector X, where A is specified by the arrays
	u[1..m][1..n], w[1..n], v[1..n][1..n] as returned by NUMsvdcmp.
	m and n are the dimensions of a, and will be equal for square
	matrices. b[1..m] is the input right-hand side. x[1..n] is the
	output solution vector.
	Possible errors: no memory.
*/

int NUMludcmp (double **a, long n, long *indx, double *d);
int NUMludcmp_f (float **a, long n, long *indx, float *d);
/*	Given a matrix a[1..n][1..n], this routine replaces it by the 
	LU decomposition of a rowwise permutation of itself.
	a	: matrix [1..n][1..n]
	n	: dimension of matrix
	indx	: output vector[1..n] that records the row permutation effected by
			partial pivoting.
	d	: output +/-1 depending on whether the number of ro interchanges was
		even/odd.
*/

int NUMcholeskyDecomposition(double **a, long n, double d[]);
/*
	Cholesky decomposition of a symmetric positive definite matrix.
*/

void NUMcholeskySolve (double **a, long n, double d[], double b[], double x[]);
/*
	Solves A.x=b for x. A[][] and d[] are output from NUMcholeskyDecomposition.
*/

double **NUMcholdc_inverse (double **a, long n, double *d);
/*
	Construct inverse matrix from result of Cholesky decomposition
*/

int NUMinverse_cholesky (double **a, long n, double *lnd);
/*
	Calculates L^-1, where A = L.L' is a symmetric positive definite matrix
	and ln(determinant). L^-1 in lower, leave upper part intact.
*/

int NUMdeterminant_cholesky (double **a, long n, double *lnd);
/*
	ln(determinant) of a symmetric p.s.d. matrix
*/

double NUMmahalanobisDistance_chi (double **l, double *v, double *m, long n);
/*
	Calculates squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix (li) is the inverse L^-1 of the Cholesky decomposition S = L.L'.
*/

double NUMtrace (double **a, long n);
double NUMtrace2 (double **a1, double **a2, long n);
/*
	Calculates the trace from the product matrix a1 * a2.
	a1 and a2 are [1..n][1..n] square matrices.
*/

void eigenSort_d (double d[], double **v, long n, int sort);

int NUMeigensystem_d (double **a, long n, double **evec, double eval[]);
/*
	Determines the eigensystem of a real, symmetric matrix[1..][1..n].
	Returned are: evec[1..n][1..n] with eigenvectors (columnwise) and
	eval[1..n], the eigenvalues.
	No eigenvectors or eigenvalues are returned when the function is 
	called with the corresponding parameter NULL.
	Eigenvalues (with corresponding eigenvectors) are sorted in descending order.
*/

int NUMdominantEigenvector_d (double **mns, long n, double *q, double *lambda, double tolerance);
/*
	Determines the first dominant eigenvector from a GENERAL matrix
	mns[1..n][1..].
	Besides the matrix mns, a first guess for the eigenvector q must 
	be supplied (e.g. 1,0,...,0) and a value for tolerance (iteration
	stops when fabs(lamda[k] - lambda[k-1]) <= tolerance, where lamda[k] is
	the eigenvalue at the k-th iteration step.
	The methos is described in:
	G. Golub & C. van Loan (1996), Matrix computations, third edition,
	The Johns Hopkins University Press Ltd.,
	London, (Par. 7.3.1 The Power Method)
*/

int NUMdmatrix_into_principalComponents (double **m, long nrows, long ncols,
	long numberOfComponents, double **pc);
/*
	Precondition:
		numberOfComponents > 0 && numberOfComponents <= ncols
		pc[1..nrows][1..numberOfComponents] exists
	Function:
		Calculates the first 'numberOfComponents' principal components for the
		matrix m[1..nrows][1..ncols].
	Postcondition:
		Matrix pc contains the principal components
	Algorithm:
		Singular Value Decomposition:
		1. Centre the columns of m, this results in a new matrix mc
		2. SVD the mc matrix -> U.d.V'
			(Covariance matrix from mc is mc'.mc = (U.d.V')'.(U.d.V') =
				V.d^2.V')
		3. Sort singular values d (descending) and corresponding vectors in V
		4. The principal components are now:
			pc[i][j] = sum (k=1..ncols, v[k][j] * m[i][k])
	Remark:
		The resulting configuration is unique up to reflections along the new
		principal directions.
*/

int NUMprincipalComponents_d (double **a, long n, long nComponents, double **pc);
/*
	Determines the principal components of a real symmetric matrix 
	a[1..n][1..n] as a pc[1..n][1..nComponents] column matrix.
*/

int NUMpseudoInverse_d (double **y, long nr, long nc, double **yinv, double tolerance);
/*
	Determines the pseudo-inverse Y^-1 of Y[1..nr][1..nc] via s.v.d.
	Alternative notation for pseudo-inverse: (Y'.Y)^-1.Y'
	Returns a [1..nc][1..nr] matrix
*/


int NUMsolveEquation_d (double **a, long nr, long nc, double *b, double tol, double *x);
/*
	Solve the equation: a.x = b;
	a[1..nr][1..nc], b[1..nr] and the unknown x[1..nc]
	a & b are destroyed during the computation.
	Algorithm: s.v.d.
*/

int NUMsolveEquations_d (double **a, long nr, long nc, double **b, long ncb, double tol, double **x);
/*
	Solve the equation: a.x = b;
	a[1..nr][1..nc], b[1..nr][1..nc2] and the unknown x[1..nc][1..nc2]
	a & b are destroyed during the computation.
	Algorithm: s.v.d.
*/

void NUMsolveNonNegativeLeastSquaresRegression (double **a, long nr, long nc,
	double *b, double tol, long itermax, double *x);
/*
	Solve the equation: a.x = b for x under the constraint: all x[i] >= 0;
	a[1..nr][1..nc], b[1..nr] and x[1..nc].
	Algorithm: Alternating least squares.
	Borg & Groenen (1997), Modern multidimensional scaling, Springer, page 180.
*/

int NUMsolveConstrainedLSQuadraticRegression (double **o, const double y[],
	long n, double *alpha, double *gamma);
/*
	Solve y[i] = alpha + beta * x[i] + gamma * x[i]^2, with i = 1..n, 
	subject to the constraint beta^2 = 4 * alpha * gamma, for alpha and 
	gamma (Least Squares).
	The input Vandermonde-matrix o[1..n,1..3] has columns with 1, x[i] and
	x[i]^2, respectively.
	The algorithm is according to:
	Jos M.F. Ten Berge (1983), A generalization of Verhelst's solution for 
	a constrained regression problem in ALSCAL and related MDS-algorithms,
	Psychometrika 48, 631-638.
*/

int NUMsolveWeaklyConstrainedLinearRegression (double **f, long n, long m, double phi[],
	double alpha, double delta, double t[]);
/*
	Solve g(t) = ||Ft - phi||^2 + alpha (t't - delta)^2 for t[1..m], 
	where F[1..n][1..m] is a matrix, phi[1..n] a given vector, and alpha
	and delta are fixed numbers.
	This class of functions is composed of a linear regression function and 
	a penalty function for the sum of squared regression weights. It is weakly
	constrained because the penalty function prohibits a relatively large
	departure of t't from delta.
	The solution is due to:
	Jos M.F. ten Berge (1991), A general solution for a class of weakly
	constrained linear regression problems, Psychometrika 56, 601-609.
	Preconditions:
		n >= m
		alpha >= 0
*/

int NUMProcrustes (double **x, double **y, long nPoints, 
	long nDimensions, double **t, double *v, double *s);
/*
	Given two configurations x and y (nPoints x nDimensions), find the 
	the Procrustes rotation/reflection matrix T, the translation vector v and the scaling
	factor s such that Y = sXT+1v' (1 is the nPoints vector with ones).
	Solution: see Borg and Groenen (1997), Modern Multidimensional Scaling, pp 340-346.
	When on input v == NULL or s == NULL, only the matrix T will be solved for:
	the orthogonal Procrustes transform.
*/

int NUMnrbis (void (*f)(double x, double *fx, double *dfx, void *closure), 
	double x1, double x2, void *closure, double *root);
/*
	Find the root of a function between x1 and x2.
	Method: Newton-Raphson with bisection (i.e., derivative is known!).
	Error condition:
		root not bracketed.
*/

double NUMridders (double (*f) (double x, void *closure), double x1, double x2, void *closure);
/*
	Return the root of a function f bracketed in [xlow, xhigh].
	Error condition:
		root not bracketed.
*/

int NUMmspline (double knot[], long nKnots, long order, long i, double x, 
	double *y);
/*
	Calculates an M-spline for a knot sequence.
	After Ramsay (1988), Monotone splines in action, Statistical Science 4.
	
	M-splines of order k have degree k-1. 
	M-splines are zero outside interval [ knot[i], knot[i+order] ).
	First and last 'order' knots are equal, i.e.,
	knot[1] = ... = knot[order] && knot[nKnots-order+1] = ... knot[nKnots].
	Error condition: no memory.
*/

int NUMispline (double aknot[], long nKnots, long order, long i, double x,
	double *y);
/*
	Calculates an I-spline for simple knot sequences: only one knot at each 
	interior boundary.
	After Ramsay (1988), Monotone splines in action, Statistical Science 4.
	
	I-splines of order k have degree k (because they Integrate an M-spline
	of degree k-1).
	In the calculation of the integral of M(x|k,t), M-splines are used that 
	have two more knots, i.e., M(x|k+1,t). For reasons of efficiency we 
	demand that these extra knots are given, i.e., the 'aknot[]' argument
	contains the knot positions as if the spline to be integrated were an
	M(x|k+1,t) spline.
	knot[1] = ... = knot[order+1] && knot[nKnots-order] = ... knot[nKnots]
	Error condition: no memory.
*/

double NUMwilksLambda (double *lambda, long from, long to);
/*
	Calculate: Product (i=from..to; 1/(1+lambda[i]))
	Preconditions: to >= from
*/

double NUMincompleteBeta (double a, double b, double x);
/*
	Incomplete beta function Ix(a,b).
	Preconditions: a, b > 0; 0 <= x <= 1
*/

double NUMbetaContinuedFraction(double a, double b, double x);

double NUMfactln (int n);
/* Returns ln (n!) */

/***** STATISTICS: PROBABILITY DENSITY FUNCTIONS ********************/

double NUMstudentP (double t, long df);
/*
	The area under the student T-distribution from -infinity to t.
	Precondition: df > 0
*/

double NUMstudentQ (double t, long df);
/*
	The area under the student T distribution from t to +infinity.
	Precondition: df > 0
*/

double NUMfisherP (double f, long df1, long df2);
/*
	The area under Fisher's F-distribution from 0 to f
	Preconditions: f >= 0, df1 > 0, df2 > 0
*/

double NUMfisherQ (double f, long df1, long df2);
/*
	The area under Fisher's F-distribution from f to +infinity
	Preconditions: f >= 0, df1 > 0, df2 > 0
*/

double NUMinvGaussQ (double p);
/*
	Solves NUMgaussQ (x) == p for x, given p.
	Precondition: 0 < p < 1
	Method: Abramovitz & Stegun 26.2.23
	Precision: |eps(p)| < 4.5 10^-4
*/

double NUMinvChiSquareQ (double p, long df);
/*
	Solves NUMchiSquareQ (chiSquare, df) == p for chiSquare, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvStudentQ (double p, long df);
/*
	Solves NUMstudentQ (t, df) == p for t, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvFisherQ (double p, long df1, long df2);
/*
	Solves NUMfisherQ (f, df1, df2) == p for f, given p, df1, df2
	Precondition: 0 < p < 1
*/

/******  Frequency in Hz to other frequency reps ****/

double NUMmelToHertz2 (double mel);
/*
	Return 700 * (pow (10.0, mel / 2595.0) - 1)
*/

double NUMhertzToMel2 (double f);
/*
	Return 2595 * log10 (1 + f/700)
*/

double NUMmelToHertz3 (double mel);
/*
	Return mel < 1000 ? mel : 1000 * (exp (mel * log10(2) / 1000) - 1)
*/

double NUMhertzToMel3 (double hz);
/*
	Return hz < 1000 ? hz : 1000 * log10 (1 + hz / 1000) / log10 (2)
*/

double NUMbarkToHertz2 (double bark);
/*
	Return 650 * sinh (bark / 7)
*/

double NUMhertzToBark2 (double hz);
/*
	Return 7 * ln (hz / 650 + sqrt(1 + (hz / 650)^2))
*/

double NUMhertzToBark_traunmueller (double hz);
/*
	return 26.81 * hz /(1960 + hz) -0.53;
*/

double NUMbarkToHertz_traunmueller (double bark);
/*
	return 1960* (bark + 0.53) / (26.28 - bark);
*/

double NUMbarkToHertz_schroeder (double bark);
/*
	return 650.0 * sinh (bark / 7.0);
*/

double NUMbarkToHertz_zwickerterhardt (double hz);
/*
	return 13 * atan (0.00076 * hz) + 3.5 * atan (hz / 7500);
*/

double NUMhertzToBark_schroeder (double hz);
/*
	return 7.0 * log (hz / 650 + sqrt (1 + (hz / 650)^2));
*/

double NUMbladonlindblomfilter_amplitude (double zc, double z);
/* 
    Amplitude of filter at dz (barks) from centre frequency. 
    dz may be positive and negative.
	
	The bladonlindblomfilter function is:
	
	z' = zc - z + 0.474
	10 log10 F(z') = 15.81 + 7.5 z' - 17.5 sqrt( 1 + z'^2 )

	Reference: Bladon, R.A.W & Lindblom, B., (1980), 
	"Modeling the judgment of vowel quality differences", JASA 69, 1414-1422. 	
	The filter has a bandwidth of 1.43 Bark, the maximum occurs at z = zc, 
	and the slopes are -10 dB/Bark and +25 dB/Bark.
 */
 
double NUMsekeyhansonfilter_amplitude (double zc, double z);
/* 
    Amplitude of filter at dz (barks) from centre frequency. 
    dz may be positive and negative.
	
	The sekeyhansonfilter function is:
	z' = zc - z - 0.215
	10 log10 F(z') = 7 - 7.5 * z' - 17.5 * sqrt( 0.196 + z'^2 )
	
	Reference: Sekey, A. & Hanson, B.A. (1984),
	"Improved 1-Bark bandwidth auditory filter", JASA 75, 1902-1904.
	The filter function has a bandwidth of 1 Bark, the maximum response
	occurs at z=zc, and the slopes are +10 dB/Bark and -25 dB/Bark.
	It is an improved version of bladonlindblomfilter.
 */

double NUMtriangularfilter_amplitude (double fl, double fc, double fh, 
	double f);
/*
	Filterfunction that intermediates in Mel frequency cepstral coefficients
	calculation.
	The filter function is 
	
	         (f-fl)/(fc-fl)  fl < f < fc
	H(z) =   (fh-f)/(fh-fc)  fc < f < fh
	         0               otherwise
	Preconditions:
		0 < fl < fh
 */
 
double NUMformantfilter_amplitude (double fc, double bw, double f);
/*
	Filterfunction with a formant-like shape on a linear freq. scale.

	H(f) = 1.0 / (dq * dq + 1.0), where
		dq = (fc * fc - f * f) / (bw * f)
*/

int NUMburg (float x[], long n, float a[], int m, float *xms);
/*
	Calculates linear prediction coefficients according to the algorithm
	from J.P. Burg as described by N.Anderson in Childers, D. (ed), Modern
	Spectrum Analysis, IEEE Press, 1978, 252-255.
*/

int NUMfmatrix_to_dBs (float **m, long rb, long re, long cb, long ce, 
	double ref, double factor, double floor);
/*
	Transforms the values in the matrix m[rb..re][cb..ce] to dB's
	
	m[i][j] = factor * 10 * log10 (m[i][j] / ref)
	if (m[i][j] < floor) m[i][j] = floor;

	Preconditions:
		rb <= re
		cb <= ce
		ref > 0
		factor > 0
	Errors:
		Matrix elements < 0;
*/

double **NUMcosinesTable_d (long first, long last, long npoints);
/*
	Generate table with cosines.
	
	result[i][j] = cos (i * pi * (j - 1/2) / npoints)
	i=first..last; j=1..npoints
	
	Preconditions:
		1 <= first <= last
		npoints > 1
*/

/******  Interpolation ****/

int NUMspline_f (float x[], float y[], long n, float yp1, float ypn,
	float y2[]);
int NUMspline_d (double x[], double y[], long n, double yp1, double ypn,
	double y2[]);
/*
	Given arrays a[1..n] and y[1..n] containing a tabulated function, i.e.,
	y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and given values yp1 and
	ypn for the first derivative of the interpolating function at point 
	1 and n, respectively, this routine returns an array y2[1..n] that 
	contains the second derivative of the interpolating function at the
	tabulated point x.
	If yp1 and/or ypn are greaterequal 10^30, the routine is signaled to 
	set the corresponding boundary condition for a natuarl spline, with
	zero second derivative on that boundary.
*/

int NUMsplint_f (float xa[], float ya[], float y2a[], long n, float x,
	float *y);
int NUMsplint_d (double xa[], double ya[], double y2a[], long n, double x,
	double *y);
/*
	Given arrays xa[1..n] and ya[1..n] containing a tabulated function,
	i.e., y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and given the
	array y2a[1..n] which is the output of NUMspline_d above, and given
	a value of x, this routine returns an interpolated value y.
*/

/*********************** Geometry *************************************/

int NUMgetOrientationOfPoints (double x1, double y1, double x2, double y2, double x3, double y3);
/* Traverse points 1, 2 and 3. If we travel counter-clockwise the result will be 1, 
	if we travel clockwise the result will be -1 and the result will be 0 if 3 is on the line segment between 1 and 2.
*/

int NUMdoLineSegmentsIntersect (double x1, double y1, double x2, double y2, double x3, double y3,
	double x4, double y4);
/* Does the line segment from (x1,y1) to (x2,y2) intersect with the line segment from (x3,y3) to (x4,y4)? */

int NUMgetIntersectionsWithRectangle (double x1, double y1, double x2, double y2, 
	double xmin, double ymin, double xmax, double ymax, double *xi, double *yi);
/* Get the intersection points of the line through the points (x1,y1) and (x2,y2) with the
	rectangle with corners (xmin, ymin) and (xmax,ymax).
	The returned value is the number of intersections found and is either 0 or 1 or 2.
*/

void NUMgetEllipseBoundingBox (double a, double b, double cospsi,
	double *width, double *height);
/*
	Get the width and the height of the bonding box around an ellipse.
	a and b are the lengths of the long axes.
	cospsi is the cosine of the angle between the a-axis and the horizontal 
	x-axis (cs == 0 when a-axis and x-axis are perpendicular).
	 
	Parametrisation of the ellipse:
		x(phi) = a cos(psi) cos(phi) - b sin (psi) sin(phi)
		y(phi) = a sin(psi) cos(phi) + b cos(psi) sin(phi)  0 <= phi <= 2 pi
	Extrema:
		d x(phi) / dphi == 0 and d y(phi) / dphi == 0
	Solution:
		x(phi1) = a cos(psi) cos(phi1) - b sin (psi) sin(phi1)
		y(phi2) = a sin(psi) cos(phi2) + b cos(psi) sin(phi2),
	where
		phi1 = arctg ( -b/a tg(psi))
		phi2 = arctg ( b/a cotg(psi))
	Special cases are psi = 0 and pi /2 		
*/

double NUMminimize_brent (double (*f) (double x, void *closure), double a, double b,
	void *closure, double tol, double *fx);
/*
	The function returns an estimate for the minimum location with accuracy
		3 * SQRT_EPSILON * abs(x) + tol.
	The function always obtains a local minimum which coincides with
	the global one only if a function under investigation being unimodular.
	If a function being examined possesses no local minimum within
	the given range, the function returns 'a' (if f(a) < f(b)), otherwise
	it returns the right range boundary value b.

	Algorithm
	
	The function makes use of the golden section procedure combined with
	parabolic interpolation.
	At every step, the program operates at three abscissae - x, v, and w.
	x - the last and the best approximation to the minimum location,
		i.e. f(x) <= f(a) or/and f(x) <= f(b)
		(if the function f has a local minimum in (a,b), then both
		conditions are fulfiled after one or two steps).
	v, w are previous approximations to the minimum location. They may
	coincide with a, b, or x (although the algorithm tries to make all
 	u, v, and w distinct). Points x, v, and w are used to construct
	interpolating parabola whose minimum will be treated as a new
	approximation to the minimum location if the former falls within
	[a,b] and reduces the range enveloping minimum more efficient than
	the golden section procedure. 
	When f(x) has a second derivative positive at the minimum location
	(not coinciding with a or b) the procedure converges superlinearly
	at a rate order about 1.324
*/

/********************** fft ******************************************/

struct NUMfft_Table_f
{
  long n; /* Data length */
  float *trigcache;
  long *splitcache;
};

struct NUMfft_Table_d
{
  long n;
  double *trigcache;
  long *splitcache;
};

typedef struct NUMfft_Table_f *NUMfft_Table_f;
typedef struct NUMfft_Table_d *NUMfft_Table_d;

int NUMfft_Table_init_f (NUMfft_Table_f table, long n);
int NUMfft_Table_init_d (NUMfft_Table_d table, long n);
/*
	n : data size
*/

void NUMfft_Table_free_f (NUMfft_Table_f table);
void NUMfft_Table_free_d (NUMfft_Table_d table);

void NUMfft_forward_f (NUMfft_Table_f table, float *data);
void NUMfft_forward_d (NUMfft_Table_d table, double *data);
/*
	Function:
		Calculates the Fourier Transform of a set of n real-valued data points.
		Replaces this data in array data [1...n] by the positive frequency half
		of its complex Fourier Transform, with a minus sign in the exponent.
	Preconditions:
		data != NULL;
		table must have been initialised with NUMfft_Table_init_f/d
	Postconditions:
		data[1] contains real valued first component (Direct Current)
		data[2..n-1] even index : real part; odd index: imaginary part of DFT.
		data[n] contains real valued last component (Nyquist frequency)	

	Output parameters:

	data  r(1) = the sum from i=1 to i=n of r(i)

        If l =(int) (n+1)/2

          then for k = 2,...,l

             r(2*k-2) = the sum from i = 1 to i = n of

                  r(i)*cos((k-1)*(i-1)*2*pi/n)

             r(2*k-1) = the sum from i = 1 to i = n of

                 -r(i)*sin((k-1)*(i-1)*2*pi/n)

        if n is even

             r(n) = the sum from i = 1 to i = n of

                  (-1)**(i-1)*r(i)

		i.e., the ordering of the output array will be for n even
			r(1),(r(2),i(2)),(r(3),i(3)),...,(r(l-1),i(l-1)),r(l).
		Or ...., (r(l),i(l)) for n uneven. 
			
 *****  note
             this transform is unnormalized since a call of NUMfft_forward
             followed by a call of NUMfft_backward will multiply the input
             sequence by n.
*/

void NUMfft_backward_f (NUMfft_Table_f table, float *data);
void NUMfft_backward_d (NUMfft_Table_d table, double *data);
/*
	Function:
		Calculates the inverse transform of a complex array if it is the transform of real data.
		(Result in this case must be multiplied by 1/n.)
	Preconditions:
		n is an integer power of 2.
		data != NULL;
		data [1] contains real valued first component (Direct Current)
		data [2..n-1] even index : real part; odd index: imaginary part of DFT.
		data [n] contains real valued last component (Nyquist frequency)
		
		table must have been initialised with NUMfft_Table_init_f/d

	Output parameters

	data       for n even and for i = 1,...,n

             r(i) = r(1)+(-1)**(i-1)*r(n)

                  plus the sum from k=2 to k=n/2 of

                   2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)

                  -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

        for n odd and for i = 1,...,n

             r(i) = r(1) plus the sum from k=2 to k=(n+1)/2 of

                  2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)

                 -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)

 *****  note
             this transform is unnormalized since a call of NUMfft_forward
             followed by a call of NUMfft_backward will multiply the input
             sequence by n.
*/

/**** Compatibility with NR fft's */

int NUMforwardRealFastFourierTransform_f (float  *data, long n);
int NUMforwardRealFastFourierTransform_d (double  *data, long n);
/*
	Function:
		Calculates the Fourier Transform of a set of n real-valued data points.
		Replaces this data in array data [1...n] by the positive frequency half
		of its complex Fourier Transform, with a minus sign in the exponent.
	Preconditions:
		n is an integer power of 2.
		data != NULL;
	Postconditions:
		data [1] contains real valued first component (Direct Current)
		data [2] contains real valued last component (Nyquist frequency)
		data [3..n] odd index : real part; even index: imaginary part of DFT.
*/
int NUMreverseRealFastFourierTransform_f (float  *data, long n);
int NUMreverseRealFastFourierTransform_d (double  *data, long n);
/*
	Function:
		Calculates the inverse transform of a complex array if it is the transform of real data.
		(Result in this case must be multiplied by 1/n.)
	Preconditions:
		n is an integer power of 2.
		data != NULL;
		data [1] contains real valued first component (Direct Current)
		data [2] contains real valued last component (Nyquist frequency)
		data [3..n] odd index : real part; even index: imaginary part of DFT.
*/
int NUMrealft_f (float  *data, long n, int direction);    /* Please stop using. */ 
int NUMrealft_d (double *data, long n, int direction);
#define NUMrealft NUMrealft_f

#endif /* _NUM2_h_ */
