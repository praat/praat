/* matrix/file_source.c
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
FUNCTION (gsl_matrix, fread) (FILE * stream, TYPE (gsl_matrix) * m)
{
  int status = 0;

  const size_t size1 = m->size1; 
  const size_t size2 = m->size2;

  const size_t tda = m->tda;

  if (tda == size2) /* the rows are contiguous */
    {
      status = FUNCTION (gsl_block, raw_fread) (stream, 
                                                m->data, 
                                                size1 * size2, 1);
    }
  else
    {
      size_t i;

      for (i = 0 ; i < size1 ; i++)  /* read each row separately */
        {
          status = FUNCTION (gsl_block, raw_fread) (stream, 
                                                    m->data + i * tda, 
                                                    size2, 1);
          if (status)
            break;
        }
    }

  return status;
}

int
FUNCTION (gsl_matrix, fwrite) (FILE * stream, const TYPE (gsl_matrix) * m)
{
  int status = 0;

  const size_t size1 = m->size1; 
  const size_t size2 = m->size2;

  const size_t tda = m->tda;

  if (tda == size2) /* the rows are contiguous */
    {
      status = FUNCTION (gsl_block, raw_fwrite) (stream, 
                                                 m->data, 
                                                 size1 * size2, 1);
    }
  else
    {
      size_t i;

      for (i = 0 ; i < size1 ; i++)  /* write each row separately */
        {
          status = FUNCTION (gsl_block, raw_fwrite) (stream, 
                                                     m->data + i * tda, 
                                                     size2, 1);
          if (status)
            break;
        }
    }

  return status;

}

#if !(USES_LONGDOUBLE && !HAVE_PRINTF_LONGDOUBLE)
int
FUNCTION (gsl_matrix, fprintf) (FILE * stream, const TYPE (gsl_matrix) * m,
                                const char *format)
{
  int status = 0;

  const size_t size1 = m->size1; 
  const size_t size2 = m->size2;

  const size_t tda = m->tda;

  if (tda == size2) /* the rows are contiguous */
    {
      status = FUNCTION (gsl_block, raw_fprintf) (stream, 
                                                  m->data, 
                                                  size1 * size2, 1,
                                                  format);
    }
  else
    {
      size_t i;

      for (i = 0 ; i < size1 ; i++)  /* print each row separately */
        {
          status = FUNCTION (gsl_block, raw_fprintf) (stream, 
                                                      m->data + i * tda, 
                                                      size2, 1,
                                                      format);
          if (status)
            break;
        }
    }

  return status;
}

int
FUNCTION (gsl_matrix, fscanf) (FILE * stream, TYPE (gsl_matrix) * m)
{
  int status = 0;

  const size_t size1 = m->size1; 
  const size_t size2 = m->size2;

  const size_t tda = m->tda;

  if (tda == size2)  /* the rows are contiguous */
    {
      status = FUNCTION (gsl_block, raw_fscanf) (stream, 
                                                 m->data, 
                                                 size1 * size2, 1);
    }
  else
    {
      size_t i;

      for (i = 0 ; i < size1 ; i++)  /* scan each row separately */
        {
          status = FUNCTION (gsl_block, raw_fscanf) (stream, 
                                                     m->data + i * tda, 
                                                     size2, 1);
          if (status)
            break;
        }
    }

  return status;
}
#endif

