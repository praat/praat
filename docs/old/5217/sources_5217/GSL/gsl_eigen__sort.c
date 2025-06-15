/* eigen/sort.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2006, 2007 Gerard Jungman, Patrick Alken
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

/* Author:  G. Jungman, Modified: B. Gough. */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_eigen.h"
#include "gsl_complex.h"
#include "gsl_complex_math.h"

/* The eigen_sort below is not very good, but it is simple and
 * self-contained. We can always implement an improved sort later.  */

int
gsl_eigen_symmv_sort (gsl_vector * eval, gsl_matrix * evec, 
                      gsl_eigen_sort_t sort_type)
{
  if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (eval->size != evec->size1)
    {
      GSL_ERROR ("eigenvalues must match eigenvector matrix", GSL_EBADLEN);
    }
  else
    {
      const size_t N = eval->size;
      size_t i;

      for (i = 0; i < N - 1; i++)
        {
          size_t j;
          size_t k = i;

          double ek = gsl_vector_get (eval, i);

          /* search for something to swap */
          for (j = i + 1; j < N; j++)
            {
              int test;
              const double ej = gsl_vector_get (eval, j);

              switch (sort_type)
                {       
                case GSL_EIGEN_SORT_VAL_ASC:
                  test = (ej < ek);
                  break;
                case GSL_EIGEN_SORT_VAL_DESC:
                  test = (ej > ek);
                  break;
                case GSL_EIGEN_SORT_ABS_ASC:
                  test = (fabs (ej) < fabs (ek));
                  break;
                case GSL_EIGEN_SORT_ABS_DESC:
                  test = (fabs (ej) > fabs (ek));
                  break;
                default:
                  GSL_ERROR ("unrecognized sort type", GSL_EINVAL);
                }

              if (test)
                {
                  k = j;
                  ek = ej;
                }
            }

          if (k != i)
            {
              /* swap eigenvalues */
              gsl_vector_swap_elements (eval, i, k);

              /* swap eigenvectors */
              gsl_matrix_swap_columns (evec, i, k);
            }
        }

      return GSL_SUCCESS;
    }
}


int
gsl_eigen_hermv_sort (gsl_vector * eval, gsl_matrix_complex * evec, 
                      gsl_eigen_sort_t sort_type)
{
  if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (eval->size != evec->size1)
    {
      GSL_ERROR ("eigenvalues must match eigenvector matrix", GSL_EBADLEN);
    }
  else
    {
      const size_t N = eval->size;
      size_t i;

      for (i = 0; i < N - 1; i++)
        {
          size_t j;
          size_t k = i;

          double ek = gsl_vector_get (eval, i);

          /* search for something to swap */
          for (j = i + 1; j < N; j++)
            {
              int test;
              const double ej = gsl_vector_get (eval, j);

              switch (sort_type)
                {       
                case GSL_EIGEN_SORT_VAL_ASC:
                  test = (ej < ek);
                  break;
                case GSL_EIGEN_SORT_VAL_DESC:
                  test = (ej > ek);
                  break;
                case GSL_EIGEN_SORT_ABS_ASC:
                  test = (fabs (ej) < fabs (ek));
                  break;
                case GSL_EIGEN_SORT_ABS_DESC:
                  test = (fabs (ej) > fabs (ek));
                  break;
                default:
                  GSL_ERROR ("unrecognized sort type", GSL_EINVAL);
                }

              if (test)
                {
                  k = j;
                  ek = ej;
                }
            }

          if (k != i)
            {
              /* swap eigenvalues */
              gsl_vector_swap_elements (eval, i, k);

              /* swap eigenvectors */
              gsl_matrix_complex_swap_columns (evec, i, k);
            }
        }

      return GSL_SUCCESS;
    }
}

int
gsl_eigen_nonsymmv_sort (gsl_vector_complex * eval,
                         gsl_matrix_complex * evec, 
                         gsl_eigen_sort_t sort_type)
{
  if (evec && (evec->size1 != evec->size2))
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (evec && (eval->size != evec->size1))
    {
      GSL_ERROR ("eigenvalues must match eigenvector matrix", GSL_EBADLEN);
    }
  else
    {
      const size_t N = eval->size;
      size_t i;

      for (i = 0; i < N - 1; i++)
        {
          size_t j;
          size_t k = i;

          gsl_complex ek = gsl_vector_complex_get (eval, i);

          /* search for something to swap */
          for (j = i + 1; j < N; j++)
            {
              int test;
              const gsl_complex ej = gsl_vector_complex_get (eval, j);

              switch (sort_type)
                {       
                case GSL_EIGEN_SORT_ABS_ASC:
                  test = (gsl_complex_abs (ej) < gsl_complex_abs (ek));
                  break;
                case GSL_EIGEN_SORT_ABS_DESC:
                  test = (gsl_complex_abs (ej) > gsl_complex_abs (ek));
                  break;
                case GSL_EIGEN_SORT_VAL_ASC:
                case GSL_EIGEN_SORT_VAL_DESC:
                default:
                  GSL_ERROR ("invalid sort type", GSL_EINVAL);
                }

              if (test)
                {
                  k = j;
                  ek = ej;
                }
            }

          if (k != i)
            {
              /* swap eigenvalues */
              gsl_vector_complex_swap_elements (eval, i, k);

              /* swap eigenvectors */
              if (evec)
                gsl_matrix_complex_swap_columns (evec, i, k);
            }
        }

      return GSL_SUCCESS;
    }
}

int
gsl_eigen_gensymmv_sort (gsl_vector * eval, gsl_matrix * evec, 
                         gsl_eigen_sort_t sort_type)
{
  int s;

  s = gsl_eigen_symmv_sort(eval, evec, sort_type);

  return s;
}

int
gsl_eigen_genhermv_sort (gsl_vector * eval, gsl_matrix_complex * evec, 
                         gsl_eigen_sort_t sort_type)
{
  int s;

  s = gsl_eigen_hermv_sort(eval, evec, sort_type);

  return s;
}

int
gsl_eigen_genv_sort (gsl_vector_complex * alpha, gsl_vector * beta,
                     gsl_matrix_complex * evec, gsl_eigen_sort_t sort_type)
{
  if (evec->size1 != evec->size2)
    {
      GSL_ERROR ("eigenvector matrix must be square", GSL_ENOTSQR);
    }
  else if (alpha->size != evec->size1 || beta->size != evec->size1)
    {
      GSL_ERROR ("eigenvalues must match eigenvector matrix", GSL_EBADLEN);
    }
  else
    {
      const size_t N = alpha->size;
      size_t i;

      for (i = 0; i < N - 1; i++)
        {
          size_t j;
          size_t k = i;

          gsl_complex ak = gsl_vector_complex_get (alpha, i);
          double bk = gsl_vector_get(beta, i);
          gsl_complex ek;

          if (bk < GSL_DBL_EPSILON)
            {
              GSL_SET_COMPLEX(&ek,
                              GSL_SIGN(GSL_REAL(ak)) ? GSL_POSINF : GSL_NEGINF,
                              GSL_SIGN(GSL_IMAG(ak)) ? GSL_POSINF : GSL_NEGINF);
            }
          else
            ek = gsl_complex_div_real(ak, bk);

          /* search for something to swap */
          for (j = i + 1; j < N; j++)
            {
              int test;
              const gsl_complex aj = gsl_vector_complex_get (alpha, j);
              double bj = gsl_vector_get(beta, j);
              gsl_complex ej;

              if (bj < GSL_DBL_EPSILON)
                {
                  GSL_SET_COMPLEX(&ej,
                                  GSL_SIGN(GSL_REAL(aj)) ? GSL_POSINF : GSL_NEGINF,
                                  GSL_SIGN(GSL_IMAG(aj)) ? GSL_POSINF : GSL_NEGINF);
                }
              else
                ej = gsl_complex_div_real(aj, bj);

              switch (sort_type)
                {       
                case GSL_EIGEN_SORT_ABS_ASC:
                  test = (gsl_complex_abs (ej) < gsl_complex_abs (ek));
                  break;
                case GSL_EIGEN_SORT_ABS_DESC:
                  test = (gsl_complex_abs (ej) > gsl_complex_abs (ek));
                  break;
                case GSL_EIGEN_SORT_VAL_ASC:
                case GSL_EIGEN_SORT_VAL_DESC:
                default:
                  GSL_ERROR ("invalid sort type", GSL_EINVAL);
                }

              if (test)
                {
                  k = j;
                  ek = ej;
                }
            }

          if (k != i)
            {
              /* swap eigenvalues */
              gsl_vector_complex_swap_elements (alpha, i, k);
              gsl_vector_swap_elements (beta, i, k);

              /* swap eigenvectors */
              gsl_matrix_complex_swap_columns (evec, i, k);
            }
        }

      return GSL_SUCCESS;
    }
}
