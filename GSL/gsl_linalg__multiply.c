/* linalg/multiply.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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

/* Author:  G. Jungman */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_linalg.h"

#define SWAP_SIZE_T(a, b)  do { size_t swap_tmp = a; a = b; b = swap_tmp; } while(0)

int
gsl_linalg_matmult (const gsl_matrix * A, const gsl_matrix * B, gsl_matrix * C)
{
  if (A->size2 != B->size1 || A->size1 != C->size1 || B->size2 != C->size2)
    {
      GSL_ERROR ("matrix sizes are not conformant", GSL_EBADLEN);
    }
  else
    {
      double a, b;
      double temp;
      size_t i, j, k;

      for (i = 0; i < C->size1; i++)
        {
          for (j = 0; j < C->size2; j++)
            {
              a = gsl_matrix_get (A, i, 0);
              b = gsl_matrix_get (B, 0, j);
              temp = a * b;
              for (k = 1; k < A->size2; k++)
                {
                  a = gsl_matrix_get (A, i, k);
                  b = gsl_matrix_get (B, k, j);
                  temp += a * b;
                }
              gsl_matrix_set (C, i, j, temp);
            }
        }

      return GSL_SUCCESS;
    }
}


int
gsl_linalg_matmult_mod (const gsl_matrix * A, gsl_linalg_matrix_mod_t modA,
                    const gsl_matrix * B, gsl_linalg_matrix_mod_t modB,
                    gsl_matrix * C)
{
  if (modA == GSL_LINALG_MOD_NONE && modB == GSL_LINALG_MOD_NONE)
    {
      return gsl_linalg_matmult (A, B, C);
    }
  else
    {
      size_t dim1_A = A->size1;
      size_t dim2_A = A->size2;
      size_t dim1_B = B->size1;
      size_t dim2_B = B->size2;
      size_t dim1_C = C->size1;
      size_t dim2_C = C->size2;

      if (modA & GSL_LINALG_MOD_TRANSPOSE)
        SWAP_SIZE_T (dim1_A, dim2_A);
      if (modB & GSL_LINALG_MOD_TRANSPOSE)
        SWAP_SIZE_T (dim1_B, dim2_B);

      if (dim2_A != dim1_B || dim1_A != dim1_C || dim2_B != dim2_C)
        {
          GSL_ERROR ("matrix sizes are not conformant", GSL_EBADLEN);
        }
      else
        {
          double a, b;
          double temp;
          size_t i, j, k;
          size_t a1, a2, b1, b2;

          for (i = 0; i < dim1_C; i++)
            {
              for (j = 0; j < dim2_C; j++)
                {
                  a1 = i;
                  a2 = 0;
                  b1 = 0;
                  b2 = j;
                  if (modA & GSL_LINALG_MOD_TRANSPOSE)
                    SWAP_SIZE_T (a1, a2);
                  if (modB & GSL_LINALG_MOD_TRANSPOSE)
                    SWAP_SIZE_T (b1, b2);

                  a = gsl_matrix_get (A, a1, a2);
                  b = gsl_matrix_get (B, b1, b2);
                  temp = a * b;

                  for (k = 1; k < dim2_A; k++)
                    {
                      a1 = i;
                      a2 = k;
                      b1 = k;
                      b2 = j;
                      if (modA & GSL_LINALG_MOD_TRANSPOSE)
                        SWAP_SIZE_T (a1, a2);
                      if (modB & GSL_LINALG_MOD_TRANSPOSE)
                        SWAP_SIZE_T (b1, b2);
                      a = gsl_matrix_get (A, a1, a2);
                      b = gsl_matrix_get (B, b1, b2);
                      temp += a * b;
                    }

                  gsl_matrix_set (C, i, j, temp);
                }
            }

          return GSL_SUCCESS;
        }
    }
}
