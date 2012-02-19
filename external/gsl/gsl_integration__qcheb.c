/* integration/qcheb.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Brian Gough
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
#include <stdlib.h>
#include "gsl_math.h"
#include "gsl_integration.h"


/* This function computes the 12-th order and 24-th order Chebyshev
   approximations to f(x) on [a,b] */

void
gsl_integration_qcheb (gsl_function * f, double a, double b, double *cheb12, double *cheb24)
{
  size_t i;
  double fval[25], v[12];

  /* These are the values of cos(pi*k/24) for k=1..11 needed for the
     Chebyshev expansion of f(x) */
  
  const double x[11] = { 0.9914448613738104,     
                         0.9659258262890683,
                         0.9238795325112868,     
                         0.8660254037844386,
                         0.7933533402912352,     
                         0.7071067811865475,
                         0.6087614290087206,     
                         0.5000000000000000,
                         0.3826834323650898,     
                         0.2588190451025208,
                         0.1305261922200516 };
  
  const double center = 0.5 * (b + a);
  const double half_length =  0.5 * (b - a);
  
  fval[0] = 0.5 * GSL_FN_EVAL (f, b);
  fval[12] = GSL_FN_EVAL (f, center);
  fval[24] = 0.5 * GSL_FN_EVAL (f, a);

  for (i = 1; i < 12; i++)
    {
      const size_t j = 24 - i;
      const double u = half_length * x[i-1];
      fval[i] = GSL_FN_EVAL(f, center + u);
      fval[j] = GSL_FN_EVAL(f, center - u);
    }

  for (i = 0; i < 12; i++)
    {
      const size_t j = 24 - i;
      v[i] = fval[i] - fval[j];
      fval[i] = fval[i] + fval[j];
    }

  {
    const double alam1 = v[0] - v[8];
    const double alam2 = x[5] * (v[2] - v[6] - v[10]);

    cheb12[3] = alam1 + alam2;
    cheb12[9] = alam1 - alam2;
  }

  {
    const double alam1 = v[1] - v[7] - v[9];
    const double alam2 = v[3] - v[5] - v[11];
    {
      const double alam = x[2] * alam1 + x[8] * alam2;

      cheb24[3] = cheb12[3] + alam;
      cheb24[21] = cheb12[3] - alam;
    }

    {
      const double alam = x[8] * alam1 - x[2] * alam2;
      cheb24[9] = cheb12[9] + alam;
      cheb24[15] = cheb12[9] - alam;
    }
  }

  {
    const double part1 = x[3] * v[4];
    const double part2 = x[7] * v[8];
    const double part3 = x[5] * v[6];
    
    {
      const double alam1 = v[0] + part1 + part2;
      const double alam2 = x[1] * v[2] + part3 + x[9] * v[10];
      
      cheb12[1] = alam1 + alam2;
      cheb12[11] = alam1 - alam2;
    }
    
    {
      const double alam1 = v[0] - part1 + part2;
      const double alam2 = x[9] * v[2] - part3 + x[1] * v[10];
      cheb12[5] = alam1 + alam2;
      cheb12[7] = alam1 - alam2;
    }
  }

  {
    const double alam = (x[0] * v[1] + x[2] * v[3] + x[4] * v[5]
                + x[6] * v[7] + x[8] * v[9] + x[10] * v[11]);
    cheb24[1] = cheb12[1] + alam;
    cheb24[23] = cheb12[1] - alam;
  }

  {
    const double alam = (x[10] * v[1] - x[8] * v[3] + x[6] * v[5] 
                - x[4] * v[7] + x[2] * v[9] - x[0] * v[11]);
    cheb24[11] = cheb12[11] + alam;
    cheb24[13] = cheb12[11] - alam;
  }

  {
    const double alam = (x[4] * v[1] - x[8] * v[3] - x[0] * v[5] 
                - x[10] * v[7] + x[2] * v[9] + x[6] * v[11]);
    cheb24[5] = cheb12[5] + alam;
    cheb24[19] = cheb12[5] - alam;
  }

  {
    const double alam = (x[6] * v[1] - x[2] * v[3] - x[10] * v[5] 
                + x[0] * v[7] - x[8] * v[9] - x[4] * v[11]);
    cheb24[7] = cheb12[7] + alam;
    cheb24[17] = cheb12[7] - alam;
  }

  for (i = 0; i < 6; i++)
    {
      const size_t j = 12 - i;
      v[i] = fval[i] - fval[j];
      fval[i] = fval[i] + fval[j];
    }

  {
    const double alam1 = v[0] + x[7] * v[4];
    const double alam2 = x[3] * v[2];

    cheb12[2] = alam1 + alam2;
    cheb12[10] = alam1 - alam2;
  }

  cheb12[6] = v[0] - v[4];

  {
    const double alam = x[1] * v[1] + x[5] * v[3] + x[9] * v[5];
    cheb24[2] = cheb12[2] + alam;
    cheb24[22] = cheb12[2] - alam;
  }

  {
    const double alam = x[5] * (v[1] - v[3] - v[5]);
    cheb24[6] = cheb12[6] + alam;
    cheb24[18] = cheb12[6] - alam;
  }

  {
    const double alam = x[9] * v[1] - x[5] * v[3] + x[1] * v[5];
    cheb24[10] = cheb12[10] + alam;
    cheb24[14] = cheb12[10] - alam;
  }

  for (i = 0; i < 3; i++)
    {
      const size_t j = 6 - i;
      v[i] = fval[i] - fval[j];
      fval[i] = fval[i] + fval[j];
    }

  cheb12[4] = v[0] + x[7] * v[2];
  cheb12[8] = fval[0] - x[7] * fval[2];

  {
    const double alam = x[3] * v[1];
    cheb24[4] = cheb12[4] + alam;
    cheb24[20] = cheb12[4] - alam;
  }

  {
    const double alam = x[7] * fval[1] - fval[3];
    cheb24[8] = cheb12[8] + alam;
    cheb24[16] = cheb12[8] - alam;
  }

  cheb12[0] = fval[0] + fval[2];

  {
    const double alam = fval[1] + fval[3];
    cheb24[0] = cheb12[0] + alam;
    cheb24[24] = cheb12[0] - alam;
  }

  cheb12[12] = v[0] - v[2];
  cheb24[12] = cheb12[12];

  for (i = 1; i < 12; i++)
    {
      cheb12[i] *= 1.0 / 6.0;
    }

  cheb12[0] *= 1.0 / 12.0;
  cheb12[12] *= 1.0 / 12.0;

  for (i = 1; i < 24; i++)
    {
      cheb24[i] *= 1.0 / 12.0;
    }

  cheb24[0] *= 1.0 / 24.0;
  cheb24[24] *= 1.0 / 24.0;
}
