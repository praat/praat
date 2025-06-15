/* vector/copy_source.c
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
FUNCTION (gsl_vector, memcpy) (TYPE (gsl_vector) * dest,
                               const TYPE (gsl_vector) * src)
{
  const size_t src_size = src->size;
  const size_t dest_size = dest->size;

  if (src_size != dest_size)
    {
      GSL_ERROR ("vector lengths are not equal", GSL_EBADLEN);
    }

  {
    const size_t src_stride = src->stride ;
    const size_t dest_stride = dest->stride ;
    size_t j;

    for (j = 0; j < src_size; j++)
      {
        size_t k;

        for (k = 0; k < MULTIPLICITY; k++) 
          {
            dest->data[MULTIPLICITY * dest_stride * j + k] 
              = src->data[MULTIPLICITY * src_stride * j + k];
          }
      }
  }

  return GSL_SUCCESS;
}

