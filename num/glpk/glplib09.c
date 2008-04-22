/* glplib09.c (formatted input/output) */

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
*  lib_doprnt - perform formatted output (basic routine)
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  int lib_doprnt(int (*func)(void *info, int c), void *info,
*     const char *fmt, va_list arg);
*
*  DESCRIPTION
*
*  The routine xdoprnt is a basic routine to perform formatted output.
*  It is equivalent to the standard routine vfprintf, except that the
*  output is passed to the formal routine func rather than to an output
*  stream.
*
*  The formal routine func is called every time a next character should
*  be written. If there is an output error, it should return a negative
*  value, in which case the output is terminated.
*
*  RETURNS
*
*  The routine xdoprnt returns the number of characters passed to the
*  formal routine func. However, if there was an output error (detected
*  by the formal routine func), a negative value is returned. */

int lib_doprnt(int (*func)(void *info, int c), void *info,
      const char *fmt, va_list arg)
{     int cnt, j;
      char buf[4000+1];
#ifdef HAVE_VSNPRINTF
      cnt = vsnprintf(buf, sizeof(buf), fmt, arg);
#else
      cnt = vsprintf(buf, fmt, arg);
#endif
      xassert(0 <= cnt && cnt < sizeof(buf));
      xassert((int)strlen(buf) == cnt);
      for (j = 0; j < cnt; j++)
      {  if (func(info, (unsigned char)buf[j]) < 0)
         {  cnt = -1;
            break;
         }
      }
      return cnt;
}

/**********************************************************************/

static int my_put(void *file, int c)
{     return
         xfputc(c, file);
}

int xfprintf(XFILE *file, const char *fmt, ...)
{     va_list arg;
      int ret;
      va_start(arg, fmt);
#if 0
      ret = vfprintf(file->fp, fmt, arg);
#else
      ret = lib_doprnt(my_put, file, fmt, arg);
#endif
      va_end(arg);
      return ret;
}

/* eof */
