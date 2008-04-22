/* glpapi12.c (library environment routines) */

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

#include "glpapi.h"
#define xfault xerror

/***********************************************************************
*  NAME
*
*  glp_version - determine library version
*
*  SYNOPSIS
*
*  const char *glp_version(void);
*
*  RETURNS
*
*  The routine glp_version returns a pointer to a null-terminated
*  character string, which specifies the version of the GLPK library in
*  the form "X.Y", where X is the major version number, and Y is the
*  minor version number, for example, "4.16". */

const char *glp_version(void)
{     return
         lib_version();
}

/***********************************************************************
*  NAME
*
*  glp_term_out - enable/disable terminal output
*
*  SYNOPSIS
*
*  void glp_term_out(int flag);
*
*  DESCRIPTION
*
*  Depending on the parameter flag the routine glp_term_out enables or
*  disables terminal output performed by glpk routines:
*
*  GLP_ON  - enable terminal output;
*  GLP_OFF - disable terminal output. */

void glp_term_out(int flag)
{     LIBENV *env = lib_link_env();
      env->term_out = GLP_ON;
      if (!(flag == GLP_ON || flag == GLP_OFF))
         xfault("glp_term_out: flag = %d; invalid value\n", flag);
      env->term_out = flag;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_term_hook - install hook to intercept terminal output
*
*  SYNOPSIS
*
*  void glp_term_hook(int (*func)(void *info, const char *s),
*     void *info);
*
*  DESCRIPTION
*
*  The routine glp_term_hook installs the user-defined hook routine to
*  intercept all terminal output performed by glpk routines.
*
*  This feature can be used to redirect the terminal output to other
*  destination, for example to a file or a text window.
*
*  The parameter func specifies the user-defined hook routine. It is
*  called from an internal printing routine, which passes to it two
*  parameters: info and s. The parameter info is a transit pointer,
*  specified in the corresponding call to the routine glp_term_hook;
*  it may be used to pass some information to the hook routine. The
*  parameter s is a pointer to the null terminated character string,
*  which is intended to be written to the terminal. If the hook routine
*  returns zero, the printing routine writes the string s to the
*  terminal in a usual way; otherwise, if the hook routine returns
*  non-zero, no terminal output is performed.
*
*  To uninstall the hook routine the parameters func and info should be
*  specified as NULL. */

void glp_term_hook(int (*func)(void *info, const char *s), void *info)
{     lib_term_hook(func, info);
      return;
}

/***********************************************************************
*  NAME
*
*  glp_malloc - allocate memory block
*
*  SYNOPSIS
*
*  void *glp_malloc(int size);
*
*  DESCRIPTION
*
*  The routine glp_malloc allocates a memory block of size bytes long.
*
*  Note that being allocated the memory block contains arbitrary data
*  (not binary zeros).
*
*  RETURNS
*
*  The routine glp_malloc returns a pointer to the allocated block.
*  To free this block the routine glp_free (not free!) must be used. */

void *glp_malloc(int size)
{     void *ptr;
      ptr = xmalloc(size);
      return ptr;
}

/***********************************************************************
*  NAME
*
*  glp_calloc - allocate memory block
*
*  SYNOPSIS
*
*  void *glp_calloc(int n, int size);
*
*  DESCRIPTION
*
*  The routine glp_calloc allocates a memory block of (n*size) bytes
*  long.
*
*  Note that being allocated the memory block contains arbitrary data
*  (not binary zeros).
*
*  RETURNS
*
*  The routine glp_calloc returns a pointer to the allocated block.
*  To free this block the routine glp_free (not free!) must be used. */

void *glp_calloc(int n, int size)
{     void *ptr;
      ptr = xcalloc(n, size);
      return ptr;
}

/***********************************************************************
*  NAME
*
*  glp_free - free memory block
*
*  SYNOPSIS
*
*  void glp_free(void *ptr);
*
*  DESCRIPTION
*
*  The routine glp_free frees a memory block pointed to by ptr, which
*  was previuosly allocated by the routine glp_malloc or glp_calloc. */

void glp_free(void *ptr)
{     xfree(ptr);
      return;
}

/***********************************************************************
*  NAME
*
*  glp_mem_usage - get memory usage information
*
*  SYNOPSIS
*
*  void glp_mem_usage(int *count, int *cpeak, glp_long *total,
*     glp_long *tpeak);
*
*  DESCRIPTION
*
*  The routine glp_mem_usage reports some information about utilization
*  of the memory by GLPK routines. Information is stored to locations
*  specified by corresponding parameters (see below). Any parameter can
*  be specified as NULL, in which case corresponding information is not
*  stored.
*
*  *count is the number of the memory blocks currently allocated by the
*  routines xmalloc and xcalloc (one call to xmalloc or xcalloc results
*  in allocating one memory block).
*
*  *cpeak is the peak value of *count reached since the initialization
*  of the GLPK library environment.
*
*  *total is the total amount, in bytes, of the memory blocks currently
*  allocated by the routines xmalloc and xcalloc.
*
*  *tpeak is the peak value of *total reached since the initialization
*  of the GLPK library envirionment. */

void glp_mem_usage(int *count, int *cpeak, glp_long *total,
      glp_long *tpeak)
{     xlong_t total1, tpeak1;
      lib_mem_usage(count, cpeak, &total1, &tpeak1);
      if (total != NULL) total->lo = total1.lo, total->hi = total1.hi;
      if (tpeak != NULL) tpeak->lo = tpeak1.lo, tpeak->hi = tpeak1.hi;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_mem_limit - set memory usage limit
*
*  SYNOPSIS
*
*  void glp_mem_limit(int limit);
*
*  DESCRIPTION
*
*  The routine glp_mem_limit limits the amount of memory available for
*  dynamic allocation (in GLPK routines) to limit megabytes. */

void glp_mem_limit(int limit)
{     if (limit < 0)
         xfault("glp_mem_limit: limit = %d; invalid parameter\n",
            limit);
      lib_mem_limit(xlmul(xlset(limit), xlset(1 << 20)));
      return;
}

#if 0
/***********************************************************************
*  NAME
*
*  glp_fopen - open file
*
*  SYNOPSIS
*
*  FILE *glp_fopen(const char *fname, const char *mode);
*
*  DESCRIPTION
*
*  The routine glp_fopen opens a file using the character string fname
*  as the file name and the character string mode as the open mode.
*
*  RETURNS
*
*  If the file is successfully open, the routine glp_fopen returns a
*  pointer to an i/o stream associated with the file (i.e. a pointer to
*  an object of the FILE type). Otherwise the routine return NULL. */

FILE *glp_fopen(const char *fname, const char *mode)
{     FILE *fp;
      fp = xfopen(fname, mode);
      return fp;
}

/***********************************************************************
*  NAME
*
*  glp_fclose - close file
*
*  SYNOPSIS
*
*  void glp_fclose(FILE *fp);
*
*  DESCRIPTION
*
*  The routine glp_fclose closes a file associated with i/o stream,
*  which the parameter fp points to. It is assumed that the file was
*  open by the routine glp_fopen. */

void glp_fclose(FILE *fp)
{     xfclose(fp);
      return;
}
#endif

/***********************************************************************
*  NAME
*
*  glp_free_env - free GLPK library environment
*
*  SYNOPSIS
*
*  void glp_free_env(void);
*
*  DESCRIPTION
*
*  The routine glp_free_env frees all resources used by GLPK routines
*  (memory blocks, etc.) which are currently still in use.
*
*  USAGE NOTES
*
*  Normally the application program does not need to call this routine,
*  because GLPK routines always free all unused resources. However, if
*  the application program even has deleted all problem objects, there
*  will be several memory blocks still allocated for the library needs.
*  For some reasons the application program may want GLPK to free this
*  memory, in which case it should call glp_free_env.
*
*  Note that a call to glp_free_env invalidates all problem objects as
*  if no GLPK routine were called. */

void glp_free_env(void)
{     lib_free_env();
      return;
}

/* eof */
