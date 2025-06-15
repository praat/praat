/* block/fprintf_source.c
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

#if !(USES_LONGDOUBLE && !HAVE_PRINTF_LONGDOUBLE)

int
FUNCTION (gsl_block, fprintf) (FILE * stream, const TYPE(gsl_block) * b, const char *format)
{
  size_t n = b->size ;
  
  ATOMIC * data = b->data ;
  
  size_t i;

  for (i = 0; i < n; i++)
    {
      int k;
      int status;

      for (k = 0; k < MULTIPLICITY; k++)
        {
          if (k > 0)
            {
              status = putc (' ', stream);

              if (status == EOF)
                {
                  GSL_ERROR ("putc failed", GSL_EFAILED);
                }
            }
          status = fprintf (stream,
                            format,
                            data[MULTIPLICITY * i + k]);
          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }
        }

      status = putc ('\n', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }
    }

  return 0;
}

int
FUNCTION (gsl_block, fscanf) (FILE * stream, TYPE(gsl_block) * b)
{
  size_t n = b->size ;
  
  ATOMIC * data = b->data ;

  size_t i;

  for (i = 0; i < n; i++)
    {
      int k;
      for (k = 0; k < MULTIPLICITY; k++)
        {
          ATOMIC_IO tmp ;

          int status = fscanf (stream, IN_FORMAT, &tmp) ;
          
          data [MULTIPLICITY * i + k] = tmp;


          if (status != 1)
            {
              GSL_ERROR ("fscanf failed", GSL_EFAILED);
            }
        }
    }

  return GSL_SUCCESS;
}


int
FUNCTION (gsl_block, raw_fprintf) (FILE * stream, 
                                   const ATOMIC * data,
                                   const size_t n,
                                   const size_t stride, 
                                   const char *format)
{
  size_t i;

  for (i = 0; i < n; i++)
    {
      int k;
      int status;

      for (k = 0; k < MULTIPLICITY; k++)
        {
          if (k > 0)
            {
              status = putc (' ', stream);

              if (status == EOF)
                {
                  GSL_ERROR ("putc failed", GSL_EFAILED);
                }
            }
          status = fprintf (stream,
                            format,
                            data[MULTIPLICITY * i * stride + k]);
          if (status < 0)
            {
              GSL_ERROR ("fprintf failed", GSL_EFAILED);
            }
        }

      status = putc ('\n', stream);

      if (status == EOF)
        {
          GSL_ERROR ("putc failed", GSL_EFAILED);
        }
    }

  return 0;
}

int
FUNCTION (gsl_block, raw_fscanf) (FILE * stream, 
                                  ATOMIC * data,
                                  const size_t n, 
                                  const size_t stride)
{
  size_t i;

  for (i = 0; i < n; i++)
    {
      int k;
      for (k = 0; k < MULTIPLICITY; k++)
        {
          ATOMIC_IO tmp;

          int status = fscanf (stream, IN_FORMAT, &tmp) ;

          data [MULTIPLICITY * i * stride + k] = tmp;

          if (status != 1)
            GSL_ERROR ("fscanf failed", GSL_EFAILED);
        }
    }

  return GSL_SUCCESS;
}

#endif
