/* poly/balance.c
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

static void balance_companion_matrix (double *m, size_t n);

#define RADIX 2
#define RADIX2 (RADIX*RADIX)

static void
balance_companion_matrix (double *m, size_t nc)
{
  int not_converged = 1;

  double row_norm = 0;
  double col_norm = 0;

  while (not_converged)
    {
      size_t i, j;
      double g, f, s;

      not_converged = 0;

      for (i = 0; i < nc; i++)
        {
          /* column norm, excluding the diagonal */

          if (i != nc - 1)
            {
              col_norm = fabs (MAT (m, i + 1, i, nc));
            }
          else
            {
              col_norm = 0;

              for (j = 0; j < nc - 1; j++)
                {
                  col_norm += fabs (MAT (m, j, nc - 1, nc));
                }
            }

          /* row norm, excluding the diagonal */

          if (i == 0)
            {
              row_norm = fabs (MAT (m, 0, nc - 1, nc));
            }
          else if (i == nc - 1)
            {
              row_norm = fabs (MAT (m, i, i - 1, nc));
            }
          else
            {
              row_norm = (fabs (MAT (m, i, i - 1, nc)) 
                          + fabs (MAT (m, i, nc - 1, nc)));
            }

          if (col_norm == 0 || row_norm == 0)
            {
              continue;
            }

          g = row_norm / RADIX;
          f = 1;
          s = col_norm + row_norm;

          while (col_norm < g)
            {
              f *= RADIX;
              col_norm *= RADIX2;
            }

          g = row_norm * RADIX;

          while (col_norm > g)
            {
              f /= RADIX;
              col_norm /= RADIX2;
            }

          if ((row_norm + col_norm) < 0.95 * s * f)
            {
              not_converged = 1;

              g = 1 / f;

              if (i == 0)
                {
                  MAT (m, 0, nc - 1, nc) *= g;
                }
              else
                {
                  MAT (m, i, i - 1, nc) *= g;
                  MAT (m, i, nc - 1, nc) *= g;
                }

              if (i == nc - 1)
                {
                  for (j = 0; j < nc; j++)
                    {
                      MAT (m, j, i, nc) *= f;
                    }
                }
              else
                {
                  MAT (m, i + 1, i, nc) *= f;
                }
            }
        }
    }
}
