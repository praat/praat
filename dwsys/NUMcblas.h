 #ifndef _NUMcblas_h_
 #define _NUMcblas_h_
 /* NUMcblas.h
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
 djmw 20020923 GPL header
 djmw 20110308 Latest modification
*/

#define xerbla_(src,info) Melder_throw (Melder_peek8to32 (src), U": parameter ", *info, U"not correct!")

int NUMblas_daxpy (integer *n, double *da, double *dx,	integer *incx, double *dy, integer *incy);
/*     constant times a vector plus a vector.
       uses unrolled loops for increments equal to one. */

int NUMblas_dcopy (integer *n, double *dx, integer *incx, double *dy, integer *incy);
/*     copies a vector, x, to a vector, y.
       uses unrolled loops for increments equal to one.
*/

double NUMblas_ddot (integer *n, double *dx, integer *incx, double *dy, integer *incy);
/*     forms the dot product of two vectors.
       uses unrolled loops for increments equal to one. */

int NUMblas_dgemm (const char *transa, const char *transb, integer *m, integer *n, integer *k, double *alpha,
	double *a, integer *lda, double *b, integer *ldb, double *beta, double *c, integer *ldc);
/*  Purpose
    =======
    NUMblas_dgemm  performs one of the matrix-matrix operations
       C := alpha*op( A )*op( B ) + beta*C,
    where  op( X ) is one of
       op( X ) = X   or   op( X ) = X',
    alpha and beta are scalars, and A, B and C are matrices, with op( A )
    an m by k matrix,  op( B )  a  k by n matrix and  C an m by n matrix.
    Parameters
    ==========
    TRANSA - char*.
             On entry, TRANSA specifies the form of op( A ) to be used in
             the matrix multiplication as follows:
                TRANSA = 'N' or 'n',  op( A ) = A.
                TRANSA = 'T' or 't',  op( A ) = A'.
                TRANSA = 'C' or 'c',  op( A ) = A'.
             Unchanged on exit.
    TRANSB - char*.
             On entry, TRANSB specifies the form of op( B ) to be used in
             the matrix multiplication as follows:
                TRANSB = 'N' or 'n',  op( B ) = B.
                TRANSB = 'T' or 't',  op( B ) = B'.
                TRANSB = 'C' or 'c',  op( B ) = B'.
             Unchanged on exit.
    M      - long.
             On entry,  M  specifies  the number  of rows  of the  matrix
             op( A )  and of the  matrix  C.  M  must  be at least  zero.
             Unchanged on exit.
    N      - long.
             On entry,  N  specifies the number  of columns of the matrix
             op( B ) and the number of columns of the matrix C. N must be
             at least zero.
             Unchanged on exit.
    K      - long.
             On entry,  K  specifies  the number of columns of the matrix
             op( A ) and the number of rows of the matrix op( B ). K must
             be at least  zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, ka ), where ka is
             k  when  TRANSA = 'N' or 'n',  and is  m  otherwise.
             Before entry with  TRANSA = 'N' or 'n',  the leading  m by k
             part of the array  A  must contain the matrix  A,  otherwise
             the leading  k by m  part of the array  A  must contain  the
             matrix A.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. When  TRANSA = 'N' or 'n' then
             LDA must be at least  max( 1, m ), otherwise  LDA must be at
             least  max( 1, k ).
             Unchanged on exit.
    B      - double array of DIMENSION ( LDB, kb ), where kb is
             n  when  TRANSB = 'N' or 'n',  and is  k  otherwise.
             Before entry with  TRANSB = 'N' or 'n',  the leading  k by n
             part of the array  B  must contain the matrix  B,  otherwise
             the leading  n by k  part of the array  B  must contain  the
             matrix B.
             Unchanged on exit.
    LDB    - long.
             On entry, LDB specifies the first dimension of B as declared
             in the calling (sub) program. When  TRANSB = 'N' or 'n' then
             LDB must be at least  max( 1, k ), otherwise  LDB must be at
             least  max( 1, n ).
             Unchanged on exit.
    BETA   - double.
             On entry,  BETA  specifies the scalar  beta.  When  BETA  is
             supplied as zero then C need not be set on input.
             Unchanged on exit.
    C      - double array of DIMENSION ( LDC, n ).
             Before entry, the leading  m by n  part of the array  C must
             contain the matrix  C,  except when  beta  is zero, in which
             case C need not be set on entry.
             On exit, the array  C  is overwritten by the  m by n  matrix
             ( alpha*op( A )*op( B ) + beta*C ).
    LDC    - long.
             On entry, LDC specifies the first dimension of C as declared
             in  the  calling  (sub)  program.   LDC  must  be  at  least
             max( 1, m ).
             Unchanged on exit.
    Level 3 Blas routine.
    -- Written on 8-February-1989.
       Jack Dongarra, Argonne National Laboratory.
       Iain Duff, AERE Harwell.
       Jeremy Du Croz, Numerical Algorithms Group Ltd.
       Sven Hammarling, Numerical Algorithms Group Ltd.
*/

int NUMblas_dger (integer *m, integer *n, double *alpha, double *x, integer *incx, double *y,
	integer *incy, double *a, integer *lda);
/*  Purpose
    =======
    NUMblas_dger   performs the rank 1 operation
       A := alpha*x*y' + A,
    where alpha is a scalar, x is an m element vector, y is an n element
    vector and A is an m by n matrix.
    Parameters
    ==========
    M      - long.
             On entry, M specifies the number of rows of the matrix A.
             M must be at least zero.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the number of columns of the matrix A.
             N must be at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    X      - double array of dimension at least
             ( 1 + ( m - 1 )*abs( INCX ) ).
             Before entry, the incremented array X must contain the m
             element vector x.
             Unchanged on exit.
    INCX   - long.
             On entry, INCX specifies the increment for the elements of
             X. INCX must not be zero.
             Unchanged on exit.
    Y      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCY ) ).
             Before entry, the incremented array Y must contain the n
             element vector y.
             Unchanged on exit.
    INCY   - long.
             On entry, INCY specifies the increment for the elements of
             Y. INCY must not be zero.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, n ).
             Before entry, the leading m by n part of the array A must
             contain the matrix of coefficients. On exit, A is
             overwritten by the updated matrix.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. LDA must be at least
             max( 1, m ).
             Unchanged on exit.
    Level 2 Blas routine.
    -- Written on 22-October-1986.
       Jack Dongarra, Argonne National Lab.
       Jeremy Du Croz, Nag Central Office.
       Sven Hammarling, Nag Central Office.
       Richard Hanson, Sandia National Labs.
*/

int NUMblas_dgemv (const char *trans, integer *m, integer *n, double *alpha, double *a, integer *lda,
	double *x, integer *incx, double *beta, double *y, integer *incy);
/*  Purpose
    =======
    NUMblas_dgemv  performs one of the matrix-vector operations
       y := alpha*A*x + beta*y,   or   y := alpha*A'*x + beta*y,
    where alpha and beta are scalars, x and y are vectors and A is an
    m by n matrix.
    Parameters
    ==========
    TRANS  - char*.
             On entry, TRANS specifies the operation to be performed as
             follows:
                TRANS = 'N' or 'n'   y := alpha*A*x + beta*y.
                TRANS = 'T' or 't'   y := alpha*A'*x + beta*y.
                TRANS = 'C' or 'c'   y := alpha*A'*x + beta*y.
             Unchanged on exit.
    M      - long.
             On entry, M specifies the number of rows of the matrix A.
             M must be at least zero.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the number of columns of the matrix A.
             N must be at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, n ).
             Before entry, the leading m by n part of the array A must
             contain the matrix of coefficients.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. LDA must be at least
             max( 1, m ).
             Unchanged on exit.
    X      - double array of DIMENSION at least
             ( 1 + ( n - 1 )*abs( INCX ) ) when TRANS = 'N' or 'n'
             and at least
             ( 1 + ( m - 1 )*abs( INCX ) ) otherwise.
             Before entry, the incremented array X must contain the
             vector x.
             Unchanged on exit.
    INCX   - long.
             On entry, INCX specifies the increment for the elements of
             X. INCX must not be zero.
             Unchanged on exit.
    BETA   - double.
             On entry, BETA specifies the scalar beta. When BETA is
             supplied as zero then Y need not be set on input.
             Unchanged on exit.
    Y      - double array of DIMENSION at least
             ( 1 + ( m - 1 )*abs( INCY ) ) when TRANS = 'N' or 'n'
             and at least
             ( 1 + ( n - 1 )*abs( INCY ) ) otherwise.
             Before entry with BETA non-zero, the incremented array Y
             must contain the vector y. On exit, Y is overwritten by the
             updated vector y.
    INCY   - long.
             On entry, INCY specifies the increment for the elements of
             Y. INCY must not be zero.
             Unchanged on exit.
    Level 2 Blas routine.
    -- Written on 22-October-1986.
       Jack Dongarra, Argonne National Lab.
       Jeremy Du Croz, Nag Central Office.
       Sven Hammarling, Nag Central Office.
       Richard Hanson, Sandia National Labs.
*/

double NUMblas_dlamch (const char *cmach);
/*  Purpose
    =======

    NUMblas_dlamch determines double machine parameters.

    Arguments
    =========

    CMACH   (input) char*
            Specifies the value to be returned by DLAMCH:
            = 'E' or 'e',   DLAMCH := eps
            = 'S' or 's ,   DLAMCH := sfmin
            = 'B' or 'b',   DLAMCH := base
            = 'P' or 'p',   DLAMCH := eps*base
            = 'N' or 'n',   DLAMCH := t
            = 'R' or 'r',   DLAMCH := rnd
            = 'M' or 'm',   DLAMCH := emin
            = 'U' or 'u',   DLAMCH := rmin
            = 'L' or 'l',   DLAMCH := emax
            = 'O' or 'o',   DLAMCH := rmax

            where

            eps   = relative machine precision
            sfmin = safe minimum, such that 1/sfmin does not overflow
            base  = base of the machine
            prec  = eps*base
            t     = number of (base) digits in the mantissa
            rnd   = 1.0 when rounding occurs in addition, 0.0 otherwise
            emin  = minimum exponent before (gradual) underflow
            rmin  = underflow threshold - base**(emin-1)
            emax  = largest exponent before overflow
            rmax  = overflow threshold  - (base**emax)*(1-eps)

   =====================================================================
*/

double NUMblas_dnrm2 (integer *n, double *x, integer *incx);
/*
	NUMblas_dnrm2 returns the euclidean norm of a vector via the function
    name, so that
       DNRM2 := sqrt( x'*x )
    -- This version written on 25-October-1982.
       Modified on 14-October-1993 to inline the call to DLASSQ.
       Sven Hammarling, Nag Ltd.
       Parameter adjustments
*/

int NUMblas_drot (integer *n, double *dx, integer *incx, double *dy, integer *incy,
	double *c__, double *s);
/*     applies a plane rotation.  */

int NUMblas_dscal (integer *n, double *da, double *dx, integer *incx);
/*     scales a vector by a constant.
       uses unrolled loops for increment equal to one.
*/

int NUMblas_dswap (integer *n, double *dx, integer *incx,	double *dy, integer *incy);
/*     interchanges two vectors.
       uses unrolled loops for increments equal one. */

int NUMblas_dsymv (const char *uplo, integer *n, double *alpha, double *a, integer *lda,
	double *x, integer *incx, double *beta, double *y, integer *incy);
/*  Purpose
    =======
    NUMblas_dsymv  performs the matrix-vector  operation
       y := alpha*A*x + beta*y,
    where alpha and beta are scalars, x and y are n element vectors and
    A is an n by n symmetric matrix.
    Parameters
    ==========
    UPLO   - char*.
             On entry, UPLO specifies whether the upper or lower
             triangular part of the array A is to be referenced as
             follows:
                UPLO = 'U' or 'u'   Only the upper triangular part of A
                                    is to be referenced.
                UPLO = 'L' or 'l'   Only the lower triangular part of A
                                    is to be referenced.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the order of the matrix A.
             N must be at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, n ).
             Before entry with  UPLO = 'U' or 'u', the leading n by n
             upper triangular part of the array A must contain the upper
             triangular part of the symmetric matrix and the strictly
             lower triangular part of A is not referenced.
             Before entry with UPLO = 'L' or 'l', the leading n by n
             lower triangular part of the array A must contain the lower
             triangular part of the symmetric matrix and the strictly
             upper triangular part of A is not referenced.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. LDA must be at least
             max( 1, n ).
             Unchanged on exit.
    X      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCX ) ).
             Before entry, the incremented array X must contain the n
             element vector x.
             Unchanged on exit.
    INCX   - long.
             On entry, INCX specifies the increment for the elements of
             X. INCX must not be zero.
             Unchanged on exit.
    BETA   - double.
             On entry, BETA specifies the scalar beta. When BETA is
             supplied as zero then Y need not be set on input.
             Unchanged on exit.
    Y      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCY ) ).
             Before entry, the incremented array Y must contain the n
             element vector y. On exit, Y is overwritten by the updated
             vector y.
    INCY   - long.
             On entry, INCY specifies the increment for the elements of
             Y. INCY must not be zero.
             Unchanged on exit.
*/

int NUMblas_dsyr2 (const char *uplo, integer *n, double *alpha,	double *x, integer *incx,
	double *y, integer *incy, double *a, integer *lda);
/*  Purpose
    =======
    NUMblas_dsyr2  performs the symmetric rank 2 operation
       A := alpha*x*y' + alpha*y*x' + A,
    where alpha is a scalar, x and y are n element vectors and A is an n
    by n symmetric matrix.
    Parameters
    ==========
    UPLO   - char*.
             On entry, UPLO specifies whether the upper or lower
             triangular part of the array A is to be referenced as
             follows:
                UPLO = 'U' or 'u'   Only the upper triangular part of A
                                    is to be referenced.
                UPLO = 'L' or 'l'   Only the lower triangular part of A
                                    is to be referenced.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the order of the matrix A.
             N must be at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    X      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCX ) ).
             Before entry, the incremented array X must contain the n
             element vector x.
             Unchanged on exit.
    INCX   - long.
             On entry, INCX specifies the increment for the elements of
             X. INCX must not be zero.
             Unchanged on exit.
    Y      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCY ) ).
             Before entry, the incremented array Y must contain the n
             element vector y.
             Unchanged on exit.
    INCY   - long.
             On entry, INCY specifies the increment for the elements of
             Y. INCY must not be zero.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, n ).
             Before entry with  UPLO = 'U' or 'u', the leading n by n
             upper triangular part of the array A must contain the upper
             triangular part of the symmetric matrix and the strictly
             lower triangular part of A is not referenced. On exit, the
             upper triangular part of the array A is overwritten by the
             upper triangular part of the updated matrix.
             Before entry with UPLO = 'L' or 'l', the leading n by n
             lower triangular part of the array A must contain the lower
             triangular part of the symmetric matrix and the strictly
             upper triangular part of A is not referenced. On exit, the
             lower triangular part of the array A is overwritten by the
             lower triangular part of the updated matrix.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. LDA must be at least
             max( 1, n ).
             Unchanged on exit.
*/

int NUMblas_dsyr2k (const char *uplo, const char *trans, integer *n, integer *k, double *alpha, double *a,
	integer *lda, double *b, integer *ldb, double *beta, double *c, integer *ldc);
/*  Purpose
    =======
    NUMblas_dsyr2k  performs one of the symmetric rank 2k operations
       C := alpha*A*B' + alpha*B*A' + beta*C,
    or
       C := alpha*A'*B + alpha*B'*A + beta*C,
    where  alpha and beta  are scalars, C is an  n by n  symmetric matrix
    and  A and B  are  n by k  matrices  in the  first  case  and  k by n
    matrices in the second case.
    Parameters
    ==========
    UPLO   - char*.
             On  entry,   UPLO  specifies  whether  the  upper  or  lower
             triangular  part  of the  array  C  is to be  referenced  as
             follows:
                UPLO = 'U' or 'u'   Only the  upper triangular part of  C
                                    is to be referenced.
                UPLO = 'L' or 'l'   Only the  lower triangular part of  C
                                    is to be referenced.
             Unchanged on exit.
    TRANS  - char*.
             On entry,  TRANS  specifies the operation to be performed as
             follows:
                TRANS = 'N' or 'n'   C := alpha*A*B' + alpha*B*A' +
                                          beta*C.
                TRANS = 'T' or 't'   C := alpha*A'*B + alpha*B'*A +
                                          beta*C.
                TRANS = 'C' or 'c'   C := alpha*A'*B + alpha*B'*A +
                                          beta*C.
             Unchanged on exit.
    N      - long.
             On entry,  N specifies the order of the matrix C.  N must be
             at least zero.
             Unchanged on exit.
    K      - long.
             On entry with  TRANS = 'N' or 'n',  K  specifies  the number
             of  columns  of the  matrices  A and B,  and on  entry  with
             TRANS = 'T' or 't' or 'C' or 'c',  K  specifies  the  number
             of rows of the matrices  A and B.  K must be at least  zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry, ALPHA specifies the scalar alpha.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, ka ), where ka is
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k
             part of the array  A  must contain the matrix  A,  otherwise
             the leading  k by n  part of the array  A  must contain  the
             matrix A.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n'
             then  LDA must be at least  max( 1, n ), otherwise  LDA must
             be at least  max( 1, k ).
             Unchanged on exit.
    B      - double array of DIMENSION ( LDB, kb ), where kb is
             k  when  TRANS = 'N' or 'n',  and is  n  otherwise.
             Before entry with  TRANS = 'N' or 'n',  the  leading  n by k
             part of the array  B  must contain the matrix  B,  otherwise
             the leading  k by n  part of the array  B  must contain  the
             matrix B.
             Unchanged on exit.
    LDB    - long.
             On entry, LDB specifies the first dimension of B as declared
             in  the  calling  (sub)  program.   When  TRANS = 'N' or 'n'
             then  LDB must be at least  max( 1, n ), otherwise  LDB must
             be at least  max( 1, k ).
             Unchanged on exit.
    BETA   - double.
             On entry, BETA specifies the scalar beta.
             Unchanged on exit.
    C      - double array of DIMENSION ( LDC, n ).
             Before entry  with  UPLO = 'U' or 'u',  the leading  n by n
             upper triangular part of the array C must contain the upper
             triangular part  of the  symmetric matrix  and the strictly
             lower triangular part of C is not referenced.  On exit, the
             upper triangular part of the array  C is overwritten by the
             upper triangular part of the updated matrix.
             Before entry  with  UPLO = 'L' or 'l',  the leading  n by n
             lower triangular part of the array C must contain the lower
             triangular part  of the  symmetric matrix  and the strictly
             upper triangular part of C is not referenced.  On exit, the
             lower triangular part of the array  C is overwritten by the
             lower triangular part of the updated matrix.
    LDC    - long.
             On entry, LDC specifies the first dimension of C as declared
             in  the  calling  (sub)  program.   LDC  must  be  at  least
             max( 1, n ).
             Unchanged on exit.
    Level 3 Blas routine.
*/

int NUMblas_dtrmm (const char *side, const char *uplo, const char *transa, const char *diag,
	integer *m, integer *n, double *alpha, double *a, integer *lda,
	double *b, integer *ldb);
/*  Purpose
    =======
    NUMblas_dtrmm  performs one of the matrix-matrix operations
       B := alpha*op( A )*B,   or   B := alpha*B*op( A ),
    where  alpha  is a scalar,  B  is an m by n matrix,  A  is a unit, or
    non-unit,  upper or lower triangular matrix  and  op( A )  is one  of
       op( A ) = A   or   op( A ) = A'.
    Parameters
    ==========
    SIDE   - char*.
             On entry,  SIDE specifies whether  op( A ) multiplies B from
             the left or right as follows:
                SIDE = 'L' or 'l'   B := alpha*op( A )*B.
                SIDE = 'R' or 'r'   B := alpha*B*op( A ).
             Unchanged on exit.
    UPLO   - char*.
             On entry, UPLO specifies whether the matrix A is an upper or
             lower triangular matrix as follows:
                UPLO = 'U' or 'u'   A is an upper triangular matrix.
                UPLO = 'L' or 'l'   A is a lower triangular matrix.
             Unchanged on exit.
    TRANSA - char*.
             On entry, TRANSA specifies the form of op( A ) to be used in
             the matrix multiplication as follows:
                TRANSA = 'N' or 'n'   op( A ) = A.
                TRANSA = 'T' or 't'   op( A ) = A'.
                TRANSA = 'C' or 'c'   op( A ) = A'.
             Unchanged on exit.
    DIAG   - char*.
             On entry, DIAG specifies whether or not A is unit triangular
             as follows:
                DIAG = 'U' or 'u'   A is assumed to be unit triangular.
                DIAG = 'N' or 'n'   A is not assumed to be unit
                                    triangular.
             Unchanged on exit.
    M      - long.
             On entry, M specifies the number of rows of B. M must be at
             least zero.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the number of columns of B.  N must be
             at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry,  ALPHA specifies the scalar  alpha. When  alpha is
             zero then  A is not referenced and  B need not be set before
             entry.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, k ), where k is m
             when  SIDE = 'L' or 'l'  and is  n  when  SIDE = 'R' or 'r'.
             Before entry  with  UPLO = 'U' or 'u',  the  leading  k by k
             upper triangular part of the array  A must contain the upper
             triangular matrix  and the strictly lower triangular part of
             A is not referenced.
             Before entry  with  UPLO = 'L' or 'l',  the  leading  k by k
             lower triangular part of the array  A must contain the lower
             triangular matrix  and the strictly upper triangular part of
             A is not referenced.
             Note that when  DIAG = 'U' or 'u',  the diagonal elements of
             A  are not referenced either,  but are assumed to be  unity.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program.  When  SIDE = 'L' or 'l'  then
             LDA  must be at least  max( 1, m ),  when  SIDE = 'R' or 'r'
             then LDA must be at least max( 1, n ).
             Unchanged on exit.
    B      - double array of DIMENSION ( LDB, n ).
             Before entry,  the leading  m by n part of the array  B must
             contain the matrix  B,  and  on exit  is overwritten  by the
             transformed matrix.
    LDB    - long.
             On entry, LDB specifies the first dimension of B as declared
             in  the  calling  (sub)  program.   LDB  must  be  at  least
             max( 1, m ).
             Unchanged on exit.
    Level 3 Blas routine.
*/

int NUMblas_dtrmv (const char *uplo, const char *trans, const char *diag, integer *n,
	double *a, integer *lda, double *x, integer *incx);
/*  Purpose
    =======
    NUMblas_dtrmv  performs one of the matrix-vector operations
       x := A*x,   or   x := A'*x,
    where x is an n element vector and  A is an n by n unit, or non-unit,
    upper or lower triangular matrix.
    Parameters
    ==========
    UPLO   - char*.
             On entry, UPLO specifies whether the matrix is an upper or
             lower triangular matrix as follows:
                UPLO = 'U' or 'u'   A is an upper triangular matrix.
                UPLO = 'L' or 'l'   A is a lower triangular matrix.
             Unchanged on exit.
    TRANS  - char*.
             On entry, TRANS specifies the operation to be performed as
             follows:
                TRANS = 'N' or 'n'   x := A*x.
                TRANS = 'T' or 't'   x := A'*x.
                TRANS = 'C' or 'c'   x := A'*x.
             Unchanged on exit.
    DIAG   - char*.
             On entry, DIAG specifies whether or not A is unit
             triangular as follows:
                DIAG = 'U' or 'u'   A is assumed to be unit triangular.
                DIAG = 'N' or 'n'   A is not assumed to be unit
                                    triangular.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the order of the matrix A.
             N must be at least zero.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, n ).
             Before entry with  UPLO = 'U' or 'u', the leading n by n
             upper triangular part of the array A must contain the upper
             triangular matrix and the strictly lower triangular part of
             A is not referenced.
             Before entry with UPLO = 'L' or 'l', the leading n by n
             lower triangular part of the array A must contain the lower
             triangular matrix and the strictly upper triangular part of
             A is not referenced.
             Note that when  DIAG = 'U' or 'u', the diagonal elements of
             A are not referenced either, but are assumed to be unity.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program. LDA must be at least
             max( 1, n ).
             Unchanged on exit.
    X      - double array of dimension at least
             ( 1 + ( n - 1 )*abs( INCX ) ).
             Before entry, the incremented array X must contain the n
             element vector x. On exit, X is overwritten with the
             tranformed vector x.
    INCX   - long.
             On entry, INCX specifies the increment for the elements of
             X. INCX must not be zero.
             Unchanged on exit.
    Level 2 Blas routine.
*/

int NUMblas_dtrsm (const char *side, const char *uplo, const char *transa, const char *diag, integer *m, integer *n,
	double *alpha, double *a, integer *lda, double *b, integer *ldb);
/*  Purpose
    =======
    NUMblas_dtrsm  solves one of the matrix equations
       op( A )*X = alpha*B,   or   X*op( A ) = alpha*B,
    where alpha is a scalar, X and B are m by n matrices, A is a unit, or
    non-unit,  upper or lower triangular matrix  and  op( A )  is one  of
       op( A ) = A   or   op( A ) = A'.
    The matrix X is overwritten on B.
    Parameters
    ==========
    SIDE   - char*.
             On entry, SIDE specifies whether op( A ) appears on the left
             or right of X as follows:
                SIDE = 'L' or 'l'   op( A )*X = alpha*B.
                SIDE = 'R' or 'r'   X*op( A ) = alpha*B.
             Unchanged on exit.
    UPLO   - char*.
             On entry, UPLO specifies whether the matrix A is an upper or
             lower triangular matrix as follows:
                UPLO = 'U' or 'u'   A is an upper triangular matrix.
                UPLO = 'L' or 'l'   A is a lower triangular matrix.
             Unchanged on exit.
    TRANSA - char*.
             On entry, TRANSA specifies the form of op( A ) to be used in
             the matrix multiplication as follows:
                TRANSA = 'N' or 'n'   op( A ) = A.
                TRANSA = 'T' or 't'   op( A ) = A'.
                TRANSA = 'C' or 'c'   op( A ) = A'.
             Unchanged on exit.
    DIAG   - char*.
             On entry, DIAG specifies whether or not A is unit triangular
             as follows:
                DIAG = 'U' or 'u'   A is assumed to be unit triangular.
                DIAG = 'N' or 'n'   A is not assumed to be unit
                                    triangular.
             Unchanged on exit.
    M      - long.
             On entry, M specifies the number of rows of B. M must be at
             least zero.
             Unchanged on exit.
    N      - long.
             On entry, N specifies the number of columns of B.  N must be
             at least zero.
             Unchanged on exit.
    ALPHA  - double.
             On entry,  ALPHA specifies the scalar  alpha. When  alpha is
             zero then  A is not referenced and  B need not be set before
             entry.
             Unchanged on exit.
    A      - double array of DIMENSION ( LDA, k ), where k is m
             when  SIDE = 'L' or 'l'  and is  n  when  SIDE = 'R' or 'r'.
             Before entry  with  UPLO = 'U' or 'u',  the  leading  k by k
             upper triangular part of the array  A must contain the upper
             triangular matrix  and the strictly lower triangular part of
             A is not referenced.
             Before entry  with  UPLO = 'L' or 'l',  the  leading  k by k
             lower triangular part of the array  A must contain the lower
             triangular matrix  and the strictly upper triangular part of
             A is not referenced.
             Note that when  DIAG = 'U' or 'u',  the diagonal elements of
             A  are not referenced either,  but are assumed to be  unity.
             Unchanged on exit.
    LDA    - long.
             On entry, LDA specifies the first dimension of A as declared
             in the calling (sub) program.  When  SIDE = 'L' or 'l'  then
             LDA  must be at least  max( 1, m ),  when  SIDE = 'R' or 'r'
             then LDA must be at least max( 1, n ).
             Unchanged on exit.
    B      - double array of DIMENSION ( LDB, n ).
             Before entry,  the leading  m by n part of the array  B must
             contain  the  right-hand  side  matrix  B,  and  on exit  is
             overwritten by the solution matrix  X.
    LDB    - long.
             On entry, LDB specifies the first dimension of B as declared
             in  the  calling  (sub)  program.   LDB  must  be  at  least
             max( 1, m ).
             Unchanged on exit.
*/

integer NUMblas_idamax (integer *n, double *dx, integer *incx);
/* finds the index of element having max. absolute value.*/

#endif /* _NUMcblas_h_ */
