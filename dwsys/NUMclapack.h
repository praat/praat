#ifndef _NUMclapack_h_
#define _NUMclapack_h_
 /* NUMclapack.h
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020923 GPL header
 djmw 20030310 Latest modification
*/

/*
	The following routines all use FORTRAN column-major storage of
	matrices. A consequence is that all matrices must have been allocated
	as a single block of mxn elements.
	All matrices are passed as a vector of mxn elements.
		matrix[i][j] => vector[(j-1)*m + i] "Fortran"
		matrix[i][j] => vector[(i-1)*n + j] "C"
	The consequence is  that you have to transpose C matrices before you pass them
	to a CLAPACK routine.
	Sometimes you can avoid transposition by considering the solution
	of the transposed problem (e.g. See code in SVD_compute).

*/

#ifdef __cplusplus
	extern "C" {
#endif

int NUMlapack_dbdsqr(const char *uplo, long *n, long *ncvt, long *nru, long *ncc,
	double *d, double *e, double *vt, long *ldvt, double *u, long *ldu,
	double *c, long *ldc, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dbdsqr computes the singular value decomposition (SVD) of a real
    N-by-N (upper or lower) bidiagonal matrix B:  B = Q * S * P' (P'
    denotes the transpose of P), where S is a diagonal matrix with
    non-negative diagonal elements (the singular values of B), and Q
    and P are orthogonal matrices.

    The routine computes S, and optionally computes U * Q, P' * VT,
    or Q' * C, for given real input matrices U, VT, and C.

    See "Computing  Small Singular Values of Bidiagonal Matrices With
    Guaranteed High Relative Accuracy," by J. Demmel and W. Kahan,
    LAPACK Working Note #3 (or SIAM J. Sci. Statist. Comput. vol. 11,
    no. 5, pp. 873-912, Sept 1990) and
    "Accurate singular values and differential qd algorithms," by
    B. Parlett and V. Fernando, Technical Report CPAM-554, Mathematics
    Department, University of California at Berkeley, July 1992
    for a detailed description of the algorithm.

    Arguments
    =========

    UPLO    (input) char*
            = 'U':  B is upper bidiagonal;
            = 'L':  B is lower bidiagonal.

    N       (input) long
            The order of the matrix B.  N >= 0.

    NCVT    (input) long
            The number of columns of the matrix VT. NCVT >= 0.

    NRU     (input) long
            The number of rows of the matrix U. NRU >= 0.

    NCC     (input) long
            The number of columns of the matrix C. NCC >= 0.

    D       (input/output) double array, dimension (N)
            On entry, the n diagonal elements of the bidiagonal matrix B.
            On exit, if INFO=0, the singular values of B in decreasing
            order.

    E       (input/output) double array, dimension (N)
            On entry, the elements of E contain the
            offdiagonal elements of the bidiagonal matrix whose SVD
            is desired. On normal exit (INFO = 0), E is destroyed.
            If the algorithm does not converge (INFO > 0), D and E
            will contain the diagonal and superdiagonal elements of a
            bidiagonal matrix orthogonally equivalent to the one given
            as input. E(N) is used for workspace.

    VT      (input/output) double array, dimension (LDVT, NCVT)
            On entry, an N-by-NCVT matrix VT.
            On exit, VT is overwritten by P' * VT.
            VT is not referenced if NCVT = 0.

    LDVT    (input) long
            The leading dimension of the array VT.
            LDVT >= max(1,N) if NCVT > 0; LDVT >= 1 if NCVT = 0.

    U       (input/output) double array, dimension (LDU, N)
            On entry, an NRU-by-N matrix U.
            On exit, U is overwritten by U * Q.
            U is not referenced if NRU = 0.

    LDU     (input) long
            The leading dimension of the array U.  LDU >= max(1,NRU).

    C       (input/output) double array, dimension (LDC, NCC)
            On entry, an N-by-NCC matrix C.
            On exit, C is overwritten by Q' * C.
            C is not referenced if NCC = 0.

    LDC     (input) long
            The leading dimension of the array C.
            LDC >= max(1,N) if NCC > 0; LDC >=1 if NCC = 0.

    WORK    (workspace) double array, dimension (4*N)

    INFO    (output) long
            = 0:  successful exit
            < 0:  If INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm did not converge; D and E contain the
                  elements of a bidiagonal matrix which is orthogonally
                  similar to the input matrix B;  if INFO = i, i
                  elements of E have not converged to zero.

    Internal Parameters
    ===================

    TOLMUL  double, default = max(10,min(100,EPS**(-1/8)))
            TOLMUL controls the convergence criterion of the QR loop.
            If it is positive, TOLMUL*EPS is the desired relative
               precision in the computed singular values.
            If it is negative, abs(TOLMUL*EPS*sigma_max) is the
               desired absolute accuracy in the computed singular
               values (corresponds to relative accuracy
               abs(TOLMUL*EPS) in the largest singular value.
            abs(TOLMUL) should be between 1 and 1/EPS, and preferably
               between 10 (for fast convergence) and .1/EPS
               (for there to be some accuracy in the results).
            Default is to lose at either one eighth or 2 of the
               available decimal digits in each computed singular value
               (whichever is smaller).

    MAXITR  long, default = 6
            MAXITR controls the maximum number of passes of the
            algorithm through its inner loop. The algorithms stops
            (and so fails to converge) if the number of passes
            through the inner loop exceeds MAXITR*N**2.

    =====================================================================
*/

int NUMlapack_dgebd2(long *m, long *n, double *a, long *lda, double *d, double *e,
	double *tauq, double *taup, double *work, long *info);
/*
    Purpose
    =======

    NUMlapack_dgebd2 reduces a real general m by n matrix A to upper or lower
    bidiagonal form B by an orthogonal transformation: Q' * A * P = B.

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.

    Arguments
    =========

    M       (input) long
            The number of rows in the matrix A.  M >= 0.

    N       (input) long
            The number of columns in the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n general matrix to be reduced.
            On exit,
            if m >= n, the diagonal and the first superdiagonal are
              overwritten with the upper bidiagonal matrix B; the
              elements below the diagonal, with the array TAUQ, represent
              the orthogonal matrix Q as a product of elementary
              reflectors, and the elements above the first superdiagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors;
            if m < n, the diagonal and the first subdiagonal are
              overwritten with the lower bidiagonal matrix B; the
              elements below the first subdiagonal, with the array TAUQ,
              represent the orthogonal matrix Q as a product of
              elementary reflectors, and the elements above the diagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) double array, dimension (min(M,N))
            The diagonal elements of the bidiagonal matrix B:
            D(i) = A(i,i).

    E       (output) double array, dimension (min(M,N)-1)
            The off-diagonal elements of the bidiagonal matrix B:
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.

    TAUQ    (output) double array dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    WORK    (workspace) double array, dimension (max(M,N))

    INFO    (output) long
            = 0: successful exit.
            < 0: if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

    If m >= n,

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n,

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    The contents of A on exit are illustrated by the following examples:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )
      (  v1  v2  v3  v4  v5 )

    where d and e denote diagonal and off-diagonal elements of B, vi
    denotes an element of the vector defining H(i), and ui an element of
    the vector defining G(i).

    =====================================================================
*/



int NUMlapack_dgebrd(long *m, long *n, double *a, long *lda, double *d, double *e,
	double *tauq, double *taup, double *work, long *lwork, long *info);
/*
    Purpose
    =======

    NUMlapack_dgebrd reduces a general real M-by-N matrix A to upper or lower
    bidiagonal form B by an orthogonal transformation: Q**T * A * P = B.

    If m >= n, B is upper bidiagonal; if m < n, B is lower bidiagonal.

    Arguments
    =========

    M       (input) long
            The number of rows in the matrix A.  M >= 0.

    N       (input) long
            The number of columns in the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N general matrix to be reduced.
            On exit,
            if m >= n, the diagonal and the first superdiagonal are
              overwritten with the upper bidiagonal matrix B; the
              elements below the diagonal, with the array TAUQ, represent
              the orthogonal matrix Q as a product of elementary
              reflectors, and the elements above the first superdiagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors;
            if m < n, the diagonal and the first subdiagonal are
              overwritten with the lower bidiagonal matrix B; the
              elements below the first subdiagonal, with the array TAUQ,
              represent the orthogonal matrix Q as a product of
              elementary reflectors, and the elements above the diagonal,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) double array, dimension (min(M,N))
            The diagonal elements of the bidiagonal matrix B:
            D(i) = A(i,i).

    E       (output) double array, dimension (min(M,N)-1)
            The off-diagonal elements of the bidiagonal matrix B:
            if m >= n, E(i) = A(i,i+1) for i = 1,2,...,n-1;
            if m < n, E(i) = A(i+1,i) for i = 1,2,...,m-1.

    TAUQ    (output) double array dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The length of the array WORK.  LWORK >= max(1,M,N).
            For optimum performance LWORK >= (M+N)*NB, where NB
            is the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

    If m >= n,

       Q = H(1) H(2) . . . H(n)  and  P = G(1) G(2) . . . G(n-1)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i-1) = 0, v(i) = 1, and v(i+1:m) is stored on exit in A(i+1:m,i);
    u(1:i) = 0, u(i+1) = 1, and u(i+2:n) is stored on exit in A(i,i+2:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n,

       Q = H(1) H(2) . . . H(m-1)  and  P = G(1) G(2) . . . G(m)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors;
    v(1:i) = 0, v(i+1) = 1, and v(i+2:m) is stored on exit in A(i+2:m,i);
    u(1:i-1) = 0, u(i) = 1, and u(i+1:n) is stored on exit in A(i,i+1:n);
    tauq is stored in TAUQ(i) and taup in TAUP(i).

    The contents of A on exit are illustrated by the following examples:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  d   e   u1  u1  u1 )           (  d   u1  u1  u1  u1  u1 )
      (  v1  d   e   u2  u2 )           (  e   d   u2  u2  u2  u2 )
      (  v1  v2  d   e   u3 )           (  v1  e   d   u3  u3  u3 )
      (  v1  v2  v3  d   e  )           (  v1  v2  e   d   u4  u4 )
      (  v1  v2  v3  v4  d  )           (  v1  v2  v3  e   d   u5 )
      (  v1  v2  v3  v4  v5 )

    where d and e denote diagonal and off-diagonal elements of B, vi
    denotes an element of the vector defining H(i), and ui an element of
    the vector defining G(i).

    =====================================================================
*/

int NUMlapack_dgebak (const char *job, const char *side, long *n, long *ilo, long *ihi,
	double *scale, long *m,	double *v, long *ldv, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       September 30, 1994


    Purpose
    =======

    NUMlapack_dgebak forms the right or left eigenvectors of a real general matrix
    by backward transformation on the computed eigenvectors of the
    balanced matrix output by NUMlapack_dgebal.

    Arguments
    =========

    JOB     (input) char*
            Specifies the type of backward transformation required:
            = 'N', do nothing, return immediately;
            = 'P', do backward transformation for permutation only;
            = 'S', do backward transformation for scaling only;
            = 'B', do backward transformations for both permutation and
                   scaling.
            JOB must be the same as the argument JOB supplied to NUMlapack_dgebal.

    SIDE    (input) char*
            = 'R':  V contains right eigenvectors;
            = 'L':  V contains left eigenvectors.

    N       (input) long
            The number of rows of the matrix V.  N >= 0.

    ILO     (input) long
    IHI     (input) long
            The integers ILO and IHI determined by NUMlapack_dgebal.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    SCALE   (input) double array, dimension (N)
            Details of the permutation and scaling factors, as returned
            by NUMlapack_dgebal.

    M       (input) long
            The number of columns of the matrix V.  M >= 0.

    V       (input/output) double array, dimension (LDV,M)
            On entry, the matrix of right or left eigenvectors to be
            transformed, as returned by DHSEIN or NUMlapack_dtrevc.
            On exit, V is overwritten by the transformed eigenvectors.

    LDV     (input) long
            The leading dimension of the array V. LDV >= max(1,N).

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    =====================================================================
*/

int NUMlapack_dgebal (const char *job, long *n, double *a, long *lda, long *ilo,
	long *ihi, double *scale, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dgebal balances a general real matrix A.  This involves, first,
    permuting A by a similarity transformation to isolate eigenvalues
    in the first 1 to ILO-1 and last IHI+1 to N elements on the
    diagonal; and second, applying a diagonal similarity transformation
    to rows and columns ILO to IHI to make the rows and columns as
    close in norm as possible.  Both steps are optional.

    Balancing may reduce the 1-norm of the matrix, and improve the
    accuracy of the computed eigenvalues and/or eigenvectors.

    Arguments
    =========

    JOB     (input) char*
            Specifies the operations to be performed on A:
            = 'N':  none:  simply set ILO = 1, IHI = N, SCALE(I) = 1.0
                    for i = 1,...,N;
            = 'P':  permute only;
            = 'S':  scale only;
            = 'B':  both permute and scale.

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the input matrix A.
            On exit,  A is overwritten by the balanced matrix.
            If JOB = 'N', A is not referenced.
            See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    ILO     (output) long
    IHI     (output) long
            ILO and IHI are set to integers such that on exit
            A(i,j) = 0 if i > j and j = 1,...,ILO-1 or I = IHI+1,...,N.
            If JOB = 'N' or 'S', ILO = 1 and IHI = N.

    SCALE   (output) double array, dimension (N)
            Details of the permutations and scaling factors applied to
            A.  If P(j) is the index of the row and column interchanged
            with row and column j and D(j) is the scaling factor
            applied to row and column j, then
            SCALE(j) = P(j)    for j = 1,...,ILO-1
                     = D(j)    for j = ILO,...,IHI
                     = P(j)    for j = IHI+1,...,N.
            The order in which the interchanges are made is N to IHI+1,
            then 1 to ILO-1.

    INFO    (output) long
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The permutations consist of row and column interchanges which put
    the matrix in the form

               ( T1   X   Y  )
       P A P = (  0   B   Z  )
               (  0   0   T2 )

    where T1 and T2 are upper triangular matrices whose eigenvalues lie
    along the diagonal.  The column indices ILO and IHI mark the starting
    and ending columns of the submatrix B. Balancing consists of applying
    a diagonal similarity transformation inv(D) * B * D to make the
    1-norms of each row of B and its corresponding column nearly equal.
    The output matrix is

       ( T1     X*D          Y    )
       (  0  inv(D)*B*D  inv(D)*Z ).
       (  0      0           T2   )

    Information about the permutations P and the diagonal matrix D is
    returned in the vector SCALE.

    This subroutine is based on the EISPACK routine BALANC.

    Modified by Tzu-Yi Chen, Computer Science Division, University of
      California at Berkeley, USA

    =====================================================================
*/

int NUMlapack_dgeev (const char *jobvl, const char *jobvr, long *n, double *a, long *lda,
	double *wr, double *wi,	double *vl, long *ldvl, double *vr, long *ldvr,
	double *work, long *lwork, long *info);
/*  -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       December 8, 1999

    Purpose
    =======

    NUMlapack_dgeev computes for an N-by-N real nonsymmetric matrix A, the
    eigenvalues and, optionally, the left and/or right eigenvectors.

    The right eigenvector v(j) of A satisfies
                     A * v(j) = lambda(j) * v(j)
    where lambda(j) is its eigenvalue.
    The left eigenvector u(j) of A satisfies
                  u(j)**H * A = lambda(j) * u(j)**H
    where u(j)**H denotes the conjugate transpose of u(j).

    The computed eigenvectors are normalized to have Euclidean norm
    equal to 1 and largest component real.

    Arguments
    =========

    JOBVL   (input) char*
            = 'N': left eigenvectors of A are not computed;
            = 'V': left eigenvectors of A are computed.

    JOBVR   (input) char*
            = 'N': right eigenvectors of A are not computed;
            = 'V': right eigenvectors of A are computed.

    N       (input) long
            The order of the matrix A. N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the N-by-N matrix A.
            On exit, A has been overwritten.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    WR      (output) double array, dimension (N)
    WI      (output) double array, dimension (N)
            WR and WI contain the real and imaginary parts,
            respectively, of the computed eigenvalues.  Complex
            conjugate pairs of eigenvalues appear consecutively
            with the eigenvalue having the positive imaginary part
            first.

    VL      (output) double array, dimension (LDVL,N)
            If JOBVL = 'V', the left eigenvectors u(j) are stored one
            after another in the columns of VL, in the same order
            as their eigenvalues.
            If JOBVL = 'N', VL is not referenced.
            If the j-th eigenvalue is real, then u(j) = VL(:,j),
            the j-th column of VL.
            If the j-th and (j+1)-st eigenvalues form a complex
            conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and
            u(j+1) = VL(:,j) - i*VL(:,j+1).

    LDVL    (input) long
            The leading dimension of the array VL.  LDVL >= 1; if
            JOBVL = 'V', LDVL >= N.

    VR      (output) double array, dimension (LDVR,N)
            If JOBVR = 'V', the right eigenvectors v(j) are stored one
            after another in the columns of VR, in the same order
            as their eigenvalues.
            If JOBVR = 'N', VR is not referenced.
            If the j-th eigenvalue is real, then v(j) = VR(:,j),
            the j-th column of VR.
            If the j-th and (j+1)-st eigenvalues form a complex
            conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and
            v(j+1) = VR(:,j) - i*VR(:,j+1).

    LDVR    (input) long
            The leading dimension of the array VR.  LDVR >= 1; if
            JOBVR = 'V', LDVR >= N.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= max(1,3*N), and
            if JOBVL = 'V' or JOBVR = 'V', LWORK >= 4*N.  For good
            performance, LWORK must generally be larger.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = i, the QR algorithm failed to compute all the
                  eigenvalues, and no eigenvectors have been computed;
                  elements i+1:N of WR and WI contain eigenvalues which
                  have converged.

    =====================================================================
*/

int NUMlapack_dgehd2 (long *n, long *ilo, long *ihi, double *a, long *lda,
	double *tau, double *work, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    NUMlapack_dgehd2 reduces a real general matrix A to upper Hessenberg form H by
    an orthogonal similarity transformation:  Q' * A * Q = H .

    Arguments
    =========

    N       (input) long
            The order of the matrix A.  N >= 0.

    ILO     (input) long
    IHI     (input) long
            It is assumed that A is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to NUMlapack_dgebal; otherwise they should be
            set to 1 and N respectively. See Further Details.
            1 <= ILO <= IHI <= max(1,N).

    A       (input/output) double array, dimension (LDA,N)
            On entry, the n by n general matrix to be reduced.
            On exit, the upper triangle and the first subdiagonal of A
            are overwritten with the upper Hessenberg matrix H, and the
            elements below the first subdiagonal, with the array TAU,
            represent the orthogonal matrix Q as a product of elementary
            reflectors. See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) double array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) double array, dimension (N)

    INFO    (output) long
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrix Q is represented as a product of (ihi-ilo) elementary
    reflectors

       Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on
    exit in A(i+2:ihi,i), and tau in TAU(i).

    The contents of A are illustrated by the following example, with
    n = 7, ilo = 2 and ihi = 6:

    on entry,                        on exit,

    ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      h   h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h )
    (                         a )    (                          a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================
*/

int NUMlapack_dgehrd (long *n, long *ilo, long *ihi, double *a, long *lda,
	double *tau, double *work, long *lwork, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dgehrd reduces a real general matrix A to upper Hessenberg form H by
    an orthogonal similarity transformation:  Q' * A * Q = H .

    Arguments
    =========

    N       (input) long
            The order of the matrix A.  N >= 0.

    ILO     (input) long
    IHI     (input) long
            It is assumed that A is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to NUMlapack_dgebal; otherwise they should be
            set to 1 and N respectively. See Further Details.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the N-by-N general matrix to be reduced.
            On exit, the upper triangle and the first subdiagonal of A
            are overwritten with the upper Hessenberg matrix H, and the
            elements below the first subdiagonal, with the array TAU,
            represent the orthogonal matrix Q as a product of elementary
            reflectors. See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) double array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details). Elements 1:ILO-1 and IHI:N-1 of TAU are set to
            zero.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The length of the array WORK.  LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    The matrix Q is represented as a product of (ihi-ilo) elementary
    reflectors

       Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on
    exit in A(i+2:ihi,i), and tau in TAU(i).

    The contents of A are illustrated by the following example, with
    n = 7, ilo = 2 and ihi = 6:

    on entry,                        on exit,

    ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      a   h   h   h   h   a )
    (     a   a   a   a   a   a )    (      h   h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  h   h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h )
    (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h )
    (                         a )    (                          a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================
*/

int NUMlapack_dgelq2 (long *m, long *n, double *a, long *lda, double *tau, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dgelq2 computes an LQ factorization of a real m by n matrix A:
    A = L * Q.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n matrix A.
            On exit, the elements on and below the diagonal of the array
            contain the m by min(m,n) lower trapezoidal matrix L (L is
            lower triangular if m <= n); the elements above the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) double array, dimension (M)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(k) . . . H(2) H(1), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),
    and tau in TAU(i).

    =====================================================================
*/

int NUMlapack_dgelqf (long *m, long *n, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dgelqf computes an LQ factorization of a real M-by-N matrix A:
    A = L * Q.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the elements on and below the diagonal of the array
            contain the m-by-min(m,n) lower trapezoidal matrix L (L is
            lower triangular if m <= n); the elements above the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= max(1,M).
            For optimum performance LWORK >= M*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(k) . . . H(2) H(1), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:n) is stored on exit in A(i,i+1:n),
    and tau in TAU(i).

    =====================================================================
*/


int NUMlapack_dgelss (long *m, long *n, long *nrhs, double *a, long *lda,
	double *b, long *ldb, double *s, double *rcond, long *rank, double *work,
	long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dgelss computes the minimum norm solution to a real linear least
    squares problem:

    Minimize 2-norm(| b - A*x |).

    using the singular value decomposition (SVD) of A. A is an M-by-N
    matrix which may be rank-deficient.

    Several right hand side vectors b and solution vectors x can be
    handled in a single call; they are stored as the columns of the
    M-by-NRHS right hand side matrix B and the N-by-NRHS solution matrix
    X.

    The effective rank of A is determined by treating as zero those
    singular values which are less than RCOND times the largest singular
    value.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A. M >= 0.

    N       (input) long
            The number of columns of the matrix A. N >= 0.

    NRHS    (input) long
            The number of right hand sides, i.e., the number of columns
            of the matrices B and X. NRHS >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the first min(m,n) rows of A are overwritten with
            its right singular vectors, stored rowwise.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    B       (input/output) double array, dimension (LDB,NRHS)
            On entry, the M-by-NRHS right hand side matrix B.
            On exit, B is overwritten by the N-by-NRHS solution
            matrix X.  If m >= n and RANK = n, the residual
            sum-of-squares for the solution in the i-th column is given
            by the sum of squares of elements n+1:m in that column.

    LDB     (input) long
            The leading dimension of the array B. LDB >= max(1,max(M,N)).

    S       (output) double array, dimension (min(M,N))
            The singular values of A in decreasing order.
            The condition number of A in the 2-norm = S(1)/S(min(m,n)).

    RCOND   (input) double
            RCOND is used to determine the effective rank of A.
            Singular values S(i) <= RCOND*S(1) are treated as zero.
            If RCOND < 0, machine precision is used instead.

    RANK    (output) long
            The effective rank of A, i.e., the number of singular values
            which are greater than RCOND*S(1).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= 1, and also:
            LWORK >= 3*min(M,N) + max( 2*min(M,N), max(M,N), NRHS )
            For good performance, LWORK should generally be larger.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  the algorithm for computing the SVD failed to converge;
                  if INFO = i, i off-diagonal elements of an intermediate
                  bidiagonal form did not converge to zero.

    =====================================================================
*/


int NUMlapack_dgeqpf (long *m, long *n, double *a, long *lda, long *jpvt,
	double *tau, double *work, long *info);
/*  Purpose
    =======

    This routine is deprecated and has been replaced by routine DGEQP3.

    NUMlapack_dgeqpf computes a QR factorization with column pivoting of a
    real M-by-N matrix A: A*P = Q*R.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A. M >= 0.

    N       (input) long
            The number of columns of the matrix A. N >= 0

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the upper triangle of the array contains the
            min(M,N)-by-N upper triangular matrix R; the elements
            below the diagonal, together with the array TAU,
            represent the orthogonal matrix Q as a product of
            min(m,n) elementary reflectors.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,M).

    JPVT    (input/output) long array, dimension (N)
            On entry, if JPVT(i) .ne. 0, the i-th column of A is permuted
            to the front of A*P (a leading column); if JPVT(i) = 0,
            the i-th column of A is a free column.
            On exit, if JPVT(i) = k, then the i-th column of A*P
            was the k-th column of A.

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors.

    WORK    (workspace) double array, dimension (3*N)

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(n)

    Each H(i) has the form

       H = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i).

    The matrix P is represented in jpvt as follows: If
       jpvt(j) = i
    then the jth column of P is the ith canonical unit vector.

    =====================================================================
*/

int NUMlapack_dgeqr2 (long *m, long *n, double *a, long *lda, double *tau,
	double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dgeqr2 computes a QR factorization of a real m by n matrix A:
    A = Q * R.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n matrix A.
            On exit, the elements on and above the diagonal of the array
            contain the min(m,n) by n upper trapezoidal matrix R (R is
            upper triangular if m >= n); the elements below the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of elementary reflectors (see Further Details).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) double array, dimension (N)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),
    and tau in TAU(i).

    =====================================================================
*/

int NUMlapack_dgeqrf(long *m, long *n, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dgeqrf computes a QR factorization of a real M-by-N matrix A:
    A = Q * R.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, the elements on and above the diagonal of the array
            contain the min(M,N)-by-N upper trapezoidal matrix R (R is
            upper triangular if m >= n); the elements below the diagonal,
            with the array TAU, represent the orthogonal matrix Q as a
            product of min(m,n) elementary reflectors (see Further
            Details).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i-1) = 0 and v(i) = 1; v(i+1:m) is stored on exit in A(i+1:m,i),
	and tau in TAU(i).

    =====================================================================
*/

int NUMlapack_dgerq2(long *m, long *n, double *a, long *lda, double *tau,
	double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dgerq2 computes an RQ factorization of a real m by n matrix A:
    A = R * Q.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n matrix A.
            On exit, if m <= n, the upper triangle of the subarray
            A(1:m,n-m+1:n) contains the m by m upper triangular matrix R;
            if m >= n, the elements on and above the (m-n)-th subdiagonal
            contain the m by n upper trapezoidal matrix R; the remaining
            elements, with the array TAU, represent the orthogonal matrix
            Q as a product of elementary reflectors (see Further
            Details).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    TAU     (output) double array, dimension (min(M,N))
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace) double array, dimension (M)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The matrix Q is represented as a product of elementary reflectors

       Q = H(1) H(2) . . . H(k), where k = min(m,n).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(n-k+i+1:n) = 0 and v(n-k+i) = 1; v(1:n-k+i-1) is stored on exit in
    A(m-k+i,1:n-k+i-1), and tau in TAU(i).

    =====================================================================
*/

int NUMlapack_dgesv (long *n, long *nrhs, double *a, long *lda, long *ipiv,
	double *b, long *ldb, long *info);
/* 	Purpose
    =======

    NUMlapack_dgesv computes the solution to a real system of linear equations
       A * X = B,
    where A is an N-by-N matrix and X and B are N-by-NRHS matrices.

    The LU decomposition with partial pivoting and row interchanges is
    used to factor A as
       A = P * L * U,
    where P is a permutation matrix, L is unit lower triangular, and U is
    upper triangular.  The factored form of A is then used to solve the
    system of equations A * X = B.

    Arguments
    =========

    N       (input) long
            The number of linear equations, i.e., the order of the
            matrix A.  N >= 0.

    NRHS    (input) long
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the N-by-N coefficient matrix A.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (output) long array, dimension (N)
            The pivot indices that define the permutation matrix P;
            row i of the matrix was interchanged with row IPIV(i).

    B       (input/output) double array, dimension (LDB,NRHS)
            On entry, the N-by-NRHS matrix of right hand side matrix B.
            On exit, if INFO = 0, the N-by-NRHS solution matrix X.

    LDB     (input) long
            The leading dimension of the array B.  LDB >= max(1,N).

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, U(i,i) is exactly zero.  The factorization
                  has been completed, but the factor U is exactly
                  singular, so the solution could not be computed.

	=====================================================================
*/

int NUMlapack_dgesvd (const char *jobu, const char *jobvt, long *m, long *n, double *a, long *lda,
	double *s, double *u, long *ldu, double *vt, long *ldvt, double *work,
	long *lwork, long *info);
/*
    Purpose
    =======

    NUMlapack_dgesvd computes the singular value decomposition (SVD) of a real
    M-by-N matrix A, optionally computing the left and/or right singular
    vectors. The SVD is written

         A = U * SIGMA * transpose(V)

    where SIGMA is an M-by-N matrix which is zero except for its
    min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and
    V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA
    are the singular values of A; they are real and non-negative, and
    are returned in descending order.  The first min(m,n) columns of
    U and V are the left and right singular vectors of A.

    Note that the routine returns V**T, not V.

    Arguments
    =========

    JOBU    (input) char*
            Specifies options for computing all or part of the matrix U:
            = 'A':  all M columns of U are returned in array U:
            = 'S':  the first min(m,n) columns of U (the left singular
                    vectors) are returned in the array U;
            = 'O':  the first min(m,n) columns of U (the left singular
                    vectors) are overwritten on the array A;
            = 'N':  no columns of U (no left singular vectors) are
                    computed.

    JOBVT   (input) char*
            Specifies options for computing all or part of the matrix
            V**T:
            = 'A':  all N rows of V**T are returned in the array VT;
            = 'S':  the first min(m,n) rows of V**T (the right singular
                    vectors) are returned in the array VT;
            = 'O':  the first min(m,n) rows of V**T (the right singular
                    vectors) are overwritten on the array A;
            = 'N':  no rows of V**T (no right singular vectors) are
                    computed.

            JOBVT and JOBU cannot both be 'O'.

    M       (input) long
            The number of rows of the input matrix A.  M >= 0.

    N       (input) long
            The number of columns of the input matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit,
            if JOBU = 'O',  A is overwritten with the first min(m,n)
                            columns of U (the left singular vectors,
                            stored columnwise);
            if JOBVT = 'O', A is overwritten with the first min(m,n)
                            rows of V**T (the right singular vectors,
                            stored rowwise);
            if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A
                            are destroyed.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    S       (output) double array, dimension (min(M,N))
            The singular values of A, sorted so that S(i) >= S(i+1).

    U       (output) double array, dimension (LDU,UCOL)
            (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.
            If JOBU = 'A', U contains the M-by-M orthogonal matrix U;
            if JOBU = 'S', U contains the first min(m,n) columns of U
            (the left singular vectors, stored columnwise);
            if JOBU = 'N' or 'O', U is not referenced.

    LDU     (input) long
            The leading dimension of the array U.  LDU >= 1; if
            JOBU = 'S' or 'A', LDU >= M.

    VT      (output) double array, dimension (LDVT,N)
            If JOBVT = 'A', VT contains the N-by-N orthogonal matrix
            V**T;
            if JOBVT = 'S', VT contains the first min(m,n) rows of
            V**T (the right singular vectors, stored rowwise);
            if JOBVT = 'N' or 'O', VT is not referenced.

    LDVT    (input) long
            The leading dimension of the array VT.  LDVT >= 1; if
            JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK;
            if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged
            superdiagonal elements of an upper bidiagonal matrix B
            whose diagonal is in S (not necessarily sorted). B
            satisfies A = U * B * VT, so it has the same singular values
            as A, and singular vectors related by U and VT.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= 1.
            LWORK >= MAX(3*MIN(M,N)+MAX(M,N),5*MIN(M,N)).
            For good performance, LWORK should generally be larger.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit.
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if DBDSQR did not converge, INFO specifies how many
                  superdiagonals of an intermediate bidiagonal form B
                  did not converge to zero. See the description of WORK
                  above for details.

    =====================================================================
*/

int NUMlapack_dgetf2 (long *m, long *n, double *a, long *lda, long *ipiv, long *info);
/*  Purpose
    =======

    NUMlapack_dgetf2 computes an LU factorization of a general m-by-n matrix A
    using partial pivoting with row interchanges.

    The factorization has the form
       A = P * L * U
    where P is a permutation matrix, L is lower triangular with unit
    diagonal elements (lower trapezoidal if m > n), and U is upper
    triangular (upper trapezoidal if m < n).

    This is the right-looking Level 2 BLAS version of the algorithm.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n matrix to be factored.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    IPIV    (output) long array, dimension (min(M,N))
            The pivot indices; for 1 <= i <= min(M,N), row i of the
            matrix was interchanged with row IPIV(i).

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -k, the k-th argument had an illegal value
            > 0: if INFO = k, U(k,k) is exactly zero. The factorization
                 has been completed, but the factor U is exactly
                 singular, and division by zero will occur if it is used
                 to solve a system of equations.

    =====================================================================
*/

int NUMlapack_dgetri (long *n, double *a, long *lda, long *ipiv, double *work,
	long *lwork, long *info);
/* Purpose
    =======

    NUMlapack_dgetri computes the inverse of a matrix using the LU factorization
    computed by NUMlapack_dgetrf.

    This method inverts U and then computes inv(A) by solving the system
    inv(A)*L = inv(U) for inv(A).

    Arguments
    =========

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the factors L and U from the factorization
            A = P*L*U as computed by DGETRF.
            On exit, if INFO = 0, the inverse of the original matrix A.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (input) long array, dimension (N)
            The pivot indices from DGETRF; for 1<=i<=N, row i of the
            matrix was interchanged with row IPIV(i).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO=0, then WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= max(1,N).
            For optimal performance LWORK >= N*NB, where NB is
            the optimal blocksize returned by NUMlapack_ilaenv.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, U(i,i) is exactly zero; the matrix is
                  singular and its inverse could not be computed.

    =====================================================================
*/

int NUMlapack_dgetrf (long *m, long *n, double *a, long *lda, long *ipiv, long *info);
/*  Purpose
    =======

    NUMlapack_dgetrf computes an LU factorization of a general M-by-N matrix A
    using partial pivoting with row interchanges.

    The factorization has the form
       A = P * L * U
    where P is a permutation matrix, L is lower triangular with unit
    diagonal elements (lower trapezoidal if m > n), and U is upper
    triangular (upper trapezoidal if m < n).

    This is the right-looking Level 3 BLAS version of the algorithm.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix to be factored.
            On exit, the factors L and U from the factorization
            A = P*L*U; the unit diagonal elements of L are not stored.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    IPIV    (output) long array, dimension (min(M,N))
            The pivot indices; for 1 <= i <= min(M,N), row i of the
            matrix was interchanged with row IPIV(i).

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, U(i,i) is exactly zero. The factorization
                  has been completed, but the factor U is exactly
                  singular, and division by zero will occur if it is used
                  to solve a system of equations.

    =====================================================================
*/

int NUMlapack_dgetrs (const char *trans, long *n, long *nrhs, double *a, long *lda,
	long *ipiv, double *b, long *ldb, long *info);
/*  Purpose
    =======

    NUMlapack_dgetrs solves a system of linear equations
       A * X = B  or  A' * X = B
    with a general N-by-N matrix A using the LU factorization computed
    by DGETRF.

    Arguments
    =========

    TRANS   (input) char*
            Specifies the form of the system of equations:
            = 'N':  A * X = B  (No transpose)
            = 'T':  A'* X = B  (Transpose)
            = 'C':  A'* X = B  (Conjugate transpose = Transpose)

    N       (input) long
            The order of the matrix A.  N >= 0.

    NRHS    (input) long
            The number of right hand sides, i.e., the number of columns
            of the matrix B.  NRHS >= 0.

    A       (input) double array, dimension (LDA,N)
            The factors L and U from the factorization A = P*L*U
            as computed by DGETRF.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    IPIV    (input) long array, dimension (N)
            The pivot indices from DGETRF; for 1<=i<=N, row i of the
            matrix was interchanged with row IPIV(i).

    B       (input/output) double array, dimension (LDB,NRHS)
            On entry, the right hand side matrix B.
            On exit, the solution matrix X.

    LDB     (input) long
            The leading dimension of the array B.  LDB >= max(1,N).

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dggsvd (const char *jobu, const char *jobv, const char *jobq, long *m, long *n,
	long *p, long *k, long *l, double *a, long *lda, double *b, long *ldb,
	double *alpha, double *beta, double *u, long *ldu, double *v, long *ldv,
	double *q, long *ldq, double *work, long *iwork, long *info);
/*  Purpose
    =======

    NUMlapack_dggsvd computes the generalized singular value decomposition (GSVD)
    of an M-by-N real matrix A and P-by-N real matrix B:

        U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R )

    where U, V and Q are orthogonal matrices, and Z' is the transpose
    of Z.  Let K+L = the effective numerical rank of the matrix (A',B')',
    then R is a K+L-by-K+L nonsingular upper triangular matrix, D1 and
    D2 are M-by-(K+L) and P-by-(K+L) "diagonal" matrices and of the
    following structures, respectively:

    If M-K-L >= 0,

                        K  L
           D1 =     K ( I  0 )
                    L ( 0  C )
                M-K-L ( 0  0 )

                      K  L
           D2 =   L ( 0  S )
                P-L ( 0  0 )

                    N-K-L  K    L
      ( 0 R ) = K (  0   R11  R12 )
                L (  0    0   R22 )

    where

      C = diag( ALPHA(K+1), ... , ALPHA(K+L) ),
      S = diag( BETA(K+1),  ... , BETA(K+L) ),
      C**2 + S**2 = I.

      R is stored in A(1:K+L,N-K-L+1:N) on exit.

    If M-K-L < 0,

                      K M-K K+L-M
           D1 =   K ( I  0    0   )
                M-K ( 0  C    0   )

                        K M-K K+L-M
           D2 =   M-K ( 0  S    0  )
                K+L-M ( 0  0    I  )
                  P-L ( 0  0    0  )

                       N-K-L  K   M-K  K+L-M
      ( 0 R ) =     K ( 0    R11  R12  R13  )
                  M-K ( 0     0   R22  R23  )
                K+L-M ( 0     0    0   R33  )

    where

      C = diag( ALPHA(K+1), ... , ALPHA(M) ),
      S = diag( BETA(K+1),  ... , BETA(M) ),
      C**2 + S**2 = I.

      (R11 R12 R13 ) is stored in A(1:M, N-K-L+1:N), and R33 is stored
      ( 0  R22 R23 )
      in B(M-K+1:L,N+M-K-L+1:N) on exit.

     The routine computes C, S, R, and optionally the orthogonal
    transformation matrices U, V and Q.

    In particular, if B is an N-by-N nonsingular matrix, then the GSVD of
    A and B implicitly gives the SVD of A*inv(B):
                         A*inv(B) = U*(D1*inv(D2))*V'.
    If ( A',B')' has orthonormal columns, then the GSVD of A and B is
    also equal to the CS decomposition of A and B. Furthermore, the GSVD
    can be used to derive the solution of the eigenvalue problem:
                         A'*A x = lambda* B'*B x.
    In some literature, the GSVD of A and B is presented in the form
                     U'*A*X = ( 0 D1 ),   V'*B*X = ( 0 D2 )
    where U and V are orthogonal and X is nonsingular, D1 and D2 are
    ``diagonal''.  The former GSVD form can be converted to the latter
    form by taking the nonsingular matrix X as

                         X = Q*( I   0    )
                               ( 0 inv(R) ).

    Arguments
    =========

    JOBU    (input) char*
            = 'U':  Orthogonal matrix U is computed;
            = 'N':  U is not computed.

    JOBV    (input) char*
            = 'V':  Orthogonal matrix V is computed;
            = 'N':  V is not computed.

    JOBQ    (input) char*
            = 'Q':  Orthogonal matrix Q is computed;
            = 'N':  Q is not computed.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrices A and B.  N >= 0.

    P       (input) long
            The number of rows of the matrix B.  P >= 0.

    K       (output) long
    L       (output) long
            On exit, K and L specify the dimension of the subblocks
            described in the Purpose section.
            K + L = effective numerical rank of (A',B')'.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, A contains the triangular matrix R, or part of R.
            See Purpose for details.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,M).

    B       (input/output) double array, dimension (LDB,N)
            On entry, the P-by-N matrix B.
            On exit, B contains the triangular matrix R if M-K-L < 0.
            See Purpose for details.

    LDB     (input) long
            The leading dimension of the array B. LDA >= max(1,P).

    ALPHA   (output) double array, dimension (N)
    BETA    (output) double array, dimension (N)
            On exit, ALPHA and BETA contain the generalized singular
            value pairs of A and B;
              ALPHA(1:K) = 1,
              BETA(1:K)  = 0,
            and if M-K-L >= 0,
              ALPHA(K+1:K+L) = C,
              BETA(K+1:K+L)  = S,
            or if M-K-L < 0,
              ALPHA(K+1:M)=C, ALPHA(M+1:K+L)=0
              BETA(K+1:M) =S, BETA(M+1:K+L) =1
            and
              ALPHA(K+L+1:N) = 0
              BETA(K+L+1:N)  = 0

    U       (output) double array, dimension (LDU,M)
            If JOBU = 'U', U contains the M-by-M orthogonal matrix U.
            If JOBU = 'N', U is not referenced.

    LDU     (input) long
            The leading dimension of the array U. LDU >= max(1,M) if
            JOBU = 'U'; LDU >= 1 otherwise.

    V       (output) double array, dimension (LDV,P)
            If JOBV = 'V', V contains the P-by-P orthogonal matrix V.
            If JOBV = 'N', V is not referenced.

    LDV     (input) long
            The leading dimension of the array V. LDV >= max(1,P) if
            JOBV = 'V'; LDV >= 1 otherwise.

    Q       (output) double array, dimension (LDQ,N)
            If JOBQ = 'Q', Q contains the N-by-N orthogonal matrix Q.
            If JOBQ = 'N', Q is not referenced.

    LDQ     (input) long
            The leading dimension of the array Q. LDQ >= max(1,N) if
            JOBQ = 'Q'; LDQ >= 1 otherwise.

    WORK    (workspace) double array,
                        dimension (max(3*N,M,P)+N)

    IWORK   (workspace/output) long array, dimension (N)
            On exit, IWORK stores the sorting information. More
            precisely, the following loop will sort ALPHA
               for I = K+1, min(M,K+L)
                   swap ALPHA(I) and ALPHA(IWORK(I))
               endfor
            such that ALPHA(1) >= ALPHA(2) >= ... >= ALPHA(N).

    INFO    (output)long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            > 0:  if INFO = 1, the Jacobi-type procedure failed to
                  converge.  For further details, see subroutine NUMlapack_dtgsja.

    Internal Parameters
    ===================

    TOLA    double
    TOLB    double
            TOLA and TOLB are the thresholds to determine the effective
            rank of (A',B')'. Generally, they are set to
                     TOLA = MAX(M,N)*norm(A)*MAZHEPS,
                     TOLB = MAX(P,N)*norm(B)*MAZHEPS.
            The size of TOLA and TOLB may affect the size of backward
            errors of the decomposition.

    =====================================================================
*/

int NUMlapack_dggsvp (const char *jobu, const char *jobv, const char *jobq, long *m, long *p,
	long *n, double *a, long *lda, double *b, long *ldb, double *tola,
	double *tolb, long *k, long *l, double *u, long *ldu, double *v, long *ldv,
	double *q, long *ldq, long *iwork, double *tau, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dggsvp computes orthogonal matrices U, V and Q such that

                     N-K-L  K    L
     U'*A*Q =     K ( 0    A12  A13 )  if M-K-L >= 0;
                  L ( 0     0   A23 )
              M-K-L ( 0     0    0  )

                     N-K-L  K    L
            =     K ( 0    A12  A13 )  if M-K-L < 0;
                M-K ( 0     0   A23 )

                   N-K-L  K    L
     V'*B*Q =   L ( 0     0   B13 )
              P-L ( 0     0    0  )

    where the K-by-K matrix A12 and L-by-L matrix B13 are nonsingular
    upper triangular; A23 is L-by-L upper triangular if M-K-L >= 0,
    otherwise A23 is (M-K)-by-L upper trapezoidal.  K+L = the effective
    numerical rank of the (M+P)-by-N matrix (A',B')'.  Z' denotes the
    transpose of Z.

    This decomposition is the preprocessing step for computing the
    Generalized Singular Value Decomposition (GSVD), see subroutine
    NUMlapack_dggsvd.

    Arguments
    =========

    JOBU    (input) char*
            = 'U':  Orthogonal matrix U is computed;
            = 'N':  U is not computed.

    JOBV    (input) char*
            = 'V':  Orthogonal matrix V is computed;
            = 'N':  V is not computed.

    JOBQ    (input) char*
            = 'Q':  Orthogonal matrix Q is computed;
            = 'N':  Q is not computed.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    P       (input) long
            The number of rows of the matrix B.  P >= 0.

    N       (input) long
            The number of columns of the matrices A and B.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, A contains the triangular (or trapezoidal) matrix
            described in the Purpose section.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,M).

    B       (input/output) double array, dimension (LDB,N)
            On entry, the P-by-N matrix B.
            On exit, B contains the triangular matrix described in
            the Purpose section.

    LDB     (input) long
            The leading dimension of the array B. LDB >= max(1,P).

    TOLA    (input) double
    TOLB    (input) double
            TOLA and TOLB are the thresholds to determine the effective
            numerical rank of matrix B and a subblock of A. Generally,
            they are set to
               TOLA = MAX(M,N)*norm(A)*MAZHEPS,
               TOLB = MAX(P,N)*norm(B)*MAZHEPS.
            The size of TOLA and TOLB may affect the size of backward
            errors of the decomposition.

    K       (output) long
    L       (output) long
            On exit, K and L specify the dimension of the subblocks
            described in Purpose.
            K + L = effective numerical rank of (A',B')'.

    U       (output) double array, dimension (LDU,M)
            If JOBU = 'U', U contains the orthogonal matrix U.
            If JOBU = 'N', U is not referenced.

    LDU     (input) long
            The leading dimension of the array U. LDU >= max(1,M) if
            JOBU = 'U'; LDU >= 1 otherwise.

    V       (output) double array, dimension (LDV,M)
            If JOBV = 'V', V contains the orthogonal matrix V.
            If JOBV = 'N', V is not referenced.

    LDV     (input) long
            The leading dimension of the array V. LDV >= max(1,P) if
            JOBV = 'V'; LDV >= 1 otherwise.

    Q       (output) double array, dimension (LDQ,N)
            If JOBQ = 'Q', Q contains the orthogonal matrix Q.
            If JOBQ = 'N', Q is not referenced.

    LDQ     (input) long
            The leading dimension of the array Q. LDQ >= max(1,N) if
            JOBQ = 'Q'; LDQ >= 1 otherwise.

    IWORK   (workspace) long array, dimension (N)

    TAU     (workspace) double array, dimension (N)

    WORK    (workspace) double array, dimension (max(3*N,M,P))

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.


    Further Details
    ===============

    The subroutine uses LAPACK subroutine DGEQPF for the QR factorization
    with column pivoting to detect the effective numerical rank of the
    a matrix. It may be replaced by a better rank determination strategy.

    =====================================================================
*/

int NUMlapack_dhseqr (const char *job, const char *compz, long *n, long *ilo, long *ihi,
	double *h, long *ldh, double *wr, double *wi, double *z, long *ldz,
	double *work, long *lwork, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dhseqr computes the eigenvalues of a real upper Hessenberg matrix H
    and, optionally, the matrices T and Z from the Schur decomposition
    H = Z T Z**T, where T is an upper quasi-triangular matrix (the Schur
    form), and Z is the orthogonal matrix of Schur vectors.

    Optionally Z may be postmultiplied into an input orthogonal matrix Q,
    so that this routine can give the Schur factorization of a matrix A
    which has been reduced to the Hessenberg form H by the orthogonal
    matrix Q:  A = Q*H*Q**T = (QZ)*T*(QZ)**T.

    Arguments
    =========

    JOB     (input) char*
            = 'E':  compute eigenvalues only;
            = 'S':  compute eigenvalues and the Schur form T.

    COMPZ   (input) char*
            = 'N':  no Schur vectors are computed;
            = 'I':  Z is initialized to the unit matrix and the matrix Z
                    of Schur vectors of H is returned;
            = 'V':  Z must contain an orthogonal matrix Q on entry, and
                    the product Q*Z is returned.

    N       (input) long
            The order of the matrix H.  N >= 0.

    ILO     (input) long
    IHI     (input) long
            It is assumed that H is already upper triangular in rows
            and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally
            set by a previous call to NUMlapack_dgebal, and then passed to SGEHRD
            when the matrix output by NUMlapack_dgebal is reduced to Hessenberg
            form. Otherwise ILO and IHI should be set to 1 and N
            respectively.
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    H       (input/output) double array, dimension (LDH,N)
            On entry, the upper Hessenberg matrix H.
            On exit, if JOB = 'S', H contains the upper quasi-triangular
            matrix T from the Schur decomposition (the Schur form);
            2-by-2 diagonal blocks (corresponding to complex conjugate
            pairs of eigenvalues) are returned in standard form, with
            H(i,i) = H(i+1,i+1) and H(i+1,i)*H(i,i+1) < 0. If JOB = 'E',
            the contents of H are unspecified on exit.

    LDH     (input) long
            The leading dimension of the array H. LDH >= max(1,N).

    WR      (output) double array, dimension (N)
    WI      (output) double array, dimension (N)
            The real and imaginary parts, respectively, of the computed
            eigenvalues. If two eigenvalues are computed as a complex
            conjugate pair, they are stored in consecutive elements of
            WR and WI, say the i-th and (i+1)th, with WI(i) > 0 and
            WI(i+1) < 0. If JOB = 'S', the eigenvalues are stored in the
            same order as on the diagonal of the Schur form returned in
            H, with WR(i) = H(i,i) and, if H(i:i+1,i:i+1) is a 2-by-2
            diagonal block, WI(i) = sqrt(H(i+1,i)*H(i,i+1)) and
            WI(i+1) = -WI(i).

    Z       (input/output) double array, dimension (LDZ,N)
            If COMPZ = 'N': Z is not referenced.
            If COMPZ = 'I': on entry, Z need not be set, and on exit, Z
            contains the orthogonal matrix Z of the Schur vectors of H.
            If COMPZ = 'V': on entry Z must contain an N-by-N matrix Q,
            which is assumed to be equal to the unit matrix except for
            the submatrix Z(ILO:IHI,ILO:IHI); on exit Z contains Q*Z.
            Normally Q is the orthogonal matrix generated by NUMlapack_dorghr after
            the call to NUMlapack_dgehrd which formed the Hessenberg matrix H.

    LDZ     (input) long
            The leading dimension of the array Z.
            LDZ >= max(1,N) if COMPZ = 'I' or 'V'; LDZ >= 1 otherwise.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= max(1,N).

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, NUMlapack_dhseqr failed to compute all of the
                  eigenvalues in a total of 30*(IHI-ILO+1) iterations;
                  elements 1:ilo-1 and i+1:n of WR and WI contain those
                  eigenvalues which have been successfully computed.

    =====================================================================
*/

int NUMlapack_dlabad (double *smal, double *large);
/*  Purpose
    =======

    NUMlapack_dlabad takes as input the values computed by DLAMCH for underflow and
    overflow, and returns the square root of each of these values if the
    log of LARGE is sufficiently large.  This subroutine is intended to
    identify machines with a large exponent range, such as the Crays, and
    redefine the underflow and overflow limits to be the square roots of
    the values computed by DLAMCH.  This subroutine is needed because
    DLAMCH does not compensate for poor arithmetic in the upper half of
    the exponent range, as is found on a Cray.

    Arguments
    =========

    smal   (input/output) double
            On entry, the underflow threshold as computed by DLAMCH.
            On exit, if LOG10(LARGE) is sufficiently large, the square
            root of smal, otherwise unchanged.

    LARGE   (input/output) double
            On entry, the overflow threshold as computed by DLAMCH.
            On exit, if LOG10(LARGE) is sufficiently large, the square
            root of LARGE, otherwise unchanged.

    =====================================================================


       If it looks like we're on a Cray, take the square root of
       smal and LARGE to avoid overflow and underflow problems.
*/

int NUMlapack_dlabrd (long *m, long *n, long *nb, double *a, long *lda, double *d,
	double *e, double *tauq, double *taup, double *x, long *ldx, double *y,
	long *ldy);
/* Purpose
    =======

    NUMlapack_dlabrd reduces the first NB rows and columns of a real general
    m by n matrix A to upper or lower bidiagonal form by an orthogonal
    transformation Q' * A * P, and returns the matrices X and Y which
    are needed to apply the transformation to the unreduced part of A.

    If m >= n, A is reduced to upper bidiagonal form; if m < n, to lower
    bidiagonal form.

    This is an auxiliary routine called by DGEBRD

    Arguments
    =========

    M       (input) long
            The number of rows in the matrix A.

    N       (input) long
            The number of columns in the matrix A.

    NB      (input) long
            The number of leading rows and columns of A to be reduced.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the m by n general matrix to be reduced.
            On exit, the first NB rows and columns of the matrix are
            overwritten; the rest of the array is unchanged.
            If m >= n, elements on and below the diagonal in the first NB
              columns, with the array TAUQ, represent the orthogonal
              matrix Q as a product of elementary reflectors; and
              elements above the diagonal in the first NB rows, with the
              array TAUP, represent the orthogonal matrix P as a product
              of elementary reflectors.
            If m < n, elements below the diagonal in the first NB
              columns, with the array TAUQ, represent the orthogonal
              matrix Q as a product of elementary reflectors, and
              elements on and above the diagonal in the first NB rows,
              with the array TAUP, represent the orthogonal matrix P as
              a product of elementary reflectors.
            See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    D       (output) double array, dimension (NB)
            The diagonal elements of the first NB rows and columns of
            the reduced matrix.  D(i) = A(i,i).

    E       (output) double array, dimension (NB)
            The off-diagonal elements of the first NB rows and columns of
            the reduced matrix.

    TAUQ    (output) double array dimension (NB)
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix Q. See Further Details.

    TAUP    (output) double array, dimension (NB)
            The scalar factors of the elementary reflectors which
            represent the orthogonal matrix P. See Further Details.

    X       (output) double array, dimension (LDX,NB)
            The m-by-nb matrix X required to update the unreduced part
            of A.

    LDX     (input) long
            The leading dimension of the array X. LDX >= M.

    Y       (output) double array, dimension (LDY,NB)
            The n-by-nb matrix Y required to update the unreduced part
            of A.

    LDY     (output) long
            The leading dimension of the array Y. LDY >= N.

    Further Details
    ===============

    The matrices Q and P are represented as products of elementary
    reflectors:

       Q = H(1) H(2) . . . H(nb)  and  P = G(1) G(2) . . . G(nb)

    Each H(i) and G(i) has the form:

       H(i) = I - tauq * v * v'  and G(i) = I - taup * u * u'

    where tauq and taup are real scalars, and v and u are real vectors.

    If m >= n, v(1:i-1) = 0, v(i) = 1, and v(i:m) is stored on exit in
    A(i:m,i); u(1:i) = 0, u(i+1) = 1, and u(i+1:n) is stored on exit in
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).

    If m < n, v(1:i) = 0, v(i+1) = 1, and v(i+1:m) is stored on exit in
    A(i+2:m,i); u(1:i-1) = 0, u(i) = 1, and u(i:n) is stored on exit in
    A(i,i+1:n); tauq is stored in TAUQ(i) and taup in TAUP(i).

    The elements of the vectors v and u together form the m-by-nb matrix
    V and the nb-by-n matrix U' which are needed, with X and Y, to apply
    the transformation to the unreduced part of the matrix, using a block
    update of the form:  A := A - V*Y' - X*U'.

    The contents of A on exit are illustrated by the following examples
    with nb = 2:

    m = 6 and n = 5 (m > n):          m = 5 and n = 6 (m < n):

      (  1   1   u1  u1  u1 )           (  1   u1  u1  u1  u1  u1 )
      (  v1  1   1   u2  u2 )           (  1   1   u2  u2  u2  u2 )
      (  v1  v2  a   a   a  )           (  v1  1   a   a   a   a  )
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )
      (  v1  v2  a   a   a  )           (  v1  v2  a   a   a   a  )
      (  v1  v2  a   a   a  )

    where a denotes an element of the original matrix which is unchanged,
    vi denotes an element of the vector defining H(i), and ui an element
    of the vector defining G(i).

    =====================================================================
*/

int NUMlapack_dlacpy (const char *uplo, long *m, long *n, double *a, long *lda, double *b, long *ldb);
/*  Purpose
    =======

    NUMlapack_dlacpy copies all or part of a two-dimensional matrix A to another
    matrix B.

    Arguments
    =========

    UPLO    (input) char*
            Specifies the part of the matrix A to be copied to B.
            = 'U':      Upper triangular part
            = 'L':      Lower triangular part
            Otherwise:  All of the matrix A

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input) double array, dimension (LDA,N)
            The m by n matrix A.  If UPLO = 'U', only the upper triangle
            or trapezoid is accessed; if UPLO = 'L', only the lower
            triangle or trapezoid is accessed.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    B       (output) double array, dimension (LDB,N)
            On exit, B = A in the locations specified by UPLO.

    LDB     (input) long
            The leading dimension of the array B.  LDB >= max(1,M).

    =====================================================================
*/

int NUMlapack_dladiv (double *a, double *b, double *c, double *d, double *p, double *q);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    NUMlapack_dladiv performs complex division in  real arithmetic

                          a + i*b
               p + i*q = ---------
                          c + i*d

    The algorithm is due to Robert L. Smith and can be found
    in D. Knuth, The art of Computer Programming, Vol.2, p.195

    Arguments
    =========

    A       (input) double
    B       (input) double
    C       (input) double
    D       (input) double
            The scalars a, b, c, and d in the above expression.

    P       (output) double
    Q       (output) double
            The scalars p and q in the above expression.

    =====================================================================
*/

int NUMlapack_dlae2 (double *a, double *b, double *c, double *rt1, double *rt2);
/*   Purpose
    =======

    NUMlapack_dlae2  computes the eigenvalues of a 2-by-2 symmetric matrix
       [  A   B  ]
       [  B   C  ].
    On return, RT1 is the eigenvalue of larger absolute value, and RT2
    is the eigenvalue of smaller absolute value.

    Arguments
    =========

    A       (input) double
            The (1,1) element of the 2-by-2 matrix.

    B       (input) double
            The (1,2) and (2,1) elements of the 2-by-2 matrix.

    C       (input) double
            The (2,2) element of the 2-by-2 matrix.

    RT1     (output) double
            The eigenvalue of larger absolute value.

    RT2     (output) double
            The eigenvalue of smaller absolute value.

    Further Details
    ===============

    RT1 is accurate to a few ulps barring over/underflow.

    RT2 may be inaccurate if there is massive cancellation in the
    determinant A*C-B*B; higher precision or correctly rounded or
    correctly truncated arithmetic would be needed to compute RT2
    accurately in all cases.

    Overflow is possible only if RT1 is within a factor of 5 of overflow.
    Underflow is harmless if the input data is 0 or exceeds
       underflow_threshold / macheps.

   =====================================================================
*/

int NUMlapack_dlaev2 (double *a, double *b, double *c, double *rt1, double *rt2,
	double *cs1, double *sn1);
/*  Purpose
    =======

    NUMlapack_dlaev2 computes the eigendecomposition of a 2-by-2 symmetric matrix
       [  A   B  ]
       [  B   C  ].
    On return, RT1 is the eigenvalue of larger absolute value, RT2 is the
    eigenvalue of smaller absolute value, and (CS1,SN1) is the unit right
    eigenvector for RT1, giving the decomposition

       [ CS1  SN1 ] [  A   B  ] [ CS1 -SN1 ]  =  [ RT1  0  ]
       [-SN1  CS1 ] [  B   C  ] [ SN1  CS1 ]     [  0  RT2 ].

    Arguments
    =========

    A       (input) double
            The (1,1) element of the 2-by-2 matrix.

    B       (input) double
            The (1,2) element and the conjugate of the (2,1) element of
            the 2-by-2 matrix.

    C       (input) double
            The (2,2) element of the 2-by-2 matrix.

    RT1     (output) double
            The eigenvalue of larger absolute value.

    RT2     (output) double
            The eigenvalue of smaller absolute value.

    CS1     (output) double
    SN1     (output) double
            The vector (CS1, SN1) is a unit right eigenvector for RT1.

    Further Details
    ===============

    RT1 is accurate to a few ulps barring over/underflow.

    RT2 may be inaccurate if there is massive cancellation in the
    determinant A*C-B*B; higher precision or correctly rounded or
    correctly truncated arithmetic would be needed to compute RT2
    accurately in all cases.

    CS1 and SN1 are accurate to a few ulps barring over/underflow.

    Overflow is possible only if RT1 is within a factor of 5 of overflow.
    Underflow is harmless if the input data is 0 or exceeds
       underflow_threshold / macheps.

   =====================================================================
*/

int NUMlapack_dlags2 (long *upper, double *a1, double *a2,	double *a3, double *b1,
	double *b2, double *b3, double *csu, double *snu, double *csv, double *snv,
	double *csq, double *snq);
/*  Purpose
    =======

    NUMlapack_dlags2 computes 2-by-2 orthogonal matrices U, V and Q, such
    that if ( UPPER ) then

              U'*A*Q = U'*( A1 A2 )*Q = ( x  0  )
                          ( 0  A3 )     ( x  x  )
    and
              V'*B*Q = V'*( B1 B2 )*Q = ( x  0  )
                          ( 0  B3 )     ( x  x  )

    or if ( .NOT.UPPER ) then

              U'*A*Q = U'*( A1 0  )*Q = ( x  x  )
                          ( A2 A3 )     ( 0  x  )
    and
              V'*B*Q = V'*( B1 0  )*Q = ( x  x  )
                          ( B2 B3 )     ( 0  x  )

    The rows of the transformed A and B are parallel, where

      U = (  CSU  SNU ), V = (  CSV SNV ), Q = (  CSQ   SNQ )
          ( -SNU  CSU )      ( -SNV CSV )      ( -SNQ   CSQ )

    Z' denotes the transpose of Z.


    Arguments
    =========

    UPPER   (input) long* (boolean)
            = TRUE: the input matrices A and B are upper triangular.
            = FALSE: the input matrices A and B are lower triangular.

    A1      (input) double
    A2      (input) double
    A3      (input) double
            On entry, A1, A2 and A3 are elements of the input 2-by-2
            upper (lower) triangular matrix A.

    B1      (input) double
    B2      (input) double
    B3      (input) double
            On entry, B1, B2 and B3 are elements of the input 2-by-2
            upper (lower) triangular matrix B.

    CSU     (output) double
    SNU     (output) double
            The desired orthogonal matrix U.

    CSV     (output) double
    SNV     (output) double
            The desired orthogonal matrix V.

    CSQ     (output) double
    SNQ     (output) double
            The desired orthogonal matrix Q.

    =====================================================================
*/

int NUMlapack_dlahqr (int * wantt, int * wantz, long *n, long *ilo,
	long *ihi, double *h, long *ldh, double *wr, double *wi, long *iloz,
	long *ihiz, double *z, long *ldz, long *info);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dlahqr is an auxiliary routine called by NUMlapack_dhseqr to update the
    eigenvalues and Schur decomposition already computed by NUMlapack_dhseqr, by
    dealing with the Hessenberg submatrix in rows and columns ILO to IHI.

    Arguments
    =========

    WANTT   (input) int
            = .TRUE. : the full Schur form T is required;
            = .FALSE.: only eigenvalues are required.

    WANTZ   (input) int
            = .TRUE. : the matrix of Schur vectors Z is required;
            = .FALSE.: Schur vectors are not required.

    N       (input) long
            The order of the matrix H.  N >= 0.

    ILO     (input) long
    IHI     (input) long
            It is assumed that H is already upper quasi-triangular in
            rows and columns IHI+1:N, and that H(ILO,ILO-1) = 0 (unless
            ILO = 1). NUMlapack_dlahqr works primarily with the Hessenberg
            submatrix in rows and columns ILO to IHI, but applies
            transformations to all of H if WANTT is .TRUE..
            1 <= ILO <= max(1,IHI); IHI <= N.

    H       (input/output) double array, dimension (LDH,N)
            On entry, the upper Hessenberg matrix H.
            On exit, if WANTT is .TRUE., H is upper quasi-triangular in
            rows and columns ILO:IHI, with any 2-by-2 diagonal blocks in
            standard form. If WANTT is .FALSE., the contents of H are
            unspecified on exit.

    LDH     (input) long
            The leading dimension of the array H. LDH >= max(1,N).

    WR      (output) double array, dimension (N)
    WI      (output) double array, dimension (N)
            The real and imaginary parts, respectively, of the computed
            eigenvalues ILO to IHI are stored in the corresponding
            elements of WR and WI. If two eigenvalues are computed as a
            complex conjugate pair, they are stored in consecutive
            elements of WR and WI, say the i-th and (i+1)th, with
            WI(i) > 0 and WI(i+1) < 0. If WANTT is .TRUE., the
            eigenvalues are stored in the same order as on the diagonal
            of the Schur form returned in H, with WR(i) = H(i,i), and, if
            H(i:i+1,i:i+1) is a 2-by-2 diagonal block,
            WI(i) = sqrt(H(i+1,i)*H(i,i+1)) and WI(i+1) = -WI(i).

    ILOZ    (input) long
    IHIZ    (input) long
            Specify the rows of Z to which transformations must be
            applied if WANTZ is .TRUE..
            1 <= ILOZ <= ILO; IHI <= IHIZ <= N.

    Z       (input/output) double array, dimension (LDZ,N)
            If WANTZ is .TRUE., on entry Z must contain the current
            matrix Z of transformations accumulated by NUMlapack_dhseqr, and on
            exit Z has been updated; transformations are applied only to
            the submatrix Z(ILOZ:IHIZ,ILO:IHI).
            If WANTZ is .FALSE., Z is not referenced.

    LDZ     (input) long
            The leading dimension of the array Z. LDZ >= max(1,N).

    INFO    (output) long
            = 0: successful exit
            > 0: NUMlapack_dlahqr failed to compute all the eigenvalues ILO to IHI
                 in a total of 30*(IHI-ILO+1) iterations; if INFO = i,
                 elements i+1:ihi of WR and WI contain those eigenvalues
                 which have been successfully computed.

    Further Details
    ===============

    2-96 Based on modifications by
       David Day, Sandia National Laboratory, USA

    =====================================================================
*/

int NUMlapack_dlahrd (long *n, long *k, long *nb, double *a, long *lda,
	double *tau, double *t, long *ldt, double *y, long *ldy);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dlahrd reduces the first NB columns of a real general n-by-(n-k+1)
    matrix A so that elements below the k-th subdiagonal are zero. The
    reduction is performed by an orthogonal similarity transformation
    Q' * A * Q. The routine returns the matrices V and T which determine
    Q as a block reflector I - V*T*V', and also the matrix Y = A * V * T.

    This is an auxiliary routine called by NUMlapack_dgehrd.

    Arguments
    =========

    N       (input) long
            The order of the matrix A.

    K       (input) long
            The offset for the reduction. Elements below the k-th
            subdiagonal in the first NB columns are reduced to zero.

    NB      (input) long
            The number of columns to be reduced.

    A       (input/output) double array, dimension (LDA,N-K+1)
            On entry, the n-by-(n-k+1) general matrix A.
            On exit, the elements on and above the k-th subdiagonal in
            the first NB columns are overwritten with the corresponding
            elements of the reduced matrix; the elements below the k-th
            subdiagonal, with the array TAU, represent the matrix Q as a
            product of elementary reflectors. The other columns of A are
            unchanged. See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    TAU     (output) double array, dimension (NB)
            The scalar factors of the elementary reflectors. See Further
            Details.

    T       (output) double array, dimension (LDT,NB)
            The upper triangular matrix T.

    LDT     (input) long
            The leading dimension of the array T.  LDT >= NB.

    Y       (output) double array, dimension (LDY,NB)
            The n-by-nb matrix Y.

    LDY     (input) long
            The leading dimension of the array Y. LDY >= N.

    Further Details
    ===============

    The matrix Q is represented as a product of nb elementary reflectors

       Q = H(1) H(2) . . . H(nb).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in
    A(i+k+1:n,i), and tau in TAU(i).

    The elements of the vectors v together form the (n-k+1)-by-nb matrix
    V which is needed, with T and Y, to apply the transformation to the
    unreduced part of the matrix, using an update of the form:
    A := (I - V*T*V') * (A - Y*V').

    The contents of A on exit are illustrated by the following example
    with n = 7, k = 3 and nb = 2:

       ( a   h   a   a   a )
       ( a   h   a   a   a )
       ( a   h   a   a   a )
       ( h   h   a   a   a )
       ( v1  h   a   a   a )
       ( v1  v2  a   a   a )
       ( v1  v2  a   a   a )

    where a denotes an element of the original matrix A, h denotes a
    modified element of the upper Hessenberg matrix H, and vi denotes an
    element of the vector defining H(i).

    =====================================================================
*/

int NUMlapack_dlaln2 (int * ltrans, long *na, long *nw, double *smin,
	double *ca, double *a, long *lda, double *d1, double *d2, double *b,
	long *ldb, double *wr, double *wi, double *x, long *ldx, double *scale,
	double *xnorm, long *info);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    NUMlapack_dlaln2 solves a system of the form  (ca A - w D ) X = s B
    or (ca A' - w D) X = s B   with possible scaling ("s") and
    perturbation of A.  (A' means A-transpose.)

    A is an NA x NA real matrix, ca is a real scalar, D is an NA x NA
    real diagonal matrix, w is a real or complex value, and X and B are
    NA x 1 matrices -- real if w is real, complex if w is complex.  NA
    may be 1 or 2.

    If w is complex, X and B are represented as NA x 2 matrices,
    the first column of each being the real part and the second
    being the imaginary part.

    "s" is a scaling factor (.LE. 1), computed by NUMlapack_dlaln2, which is
    so chosen that X can be computed without overflow.  X is further
    scaled if necessary to assure that norm(ca A - w D)*norm(X) is less
    than overflow.

    If both singular values of (ca A - w D) are less than SMIN,
    SMIN*identity will be used instead of (ca A - w D).  If only one
    singular value is less than SMIN, one element of (ca A - w D) will be
    perturbed enough to make the smallest singular value roughly SMIN.
    If both singular values are at least SMIN, (ca A - w D) will not be
    perturbed.  In any case, the perturbation will be at most some small
    multiple of max( SMIN, ulp*norm(ca A - w D) ).  The singular values
    are computed by infinity-norm approximations, and thus will only be
    correct to a factor of 2 or so.

    Note: all input quantities are assumed to be smaller than overflow
    by a reasonable factor.  (See BIGNUM.)

    Arguments
    ==========

    LTRANS  (input) int
            =.TRUE.:  A-transpose will be used.
            =.FALSE.: A will be used (not transposed.)

    NA      (input) long
            The size of the matrix A.  It may (only) be 1 or 2.

    NW      (input) long
            1 if "w" is real, 2 if "w" is complex.  It may only be 1
            or 2.

    SMIN    (input) double
            The desired lower bound on the singular values of A.  This
            should be a safe distance away from underflow or overflow,
            say, between (underflow/machine precision) and  (machine
            precision * overflow ).  (See BIGNUM and ULP.)

    CA      (input) double
            The coefficient c, which A is multiplied by.

    A       (input) double array, dimension (LDA,NA)
            The NA x NA matrix A.

    LDA     (input) long
            The leading dimension of A.  It must be at least NA.

    D1      (input) double
            The 1,1 element in the diagonal matrix D.

    D2      (input) double
            The 2,2 element in the diagonal matrix D.  Not used if NW=1.

    B       (input) double array, dimension (LDB,NW)
            The NA x NW matrix B (right-hand side).  If NW=2 ("w" is
            complex), column 1 contains the real part of B and column 2
            contains the imaginary part.

    LDB     (input) long
            The leading dimension of B.  It must be at least NA.

    WR      (input) double
            The real part of the scalar "w".

    WI      (input) double
            The imaginary part of the scalar "w".  Not used if NW=1.

    X       (output) double array, dimension (LDX,NW)
            The NA x NW matrix X (unknowns), as computed by NUMlapack_dlaln2.
            If NW=2 ("w" is complex), on exit, column 1 will contain
            the real part of X and column 2 will contain the imaginary
            part.

    LDX     (input) long
            The leading dimension of X.  It must be at least NA.

    SCALE   (output) double
            The scale factor that B must be multiplied by to insure
            that overflow does not occur when computing X.  Thus,
            (ca A - w D) X  will be SCALE*B, not B (ignoring
            perturbations of A.)  It will be at most 1.

    XNORM   (output) double
            The infinity-norm of X, when X is regarded as an NA x NW
            real matrix.

    INFO    (output) long
            An error flag.  It will be set to zero if no error occurs,
            a negative number if an argument is in error, or a positive
            number if  ca A - w D  had to be perturbed.
            The possible values are:
            = 0: No error occurred, and (ca A - w D) did not have to be
                   perturbed.
            = 1: (ca A - w D) had to be perturbed to make its smallest
                 (or only) singular value greater than SMIN.
            NOTE: In the interests of speed, this routine does not
                  check the inputs for errors.

   =====================================================================
*/

double NUMlapack_dlange (const char *norm, long *m, long *n, double *a, long *lda, double *work);
/*  Purpose
    =======

    NUMlapack_dlange  returns the value of the one norm, or the Frobenius norm, or
    the  infinity norm, or the  element of  largest absolute value of a
    real matrix A.

    Description
    ===========

    DLANGE returns the value

       DLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) char*
            Specifies the value to be returned in DLANGE as described
            above.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.  When M = 0,
            DLANGE is set to zero.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.  When N = 0,
            DLANGE is set to zero.

    A       (input) double array, dimension (LDA,N)
            The m by n matrix A.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(M,1).

    WORK    (workspace) double array, dimension (LWORK),
            where LWORK >= M when NORM = 'I'; otherwise, WORK is not
            referenced.

   =====================================================================
*/

double NUMlapack_dlanhs (const char *norm, long *n, double *a, long *lda, double *work);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       October 31, 1992


    Purpose
    =======

    NUMlapack_dlanhs  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    Hessenberg matrix A.

    Description
    ===========

    NUMlapack_dlanhs returns the value

       NUMlapack_dlanhs =
	            ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) char*
            Specifies the value to be returned in NUMlapack_dlanhs as described
            above.

    N       (input) long
            The order of the matrix A.  N >= 0.  When N = 0, NUMlapack_dlanhs is
            set to zero.

    A       (input) double array, dimension (LDA,N)
            The n by n upper Hessenberg matrix A; the part of A below the
            first sub-diagonal is not referenced.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(N,1).

    WORK    (workspace) double array, dimension (LWORK),
            where LWORK >= N when NORM = 'I'; otherwise, WORK is not
            referenced.

   =====================================================================
*/

double NUMlapack_dlanst (const char *norm, long *n, double *d, double *e);
/*   Purpose
    =======

    NUMlapack_dlanst  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    real symmetric tridiagonal matrix A.

    Description
    ===========

    NUMlapack_dlanst returns the value
    NUMlapack_dlanst =
	            ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) char*
            Specifies the value to be returned in DLANST as described
            above.

    N       (input) long
            The order of the matrix A.  N >= 0.  When N = 0, DLANST is
            set to zero.

    D       (input) double array, dimension (N)
            The diagonal elements of A.

    E       (input) double array, dimension (N-1)
            The (n-1) sub-diagonal or super-diagonal elements of A.

    =====================================================================
*/

double NUMlapack_dlansy (const char *norm, const char *uplo, long *n, double *a,
	long *lda, double *work);
/*   Purpose
    =======

    NUMlapack_dlansy  returns the value of the one norm,  or the Frobenius norm, or
    the  infinity norm,  or the  element of  largest absolute value  of a
    real symmetric matrix A.

    Description
    ===========

    NUMlapack_dlansy returns the value

                ( max(abs(A(i,j))), NORM = 'M' or 'm'
                (
                ( norm1(A),         NORM = '1', 'O' or 'o'
                (
                ( normI(A),         NORM = 'I' or 'i'
                (
                ( normF(A),         NORM = 'F', 'f', 'E' or 'e'

    where  norm1  denotes the  one norm of a matrix (maximum column sum),
    normI  denotes the  infinity norm  of a matrix  (maximum row sum) and
    normF  denotes the  Frobenius norm of a matrix (square root of sum of
    squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm.

    Arguments
    =========

    NORM    (input) char*
            Specifies the value to be returned in DLANSY as described
            above.

    UPLO    (input) char*
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is to be referenced.
            = 'U':  Upper triangular part of A is referenced
            = 'L':  Lower triangular part of A is referenced

    N       (input) long
            The order of the matrix A.  N >= 0.  When N = 0, DLANSY is
            set to zero.

    A       (input) double array, dimension (LDA,N)
            The symmetric matrix A.  If UPLO = 'U', the leading n by n
            upper triangular part of A contains the upper triangular part
            of the matrix A, and the strictly lower triangular part of A
            is not referenced.  If UPLO = 'L', the leading n by n lower
            triangular part of A contains the lower triangular part of
            the matrix A, and the strictly upper triangular part of A is
            not referenced.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(N,1).

    WORK    (workspace) double array, dimension (LWORK),
            where LWORK >= N when NORM = 'I' or '1' or 'O'; otherwise,
            WORK is not referenced.

   =====================================================================
*/

int NUMlapack_dlanv2 (double *a, double *b, double *c, double *d, double *rt1r,
	double *rt1i,	double *rt2r, double *rt2i, double *cs, double *sn);
/*  -- LAPACK driver routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dlanv2 computes the Schur factorization of a real 2-by-2 nonsymmetric
    matrix in standard form:

         [ A  B ] = [ CS -SN ] [ AA  BB ] [ CS  SN ]
         [ C  D ]   [ SN  CS ] [ CC  DD ] [-SN  CS ]

    where either
    1) CC = 0 so that AA and DD are real eigenvalues of the matrix, or
    2) AA = DD and BB*CC < 0, so that AA + or - sqrt(BB*CC) are complex
    conjugate eigenvalues.

    Arguments
    =========

    A       (input/output) double
    B       (input/output) double
    C       (input/output) double
    D       (input/output) double
            On entry, the elements of the input matrix.
            On exit, they are overwritten by the elements of the
            standardised Schur form.

    RT1R    (output) double
    RT1I    (output) double
    RT2R    (output) double
    RT2I    (output) double
            The real and imaginary parts of the eigenvalues. If the
            eigenvalues are a complex conjugate pair, RT1I > 0.

    CS      (output) double
    SN      (output) double
            Parameters of the rotation matrix.

    Further Details
    ===============

    Modified by V. Sima, Research Institute for Informatics, Bucharest,
    Romania, to reduce the risk of cancellation errors,
    when computing real eigenvalues, and to ensure, if possible, that
    abs(RT1R) >= abs(RT2R).

    =====================================================================
*/

int NUMlapack_dlapll(long *n, double *x, long *incx, double *y, long *incy, double *ssmin);
/*  Purpose
    =======

    Given two column vectors X and Y, let

                         A = ( X Y ).

    The subroutine first computes the QR factorization of A = Q*R,
    and then computes the SVD of the 2-by-2 upper triangular matrix R.
    The smaller singular value of R is returned in SSMIN, which is used
    as the measurement of the linear dependency of the vectors X and Y.

    Arguments
    =========

    N       (input) long
            The length of the vectors X and Y.

    X       (input/output) double array,
                           dimension (1+(N-1)*INCX)
            On entry, X contains the N-vector X.
            On exit, X is overwritten.

    INCX    (input) long
            The increment between successive elements of X. INCX > 0.

    Y       (input/output) double array,
                           dimension (1+(N-1)*INCY)
            On entry, Y contains the N-vector Y.
            On exit, Y is overwritten.

    INCY    (input) long
            The increment between successive elements of Y. INCY > 0.

    SSMIN   (output) double
            The smallest singular value of the N-by-2 matrix A = ( X Y ).

    =====================================================================
*/

double NUMlapack_dlapy2 (double *x, double *y);
/*  Purpose
    =======

    NUMlapack_dlapy2 returns sqrt(x**2+y**2), taking care not to cause unnecessary
    overflow.

    Arguments
    =========

    X       (input) double
    Y       (input) double
            X and Y specify the values x and y.

    =====================================================================
*/

int NUMlapack_dlapmt (long *forwrd, long *m, long *n,	double *x, long *ldx, long *k);
/*  Purpose
    =======

    NUMlapack_dlapmt rearranges the columns of the M by N matrix X as specified
    by the permutation K(1),K(2),...,K(N) of the integers 1,...,N.
    If FORWRD = TRUE,  forward permutation:

         X(*,K(J)) is moved X(*,J) for J = 1,2,...,N.

    If FORWRD = FALSE, backward permutation:

         X(*,J) is moved to X(*,K(J)) for J = 1,2,...,N.

    Arguments
    =========

    FORWRD  (input) long* (boolean)
            = TRUE, forward permutation
            = FALSE, backward permutation

    M       (input) long
            The number of rows of the matrix X. M >= 0.

    N       (input) long
            The number of columns of the matrix X. N >= 0.

    X       (input/output) double array, dimension (LDX,N)
            On entry, the M by N matrix X.
            On exit, X contains the permuted matrix X.

    LDX     (input) long
            The leading dimension of the array X, LDX >= MAX(1,M).

    K       (input) long array, dimension (N)
            On entry, K contains the permutation vector.

    =====================================================================
*/

int NUMlapack_dlarf (const char *side, long *m, long *n, double *v, long *incv, double *tau,
	double *c, long *ldc, double *work);
/*
    Purpose
    =======

    NUMlapack_dlarf applies a real elementary reflector H to a real m by n matrix
    C, from either the left or the right. H is represented in the form

          H = I - tau * v * v'

    where tau is a real scalar and v is a real vector.

    If tau = 0, then H is taken to be the unit matrix.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': form  H * C
            = 'R': form  C * H

    M       (input) long
            The number of rows of the matrix C.

    N       (input) long
            The number of columns of the matrix C.

    V       (input) double array, dimension
                       (1 + (M-1)*abs(INCV)) if SIDE = 'L'
                    or (1 + (N-1)*abs(INCV)) if SIDE = 'R'
            The vector v in the representation of H. V is not used if
            TAU = 0.

    INCV    (input) long
            The increment between elements of v. INCV <> 0.

    TAU     (input) double
            The value tau in the representation of H.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by the matrix H * C if SIDE = 'L',
            or C * H if SIDE = 'R'.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) double array, dimension
                           (N) if SIDE = 'L'
                        or (M) if SIDE = 'R'

    =====================================================================
*/

int NUMlapack_dlarfb (const char *side, const char *trans, const char *direct, const char *storev,
	long *m, long *n, long *k, double *v, long *ldv, double *t, long *ldt,
	double *c, long *ldc, double *work, long *ldwork);
/*  Purpose
    =======

    NUMlapack_dlarfb applies a real block reflector H or its transpose H' to a
    real m by n matrix C, from either the left or the right.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply H or H' from the Left
            = 'R': apply H or H' from the Right

    TRANS   (input) char*
            = 'N': apply H (No transpose)
            = 'T': apply H' (Transpose)

    DIRECT  (input) char*
            Indicates how H is formed from a product of elementary
            reflectors
            = 'F': H = H(1) H(2) . . . H(k) (Forward)
            = 'B': H = H(k) . . . H(2) H(1) (Backward)

    STOREV  (input) char*
            Indicates how the vectors which define the elementary
            reflectors are stored:
            = 'C': Columnwise
            = 'R': Rowwise

    M       (input) long
            The number of rows of the matrix C.

    N       (input) long
            The number of columns of the matrix C.

    K       (input) long
            The order of the matrix T (= the number of elementary
            reflectors whose product defines the block reflector).

    V       (input) double array, dimension
                                  (LDV,K) if STOREV = 'C'
                                  (LDV,M) if STOREV = 'R' and SIDE = 'L'
                                  (LDV,N) if STOREV = 'R' and SIDE = 'R'
            The matrix V. See further details.

    LDV     (input) long
            The leading dimension of the array V.
            If STOREV = 'C' and SIDE = 'L', LDV >= max(1,M);
            if STOREV = 'C' and SIDE = 'R', LDV >= max(1,N);
            if STOREV = 'R', LDV >= K.

    T       (input) double array, dimension (LDT,K)
            The triangular k by k matrix T in the representation of the
            block reflector.

    LDT     (input) long
            The leading dimension of the array T. LDT >= K.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by H*C or H'*C or C*H or C*H'.

    LDC     (input) long
            The leading dimension of the array C. LDA >= max(1,M).

    WORK    (workspace) double array, dimension (LDWORK,K)

    LDWORK  (input) long
            The leading dimension of the array WORK.
            If SIDE = 'L', LDWORK >= max(1,N);
            if SIDE = 'R', LDWORK >= max(1,M).

    =====================================================================
*/
int NUMlapack_dlarfg (long *n, double *alpha, double *x, long *incx, double *tau);
/*  Purpose
    =======

    NUMlapack_dlarfg generates a real elementary reflector H of order n, such
    that

          H * ( alpha ) = ( beta ),   H' * H = I.
              (   x   )   (   0  )

    where alpha and beta are scalars, and x is an (n-1)-element real
    vector. H is represented in the form

          H = I - tau * ( 1 ) * ( 1 v' ) ,
                        ( v )

    where tau is a real scalar and v is a real (n-1)-element
    vector.

    If the elements of x are all zero, then tau = 0 and H is taken to be
    the unit matrix.

    Otherwise  1 <= tau <= 2.

    Arguments
    =========

    N       (input) long
            The order of the elementary reflector.

    ALPHA   (input/output) double
            On entry, the value alpha.
            On exit, it is overwritten with the value beta.

    X       (input/output) double array, dimension
                           (1+(N-2)*abs(INCX))
            On entry, the vector x.
            On exit, it is overwritten with the vector v.

    INCX    (input) long
            The increment between elements of X. INCX > 0.

    TAU     (output) double
            The value tau.

    =====================================================================
*/

int NUMlapack_dlarft (const char *direct, const char *storev, long *n, long *k,
	double *v, long *ldv, double *tau, double *t, long *ldt);
/*  Purpose
    =======

    NUMlapack_dlarft forms the triangular factor T of a real block reflector H
    of order n, which is defined as a product of k elementary reflectors.

    If DIRECT = 'F', H = H(1) H(2) . . . H(k) and T is upper triangular;

    If DIRECT = 'B', H = H(k) . . . H(2) H(1) and T is lower triangular.

    If STOREV = 'C', the vector which defines the elementary reflector
    H(i) is stored in the i-th column of the array V, and

       H  =  I - V * T * V'

    If STOREV = 'R', the vector which defines the elementary reflector
    H(i) is stored in the i-th row of the array V, and

       H  =  I - V' * T * V

    Arguments
    =========

    DIRECT  (input) char*
            Specifies the order in which the elementary reflectors are
            multiplied to form the block reflector:
            = 'F': H = H(1) H(2) . . . H(k) (Forward)
            = 'B': H = H(k) . . . H(2) H(1) (Backward)

    STOREV  (input) char*
            Specifies how the vectors which define the elementary
            reflectors are stored (see also Further Details):
            = 'C': columnwise
            = 'R': rowwise

    N       (input) long
            The order of the block reflector H. N >= 0.

    K       (input) long
            The order of the triangular factor T (= the number of
            elementary reflectors). K >= 1.

    V       (input/output) double array, dimension
                                 (LDV,K) if STOREV = 'C'
                                 (LDV,N) if STOREV = 'R'
            The matrix V. See further details.

    LDV     (input) long
            The leading dimension of the array V.
            If STOREV = 'C', LDV >= max(1,N); if STOREV = 'R', LDV >= K.

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i).

    T       (output) double array, dimension (LDT,K)
            The k by k triangular factor T of the block reflector.
            If DIRECT = 'F', T is upper triangular; if DIRECT = 'B', T is
            lower triangular. The rest of the array is not used.

    LDT     (input) long
            The leading dimension of the array T. LDT >= K.

    Further Details
    ===============

    The shape of the matrix V and the storage of the vectors which define
    the H(i) is best illustrated by the following example with n = 5 and
    k = 3. The elements equal to 1 are not stored; the corresponding
    array elements are modified but restored on exit. The rest of the
    array is not used.

    DIRECT = 'F' and STOREV = 'C':         DIRECT = 'F' and STOREV = 'R':

                 V = (  1       )                 V = (  1 v1 v1 v1 v1 )
                     ( v1  1    )                     (     1 v2 v2 v2 )
                     ( v1 v2  1 )                     (        1 v3 v3 )
                     ( v1 v2 v3 )
                     ( v1 v2 v3 )

    DIRECT = 'B' and STOREV = 'C':         DIRECT = 'B' and STOREV = 'R':

                 V = ( v1 v2 v3 )                 V = ( v1 v1  1       )
                     ( v1 v2 v3 )                     ( v2 v2 v2  1    )
                     (  1 v2 v3 )                     ( v3 v3 v3 v3  1 )
                     (     1 v3 )
                     (        1 )

    =====================================================================
*/

int NUMlapack_dlarfx (const char *side, long *m, long *n, double *v, double *tau,
	double *c, long *ldc, double *work);
/*  -- LAPACK auxiliary routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    NUMlapack_dlarfx applies a real elementary reflector H to a real m by n
    matrix C, from either the left or the right. H is represented in the
    form

          H = I - tau * v * v'

    where tau is a real scalar and v is a real vector.

    If tau = 0, then H is taken to be the unit matrix

    This version uses inline code if H has order < 11.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': form  H * C
            = 'R': form  C * H

    M       (input) long
            The number of rows of the matrix C.

    N       (input) long
            The number of columns of the matrix C.

    V       (input) double array, dimension (M) if SIDE = 'L'
                                       or (N) if SIDE = 'R'
            The vector v in the representation of H.

    TAU     (input) double
            The value tau in the representation of H.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by the matrix H * C if SIDE = 'L',
            or C * H if SIDE = 'R'.

    LDC     (input) long
            The leading dimension of the array C. LDA >= (1,M).

    WORK    (workspace) double array, dimension
                        (N) if SIDE = 'L'
                        or (M) if SIDE = 'R'
            WORK is not referenced if H has order < 11.

    =====================================================================
*/

int NUMlapack_dlartg (double *f, double *g, double *cs, double *sn, double *r);
/*  Purpose
    =======

    NUMlapack_dlartg generate a plane rotation so that

       [  CS  SN  ]  .  [ F ]  =  [ R ]   where CS**2 + SN**2 = 1.
       [ -SN  CS  ]     [ G ]     [ 0 ]

    This is a slower, more accurate version of the BLAS1 routine DROTG,
    with the following other differences:
       F and G are unchanged on return.
       If G=0, then CS=1 and SN=0.
       If F=0 and (G .ne. 0), then CS=0 and SN=1 without doing any
          floating point operations (saves work in DBDSQR when
          there are zeros on the diagonal).

    If F exceeds G in magnitude, CS will be positive.

    Arguments
    =========

    F       (input) double
            The first component of vector to be rotated.

    G       (input) double
            The second component of vector to be rotated.

    CS      (output) double
            The cosine of the rotation.

    SN      (output) double
            The sine of the rotation.

    R       (output) double
            The nonzero component of the rotated vector.

    =====================================================================
*/

int NUMlapack_dlas2 (double *f, double *g, double *h,	double *ssmin, double *ssmax);
/*  Purpose
    =======

    NUMlapack_dlas2  computes the singular values of the 2-by-2 matrix
       [  F   G  ]
       [  0   H  ].
    On return, SSMIN is the smaller singular value and SSMAX is the
    larger singular value.

    Arguments
    =========

    F       (input) double
            The (1,1) element of the 2-by-2 matrix.

    G       (input) double
            The (1,2) element of the 2-by-2 matrix.

    H       (input) double
            The (2,2) element of the 2-by-2 matrix.

    SSMIN   (output) double
            The smaller singular value.

    SSMAX   (output) double
            The larger singular value.

    Further Details
    ===============

    Barring over/underflow, all output quantities are correct to within
    a few units in the last place (ulps), even in the absence of a guard
    digit in addition/subtraction.

    In IEEE arithmetic, the code works correctly if one matrix element is
    infinite.

    Overflow will not occur unless the largest singular value itself
    overflows, or is within a few ulps of overflow. (On machines with
    partial overflow, like the Cray, overflow may occur if the largest
    singular value is within a factor of 2 of overflow.)

    Underflow is harmless if underflow is gradual. Otherwise, results
    may correspond to a matrix modified by perturbations of size near
    the underflow threshold.

    ====================================================================
*/

int NUMlapack_dlascl (const char *type, long *kl, long *ku, double *cfrom, double *cto,
	long *m, long *n, double *a, long *lda, long *info);
/*  Purpose
    =======

    NUMlapack_dlascl multiplies the M by N real matrix A by the real scalar
    CTO/CFROM.  This is done without over/underflow as long as the final
    result CTO*A(I,J)/CFROM does not over/underflow. TYPE specifies that
    A may be full, upper triangular, lower triangular, upper Hessenberg,
    or banded.

    Arguments
    =========

    TYPE    (input) char*
            TYPE indices the storage type of the input matrix.
            = 'G':  A is a full matrix.
            = 'L':  A is a lower triangular matrix.
            = 'U':  A is an upper triangular matrix.
            = 'H':  A is an upper Hessenberg matrix.
            = 'B':  A is a symmetric band matrix with lower bandwidth KL
                    and upper bandwidth KU and with the only the lower
                    half stored.
            = 'Q':  A is a symmetric band matrix with lower bandwidth KL
                    and upper bandwidth KU and with the only the upper
                    half stored.
            = 'Z':  A is a band matrix with lower bandwidth KL and upper
                    bandwidth KU.

    KL      (input) long
            The lower bandwidth of A.  Referenced only if TYPE = 'B',
            'Q' or 'Z'.

    KU      (input) long
            The upper bandwidth of A.  Referenced only if TYPE = 'B',
            'Q' or 'Z'.

    CFROM   (input) double
    CTO     (input) double
            The matrix A is multiplied by CTO/CFROM. A(I,J) is computed
            without over/underflow if the final result CTO*A(I,J)/CFROM
            can be represented without over/underflow.  CFROM must be
            nonzero.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,M)
            The matrix to be multiplied by CTO/CFROM.  See TYPE for the
            storage type.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    INFO    (output) long
            0  - successful exit
            <0 - if INFO = -i, the i-th argument had an illegal value.

    =====================================================================
*/

int NUMlapack_dlaset (const char *uplo, long *m, long *n, double *alpha, double *beta,
	double *a, long *lda);
/*  Purpose
    =======

    NUMlapack_dlaset initializes an m-by-n matrix A to BETA on the diagonal and
    ALPHA on the offdiagonals.

    Arguments
    =========

    UPLO    (input) char*
            Specifies the part of the matrix A to be set.
            = 'U':      Upper triangular part is set; the strictly lower
                        triangular part of A is not changed.
            = 'L':      Lower triangular part is set; the strictly upper
                        triangular part of A is not changed.
            Otherwise:  All of the matrix A is set.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    N       (input) long
            The number of columns of the matrix A.  N >= 0.

    ALPHA   (input) double
            The constant to which the offdiagonal elements are to be set.

    BETA    (input) double
            The constant to which the diagonal elements are to be set.

    A       (input/output) double array, dimension (LDA,N)
            On exit, the leading m-by-n submatrix of A is set as follows:

            if UPLO = 'U', A(i,j) = ALPHA, 1<=i<=j-1, 1<=j<=n,
            if UPLO = 'L', A(i,j) = ALPHA, j+1<=i<=m, 1<=j<=n,
            otherwise,     A(i,j) = ALPHA, 1<=i<=m, 1<=j<=n, i.ne.j,

            and, for all UPLO, A(i,i) = BETA, 1<=i<=min(m,n).

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

   =====================================================================
*/

int NUMlapack_dlasq1 (long *n, double *d, double *e, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dlasq1 computes the singular values of a real N-by-N bidiagonal
    matrix with diagonal D and off-diagonal E. The singular values
    are computed to high relative accuracy, in the absence of
    denormalization, underflow and overflow. The algorithm was first
    presented in

    "Accurate singular values and differential qd algorithms" by K. V.
    Fernando and B. N. Parlett, Numer. Math., Vol-67, No. 2, pp. 191-230,
    1994,

    and the present implementation is described in "An implementation of
    the dqds Algorithm (Positive Case)", LAPACK Working Note.

    Arguments
    =========

    N     (input) long
          The number of rows and columns in the matrix. N >= 0.

    D     (input/output) double array, dimension (N)
          On entry, D contains the diagonal elements of the
          bidiagonal matrix whose SVD is desired. On normal exit,
          D contains the singular values in decreasing order.

    E     (input/output) double array, dimension (N)
          On entry, elements E(1:N-1) contain the off-diagonal elements
          of the bidiagonal matrix whose SVD is desired.
          On exit, E is overwritten.

    WORK  (workspace) double array, dimension (4*N)

    INFO  (output) long
          = 0: successful exit
          < 0: if INFO = -i, the i-th argument had an illegal value
          > 0: the algorithm failed
               = 1, a split was marked by a positive value in E
               = 2, current block of Z not diagonalized after 30*N
                    iterations (in inner while loop)
               = 3, termination criterion of outer while loop not met
                    (program created more than N unreduced blocks)

    =====================================================================
*/

int NUMlapack_dlasq2 (long *n, double *z, long *info);
/*  Purpose
    =======

    NUMlapack_dlasq2 computes all the eigenvalues of the symmetric positive
    definite tridiagonal matrix associated with the qd array Z to high
    relative accuracy are computed to high relative accuracy, in the
    absence of denormalization, underflow and overflow.

    To see the relation of Z to the tridiagonal matrix, let L be a
    unit lower bidiagonal matrix with subdiagonals Z(2,4,6,,..) and
    let U be an upper bidiagonal matrix with 1's above and diagonal
    Z(1,3,5,,..). The tridiagonal is L*U or, if you prefer, the
    symmetric tridiagonal to which it is similar.

    Note : NUMlapack_dlasq2 defines a long* (boolean) variable, IEEE, which is true
    on machines which follow ieee-754 floating-point standard in their
    handling of infinities and NaNs, and false otherwise. This variable
    is passed to NUMlapack_dlasq3.

    Arguments
    =========

    N     (input) long
          The number of rows and columns in the matrix. N >= 0.

    Z     (workspace) double array, dimension ( 4*N )
          On entry Z holds the qd array. On exit, entries 1 to N hold
          the eigenvalues in decreasing order, Z( 2*N+1 ) holds the
          trace, and Z( 2*N+2 ) holds the sum of the eigenvalues. If
          N > 2, then Z( 2*N+3 ) holds the iteration count, Z( 2*N+4 )
          holds NDIVS/NIN^2, and Z( 2*N+5 ) holds the percentage of
          shifts that failed.

    INFO  (output) long
          = 0: successful exit
          < 0: if the i-th argument is a scalar and had an illegal
               value, then INFO = -i, if the i-th argument is an
               array and the j-entry had an illegal value, then
               INFO = -(i*100+j)
          > 0: the algorithm failed
                = 1, a split was marked by a positive value in E
                = 2, current block of Z not diagonalized after 30*N
                     iterations (in inner while loop)
                = 3, termination criterion of outer while loop not met
                     (program created more than N unreduced blocks)

    Further Details
    ===============
    Local Variables: I0:N0 defines a current unreduced segment of Z.
    The shifts are accumulated in SIGMA. Iteration count is in ITER.
    Ping-pong is controlled by PP (alternates between 0 and 1).

    =====================================================================
*/

int NUMlapack_dlasq3 (long *i0, long *n0, double *z, long *pp, double *dmin,
	double *sigma, double *desig, double *qmax, long *nfail, long *iter,
	long *ndiv,	long *ieee);
/*  Purpose
    =======

    NUMlapack_dlasq3 checks for deflation, computes a shift (TAU) and calls dqds.
    In case of failure it changes shifts, and tries again until output
    is positive.

    Arguments
    =========

    I0     (input) long
           First index.

    N0     (input) long
           Last index.

    Z      (input) double array, dimension ( 4*N )
           Z holds the qd array.

    PP     (input) long
           PP=0 for ping, PP=1 for pong.

    DMIN   (output) double
           Minimum value of d.

    SIGMA  (output) double
           Sum of shifts used in current segment.

    DESIG  (input/output) double
           Lower order part of SIGMA

    QMAX   (input) double
           Maximum value of q.

    NFAIL  (output) long
           Number of times shift was too big.

    ITER   (output) long
           Number of iterations.

    NDIV   (output) long
           Number of divisions.

    TTYPE  (output) long
           Shift type.

    IEEE   (input) long* (boolean)
           Flag for IEEE or non IEEE arithmetic (passed to DLASQ5).

    =====================================================================
*/

int NUMlapack_dlasq4 (long *i0, long *n0, double *z, long *pp, long *n0in, double *dmin,
	double *dmin1, double *dmin2, double *dn, double *dn1, double *dn2,
	double *tau, long *ttype);
/*  Purpose
    =======

    NUMlapack_dlasq4 computes an approximation TAU to the smallest eigenvalue
    using values of d from the previous transform.

    I0    (input) long
          First index.

    N0    (input) long
          Last index.

    Z     (input) double array, dimension ( 4*N )
          Z holds the qd array.

    PP    (input) long
          PP=0 for ping, PP=1 for pong.

    NOIN  (input) long
          The value of N0 at start of EIGTEST.

    DMIN  (input) double
          Minimum value of d.

    DMIN1 (input) double
          Minimum value of d, excluding D( N0 ).

    DMIN2 (input) double
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (input) double
          d(N)

    DN1   (input) double
          d(N-1)

    DN2   (input) double
          d(N-2)

    TAU   (output) double
          This is the shift.

    TTYPE (output) long
          Shift type.

    Further Details
    ===============
    CNST1 = 9/16

    =====================================================================
*/

int NUMlapack_dlasq5 (long *i0, long *n0, double *z, long *pp, double *tau, double *dmin,
	double *dmin1, double *dmin2, double *dn, double *dnm1, double *dnm2, long *ieee);
/*  Purpose
    =======

    NUMlapack_dlasq5 computes one dqds transform in ping-pong form, one
    version for IEEE machines another for non IEEE machines.

    Arguments
    =========

    I0    (input) long
          First index.

    N0    (input) long
          Last index.

    Z     (input) double array, dimension ( 4*N )
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid
          an extra argument.

    PP    (input) long
          PP=0 for ping, PP=1 for pong.

    TAU   (input) double
          This is the shift.

    DMIN  (output) double
          Minimum value of d.

    DMIN1 (output) double
          Minimum value of d, excluding D( N0 ).

    DMIN2 (output) double
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (output) double
          d(N0), the last value of d.

    DNM1  (output) double
          d(N0-1).

    DNM2  (output) double
          d(N0-2).

    IEEE  (input) long* (boolean)
          Flag for IEEE or non IEEE arithmetic.

    =====================================================================
*/

int NUMlapack_dlasq6 (long *i0, long *n0, double *z, long *pp, double *dmin,
	double *dmin1, double *dmin2, double *dn, double *dnm1, double *dnm2);
/*  Purpose
    =======

    NUMlapack_dlasq6 computes one dqd (shift equal to zero) transform in
    ping-pong form, with protection against underflow and overflow.

    Arguments
    =========

    I0    (input) long
          First index.

    N0    (input) long
          Last index.

    Z     (input) double array, dimension ( 4*N )
          Z holds the qd array. EMIN is stored in Z(4*N0) to avoid
          an extra argument.

    PP    (input) long
          PP=0 for ping, PP=1 for pong.

    DMIN  (output) double
          Minimum value of d.

    DMIN1 (output) double
          Minimum value of d, excluding D( N0 ).

    DMIN2 (output) double
          Minimum value of d, excluding D( N0 ) and D( N0-1 ).

    DN    (output) double
          d(N0), the last value of d.

    DNM1  (output) double
          d(N0-1).

    DNM2  (output) double
          d(N0-2).

    =====================================================================
*/


int NUMlapack_dlasr (const char *side, const char *pivot, const char *direct, long *m,
	 long *n, double *c, double *s, double *a, long *lda);
/*  Purpose
    =======

    NUMlapack_dlasr performs the transformation

       A := P*A,   when SIDE = 'L' or 'l'  (  Left-hand side )

       A := A*P',  when SIDE = 'R' or 'r'  ( Right-hand side )

    where A is an m by n real matrix and P is an orthogonal matrix,
    consisting of a sequence of plane rotations determined by the
    parameters PIVOT and DIRECT as follows ( z = m when SIDE = 'L' or 'l'
    and z = n when SIDE = 'R' or 'r' ):

    When  DIRECT = 'F' or 'f'  ( Forward sequence ) then

       P = P( z - 1 )*...*P( 2 )*P( 1 ),

    and when DIRECT = 'B' or 'b'  ( Backward sequence ) then

       P = P( 1 )*P( 2 )*...*P( z - 1 ),

    where  P( k ) is a plane rotation matrix for the following planes:

       when  PIVOT = 'V' or 'v'  ( Variable pivot ),
          the plane ( k, k + 1 )

       when  PIVOT = 'T' or 't'  ( Top pivot ),
          the plane ( 1, k + 1 )

       when  PIVOT = 'B' or 'b'  ( Bottom pivot ),
          the plane ( k, z )

    c( k ) and s( k )  must contain the  cosine and sine that define the
    matrix  P( k ).  The two by two plane rotation part of the matrix
    P( k ), R( k ), is assumed to be of the form

       R( k ) = (  c( k )  s( k ) ).
                ( -s( k )  c( k ) )

    This version vectorises across rows of the array A when SIDE = 'L'.

    Arguments
    =========

    SIDE    (input) char*
            Specifies whether the plane rotation matrix P is applied to
            A on the left or the right.
            = 'L':  Left, compute A := P*A
            = 'R':  Right, compute A:= A*P'

    DIRECT  (input) char*
            Specifies whether P is a forward or backward sequence of
            plane rotations.
            = 'F':  Forward, P = P( z - 1 )*...*P( 2 )*P( 1 )
            = 'B':  Backward, P = P( 1 )*P( 2 )*...*P( z - 1 )

    PIVOT   (input) char*
            Specifies the plane for which P(k) is a plane rotation
            matrix.
            = 'V':  Variable pivot, the plane (k,k+1)
            = 'T':  Top pivot, the plane (1,k+1)
            = 'B':  Bottom pivot, the plane (k,z)

    M       (input) long
            The number of rows of the matrix A.  If m <= 1, an immediate
            return is effected.

    N       (input) long
            The number of columns of the matrix A.  If n <= 1, an
            immediate return is effected.

    C, S    (input) double arrays, dimension
                    (M-1) if SIDE = 'L'
                    (N-1) if SIDE = 'R'
            c(k) and s(k) contain the cosine and sine that define the
            matrix P(k).  The two by two plane rotation part of the
            matrix P(k), R(k), is assumed to be of the form
            R( k ) = (  c( k )  s( k ) ).
                     ( -s( k )  c( k ) )

    A       (input/output) double array, dimension (LDA,N)
            The m by n matrix A.  On exit, A is overwritten by P*A if
            SIDE = 'R' or by A*P' if SIDE = 'L'.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,M).

    =====================================================================
*/



int NUMlapack_dlasrt (const char *id, long *n, double *d, long *info);
/*  Purpose
    =======

    Sort the numbers in D in increasing order (if ID = 'I') or
    in decreasing order (if ID = 'D' ).

    Use Quick Sort, reverting to Insertion sort on arrays of
    size <= 20. Dimension of STACK limits N to about 2**32.

    Arguments
    =========

    ID      (input) char*
            = 'I': sort D in increasing order;
            = 'D': sort D in decreasing order.

    N       (input) long
            The length of the array D.

    D       (input/output) double array, dimension (N)
            On entry, the array to be sorted.
            On exit, D has been sorted into increasing order
            (D(1) <= ... <= D(N) ) or into decreasing order
            (D(1) >= ... >= D(N) ), depending on ID.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dlassq (long *n, double *x, long *incx, double *scale, double *sumsq);
/*  Purpose
    =======

    NUMlapack_dlassq  returns the values  scl  and  smsq  such that

       ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq,

    where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is
    assumed to be non-negative and  scl  returns the value

       scl = max( scale, abs( x( i ) ) ).

    scale and sumsq must be supplied in SCALE and SUMSQ and
    scl and smsq are overwritten on SCALE and SUMSQ respectively.

    The routine makes only one pass through the vector x.

    Arguments
    =========

    N       (input) long
            The number of elements to be used from the vector X.

    X       (input) double array, dimension (N)
            The vector for which a scaled sum of squares is computed.
               x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n.

    INCX    (input) long
            The increment between successive values of the vector X.
            INCX > 0.

    SCALE   (input/output) double
            On entry, the value  scale  in the equation above.
            On exit, SCALE is overwritten with  scl , the scaling factor
            for the sum of squares.

    SUMSQ   (input/output) double
            On entry, the value  sumsq  in the equation above.
            On exit, SUMSQ is overwritten with  smsq , the basic sum of
            squares from which  scl  has been factored out.

   =====================================================================
*/

int NUMlapack_dlasv2 (double *f, double *g, double *h, double *ssmin,
	double *ssmax, double *snr, double *csr, double *snl, double *csl);
/*  Purpose
    =======

    NUMlapack_dlasv2 computes the singular value decomposition of a 2-by-2
    triangular matrix
       [  F   G  ]
       [  0   H  ].
    On return, abs(SSMAX) is the larger singular value, abs(SSMIN) is the
    smaller singular value, and (CSL,SNL) and (CSR,SNR) are the left and
    right singular vectors for abs(SSMAX), giving the decomposition

       [ CSL  SNL ] [  F   G  ] [ CSR -SNR ]  =  [ SSMAX   0   ]
       [-SNL  CSL ] [  0   H  ] [ SNR  CSR ]     [  0    SSMIN ].

    Arguments
    =========

    F       (input) double
            The (1,1) element of the 2-by-2 matrix.

    G       (input) double
            The (1,2) element of the 2-by-2 matrix.

    H       (input) double
            The (2,2) element of the 2-by-2 matrix.

    SSMIN   (output) double
            abs(SSMIN) is the smaller singular value.

    SSMAX   (output) double
            abs(SSMAX) is the larger singular value.

    SNL     (output) double
    CSL     (output) double
            The vector (CSL, SNL) is a unit left singular vector for the
            singular value abs(SSMAX).

    SNR     (output) double
    CSR     (output) double
            The vector (CSR, SNR) is a unit right singular vector for the
            singular value abs(SSMAX).

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

   =====================================================================
*/

int NUMlapack_dlaswp (long *n, double *a, long *lda, long *k1, long *k2,
	long *ipiv, long *incx);
/*  Purpose
    =======

    NUMlapack_dlaswp performs a series of row interchanges on the matrix A.
    One row interchange is initiated for each of rows K1 through K2 of A.

    Arguments
    =========

    N       (input) long
            The number of columns of the matrix A.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the matrix of column dimension N to which the row
            interchanges will be applied.
            On exit, the permuted matrix.

    LDA     (input) long
            The leading dimension of the array A.

    K1      (input) long
            The first element of IPIV for which a row interchange will
            be done.

    K2      (input) long
            The last element of IPIV for which a row interchange will
            be done.

    IPIV    (input) long array, dimension (M*abs(INCX))
            The vector of pivot indices.  Only the elements in positions
            K1 through K2 of IPIV are accessed.
            IPIV(K) = L implies rows K and L are to be interchanged.

    INCX    (input) long
            The increment between successive values of IPIV.  If IPIV
            is negative, the pivots are applied in reverse order.

   =====================================================================
*/

int NUMlapack_dlatrd (const char *uplo, long *n, long *nb, double *a, long *lda,
	double *e, double *tau, double *w, long *ldw);
/* Purpose =======

	   NUMlapack_dlatrd reduces NB rows and columns of a real symmetric matrix A to
	   symmetric tridiagonal form by an orthogonal similarity transformation
	   Q' * A * Q, and returns the matrices V and W which are needed to apply
	   the transformation to the unreduced part of A.

	   If UPLO = 'U', DLATRD reduces the last NB rows and columns of a matrix,
	   of which the upper triangle is supplied; if UPLO = 'L', DLATRD reduces
	   the first NB rows and columns of a matrix, of which the lower triangle
	   is supplied.

	   This is an auxiliary routine called by NUMlapack_dsytrd.

	   Arguments =========

	   UPLO (input) CHARACTER Specifies whether the upper or lower triangular
	   part of the symmetric matrix A is stored: = 'U': Upper triangular =
	   'L': Lower triangular

	   N (input) long The order of the matrix A.

	   NB (input) long The number of rows and columns to be reduced.

	   A (input/output) double array, dimension (LDA,N) On entry,
	   the symmetric matrix A.  If UPLO = 'U', the leading n-by-n upper
	   triangular part of A contains the upper triangular part of the matrix
	   A, and the strictly lower triangular part of A is not referenced.  If
	   UPLO = 'L', the leading n-by-n lower triangular part of A contains the
	   lower triangular part of the matrix A, and the strictly upper
	   triangular part of A is not referenced. On exit: if UPLO = 'U', the
	   last NB columns have been reduced to tridiagonal form, with the
	   diagonal elements overwriting the diagonal elements of A; the elements
	   above the diagonal with the array TAU, represent the orthogonal matrix
	   Q as a product of elementary reflectors; if UPLO = 'L', the first NB
	   columns have been reduced to tridiagonal form, with the diagonal
	   elements overwriting the diagonal elements of A; the elements below the
	   diagonal with the array TAU, represent the orthogonal matrix Q as a
	   product of elementary reflectors. See Further Details.

	   LDA (input) long The leading dimension of the array A.  LDA >=
	   (1,N).

	   E (output) double array, dimension (N-1) If UPLO = 'U',
	   E(n-nb:n-1) contains the superdiagonal elements of the last NB columns
	   of the reduced matrix; if UPLO = 'L', E(1:nb) contains the subdiagonal
	   elements of the first NB columns of the reduced matrix.

	   TAU (output) double array, dimension (N-1) The scalar factors
	   of the elementary reflectors, stored in TAU(n-nb:n-1) if UPLO = 'U',
	   and in TAU(1:nb) if UPLO = 'L'. See Further Details.

	   W (output) double array, dimension (LDW,NB) The n-by-nb
	   matrix W required to update the unreduced part of A.

	   LDW (input) long The leading dimension of the array W. LDW >=
	   max(1,N).

	   Further Details ===============

	   If UPLO = 'U', the matrix Q is represented as a product of elementary
	   reflectors

	   Q = H(n) H(n-1) . . . H(n-nb+1).

	   Each H(i) has the form

	   H(i) = I - tau * v * v'

	   where tau is a real scalar, and v is a real vector with v(i:n) = 0 and
	   v(i-1) = 1; v(1:i-1) is stored on exit in A(1:i-1,i), and tau in
	   TAU(i-1).

	   If UPLO = 'L', the matrix Q is represented as a product of elementary
	   reflectors

	   Q = H(1) H(2) . . . H(nb).

	   Each H(i) has the form

	   H(i) = I - tau * v * v'

	   where tau is a real scalar, and v is a real vector with v(1:i) = 0 and
	   v(i+1) = 1; v(i+1:n) is stored on exit in A(i+1:n,i), and tau in
	   TAU(i).

	   The elements of the vectors v together form the n-by-nb matrix V which
	   is needed, with W, to apply the transformation to the unreduced part of
	   the matrix, using a symmetric rank-2k update of the form: A := A - V*W'
	   - W*V'.

	   The contents of A on exit are illustrated by the following examples with
	   n = 5 and nb = 2:

	   if UPLO = 'U': if UPLO = 'L':

	   ( a a a v4 v5 ) ( d ) ( a a v4 v5 ) ( 1 d ) ( a 1 v5 ) ( v1 1 a ) ( d 1
	   ) ( v1 v2 a a ) ( d ) ( v1 v2 a a a )

	   where d denotes a diagonal element of the reduced matrix, a denotes an
	   element of the original matrix that is unchanged, and vi denotes an
	   element of the vector defining H(i).

	   =====================================================================
*/

int NUMlapack_dorg2l (long *m, long *n, long *k, double *	a, long *lda, double *tau,
	double *work, long *info);
/*   Purpose
    =======

    NUMlapack_dorg2l generates an m by n real matrix Q with orthonormal columns,
    which is defined as the last n columns of a product of k elementary
    reflectors of order m

          Q  =  H(k) . . . H(2) H(1)

    as returned by NUMlapack_dgeqlf.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the (n-k+i)-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQLF in the last k columns of its array
            argument A.
            On exit, the m by n matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQLF.

    WORK    (workspace) double array, dimension (N)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorg2r (long *m, long *n, long *k, double *a, long *lda,
	double *tau, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dorg2r generates an m by n real matrix Q with orthonormal columns,
    which is defined as the first n columns of a product of k elementary
    reflectors of order m

          Q  =  H(1) H(2) . . . H(k)

    as returned by NUMlapack_dgeqrf.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the i-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQRF in the first k columns of its array
            argument A.
            On exit, the m-by-n matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    WORK    (workspace) double array, dimension (N)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorgbr (const char *vect, long *m, long *n, long *k, double *a, long *lda,
	double *tau, double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dorgbr generates one of the real orthogonal matrices Q or P**T
    determined by DGEBRD when reducing a real matrix A to bidiagonal
    form: A = Q * B * P**T.  Q and P**T are defined as products of
    elementary reflectors H(i) or G(i) respectively.

    If VECT = 'Q', A is assumed to have been an M-by-K matrix, and Q
    is of order M:
    if m >= k, Q = H(1) H(2) . . . H(k) and NUMlapack_dorgbr returns the first n
    columns of Q, where m >= n >= k;
    if m < k, Q = H(1) H(2) . . . H(m-1) and NUMlapack_dorgbr returns Q as an
    M-by-M matrix.

    If VECT = 'P', A is assumed to have been a K-by-N matrix, and P**T
    is of order N:
    if k < n, P**T = G(k) . . . G(2) G(1) and NUMlapack_dorgbr returns the first m
    rows of P**T, where n >= m >= k;
    if k >= n, P**T = G(n-1) . . . G(2) G(1) and NUMlapack_dorgbr returns P**T as
    an N-by-N matrix.

    Arguments
    =========

    VECT    (input) char*
            Specifies whether the matrix Q or the matrix P**T is
            required, as defined in the transformation applied by DGEBRD:
            = 'Q':  generate Q;
            = 'P':  generate P**T.

    M       (input) long
            The number of rows of the matrix Q or P**T to be returned.
            M >= 0.

    N       (input) long
            The number of columns of the matrix Q or P**T to be returned.
            N >= 0.
            If VECT = 'Q', M >= N >= min(M,K);
            if VECT = 'P', N >= M >= min(N,K).

    K       (input) long
            If VECT = 'Q', the number of columns in the original M-by-K
            matrix reduced by DGEBRD.
            If VECT = 'P', the number of rows in the original K-by-N
            matrix reduced by DGEBRD.
            K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the vectors which define the elementary reflectors,
            as returned by DGEBRD.
            On exit, the M-by-N matrix Q or P**T.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension
                                  (min(M,K)) if VECT = 'Q'
                                  (min(N,K)) if VECT = 'P'
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i) or G(i), which determines Q or P**T, as
            returned by DGEBRD in its array argument TAUQ or TAUP.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= max(1,min(M,N)).
            For optimum performance LWORK >= min(M,N)*NB, where NB
            is the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dorghr (long *n, long *ilo, long *ihi, double *a, long *lda,
	double *tau, double *work, long *lwork, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dorghr generates a real orthogonal matrix Q which is defined as the
    product of IHI-ILO elementary reflectors of order N, as returned by
    NUMlapack_dgehrd:

    Q = H(ilo) H(ilo+1) . . . H(ihi-1).

    Arguments
    =========

    N       (input) long
            The order of the matrix Q. N >= 0.

    ILO     (input) long
    IHI     (input) long
            ILO and IHI must have the same values as in the previous call
            of NUMlapack_dgehrd. Q is equal to the unit matrix except in the
            submatrix Q(ilo+1:ihi,ilo+1:ihi).
            1 <= ILO <= IHI <= N, if N > 0; ILO=1 and IHI=0, if N=0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the vectors which define the elementary reflectors,
            as returned by NUMlapack_dgehrd.
            On exit, the N-by-N orthogonal matrix Q.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,N).

    TAU     (input) double array, dimension (N-1)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dgehrd.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= IHI-ILO.
            For optimum performance LWORK >= (IHI-ILO)*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dorgl2 (long *m, long *n, long *k, double *a, long *lda, double *tau,
	double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dorgl2 generates an m by n real matrix Q with orthonormal rows,
    which is defined as the first m rows of a product of k elementary
    reflectors of order n

          Q  =  H(k) . . . H(2) H(1)

    as returned by  NUMlapack_dgelqf.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. N >= M.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. M >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the i-th row must contain the vector which defines
            the elementary reflector H(i), for i = 1,2,...,k, as returned
            by NUMlapack_dgelqf in the first k rows of its array argument A.
            On exit, the m-by-n matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dgelqf.

    WORK    (workspace) double array, dimension (M)

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorglq (long *m, long *n, long *k, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dorglq generates an M-by-N real matrix Q with orthonormal rows,
    which is defined as the first M rows of a product of K elementary
    reflectors of order N

          Q  =  H(k) . . . H(2) H(1)

    as returned by DGELQf.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. N >= M.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. M >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the i-th row must contain the vector which defines
            the elementary reflector H(i), for i = 1,2,...,k, as returned
            by NUMlapack_dgelqf in the first k rows of its array argument A.
            On exit, the M-by-N matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dgelqf.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= max(1,M).
            For optimum performance LWORK >= M*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorgql (long *m, long *n, long *k, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dorgql generates an M-by-N real matrix Q with orthonormal columns,
    which is defined as the last N columns of a product of K elementary
    reflectors of order M

          Q  =  H(k) . . . H(2) H(1)

    as returned by DGEQLF.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the (n-k+i)-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQLF in the last k columns of its array
            argument A.
            On exit, the M-by-N matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQLF.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorgqr (long *m, long *n, long *k, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dorgqr generates an M-by-N real matrix Q with orthonormal columns,
    which is defined as the first N columns of a product of K elementary
    reflectors of order M

          Q  =  H(1) H(2) . . . H(k)

    as returned by DGEQRF.

    Arguments
    =========

    M       (input) long
            The number of rows of the matrix Q. M >= 0.

    N       (input) long
            The number of columns of the matrix Q. M >= N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines the
            matrix Q. N >= K >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the i-th column must contain the vector which
            defines the elementary reflector H(i), for i = 1,2,...,k, as
            returned by DGEQRF in the first k columns of its array
            argument A.
            On exit, the M-by-N matrix Q.

    LDA     (input) long
            The first dimension of the array A. LDA >= max(1,M).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= max(1,N).
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument has an illegal value

    =====================================================================
*/

int NUMlapack_dorgtr (const char *uplo, long *n, double *a, long *lda, double *tau,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dorgtr generates a real orthogonal matrix Q which is defined as the
    product of n-1 elementary reflectors of order N, as returned by
    NUMlapack_dsytrd:

    if UPLO = 'U', Q = H(n-1) . . . H(2) H(1),

    if UPLO = 'L', Q = H(1) H(2) . . . H(n-1).

    Arguments
    =========

    UPLO    (input) char*
            = 'U': Upper triangle of A contains elementary reflectors
                   from NUMlapack_dsytrd;
            = 'L': Lower triangle of A contains elementary reflectors
                   from NUMlapack_dsytrd.

    N       (input) long
            The order of the matrix Q. N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the vectors which define the elementary reflectors,
            as returned by NUMlapack_dsytrd.
            On exit, the N-by-N orthogonal matrix Q.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,N).

    TAU     (input) double array, dimension (N-1)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dsytrd.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK. LWORK >= max(1,N-1).
            For optimum performance LWORK >= (N-1)*NB, where NB is
            the optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dorm2r (const char *side, const char *trans, long *m, long *n, long *k,
	double *a, long *lda, double *tau, double *c, long *ldc, double *work,
	long *info);
/*  Purpose
    =======

    NUMlapack_dorm2r overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(1) H(2) . . . H(k)

    as returned by DGEQRF. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) char*
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) double array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQRF in the first k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) long
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) double array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dormbr (const char *vect, const char *side, const char *trans, long *m, long *n,
	long *k, double *a, long *lda, double *tau, double *c, long *ldc,
	double *work, long *lwork, long *info);
/*  Purpose
    =======

    If VECT = 'Q', NUMlapack_dormbr overwrites the general real M-by-N matrix C
    with
                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    If VECT = 'P', NUMlapack_dormbr overwrites the general real M-by-N matrix C
    with
                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      P * C          C * P
    TRANS = 'T':      P**T * C       C * P**T

    Here Q and P**T are the orthogonal matrices determined by DGEBRD when
    reducing a real matrix A to bidiagonal form: A = Q * B * P**T. Q and
    P**T are defined as products of elementary reflectors H(i) and G(i)
    respectively.

    Let nq = m if SIDE = 'L' and nq = n if SIDE = 'R'. Thus nq is the
    order of the orthogonal matrix Q or P**T that is applied.

    If VECT = 'Q', A is assumed to have been an NQ-by-K matrix:
    if nq >= k, Q = H(1) H(2) . . . H(k);
    if nq < k, Q = H(1) H(2) . . . H(nq-1).

    If VECT = 'P', A is assumed to have been a K-by-NQ matrix:
    if k < nq, P = G(1) G(2) . . . G(k);
    if k >= nq, P = G(1) G(2) . . . G(nq-1).

    Arguments
    =========

    VECT    (input) char*
            = 'Q': apply Q or Q**T;
            = 'P': apply P or P**T.

    SIDE    (input) char*
            = 'L': apply Q, Q**T, P or P**T from the Left;
            = 'R': apply Q, Q**T, P or P**T from the Right.

    TRANS   (input) char*
            = 'N':  No transpose, apply Q  or P;
            = 'T':  Transpose, apply Q**T or P**T.

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            If VECT = 'Q', the number of columns in the original
            matrix reduced by DGEBRD.
            If VECT = 'P', the number of rows in the original
            matrix reduced by DGEBRD.
            K >= 0.

    A       (input) double array, dimension
                                  (LDA,min(nq,K)) if VECT = 'Q'
                                  (LDA,nq)        if VECT = 'P'
            The vectors which define the elementary reflectors H(i) and
            G(i), whose products determine the matrices Q and P, as
            returned by DGEBRD.

    LDA     (input) long
            The leading dimension of the array A.
            If VECT = 'Q', LDA >= max(1,nq);
            if VECT = 'P', LDA >= max(1,min(nq,K)).

    TAU     (input) double array, dimension (min(nq,K))
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i) or G(i) which determines Q or P, as returned
            by DGEBRD in the array argument TAUQ or TAUP.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q
            or P*C or P**T*C or C*P or C*P**T.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dorml2 (const char *side, const char *trans, long *m, long *n, long *k, double *a,
	long *lda, double *tau, double *c, long *ldc, double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dorml2 overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by NUMlapack_dgelqf. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) char*
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) double array, dimension
                                 (LDA,M) if SIDE = 'L',
                                 (LDA,N) if SIDE = 'R'
            The i-th row must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            NUMlapack_dgelqf in the first k rows of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,K).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dgelqf.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) double array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dormlq (const char *side, const char *trans, long *m, long *n,
	long *k, double *a, long *lda, double *tau, double *c,
	long *ldc, double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dormlq overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(k) . . . H(2) H(1)

    as returned by NUMlapack_dgelqf. Q is of order M if SIDE = 'L' and of order N
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    TRANS   (input) char*
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) double array, dimension
                                 (LDA,M) if SIDE = 'L',
                                 (LDA,N) if SIDE = 'R'
            The i-th row must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            NUMlapack_dgelqf in the first k rows of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,K).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by NUMlapack_dgelqf.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dormqr (const char *side, const char *trans, long *m, long *n, long *k,
	double *a, long *lda, double *tau, double *c, long *ldc, double *work,
	long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dormqr overwrites the general real M-by-N matrix C with

                    SIDE = 'L'     SIDE = 'R'
    TRANS = 'N':      Q * C          C * Q
    TRANS = 'T':      Q**T * C       C * Q**T

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(1) H(2) . . . H(k)

    as returned by DGEQRF. Q is of order M if SIDE = 'L' and of order N
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply Q or Q**T from the Left;
            = 'R': apply Q or Q**T from the Right.

    TRANS   (input) char*
            = 'N':  No transpose, apply Q;
            = 'T':  Transpose, apply Q**T.

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) double array, dimension (LDA,K)
            The i-th column must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGEQRF in the first k columns of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) long
            The leading dimension of the array A.
            If SIDE = 'L', LDA >= max(1,M);
            if SIDE = 'R', LDA >= max(1,N).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGEQRF.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the M-by-N matrix C.
            On exit, C is overwritten by Q*C or Q**T*C or C*Q**T or C*Q.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.
            If SIDE = 'L', LWORK >= max(1,N);
            if SIDE = 'R', LWORK >= max(1,M).
            For optimum performance LWORK >= N*NB if SIDE = 'L', and
            LWORK >= M*NB if SIDE = 'R', where NB is the optimal
            blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dormr2 (const char *side, const char *trans, long *m, long *n, long *k,
	double *a, long *lda, double *tau, double *c, long *ldc, double *work,
	long *info);
/*  Purpose
    =======

    NUMlapack_dormr2 overwrites the general real m by n matrix C with

          Q * C  if SIDE = 'L' and TRANS = 'N', or

          Q'* C  if SIDE = 'L' and TRANS = 'T', or

          C * Q  if SIDE = 'R' and TRANS = 'N', or

          C * Q' if SIDE = 'R' and TRANS = 'T',

    where Q is a real orthogonal matrix defined as the product of k
    elementary reflectors

          Q = H(1) H(2) . . . H(k)

    as returned by DGERQF. Q is of order m if SIDE = 'L' and of order n
    if SIDE = 'R'.

    Arguments
    =========

    SIDE    (input) char*
            = 'L': apply Q or Q' from the Left
            = 'R': apply Q or Q' from the Right

    TRANS   (input) char*
            = 'N': apply Q  (No transpose)
            = 'T': apply Q' (Transpose)

    M       (input) long
            The number of rows of the matrix C. M >= 0.

    N       (input) long
            The number of columns of the matrix C. N >= 0.

    K       (input) long
            The number of elementary reflectors whose product defines
            the matrix Q.
            If SIDE = 'L', M >= K >= 0;
            if SIDE = 'R', N >= K >= 0.

    A       (input) double array, dimension
                                 (LDA,M) if SIDE = 'L',
                                 (LDA,N) if SIDE = 'R'
            The i-th row must contain the vector which defines the
            elementary reflector H(i), for i = 1,2,...,k, as returned by
            DGERQF in the last k rows of its array argument A.
            A is modified by the routine but restored on exit.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,K).

    TAU     (input) double array, dimension (K)
            TAU(i) must contain the scalar factor of the elementary
            reflector H(i), as returned by DGERQF.

    C       (input/output) double array, dimension (LDC,N)
            On entry, the m by n matrix C.
            On exit, C is overwritten by Q*C or Q'*C or C*Q' or C*Q.

    LDC     (input) long
            The leading dimension of the array C. LDC >= max(1,M).

    WORK    (workspace) double array, dimension
                                     (N) if SIDE = 'L',
                                     (M) if SIDE = 'R'

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dpotf2 (const char *uplo, long *n, double *a, long *lda, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       February 29, 1992


    Purpose
    =======

    NUMlapack_dpotf2 computes the Cholesky factorization of a real symmetric
    positive definite matrix A.

    The factorization has the form
       A = U' * U ,  if UPLO = 'U', or
       A = L  * L',  if UPLO = 'L',
    where U is an upper triangular matrix and L is lower triangular.

    This is the unblocked version of the algorithm, calling Level 2 BLAS.

    Arguments
    =========

    UPLO    (input) char *
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is stored.
            = 'U':  Upper triangular
            = 'L':  Lower triangular

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            n by n upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n by n lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.

            On exit, if INFO = 0, the factor U or L from the Cholesky
            factorization A = U'*U  or A = L*L'.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -k, the k-th argument had an illegal value
            > 0: if INFO = k, the leading minor of order k is not
                 positive definite, and the factorization could not be
                 completed.

*/

int NUMlapack_drscl (long *n, double *sa, double *sx,	long *incx);
/*  Purpose
    =======

    NUMlapack_drscl multiplies an n-element real vector x by the real scalar 1/a.
    This is done without overflow or underflow as long as
    the final result x/a does not overflow or underflow.

    Arguments
    =========

    N       (input) long
            The number of components of the vector x.

    SA      (input) double
            The scalar a which is used to divide each component of x.
            SA must be >= 0, or the subroutine will divide by zero.

    SX      (input/output) double array, dimension
                           (1+(N-1)*abs(INCX))
            The n-element vector x.

    INCX    (input) long
            The increment between successive values of the vector SX.
            > 0:  SX(1) = X(1) and SX(1+(i-1)*INCX) = x(i),     1< i<= n

   =====================================================================
*/


int NUMlapack_dsteqr (const char *compz, long *n, double *d, double *e, double *z, long *ldz,
	double *work, long *info);
/*  Purpose
    =======

    NUMlapack_dsteqr computes all eigenvalues and, optionally, eigenvectors of a
    symmetric tridiagonal matrix using the implicit QL or QR method.
    The eigenvectors of a full or band symmetric matrix can also be found
    if NUMlapack_dsytrd or DSPTRD or DSBTRD has been used to reduce this matrix to
    tridiagonal form.

    Arguments
    =========

    COMPZ   (input) char*
            = 'N':  Compute eigenvalues only.
            = 'V':  Compute eigenvalues and eigenvectors of the original
                    symmetric matrix.  On entry, Z must contain the
                    orthogonal matrix used to reduce the original matrix
                    to tridiagonal form.
            = 'I':  Compute eigenvalues and eigenvectors of the
                    tridiagonal matrix.  Z is initialized to the identity
                    matrix.

    N       (input) long
            The order of the matrix.  N >= 0.

    D       (input/output) double array, dimension (N)
            On entry, the diagonal elements of the tridiagonal matrix.
            On exit, if INFO = 0, the eigenvalues in ascending order.

    E       (input/output) double array, dimension (N-1)
            On entry, the (n-1) subdiagonal elements of the tridiagonal
            matrix.
            On exit, E has been destroyed.

    Z       (input/output) double array, dimension (LDZ, N)
            On entry, if  COMPZ = 'V', then Z contains the orthogonal
            matrix used in the reduction to tridiagonal form.
            On exit, if INFO = 0, then if  COMPZ = 'V', Z contains the
            orthonormal eigenvectors of the original symmetric matrix,
            and if COMPZ = 'I', Z contains the orthonormal eigenvectors
            of the symmetric tridiagonal matrix.
            If COMPZ = 'N', then Z is not referenced.

    LDZ     (input) long
            The leading dimension of the array Z.  LDZ >= 1, and if
            eigenvectors are desired, then  LDZ >= max(1,N).

    WORK    (workspace) double array, dimension (max(1,2*N-2))
            If COMPZ = 'N', then WORK is not referenced.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm has failed to find all the eigenvalues in
                  a total of 30*N iterations; if INFO = i, then i
                  elements of E have not converged to zero; on exit, D
                  and E contain the elements of a symmetric tridiagonal
                  matrix which is orthogonally similar to the original
                  matrix.

    =====================================================================
*/


int NUMlapack_dsterf (long *n, double *d, double *e, long *info);
/*  Purpose
    =======

    NUMlapack_dsterf computes all eigenvalues of a symmetric tridiagonal matrix
    using the Pal-Walker-Kahan variant of the QL or QR algorithm.

    Arguments
    =========

    N       (input) long
            The order of the matrix.  N >= 0.

    D       (input/output) double array, dimension (N)
            On entry, the n diagonal elements of the tridiagonal matrix.
            On exit, if INFO = 0, the eigenvalues in ascending order.

    E       (input/output) double array, dimension (N-1)
            On entry, the (n-1) subdiagonal elements of the tridiagonal
            matrix.
            On exit, E has been destroyed.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  the algorithm failed to find all of the eigenvalues in
                  a total of 30*N iterations; if INFO = i, then i
                  elements of E have not converged to zero.

    =====================================================================
*/


int NUMlapack_dsyev (const char *jobz, const char *uplo, long *n, double *a,	long *lda,
	double *w, double *work, long *lwork, long *info);
/* Purpose =======

	NUMlapack_dsyev computes all eigenvalues and, optionally, eigenvectors of a
	real symmetric matrix A.

    Arguments
    =========

    JOBZ    (input) char*
            = 'N':  Compute eigenvalues only;
            = 'V':  Compute eigenvalues and eigenvectors.

    UPLO    (input) char*
            = 'U':  Upper triangle of A is stored;
            = 'L':  Lower triangle of A is stored.

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA, N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the
            leading N-by-N upper triangular part of A contains the
            upper triangular part of the matrix A.  If UPLO = 'L',
            the leading N-by-N lower triangular part of A contains
            the lower triangular part of the matrix A.
            On exit, if JOBZ = 'V', then if INFO = 0, A contains the
            orthonormal eigenvectors of the matrix A.
            If JOBZ = 'N', then on exit the lower triangle (if UPLO='L')
            or the upper triangle (if UPLO='U') of A, including the
            diagonal, is destroyed.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    W       (output) double array, dimension (N)
            If INFO = 0, the eigenvalues in ascending order.

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The length of the array WORK.  LWORK >= max(1,3*N-1).
            For optimal efficiency, LWORK >= (NB+2)*N,
            where NB is the blocksize for NUMlapack_dsytrd returned by NUMlapack_ilaenv.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value
            > 0:  if INFO = i, the algorithm failed to converge; i
                  off-diagonal elements of an intermediate tridiagonal
                  form did not converge to zero.

	=====================================================================
*/


int NUMlapack_dsytd2 (const char *uplo, long *n, double *a, long *lda, double *d,
	double *e, double *tau, long *info);
/*  Purpose
    =======

    NUMlapack_dsytd2 reduces a real symmetric matrix A to symmetric tridiagonal
    form T by an orthogonal similarity transformation: Q' * A * Q = T.

    Arguments
    =========

    UPLO    (input) char*
            Specifies whether the upper or lower triangular part of the
            symmetric matrix A is stored:
            = 'U':  Upper triangular
            = 'L':  Lower triangular

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            n-by-n upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n-by-n lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.
            On exit, if UPLO = 'U', the diagonal and first superdiagonal
            of A are overwritten by the corresponding elements of the
            tridiagonal matrix T, and the elements above the first
            superdiagonal, with the array TAU, represent the orthogonal
            matrix Q as a product of elementary reflectors; if UPLO
            = 'L', the diagonal and first subdiagonal of A are over-
            written by the corresponding elements of the tridiagonal
            matrix T, and the elements below the first subdiagonal, with
            the array TAU, represent the orthogonal matrix Q as a product
            of elementary reflectors. See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    D       (output) double array, dimension (N)
            The diagonal elements of the tridiagonal matrix T:
            D(i) = A(i,i).

    E       (output) double array, dimension (N-1)
            The off-diagonal elements of the tridiagonal matrix T:
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.

    TAU     (output) double array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.

    Further Details
    ===============

    If UPLO = 'U', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(n-1) . . . H(2) H(1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in
    A(1:i-1,i+1), and tau in TAU(i).

    If UPLO = 'L', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(1) H(2) . . . H(n-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),
    and tau in TAU(i).

    The contents of A on exit are illustrated by the following examples
    with n = 5:

    if UPLO = 'U':                       if UPLO = 'L':

      (  d   e   v2  v3  v4 )              (  d                  )
      (      d   e   v3  v4 )              (  e   d              )
      (          d   e   v4 )              (  v1  e   d          )
      (              d   e  )              (  v1  v2  e   d      )
      (                  d  )              (  v1  v2  v3  e   d  )

    where d and e denote diagonal and off-diagonal elements of T, and vi
    denotes an element of the vector defining H(i).

    =====================================================================
*/

int NUMlapack_dsytrd (const char *uplo, long *n, double *a, long *lda, double *d,
	double *e, double *tau, double *work, long *lwork, long *info);
/*  Purpose
    =======

    NUMlapack_dsytrd reduces a real symmetric matrix A to real symmetric
    tridiagonal form T by an orthogonal similarity transformation:
    Q**T * A * Q = T.

    Arguments
    =========

    UPLO    (input) char*
            = 'U':  Upper triangle of A is stored;
            = 'L':  Lower triangle of A is stored.

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the symmetric matrix A.  If UPLO = 'U', the leading
            N-by-N upper triangular part of A contains the upper
            triangular part of the matrix A, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading N-by-N lower triangular part of A contains the lower
            triangular part of the matrix A, and the strictly upper
            triangular part of A is not referenced.
            On exit, if UPLO = 'U', the diagonal and first superdiagonal
            of A are overwritten by the corresponding elements of the
            tridiagonal matrix T, and the elements above the first
            superdiagonal, with the array TAU, represent the orthogonal
            matrix Q as a product of elementary reflectors; if UPLO
            = 'L', the diagonal and first subdiagonal of A are over-
            written by the corresponding elements of the tridiagonal
            matrix T, and the elements below the first subdiagonal, with
            the array TAU, represent the orthogonal matrix Q as a product
            of elementary reflectors. See Further Details.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    D       (output) double array, dimension (N)
            The diagonal elements of the tridiagonal matrix T:
            D(i) = A(i,i).

    E       (output) double array, dimension (N-1)
            The off-diagonal elements of the tridiagonal matrix T:
            E(i) = A(i,i+1) if UPLO = 'U', E(i) = A(i+1,i) if UPLO = 'L'.

    TAU     (output) double array, dimension (N-1)
            The scalar factors of the elementary reflectors (see Further
            Details).

    WORK    (workspace/output) double array, dimension (LWORK)
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK.

    LWORK   (input) long
            The dimension of the array WORK.  LWORK >= 1.
            For optimum performance LWORK >= N*NB, where NB is the
            optimal blocksize.

            If LWORK = -1, then a workspace query is assumed; the routine
            only calculates the optimal size of the WORK array, returns
            this value as the first entry of the WORK array, and no error
            message related to LWORK is issued by XERBLA.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    If UPLO = 'U', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(n-1) . . . H(2) H(1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(i+1:n) = 0 and v(i) = 1; v(1:i-1) is stored on exit in
    A(1:i-1,i+1), and tau in TAU(i).

    If UPLO = 'L', the matrix Q is represented as a product of elementary
    reflectors

       Q = H(1) H(2) . . . H(n-1).

    Each H(i) has the form

       H(i) = I - tau * v * v'

    where tau is a real scalar, and v is a real vector with
    v(1:i) = 0 and v(i+1) = 1; v(i+2:n) is stored on exit in A(i+2:n,i),
    and tau in TAU(i).

    The contents of A on exit are illustrated by the following examples
    with n = 5:

    if UPLO = 'U':                       if UPLO = 'L':

      (  d   e   v2  v3  v4 )              (  d                  )
      (      d   e   v3  v4 )              (  e   d              )
      (          d   e   v4 )              (  v1  e   d          )
      (              d   e  )              (  v1  v2  e   d      )
      (                  d  )              (  v1  v2  v3  e   d  )

    where d and e denote diagonal and off-diagonal elements of T, and vi
    denotes an element of the vector defining H(i).

    =====================================================================
*/

int NUMlapack_dtgsja(const char *jobu, const char *jobv, const char *jobq, long *m, long *p,
	long *n, long *k, long *l, double *a, long *lda, double *b, long *ldb,
	double *tola, double *tolb, double *alpha, double *beta, double *u,
	long *ldu, double *v, long *ldv, double *q, long *ldq, double *work,
	long *ncycle, long *info);
/*  Purpose
    =======

    NUMlapack_dtgsja computes the generalized singular value decomposition (GSVD)
    of two real upper triangular (or trapezoidal) matrices A and B.

    On entry, it is assumed that matrices A and B have the following
    forms, which may be obtained by the preprocessing subroutine NUMlapack_dggsvp
    from a general M-by-N matrix A and P-by-N matrix B:

                 N-K-L  K    L
       A =    K ( 0    A12  A13 ) if M-K-L >= 0;
              L ( 0     0   A23 )
          M-K-L ( 0     0    0  )

               N-K-L  K    L
       A =  K ( 0    A12  A13 ) if M-K-L < 0;
          M-K ( 0     0   A23 )

               N-K-L  K    L
       B =  L ( 0     0   B13 )
          P-L ( 0     0    0  )

    where the K-by-K matrix A12 and L-by-L matrix B13 are nonsingular
    upper triangular; A23 is L-by-L upper triangular if M-K-L >= 0,
    otherwise A23 is (M-K)-by-L upper trapezoidal.

    On exit,

                U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R ),

    where U, V and Q are orthogonal matrices, Z' denotes the transpose
    of Z, R is a nonsingular upper triangular matrix, and D1 and D2 are
    ``diagonal'' matrices, which are of the following structures:

    If M-K-L >= 0,

                        K  L
           D1 =     K ( I  0 )
                    L ( 0  C )
                M-K-L ( 0  0 )

                      K  L
           D2 = L   ( 0  S )
                P-L ( 0  0 )

                   N-K-L  K    L
      ( 0 R ) = K (  0   R11  R12 ) K
                L (  0    0   R22 ) L

    where

      C = diag( ALPHA(K+1), ... , ALPHA(K+L) ),
      S = diag( BETA(K+1),  ... , BETA(K+L) ),
      C**2 + S**2 = I.

      R is stored in A(1:K+L,N-K-L+1:N) on exit.

    If M-K-L < 0,

                   K M-K K+L-M
        D1 =   K ( I  0    0   )
             M-K ( 0  C    0   )

                     K M-K K+L-M
        D2 =   M-K ( 0  S    0   )
             K+L-M ( 0  0    I   )
               P-L ( 0  0    0   )

                   N-K-L  K   M-K  K+L-M
   ( 0 R ) =    K ( 0    R11  R12  R13  )
              M-K ( 0     0   R22  R23  )
            K+L-M ( 0     0    0   R33  )

    where
    C = diag( ALPHA(K+1), ... , ALPHA(M) ),
    S = diag( BETA(K+1),  ... , BETA(M) ),
    C**2 + S**2 = I.

    R = ( R11 R12 R13 ) is stored in A(1:M, N-K-L+1:N) and R33 is stored
        (  0  R22 R23 )
    in B(M-K+1:L,N+M-K-L+1:N) on exit.

    The computation of the orthogonal transformation matrices U, V or Q
    is optional.  These matrices may either be formed explicitly, or they
    may be postmultiplied into input matrices U1, V1, or Q1.

    Arguments
    =========

    JOBU    (input) char*
            = 'U':  U must contain an orthogonal matrix U1 on entry, and
                    the product U1*U is returned;
            = 'I':  U is initialized to the unit matrix, and the
                    orthogonal matrix U is returned;
            = 'N':  U is not computed.

    JOBV    (input) char*
            = 'V':  V must contain an orthogonal matrix V1 on entry, and
                    the product V1*V is returned;
            = 'I':  V is initialized to the unit matrix, and the
                    orthogonal matrix V is returned;
            = 'N':  V is not computed.

    JOBQ    (input) char*
            = 'Q':  Q must contain an orthogonal matrix Q1 on entry, and
                    the product Q1*Q is returned;
            = 'I':  Q is initialized to the unit matrix, and the
                    orthogonal matrix Q is returned;
            = 'N':  Q is not computed.

    M       (input) long
            The number of rows of the matrix A.  M >= 0.

    P       (input) long
            The number of rows of the matrix B.  P >= 0.

    N       (input) long
            The number of columns of the matrices A and B.  N >= 0.

    K       (input) long
    L       (input) long
            K and L specify the subblocks in the input matrices A and B:
            A23 = A(K+1:MIN(K+L,M),N-L+1:N) and B13 = B(1:L,N-L+1:N)
            of A and B, whose GSVD is going to be computed by NUMlapack_dtgsja.
            See Further details.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the M-by-N matrix A.
            On exit, A(N-K+1:N,1:MIN(K+L,M) ) contains the triangular
            matrix R or part of R.  See Purpose for details.

    LDA     (input) long
            The leading dimension of the array A. LDA >= max(1,M).

    B       (input/output) double array, dimension (LDB,N)
            On entry, the P-by-N matrix B.
            On exit, if necessary, B(M-K+1:L,N+M-K-L+1:N) contains
            a part of R.  See Purpose for details.

    LDB     (input) long
            The leading dimension of the array B. LDB >= max(1,P).

    TOLA    (input) double
    TOLB    (input) double
            TOLA and TOLB are the convergence criteria for the Jacobi-
            Kogbetliantz iteration procedure. Generally, they are the
            same as used in the preprocessing step, say
                TOLA = max(M,N)*norm(A)*MAZHEPS,
                TOLB = max(P,N)*norm(B)*MAZHEPS.

    ALPHA   (output) double array, dimension (N)
    BETA    (output) double array, dimension (N)
            On exit, ALPHA and BETA contain the generalized singular
            value pairs of A and B;
              ALPHA(1:K) = 1,
              BETA(1:K)  = 0,
            and if M-K-L >= 0,
              ALPHA(K+1:K+L) = diag(C),
              BETA(K+1:K+L)  = diag(S),
            or if M-K-L < 0,
              ALPHA(K+1:M)= C, ALPHA(M+1:K+L)= 0
              BETA(K+1:M) = S, BETA(M+1:K+L) = 1.
            Furthermore, if K+L < N,
              ALPHA(K+L+1:N) = 0 and
              BETA(K+L+1:N)  = 0.

    U       (input/output) double array, dimension (LDU,M)
            On entry, if JOBU = 'U', U must contain a matrix U1 (usually
            the orthogonal matrix returned by NUMlapack_dggsvp).
            On exit,
            if JOBU = 'I', U contains the orthogonal matrix U;
            if JOBU = 'U', U contains the product U1*U.
            If JOBU = 'N', U is not referenced.

    LDU     (input) long
            The leading dimension of the array U. LDU >= max(1,M) if
            JOBU = 'U'; LDU >= 1 otherwise.

    V       (input/output) double array, dimension (LDV,P)
            On entry, if JOBV = 'V', V must contain a matrix V1 (usually
            the orthogonal matrix returned by NUMlapack_dggsvp).
            On exit,
            if JOBV = 'I', V contains the orthogonal matrix V;
            if JOBV = 'V', V contains the product V1*V.
            If JOBV = 'N', V is not referenced.

    LDV     (input) long
            The leading dimension of the array V. LDV >= max(1,P) if
            JOBV = 'V'; LDV >= 1 otherwise.

    Q       (input/output) double array, dimension (LDQ,N)
            On entry, if JOBQ = 'Q', Q must contain a matrix Q1 (usually
            the orthogonal matrix returned by NUMlapack_dggsvp).
            On exit,
            if JOBQ = 'I', Q contains the orthogonal matrix Q;
            if JOBQ = 'Q', Q contains the product Q1*Q.
            If JOBQ = 'N', Q is not referenced.

    LDQ     (input) long
            The leading dimension of the array Q. LDQ >= max(1,N) if
            JOBQ = 'Q'; LDQ >= 1 otherwise.

    WORK    (workspace) double array, dimension (2*N)

    NCYCLE  (output) long
            The number of cycles required for convergence.

    INFO    (output) long
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value.
            = 1:  the procedure does not converge after MAXIT cycles.

    Internal Parameters
    ===================

    MAXIT   long
            MAXIT specifies the total loops that the iterative procedure
            may take. If after MAXIT cycles, the routine fails to
            converge, we return INFO = 1.

    Further Details
    ===============

    NUMlapack_dtgsja essentially uses a variant of Kogbetliantz algorithm to reduce
    min(L,M-K)-by-L triangular (or trapezoidal) matrix A23 and L-by-L
    matrix B13 to the form:

             U1'*A13*Q1 = C1*R1; V1'*B13*Q1 = S1*R1,

    where U1, V1 and Q1 are orthogonal matrix, and Z' is the transpose
    of Z.  C1 and S1 are diagonal matrices satisfying

                  C1**2 + S1**2 = I,

    and R1 is an L-by-L nonsingular upper triangular matrix.

    =====================================================================
*/

int NUMlapack_dtrevc (const char *side, const char *howmny, int * select, long *n,
	double *t, long *ldt, double *vl, long *ldvl, double *vr, long *ldvr,
	long *mm, long *m, double *work, long *info);
/*  -- LAPACK routine (version 3.0) --
       Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,
       Courant Institute, Argonne National Lab, and Rice University
       June 30, 1999


    Purpose
    =======

    NUMlapack_dtrevc computes some or all of the right and/or left eigenvectors of
    a real upper quasi-triangular matrix T.

    The right eigenvector x and the left eigenvector y of T corresponding
    to an eigenvalue w are defined by:

                 T*x = w*x,     y'*T = w*y'

    where y' denotes the conjugate transpose of the vector y.

    If all eigenvectors are requested, the routine may either return the
    matrices X and/or Y of right or left eigenvectors of T, or the
    products Q*X and/or Q*Y, where Q is an input orthogonal
    matrix. If T was obtained from the real-Schur factorization of an
    original matrix A = Q*T*Q', then Q*X and Q*Y are the matrices of
    right or left eigenvectors of A.

    T must be in Schur canonical form (as returned by NUMlapack_dhseqr), that is,
    block upper triangular with 1-by-1 and 2-by-2 diagonal blocks; each
    2-by-2 diagonal block has its diagonal elements equal and its
    off-diagonal elements of opposite sign.  Corresponding to each 2-by-2
    diagonal block is a complex conjugate pair of eigenvalues and
    eigenvectors; only one eigenvector of the pair is computed, namely
    the one corresponding to the eigenvalue with positive imaginary part.

    Arguments
    =========

    SIDE    (input) char*
            = 'R':  compute right eigenvectors only;
            = 'L':  compute left eigenvectors only;
            = 'B':  compute both right and left eigenvectors.

    HOWMNY  (input) char*
            = 'A':  compute all right and/or left eigenvectors;
            = 'B':  compute all right and/or left eigenvectors,
                    and backtransform them using the input matrices
                    supplied in VR and/or VL;
            = 'S':  compute selected right and/or left eigenvectors,
                    specified by the int array SELECT.

    SELECT  (input/output) int array, dimension (N)
            If HOWMNY = 'S', SELECT specifies the eigenvectors to be
            computed.
            If HOWMNY = 'A' or 'B', SELECT is not referenced.
            To select the real eigenvector corresponding to a real
            eigenvalue w(j), SELECT(j) must be set to .TRUE..  To select
            the complex eigenvector corresponding to a complex conjugate
            pair w(j) and w(j+1), either SELECT(j) or SELECT(j+1) must be
            set to .TRUE.; then on exit SELECT(j) is .TRUE. and
            SELECT(j+1) is .FALSE..

    N       (input) long
            The order of the matrix T. N >= 0.

    T       (input) double array, dimension (LDT,N)
            The upper quasi-triangular matrix T in Schur canonical form.

    LDT     (input) long
            The leading dimension of the array T. LDT >= max(1,N).

    VL      (input/output) double array, dimension (LDVL,MM)
            On entry, if SIDE = 'L' or 'B' and HOWMNY = 'B', VL must
            contain an N-by-N matrix Q (usually the orthogonal matrix Q
            of Schur vectors returned by NUMlapack_dhseqr).
            On exit, if SIDE = 'L' or 'B', VL contains:
            if HOWMNY = 'A', the matrix Y of left eigenvectors of T;
                             VL has the same quasi-lower triangular form
                             as T'. If T(i,i) is a real eigenvalue, then
                             the i-th column VL(i) of VL  is its
                             corresponding eigenvector. If T(i:i+1,i:i+1)
                             is a 2-by-2 block whose eigenvalues are
                             complex-conjugate eigenvalues of T, then
                             VL(i)+sqrt(-1)*VL(i+1) is the complex
                             eigenvector corresponding to the eigenvalue
                             with positive real part.
            if HOWMNY = 'B', the matrix Q*Y;
            if HOWMNY = 'S', the left eigenvectors of T specified by
                             SELECT, stored consecutively in the columns
                             of VL, in the same order as their
                             eigenvalues.
            A complex eigenvector corresponding to a complex eigenvalue
            is stored in two consecutive columns, the first holding the
            real part, and the second the imaginary part.
            If SIDE = 'R', VL is not referenced.

    LDVL    (input) long
            The leading dimension of the array VL.  LDVL >= max(1,N) if
            SIDE = 'L' or 'B'; LDVL >= 1 otherwise.

    VR      (input/output) double array, dimension (LDVR,MM)
            On entry, if SIDE = 'R' or 'B' and HOWMNY = 'B', VR must
            contain an N-by-N matrix Q (usually the orthogonal matrix Q
            of Schur vectors returned by NUMlapack_dhseqr).
            On exit, if SIDE = 'R' or 'B', VR contains:
            if HOWMNY = 'A', the matrix X of right eigenvectors of T;
                             VR has the same quasi-upper triangular form
                             as T. If T(i,i) is a real eigenvalue, then
                             the i-th column VR(i) of VR  is its
                             corresponding eigenvector. If T(i:i+1,i:i+1)
                             is a 2-by-2 block whose eigenvalues are
                             complex-conjugate eigenvalues of T, then
                             VR(i)+sqrt(-1)*VR(i+1) is the complex
                             eigenvector corresponding to the eigenvalue
                             with positive real part.
            if HOWMNY = 'B', the matrix Q*X;
            if HOWMNY = 'S', the right eigenvectors of T specified by
                             SELECT, stored consecutively in the columns
                             of VR, in the same order as their
                             eigenvalues.
            A complex eigenvector corresponding to a complex eigenvalue
            is stored in two consecutive columns, the first holding the
            real part and the second the imaginary part.
            If SIDE = 'L', VR is not referenced.

    LDVR    (input) long
            The leading dimension of the array VR.  LDVR >= max(1,N) if
            SIDE = 'R' or 'B'; LDVR >= 1 otherwise.

    MM      (input) long
            The number of columns in the arrays VL and/or VR. MM >= M.

    M       (output) long
            The number of columns in the arrays VL and/or VR actually
            used to store the eigenvectors.
            If HOWMNY = 'A' or 'B', M is set to N.
            Each selected real eigenvector occupies one column and each
            selected complex eigenvector occupies two columns.

    WORK    (workspace) double array, dimension (3*N)

    INFO    (output) INTEGER
            = 0:  successful exit
            < 0:  if INFO = -i, the i-th argument had an illegal value

    Further Details
    ===============

    The algorithm used in this program is basically backward (forward)
    substitution, with scaling to make the the code robust against
    possible overflow.

    Each eigenvector is normalized so that the element of largest
    magnitude has magnitude 1; here the magnitude of a complex number
    (x,y) is taken to be |x| + |y|.

    =====================================================================
*/

int NUMlapack_dtrti2 (const char *uplo, const char *diag, long *n, double *a, long *lda, long *info);
/*  Purpose
    =======

    NUMlapack_dtrti2 computes the inverse of a real upper or lower triangular
    matrix.

    This is the Level 2 BLAS version of the algorithm.

    Arguments
    =========

    UPLO    (input) char*
            Specifies whether the matrix A is upper or lower triangular.
            = 'U':  Upper triangular
            = 'L':  Lower triangular

    DIAG    (input) char*
            Specifies whether or not the matrix A is unit triangular.
            = 'N':  Non-unit triangular
            = 'U':  Unit triangular

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the triangular matrix A.  If UPLO = 'U', the
            leading n by n upper triangular part of the array A contains
            the upper triangular matrix, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading n by n lower triangular part of the array A contains
            the lower triangular matrix, and the strictly upper
            triangular part of A is not referenced.  If DIAG = 'U', the
            diagonal elements of A are also not referenced and are
            assumed to be 1.

            On exit, the (triangular) inverse of the original matrix, in
            the same storage format.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -k, the k-th argument had an illegal value

    =====================================================================
*/

int NUMlapack_dtrtri (const char *uplo, const char *diag, long *n, double *
	a, long *lda, long *info);
/*  Purpose
    =======

    NUMlapack_dtrtri computes the inverse of a real upper or lower triangular
    matrix A.

    This is the Level 3 BLAS version of the algorithm.

    Arguments
    =========

    UPLO    (input) char*
            = 'U':  A is upper triangular;
            = 'L':  A is lower triangular.

    DIAG    (input) char*
            = 'N':  A is non-unit triangular;
            = 'U':  A is unit triangular.

    N       (input) long
            The order of the matrix A.  N >= 0.

    A       (input/output) double array, dimension (LDA,N)
            On entry, the triangular matrix A.  If UPLO = 'U', the
            leading N-by-N upper triangular part of the array A contains
            the upper triangular matrix, and the strictly lower
            triangular part of A is not referenced.  If UPLO = 'L', the
            leading N-by-N lower triangular part of the array A contains
            the lower triangular matrix, and the strictly upper
            triangular part of A is not referenced.  If DIAG = 'U', the
            diagonal elements of A are also not referenced and are
            assumed to be 1.
            On exit, the (triangular) inverse of the original matrix, in
            the same storage format.

    LDA     (input) long
            The leading dimension of the array A.  LDA >= max(1,N).

    INFO    (output) long
            = 0: successful exit
            < 0: if INFO = -i, the i-th argument had an illegal value
            > 0: if INFO = i, A(i,i) is exactly zero.  The triangular
                 matrix is singular and its inverse can not be computed.

    =====================================================================
*/

long NUMlapack_ieeeck (long *ispec, float *zero, float *one);
/*  Purpose
    =======

    NUMlapack_ieeeck is called from the NUMlapack_ilaenv to verify that Infinity and
    possibly NaN arithmetic is safe (i.e. will not trap).

    Arguments
    =========

    ISPEC   (input) long
            Specifies whether to test just for inifinity arithmetic
            or whether to test for infinity and NaN arithmetic.
            = 0: Verify infinity arithmetic only.
            = 1: Verify infinity and NaN arithmetic.

    ZERO    (input) REAL
            Must contain the value 0.0
            This is passed to prevent the compiler from optimizing
            away this code.

    ONE     (input) REAL
            Must contain the value 1.0
            This is passed to prevent the compiler from optimizing
            away this code.

    RETURN VALUE:  long
            = 0:  Arithmetic failed to produce the correct answers
            = 1:  Arithmetic produced the correct answers
*/


long NUMlapack_ilaenv (long *ispec, const char *name, const char *opts, long *n1,
	long *n2, long *n3, long *n4, long name_len, long opts_len);
/*  Purpose
    =======

    NUMlapack_ilaenv is called from the LAPACK routines to choose problem-dependent
    parameters for the local environment.  See ISPEC for a description of
    the parameters.

    This version provides a set of parameters which should give good,
    but not optimal, performance on many of the currently available
    computers.  Users are encouraged to modify this subroutine to set
    the tuning parameters for their particular machine using the option
    and problem size information in the arguments.

    This routine will not function correctly if it is converted to all
    lower case.  Converting it to all upper case is allowed.

    Arguments
    =========

    ISPEC   (input) long
            Specifies the parameter to be returned as the value of
            NUMlapack_ilaenv.
            = 1: the optimal blocksize; if this value is 1, an unblocked
                 algorithm will give the best performance.
            = 2: the minimum block size for which the block routine
                 should be used; if the usable block size is less than
                 this value, an unblocked routine should be used.
            = 3: the crossover point (in a block routine, for N less
                 than this value, an unblocked routine should be used)
            = 4: the number of shifts, used in the nonsymmetric
                 eigenvalue routines
            = 5: the minimum column dimension for blocking to be used;
                 rectangular blocks must have dimension at least k by m,
                 where k is given by NUMlapack_ilaenv(2,...) and m by NUMlapack_ilaenv(5,...)
            = 6: the crossover point for the SVD (when reducing an m by n
                 matrix to bidiagonal form, if max(m,n)/min(m,n) exceeds
                 this value, a QR factorization is used first to reduce
                 the matrix to a triangular form.)
            = 7: the number of processors
            = 8: the crossover point for the multishift QR and QZ methods
                 for nonsymmetric eigenvalue problems.
            = 9: maximum size of the subproblems at the bottom of the
                 computation tree in the divide-and-conquer algorithm
                 (used by xGELSD and xGESDD)
            =10: ieee NaN arithmetic can be trusted not to trap
            =11: infinity arithmetic can be trusted not to trap

    NAME    (input) CHARACTER*(*)
            The name of the calling subroutine, in either upper case or
            lower case.

    OPTS    (input) CHARACTER*(*)
            The character options to the subroutine NAME, concatenated
            into a single character string.  For example, UPLO = 'U',
            TRANS = 'T', and DIAG = 'N' for a triangular routine would
            be specified as OPTS = 'UTN'.

    N1      (input) long
    N2      (input) long
    N3      (input) long
    N4      (input) long
            Problem dimensions for the subroutine NAME; these may not all
            be required.

   (NUMlapack_ilaenv) (output) long
            >= 0: the value of the parameter specified by ISPEC
            < 0:  if NUMlapack_ilaenv = -k, the k-th argument had an illegal value.

    Further Details
    ===============

    The following conventions have been used when calling NUMlapack_ilaenv from the
    LAPACK routines:
    1)  OPTS is a concatenation of all of the character options to
        subroutine NAME, in the same order that they appear in the
        argument list for NAME, even if they are not used in determining
        the value of the parameter specified by ISPEC.
    2)  The problem dimensions N1, N2, N3, N4 are specified in the order
        that they appear in the argument list for NAME.  N1 is used
        first, N2 second, and so on, and unused problem dimensions are
        passed a value of -1.
    3)  The parameter value returned by NUMlapack_ilaenv is checked for validity in
        the calling subroutine.  For example, NUMlapack_ilaenv is used to retrieve
        the optimal blocksize for STRTRI as follows:

        NB = NUMlapack_ilaenv( 1, 'STRTRI', UPLO // DIAG, N, -1, -1, -1 )
        IF( NB.LE.1 ) NB = MAX( 1, N )

    =====================================================================
*/

#ifdef __cplusplus
	}
#endif

#endif /* _NUMclapack_h_ */
