/* matrix/oper_complex_source.c
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

int
FUNCTION (gsl_matrix, add) (TYPE (gsl_matrix) * a,
                            const TYPE (gsl_matrix) * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      GSL_ERROR ("matrices must have same dimensions", GSL_EBADLEN);
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
        {
          for (j = 0; j < N; j++)
            {
              const size_t aij = 2 * (i * tda_a + j);
              const size_t bij = 2 * (i * tda_b + j);

              a->data[aij] += b->data[bij];
              a->data[aij + 1] += b->data[bij + 1];
            }
        }

      return GSL_SUCCESS;
    }
}

int
FUNCTION (gsl_matrix, sub) (TYPE (gsl_matrix) * a,
                            const TYPE (gsl_matrix) * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      GSL_ERROR ("matrices must have same dimensions", GSL_EBADLEN);
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
        {
          for (j = 0; j < N; j++)
            {
              const size_t aij = 2 * (i * tda_a + j);
              const size_t bij = 2 * (i * tda_b + j);

              a->data[aij] -= b->data[bij];
              a->data[aij + 1] -= b->data[bij + 1];
            }
        }

      return GSL_SUCCESS;
    }
}

int
FUNCTION (gsl_matrix, mul_elements) (TYPE (gsl_matrix) * a,
                                     const TYPE (gsl_matrix) * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      GSL_ERROR ("matrices must have same dimensions", GSL_EBADLEN);
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
        {
          for (j = 0; j < N; j++)
            {
              const size_t aij = 2 * (i * tda_a + j);
              const size_t bij = 2 * (i * tda_b + j);

              ATOMIC ar = a->data[aij];
              ATOMIC ai = a->data[aij + 1];

              ATOMIC br = b->data[bij];
              ATOMIC bi = b->data[bij + 1];

              a->data[aij] = ar * br - ai * bi;
              a->data[aij + 1] = ar * bi + ai * br;
            }
        }

      return GSL_SUCCESS;
    }
}

int
FUNCTION (gsl_matrix, div_elements) (TYPE (gsl_matrix) * a,
                                     const TYPE (gsl_matrix) * b)
{
  const size_t M = a->size1;
  const size_t N = a->size2;

  if (b->size1 != M || b->size2 != N)
    {
      GSL_ERROR ("matrices must have same dimensions", GSL_EBADLEN);
    }
  else
    {
      const size_t tda_a = a->tda;
      const size_t tda_b = b->tda;

      size_t i, j;

      for (i = 0; i < M; i++)
        {
          for (j = 0; j < N; j++)
            {
              const size_t aij = 2 * (i * tda_a + j);
              const size_t bij = 2 * (i * tda_b + j);

              ATOMIC ar = a->data[aij];
              ATOMIC ai = a->data[aij + 1];

              ATOMIC br = b->data[bij];
              ATOMIC bi = b->data[bij + 1];

              ATOMIC s = 1.0 / hypot(br, bi);

              ATOMIC sbr = s * br;
              ATOMIC sbi = s * bi;
              
              a->data[aij] = (ar * sbr + ai * sbi) * s;
              a->data[aij + 1] = (ai * sbr - ar * sbi) * s;
            }
        }

      return GSL_SUCCESS;
    }
}

int FUNCTION (gsl_matrix, scale) (TYPE (gsl_matrix) * a, const BASE x)
{
  const size_t M = a->size1;
  const size_t N = a->size2;
  const size_t tda = a->tda;

  size_t i, j;

  ATOMIC xr = GSL_REAL(x);
  ATOMIC xi = GSL_IMAG(x);

  for (i = 0; i < M; i++)
    {
      for (j = 0; j < N; j++)
        {
          const size_t aij = 2 * (i * tda + j);

          ATOMIC ar = a->data[aij];
          ATOMIC ai = a->data[aij + 1];
          
          a->data[aij] = ar * xr - ai * xi;
          a->data[aij + 1] = ar * xi + ai * xr;
        }
    }

  return GSL_SUCCESS;
}

int FUNCTION (gsl_matrix, add_constant) (TYPE (gsl_matrix) * a, const BASE x)
{
  const size_t M = a->size1;
  const size_t N = a->size2;
  const size_t tda = a->tda;

  size_t i, j;

  for (i = 0; i < M; i++)
    {
      for (j = 0; j < N; j++)
        {
          a->data[2 * (i * tda + j)] += GSL_REAL (x);
          a->data[2 * (i * tda + j) + 1] += GSL_IMAG (x);
        }
    }

  return GSL_SUCCESS;
}


int FUNCTION (gsl_matrix, add_diagonal) (TYPE (gsl_matrix) * a, const BASE x)
{
  const size_t M = a->size1;
  const size_t N = a->size2;
  const size_t tda = a->tda;
  const size_t loop_lim = (M < N ? M : N);
  size_t i;
  for (i = 0; i < loop_lim; i++)
    {
      a->data[2 * (i * tda + i)] += GSL_REAL (x);
      a->data[2 * (i * tda + i) + 1] += GSL_IMAG (x);
    }

  return GSL_SUCCESS;
}
