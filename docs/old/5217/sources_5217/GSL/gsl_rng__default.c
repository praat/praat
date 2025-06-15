/* rng/default.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
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
#include <stdio.h>
#include <string.h>
#include "gsl_rng.h"
#include "gsl_errno.h"

/* The initial defaults are defined in the file mt.c, so we can get
   access to the static parts of the default generator. */

const gsl_rng_type *
gsl_rng_env_setup (void)
{
  unsigned long int seed = 0;
  const char *p = getenv ("GSL_RNG_TYPE");

  if (p)
    {
      const gsl_rng_type **t, **t0 = gsl_rng_types_setup ();

      gsl_rng_default = 0;

      /* check GSL_RNG_TYPE against the names of all the generators */

      for (t = t0; *t != 0; t++)
        {
          if (strcmp (p, (*t)->name) == 0)
            {
              gsl_rng_default = *t;
              break;
            }
        }

      if (gsl_rng_default == 0)
        {
          int i = 0;

          fprintf (stderr, "GSL_RNG_TYPE=%s not recognized\n", p);
          fprintf (stderr, "Valid generator types are:\n");

          for (t = t0; *t != 0; t++)
            {
              fprintf (stderr, " %18s", (*t)->name);

              if ((++i) % 4 == 0)
                {
                  fputc ('\n', stderr);
                }
            }

          fputc ('\n', stderr);

          GSL_ERROR_VAL ("unknown generator", GSL_EINVAL, 0);
        }

      fprintf (stderr, "GSL_RNG_TYPE=%s\n", gsl_rng_default->name);
    }
  else
    {
      gsl_rng_default = gsl_rng_mt19937;
    }

  p = getenv ("GSL_RNG_SEED");

  if (p)
    {
      seed = strtoul (p, 0, 0);
      fprintf (stderr, "GSL_RNG_SEED=%lu\n", seed);
    };

  gsl_rng_default_seed = seed;

  return gsl_rng_default;
}
