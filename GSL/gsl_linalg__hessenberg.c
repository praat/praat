/* linalg/hessenberg.c
 * 
 * Copyright (C) 2006 Patrick Alken
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "gsl__config.h"
#include "gsl_linalg.h"
#include "gsl_matrix.h"
#include "gsl_vector.h"

/*
gsl_linalg_hessenberg_decomp()
  Compute the Householder reduction to Hessenberg form of a
square N-by-N matrix A.

H = U^t A U

See Golub & Van Loan, "Matrix Computations" (3rd ed), algorithm
7.4.2

Inputs: A   - matrix to reduce
        tau - where to store scalar factors in Householder
              matrices; this vector must be of length N,
              where N is the order of A

Return: GSL_SUCCESS unless error occurs

Notes: on output, the upper triangular portion of A (including
the diagaonal and subdiagonal) contains the Hessenberg matrix.
The lower triangular portion (below the subdiagonal) contains
the Householder vectors which can be used to construct
the similarity transform matrix U.

The matrix U is

U = U(1) U(2) ... U(n - 2)

where

U(i) = I - tau(i) * v(i) * v(i)^t

and the vector v(i) is stored in column i of the matrix A
underneath the subdiagonal. So the first element of v(i)
is stored in row i + 2, column i, the second element at
row i + 3, column i, and so on.

Also note that for the purposes of computing U(i),
v(1:i) = 0, v(i + 1) = 1, and v(i+2:n) is what is stored in
column i of A beneath the subdiagonal.
*/

int
gsl_linalg_hessenberg_decomp(gsl_matrix *A, gsl_vector *tau)
{
  const size_t N = A->size1;

  if (N != A->size2)
    {
      GSL_ERROR ("Hessenberg reduction requires square matrix",
                 GSL_ENOTSQR);
    }
  else if (N != tau->size)
    {
      GSL_ERROR ("tau vector must match matrix size", GSL_EBADLEN);
    }
  else if (N < 3)
    {
      /* nothing to do */
      return GSL_SUCCESS;
    }
  else
    {
      size_t i;           /* looping */
      gsl_vector_view c,  /* matrix column */
                      hv; /* householder vector */
      gsl_matrix_view m;
      double tau_i;       /* beta in algorithm 7.4.2 */

      for (i = 0; i < N - 2; ++i)
        {
          /*
           * make a copy of A(i + 1:n, i) and store it in the section
           * of 'tau' that we haven't stored coefficients in yet
           */

          c = gsl_matrix_subcolumn(A, i, i + 1, N - i - 1);

          hv = gsl_vector_subvector(tau, i + 1, N - (i + 1));
          gsl_vector_memcpy(&hv.vector, &c.vector);

          /* compute householder transformation of A(i+1:n,i) */
          tau_i = gsl_linalg_householder_transform(&hv.vector);

          /* apply left householder matrix (I - tau_i v v') to A */
          m = gsl_matrix_submatrix(A, i + 1, i, N - (i + 1), N - i);
          gsl_linalg_householder_hm(tau_i, &hv.vector, &m.matrix);

          /* apply right householder matrix (I - tau_i v v') to A */
          m = gsl_matrix_submatrix(A, 0, i + 1, N, N - (i + 1));
          gsl_linalg_householder_mh(tau_i, &hv.vector, &m.matrix);

          /* save Householder coefficient */
          gsl_vector_set(tau, i, tau_i);

          /*
           * store Householder vector below the subdiagonal in column
           * i of the matrix. hv(1) does not need to be stored since
           * it is always 1.
           */
          c = gsl_vector_subvector(&c.vector, 1, c.vector.size - 1);
          hv = gsl_vector_subvector(&hv.vector, 1, hv.vector.size - 1);
          gsl_vector_memcpy(&c.vector, &hv.vector);
        }

      return GSL_SUCCESS;
    }
} /* gsl_linalg_hessenberg_decomp() */

/*
gsl_linalg_hessenberg_unpack()
  Construct the matrix U which transforms a matrix A into
its upper Hessenberg form:

H = U^t A U

by unpacking the information stored in H from gsl_linalg_hessenberg().

U is a product of Householder matrices:

U = U(1) U(2) ... U(n - 2)

where

U(i) = I - tau(i) * v(i) * v(i)^t

The v(i) are stored in the lower triangular part of H by
gsl_linalg_hessenberg(). The tau(i) are stored in the vector tau.

Inputs: H       - Hessenberg matrix computed from
                  gsl_linalg_hessenberg()
        tau     - tau vector computed from gsl_linalg_hessenberg()
        U       - (output) where to store similarity matrix

Return: success or error
*/

int
gsl_linalg_hessenberg_unpack(gsl_matrix * H, gsl_vector * tau,
                             gsl_matrix * U)
{
  int s;

  gsl_matrix_set_identity(U);

  s = gsl_linalg_hessenberg_unpack_accum(H, tau, U);

  return s;
} /* gsl_linalg_hessenberg_unpack() */

/*
gsl_linalg_hessenberg_unpack_accum()
  This routine is the same as gsl_linalg_hessenberg_unpack(), except
instead of storing the similarity matrix in U, it accumulates it,
so that

U -> U * [ U(1) U(2) ... U(n - 2) ]

instead of:

U -> U(1) U(2) ... U(n - 2)

Inputs: H       - Hessenberg matrix computed from
                  gsl_linalg_hessenberg()
        tau     - tau vector computed from gsl_linalg_hessenberg()
        V       - (input/output) where to accumulate similarity matrix

Return: success or error

Notes: 1) On input, V needs to be initialized. The Householder matrices
          are accumulated into V, so on output,

            V_out = V_in * U(1) * U(2) * ... * U(n - 2)

          so if you just want the product of the Householder matrices,
          initialize V to the identity matrix before calling this
          function.

       2) V does not have to be square, but must have the same
          number of columns as the order of H
*/

int
gsl_linalg_hessenberg_unpack_accum(gsl_matrix * H, gsl_vector * tau,
                                   gsl_matrix * V)
{
  const size_t N = H->size1;

  if (N != H->size2)
    {
      GSL_ERROR ("Hessenberg reduction requires square matrix",
                 GSL_ENOTSQR);
    }
  else if (N != tau->size)
    {
      GSL_ERROR ("tau vector must match matrix size", GSL_EBADLEN);
    }
  else if (N != V->size2)
    {
      GSL_ERROR ("V matrix has wrong dimension", GSL_EBADLEN);
    }
  else
    {
      size_t j;           /* looping */
      double tau_j;       /* householder coefficient */
      gsl_vector_view c,  /* matrix column */
                      hv; /* householder vector */
      gsl_matrix_view m;

      if (N < 3)
        {
          /* nothing to do */
          return GSL_SUCCESS;
        }

      for (j = 0; j < (N - 2); ++j)
        {
          c = gsl_matrix_column(H, j);

          tau_j = gsl_vector_get(tau, j);

          /*
           * get a view to the householder vector in column j, but
           * make sure hv(2) starts at the element below the
           * subdiagonal, since hv(1) was never stored and is always
           * 1
           */
          hv = gsl_vector_subvector(&c.vector, j + 1, N - (j + 1));

          /*
           * Only operate on part of the matrix since the first
           * j + 1 entries of the real householder vector are 0
           *
           * V -> V * U(j)
           *
           * Note here that V->size1 is not necessarily equal to N
           */
          m = gsl_matrix_submatrix(V, 0, j + 1, V->size1, N - (j + 1));

          /* apply right Householder matrix to V */
          gsl_linalg_householder_mh(tau_j, &hv.vector, &m.matrix);
        }

      return GSL_SUCCESS;
    }
} /* gsl_linalg_hessenberg_unpack_accum() */

/*
gsl_linalg_hessenberg_set_zero()
  Zero out the lower triangular portion of the Hessenberg matrix H.
This is useful when Householder vectors may be stored in the lower
part of H, but eigenvalue solvers need some scratch space with zeros.
*/

int
gsl_linalg_hessenberg_set_zero(gsl_matrix * H)
{
  const size_t N = H->size1;
  size_t i, j;

  if (N < 3)
    return GSL_SUCCESS;

  for (j = 0; j < N - 2; ++j)
    {
      for (i = j + 2; i < N; ++i)
        {
          gsl_matrix_set(H, i, j, 0.0);
        }
    }

  return GSL_SUCCESS;
} /* gsl_linalg_hessenberg_set_zero() */

/*
gsl_linalg_hessenberg_submatrix()

  This routine does the same thing as gsl_linalg_hessenberg(),
except that it operates on a submatrix of a larger matrix, but
updates the larger matrix with the Householder transformations.

For example, suppose

M = [ M_{11} | M_{12} | M_{13} ]
    [   0    |   A    | M_{23} ]
    [   0    |   0    | M_{33} ]

where M_{11} and M_{33} are already in Hessenberg form, and we
just want to reduce A to Hessenberg form. Applying the transformations
to A alone will cause the larger matrix M to lose its similarity
information. So this routine updates M_{12} and M_{23} as A gets
reduced.

Inputs: M   - total matrix
        A   - (sub)matrix to reduce
        top - row index of top of A in M
        tau - where to store scalar factors in Householder
              matrices; this vector must be of length N,
              where N is the order of A

Return: GSL_SUCCESS unless error occurs

Notes: on output, the upper triangular portion of A (including
the diagaonal and subdiagonal) contains the Hessenberg matrix.
The lower triangular portion (below the subdiagonal) contains
the Householder vectors which can be used to construct
the similarity transform matrix U.

The matrix U is

U = U(1) U(2) ... U(n - 2)

where

U(i) = I - tau(i) * v(i) * v(i)^t

and the vector v(i) is stored in column i of the matrix A
underneath the subdiagonal. So the first element of v(i)
is stored in row i + 2, column i, the second element at
row i + 3, column i, and so on.

Also note that for the purposes of computing U(i),
v(1:i) = 0, v(i + 1) = 1, and v(i+2:n) is what is stored in
column i of A beneath the subdiagonal.
*/

int
gsl_linalg_hessenberg_submatrix(gsl_matrix *M, gsl_matrix *A, size_t top,
                                gsl_vector *tau)
{
  const size_t N = A->size1;
  const size_t N_M = M->size1;

  if (N != A->size2)
    {
      GSL_ERROR ("Hessenberg reduction requires square matrix",
                 GSL_ENOTSQR);
    }
  else if (N != tau->size)
    {
      GSL_ERROR ("tau vector must match matrix size", GSL_EBADLEN);
    }
  else if (N < 3)
    {
      /* nothing to do */
      return GSL_SUCCESS;
    }
  else
    {
      size_t i;           /* looping */
      gsl_vector_view c,  /* matrix column */
                      hv; /* householder vector */
      gsl_matrix_view m;
      double tau_i;       /* beta in algorithm 7.4.2 */

      for (i = 0; i < N - 2; ++i)
        {
          /*
           * make a copy of A(i + 1:n, i) and store it in the section
           * of 'tau' that we haven't stored coefficients in yet
           */

          c = gsl_matrix_subcolumn(A, i, i + 1, N - i - 1);

          hv = gsl_vector_subvector(tau, i + 1, N - (i + 1));
          gsl_vector_memcpy(&hv.vector, &c.vector);

          /* compute householder transformation of A(i+1:n,i) */
          tau_i = gsl_linalg_householder_transform(&hv.vector);

          /*
           * apply left householder matrix (I - tau_i v v') to
           * [ A | M_{23} ]
           */
          m = gsl_matrix_submatrix(M,
                                   top + i + 1,
                                   top + i,
                                   N - (i + 1),
                                   N_M - top - i);
          gsl_linalg_householder_hm(tau_i, &hv.vector, &m.matrix);

          /*
           * apply right householder matrix (I - tau_i v v') to
           *
           * [ M_{12} ]
           * [   A    ]
           */
          m = gsl_matrix_submatrix(M,
                                   0,
                                   top + i + 1,
                                   top + N,
                                   N - (i + 1));
          gsl_linalg_householder_mh(tau_i, &hv.vector, &m.matrix);

          /* save Householder coefficient */
          gsl_vector_set(tau, i, tau_i);

          /*
           * store Householder vector below the subdiagonal in column
           * i of the matrix. hv(1) does not need to be stored since
           * it is always 1.
           */
          c = gsl_vector_subvector(&c.vector, 1, c.vector.size - 1);
          hv = gsl_vector_subvector(&hv.vector, 1, hv.vector.size - 1);
          gsl_vector_memcpy(&c.vector, &hv.vector);
        }

      return GSL_SUCCESS;
    }
} /* gsl_linalg_hessenberg_submatrix() */
