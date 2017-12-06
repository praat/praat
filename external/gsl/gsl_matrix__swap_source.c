/* matrix/swap_source.c
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
FUNCTION (gsl_matrix, swap_rows) (TYPE (gsl_matrix) * m,
                                 const size_t i, const size_t j)
{
  const size_t size1 = m->size1;
  const size_t size2 = m->size2;

  if (i >= size1)
    {
      GSL_ERROR ("first row index is out of range", GSL_EINVAL);
    }

  if (j >= size1)
    {
      GSL_ERROR ("second row index is out of range", GSL_EINVAL);
    }

  if (i != j)
    {
      ATOMIC *row1 = m->data + MULTIPLICITY * i * m->tda;
      ATOMIC *row2 = m->data + MULTIPLICITY * j * m->tda;
      
      size_t k;
      
      for (k = 0; k < MULTIPLICITY * size2; k++)
        {
          ATOMIC tmp = row1[k] ;
          row1[k] = row2[k] ;
          row2[k] = tmp ;
        }
    }

  return GSL_SUCCESS;
}

int
FUNCTION (gsl_matrix, swap_columns) (TYPE (gsl_matrix) * m,
                                     const size_t i, const size_t j)
{
  const size_t size1 = m->size1;
  const size_t size2 = m->size2;

  if (i >= size2)
    {
      GSL_ERROR ("first column index is out of range", GSL_EINVAL);
    }

  if (j >= size2)
    {
      GSL_ERROR ("second column index is out of range", GSL_EINVAL);
    }

  if (i != j)
    {
      ATOMIC *col1 = m->data + MULTIPLICITY * i;
      ATOMIC *col2 = m->data + MULTIPLICITY * j;
      
      size_t p;
      
      for (p = 0; p < size1; p++)
        {
          size_t k;
          size_t n = p * MULTIPLICITY * m->tda;
 
          for (k = 0; k < MULTIPLICITY; k++)
            {
              ATOMIC tmp = col1[n+k] ;
              col1[n+k] = col2[n+k] ;
              col2[n+k] = tmp ;
            }
        }
    }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_matrix, swap_rowcol) (TYPE (gsl_matrix) * m,
                                    const size_t i, const size_t j)
{
  const size_t size1 = m->size1;
  const size_t size2 = m->size2;

  if (size1 != size2)
    {
      GSL_ERROR ("matrix must be square to swap row and column", GSL_ENOTSQR);
    }

  if (i >= size1)
    {
      GSL_ERROR ("row index is out of range", GSL_EINVAL);
    }

  if (j >= size2)
    {
      GSL_ERROR ("column index is out of range", GSL_EINVAL);
    }

  {
    ATOMIC *row = m->data + MULTIPLICITY * i * m->tda;
    ATOMIC *col = m->data + MULTIPLICITY * j;
      
    size_t p;
    
    for (p = 0; p < size1; p++)
      {
        size_t k;

        size_t r = p * MULTIPLICITY;
        size_t c = p * MULTIPLICITY * m->tda;
        
          for (k = 0; k < MULTIPLICITY; k++)
            {
              ATOMIC tmp = col[c+k] ;
              col[c+k] = row[r+k] ;
              row[r+k] = tmp ;
            }
        }
    }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_matrix, transpose) (TYPE (gsl_matrix) * m)
{
  const size_t size1 = m->size1;
  const size_t size2 = m->size2;
  size_t i, j, k;

  if (size1 != size2)
    {
      GSL_ERROR ("matrix must be square to take transpose", GSL_ENOTSQR);
    }

  for (i = 0; i < size1; i++)
    {
      for (j = i + 1 ; j < size2 ; j++) 
        {
          for (k = 0; k < MULTIPLICITY; k++)
            {
              size_t e1 = (i *  m->tda + j) * MULTIPLICITY + k ;
              size_t e2 = (j *  m->tda + i) * MULTIPLICITY + k ;
              {
                ATOMIC tmp = m->data[e1] ;
                m->data[e1] = m->data[e2] ;
                m->data[e2] = tmp ;
              }
            }
        }
    }

  return GSL_SUCCESS;
}

int
FUNCTION (gsl_matrix, transpose_memcpy) (TYPE (gsl_matrix) * dest, 
                                         const TYPE (gsl_matrix) * src)
{
  const size_t src_size1 = src->size1;
  const size_t src_size2 = src->size2;

  const size_t dest_size1 = dest->size1;
  const size_t dest_size2 = dest->size2;

  size_t i, j, k;

  if (dest_size2 != src_size1 || dest_size1 != src_size2)
    {
      GSL_ERROR ("dimensions of dest matrix must be transpose of src matrix", 
                 GSL_EBADLEN);
    }

  for (i = 0; i < dest_size1; i++)
    {
      for (j = 0 ; j < dest_size2; j++) 
        {
          for (k = 0; k < MULTIPLICITY; k++)
            {
              size_t e1 = (i *  dest->tda + j) * MULTIPLICITY + k ;
              size_t e2 = (j *  src->tda + i) * MULTIPLICITY + k ;

              dest->data[e1] = src->data[e2] ;
            }
        }
    }

  return GSL_SUCCESS;
}
