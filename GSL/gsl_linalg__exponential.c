/* linalg/exponential.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002, 2007 Gerard Jungman, Brian Gough
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

/* Calculate the matrix exponential, following
 * Moler + Van Loan, SIAM Rev. 20, 801 (1978).
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_mode.h"
#include "gsl_errno.h"
#include "gsl_blas.h"

#include "gsl_linalg.h"


/* store one of the suggested choices for the
 * Taylor series / square  method from Moler + VanLoan
 */
struct moler_vanloan_optimal_suggestion
{
  int k;
  int j;
};
typedef  struct moler_vanloan_optimal_suggestion  mvl_suggestion_t;


/* table from Moler and Van Loan
 * mvl_tab[gsl_mode_t][matrix_norm_group]
 */
static mvl_suggestion_t mvl_tab[3][6] =
{
  /* double precision */
  {
    { 5, 1 }, { 5, 4 }, { 7, 5 }, { 9, 7 }, { 10, 10 }, { 8, 14 }
  },

  /* single precision */
  {
    { 2, 1 }, { 4, 0 }, { 7, 1 }, { 6, 5 }, { 5, 9 }, { 7, 11 }
  },

  /* approx precision */
  {
    { 1, 0 }, { 3, 0 }, { 5, 1 }, { 4, 5 }, { 4, 8 }, { 2, 11 }
  }
};


inline
static double
sup_norm(const gsl_matrix * A)
{
  double min, max;
  gsl_matrix_minmax(A, &min, &max);
  return GSL_MAX_DBL(fabs(min), fabs(max));
}


static
mvl_suggestion_t
obtain_suggestion(const gsl_matrix * A, gsl_mode_t mode)
{
  const unsigned int mode_prec = GSL_MODE_PREC(mode);
  const double norm_A = sup_norm(A);
  if(norm_A < 0.01) return mvl_tab[mode_prec][0];
  else if(norm_A < 0.1) return mvl_tab[mode_prec][1];
  else if(norm_A < 1.0) return mvl_tab[mode_prec][2];
  else if(norm_A < 10.0) return mvl_tab[mode_prec][3];
  else if(norm_A < 100.0) return mvl_tab[mode_prec][4];
  else if(norm_A < 1000.0) return mvl_tab[mode_prec][5];
  else
  {
    /* outside the table we simply increase the number
     * of squarings, bringing the reduced matrix into
     * the range of the table; this is obviously suboptimal,
     * but that is the price paid for not having those extra
     * table entries
     */
    const double extra = log(1.01*norm_A/1000.0) / M_LN2;
    const int extra_i = (unsigned int) ceil(extra);
    mvl_suggestion_t s = mvl_tab[mode][5];
    s.j += extra_i;
    return s;
  }
}


/* use series representation to calculate matrix exponential;
 * this is used for small matrices; we use the sup_norm
 * to measure the size of the terms in the expansion
 */
static void
matrix_exp_series(
  const gsl_matrix * B,
  gsl_matrix * eB,
  int number_of_terms
  )
{
  int count;
  gsl_matrix * temp = gsl_matrix_calloc(B->size1, B->size2);

  /* init the Horner polynomial evaluation,
   * eB = 1 + B/number_of_terms; we use
   * eB to collect the partial results
   */  
  gsl_matrix_memcpy(eB, B);
  gsl_matrix_scale(eB, 1.0/number_of_terms);
  gsl_matrix_add_diagonal(eB, 1.0);
  for(count = number_of_terms-1; count >= 1; --count)
  {
    /*  mult_temp = 1 + B eB / count  */
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, B, eB, 0.0, temp);
    gsl_matrix_scale(temp, 1.0/count);
    gsl_matrix_add_diagonal(temp, 1.0);

    /*  transfer partial result out of temp */
    gsl_matrix_memcpy(eB, temp);
  }

  /* now eB holds the full result; we're done */
  gsl_matrix_free(temp);
}


int
gsl_linalg_exponential_ss(
  const gsl_matrix * A,
  gsl_matrix * eA,
  gsl_mode_t mode
  )
{
  if(A->size1 != A->size2)
  {
    GSL_ERROR("cannot exponentiate a non-square matrix", GSL_ENOTSQR);
  }
  else if(A->size1 != eA->size1 || A->size2 != eA->size2)
  {
    GSL_ERROR("exponential of matrix must have same dimension as matrix", GSL_EBADLEN);
  }
  else
  {
    int i;
    const mvl_suggestion_t sugg = obtain_suggestion(A, mode);
    const double divisor = exp(M_LN2 * sugg.j);

    gsl_matrix * reduced_A = gsl_matrix_alloc(A->size1, A->size2);

    /*  decrease A by the calculated divisor  */
    gsl_matrix_memcpy(reduced_A, A);
    gsl_matrix_scale(reduced_A, 1.0/divisor);

    /*  calculate exp of reduced matrix; store in eA as temp  */
    matrix_exp_series(reduced_A, eA, sugg.k);

    /*  square repeatedly; use reduced_A for scratch */
    for(i = 0; i < sugg.j; ++i)
    {
      gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, eA, eA, 0.0, reduced_A);
      gsl_matrix_memcpy(eA, reduced_A);
    }

    gsl_matrix_free(reduced_A);

    return GSL_SUCCESS;
  }
}

