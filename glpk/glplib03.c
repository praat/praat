/* glplib03.c (error handling) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Copyright (C) 2000, 01, 02, 03, 04, 05, 06, 07, 08 Andrew Makhorin,
*  Department for Applied Informatics, Moscow Aviation Institute,
*  Moscow, Russia. All rights reserved. E-mail: <mao@mai2.rcnet.ru>.
*
*  GLPK is free software: you can redistribute it and/or modify it
*  under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  GLPK is distributed in the hope that it will be useful, but WITHOUT
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public
*  License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with GLPK. If not, see <http://www.gnu.org/licenses/>.
***********************************************************************/

#define _GLPSTD_STDIO
#include "glplib.h"

/***********************************************************************
*  NAME
*
*  xassert - check for logical condition
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void xassert(int expr);
*
*  DESCRIPTION
*
*  The routine xassert (implemented as a macro) checks for a logical
*  condition specified by the parameter expr. If the condition is false
*  (i.e. the value of expr is zero), the routine writes a message to
*  the terminal and abnormally terminates the program. */

void lib_xassert(const char *expr, const char *file, int line)
{     lib_xerror1(file, line)("Assertion failed: %s\n", expr);
      /* no return */
}

/***********************************************************************
*  NAME
*
*  xerror - display error message and terminate execution
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void xerror(const char *fmt, ...);
*
*  DESCRIPTION
*
*  The routine xerror (implemented as a macro) formats its parameters
*  under the format control string fmt, writes the formatted message to
*  the terminal, and abnormally terminates the program. */

xerror_t lib_xerror1(const char *file, int line)
{     LIBENV *env = lib_link_env();
      env->err_file = file;
      env->err_line = line;
      return lib_xerror2;
}

void lib_xerror2(const char *fmt, ...)
{     LIBENV *env = lib_link_env();
      va_list arg;
      va_start(arg, fmt);
      xvprintf(fmt, arg);
      va_end(arg);
      xprintf("Error detected in file %s at line %d\n",
         env->err_file, env->err_line);
      fflush(stdout);
      fflush(stderr);
      abort();
      /* no return */
}

void lib_fault_hook(int (*func)(void *info, char *buf), void *info)
{     /* (obsolete) */
      xassert(func == func);
      xassert(info == info);
      return;
}

/***********************************************************************
*  NAME
*
*  lib_err_msg - save error message string
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void lib_err_msg(const char *msg);
*
*  DESCRIPTION
*
*  The routine lib_err_msg saves an error message string specified by
*  the parameter msg. The message is obtained by some library routines
*  with a call to strerror(errno). */

void lib_err_msg(const char *msg)
{     LIBENV *env = lib_link_env();
      int len = strlen(msg);
      if (len >= sizeof(env->err_msg))
         len = sizeof(env->err_msg) - 1;
      memcpy(env->err_msg, msg, len);
      if (len > 0 && env->err_msg[len-1] == '\n') len--;
      env->err_msg[len] = '\0';
      return;
}

/***********************************************************************
*  NAME
*
*  xerrmsg - retrieve error message string
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  const char *xerrmsg(void);
*
*  RETURNS
*
*  The routine xerrmsg returns a pointer to an error message string
*  previously set by some library routine to indicate an error. */

const char *xerrmsg(void)
{     LIBENV *env = lib_link_env();
      return env->err_msg;
}

/* eof */
