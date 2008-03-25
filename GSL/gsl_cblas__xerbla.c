/* xerbla.c
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "gsl_cblas.h"
#include "gsl_cblas__cblas.h"

void
cblas_xerbla (int p, const char *rout, const char *form, ...)
{
  va_list ap;

  va_start (ap, form);

  if (p)
    {
      fprintf (stderr, "Parameter %d to routine %s was incorrect\n", p, rout);
    }

  vfprintf (stderr, form, ap);
  va_end (ap);

  abort ();
}
