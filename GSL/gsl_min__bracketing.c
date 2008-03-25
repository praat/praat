/* min/bracketing.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Fabrice Rossi
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

/* bracketing.c -- find an initial bracketing interval for a function to minimize */

#include "gsl__config.h"
#include "gsl_math.h"
#include "gsl_errno.h"
#include "gsl_min.h"
#include "gsl_machine.h"

#include "gsl_min__min.h"

int 
gsl_min_find_bracket(gsl_function *f,double *x_minimum,double * f_minimum,
                     double * x_lower, double * f_lower, 
                     double * x_upper, double * f_upper,
                     size_t eval_max)
{
  /* The three following variables must be declared volatile to avoid storage
     in extended precision registers available on some architecture. The code
     relies on the ability to compare double values. As the values will be
     store in regular memory, the extended precision will then be lost and
     values that are different in extended precision might have equal
     representation in double precision. This behavior might break the
     algorithm. 
   */
  volatile double f_left = *f_lower;
  volatile double f_right = *f_upper;
  volatile double f_center;
  double x_left = *x_lower;
  double x_right= *x_upper; 
  double x_center;
  const double golden = 0.3819660;      /* golden = (3 - sqrt(5))/2 */
  size_t nb_eval = 0;
  
  
  if (f_right >= f_left) 
    {
      x_center = (x_right - x_left) * golden + x_left;
      nb_eval++;
      SAFE_FUNC_CALL (f, x_center, &f_center);
    }
  else
    {
      x_center = x_right ;
      f_center = f_right ;
      x_right = (x_center - x_left) / golden + x_left;
      nb_eval++;
      SAFE_FUNC_CALL (f, x_right, &f_right);
    }
  
  do
    {
      if (f_center < f_left )
        {
          if (f_center < f_right)
            {
              *x_lower = x_left;
              *x_upper = x_right;
              *x_minimum = x_center;
              *f_lower = f_left;
              *f_upper = f_right;
              *f_minimum = f_center;
/*            gsl_ieee_printf_double (&f_left);
              printf(" ");
              gsl_ieee_printf_double (&f_center);
              printf("\n");*/
              return GSL_SUCCESS;
            }
          else if (f_center > f_right)
            {
              x_left = x_center;
              f_left = f_center;
              x_center = x_right;
              f_center = f_right;
              x_right = (x_center - x_left) / golden + x_left;
              nb_eval++;
              SAFE_FUNC_CALL (f, x_right, &f_right);
            }
          else /* f_center == f_right */
            {
              x_right = x_center;
              f_right = f_center;
              x_center = (x_right - x_left) * golden + x_left;
              nb_eval++;
              SAFE_FUNC_CALL (f, x_center, &f_center);
            }
        }
      else /* f_center >= f_left */
        {
          x_right = x_center;
          f_right = f_center;
          x_center = (x_right - x_left) * golden + x_left;
          nb_eval++;
          SAFE_FUNC_CALL (f, x_center, &f_center);
        }
    }
  while (nb_eval < eval_max 
         && (x_right - x_left) > GSL_SQRT_DBL_EPSILON * ( (x_right + x_left) * 0.5 ) + GSL_SQRT_DBL_EPSILON);
  *x_lower = x_left;
  *x_upper = x_right;
  *x_minimum = x_center;
  *f_lower = f_left;
  *f_upper = f_right;
  *f_minimum = f_center;
  return GSL_FAILURE;
}

