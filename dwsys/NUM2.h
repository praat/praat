#ifndef _NUM2_h_
#define _NUM2_h_
/* NUM2.h
 *
 * Copyright (C) 1997-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020815 GPL header
 djmw 20121024 Latest modification.
*/

#include <limits.h>
#include "melder.h"
#include "regularExp.h"

/* machine precision */
#define NUMeps 2.2e-16

int NUMstrcmp (const char *s1, const char *s2);
/*
	Compares strings, accepts NUL-strings (NULL < 'a')
	return s1 == NULL ? (s2 == NULL ? 0 : - 1) :
		(s2 == NULL ? 1 : strcmp (s1, s2));
*/

int NUMstring_containsPrintableCharacter (const char32 *s);

void NUMstring_chopWhiteSpaceAtExtremes_inline (char32 *string);

double *NUMstring_to_numbers (const char32 *s, long *numbers_found);
/* return array with the number of numbers found */

/*
 * Acceptable ranges e.g. "1 4 2 3:7 4:3 3:5:2" -->
 * 1, 4, 2, 3, 4, 5, 6, 7, 4, 3, 3, 4, 5, 4, 3, 2
 * Overlap is allowed. Ranges can go up and down.
 */
long *NUMstring_getElementsOfRanges (const char32 *ranges, long maximumElement, long *numberOfElements, long *numberOfMultiples, const char32 *elementType, bool sortedUniques);

char32 * NUMstring_timeNoDot (double time);

int NUMstrings_equal (const char32 **s1, const char32 **s2, long lo, long hi);

void NUMstrings_copyElements (char32 **from, char32**to, long lo, long hi);

void NUMstrings_free (char32 **s, long lo, long hi);

int NUMstrings_setSequentialNumbering (char32 **s, long lo, long hi, const char32 *precursor, long number, long increment, int asArray);
/*
	Set s[lo]   = precursor<number>
	    s[lo+1] = precursor<number+1>
		...
		s[hi]   = precursor<number+hi-lo>
*/

char32 **NUMstrings_copy (char32 **from, long lo, long hi);

regexp *NUMregexp_compile (const char32 *regexp);
/* Compiles a regular expression to a datastructure used by the regexp engine */


char32 *strstr_regexp (const char32 *string, const char32 *search_regexp);
/*
	Returns a pointer to the first occurrence in 'string' of the
	regular expression 'searchRE'. It returns a null pointer if
	no match is found.
*/

char32 **strs_replace (char32 **from, long lo, long hi, const char32 *search,
	const char32 *replace, int maximumNumberOfReplaces, long *nmatches,
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

char32 *str_replace_literal (const char32 *string, const char32 *search,
	const char32 *replace, long maximumNumberOfReplaces, long *nmatches);
/*
	Search and replace in 'string'.
	The maximum number of replaces is limited by 'maximumNumberOfReplaces'.
*/

char32 *str_replace_regexp (const char32 *string, regexp *search_compiled,
	const char32 *replace_regexp, long maximumNumberOfReplaces, long *nmatches);
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


void NUMdmatrix_printMatlabForm (double **m, long nr, long nc, const char32 *name);
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

bool NUMdmatrix_hasFiniteElements (double **m, long row1, long row2, long col1, long col2);
/* true if all the elements are finite i.e. not plus or minus infinity, and not NaN */

void NUMdmatrix_diagnoseCells (double **m, long rb, long re, long cb, long ce, long maximumNumberOfPositionsToReport);
/* which cells are not finite? */

double **NUMdmatrix_transpose (double **m, long nr, long nc);
/*
	Transpose a nr x nc matrix.
*/


/*  NUMvector_extrema
 * Function:
 *     compute minimum and maximum values of array v[lo..hi].
 * Precondition:
 *     lo and hi must be valid indices in the array.
*/
template <class T>
void NUMvector_extrema (T *v, long lo, long hi, double *p_min, double *p_max) {
    T min = v[lo];
    T max = min;
    for (long i = lo + 1; i <= hi; i++)
    {
        if (v[i] < min) min = v[i];
        else if (v[i] > max) max = v[i];
    }
    if (p_min) {
		*p_min = min;
	}
	if (p_max) {
		*p_max = max;
	}
}

template <class T>
void NUMmatrix_extrema (T **x, long rb, long re, long cb, long ce, double *p_min, double *p_max) {
    T min = x[rb][cb], max = min;
    for (long i = rb; i <= re; i++) {
        for (long j = cb; j <= ce; j++) {
            T t = x[i][j];
            if (t < min) min = t;
            else if (t > max) max = t;
        }
    }
    if (p_min) {
		*p_min = min;
	}
	if (p_max) {
		*p_max = max;
	}
}



/* NUMvector_clip
    Clip array values.
    c[i] = c[i] < min ? min : (c[i] > max ? max : c[i])
*/
template <class T>
void NUMvector_clip (T *v, long lo, long hi, double min, double max) {
    for (long i = lo; i <= hi; i++)
    {
        if (v[i] < min) v[i] = min;
        else if (v[i] > max) v[i] = max;
    }
}

template<class T>
T ** NUMmatrix_transpose (T **m, long nr, long nc) {
    autoNUMmatrix<T> to (1, nc, 1, nr);

    for (long i = 1; i <= nr; i++) {
        for (long j = 1; j <= nc; j++) {
            to[j][i] = m[i][j];
        }
    }
    return to.transfer();
}

double NUMvector_normalize1 (double v[], long n);

double NUMvector_normalize2 (double v[], long n);

double NUMvector_getNorm1 (const double v[], long n);

double NUMvector_getNorm2 (const double v[], long n);

void  NUMcentreRows (double **a, long rb, long re, long cb, long ce);
/*
	a[i][j] -= a[i][.]
*/
void NUMcentreColumns (double **a, long rb, long re, long cb, long ce, double *centres);
/*
	a[i][j] -= a[.][j]
	if centres != NULL the means are returned in centres[1..re-rb+1]
*/

void NUMdoubleCentre (double **a, long rb, long re, long cb, long ce);
/*
	Function: Make the average value of each column and each row zero.
		a[i][j] += - a[i][.] - a[.][j] + a[.][.]
*/

void NUMnormalizeRows (double **a, long nr, long nc, double norm);

void NUMnormalizeColumns (double **a, long nr, long nc, double norm);
/*
	Scale a[.][j] such that sqrt (Sum(a[i][j]^2, i=1..nPoints)) = norm.
*/

void NUMnormalize (double **a, long nr, long nc, double norm);
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

void NUMvector_avevar (double *a, long n, double *average, double *variance);

void NUMcolumn_avevar (double **a, long nr, long nc, long icol, double *average, double *variance);
/*
	Get mean and variance of a column.
	When average and/or variance are NULL, the corresponding output is
	NOT given.
 */



/* 
 * Given n data points x[1]...x[n], this routine produces the mean and variance
 * 
 * 	mean = sum (i=1, n, x[i]) / n
 * 	var = sum (i=1, n, (x[i] - mean)^2)
 * The sum and the variance are calculated using the pairwise algorithm. 
 * 
 * The algorithm is described in Chan, Golub & Leveque (1979), Updating formulae and a pairwise 
 * algorithm for computing sample variances. Stanford working paper STAN-CS-79-773, 1--22.
 * It was translated from FORTRAN code wich contained a lot of typing errors into C++ by David Weenink.
 * 
 * According to their paper the error analysis shows that the used algorithm is numerically the most stable of 
 * the ones they analyzed (especially for n >> 1 and condition number kappa >> 1).
 */
void NUMmeanAndVariance (double x[], long n, double *p_mean, double *p_var) noexcept;

void NUMcolumn2_avevar (double **a, long nr, long nc, long icol1, long icol2, double *average1, double *variance1, double *average2, double *variance2, double *covariance);
/*
	Get mean and variance of two columns.
	When average and/or variance are NULL, the corresponding output is
	NOT given.
 */

void NUMvector_smoothByMovingAverage (double *xin, long n, long nwindow, double *xout);

void NUMcovarianceFromColumnCentredMatrix (double **x, long nrows, long ncols, long ndf, double **covar);
/*
	Calculate covariance matrix(ncols x ncols) from data matrix (nrows x ncols);
	The matrix covar must already have been allocated and centered.
	covar[i][j] = sum (k=1..nrows, x[i]k]*x[k][j])/(nrows - ndf)
*/

double NUMmultivariateKurtosis (double **x, long nrows, long ncols, int method);
/*
	calculate multivariate kurtosis.
	method = 1 : Schott (2001), J. of Statistical planning and Inference 94, 25-36.
*/

void NUMmad (double *x, long n, double *location, int wantlocation, double *mad, double *work);
/*
	Computes the median absolute deviation, i.e., the median of the
	absolute deviations from the median, and adjust by a factor for
	asymptotically normal consistency, i.e. the returned value is 1.4826*mad which
	makes the returned value "equal" to the standard deviation if the data is normally distributed.
	You either GIVE the median location (if wantlocation = 0) or it
	will be calculated (if wantlocation = 1);

	work is a working array (1..n) that can be used for efficiency reasons.
	If work == NULL, the routine allocates (and destroys) its own memory.
 */

void NUMstatistics_huber (double *x, long n, double *location, int wantlocation,
	double *scale, int wantscale, double k, double tol, double *work);
/*
	Finds the Huber M-estimator for location with scale specified,
	scale with location specified, or both if neither is specified.
	k Winsorizes at `k' standard deviations.

	work is a working array (1..n) that can be used for efficiency reasons.
	If work == NULL, the routine allocates (and destroys) its own memory.
*/

void NUMmonotoneRegression (const double x[], long n, double xs[]);
/*
	Find numbers xs[1..n] that have a monotone relationship with
	the numbers in x[1..n].
	The xs[i] will be ascending.
*/


/* NUMsort2:
    NUMsort2 uses heapsort to sort the second array in parallel with the first one.

    Algorithm follows p. 145 and 642 in:
    Donald E. Knuth (1998): The art of computer programming. Third edition. Vol. 3: sorting and searching.
        Boston: Addison-Wesley, printed may 2002.
    Modification: there is no distinction between record and key and
        Floyd's optimization (page 642) is used.
    Sorts (inplace) an array a[1..n] into ascending order using the Heapsort algorithm,
    while making the corresponding rearrangement of the companion
    array b[1..n]. A characteristic of heapsort is that it does not conserve
    the order of equals: e.g., the array 3,1,1,2 will be sorted as 1,1,2,3.
    It may occur that a_sorted[1] = a_presorted[2] and a_sorted[2] = a_presorted[1]
*/
template<class T1, class T2>
void NUMsort2 (long n, T1 *a, T2 *b) {
    long l, r, j, i, imin;
    T1 k, min;
    T2 kb, min2;
    if (n < 2) {
        return;   /* Already sorted. */
    }
    if (n == 2) {
        if (a[1] > a[2]) {
            min = a[2]; a[2] = a[1]; a[1] = min;
            min2 = b[2]; b[2] = b[1]; b[1] = min2;
        }
        return;
    }
    if (n <= 12) {
        for (i = 1; i < n; i++) {
            min = a[i];
            imin = i;
            for (j = i + 1; j <= n; j++) {
                if (a[j] < min) {
                    min = a [j];
                    imin = j;
                }
            }
            a[imin] = a[i]; a[i] = min;
            min2 = b[imin]; b[imin] = b[i]; b[i] = min2;
        }
        return;
    }
    /* H1 */
    l = (n >> 1) + 1;
    r = n;
    for (;;) {
        if (l > 1) {
            l--;
            k = a[l]; kb = b[l];
        } else /* l == 1 */ {
            k = a[r]; kb = b[r];
            a[r] = a[1]; b[r] = b[1];
            r--;
            if (r == 1) {
                a[1] = k; b[1] = kb; return;
            }
        }
        /* H3 */
        j = l;
        for (;;) { /* H4 */
            i = j;
            j = j << 1;
            if (j > r) break;
            if (j < r && a[j] < a[j + 1]) j++; /* H5 */
            /* if (k >= a[j]) break; H6 */
            a[i] = a[j]; b[i] = b[j]; /* H7 */
        }
        /* a[i] = k; b[i] = kb; H8 */
        for (;;) { /*H8' */
            j = i;
            i = j >> 1;
            /* H9' */
            if (j == l || k <= a[i]) {
                a[j] = k; b[j] = kb; break;
            }
            a[j] = a[i]; b[j] = b[i];
        }
    }
}

void NUMindexx (const double a[], long n, long indx[]);
void NUMindexx_s (char32 *a[], long n, long indx[]);
/*
	Indexes the array a[1..n], i.e., outputs the array indx[1..n] such that
	a[ indx[i] ] is in ascending order for i=1..n;
	No preservation of order amoung equals (see NUMsort2_...)
*/


/* NUMrank:
 *  Replace content of array by rank number, including midranking of ties.
 *  E.g. The elements {10, 20.1, 20.1, 20.1, 20.1, 30} in array a will be replaced
 *  by {1, 3.5, 3.5, 3.5, 3.5, 4}, respectively. *
 */
template <class T>
void NUMrank (long n, T *a) {
    long jt, j = 1;
    while (j < n) {
        for (jt = j + 1; jt <= n && a[jt] == a[j]; jt++) {}
        T rank = (j + jt - 1) * 0.5;
        for (long i = j; i <= jt - 1; i++) {
            a[i] = rank;
        }
        j = jt;
    }
    if (j == n) a[n] = n;
}

void NUMrankColumns (double **m, long rb, long re, long cb, long ce);

void NUMlocate (double *xx, long n, double x, long *index);
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

void NUMtred2 (double **a, long n, double d[], double e[]);
/*
	Householder reduction of a real, symmetric matrix a[1..n][1..n]. On output,
	a is replaced by the orthogonal matrix Q effecting the transformation.
	d[1..n] returns the diagonal elements of the tridiagonal matrix, and e[1..n]
	the off-diagonal elements, with e[1] = 0.
*/

int NUMtqli (double d[], double e[], long n, double **z);
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

int NUMgaussj (double **a, long n, double **b, long m);
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
/*	Given a matrix a[1..n][1..n], this routine replaces it by the
	LU decomposition of a rowwise permutation of itself.
	a	: matrix [1..n][1..n]
	n	: dimension of matrix
	indx	: output vector[1..n] that records the row permutation effected by
			partial pivoting.
	d	: output +/-1 depending on whether the number of ro interchanges was
		even/odd.
*/

int NUMcholeskyDecomposition (double **a, long n, double d[]);
/*
	Cholesky decomposition of a symmetric positive definite matrix.
*/

void NUMcholeskySolve (double **a, long n, double d[], double b[], double x[]);
/*
	Solves A.x=b for x. A[][] and d[] are output from NUMcholeskyDecomposition.
*/

void NUMlowerCholeskyInverse (double **a, long n, double *lnd);
/*
	Calculates L^-1, where A = L.L' is a symmetric positive definite matrix
	and ln(determinant). L^-1 in lower, leave upper part intact.
*/

double **NUMinverseFromLowerCholesky (double **m, long n);
/*
	Return the complete matrix inverse (m x m).
	Input is the lower Cholesky decomposition of the inverse as calculated by NUMlowerCholeskyInverse.
*/

void NUMdeterminant_cholesky (double **a, long n, double *lnd);
/*
	ln(determinant) of a symmetric p.s.d. matrix
*/

double NUMmahalanobisDistance_chi (double **l, double *v, double *m, long nr, long nc);
/*
	Calculates squared Mahalanobis distance: (v-m)'S^-1(v-m).
	Input matrix (li) is the inverse L^-1 of the Cholesky decomposition S = L.L'
	as calculated by NUMlowerCholeskyInverse or 1-row for a diagonal matrix (nr =1)
	Mahalanobis distance calculation. S = L.L' -> S**-1 = L**-1' . L**-1
		(x-m)'S**-1 (x-m) = (x-m)'L**-1' . L**-1. (x-m) =
			(L**-1.(x-m))' . (L**-1.(x-m))
*/

double NUMtrace (double **a, long n);
double NUMtrace2 (double **a1, double **a2, long n);
/*
	Calculates the trace from the product matrix a1 * a2.
	a1 and a2 are [1..n][1..n] square matrices.
*/

void eigenSort (double d[], double **v, long n, int sort);

void NUMeigensystem (double **a, long n, double **evec, double eval[]);
/*
	Determines the eigensystem of a real, symmetric matrix[1..][1..n].
	Returned are: evec[1..n][1..n] with eigenvectors (columnwise) and
	eval[1..n], the eigenvalues.
	No eigenvectors or eigenvalues are returned when the function is
	called with the corresponding parameter NULL.
	Eigenvalues (with corresponding eigenvectors) are sorted in descending order.
*/

void NUMdmatrix_projectRowsOnEigenspace (double **data, long numberOfRows, long from_col, double **eigenvectors, long numberOfEigenvectors, long dimension, double **projection, long to_col);
/* Input:
	data[numberOfRows, from_col - 1 + my dimension] 
		contains the 'numberOfRows' vectors to be projected on the eigenspace. 
	eigenvectors [numberOfEigenvectors][dimension] 
		the eigenvectors stored as rows
   Input/Output
		projection [numberOfRows, to_colbegin - 1 + numberOfDimensionsToKeep] 
		the projected vectors from 'data'

   Project (part of) the vectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
 */

void NUMdmatrix_projectColumnsOnEigenspace (double **data, long numberOfColumns, double **eigenvectors, long numberOfEigenvectors, long dimension, double **projection);
/* Input:
 	data[dimension, numberOfColumns]
 		contains the column vectors to be projected on the eigenspace.
  eigenvectors [numberOfEigenvectors][dimension]
 		the eigenvectors stored as rowvectors
 Input/Output
 	projection [numberOfEigenvectors, numberOfColumns] 
 		the projected vectors from 'data'
 
 Project the columnvectors in matrix 'data' along the 'numberOfEigenvectors' eigenvectors into the matrix 'projection'.
*/


void NUMdominantEigenvector (double **mns, long n, double *q, double *lambda, double tolerance);
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

void NUMdmatrix_into_principalComponents (double **m, long nrows, long ncols,
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

void NUMprincipalComponents (double **a, long n, long nComponents, double **pc);
/*
	Determines the principal components of a real symmetric matrix
	a[1..n][1..n] as a pc[1..n][1..nComponents] column matrix.
*/

void NUMpseudoInverse (double **y, long nr, long nc, double **yinv, double tolerance);
/*
	Determines the pseudo-inverse Y^-1 of Y[1..nr][1..nc] via s.v.d.
	Alternative notation for pseudo-inverse: (Y'.Y)^-1.Y'
	Returns a [1..nc][1..nr] matrix
*/

long NUMsolveQuadraticEquation (double a, double b, double c, double *x1, double *x2);
/*
	Finds the real roots of ax^2 + bx + c = 0.
	The number of real roots is returned and their locations in x1 and x2.
	If only one root found it is stored in x1.
	If no roots found then x1 and x2 will not be changed.
*/

void NUMsolveEquation (double **a, long nr, long nc, double *b, double tol, double *x);
/*
	Solve the equation: a.x = b;
	a[1..nr][1..nc], b[1..nr] and the unknown x[1..nc]
	a & b are destroyed during the computation.
	Algorithm: s.v.d.
*/

void NUMsolveEquations (double **a, long nr, long nc, double **b, long ncb, double tol, double **x);
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

void NUMsolveConstrainedLSQuadraticRegression (double **o, const double y[],
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

void NUMsolveWeaklyConstrainedLinearRegression (double **f, long n, long m, double phi[],
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

void NUMProcrustes (double **x, double **y, long nPoints,
	long nDimensions, double **t, double v[], double *s);
/*
	Given two configurations x and y (nPoints x nDimensions), find the
	the Procrustes rotation/reflection matrix T, the translation vector v and the scaling
	factor s such that Y = sXT+1v' (1 is the nPoints vector with ones).
	Solution: see Borg and Groenen (1997), Modern Multidimensional Scaling, pp 340-346.
	When on input v == NULL or s == NULL, only the matrix T will be solved for:
	the orthogonal Procrustes transform.
*/

void NUMnrbis (void (*f)(double x, double *fx, double *dfx, void *closure), double xmin, double xmax, void *closure, double *root);
/*
	Find the root of a function between xmin and xmax.
	Method: Newton-Raphson with bisection (i.e., derivative is known!).
	Error condition:
		return undefined if root not bracketed.
*/

double NUMridders (double (*f) (double x, void *closure), double xmin, double xmax, void *closure);
/*
	Return the root of a function f bracketed in [xmin, xmax].
	Error condition:
		root not bracketed.
*/

void NUMmspline (double knot[], long nKnots, long order, long i, double x,
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

void NUMispline (double aknot[], long nKnots, long order, long i, double x,
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

double NUMlnBeta (double a, double b);
/*
	Computes the logarithm of the beta function log(B(a,b) subject to
	a and b not being negative integers.
*/

double NUMbeta2 (double z, double w);//temporarily

double NUMbetaContinuedFraction(double a, double b, double x);

double NUMfactln (int n);
/* Returns ln (n!) */

void NUMlngamma_complex (double zr, double zi, double *lnr, double *arg);
/* Log[Gamma(z)] for z complex, z not a negative integer
 * Uses complex Lanczos method. Note that the phase part (arg)
 * is not well-determined when |z| is very large, due
 * to inevitable roundoff in restricting to (-pi, pi].
 * The absolute value part (lnr), however, never suffers.
 *
 * Calculates:
 *   lnr = log|Gamma(z)|
 *   arg = arg(Gamma(z))  in (-Pi, Pi]
 */

/***** STATISTICS: PROBABILITY DENSITY FUNCTIONS ********************/

double NUMlogNormalP (double x, double zeta, double sigma);
/* Area under log normal from 0 to x */

double NUMlogNormalQ (double x, double zeta, double sigma);
/* Area under log normal from x to +infinity */

double NUMstudentP (double t, double df);
/*
	The area under the student T-distribution from -infinity to t.
	Precondition: df > 0
*/

double NUMstudentQ (double t, double df);
/*
	The area under the student T distribution from t to +infinity.
	Precondition: df > 0
*/

double NUMfisherP (double f, double df1, double df2);
/*
	The area under Fisher's F-distribution from 0 to f
	Preconditions: f >= 0, df1 > 0, df2 > 0
*/

double NUMfisherQ (double f, double df1, double df2);
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

double NUMinvChiSquareQ (double p, double df);
/*
	Solves NUMchiSquareQ (chiSquare, df) == p for chiSquare, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvStudentQ (double p, double df);
/*
	Solves NUMstudentQ (t, df) == p for t, given p, df.
	Preconditions: 0 < p < 1, df > 0
*/

double NUMinvFisherQ (double p, double df1, double df2);
/*
	Solves NUMfisherQ (f, df1, df2) == p for f, given p, df1, df2
	Precondition: 0 < p < 1
*/

double NUMtukeyQ (double q, double cc, double df, double rr);
/*    Computes the probability that the maximum of rr studentized
 *    ranges, each based on cc means and with df degrees of freedom
 *    for the standard error, is larger than q.
 */

double NUMinvTukeyQ (double p, double cc, double df, double rr);
/* Solves NUMtukeyQ (q, rr, cc, df) == p for q given p, rr, cc and df.
 * Computes the quantiles of the maximum of rr studentized
 * ranges, each based on cc means and with df degrees of freedom
 * for the standard error, is larger than q.
 *   p = probability (alpha)
 *  rr = no. of rows or groups
 *  cc = no. of columns or treatments
 *  df = degrees of freedom of error term
 */

double NUMnormalityTest_HenzeZirkler (double **data, long n, long p, double *beta, double *tnb, double *lnmu, double *lnvar);
/*
	Multivariate normality test of nxp data matrix according to the method described in Henze & Wagner (1997).
	The test statistic is returned in tnb, together with the lognormal mean 'lnmu' and the lognormal variance 'lnvar'.
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
	Preconditions: f > 0 && bw > 0
*/

int NUMburg (double x[], long n, double a[], int m, double *xms);
/*
	Calculates linear prediction coefficients according to the algorithm
	from J.P. Burg as described by N.Anderson in Childers, D. (ed), Modern
	Spectrum Analysis, IEEE Press, 1978, 252-255.
*/

void NUMdmatrix_to_dBs (double **m, long rb, long re, long cb, long ce,
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

double **NUMcosinesTable (long first, long last, long npoints);
/*
	Generate table with cosines.

	result[i][j] = cos (i * pi * (j - 1/2) / npoints)
	i=first..last; j=1..npoints

	Preconditions:
		1 <= first <= last
		npoints > 1
*/

/******  Interpolation ****/

void NUMspline (double x[], double y[], long n, double yp1, double ypn, double y2[]);
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

void NUMsplint (double xa[], double ya[], double y2a[], long n, double x, double *y);
/*
	Given arrays xa[1..n] and ya[1..n] containing a tabulated function,
	i.e., y[i] = f(x[i]), with x[1] < x[2] < ... < x[n], and given the
	array y2a[1..n] which is the output of NUMspline above, and given
	a value of x, this routine returns an interpolated value y.
*/

double NUMsincpi (const double x);
/* Calculates sin(pi*x)/(pi*x) */
double NUMsinc (const double x);
/* Calculates sin(x)/(x) */

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

bool NUMclipLineWithinRectangle (double xl1, double yl1, double xl2, double yl2, double xr1, double yr1,
	double xr2, double yr2, double *xo1, double *yo1, double *xo2, double *yo2);
/*
	If true, then returns in (xo1, yo1) and (xo2, yo2) the coordinates of that piece of the line (xl1, yl1)..(xl2, yl2)
	that can be drawn within the rectangle with lowerleft corner (xr1, yr1) and upperright (xr2, yr2).
	Returns false if there is nothing to be drawn inside.
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

struct structNUMfft_Table
{
  long n;
  double *trigcache;
  long *splitcache;
};

typedef struct structNUMfft_Table *NUMfft_Table;

void NUMfft_Table_init (NUMfft_Table table, long n);
/*
	n : data size
*/

struct autoNUMfft_Table : public structNUMfft_Table {
        autoNUMfft_Table () throw () {
                n = 0;
                trigcache = 0;
                splitcache = 0;
        }
        ~autoNUMfft_Table () {
                NUMvector_free (trigcache, 0);
                NUMvector_free (splitcache, 0);
        }
};

void NUMfft_forward (NUMfft_Table table, double *data);
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

void NUMfft_backward (NUMfft_Table table, double *data);
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

void NUMforwardRealFastFourierTransform (double  *data, long n);
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
void NUMreverseRealFastFourierTransform (double  *data, long n);
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
void NUMrealft (double *data, long n, int direction);

long NUMgetIndexFromProbability (double *probs, long nprobs, double p);

// Fit the line y= ax+b
void NUMlineFit (double *x, double *y, long numberOfPoints, double *m, double *intercept, int method);
/* method
 * 1 least squares
 * 2 rubust incomplete Theil O(N/2)
 * 3 robust complete Theil (very slow for large N, O(N^2))
 */

void NUMlineFit_theil (double *x, double *y, long numberOfPoints, double *m, double *intercept, bool incompleteMethod);
/*
 * Preconditions:
 *		all x[i] must be different, i.e. x[i] != x[j] for all i = 1..(numberOfPoints - 1), j = (i+1) ..numberOfPoints
 * Algorithm:
 * Theils robust line fit method:
 * 1. Use all combination of pairs (x[i],y[i]), (x[j],y[j]) to calculate an intercept m[k] as
 *    m[k] = (y[j] - y[i]) / (x[j] - x[i]).
 *    There will be (numberOfPoints - 1) * numberOfPoints / 2 numbers m[k].
 * 2. Take the median value m of all the m[k].
 * 3. Calculate the numberOfPoints intercepts b[i] as b[i] = y[i] - m * x[i]
 * 4. Take the median value b of all the b[i] values
 * 
 * If incompleteMethod we use Theil's incomplete method to reduce the number of combinations.
 * I.e. split the data in two equal parts at n2 = numberOfPoints / 2  and then calculate the numberOfPoints/2 intercepts m[i] as
 *   m[i] = (y[n2+i] - y[i]) / (x[n2 + i] - x[i]).
 * The rest proceeds as outlined above
 */


void NUMlineFit_LS (double *x, double *y, long numberOfPoints, double *m, double *intercept);

/* The binomial distribution has the form,

   f(x) =  n!/(x!(n-x)!) * p^x (1-p)^(n-x) for integer 0 <= x <= n
        =  0                               otherwise

   This implementation follows the public domain ranlib function
   "ignbin", the bulk of which is the BTPE (Binomial Triangle
   Parallelogram Exponential) algorithm introduced in
   Kachitvichyanukul and Schmeiser[1].  It has been translated to use
   modern C coding standards.

   If n is small and/or p is near 0 or near 1 (specifically, if
   n*min(p,1-p) < SMALL_MEAN), then a different algorithm, called
   BINV, is used which has an average runtime that scales linearly
   with n*min(p,1-p).

   But for larger problems, the BTPE algorithm takes the form of two
   functions b(x) and t(x) -- "bottom" and "top" -- for which b(x) <
   f(x)/f(M) < t(x), with M = floor(n*p+p).  b(x) defines a triangular
   region, and t(x) includes a parallelogram and two tails.  Details
   (including a nice drawing) are in the paper.

   [1] Kachitvichyanukul, V. and Schmeiser, B. W.  Binomial Random
   Variate Generation.  Communications of the ACM, 31, 2 (February,
   1988) 216.

   Note, Bruce Schmeiser (personal communication) points out that if
   you want very fast binomial deviates, and you are happy with
   approximate results, and/or n and n*p are both large, then you can
   just use gaussian estimates: mean=n*p, variance=n*p*(1-p).

   This implementation by James Theiler, April 2003, after obtaining
   permission -- and some good advice -- from Drs. Kachitvichyanukul
   and Schmeiser to use their code as a starting point, and then doing
   a little bit of tweaking.

   Additional polishing for GSL coding standards by Brian Gough.  */

long NUMrandomBinomial (double p, long n);
double NUMrandomBinomial_real (double p, long n);

// IEEE: Programs for digital signal processing section 4.3 LPTRN (modfied)

// lpc[1..n] to rc[1..n]
void NUMlpc_lpc_to_rc (double *lpc, long p, double *rc);

// rc[1..n] to area[1..n+1], area[m+1] = 0.0001; (1 cm^2)
void NUMlpc_rc_to_area (double *rc, long n, double *area);

// area[1..n] to rc[1..n-1] (modification: LPTRN assumes area[n+1])
void NUMlpc_area_to_rc (double *area, long n, double *rc);

// area[1..n] to lpc[1..n-1]! (modification: lptrn gives lpc[1] = 1 we don't)
void NUMlpc_area_to_lpc (double *area, long n, double *lpc);

// lpc[1..n] to area[1..n+1], area[m+1] = 0.0001; (1 cm^2)
void NUMlpc_lpc_to_area (double *lpc, long m, double *area);

#endif // _NUM2_h_
