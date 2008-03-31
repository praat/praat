/* glplib10.c (standard time) */

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

#define _GLPSTD_TIME
#include "glplib.h"

/* platform-independent ISO C version */

/***********************************************************************
*  NAME
*
*  xtime - determine the current universal time
*
*  SYNOPSIS
*
*  #include "glplib.h"
*  xlong_t xtime(void);
*
*  RETURNS
*
*  The routine xtime returns the current universal time (UTC), in
*  milliseconds, elapsed since 00:00:00 GMT January 1, 1970. */

#if 0
xlong_t xtime(void)
{     static const int epoch = 2440588; /* jday(1, 1, 1970) */
      time_t timer;
      struct tm *tm;
      xlong_t t;
      int j;
      timer = time(NULL);
      tm = gmtime(&timer);
      j = jday(tm->tm_mday, tm->tm_mon + 1, 1900 + tm->tm_year);
      xassert(j >= 0);
      t = xlset(j - epoch);
      t = xlmul(t, xlset(24));
      t = xladd(t, xlset(tm->tm_hour));
      t = xlmul(t, xlset(60));
      t = xladd(t, xlset(tm->tm_min));
      t = xlmul(t, xlset(60));
      t = xladd(t, xlset(tm->tm_sec));
      t = xlmul(t, xlset(1000));
      return t;
}
#else
static xlong_t zeit(void)
{     static const int epoch = 2440588; /* jday(1, 1, 1970) */
      time_t timer;
      struct tm *tm;
      xlong_t t;
      int j;
      timer = time(NULL);
      tm = gmtime(&timer);
      j = jday(tm->tm_mday, tm->tm_mon + 1, 1900 + tm->tm_year);
      xassert(j >= 0);
      t = xlset(j - epoch);
      t = xlmul(t, xlset(24));
      t = xladd(t, xlset(tm->tm_hour));
      t = xlmul(t, xlset(60));
      t = xladd(t, xlset(tm->tm_min));
      t = xlmul(t, xlset(60));
      t = xladd(t, xlset(tm->tm_sec));
      t = xlmul(t, xlset(1000));
      return t;
}

xlong_t xtime(void)
{     LIBENV *env = lib_link_env();
      xlong_t t;
      clock_t c;
      double secs;
      xassert(sizeof(clock_t) <= sizeof(env->c_init));
      t = zeit();
      if (xlcmp(xlsub(t, env->t_init), xlset(600 * 1000)) <= 0)
      {  /* not more than ten minutes since the last call */
         memcpy(&c, env->c_init, sizeof(clock_t));
         secs = (double)(clock() - c) / (double)CLOCKS_PER_SEC;
         if (0.0 <= secs && secs <= 1000.0)
         {  /* looks like correct value */
            t = xladd(env->t_init, xlset(1000.0 * secs + 0.5));
            goto done;
         }
      }
      /* re-initialize */
      if (xlcmp(t, env->t_last) < 0) t = env->t_last;
      env->t_init = t;
      c = clock();
      memcpy(env->c_init, &c, sizeof(clock_t));
done: xassert(xlcmp(env->t_last, t) <= 0);
      env->t_last = t;
      return t;
}
#endif

double xdifftime(xlong_t t1, xlong_t t0)
{     /* compute the difference between two time values, in seconds */
      return xltod(xlsub(t1, t0)) / 1000.0;
}

/* eof */
