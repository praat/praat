/* matrix/matrix_source.c
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

#ifndef HIDE_INLINE_STATIC
BASE
FUNCTION (gsl_matrix, get) (const TYPE (gsl_matrix) * m,
                            const size_t i, const size_t j)
{
  BASE zero = ZERO;

  if (gsl_check_range)
    {
      if (i >= m->size1)        /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_VAL ("first index out of range", GSL_EINVAL, zero);
        }
      else if (j >= m->size2)   /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_VAL ("second index out of range", GSL_EINVAL, zero);
        }
    }
  return *(BASE *) (m->data + MULTIPLICITY * (i * m->tda + j));
}

void
FUNCTION (gsl_matrix, set) (TYPE (gsl_matrix) * m,
                            const size_t i, const size_t j,
                            const BASE x)
{
  if (gsl_check_range)
    {
      if (i >= m->size1)        /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_VOID ("first index out of range", GSL_EINVAL);
        }
      else if (j >= m->size2)   /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_VOID ("second index out of range", GSL_EINVAL);
        }
    }
  *(BASE *) (m->data + MULTIPLICITY * (i * m->tda + j)) = x;
}


BASE *
FUNCTION (gsl_matrix, ptr) (TYPE (gsl_matrix) * m,
                            const size_t i, const size_t j)
{
  if (gsl_check_range)
    {
      if (i >= m->size1)        /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_NULL ("first index out of range", GSL_EINVAL);
        }
      else if (j >= m->size2)   /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_NULL ("second index out of range", GSL_EINVAL);
        }
    }
  return (BASE *) (m->data + MULTIPLICITY * (i * m->tda + j));
}


const BASE *
FUNCTION (gsl_matrix, const_ptr) (const TYPE (gsl_matrix) * m,
                                  const size_t i, const size_t j)
{
  if (gsl_check_range)
    {
      if (i >= m->size1)        /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_NULL ("first index out of range", GSL_EINVAL);
        }
      else if (j >= m->size2)   /* size_t is unsigned, can't be negative */
        {
          GSL_ERROR_NULL ("second index out of range", GSL_EINVAL);
        }
    }
  return (const BASE *) (m->data + MULTIPLICITY * (i * m->tda + j));
}
#endif
