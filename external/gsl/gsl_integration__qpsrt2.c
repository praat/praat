/* integration/qpsrt2.c
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

/* The smallest interval has the largest error.  Before bisecting
   decrease the sum of the errors over the larger intervals
   (error_over_large_intervals) and perform extrapolation. */

static int
increase_nrmax (gsl_integration_workspace * workspace);

static int
increase_nrmax (gsl_integration_workspace * workspace)
{
  int k;
  int id = workspace->nrmax;
  int jupbnd;

  const size_t * level = workspace->level;
  const size_t * order = workspace->order;

  size_t limit = workspace->limit ;
  size_t last = workspace->size - 1 ;

  if (last > (1 + limit / 2))
    {
      jupbnd = limit + 1 - last;
    }
  else
    {
      jupbnd = last;
    }
  
  for (k = id; k <= jupbnd; k++)
    {
      size_t i_max = order[workspace->nrmax];
      
      workspace->i = i_max ;

      if (level[i_max] < workspace->maximum_level)
        {
          return 1;
        }

      workspace->nrmax++;

    }
  return 0;
}

static int
large_interval (gsl_integration_workspace * workspace)
{
  size_t i = workspace->i ;
  const size_t * level = workspace->level;
  
  if (level[i] < workspace->maximum_level)
    {
      return 1 ;
    }
  else
    {
      return 0 ;
    }
}

