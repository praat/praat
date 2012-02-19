/* poly/qr.c
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

static int qr_companion (double *h, size_t nc, gsl_complex_packed_ptr z);

static int
qr_companion (double *h, size_t nc, gsl_complex_packed_ptr zroot)
{
  double t = 0.0;

  size_t iterations, e, i, j, k, m;

  double w, x, y, s, z;

  double p = 0, q = 0, r = 0; 

  /* FIXME: if p,q,r, are not set to zero then the compiler complains
     that they ``might be used uninitialized in this
     function''. Looking at the code this does seem possible, so this
     should be checked. */

  int notlast;

  size_t n = nc;

next_root:

  if (n == 0)
    return GSL_SUCCESS ;

  iterations = 0;

next_iteration:

  for (e = n; e >= 2; e--)
    {
      double a1 = fabs (FMAT (h, e, e - 1, nc));
      double a2 = fabs (FMAT (h, e - 1, e - 1, nc));
      double a3 = fabs (FMAT (h, e, e, nc));

      if (a1 <= GSL_DBL_EPSILON * (a2 + a3))
        break;
    }

  x = FMAT (h, n, n, nc);

  if (e == n)
    {
      GSL_SET_COMPLEX_PACKED (zroot, n-1, x + t, 0); /* one real root */
      n--;
      goto next_root;
      /*continue;*/
    }

  y = FMAT (h, n - 1, n - 1, nc);
  w = FMAT (h, n - 1, n, nc) * FMAT (h, n, n - 1, nc);

  if (e == n - 1)
    {
      p = (y - x) / 2;
      q = p * p + w;
      y = sqrt (fabs (q));

      x += t;

      if (q > 0)                /* two real roots */
        {
          if (p < 0)
            y = -y;
          y += p;

          GSL_SET_COMPLEX_PACKED (zroot, n-1, x - w / y, 0);
          GSL_SET_COMPLEX_PACKED (zroot, n-2, x + y, 0);
        }
      else
        {
          GSL_SET_COMPLEX_PACKED (zroot, n-1, x + p, -y);
          GSL_SET_COMPLEX_PACKED (zroot, n-2, x + p, y);
        }
      n -= 2;

      goto next_root;
      /*continue;*/
    }

  /* No more roots found yet, do another iteration */

  if (iterations == 60)  /* increased from 30 to 60 */
    {
      /* too many iterations - give up! */

      return GSL_FAILURE ;
    }

  if (iterations % 10 == 0 && iterations > 0)
    {
      /* use an exceptional shift */

      t += x;

      for (i = 1; i <= n; i++)
        {
          FMAT (h, i, i, nc) -= x;
        }

      s = fabs (FMAT (h, n, n - 1, nc)) + fabs (FMAT (h, n - 1, n - 2, nc));
      y = 0.75 * s;
      x = y;
      w = -0.4375 * s * s;
    }

  iterations++;

  for (m = n - 2; m >= e; m--)
    {
      double a1, a2, a3;

      z = FMAT (h, m, m, nc);
      r = x - z;
      s = y - z;
      p = FMAT (h, m, m + 1, nc) + (r * s - w) / FMAT (h, m + 1, m, nc);
      q = FMAT (h, m + 1, m + 1, nc) - z - r - s;
      r = FMAT (h, m + 2, m + 1, nc);
      s = fabs (p) + fabs (q) + fabs (r);
      p /= s;
      q /= s;
      r /= s;

      if (m == e)
        break;
      
      a1 = fabs (FMAT (h, m, m - 1, nc));
      a2 = fabs (FMAT (h, m - 1, m - 1, nc));
      a3 = fabs (FMAT (h, m + 1, m + 1, nc));

      if (a1 * (fabs (q) + fabs (r)) <= GSL_DBL_EPSILON * fabs (p) * (a2 + a3))
        break;
    }

  for (i = m + 2; i <= n; i++)
    {
      FMAT (h, i, i - 2, nc) = 0;
    }

  for (i = m + 3; i <= n; i++)
    {
      FMAT (h, i, i - 3, nc) = 0;
    }

  /* double QR step */

  for (k = m; k <= n - 1; k++)
    {
      notlast = (k != n - 1);

      if (k != m)
        {
          p = FMAT (h, k, k - 1, nc);
          q = FMAT (h, k + 1, k - 1, nc);
          r = notlast ? FMAT (h, k + 2, k - 1, nc) : 0.0;

          x = fabs (p) + fabs (q) + fabs (r);

          if (x == 0)
            continue;           /* FIXME????? */

          p /= x;
          q /= x;
          r /= x;
        }

      s = sqrt (p * p + q * q + r * r);

      if (p < 0)
        s = -s;

      if (k != m)
        {
          FMAT (h, k, k - 1, nc) = -s * x;
        }
      else if (e != m)
        {
          FMAT (h, k, k - 1, nc) *= -1;
        }

      p += s;
      x = p / s;
      y = q / s;
      z = r / s;
      q /= p;
      r /= p;

      /* do row modifications */

      for (j = k; j <= n; j++)
        {
          p = FMAT (h, k, j, nc) + q * FMAT (h, k + 1, j, nc);

          if (notlast)
            {
              p += r * FMAT (h, k + 2, j, nc);
              FMAT (h, k + 2, j, nc) -= p * z;
            }

          FMAT (h, k + 1, j, nc) -= p * y;
          FMAT (h, k, j, nc) -= p * x;
        }

      j = (k + 3 < n) ? (k + 3) : n;

      /* do column modifications */

      for (i = e; i <= j; i++)
        {
          p = x * FMAT (h, i, k, nc) + y * FMAT (h, i, k + 1, nc);

          if (notlast)
            {
              p += z * FMAT (h, i, k + 2, nc);
              FMAT (h, i, k + 2, nc) -= p * r;
            }
          FMAT (h, i, k + 1, nc) -= p * q;
          FMAT (h, i, k, nc) -= p;
        }
    }

  goto next_iteration;
}

