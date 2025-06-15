/* linalg/apply_givens.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2007 Gerard Jungman, Brian Gough
 * Copyright (C) 2004 Joerg Wensch, modifications for LQ.
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

inline static void
apply_givens_qr (size_t M, size_t N, gsl_matrix * Q, gsl_matrix * R,
                 size_t i, size_t j, double c, double s)
{
  size_t k;

  /* Apply rotation to matrix Q,  Q' = Q G */

#if USE_BLAS
  {
    gsl_matrix_view Q0M = gsl_matrix_submatrix(Q,0,0,M,j+1);
    gsl_vector_view Qi = gsl_matrix_column(&Q0M.matrix,i);
    gsl_vector_view Qj = gsl_matrix_column(&Q0M.matrix,j);
    gsl_blas_drot(&Qi.vector, &Qj.vector, c, -s);
  }
#else
  for (k = 0; k < M; k++)
    {
      double qki = gsl_matrix_get (Q, k, i);
      double qkj = gsl_matrix_get (Q, k, j);
      gsl_matrix_set (Q, k, i, qki * c - qkj * s);
      gsl_matrix_set (Q, k, j, qki * s + qkj * c);
    }
#endif

  /* Apply rotation to matrix R, R' = G^T R (note: upper triangular so
     zero for column < row) */

#if USE_BLAS
  {
    k = GSL_MIN(i,j);
    gsl_matrix_view R0 = gsl_matrix_submatrix(R, 0, k, j+1, N-k);
    gsl_vector_view Ri = gsl_matrix_row(&R0.matrix,i);
    gsl_vector_view Rj = gsl_matrix_row(&R0.matrix,j);
    gsl_blas_drot(&Ri.vector, &Rj.vector, c, -s);
  }
#else
  for (k = GSL_MIN (i, j); k < N; k++)
    {
      double rik = gsl_matrix_get (R, i, k);
      double rjk = gsl_matrix_get (R, j, k);
      gsl_matrix_set (R, i, k, c * rik - s * rjk);
      gsl_matrix_set (R, j, k, s * rik + c * rjk);
    }
#endif
}

inline static void
apply_givens_lq (size_t M, size_t N, gsl_matrix * Q, gsl_matrix * L,
                 size_t i, size_t j, double c, double s)
{
  size_t k;

  /* Apply rotation to matrix Q,  Q' = G Q */

#if USE_BLAS
  {
    gsl_matrix_view Q0M = gsl_matrix_submatrix(Q,0,0,j+1,M);
    gsl_vector_view Qi = gsl_matrix_row(&Q0M.matrix,i);
    gsl_vector_view Qj = gsl_matrix_row(&Q0M.matrix,j);
    gsl_blas_drot(&Qi.vector, &Qj.vector, c, -s);
  }
#else
  for (k = 0; k < M; k++)
    {
      double qik = gsl_matrix_get (Q, i, k);
      double qjk = gsl_matrix_get (Q, j, k);
      gsl_matrix_set (Q, i, k, qik * c - qjk * s);
      gsl_matrix_set (Q, j, k, qik * s + qjk * c);
    }
#endif

  /* Apply rotation to matrix L, L' = L G^T (note: lower triangular so
     zero for column > row) */

#if USE_BLAS
  {
    k = GSL_MIN(i,j);
    gsl_matrix_view L0 = gsl_matrix_submatrix(L, k, 0, N-k, j+1);
    gsl_vector_view Li = gsl_matrix_column(&L0.matrix,i);
    gsl_vector_view Lj = gsl_matrix_column(&L0.matrix,j);
    gsl_blas_drot(&Li.vector, &Lj.vector, c, -s);
  }
#else
  for (k = GSL_MIN (i, j); k < N; k++)
    {
      double lki = gsl_matrix_get (L, k, i);
      double lkj = gsl_matrix_get (L, k, j);
      gsl_matrix_set (L, k, i, c * lki - s * lkj);
      gsl_matrix_set (L, k, j, s * lki + c * lkj);
    }
#endif
}

inline static void
apply_givens_vec (gsl_vector * v, size_t i, size_t j, double c, double s)
{
  /* Apply rotation to vector v' = G^T v */

  double vi = gsl_vector_get (v, i);
  double vj = gsl_vector_get (v, j);
  gsl_vector_set (v, i, c * vi - s * vj);
  gsl_vector_set (v, j, s * vi + c * vj);
}

