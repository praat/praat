#ifndef _NUMlapack_h_
#define _NUMlapack_h_
/* NUMlapack.h
 *
 * Copyright (C) 2020 David Weenink
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

#include "melder.h"
#include "clapack.h"
#undef max
#undef min

/*
	Interface for the lapack routines used in Praat.
	
	The LAPACK routines all use col major layout of matrices.
	The matrix 
	1 2
	3 4
	can be layed out in memory as 1 2 3 4 (row major, C++) or as 1 3 2 4 (column major, fortran).
	
	Sometimes you can avoid an explicit matrix conversion to col major layout by using the solution for the transposed problem.
	
	The purpose of a routine and its parameter descriptions have all been copied from the LAPACK netlib repository.
	
	Our NUMlapack_<name>_ interface merely removes inconvenient references.
	We used the following conventions in the interface definitions:
	1. The interface name is the CLAPACK name with "NUMlapack_" in front.
		(e.g. the lapack routine dgeev_ becomes NUMlapack_dgeev_)
	2. We have removed all inconvenient pointers to integers.
		(e.g. "integer *n, the third argument type of dgeev_, has become
		"integer n" in NUMlapack_dgeev_)
	3. Array pointers are always zero-based and made explicit by using "[]" after the variable name
		instead of "*" before the name.
		(e.g. "double inout_a []" instead of "double *inout_a" as the fourth
		argument of NUMlapack_dgeev_)
	4. Scalar pointers should never be the nullptr.
	5. Array pointers whose name starts with "inout_" should never equal the nullptr.
		The arrays they point to always have to be of the correct dimension.
	6. Only an array pointer's name starting with "inoutout_" may contain a nullptr 
		occasionaly if the rest of the parameter settings allow.
*/

static inline double NUMlapack_dlamch_ (const char *cmach) {
	return dlamch_(cmach);
}

static inline int NUMlapack_dgeev_ (const char *jobvl, const char *jobvr, integer n, double inout_a [], integer lda, double inout_wr [], double inout_wi [], double inoutout_vl [], integer ldvl, double inoutout_vr [], integer ldvr, double inout_work [], integer lwork, integer *info)
{
	return dgeev_ (jobvl, jobvr, & n, inout_a, & lda, inout_wr, inout_wi, inoutout_vl, & ldvl, inoutout_vr, & ldvr, inout_work, & lwork, info);
}
/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGEEV computes for an N-by-N real nonsymmetric matrix A, the */
/*  eigenvalues and, optionally, the left and/or right eigenvectors. */

/*  The right eigenvector v(j) of A satisfies */
/*                   A * v(j) = lambda(j) * v(j) */
/*  where lambda(j) is its eigenvalue. */
/*  The left eigenvector u(j) of A satisfies */
/*                u(j)**H * A = lambda(j) * u(j)**H */
/*  where u(j)**H denotes the conjugate transpose of u(j). */

/*  The computed eigenvectors are normalized to have Euclidean norm */
/*  equal to 1 and largest component real. */

/*  Arguments */
/*  ========= */

/*  JOBVL   (input) CHARACTER*1 */
/*          = 'N': left eigenvectors of A are not computed; */
/*          = 'V': left eigenvectors of A are computed. */

/*  JOBVR   (input) CHARACTER*1 */
/*          = 'N': right eigenvectors of A are not computed; */
/*          = 'V': right eigenvectors of A are computed. */

/*  N       (input) INTEGER */
/*          The order of the matrix A. N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the N-by-N matrix A. */
/*          On exit, A has been overwritten. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  WR      (output) DOUBLE PRECISION array, dimension (N) */
/*  WI      (output) DOUBLE PRECISION array, dimension (N) */
/*          WR and WI contain the real and imaginary parts, */
/*          respectively, of the computed eigenvalues.  Complex */
/*          conjugate pairs of eigenvalues appear consecutively */
/*          with the eigenvalue having the positive imaginary part */
/*          first. */

/*  VL      (output) DOUBLE PRECISION array, dimension (LDVL,N) */
/*          If JOBVL = 'V', the left eigenvectors u(j) are stored one */
/*          after another in the columns of VL, in the same order */
/*          as their eigenvalues. */
/*          If JOBVL = 'N', VL is not referenced. */
/*          If the j-th eigenvalue is real, then u(j) = VL(:,j), */
/*          the j-th column of VL. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then u(j) = VL(:,j) + i*VL(:,j+1) and */
/*          u(j+1) = VL(:,j) - i*VL(:,j+1). */

/*  LDVL    (input) INTEGER */
/*          The leading dimension of the array VL.  LDVL >= 1; if */
/*          JOBVL = 'V', LDVL >= N. */

/*  VR      (output) DOUBLE PRECISION array, dimension (LDVR,N) */
/*          If JOBVR = 'V', the right eigenvectors v(j) are stored one */
/*          after another in the columns of VR, in the same order */
/*          as their eigenvalues. */
/*          If JOBVR = 'N', VR is not referenced. */
/*          If the j-th eigenvalue is real, then v(j) = VR(:,j), */
/*          the j-th column of VR. */
/*          If the j-th and (j+1)-st eigenvalues form a complex */
/*          conjugate pair, then v(j) = VR(:,j) + i*VR(:,j+1) and */
/*          v(j+1) = VR(:,j) - i*VR(:,j+1). */

/*  LDVR    (input) INTEGER */
/*          The leading dimension of the array VR.  LDVR >= 1; if */
/*          JOBVR = 'V', LDVR >= N. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK.  LWORK >= max(1,3*N), and */
/*          if JOBVL = 'V' or JOBVR = 'V', LWORK >= 4*N.  For good */
/*          performance, LWORK must generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = i, the QR algorithm failed to compute all the */
/*                eigenvalues, and no eigenvectors have been computed; */
/*                elements i+1:N of WR and WI contain eigenvalues which */
/*                have converged. */

/*  ===================================================================== */


static inline int NUMlapack_dgesvd_ (const char *jobu, const char *jobvt, integer m, integer n, double inout_a [], integer lda, double inout_s [], double inout_u [], integer ldu, double inoutout_vt [], integer ldvt, double inout_work [], integer lwork, integer *info) {
	return dgesvd_ (jobu, jobvt, & m, & n, inout_a, & lda, inout_s, inout_u, & ldu, inoutout_vt, & ldvt, inout_work, & lwork, info);
}
/*  -- LAPACK driver routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGESVD computes the singular value decomposition (SVD) of a real */
/*  M-by-N matrix A, optionally computing the left and/or right singular */
/*  vectors. The SVD is written */

/*       A = U * SIGMA * transpose(V) */

/*  where SIGMA is an M-by-N matrix which is zero except for its */
/*  min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and */
/*  V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA */
/*  are the singular values of A; they are real and non-negative, and */
/*  are returned in descending order.  The first min(m,n) columns of */
/*  U and V are the left and right singular vectors of A. */

/*  Note that the routine returns V**T, not V. */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix U: */
/*          = 'A':  all M columns of U are returned in array U: */
/*          = 'S':  the first min(m,n) columns of U (the left singular */
/*                  vectors) are returned in the array U; */
/*          = 'O':  the first min(m,n) columns of U (the left singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no columns of U (no left singular vectors) are */
/*                  computed. */

/*  JOBVT   (input) CHARACTER*1 */
/*          Specifies options for computing all or part of the matrix */
/*          V**T: */
/*          = 'A':  all N rows of V**T are returned in the array VT; */
/*          = 'S':  the first min(m,n) rows of V**T (the right singular */
/*                  vectors) are returned in the array VT; */
/*          = 'O':  the first min(m,n) rows of V**T (the right singular */
/*                  vectors) are overwritten on the array A; */
/*          = 'N':  no rows of V**T (no right singular vectors) are */
/*                  computed. */

/*          JOBVT and JOBU cannot both be 'O'. */

/*  M       (input) INTEGER */
/*          The number of rows of the input matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the input matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, */
/*          if JOBU = 'O',  A is overwritten with the first min(m,n) */
/*                          columns of U (the left singular vectors, */
/*                          stored columnwise); */
/*          if JOBVT = 'O', A is overwritten with the first min(m,n) */
/*                          rows of V**T (the right singular vectors, */
/*                          stored rowwise); */
/*          if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A */
/*                          are destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,M). */

/*  S       (output) DOUBLE PRECISION array, dimension (min(M,N)) */
/*          The singular values of A, sorted so that S(i) >= S(i+1). */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,UCOL) */
/*          (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'. */
/*          If JOBU = 'A', U contains the M-by-M orthogonal matrix U; */
/*          if JOBU = 'S', U contains the first min(m,n) columns of U */
/*          (the left singular vectors, stored columnwise); */
/*          if JOBU = 'N' or 'O', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U.  LDU >= 1; if */
/*          JOBU = 'S' or 'A', LDU >= M. */

/*  VT      (output) DOUBLE PRECISION array, dimension (LDVT,N) */
/*          If JOBVT = 'A', VT contains the N-by-N orthogonal matrix */
/*          V**T; */
/*          if JOBVT = 'S', VT contains the first min(m,n) rows of */
/*          V**T (the right singular vectors, stored rowwise); */
/*          if JOBVT = 'N' or 'O', VT is not referenced. */

/*  LDVT    (input) INTEGER */
/*          The leading dimension of the array VT.  LDVT >= 1; if */
/*          JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N). */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK; */
/*          if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged */
/*          superdiagonal elements of an upper bidiagonal matrix B */
/*          whose diagonal is in S (not necessarily sorted). B */
/*          satisfies A = U * B * VT, so it has the same singular values */
/*          as A, and singular vectors related by U and VT. */

/*  LWORK   (input) INTEGER */
/*          The dimension of the array WORK. */
/*          LWORK >= MAX(1,3*MIN(M,N)+MAX(M,N),5*MIN(M,N)). */
/*          For good performance, LWORK should generally be larger. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit. */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if DBDSQR did not converge, INFO specifies how many */
/*                superdiagonals of an intermediate bidiagonal form B */
/*                did not converge to zero. See the description of WORK */
/*                above for details. */

/*  ===================================================================== */


static inline int NUMlapack_dggsvd_ (const char *jobu, const char *jobv, const char *jobq, integer m, integer n,	integer p, integer *inout_k, integer *inout_l, double inout_a [], integer lda, double b [], integer ldb, double inout_alpha [], double inout_beta [], double out_u [], integer ldu, double out_v [], integer ldv, double out_q [], integer ldq, double inout_work [], integer inout_iwork [], integer *info) {
		return dggsvd_ (jobu, jobv, jobq, & m, & n,	& p, inout_k, inout_l, inout_a, & lda, b, & ldb, inout_alpha, inout_beta, out_u, & ldu, out_v, & ldv, out_q, & ldq, inout_work, inout_iwork, info);
}
/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DGGSVD computes the generalized singular value decomposition (GSVD) */
/*  of an M-by-N real matrix A and P-by-N real matrix B: */

/*      U'*A*Q = D1*( 0 R ),    V'*B*Q = D2*( 0 R ) */

/*  where U, V and Q are orthogonal matrices, and Z' is the transpose */
/*  of Z.  Let K+L = the effective numerical rank of the matrix (A',B')', */
/*  then R is a K+L-by-K+L nonsingular upper triangular matrix, D1 and */
/*  D2 are M-by-(K+L) and P-by-(K+L) "diagonal" matrices and of the */
/*  following structures, respectively: */

/*  If M-K-L >= 0, */

/*                      K  L */
/*         D1 =     K ( I  0 ) */
/*                  L ( 0  C ) */
/*              M-K-L ( 0  0 ) */

/*                    K  L */
/*         D2 =   L ( 0  S ) */
/*              P-L ( 0  0 ) */

/*                  N-K-L  K    L */
/*    ( 0 R ) = K (  0   R11  R12 ) */
/*              L (  0    0   R22 ) */

/*  where */

/*    C = diag( ALPHA(K+1), ... , ALPHA(K+L) ), */
/*    S = diag( BETA(K+1),  ... , BETA(K+L) ), */
/*    C**2 + S**2 = I. */

/*    R is stored in A(1:K+L,N-K-L+1:N) on exit. */

/*  If M-K-L < 0, */

/*                    K M-K K+L-M */
/*         D1 =   K ( I  0    0   ) */
/*              M-K ( 0  C    0   ) */

/*                      K M-K K+L-M */
/*         D2 =   M-K ( 0  S    0  ) */
/*              K+L-M ( 0  0    I  ) */
/*                P-L ( 0  0    0  ) */

/*                     N-K-L  K   M-K  K+L-M */
/*    ( 0 R ) =     K ( 0    R11  R12  R13  ) */
/*                M-K ( 0     0   R22  R23  ) */
/*              K+L-M ( 0     0    0   R33  ) */

/*  where */

/*    C = diag( ALPHA(K+1), ... , ALPHA(M) ), */
/*    S = diag( BETA(K+1),  ... , BETA(M) ), */
/*    C**2 + S**2 = I. */

/*    (R11 R12 R13 ) is stored in A(1:M, N-K-L+1:N), and R33 is stored */
/*    ( 0  R22 R23 ) */
/*    in B(M-K+1:L,N+M-K-L+1:N) on exit. */

/*  The routine computes C, S, R, and optionally the orthogonal */
/*  transformation matrices U, V and Q. */

/*  In particular, if B is an N-by-N nonsingular matrix, then the GSVD of */
/*  A and B implicitly gives the SVD of A*inv(B): */
/*                       A*inv(B) = U*(D1*inv(D2))*V'. */
/*  If ( A',B')' has orthonormal columns, then the GSVD of A and B is */
/*  also equal to the CS decomposition of A and B. Furthermore, the GSVD */
/*  can be used to derive the solution of the eigenvalue problem: */
/*                       A'*A x = lambda* B'*B x. */
/*  In some literature, the GSVD of A and B is presented in the form */
/*                   U'*A*X = ( 0 D1 ),   V'*B*X = ( 0 D2 ) */
/*  where U and V are orthogonal and X is nonsingular, D1 and D2 are */
/*  ``diagonal''.  The former GSVD form can be converted to the latter */
/*  form by taking the nonsingular matrix X as */

/*                       X = Q*( I   0    ) */
/*                             ( 0 inv(R) ). */

/*  Arguments */
/*  ========= */

/*  JOBU    (input) CHARACTER*1 */
/*          = 'U':  Orthogonal matrix U is computed; */
/*          = 'N':  U is not computed. */

/*  JOBV    (input) CHARACTER*1 */
/*          = 'V':  Orthogonal matrix V is computed; */
/*          = 'N':  V is not computed. */

/*  JOBQ    (input) CHARACTER*1 */
/*          = 'Q':  Orthogonal matrix Q is computed; */
/*          = 'N':  Q is not computed. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrices A and B.  N >= 0. */

/*  P       (input) INTEGER */
/*          The number of rows of the matrix B.  P >= 0. */

/*  K       (output) INTEGER */
/*  L       (output) INTEGER */
/*          On exit, K and L specify the dimension of the subblocks */
/*          described in the Purpose section. */
/*          K + L = effective numerical rank of (A',B')'. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the M-by-N matrix A. */
/*          On exit, A contains the triangular matrix R, or part of R. */
/*          See Purpose for details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A. LDA >= max(1,M). */

/*  B       (input/output) DOUBLE PRECISION array, dimension (LDB,N) */
/*          On entry, the P-by-N matrix B. */
/*          On exit, B contains the triangular matrix R if M-K-L < 0. */
/*          See Purpose for details. */

/*  LDB     (input) INTEGER */
/*          The leading dimension of the array B. LDB >= max(1,P). */

/*  ALPHA   (output) DOUBLE PRECISION array, dimension (N) */
/*  BETA    (output) DOUBLE PRECISION array, dimension (N) */
/*          On exit, ALPHA and BETA contain the generalized singular */
/*          value pairs of A and B; */
/*            ALPHA(1:K) = 1, */
/*            BETA(1:K)  = 0, */
/*          and if M-K-L >= 0, */
/*            ALPHA(K+1:K+L) = C, */
/*            BETA(K+1:K+L)  = S, */
/*          or if M-K-L < 0, */
/*            ALPHA(K+1:M)=C, ALPHA(M+1:K+L)=0 */
/*            BETA(K+1:M) =S, BETA(M+1:K+L) =1 */
/*          and */
/*            ALPHA(K+L+1:N) = 0 */
/*            BETA(K+L+1:N)  = 0 */

/*  U       (output) DOUBLE PRECISION array, dimension (LDU,M) */
/*          If JOBU = 'U', U contains the M-by-M orthogonal matrix U. */
/*          If JOBU = 'N', U is not referenced. */

/*  LDU     (input) INTEGER */
/*          The leading dimension of the array U. LDU >= max(1,M) if */
/*          JOBU = 'U'; LDU >= 1 otherwise. */

/*  V       (output) DOUBLE PRECISION array, dimension (LDV,P) */
/*          If JOBV = 'V', V contains the P-by-P orthogonal matrix V. */
/*          If JOBV = 'N', V is not referenced. */

/*  LDV     (input) INTEGER */
/*          The leading dimension of the array V. LDV >= max(1,P) if */
/*          JOBV = 'V'; LDV >= 1 otherwise. */

/*  Q       (output) DOUBLE PRECISION array, dimension (LDQ,N) */
/*          If JOBQ = 'Q', Q contains the N-by-N orthogonal matrix Q. */
/*          If JOBQ = 'N', Q is not referenced. */

/*  LDQ     (input) INTEGER */
/*          The leading dimension of the array Q. LDQ >= max(1,N) if */
/*          JOBQ = 'Q'; LDQ >= 1 otherwise. */

/*  WORK    (workspace) DOUBLE PRECISION array, */
/*                      dimension (max(3*N,M,P)+N) */

/*  IWORK   (workspace/output) INTEGER array, dimension (N) */
/*          On exit, IWORK stores the sorting information. More */
/*          precisely, the following loop will sort ALPHA */
/*             for I = K+1, min(M,K+L) */
/*                 swap ALPHA(I) and ALPHA(IWORK(I)) */
/*             endfor */
/*          such that ALPHA(1) >= ALPHA(2) >= ... >= ALPHA(N). */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */
/*          > 0:  if INFO = 1, the Jacobi-type procedure failed to */
/*                converge.  For further details, see subroutine DTGSJA. */

/*  Internal Parameters */
/*  =================== */

/*  TOLA    DOUBLE PRECISION */
/*  TOLB    DOUBLE PRECISION */
/*          TOLA and TOLB are the thresholds to determine the effective */
/*          rank of (A',B')'. Generally, they are set to */
/*                   TOLA = MAX(M,N)*norm(A)*MAZHEPS, */
/*                   TOLB = MAX(P,N)*norm(B)*MAZHEPS. */
/*          The size of TOLA and TOLB may affect the size of backward */
/*          errors of the decomposition. */

/*  Further Details */
/*  =============== */

/*  2-96 Based on modifications by */
/*     Ming Gu and Huan Ren, Computer Science Division, University of */
/*     California at Berkeley, USA */

/*  ===================================================================== */

static inline int NUMlapack_dhseqr_ (const char *job, const char *compz, integer n, integer ilo, integer ihi, double inout_h [], integer ldh, double inout_wr [], double inout_wi [], double inoutout_z [], integer ldz, double inout_work [], integer lwork, integer *info) 
{
	return dhseqr_ (job, compz, & n, & ilo, & ihi, inout_h, & ldh, inout_wr, inout_wi, inoutout_z, & ldz, inout_work, & lwork, info);
}
/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, Univ. of Colorado Denver and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     Purpose */
/*     ======= */

/*     DHSEQR computes the eigenvalues of a Hessenberg matrix H */
/*     and, optionally, the matrices T and Z from the Schur decomposition */
/*     H = Z T Z**T, where T is an upper quasi-triangular matrix (the */
/*     Schur form), and Z is the orthogonal matrix of Schur vectors. */

/*     Optionally Z may be postmultiplied into an input orthogonal */
/*     matrix Q so that this routine can give the Schur factorization */
/*     of a matrix A which has been reduced to the Hessenberg form H */
/*     by the orthogonal matrix Q:  A = Q*H*Q**T = (QZ)*T*(QZ)**T. */

/*     Arguments */
/*     ========= */

/*     JOB   (input) CHARACTER*1 */
/*           = 'E':  compute eigenvalues only; */
/*           = 'S':  compute eigenvalues and the Schur form T. */

/*     COMPZ (input) CHARACTER*1 */
/*           = 'N':  no Schur vectors are computed; */
/*           = 'I':  Z is initialized to the unit matrix and the matrix Z */
/*                   of Schur vectors of H is returned; */
/*           = 'V':  Z must contain an orthogonal matrix Q on entry, and */
/*                   the product Q*Z is returned. */

/*     N     (input) INTEGER */
/*           The order of the matrix H.  N .GE. 0. */

/*     ILO   (input) INTEGER */
/*     IHI   (input) INTEGER */
/*           It is assumed that H is already upper triangular in rows */
/*           and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally */
/*           set by a previous call to DGEBAL, and then passed to DGEHRD */
/*           when the matrix output by DGEBAL is reduced to Hessenberg */
/*           form. Otherwise ILO and IHI should be set to 1 and N */
/*           respectively.  If N.GT.0, then 1.LE.ILO.LE.IHI.LE.N. */
/*           If N = 0, then ILO = 1 and IHI = 0. */

/*     H     (input/output) DOUBLE PRECISION array, dimension (LDH,N) */
/*           On entry, the upper Hessenberg matrix H. */
/*           On exit, if INFO = 0 and JOB = 'S', then H contains the */
/*           upper quasi-triangular matrix T from the Schur decomposition */
/*           (the Schur form); 2-by-2 diagonal blocks (corresponding to */
/*           complex conjugate pairs of eigenvalues) are returned in */
/*           standard form, with H(i,i) = H(i+1,i+1) and */
/*           H(i+1,i)*H(i,i+1).LT.0. If INFO = 0 and JOB = 'E', the */
/*           contents of H are unspecified on exit.  (The output value of */
/*           H when INFO.GT.0 is given under the description of INFO */
/*           below.) */

/*           Unlike earlier versions of DHSEQR, this subroutine may */
/*           explicitly H(i,j) = 0 for i.GT.j and j = 1, 2, ... ILO-1 */
/*           or j = IHI+1, IHI+2, ... N. */

/*     LDH   (input) INTEGER */
/*           The leading dimension of the array H. LDH .GE. max(1,N). */

/*     WR    (output) DOUBLE PRECISION array, dimension (N) */
/*     WI    (output) DOUBLE PRECISION array, dimension (N) */
/*           The real and imaginary parts, respectively, of the computed */
/*           eigenvalues. If two eigenvalues are computed as a complex */
/*           conjugate pair, they are stored in consecutive elements of */
/*           WR and WI, say the i-th and (i+1)th, with WI(i) .GT. 0 and */
/*           WI(i+1) .LT. 0. If JOB = 'S', the eigenvalues are stored in */
/*           the same order as on the diagonal of the Schur form returned */
/*           in H, with WR(i) = H(i,i) and, if H(i:i+1,i:i+1) is a 2-by-2 */
/*           diagonal block, WI(i) = sqrt(-H(i+1,i)*H(i,i+1)) and */
/*           WI(i+1) = -WI(i). */

/*     Z     (input/output) DOUBLE PRECISION array, dimension (LDZ,N) */
/*           If COMPZ = 'N', Z is not referenced. */
/*           If COMPZ = 'I', on entry Z need not be set and on exit, */
/*           if INFO = 0, Z contains the orthogonal matrix Z of the Schur */
/*           vectors of H.  If COMPZ = 'V', on entry Z must contain an */
/*           N-by-N matrix Q, which is assumed to be equal to the unit */
/*           matrix except for the submatrix Z(ILO:IHI,ILO:IHI). On exit, */
/*           if INFO = 0, Z contains Q*Z. */
/*           Normally Q is the orthogonal matrix generated by DORGHR */
/*           after the call to DGEHRD which formed the Hessenberg matrix */
/*           H. (The output value of Z when INFO.GT.0 is given under */
/*           the description of INFO below.) */

/*     LDZ   (input) INTEGER */
/*           The leading dimension of the array Z.  if COMPZ = 'I' or */
/*           COMPZ = 'V', then LDZ.GE.MAX(1,N).  Otherwize, LDZ.GE.1. */

/*     WORK  (workspace/output) DOUBLE PRECISION array, dimension (LWORK) */
/*           On exit, if INFO = 0, WORK(1) returns an estimate of */
/*           the optimal value for LWORK. */

/*     LWORK (input) INTEGER */
/*           The dimension of the array WORK.  LWORK .GE. max(1,N) */
/*           is sufficient and delivers very good and sometimes */
/*           optimal performance.  However, LWORK as large as 11*N */
/*           may be required for optimal performance.  A workspace */
/*           query is recommended to determine the optimal workspace */
/*           size. */

/*           If LWORK = -1, then DHSEQR does a workspace query. */
/*           In this case, DHSEQR checks the input parameters and */
/*           estimates the optimal workspace size for the given */
/*           values of N, ILO and IHI.  The estimate is returned */
/*           in WORK(1).  No error message related to LWORK is */
/*           issued by XERBLA.  Neither H nor Z are accessed. */


/*     INFO  (output) INTEGER */
/*             =  0:  successful exit */
/*           .LT. 0:  if INFO = -i, the i-th argument had an illegal */
/*                    value */
/*           .GT. 0:  if INFO = i, DHSEQR failed to compute all of */
/*                the eigenvalues.  Elements 1:ilo-1 and i+1:n of WR */
/*                and WI contain those eigenvalues which have been */
/*                successfully computed.  (Failures are rare.) */

/*                If INFO .GT. 0 and JOB = 'E', then on exit, the */
/*                remaining unconverged eigenvalues are the eigen- */
/*                values of the upper Hessenberg matrix rows and */
/*                columns ILO through INFO of the final, output */
/*                value of H. */

/*                If INFO .GT. 0 and JOB   = 'S', then on exit */

/*           (*)  (initial value of H)*U  = U*(final value of H) */

/*                where U is an orthogonal matrix.  The final */
/*                value of H is upper Hessenberg and quasi-triangular */
/*                in rows and columns INFO+1 through IHI. */

/*                If INFO .GT. 0 and COMPZ = 'V', then on exit */

/*                  (final value of Z)  =  (initial value of Z)*U */

/*                where U is the orthogonal matrix in (*) (regard- */
/*                less of the value of JOB.) */

/*                If INFO .GT. 0 and COMPZ = 'I', then on exit */
/*                      (final value of Z)  = U */
/*                where U is the orthogonal matrix in (*) (regard- */
/*                less of the value of JOB.) */

/*                If INFO .GT. 0 and COMPZ = 'N', then Z is not */
/*                accessed. */

/*     ================================================================ */
/*             Default values supplied by */
/*             ILAENV(ISPEC,'DHSEQR',JOB(:1)//COMPZ(:1),N,ILO,IHI,LWORK). */
/*             It is suggested that these defaults be adjusted in order */
/*             to attain best performance in each particular */
/*             computational environment. */

/*            ISPEC=12: The DLAHQR vs DLAQR0 crossover point. */
/*                      Default: 75. (Must be at least 11.) */

/*            ISPEC=13: Recommended deflation window size. */
/*                      This depends on ILO, IHI and NS.  NS is the */
/*                      number of simultaneous shifts returned */
/*                      by ILAENV(ISPEC=15).  (See ISPEC=15 below.) */
/*                      The default for (IHI-ILO+1).LE.500 is NS. */
/*                      The default for (IHI-ILO+1).GT.500 is 3*NS/2. */

/*            ISPEC=14: Nibble crossover point. (See IPARMQ for */
/*                      details.)  Default: 14% of deflation window */
/*                      size. */

/*            ISPEC=15: Number of simultaneous shifts in a multishift */
/*                      QR iteration. */

/*                      If IHI-ILO+1 is ... */

/*                      greater than      ...but less    ... the */
/*                      or equal to ...      than        default is */

/*                           1               30          NS =   2(+) */
/*                          30               60          NS =   4(+) */
/*                          60              150          NS =  10(+) */
/*                         150              590          NS =  ** */
/*                         590             3000          NS =  64 */
/*                        3000             6000          NS = 128 */
/*                        6000             infinity      NS = 256 */

/*                  (+)  By default some or all matrices of this order */
/*                       are passed to the implicit double shift routine */
/*                       DLAHQR and this parameter is ignored.  See */
/*                       ISPEC=12 above and comments in IPARMQ for */
/*                       details. */

/*                 (**)  The asterisks (**) indicate an ad-hoc */
/*                       function of N increasing from 10 to 64. */

/*            ISPEC=16: Select structured matrix multiply. */
/*                      If the number of simultaneous shifts (specified */
/*                      by ISPEC=15) is less than 14, then the default */
/*                      for ISPEC=16 is 0.  Otherwise the default for */
/*                      ISPEC=16 is 2. */

/*     ================================================================ */
/*     Based on contributions by */
/*        Karen Braman and Ralph Byers, Department of Mathematics, */
/*        University of Kansas, USA */

/*     ================================================================ */
/*     References: */
/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part I: Maintaining Well Focused Shifts, and Level 3 */
/*       Performance, SIAM Journal of Matrix Analysis, volume 23, pages */
/*       929--947, 2002. */

/*       K. Braman, R. Byers and R. Mathias, The Multi-Shift QR */
/*       Algorithm Part II: Aggressive Early Deflation, SIAM Journal */
/*       of Matrix Analysis, volume 23, pages 948--973, 2002. */

/*     ================================================================ */

static inline int NUMlapack_dpotf2_ (const char *uplo, integer n, double inout_a [], integer lda, integer *info)
{
	return dpotf2_ (uplo, & n, inout_a, & lda, info);
}
/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DPOTF2 computes the Cholesky factorization of a real symmetric */
/*  positive definite matrix A. */

/*  The factorization has the form */
/*     A = U' * U ,  if UPLO = 'U', or */
/*     A = L  * L',  if UPLO = 'L', */
/*  where U is an upper triangular matrix and L is lower triangular. */

/*  This is the unblocked version of the algorithm, calling Level 2 BLAS. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the upper or lower triangular part of the */
/*          symmetric matrix A is stored. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the leading */
/*          n by n upper triangular part of A contains the upper */
/*          triangular part of the matrix A, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n by n lower triangular part of A contains the lower */
/*          triangular part of the matrix A, and the strictly upper */
/*          triangular part of A is not referenced. */

/*          On exit, if INFO = 0, the factor U or L from the Cholesky */
/*          factorization A = U'*U  or A = L*L'. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */
/*          > 0: if INFO = k, the leading minor of order k is not */
/*               positive definite, and the factorization could not be */
/*               completed. */

/*  ===================================================================== */

static inline int NUMlapack_dsyev_ (const char *jobz, const char *uplo, integer n,
	double inout_a [], integer lda,	double inout_w [], double inout_work [],
	integer lwork, integer *info)
{
	return dsyev_ (jobz, uplo, & n, inout_a, & lda, inout_w, inout_work, & lwork, info);
}
/*  -- LAPACK driver routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DSYEV computes all eigenvalues and, optionally, eigenvectors of a */
/*  real symmetric matrix A. */

/*  Arguments */
/*  ========= */

/*  JOBZ    (input) CHARACTER*1 */
/*          = 'N':  Compute eigenvalues only; */
/*          = 'V':  Compute eigenvalues and eigenvectors. */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  Upper triangle of A is stored; */
/*          = 'L':  Lower triangle of A is stored. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA, N) */
/*          On entry, the symmetric matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of A contains the */
/*          upper triangular part of the matrix A.  If UPLO = 'L', */
/*          the leading N-by-N lower triangular part of A contains */
/*          the lower triangular part of the matrix A. */
/*          On exit, if JOBZ = 'V', then if INFO = 0, A contains the */
/*          orthonormal eigenvectors of the matrix A. */
/*          If JOBZ = 'N', then on exit the lower triangle (if UPLO='L') */
/*          or the upper triangle (if UPLO='U') of A, including the */
/*          diagonal, is destroyed. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  W       (output) DOUBLE PRECISION array, dimension (N) */
/*          If INFO = 0, the eigenvalues in ascending order. */

/*  WORK    (workspace/output) DOUBLE PRECISION array, dimension (MAX(1,LWORK)) */
/*          On exit, if INFO = 0, WORK(1) returns the optimal LWORK. */

/*  LWORK   (input) INTEGER */
/*          The length of the array WORK.  LWORK >= max(1,3*N-1). */
/*          For optimal efficiency, LWORK >= (NB+2)*N, */
/*          where NB is the blocksize for DSYTRD returned by ILAENV. */

/*          If LWORK = -1, then a workspace query is assumed; the routine */
/*          only calculates the optimal size of the WORK array, returns */
/*          this value as the first entry of the WORK array, and no error */
/*          message related to LWORK is issued by XERBLA. */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value */
/*          > 0:  if INFO = i, the algorithm failed to converge; i */
/*                off-diagonal elements of an intermediate tridiagonal */
/*                form did not converge to zero. */

/*  ===================================================================== */


static inline int NUMlapack_dtrtri_ (const char *uplo, const char *diag, integer n, double
	inout_a [], integer lda, integer *inout_info)
{
	return dtrtri_ (uplo, diag, & n, inout_a, & lda, inout_info);
}
/*  -- LAPACK routine (version 3.1) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTRI computes the inverse of a real upper or lower triangular */
/*  matrix A. */

/*  This is the Level 3 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          = 'U':  A is upper triangular; */
/*          = 'L':  A is lower triangular. */

/*  DIAG    (input) CHARACTER*1 */
/*          = 'N':  A is non-unit triangular; */
/*          = 'U':  A is unit triangular. */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular matrix A.  If UPLO = 'U', the */
/*          leading N-by-N upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading N-by-N lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced.  If DIAG = 'U', the */
/*          diagonal elements of A are also not referenced and are */
/*          assumed to be 1. */
/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same storage format. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -i, the i-th argument had an illegal value */
/*          > 0: if INFO = i, A(i,i) is exactly zero.  The triangular */
/*               matrix is singular and its inverse can not be computed. */

/*  ===================================================================== */

static inline int NUMlapack_dtrti2_ (const char *uplo, const char *diag, integer n, double inout_a [], integer lda, integer *inout_info)
{
	return dtrti2_ (uplo, diag, & n, inout_a, & lda, inout_info);
}
/*  -- LAPACK routine (version 3.2) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley and NAG Ltd.. */
/*     November 2006 */

/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */

/*  Purpose */
/*  ======= */

/*  DTRTI2 computes the inverse of a real upper or lower triangular */
/*  matrix. */

/*  This is the Level 2 BLAS version of the algorithm. */

/*  Arguments */
/*  ========= */

/*  UPLO    (input) CHARACTER*1 */
/*          Specifies whether the matrix A is upper or lower triangular. */
/*          = 'U':  Upper triangular */
/*          = 'L':  Lower triangular */

/*  DIAG    (input) CHARACTER*1 */
/*          Specifies whether or not the matrix A is unit triangular. */
/*          = 'N':  Non-unit triangular */
/*          = 'U':  Unit triangular */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  A       (input/output) DOUBLE PRECISION array, dimension (LDA,N) */
/*          On entry, the triangular matrix A.  If UPLO = 'U', the */
/*          leading n by n upper triangular part of the array A contains */
/*          the upper triangular matrix, and the strictly lower */
/*          triangular part of A is not referenced.  If UPLO = 'L', the */
/*          leading n by n lower triangular part of the array A contains */
/*          the lower triangular matrix, and the strictly upper */
/*          triangular part of A is not referenced.  If DIAG = 'U', the */
/*          diagonal elements of A are also not referenced and are */
/*          assumed to be 1. */

/*          On exit, the (triangular) inverse of the original matrix, in */
/*          the same storage format. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  INFO    (output) INTEGER */
/*          = 0: successful exit */
/*          < 0: if INFO = -k, the k-th argument had an illegal value */

/*  ===================================================================== */

#endif /*_NUMlapack_h_ */
