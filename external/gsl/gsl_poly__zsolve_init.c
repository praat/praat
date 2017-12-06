/* poly/zsolve_init.c
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
#include <math.h>
#include "gsl_math.h"
#include "gsl_complex.h"
#include "gsl_poly.h"
#include "gsl_errno.h"

gsl_poly_complex_workspace * 
gsl_poly_complex_workspace_alloc (size_t n)
{
  size_t nc ;

  gsl_poly_complex_workspace * w ;
  
  if (n == 0)
    {
      GSL_ERROR_VAL ("matrix size n must be positive integer", GSL_EDOM, 0);
    }

  w = (gsl_poly_complex_workspace *) 
    malloc (sizeof(gsl_poly_complex_workspace));

  if (w == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for struct", GSL_ENOMEM, 0);
    }

  nc = n - 1;

  w->nc = nc;

  w->matrix = (double *) malloc (nc * nc * sizeof(double));

  if (w->matrix == 0)
    {
      free (w) ;       /* error in constructor, avoid memory leak */
      
      GSL_ERROR_VAL ("failed to allocate space for workspace matrix", 
                        GSL_ENOMEM, 0);
    }

  return w ;
}

void 
gsl_poly_complex_workspace_free (gsl_poly_complex_workspace * w)
{
  free(w->matrix) ;
  free(w);
}
