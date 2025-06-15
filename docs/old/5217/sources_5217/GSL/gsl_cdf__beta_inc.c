/* specfunc/beta_inc.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  G. Jungman */
/* Modified for cdfs by Brian Gough, June 2003 */

static double
beta_cont_frac (const double a, const double b, const double x,
                const double epsabs)
{
  const unsigned int max_iter = 512;    /* control iterations      */
  const double cutoff = 2.0 * GSL_DBL_MIN;      /* control the zero cutoff */
  unsigned int iter_count = 0;
  double cf;

  /* standard initialization for continued fraction */
  double num_term = 1.0;
  double den_term = 1.0 - (a + b) * x / (a + 1.0);

  if (fabs (den_term) < cutoff)
    den_term = GSL_NAN;

  den_term = 1.0 / den_term;
  cf = den_term;

  while (iter_count < max_iter)
    {
      const int k = iter_count + 1;
      double coeff = k * (b - k) * x / (((a - 1.0) + 2 * k) * (a + 2 * k));
      double delta_frac;

      /* first step */
      den_term = 1.0 + coeff * den_term;
      num_term = 1.0 + coeff / num_term;

      if (fabs (den_term) < cutoff)
        den_term = GSL_NAN;

      if (fabs (num_term) < cutoff)
        num_term = GSL_NAN;

      den_term = 1.0 / den_term;

      delta_frac = den_term * num_term;
      cf *= delta_frac;

      coeff = -(a + k) * (a + b + k) * x / ((a + 2 * k) * (a + 2 * k + 1.0));

      /* second step */
      den_term = 1.0 + coeff * den_term;
      num_term = 1.0 + coeff / num_term;

      if (fabs (den_term) < cutoff)
        den_term = GSL_NAN;

      if (fabs (num_term) < cutoff)
        num_term = GSL_NAN;

      den_term = 1.0 / den_term;

      delta_frac = den_term * num_term;
      cf *= delta_frac;

      if (fabs (delta_frac - 1.0) < 2.0 * GSL_DBL_EPSILON)
        break;

      if (cf * fabs (delta_frac - 1.0) < epsabs)
        break;

      ++iter_count;
    }

  if (iter_count >= max_iter)
    return GSL_NAN;

  return cf;
}

/* The function beta_inc_AXPY(A,Y,a,b,x) computes A * beta_inc(a,b,x)
   + Y taking account of possible cancellations when using the
   hypergeometric transformation beta_inc(a,b,x)=1-beta_inc(b,a,1-x).

   It also adjusts the accuracy of beta_inc() to fit the overall
   absolute error when A*beta_inc is added to Y. (e.g. if Y >>
   A*beta_inc then the accuracy of beta_inc can be reduced) */

static double
beta_inc_AXPY (const double A, const double Y,
               const double a, const double b, const double x)
{
  if (x == 0.0)
    {
      return A * 0 + Y;
    }
  else if (x == 1.0)
    {
      return A * 1 + Y;
    }
  else
    {
      double ln_beta = gsl_sf_lnbeta (a, b);
      double ln_pre = -ln_beta + a * log (x) + b * log1p (-x);

      double prefactor = exp (ln_pre);

      if (x < (a + 1.0) / (a + b + 2.0))
        {
          /* Apply continued fraction directly. */
          double epsabs = fabs (Y / (A * prefactor / a)) * GSL_DBL_EPSILON;

          double cf = beta_cont_frac (a, b, x, epsabs);

          return A * (prefactor * cf / a) + Y;
        }
      else
        {
          /* Apply continued fraction after hypergeometric transformation. */
          double epsabs =
            fabs ((A + Y) / (A * prefactor / b)) * GSL_DBL_EPSILON;
          double cf = beta_cont_frac (b, a, 1.0 - x, epsabs);
          double term = prefactor * cf / b;

          if (A == -Y)
            {
              return -A * term;
            }
          else
            {
              return A * (1 - term) + Y;
            }
        }
    }
}

/* Direct series evaluation for testing purposes only */

#if 0
static double
beta_series (const double a, const double b, const double x,
             const double epsabs)
{
  double f = x / (1 - x);
  double c = (b - 1) / (a + 1) * f;
  double s = 1;
  double n = 0;

  s += c;

  do
    {
      n++;
      c *= -f * (2 + n - b) / (2 + n + a);
      s += c;
    }
  while (n < 512 && fabs (c) > GSL_DBL_EPSILON * fabs (s) + epsabs);

  s /= (1 - x);

  return s;
}
#endif
