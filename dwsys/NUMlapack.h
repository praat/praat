/* NUMlapack.h
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020418 Notify that product svd does not work.
 djmw 20110308 Latest modification
*/
#ifndef _NUMlapack_h_
#define _NUMlapack_h_

#ifndef _NUM2_h_
	#include "NUM2.h"
#endif

#define NUM_NOTRANSPOSE 0
#define NUM_TRANSPOSE 1
#define NUM_LEFT 0
#define NUM_RIGHT 1
#define NUM_BYROW 1
#define NUM_BYCOLUMN 0

/*
From: http://www.netlib.org/lapack/faq.html

LAPACK is a freely-available software package. It is available from netlib
via anonymous ftp and the World Wide Web. Thus, it can be included in
commercial software packages (and has been).
We only ask that proper credit be given to the authors.

Like all software, it is copyrighted. It is not trademarked, but we do ask
the following:

If you modify the source for these routines we ask that you change the name of the routine and comment the changes made to the original.

We will gladly answer any questions regarding the software. If a modification
is done, however, it is the responsibility of the person who modified the
 outine to provide support.

*/

/*
The following routines are modified C versions of LAPACK fortran sources.
Although there also is an official C-version of these fortran routines the
latter have severe drawbacks. They all imply call-by-reference and a fortran
like array structure (column-wise storage).
We have changed the following things:

1. Our matrices are all dimensioned a[1:m, 1:n] (contrary to standard C where
   numbering starts at 0).
2. All the matrices should be allocated as one contiguous block of storage (in C
   only the rows need to be contiguously allocated). A number of procedures
   depend critically on this fact: all procedures that approach the columns of a
   matrix as a vector by indexing its first element and then using an
   'increment' of size 'number of columns' to approach the next elements of the
   vector.
3. In C, array storage is row wise. The leading dimension hack in many routines
   therefor works opposite to the fortran implementation: it must be used for
   indexing COLUMNS instead of rows (lda =1 to index by row, lda=n to index by
   column). This also implies that the leading dimension of a matrix equals the
   number of columns.
   With this trick we can also address vectors from a matrix by column and by
   row, however we have to use a little trick.
   In the fortran routines it often happens that a sub-row or a sub-column is
   passed to a subroutine in which it is referenced as a VECTOR, i.e.
   someArgument(i).
   To copy this in C we have to translate
		call sub (.., A(I,J), ..)
   into:
		sub (.., &A[i][j] - 1, ..)
   We do this with a macro: #define TOVEC(x) &(x) - 1, because our matrices
   are dimensioned as A[1:m,1:n]

   For addressing submatrices in C we can not imitate the fortran trick
    	call somesub (..., A(I,J), lda,...), however.
   We therefor use explicit indexing, i.e., we address the part to be updated:
   as in NUMapplyFactoredHouseholder (c, rb, re, cb, ce,...)
		where rb/re and cb/ce refer to the start/end rows and columns,
		respectively.

3. We changed routine names:

	dnrm2	NUMnorm2
	dlapy2	NUMpythagoras
	dcopy	NUMcopyElements
	ddot	NUMdotproduct
	daxpy	NUMdaxpy
	dscal	NUMvector_scale
	drot	NUMplaneRotation
	dlas2	NUMsvcmp22
	dlartg	NUMfindGivens
	dgeqr2	NUMhouseholderQR
	dgerq2	NUMhouseholderRQ
	dgeqpf	NUMhouseholderQRwithColumnPivoting
	dlarfg	NUMfindHouseholder
	dlarf	NUMapplyFactoredHouseholder
	dorm2r	NUMapplyFactoredHouseholders
	dlapll	NUMparallelVectors
	dlapmt	NUMpermuteColumns
	dlasv2	NUMsvdcmp22
	dlags2	NUMgsvdcmp22
	dtgsja	NUMgsvdFromUpperTriangulars
	dggsvp	NUMmatricesToUpperTriangularForms
*/

void NUMidentity (double **a, integer rb, integer re, integer cb);
/*
	Set a[rb:re, cb:cb+re-rb] to identity matrix.
*/

double NUMpythagoras (double a, double b);
/*
	Returns sqrt (a^2 + b^2), taking care not to cause unnecessary
	overflow.
*/

double NUMnorm2 (integer n, double *x, integer incx);
/*
	Returns the euclidean norm of a vector so that

	NUMvector_norm2 := sqrt (x'*x)
*/

double NUMfrobeniusnorm (integer m, integer n, double **x);
/*
	Returns frobenius norm of matrix sqrt (sum (i=1:m, j=1:n, x[i][j]^2))
*/

double NUMdotproduct (integer n, double x[], integer incx, double y[], integer incy);
/*
	Returns the dot product of two vectors.
*/

void NUMcopyElements (integer n, double x[], integer incx, double y[], integer incy);
/*
	Copies a vector, x, to a vector, y.
*/


void NUMdaxpy (integer n, double da, double x[], integer incx, double y[], integer incy);
/*
	Constant times a vector plus a vector y[] += da*x[].
*/

void NUMvector_scale (integer n, double da, double dx[], integer incx);
/*
	Scales a vector by a constant.
*/

void NUMplaneRotation (integer n, double x[], integer incx, double y[], integer incy, double c, double s);
/*
	Rotates vector's x and y.
*/

void NUMpermuteColumns (int forward, integer m, integer n, double **x, integer *perm);
/*
	Rearranges the columns of the m by n matrix X as specified
	by the permutation perm[1], perm[2], ..., perm[n] of the integers 1, ..., n.
	if forward != 0,  forward permutation:

		x[*,perm[j]] is moved to x[*,j] for j = 1, 2, ..., n.

	if forward == 0, backward permutation:

		x[*,j] is moved to x[*,perm[j]] for j = 1, 2, ..., n.

	arguments
	=========

	forward	!= 0 forward permutation
			== 0 backward permutation

	m		the number of rows of the matrix X. m > 0.

	n		the number of columns of the matrix X. n > 0.

	x		on entry, the matrix x[1:m, 1:n].
			on exit, x contains the permuted matrix X.

	perm	the permutation vector [1..n].
*/

void NUMfindHouseholder (integer n, double *alpha, double x[], integer incx, double *tau);
/*
	Find a real elementary reflector H of order n, such that

		H * ( alpha ) = ( beta ),   H' * H = i.
			(   x   )   (   0  )

	where alpha and beta are scalars, and x is an (n-1)-element real
	vector. H is represented in the form

		H = I - tau * ( 1 ) * ( 1 v' ) ,
					  ( v )

	where tau is a real scalar and v is a real (n-1)-element vector.

	If the elements of x are all zero, then tau = 0 and H is taken to be
	the unit matrix.

	otherwise  1 <= tau <= 2.

	arguments
	=========

	n		the order of the elementary reflector.

	alpha	(input/output) double
			on entry, the value alpha.
			on exit, it is overwritten with the value beta.

	x		(input/output) double array, dimension (1+(n-2)*abs(incx))
			on entry, the vector x.
			on exit, it is overwritten with the vector v.

	incx	the increment between elements of x. incx > 0.

	tau		(output) the value tau.

*/

void NUMfindGivens (double f, double g, double *cs, double *sn, double *r);
/*

	Generate a 2 dimensional rotation so that

		[  cs  sn  ]  .  [ f ]  =  [ r ]   where cs**2 + sn**2 = 1.
		[ -sn  cs  ]     [ g ]     [ 0 ]

		if g=0, then cs=1 and sn=0.
		if f=0 and (g != 0), then cs=0 and sn=1

		if f exceeds g in magnitude, cs will be positive.

	Arguments
	=========

	f       the first component of vector to be rotated.

	g       the second component of vector to be rotated.

	cs      the cosine of the rotation.

	sn      the sine of the rotation.

	r       the nonzero component of the rotated vector.
*/

void NUMapplyFactoredHouseholder (double **c, integer rb, integer re, integer cb, integer ce, double v[], integer incv,
	double tau, int side);
/*
	Applies a real elementary reflector H to the (rb:re, cb:ce) part of a real matrix
	C (1:,1;), from either the left or the right. H is represented in the form

		H = i - tau * v * v'

	where tau is a real scalar and v is a real vector.

	if tau = 0, then H is taken to be the unit matrix.

	arguments
	=========

	side	(input) int
			= NUM_LEFTSIDE:  form  H * C
			= NUM_RIGHTSIDE: form  C * H

	rb, re	begin/end row of the (sub) matrix.

	cb, ce	begin/end columns of the (sub) matrix.

	v		double array, dimension
				(1 + (m-1)*abs(incv)) if side = NUM_LEFTSIDE
				or (1 + (n-1)*abs(incv)) if side = NUM_RIGHTSIDE
			the vector v in the representation of H. v is not used if tau = 0.

	incv	integer
			the increment between elements of v. incv <> 0.

	tau		(input) double
			the value tau in the representation of H.

	c      (input/output) double array, dimension (m,n)
			on entry, the m by n matrix C.
			on exit, C is overwritten by the matrix H * C or C * H.
*/



void NUMapplyFactoredHouseholders (double **c, integer rb, integer re, integer cb, integer ce, double **v,
	integer rbv, integer rev, integer cbv, integer cev, integer incv, double tau[], int side, int trans);
/*
	Overwrites the general real m by n matrix C with

		Q * C  if side = NUM_LEFTSIDE and trans = NUM_NOTRANSPOSE, or

		Q'* C  if side = NUM_LEFTSIDE and trans = NUM_TRANSPOSE, or

		C * Q  if side = NUM_RIGHTSIDE and trans = NUM_NOTRANSPOSE, or

		C * Q' if side = NUM_RIGHTSIDE and trans = NUM_TRANSPOSE,

	where Q is a real orthogonal matrix defined as the product of k
	elementary reflectors

		Q = H(1) H(2) . . . H(k)

	Q is of order m if side = NUM_LEFTSIDE and of order n
	if side = NUM_RIGHTSIDE.

	arguments
	=========

	c		on entry, the matrix c[rb:re, cb:ce].
			on exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

	rb, re	begin/end rows of the matrix C. (m = (re-rb+1)) >= 0.

	cb, ce	begin/end columns of the matrix C. (n = (ce-cb+1)) >= 0.

	k		(input) integer
			the number of elementary reflectors whose product defines
			the matrix Q.
			if side = NUM_LEFTSIDE, m >= k >= 0;
			if side = NUM_RIGHTSIDE, n >= k >= 0.

	v		(input) double array [rbv:rev, cbv:cev].
			the i-th row or column must contain the vector which defines the
			elementary reflector H(i), for i = 1,2,...,k.
			v is modified by the routine but restored on exit.

	rbv, rev	begin/end rows of the matrix V.

	cbv, cev	begin/end columns of the matrix V.


	incv	the increment between elements of v. incv <> 0.
			if incv = 1 the H(i) are row vectors
			if incv > 1 the H(i) are columnvectors

	tau		(input) double array[1:n]
			tau[i] must contain the scalar factor of the elementary
			reflector H(i), as returned by one of the QR or RQ factorizations.

	side	(input) int
			= NUM_LEFTSIDE: apply Q or Q' from the left
			= NUM_RIGHTSIDE: apply Q or Q' from the right

	trans	(input) int
			= NUM_NOTRANSPOSE: apply Q  (no transpose)
			= NUM_TRANSPOSE: apply Q' (transpose)
*/

void NUMeigencmp22 (double a, double b, double c, double *rt1, double *rt2,
	double *cs1, double *sn1 );
/*  NUMeigencmp22 computes the eigendecomposition of a 2-by-2 symmetric matrix
		[  a   b  ]
		[  b   c  ].
	on return, rt1 is the eigenvalue of larger absolute value, rt2 is the
	eigenvalue of smaller absolute value, and (cs1,sn1) is the unit right
	eigenvector for rt1, giving the decomposition

		[ cs1  sn1 ] [  a   b  ] [ cs1 -sn1 ]  =  [ rt1  0  ]
		[-sn1  cs1 ] [  b   c  ] [ sn1  cs1 ]     [  0  rt2 ].



	rt1 is accurate to a few ulps barring over/underflow.

	rt2 may be inaccurate if there is massive cancellation in the
	determinant a*c-b*b; higher precision or correctly rounded or
	correctly truncated arithmetic would be needed to compute rt2
	accurately in all cases.

	cs1 and sn1 are accurate to a few ulps barring over/underflow.

	overflow is possible only if rt1 is within a factor of 5 of overflow.
	underflow is harmless if the input data is 0 or exceeds
	underflow_threshold / macheps.
*/


void NUMhouseholderQR (double **a, integer rb, integer re, integer cb, integer ce, integer ncol, double tau[]);
/*
	Computes a QR factorization of a real (sub) matrix (rb:re, cb:ce) of A (:, :lda)
	A = Q * R.

	arguments
	=========

	a		(input/output) double precision array, dimension (m,n)
			on entry, the m by n matrix A.
			on exit, the elements on and above the diagonal of the array
			contain the min(m,n) by n upper trapezoidal matrix R (R is
			upper triangular if m >= n); the elements below the diagonal,
			with the array tau, represent the orthogonal matrix Q as a
			product of elementary reflectors (see further details).

	rb, re	(input) begin/end rows of the matrix A. (re - rb) >= 0.

	cb, ce	(input)  begin/end columns of the matrix A.  (ce - cb) >= 0

	lda		(input) the leading dimension of the array a, i.e., the number of rows.

	tau		(output) double array, dimension (min(m,n))
			the scalar factors of the elementary reflectors (see further details).

	Further details
	===============

	The matrix Q is represented as a product of elementary reflectors

		Q = H(1) H(2) . . . H(k), where k = min(m,n).

	each H(i) has the form

	H(i) = i - tau * v * v'

	where tau is a real scalar, and v is a real vector with
	v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in a(rb+i-1:ce, cb+i-1),
	and tau in tau(i).
*/


void NUMhouseholderQRwithColumnPivoting (integer m, integer n, double **a, integer ncol, integer *pivot, double tau[]);
/*
	Computes a QR factorization with column pivoting of a
	real m-by-n matrix A: A*P = Q*R.

	arguments
	=========

	m		the number of rows of the matrix A. m >= 0.

	n		the number of columns of the matrix A. n >= 0

	a		on entry, the m-by-n matrix A.
			on exit, the upper triangle of the array contains the
			min(m,n)-by-n upper triangular matrix R; the elements
			below the diagonal, together with the array tau,
			represent the orthogonal matrix Q as a product of
			min(m,n) elementary reflectors.

	ncol	the leading dimension of the array A, number of columns

	pivot	on entry, if pivot[i] != 0, the i-th column of A is permuted
			to the front of A*P (A leading column); if pivot[i] = 0,
			the i-th column of A is a free column.
			on exit, if pivot[i] = k, then the i-th column of A*P
			was the k-th column of A.

	tau		(output) double array, dimension (min(m,n))
			the scalar factors of the elementary reflectors.

	Further details
	===============

	The matrix Q is represented as a product of elementary reflectors

		Q = H(1) H(2) . . . H(n)

	Each H(i) has the form

		H = I - tau * v * v'

	where tau is a real scalar, and v is a real vector with
	v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i).

	The matrix P is represented in pivot as follows: If
		pivot[j] = i
	then the jth column of P is the ith canonical unit vector.
*/


void NUMhouseholderRQ (double **a, integer rb, integer re, integer cb, integer ce, double tau[]);
/*
	Computes an RQ factorization of a real m by n matrix A: A = R * Q.

	arguments
	=========

	rb, re	begin/end rows of the matrix A ==> m = re - rb + 1.

	cb, ce	begin/end columns of the matrix A ==> n = ce -cb + 1.

	a		(input/output) double array, dimension (rb:re, cb:ce)
			on entry, the m by n matrix A.
			on exit, if m <= n, the upper triangle of the subarray
			a(1:m,n-m+1:n) contains the m by m upper triangular matrix R;
			if m >= n, the elements on and above the (m-n)-th subdiagonal
			contain the m by n upper trapezoidal matrix R; the remaining
			elements, with the array tau, represent the orthogonal matrix
			Q as a product of elementary reflectors (see further
			details).

	tau		(output) double array [1: (min(m,n)].
			the scalar factors of the elementary reflectors (see further
			details).

	Further details
	===============

	The matrix Q is represented as a product of elementary reflectors

		Q = H(1) H(2) . . . H(k), where k = min (m,n).

	each H(i) has the form

		H(i) = I - tau * v * v'

	where tau is a real scalar, and v is a real vector with
	v(n-k+i+1:n) = 0 and v(n-k+i) = 1; v(1:n-k+i-1) is stored on exit in
	A(m-k+i,1:n-k+i-1), and tau in tau(i).
*/


void NUMparallelVectors (integer n, double x[], integer incx, double y[], integer incy, double *svmin);
/*
	Given two column vectors x and y, let

		A = ( x y ).

	the subroutine first computes the QR factorization of A = Q*R,
	and then computes the svd of the 2-by-2 upper triangular matrix R.
	The smaller singular value of R is returned in svmin, which is used
	as the measurement of the linear dependency of the vectors x and y.

	arguments
	=========

	n		the length of the vectors x and y.

	x		(input/output) double array [1: (1+(n-1)*incx].
			on entry, x contains the n-vector x.
			on exit, x is overwritten.

	incx	the increment between successive elements of x. incx > 0.

	y		(input/output) double array [1: (1+(n-1)*incy].
			on entry, y contains the n-vector y.
			on exit, y is overwritten.

	incy	the increment between successive elements of y. incy > 0.

	svmin	the smallest singular value of the n-by-2 matrix A = ( x y ).
*/



void NUMsvdcmp22 (double f, double g, double h, double *svmin, double *svmax,
	double *snr, double *csr, double *snl, double *csl);
/*

	Computes the SVD of a 2x2 triangular matrix:
	[ csl snl ] . [ f  g ] . [ csr -snr ] = [ svmax  0 ]
	[-snl csl ]   [ 0  h ]   [ snr  csr]    [ 0  svmin ]

	The absolute value of svmax is larger singular value, absolute value of
	svmin is smaller singular value.
	Both csr^2 + snr^2 = 1 and csl^2 + snl^2 = 1.

	Arguments
	=========

	f, g, h the (1,1), (1,2) and (2,2) element of the 2-by-2 matrix.

	svmin	abs(svmin) is the smaller singular value.

	svmax	abs(svmax) is the larger singular value.

	csl,	the vector (csl, snl) is a unit left singular vector for the
	snl		singular value abs(svmax).


	csr,	the vector (csr, snr) is a unit right singular vector for the
	snr		singular value abs(svmax).


	Further Details
	===============

	Any input parameter may be aliased with any output parameter.

	Barring over/underflow and assuming a guard digit in subtraction, all
	output quantities are correct to within a few units in the last
	place (ulps).

	In IEEE arithmetic, the code works correctly if one matrix element is
	infinite.

	Overflow will not occur unless the largest singular value itself
	overflows or is within a few ulps of overflow. (On machines with
	partial overflow, like the Cray, overflow may occur if the largest
	singular value is within a factor of 2 of overflow.)

	Underflow is harmless if underflow is gradual. Otherwise, results
	may correspond to a matrix modified by perturbations of size near
	the underflow threshold.

	Z. Bai & J. Demmel (1993), "Computing the generalized singular value decomposition",
	SIAM J. Sci. Comput. 14, 1464 - 1486.
*/


void NUMgsvdcmp22 (int upper, int product, double a1, double a2, double a3, double b1, double b2, double b3,
	double *csu, double *snu, double *csv, double *snv, double *csq, double *snq);
/*

	Compute 2-by-2 orthogonal matrices U, V and Q, such
	that if (upper) then

		U'*A*Q = U'*( a1 a2 )*Q = ( x  0  )
					( 0  a3 )     ( x  x  )
	and
		V'*B*Q = V'*( b1 b2 )*Q = ( x  0  )
					( 0  b3 )     ( x  x  )

	or if (! upper ) then

		U'*A*Q = U'*( a1 0  )*Q = ( x  x  )
					( a2 a3 )     ( 0  x  )
	and
		V'*B*Q = V'*( b1 0  )*Q = ( x  x  )
					( b2 b3 )     ( 0  x  )

	the rows of the transformed A and B are parallel, where

		U = (  csu  snu ), V = (  csv snv ), Q = (  csq   snq )
			( -snu  csu )      ( -snv csv )      ( -snq   csq )

	Z' denotes the transpose of Z.


	Arguments
	=========

	upper != 0:	the input matrices A and B are upper triangular.
		   = 0: the input matrices A and B are lower triangular.

	product	if true the product svd is calculated intead of the quotient svd.

	a1, a2, a3	elements of the input 2-by-2
				upper (lower) triangular matrix A.

	b1, b2, b3	elements of the input 2-by-2
				upper (lower) triangular matrix B.

	csu, snu	the desired orthogonal matrix U.

	csv, snv 	the desired orthogonal matrix V.

	csq, snq	the desired orthogonal matrix Q.

*/


void NUMsvcmp22 (double f, double g, double h, double *svmin, double *svmax );
/*

	Compute the singular values of the 2-by-2 triangular matrix
		[  f   g  ]
		[  0   h  ].
	On return, svmin is the smaller singular value and svmax is the
	larger singular value.

	Arguments
	=========

	f, g, h       the [1,1] [1,2] and [2,1] elements of the 2-by-2 matrix.

	svmin   the smaller singular value.

	svmax   the larger singular value.

	Further details
	===============

	Barring over/underflow, all output quantities are correct to within
	a few units in the last place (ulps), even in the absence of a guard
	digit in addition/subtraction.

	In ieee arithmetic, the code works correctly if one matrix element is
	infinite.

	Overflow will not occur unless the largest singular value itself
	overflows, or is within a few ulps of overflow (on machines with
	partial overflow, like the cray, overflow may occur if the largest
	singular value is within a factor of 2 of overflow).

	Underflow is harmless if underflow is gradual. Otherwise, results
	may correspond to a matrix modified by perturbations of size near
	the underflow threshold.

*/


void NUMgsvdFromUpperTriangulars (double **a, integer m, integer n, double **b, integer p,
	int product, integer k, integer l, double tola, double tolb, double *alpha, double *beta,
	double **u, double **v, double **q, integer *ncycle);
/*
	Computes the generalized singular value decomposition (gsvd)
	of two real upper triangular (or trapezoidal) matrices A and B.

	On entry, it is assumed that matrices A and B have the following
	forms, which may be obtained by the preprocessing subroutine dggsvp
	from a general m-by-n matrix A and p-by-n matrix B:

	             n-k-l  k    l
	   A =    k ( 0    A12  A13 ) if m-k-l >= 0;
	          l ( 0     0   A23 )
	      m-k-l ( 0     0    0  )

	           n-k-l  k    l
	   A =  k ( 0    A12  A13 ) if m-k-l < 0;
	      m-k ( 0     0   A23 )

	           n-k-l  k    l
	   B =  l ( 0     0   B13 )
	      p-l ( 0     0    0  )

	where the k-by-k matrix A12 and l-by-l matrix B13 are nonsingular
	upper triangular; A23 is l-by-l upper triangular if m-k-l >= 0,
	otherwise A23 is (m-k)-by-l upper trapezoidal.

	On exit,

	            U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R ),

	where U, V and Q are orthogonal matrices, Z' denotes the transpose
	of Z, R is a nonsingular upper triangular matrix, and D1 and D2 are
	``diagonal'' matrices, which are of the following structures:

	if m-k-l >= 0,

	                    k  l
	       D1 =     k ( I  0 )
	                l ( 0  C )
	            m-k-l ( 0  0 )

	                  k  l
	       D2 = l   ( 0  S )
	            p-l ( 0  0 )

	               n-k-l  k    l
	  ( 0 R ) = k (  0   R11  R12 ) k
	            l (  0    0   R22 ) l

	where

	  C = Diag (alpha(k+1), ... , alpha(k+l)),
	  S = Diag (beta(k+1),  ... , beta(k+l)),
	  C**2 + S**2 = i.

	  R is stored in A(1:k+l,n-k-l+1:n) on exit.

	if m-k-l < 0,

	               k m-k k+l-m
	    D1 =   k ( I  0    0   )
	         m-k ( 0  C    0   )

	                 k m-k k+l-m
	    D2 =   m-k ( 0  S    0   )
	         k+l-m ( 0  0    I   )
	           p-l ( 0  0    0   )

	               n-k-l  k   m-k  k+l-m
   ( 0 R ) =    k ( 0    R11  R12  R13  )
	          m-k ( 0     0   R22  R23  )
	        k+l-m ( 0     0    0   R33  )

	where
	C = Diag (alpha(k+1), ... , alpha(m)),
	S = Diag (beta(k+1),  ... , beta(m)),
	C**2 + S**2 = i.

	R = ( R11 R12 R13 ) is stored in A(1:m, n-k-l+1:n) and R33 is stored
	    (  0  R22 R23 )
	in B(m-k+1:l,n+m-k-l+1:n) on exit.

	the computation of the orthogonal transformation matrices U, V or Q
	is optional.  these matrices may either be formed explicitly, or they
	may be postmultiplied into input matrices U1, V1, or Q1.

	arguments
	=========

	m       the number of rows of the matrix A.  m >= 0.

	p       the number of rows of the matrix B.  p >= 0.

	n       the number of columns of the matrices A and B.  n >= 0.

	k, l    k and l specify the subblocks in the input matrices A and B:
	        A23 = A(k+1:min(k+l,m), n-l+1:n) and B13 = B(1:l, n-l+1:n)
	        of A and B, whose gsvd is going to be computed.
	        see further details.

	a       on entry, the m-by-n matrix A.
	        on exit, A(n-k+1:n, 1:min(k+l,m) ) contains the triangular
	        matrix R or part of R.  see purpose for details.

	b       on entry, the p-by-n matrix B.
	        on exit, if necessary, B(m-k+1:l, n+m-k-l+1:n) contains
	        a part of R.  see purpose for details.

	product	if true the product svd is calculated instead of the quotient svd

	tola,	the convergence criteria for the Jacobi- Kogbetliantz
	tolb    iteration procedure. Generally, they are the
	        same as used in the preprocessing step, say
	            tola = max(m,n) * norm(a) * macheps,
	            tolb = max(p,n) * norm(b) * macheps.

	alpha,	array[1:n]
	beta	on exit, alpha and beta contain the generalized singular
	        value pairs of A and B;
				alpha(1:k) = 1,
				beta(1:k)  = 0,
	        and if m-k-l >= 0,
				alpha(k+1:k+l) = diag(C),
				beta(k+1:k+l)  = diag(S),
	        or if m-k-l < 0,
				alpha(k+1:m)= C, alpha(m+1:k+l)= 0
				beta(k+1:m) = S, beta(m+1:k+l) = 1.
	        furthermore, if k+l < n,
				alpha(k+l+1:n) = 0 and
				beta(k+l+1:n)  = 0.

	u       on entry, if u != NULL, u must contain a matrix U1 (usually
	        the orthogonal matrix returned by NUMmatricesToUpperTriangularForms).
	        on exit,
	        if u != NULL, u contains the product U1*U.
	        if u == NULL, u is not referenced.

	v       on entry, if v != NULL, v must contain a matrix V1 (usually
	        the orthogonal matrix returned by NUMmatricesToUpperTriangularForms).
	        on exit,
	        if u != NULL, v contains the product V1*V.
	        if u == NULL, v is not referenced.

	q       on entry, if u != NULL, q must contain a matrix Q1 (usually
	        the orthogonal matrix returned by NUMmatricesToUpperTriangularForms).
	        on exit,
	        if u != NULL, q contains the product Q1*Q.
	        if u == NULL, q is not referenced.

	ncycle  the number of cycles required for convergence.

	internal parameters
	===================

	maxit   specifies the total loops that the iterative procedure
	        may take. if after maxit cycles, the routine fails to
	        converge, we return with an error message.

	Further details
	===============

	NUMgsvdFromUpperTriangulars essentially uses a variant of Kogbetliantz algorithm
	to reduce min(l,m-k)-by-l triangular (or trapezoidal) matrix A23 and l-by-l
	matrix B13 to the form:

	         U1'*A13*Q1 = C1*R1; V1'*B13*Q1 = S1*R1,

	where U1, V1 and Q1 are orthogonal matrix, and Z' is the transpose
	of Z.  C1 and S1 are diagonal matrices satisfying

	              C1**2 + S1**2 = I,

	and R1 is an l-by-l nonsingular upper triangular matrix.
*/


void NUMmatricesToUpperTriangularForms (double **a, integer m, integer n, double **b, integer p,
	double tola, double tolb, integer *kk, integer *ll, double **u, double **v, double **q);
/*
	Computes orthogonal matrices U, V and Q such that

					n-k-l  k    l
	U'*A*Q =     k ( 0    A12  A13 )  if m-k-l >= 0;
				 l ( 0     0   A23 )
			 m-k-l ( 0     0    0  )

					n-k-l  k    l
		   =     k ( 0    A12  A13 )  if m-k-l < 0;
			   m-k ( 0     0   A23 )

				  n-k-l  k    l
	V'*B*Q =   l ( 0     0   B13 )
			 p-l ( 0     0    0  )

	where the k-by-k matrix A12 and l-by-l matrix B13 are nonsingular
	upper triangular; A23 is l-by-l upper triangular if m-k-l >= 0,
	otherwise A23 is (m-k)-by-l upper trapezoidal.  k+l = the effective
	numerical rank of the (m+p)-by-n matrix (A',B')'. Z' denotes the
	transpose of Z.

	This decomposition is the preprocessing step for computing the
	generalized singular value decomposition (gsvd), see subroutine
	NUMgsvdFromUpperTriangulars.

	arguments
	=========

	m		the number of rows of the matrix A.  m > 0.

	p		the number of rows of the matrix B.  p > 0.

	n		the number of columns of the matrices A and B.  n > 0.

	a		on entry, the m-by-n matrix A.
			on exit, A contains the triangular (or trapezoidal) matrix
			described in the purpose section.

	b		on entry, the p-by-n matrix B.
			on exit, B contains the triangular matrix described in
			the purpose section.


	tola,	the thresholds to determine the effective numerical rank of matrix B
	tolb	and a subblock of A. Generally, they are set to
				tola = max(m,n)*norm(A)*macheps,
				tolb = max(p,n)*norm(B)*macheps.
			the size of tola and tolb may affect the size of backward
			errors of the decomposition.

	k, l	on exit, k and l specify the dimension of the subblocks
			described in purpose.
			k + l = effective numerical rank of (A',B')'.

	u       if u != NULL, u contains the orthogonal matrix U.
			if u == NULL, u is not referenced.

	v		if v != NULL, v contains the orthogonal matrix V.
			if v == NULL, v is not referenced.

	q		if q != NULL, q contains the orthogonal matrix Q.
			if q == NULL, q is not referenced.

	Further details
	===============

	The program uses NUMhouseholderQRwithColumnPivoting for the QR factorization
	to detect the effective numerical rank of the A matrix.
	It may be replaced by a better rank determination strategy.
*/


void NUMgsvdcmp (double **a, integer m, integer n, double **b, integer p, int product, integer *k, integer *l,
	double *alpha, double *beta, double **u, double **v, double **q, int invertR);
/*
	Computes the generalized singular value decomposition (gsvd)
	of an m-by-n real matrix A and p-by-n real matrix B:

		U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R )

	where U, V and Q are orthogonal matrices, and Z' is the transpose
	of Z.  Let k+l = the effective numerical rank of the matrix (A',B')',
	then R is a k+l-by-k+l nonsingular upper triangular matrix, D1 and
	D2 are m-by-(k+l) and p-by-(k+l) "diagonal" matrices and of the
	following structures, respectively:

	if m-k-l >= 0,

					 k  l
		D1 =     k ( I  0 )
				 l ( 0  C )
			 m-k-l ( 0  0 )

				   k  l
		D2 =   l ( 0  S )
			 p-l ( 0  0 )

				n-k-l  k    l
	( 0 R ) = k (  0   R11  R12 )
			  l (  0    0   R22 )

	where

	C = Diag( alpha(k+1), ... , alpha(k+l) ),
	S = Diag( beta(k+1),  ... , beta(k+l) ),
	C**2 + S**2 = I.

	R is stored in A(1:k+l,n-k-l+1:n) on exit.

	if m-k-l < 0,

					   k m-k k+l-m
			D1 =   k ( I  0    0   )
				 m-k ( 0  C    0   )

						 k m-k k+l-m
			D2 =   m-k ( 0  S    0  )
				 k+l-m ( 0  0    I  )
				   p-l ( 0  0    0  )

					 n-k-l  k   m-k  k+l-m
	( 0 R ) =     k ( 0    R11  R12  R13  )
				m-k ( 0     0   R22  R23  )
			  k+l-m ( 0     0    0   R33  )

	where

	C = diag( alpha(k+1), ... , alpha(m) ),
	S = diag( beta(k+1),  ... , beta(m) ),
	C**2 + S**2 = I.

		(R11 R12 R13 ) is stored in A(1:m, n-k-l+1:n), and R33 is stored
		( 0  R22 R23 )
	in B(m-k+1:l,n+m-k-l+1:n) on exit.

	The routine computes C, S, R, and optionally the orthogonal
	transformation matrices U, V and Q.

	In particular, if B is an n-by-n nonsingular matrix and product == 0,
	then the gsvd of A and B implicitly gives the (quotient) svd of A * inv(B):
								A * inv(B) = U*(D1*inv(D2))*V',
	If product != 0 then the gsvd of A and B implicitly gives the product svd
	of A * B: A * B = U * (D1 * D2) * V'.
	If ( A',B')' has orthonormal columns, then the gsvd of A and B is
	also equal to the CS decomposition of A and B. Furthermore, the gsvd
	can be used to derive the solution of the eigenvalue problem:
					A'*A x = lambda* B'*B x.
	In some literature, the gsvd of A and B is presented in the form
				U'*A*X = ( 0 D1 ),   V'*B*X = ( 0 D2 )
	where U and V are orthogonal and X is nonsingular, D1 and D2 are
	``diagonal''. The former gsvd form can be converted to the latter
	form by taking the nonsingular matrix X as

					X = Q*( I   0    )
						  ( 0 inv(R) ).

 	arguments
	=========

	a		on entry, the m-by-n matrix A.
			on exit, a contains the triangular matrix R, or part of R.
			see purpose for details.

	m		the number of rows of the matrix A.  m > 0.

	n       the number of columns of the matrices A and B.  n > 0.

	b		on entry, the p-by-n matrix B.
			on exit, b contains the triangular matrix R if m-k-l < 0.
			see purpose for details.

	p       the number of rows of the matrix B.  p > 0.

	k, l    on exit, k and l specify the dimension of the subblocks
			described in the purpose section.
			k + l = effective numerical rank of (A',B')'.

	product	svd of A * B instead of A * inv (B).
			Watch out: you can only use product=0 (i.e. QSVD).

	alpha,	on exit, alpha and beta contain the generalized singular
	beta	value pairs of A and B;
				alpha(1:k) = 1,
				beta(1:k)  = 0,
			and if m-k-l >= 0,
				alpha(k+1:k+l) = C,
			  beta(k+1:k+l)  = S,
			or if m-k-l < 0,
				alpha(k+1:m)=C, alpha(m+1:k+l)=0
				beta(k+1:m) =S, beta(m+1:k+l) =1
			and
				alpha(k+l+1:n) = 0
				beta(k+l+1:n)  = 0

	u		if u != NULL, u contains the m-by-m orthogonal matrix U.
			if u == NULL, u is not referenced.

	v		if v != NULL, v contains the p-by-p orthogonal matrix V.
			if v == NULL, v is not referenced.

	q		if q != NULL, q contains the n-by-n orthogonal matrix Q.
			if q == NULL, q is not referenced.

	Internal parameters
	===================

	tola	double
	tolb	double
			tola and tolb are the thresholds to determine the effective
			rank of (A',B')'. Generally, they are set to
				tola = max(m,n) * norm(A) * macheps,
				tolb = max(p,n) * norm(B) * macheps.
			the size of tola and tolb may affect the size of backward
			errors of the decomposition.
*/

void NUMtriangularInverse (int upper, int unitDiagonal, integer n, double **a);
/*
	Computes inverse of triangular matrix.

*/

#endif /* _NUMlapack_h_ */
