/* integration/qmomof.c
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
#include <stdlib.h>
#include "gsl_integration.h"
#include "gsl_errno.h"

static void
compute_moments (double par, double * cheb);

static int
dgtsl (size_t n, double *c, double *d, double *e, double *b);

gsl_integration_qawo_table *
gsl_integration_qawo_table_alloc (double omega, double L, 
                                  enum gsl_integration_qawo_enum sine,
                                  size_t n)
{
  gsl_integration_qawo_table *t;
  double * chebmo;

  if (n == 0)
    {
      GSL_ERROR_VAL ("table length n must be positive integer",
                        GSL_EDOM, 0);
    }

  t = (gsl_integration_qawo_table *)
    malloc (sizeof (gsl_integration_qawo_table));

  if (t == 0)
    {
      GSL_ERROR_VAL ("failed to allocate space for qawo_table struct",
                        GSL_ENOMEM, 0);
    }

  chebmo = (double *)  malloc (25 * n * sizeof (double));

  if (chebmo == 0)
    {
      free (t);
      GSL_ERROR_VAL ("failed to allocate space for chebmo block",
                        GSL_ENOMEM, 0);
    }

  t->n = n;
  t->sine = sine;
  t->omega = omega;
  t->L = L;
  t->par = 0.5 * omega * L;
  t->chebmo = chebmo;

  /* precompute the moments */

  { 
    size_t i;
    double scale = 1.0;

    for (i = 0 ; i < t->n; i++)
      {
        compute_moments (t->par * scale, t->chebmo + 25*i);
        scale *= 0.5;
      }
  }

  return t;
}

int
gsl_integration_qawo_table_set (gsl_integration_qawo_table * t,
                                    double omega, double L,
                                    enum gsl_integration_qawo_enum sine)
{
  t->omega = omega;
  t->sine = sine;
  t->L = L;
  t->par = 0.5 * omega * L;

  /* recompute the moments */

  { 
    size_t i;
    double scale = 1.0;

    for (i = 0 ; i < t->n; i++)
      {
        compute_moments (t->par * scale, t->chebmo + 25*i);
        scale *= 0.5;
      }
  }

  return GSL_SUCCESS;
}


int
gsl_integration_qawo_table_set_length (gsl_integration_qawo_table * t,
                                           double L)
{
  /* return immediately if the length is the same as the old length */

  if (L == t->L)
    return GSL_SUCCESS;

  /* otherwise reset the table and compute the new parameters */

  t->L = L;
  t->par = 0.5 * t->omega * L;

  /* recompute the moments */

  { 
    size_t i;
    double scale = 1.0;

    for (i = 0 ; i < t->n; i++)
      {
        compute_moments (t->par * scale, t->chebmo + 25*i);
        scale *= 0.5;
      }
  }

  return GSL_SUCCESS;
}


void
gsl_integration_qawo_table_free (gsl_integration_qawo_table * t)
{
  free (t->chebmo);
  free (t);
}

static void
compute_moments (double par, double *chebmo)
{
  double v[28], d[25], d1[25], d2[25];

  const size_t noeq = 25;
  
  const double par2 = par * par;
  const double par4 = par2 * par2;
  const double par22 = par2 + 2.0;

  const double sinpar = sin (par);
  const double cospar = cos (par);

  size_t i;

  /* compute the chebyschev moments with respect to cosine */

  double ac = 8 * cospar;
  double as = 24 * par * sinpar;

  v[0] = 2 * sinpar / par;
  v[1] = (8 * cospar + (2 * par2 - 8) * sinpar / par) / par2;
  v[2] = (32 * (par2 - 12) * cospar
          + (2 * ((par2 - 80) * par2 + 192) * sinpar) / par) / par4;

  if (fabs (par) <= 24)
    {
      /* compute the moments as the solution of a boundary value
         problem using the asyptotic expansion as an endpoint */
      
      double an2, ass, asap;
      double an = 6;
      size_t k;

      for (k = 0; k < noeq - 1; k++)
        {
          an2 = an * an;
          d[k] = -2 * (an2 - 4) * (par22 - 2 * an2);
          d2[k] = (an - 1) * (an - 2) * par2;
          d1[k + 1] = (an + 3) * (an + 4) * par2;
          v[k + 3] = as - (an2 - 4) * ac;
          an = an + 2.0;
        }

      an2 = an * an;

      d[noeq - 1] = -2 * (an2 - 4) * (par22 - 2 * an2);
      v[noeq + 2] = as - (an2 - 4) * ac;
      v[3] = v[3] - 56 * par2 * v[2];

      ass = par * sinpar;
      asap = (((((210 * par2 - 1) * cospar - (105 * par2 - 63) * ass) / an2
                - (1 - 15 * par2) * cospar + 15 * ass) / an2 
               - cospar + 3 * ass) / an2 
              - cospar) / an2;
      v[noeq + 2] = v[noeq + 2] - 2 * asap * par2 * (an - 1) * (an - 2);

      dgtsl (noeq, d1, d, d2, v + 3);

    }
  else
    {
      /* compute the moments by forward recursion */
      size_t k;
      double an = 4;

      for (k = 3; k < 13; k++)
        {
          double an2 = an * an;
          v[k] = ((an2 - 4) * (2 * (par22 - 2 * an2) * v[k - 1] - ac)
                  + as - par2 * (an + 1) * (an + 2) * v[k - 2]) 
            / (par2 * (an - 1) * (an - 2));
          an = an + 2.0;
        }
    }


  for (i = 0; i < 13; i++)
    {
      chebmo[2 * i] = v[i];
    }

  /* compute the chebyschev moments with respect to sine */

  v[0] = 2 * (sinpar - par * cospar) / par2;
  v[1] = (18 - 48 / par2) * sinpar / par2 + (-2 + 48 / par2) * cospar / par;

  ac = -24 * par * cospar;
  as = -8 * sinpar;

  if (fabs (par) <= 24)
    {
      /* compute the moments as the solution of a boundary value
         problem using the asyptotic expansion as an endpoint */

      size_t k;
      double an2, ass, asap;
      double an = 5;

      for (k = 0; k < noeq - 1; k++)
        {
          an2 = an * an;
          d[k] = -2 * (an2 - 4) * (par22 - 2 * an2);
          d2[k] = (an - 1) * (an - 2) * par2;
          d1[k + 1] = (an + 3) * (an + 4) * par2;
          v[k + 2] = ac + (an2 - 4) * as;
          an = an + 2.0;
        }
      
      an2 = an * an;

      d[noeq - 1] = -2 * (an2 - 4) * (par22 - 2 * an2);
      v[noeq + 1] = ac + (an2 - 4) * as;
      v[2] = v[2] - 42 * par2 * v[1];

      ass = par * cospar;
      asap = (((((105 * par2 - 63) * ass - (210 * par2 - 1) * sinpar) / an2
                + (15 * par2 - 1) * sinpar
                - 15 * ass) / an2 - sinpar - 3 * ass) / an2 - sinpar) / an2;
      v[noeq + 1] = v[noeq + 1] - 2 * asap * par2 * (an - 1) * (an - 2);

      dgtsl (noeq, d1, d, d2, v + 2);

    }
  else
    {
      /* compute the moments by forward recursion */
      size_t k;
      double an = 3;
      for (k = 2; k < 12; k++)
        {
          double an2 = an * an;
          v[k] = ((an2 - 4) * (2 * (par22 - 2 * an2) * v[k - 1] + as)
                  + ac - par2 * (an + 1) * (an + 2) * v[k - 2]) 
            / (par2 * (an - 1) * (an - 2));
          an = an + 2.0;
        }
    }

  for (i = 0; i < 12; i++)
    {
      chebmo[2 * i + 1] = v[i];
    }

}

static int
dgtsl (size_t n, double *c, double *d, double *e, double *b)
{
  /* solves a tridiagonal matrix A x = b 
     
     c[1 .. n - 1]   subdiagonal of the matrix A
     d[0 .. n - 1]   diagonal of the matrix A
     e[0 .. n - 2]   superdiagonal of the matrix A

     b[0 .. n - 1]   right hand side, replaced by the solution vector x */

  size_t k;

  c[0] = d[0];

  if (n == 0)
    {
      return GSL_SUCCESS;
    }

  if (n == 1)
    {
      b[0] = b[0] / d[0] ;
      return GSL_SUCCESS;
    }

  d[0] = e[0];
  e[0] = 0;
  e[n - 1] = 0;

  for (k = 0; k < n - 1; k++)
    {
      size_t k1 = k + 1;

      if (fabs (c[k1]) >= fabs (c[k]))
        {
          {
            double t = c[k1];
            c[k1] = c[k];
            c[k] = t;
          };
          {
            double t = d[k1];
            d[k1] = d[k];
            d[k] = t;
          };
          {
            double t = e[k1];
            e[k1] = e[k];
            e[k] = t;
          };
          {
            double t = b[k1];
            b[k1] = b[k];
            b[k] = t;
          };
        }

      if (c[k] == 0)
        {
          return GSL_FAILURE ;
        }

      {
        double t = -c[k1] / c[k];

        c[k1] = d[k1] + t * d[k];
        d[k1] = e[k1] + t * e[k];
        e[k1] = 0;
        b[k1] = b[k1] + t * b[k];
      }

    }

  if (c[n - 1] == 0)
    {
      return GSL_FAILURE;
    }


  b[n - 1] = b[n - 1] / c[n - 1];

  b[n - 2] = (b[n - 2] - d[n - 2] * b[n - 1]) / c[n - 2];

  for (k = n ; k > 2; k--)
    {
      size_t kb = k - 3;
      b[kb] = (b[kb] - d[kb] * b[kb + 1] - e[kb] * b[kb + 2]) / c[kb];
    }

  return GSL_SUCCESS;
}
