/* err/error.c
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

gsl_error_handler_t * gsl_error_handler = NULL;

static void no_error_handler (const char *reason, const char *file, int line, int gsl_errno);

void
gsl_error (const char * reason, const char * file, int line, int gsl_errno)
{
  if (gsl_error_handler) 
    {
      (*gsl_error_handler) (reason, file, line, gsl_errno);
      return ;
    }

  gsl_stream_printf ("ERROR", file, line, reason);

  fflush (stdout);
  fprintf (stderr, "Default GSL error handler invoked.\n");
  fflush (stderr);

  abort ();
}

gsl_error_handler_t *
gsl_set_error_handler (gsl_error_handler_t * new_handler)
{
  gsl_error_handler_t * previous_handler = gsl_error_handler;
  gsl_error_handler = new_handler;
  return previous_handler;
}


gsl_error_handler_t *
gsl_set_error_handler_off (void)
{
  gsl_error_handler_t * previous_handler = gsl_error_handler;
  gsl_error_handler = no_error_handler;
  return previous_handler;
}

static void
no_error_handler (const char *reason, const char *file, int line, int gsl_errno)
{
  /* do nothing */
  reason = 0;
  file = 0;
  line = 0;
  gsl_errno = 0;
  return;
}


