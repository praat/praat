/* eigen/qrstep.c
 * 
 * Copyright (C) 2007 Brian Gough
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

/* remove off-diagonal elements which are neglegible compared with the
   neighboring diagonal elements */

static void
chop_small_elements (const size_t N, const double d[], double sd[])
{
  double d_i = d[0];

  size_t i;

  for (i = 0; i < N - 1; i++)
    {
      double sd_i = sd[i];
      double d_ip1 = d[i + 1];

      if (fabs (sd_i) < GSL_DBL_EPSILON * (fabs (d_i) + fabs (d_ip1)))
        {
          sd[i] = 0.0;
        }
      d_i = d_ip1;
    }
}

/* Generate a Givens rotation (cos,sin) which takes v=(x,y) to (|v|,0) 

   From Golub and Van Loan, "Matrix Computations", Section 5.1.8 */

inline static void
create_givens (const double a, const double b, double *c, double *s)
{
  if (b == 0)
    {
      *c = 1;
      *s = 0;
    }
  else if (fabs (b) > fabs (a))
    {
      double t = -a / b;
      double s1 = 1.0 / sqrt (1 + t * t);
      *s = s1;
      *c = s1 * t;
    }
  else
    {
      double t = -b / a;
      double c1 = 1.0 / sqrt (1 + t * t);
      *c = c1;
      *s = c1 * t;
    }
}

inline static double
trailing_eigenvalue (const size_t n, const double d[], const double sd[])
{
  double ta = d[n - 2];
  double tb = d[n - 1];
  double tab = sd[n - 2];

  double dt = (ta - tb) / 2.0;

  double mu;

  if (dt > 0)
    {
      mu = tb - tab * (tab / (dt + hypot (dt, tab)));
    }
  else if (dt == 0) 
    {
      mu = tb - fabs(tab);
    }
  else
    {
      mu = tb + tab * (tab / ((-dt) + hypot (dt, tab)));
    }

  return mu;
}

static void
qrstep (const size_t n, double d[], double sd[], double gc[], double gs[])
{
  double x, z;
  double ak, bk, zk, ap, bp, aq, bq;
  size_t k;

  double mu = trailing_eigenvalue (n, d, sd);

  x = d[0] - mu;
  z = sd[0];

  ak = 0;
  bk = 0;
  zk = 0;

  ap = d[0];
  bp = sd[0];

  aq = d[1];

  if (n == 2)
    {
      double c, s;
      create_givens (x, z, &c, &s);

      if (gc != NULL)
        gc[0] = c; 
      if (gs != NULL)
        gs[0] = s;

      {
        double ap1 = c * (c * ap - s * bp) + s * (s * aq - c * bp);
        double bp1 = c * (s * ap + c * bp) - s * (s * bp + c * aq);

        double aq1 = s * (s * ap + c * bp) + c * (s * bp + c * aq);

        ak = ap1;
        bk = bp1;

        ap = aq1;
      }

      d[0] = ak;
      sd[0] = bk;
      d[1] = ap;

      return;
    }

  bq = sd[1];

  for (k = 0; k < n - 1; k++)
    {
      double c, s;
      create_givens (x, z, &c, &s);

      /* store Givens rotation */
      if (gc != NULL)
        gc[k] = c; 
      if (gs != NULL)
        gs[k] = s;

      /* compute G' T G */

      {
        double bk1 = c * bk - s * zk;

        double ap1 = c * (c * ap - s * bp) + s * (s * aq - c * bp);
        double bp1 = c * (s * ap + c * bp) - s * (s * bp + c * aq);
        double zp1 = -s * bq;

        double aq1 = s * (s * ap + c * bp) + c * (s * bp + c * aq);
        double bq1 = c * bq;

        ak = ap1;
        bk = bp1;
        zk = zp1;

        ap = aq1;
        bp = bq1;

        if (k < n - 2)
          aq = d[k + 2];
        if (k < n - 3)
          bq = sd[k + 2];

        d[k] = ak;

        if (k > 0)
          sd[k - 1] = bk1;

        if (k < n - 2)
          sd[k + 1] = bp;

        x = bk;
        z = zk;
      }
    }

  /* k = n - 1 */
  d[k] = ap;
  sd[k - 1] = bk;
}
