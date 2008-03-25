/* integration/ptsort.c
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

static void
sort_results (gsl_integration_workspace * workspace);

static void
sort_results (gsl_integration_workspace * workspace)
{
  size_t i;
  
  double * elist = workspace->elist ;
  size_t * order = workspace->order ;

  size_t nint = workspace->size;

  for (i = 0; i < nint; i++)
    {
      size_t i1 = order[i];
      double e1 = elist[i1];
      size_t i_max = i1;
      size_t j;

      for (j = i + 1; j < nint; j++)
        {
          size_t i2 = order[j];
          double e2 = elist[i2];

          if (e2 >= e1)
            {
              i_max = i2;
              e1 = e2;
            }
        }

      if (i_max != i1)
        {
          order[i] = order[i_max];
          order[i_max] = i1;
        }
    }

  workspace->i = order[0] ;
}


