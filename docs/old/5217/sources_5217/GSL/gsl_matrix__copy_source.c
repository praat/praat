/* matrix/copy_source.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 Gerard Jungman, Brian Gough
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
FUNCTION (gsl_matrix, memcpy) (TYPE (gsl_matrix) * dest,
                               const TYPE (gsl_matrix) * src)
{
  const size_t src_size1 = src->size1;
  const size_t src_size2 = src->size2;
  const size_t dest_size1 = dest->size1;
  const size_t dest_size2 = dest->size2;

  if (src_size1 != dest_size1 || src_size2 != dest_size2)
    {
      GSL_ERROR ("matrix sizes are different", GSL_EBADLEN);
    }

  {
    const size_t src_tda = src->tda ;
    const size_t dest_tda = dest->tda ;
    size_t i, j;

    for (i = 0; i < src_size1 ; i++)
      {
        for (j = 0; j < MULTIPLICITY * src_size2; j++)
          {
            dest->data[MULTIPLICITY * dest_tda * i + j] 
              = src->data[MULTIPLICITY * src_tda * i + j];
          }
      }
  }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_matrix, swap) (TYPE (gsl_matrix) * dest, TYPE (gsl_matrix) * src)
{
  const size_t src_size1 = src->size1;
  const size_t src_size2 = src->size2;
  const size_t dest_size1 = dest->size1;
  const size_t dest_size2 = dest->size2;

  if (src_size1 != dest_size1 || src_size2 != dest_size2)
    {
      GSL_ERROR ("matrix sizes are different", GSL_EBADLEN);
    }

  {
    const size_t src_tda = src->tda ;
    const size_t dest_tda = dest->tda ;
    size_t i, j;

    for (i = 0; i < src_size1 ; i++)
      {
        for (j = 0; j < MULTIPLICITY * src_size2; j++)
          {
            ATOMIC tmp = src->data[MULTIPLICITY * src_tda * i + j];
            src->data[MULTIPLICITY * src_tda * i + j] 
              = dest->data[MULTIPLICITY * dest_tda * i + j];
            dest->data[MULTIPLICITY * dest_tda * i + j] = tmp ;
          }
      }
  }

  return GSL_SUCCESS;
}




