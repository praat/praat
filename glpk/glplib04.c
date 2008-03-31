/* glplib04.c (terminal input/output) */

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

int xgetc(void)
{     /* read character from the terminal */
      int c;
      c = fgetc(stdin);
      if (c == EOF)
         c = XEOF;
      else
         xassert(0x00 <= c && c <= 0xFF);
      return c;
}

/***********************************************************************
*  NAME
*
*  xputc - write character to the terminal
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void xputc(int c);
*
*  DESCRIPTION
*
*  The routine xputc writes the character c to the terminal. */

void xputc(int c)
{     LIBENV *env = lib_link_env();
      /* if terminal output is disabled, do nothing */
      if (!env->term_out) goto skip;
      /* pass the character to the user-defined routine */
      if (env->term_hook != NULL)
      {  char s[1+1];
         s[0] = (char)c, s[1] = '\0';
         if (env->term_hook(env->term_info, s) != 0) goto skip;
      }
      /* write the character to the terminal */
      fputc(c, stdout);
      /* write the character to the log file */
      if (env->log_file != NULL) fputc(c, env->log_file);
skip: return;
}

/***********************************************************************
*  NAME
*
*  xprintf - write formatted output to the terminal
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void xprintf(const char *fmt, ...);
*
*  DESCRIPTION
*
*  The routine xprintf formats its parameters under the format control
*  string fmt and writes the formatted output to the terminal. */

void xprintf(const char *fmt, ...)
{     va_list arg;
      va_start(arg, fmt);
      xvprintf(fmt, arg);
      va_end(arg);
      return;
}

/***********************************************************************
*  NAME
*
*  xvprintf - write formatted output to the terminal
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void xvprintf(const char *fmt, va_list arg);
*
*  The routine xprintf formats its parameters under the format control
*  string fmt and writes the formatted output to the terminal. */

static int func(void *info, int c)
{     xassert(info == NULL);
      xassert(0x00 <= c && c <= 0xFF);
      xputc(c);
      return c;
}

void xvprintf(const char *fmt, va_list arg)
{     lib_doprnt(func, NULL, fmt, arg);
      return;
}

/***********************************************************************
*  NAME
*
*  lib_term_hook - install hook to intercept terminal output
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  void lib_term_hook(int (*func)(void *info, const char *s),
*     void *info);
*
*  DESCRIPTION
*
*  The routine lib_term_hook installs the user-defined hook routine to
*  intercept all terminal output performed by glpk routines.
*
*  This feature can be used to redirect the terminal output to other
*  destination, for example to a file or a text window.
*
*  The parameter func specifies the user-defined hook routine. It is
*  called from an internal printing routine, which passes to it two
*  parameters: info and s. The parameter info is a transit pointer,
*  specified in the corresponding call to the routine lib_term_hook;
*  it may be used to pass some information to the hook routine. The
*  parameter s is a pointer to the null terminated character string,
*  which is intended to be written to the terminal. If the hook routine
*  returns zero, the printing routine writes the string s to the
*  terminal as usual; otherwise, if the hook routine returns non-zero,
*  no terminal output is performed.
*
*  To uninstall the hook routine the parameters func and info should be
*  specified as NULL. */

void lib_term_hook(int (*func)(void *info, const char *s), void *info)
{     LIBENV *env = lib_link_env();
      if (func == NULL)
      {  env->term_hook = NULL;
         env->term_info = NULL;
      }
      else
      {  env->term_hook = func;
         env->term_info = info;
      }
      return;
}

void lib_print_hook(int (*func)(void *info, char *buf), void *info)
{     /* (obsolete) */
#if 0 /* iso c complains */
      int (*hook)(void *, const char *) = (void *)func;
#else
      int (*hook)(void *, const char *) = (int(*)(void *, const char *))
         (func);
#endif
      lib_term_hook(hook, info);
      return;
}

/**********************************************************************/

int lib_open_log(const char *fname)
{     /* open hardcopy file */
      LIBENV *env = lib_link_env();
      if (env->log_file != NULL)
      {  /* hardcopy file is already open */
         return 1;
      }
      env->log_file = fopen(fname, "w");
      if (env->log_file == NULL)
      {  /* cannot create hardcopy file */
         return 2;
      }
      setvbuf(env->log_file, NULL, _IOLBF, BUFSIZ);
      return 0;
}

int lib_close_log(void)
{     /* close hardcopy file */
      LIBENV *env = lib_link_env();
      if (env->log_file == NULL)
      {  /* hardcopy file is already closed */
         return 1;
      }
      fclose(env->log_file);
      env->log_file = NULL;
      return 0;
}

/* eof */
