/* err/stream.c
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

#include "gsl__config.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "gsl_errno.h"
#include "gsl_message.h"

FILE * gsl_stream = NULL ;
gsl_stream_handler_t * gsl_stream_handler = NULL;

void
gsl_stream_printf (const char *label, const char *file, int line, 
                   const char *reason)
{
  if (gsl_stream == NULL)
    {
      gsl_stream = stderr;
    }
  if (gsl_stream_handler)
    {
      (*gsl_stream_handler) (label, file, line, reason);
      return;
    }
  fprintf (gsl_stream, "gsl: %s:%d: %s: %s\n", file, line, label, reason);

}

gsl_stream_handler_t *
gsl_set_stream_handler (gsl_stream_handler_t * new_handler)
{
  gsl_stream_handler_t * previous_handler = gsl_stream_handler;
  gsl_stream_handler = new_handler;
  return previous_handler;
}

FILE *
gsl_set_stream (FILE * new_stream)
{
  FILE * previous_stream;
  if (gsl_stream == NULL) {
    gsl_stream = stderr;
  }
  previous_stream = gsl_stream;
  gsl_stream = new_stream;
  return previous_stream;
}
