/* fft/real_init.c
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

TYPE(gsl_fft_real_wavetable) *
FUNCTION(gsl_fft_real_wavetable,alloc) (size_t n)
{
  int status;
  size_t i;
  size_t n_factors;
  size_t t, product, product_1, q;
  double d_theta;

  TYPE(gsl_fft_real_wavetable) * wavetable;

  if (n == 0)
    {
      GSL_ERROR_VAL ("length n must be positive integer", GSL_EDOM, 0);
    }

  wavetable = (TYPE(gsl_fft_real_wavetable) *) 
    malloc(sizeof(TYPE(gsl_fft_real_wavetable)));

  if (wavetable == NULL)
    {
      GSL_ERROR_VAL ("failed to allocate struct", GSL_ENOMEM, 0);
    }

  if (n == 1) 
    {
      wavetable->trig = 0;
    }
  else
    {
      wavetable->trig = (TYPE(gsl_complex) *) 
        malloc ((n / 2) * sizeof (TYPE(gsl_complex)));
      
      if (wavetable->trig == NULL)
        {
          /* error in constructor, prevent memory leak */
          
          free(wavetable) ; 

          GSL_ERROR_VAL ("failed to allocate trigonometric lookup table", 
                            GSL_ENOMEM, 0);
        }
    }

  wavetable->n = n;

  status = fft_real_factorize (n, &n_factors, wavetable->factor);

  if (status)
    {
      /* error in constructor, prevent memory leak */
      
      free(wavetable->trig);
      free(wavetable) ; 

      GSL_ERROR_VAL ("factorization failed", GSL_EFACTOR, 0);
    }

  wavetable->nf = n_factors;

  d_theta = 2.0 * M_PI / ((double) n);

  t = 0;
  product = 1;
  for (i = 0; i < wavetable->nf; i++)
    {
      size_t j;
      const size_t factor = wavetable->factor[i];
      wavetable->twiddle[i] = wavetable->trig + t;
      product_1 = product;      /* product_1 = p_(i-1) */
      product *= factor;
      q = n / product;

      for (j = 1; j < factor; j++)
        {
          size_t k;
          size_t m = 0;
          for (k = 1; k < (product_1 + 1) / 2; k++)
            {
              double theta;
              m = m + j * q;
              m = m % n;
              theta = d_theta * m;      /*  d_theta*j*k*q */
              GSL_REAL(wavetable->trig[t]) = cos (theta);
              GSL_IMAG(wavetable->trig[t]) = sin (theta);

              t++;
            }
        }
    }

  if (t > (n / 2))
    {
      /* error in constructor, prevent memory leak */

      free(wavetable->trig);
      free(wavetable) ; 

      GSL_ERROR_VAL ("overflowed trigonometric lookup table", 
                        GSL_ESANITY, 0);
    }

  return wavetable;
}

TYPE(gsl_fft_real_workspace) *
FUNCTION(gsl_fft_real_workspace,alloc) (size_t n)
{
  TYPE(gsl_fft_real_workspace) * workspace;

  if (n == 0)
    {
      GSL_ERROR_VAL ("length n must be positive integer", GSL_EDOM, 0);
    }

  workspace = (TYPE(gsl_fft_real_workspace) *) 
    malloc(sizeof(TYPE(gsl_fft_real_workspace)));

  if (workspace == NULL)
    {
      GSL_ERROR_VAL ("failed to allocate struct", GSL_ENOMEM, 0);
    }

  workspace->n = n;

  workspace->scratch = (BASE *) malloc (n * sizeof (BASE));

  if (workspace->scratch == NULL)
    {
      /* error in constructor, prevent memory leak */
      
      free(workspace) ; 

      GSL_ERROR_VAL ("failed to allocate scratch space", GSL_ENOMEM, 0);
    }

  return workspace;
}



void
FUNCTION(gsl_fft_real_wavetable,free) (TYPE(gsl_fft_real_wavetable) * wavetable)
{

  /* release trigonometric lookup tables */

  free (wavetable->trig);
  wavetable->trig = NULL;

  free (wavetable) ;
}

void
FUNCTION(gsl_fft_real_workspace,free) (TYPE(gsl_fft_real_workspace) * workspace)
{

  /* release scratch space */

  free (workspace->scratch);
  workspace->scratch = NULL;

  free (workspace) ;
}
