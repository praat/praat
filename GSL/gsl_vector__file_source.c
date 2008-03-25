/* vector/file_source.c
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
FUNCTION (gsl_vector, fread) (FILE * stream, TYPE (gsl_vector) * v)
{
  int status = FUNCTION (gsl_block, raw_fread) (stream,
                                                v->data,
                                                v->size,
                                                v->stride);
  return status;
}

int
FUNCTION (gsl_vector, fwrite) (FILE * stream, const TYPE (gsl_vector) * v)
{
  int status = FUNCTION (gsl_block, raw_fwrite) (stream,
                                                 v->data,
                                                 v->size,
                                                 v->stride);
  return status;
}

#if !(USES_LONGDOUBLE && !HAVE_PRINTF_LONGDOUBLE)
int
FUNCTION (gsl_vector, fprintf) (FILE * stream, const TYPE (gsl_vector) * v,
                                const char *format)
{
  int status = FUNCTION (gsl_block, raw_fprintf) (stream,
                                                  v->data,
                                                  v->size,
                                                  v->stride,
                                                  format);
  return status;
}

int
FUNCTION (gsl_vector, fscanf) (FILE * stream, TYPE (gsl_vector) * v)
{
  int status = FUNCTION (gsl_block, raw_fscanf) (stream,
                                                 v->data,
                                                 v->size,
                                                 v->stride);
  return status;
}
#endif

