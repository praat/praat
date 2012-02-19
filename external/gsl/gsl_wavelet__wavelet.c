/* wavelet/wavelet.c
 * 
 * Copyright (C) 2004 Ivo Alxneit
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
#include "gsl_errno.h"
#include "gsl_wavelet.h"

gsl_wavelet *
gsl_wavelet_alloc (const gsl_wavelet_type * T, size_t k)
{
  int status;

  gsl_wavelet *w = (gsl_wavelet *) malloc (sizeof (gsl_wavelet));

  if (w == NULL)
    {
      GSL_ERROR_VAL ("failed to allocate space for wavelet struct",
                     GSL_ENOMEM, 0);
    };

  w->type = T;

  status = (T->init) (&(w->h1), &(w->g1), &(w->h2), &(w->g2),
                      &(w->nc), &(w->offset), k);

  if (status)
    {
      free (w);
      GSL_ERROR_VAL ("invalid wavelet member", GSL_EINVAL, 0);
    }

  return w;
}

void
gsl_wavelet_free (gsl_wavelet * w)
{
  free (w);
}

const char *
gsl_wavelet_name (const gsl_wavelet * w)
{
  return w->type->name;
}


/* Let's not export this for now (BJG) */
#if 0
void
gsl_wavelet_print (const gsl_wavelet * w)
{
  size_t n = w->nc;
  size_t i;

  printf ("Wavelet type: %s\n", w->type->name);

  printf
    (" h1(%d):%12.8f   g1(%d):%12.8f       h2(%d):%12.8f   g2(%d):%12.8f\n",
     0, w->h1[0], 0, w->g1[0], 0, w->h2[0], 0, w->g2[0]);

  for (i = 1; i < (n < 10 ? n : 10); i++)
    {
      printf
        (" h1(%d):%12.8f   g1(%d):%12.8f       h2(%d):%12.8f   g2(%d):%12.8f\n",
         i, w->h1[i], i, w->g1[i], i, w->h2[i], i, w->g2[i]);
    }

  for (; i < n; i++)
    {
      printf
        ("h1(%d):%12.8f  g1(%d):%12.8f      h2(%d):%12.8f  g2(%d):%12.8f\n",
         i, w->h1[i], i, w->g1[i], i, w->h2[i], i, w->g2[i]);
    }
}
#endif

gsl_wavelet_workspace *
gsl_wavelet_workspace_alloc (size_t n)
{
  gsl_wavelet_workspace *work;

  if (n == 0)
    {
      GSL_ERROR_VAL ("length n must be positive integer", GSL_EDOM, 0);
    }

  work = (gsl_wavelet_workspace *) malloc (sizeof (gsl_wavelet_workspace));

  if (work == NULL)
    {
      GSL_ERROR_VAL ("failed to allocate struct", GSL_ENOMEM, 0);
    }

  work->n = n;
  work->scratch = (double *) malloc (n * sizeof (double));

  if (work->scratch == NULL)
    {
      /* error in constructor, prevent memory leak */
      free (work);
      GSL_ERROR_VAL ("failed to allocate scratch space", GSL_ENOMEM, 0);
    }

  return work;
}

void
gsl_wavelet_workspace_free (gsl_wavelet_workspace * work)
{
  /* release scratch space */
  free (work->scratch);
  work->scratch = NULL;
  free (work);
}
