/* glplib.h (low-level library routines) */

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

#ifndef _GLPLIB_H
#define _GLPLIB_H

#include "glpstd.h"

typedef struct { int lo, hi; } xlong_t;
/* long integer data type */

typedef struct { xlong_t quot, rem; } xldiv_t;
/* result of long integer division */

typedef struct LIBENV LIBENV;
typedef struct LIBMEM LIBMEM;
typedef struct XFILE XFILE;

struct LIBENV
{     /* library environment block */
      char version[7+1];
      /* version string returned by the routine glp_version */
      /*--------------------------------------------------------------*/
      /* memory allocation */
      xlong_t mem_limit;
      /* maximal amount of memory (in bytes) available for dynamic
         allocation */
      LIBMEM *mem_ptr;
      /* pointer to the linked list of allocated memory blocks */
      int mem_count;
      /* total number of currently allocated memory blocks */
      int mem_cpeak;
      /* peak value of mem_count */
      xlong_t mem_total;
      /* total amount of currently allocated memory (in bytes; is the
         sum of the size field over all memory block descriptors) */
      xlong_t mem_tpeak;
      /* peak value of mem_total */
      /*--------------------------------------------------------------*/
      /* terminal output */
      int term_out;
      /* flag to enable/disable terminal output */
      int (*term_hook)(void *info, const char *s);
      /* user-defined routine to intercept terminal output */
      void *term_info;
      /* transit pointer passed to the routine term_hook */
      /*--------------------------------------------------------------*/
      /* input/output streams */
      char err_msg[1000+1];
      XFILE *file_ptr;
      /* pointer to the linked list of active stream descriptors */
      void *log_file; /* FILE *log_file; */
      /* output stream used to hardcopy all terminal output; NULL means
         no hardcopying */
      const char *err_file;
      int err_line;
      /*--------------------------------------------------------------*/
      /* standard time */
      xlong_t t_init, t_last;
      char c_init[sizeof(double)]; /* clock_t c_init; */
      /*--------------------------------------------------------------*/
      /* shared libraries */
      void *h_odbc;  /* handle to ODBC shared library */
      void *h_mysql; /* handle to MySQL shared library */
};

#define LIB_MEM_FLAG 0x20101960
/* memory block descriptor flag */

struct LIBMEM
{     /* memory block descriptor */
      int flag;
      /* descriptor flag */
      int size;
      /* size of block (in bytes, including descriptor) */
      LIBMEM *prev;
      /* pointer to previous memory block descriptor */
      LIBMEM *next;
      /* pointer to next memory block descriptor */
};

struct XFILE
{     /* input/output stream descriptor */
      int type;
      /* stream handle type: */
#define FH_FILE   0x11  /* FILE   */
#define FH_ZLIB   0x22  /* gzFile */
      void *fh;
      /* pointer to stream handle */
      XFILE *prev;
      /* pointer to previous stream descriptor */
      XFILE *next;
      /* pointer to next stream descriptor */
};

#define XEOF (-1)

#define bigmul _glp_lib_bigmul
void bigmul(int n, int m, unsigned short x[], unsigned short y[]);
/* multiply unsigned integer numbers of arbitrary precision */

#define bigdiv _glp_lib_bigdiv
void bigdiv(int n, int m, unsigned short x[], unsigned short y[]);
/* divide unsigned integer numbers of arbitrary precision */

#define xlset _glp_lib_xlset
xlong_t xlset(int x);
/* expand integer to long integer */

#define xlneg _glp_lib_xlneg
xlong_t xlneg(xlong_t x);
/* negate long integer */

#define xladd _glp_lib_xladd
xlong_t xladd(xlong_t x, xlong_t y);
/* add long integers */

#define xlsub _glp_lib_xlsub
xlong_t xlsub(xlong_t x, xlong_t y);
/* subtract long integers */

#define xlcmp _glp_lib_xlcmp
int xlcmp(xlong_t x, xlong_t y);
/* compare long integers */

#define xlmul _glp_lib_xlmul
xlong_t xlmul(xlong_t x, xlong_t y);
/* multiply long integers */

#define xldiv _glp_lib_xldiv
xldiv_t xldiv(xlong_t x, xlong_t y);
/* divide long integers */

#define xltod _glp_lib_xltod
double xltod(xlong_t x);
/* convert long integer to double */

#define xltoa _glp_lib_xltoa
char *xltoa(xlong_t x, char *s);
/* convert long integer to character string */

#define lib_set_ptr _glp_lib_set_ptr
void lib_set_ptr(void *ptr);
/* store global pointer in TLS */

#define lib_get_ptr _glp_lib_get_ptr
void *lib_get_ptr(void);
/* retrieve global pointer from TLS */

#define lib_init_env _glp_lib_init_env
int lib_init_env(void);
/* initialize library environment */

#define lib_link_env _glp_lib_link_env
LIBENV *lib_link_env(void);
/* retrieve pointer to library environment block */

#define lib_version _glp_lib_version
const char *lib_version(void);
/* determine library version */

#define lib_free_env _glp_lib_free_env
int lib_free_env(void);
/* free library environment */

#define xgetc _glp_lib_xgetc
int xgetc(void);
/* read character from the terminal */

#define xputc _glp_lib_xputc
void xputc(int c);
/* write character to the terminal */

#define xprintf _glp_lib_xprintf
void xprintf(const char *fmt, ...);
/* write formatted output to the terminal */

#define xvprintf _glp_lib_xvprintf
void xvprintf(const char *fmt, va_list arg);
/* write formatted output to the terminal */

#define lib_term_hook _glp_lib_term_hook
void lib_term_hook(int (*func)(void *info, const char *s), void *info);
/* install hook to intercept terminal output */

#define lib_print_hook _glp_lib_print_hook
void lib_print_hook(int (*func)(void *info, char *buf), void *info);
/* (obsolete) */

#if 0
#define xerror _glp_lib_xerror
void xerror(const char *fmt, ...);
/* display error message and terminate execution */
#else
#define xerror lib_xerror1(__FILE__, __LINE__)

typedef void (*xerror_t)(const char *fmt, ...);

#define lib_xerror1 _glp_lib_xerror1
xerror_t lib_xerror1(const char *file, int line);

#define lib_xerror2 _glp_lib_xerror2
void lib_xerror2(const char *fmt, ...);
#endif

#define lib_fault_hook _glp_lib_fault_hook
void lib_fault_hook(int (*func)(void *info, char *buf), void *info);
/* (obsolete) */

#define xassert(expr) \
      ((void)((expr) || (lib_xassert(#expr, __FILE__, __LINE__), 1)))

#define lib_xassert _glp_lib_xassert
void lib_xassert(const char *expr, const char *file, int line);
/* check for logical condition */

/* some processors need data to be properly aligned; the macro
   align_boundary defines the boundary that fits for all data types;
   the macro align_datasize enlarges the specified size of a data item
   to provide a proper alignment of immediately following data */

#define align_boundary sizeof(double)

#define align_datasize(size) ((((size) + (align_boundary - 1)) / \
      align_boundary) * align_boundary)

#define xmalloc _glp_lib_xmalloc
void *xmalloc(int size);
/* allocate memory block */

#define xcalloc _glp_lib_xcalloc
void *xcalloc(int n, int size);
/* allocate memory block */

#define xfree _glp_lib_xfree
void xfree(void *ptr);
/* free memory block */

#define lib_mem_limit _glp_lib_mem_limit
void lib_mem_limit(xlong_t limit);
/* set memory allocation limit */

#define lib_mem_usage _glp_lib_mem_usage
void lib_mem_usage(int *count, int *cpeak, xlong_t *total,
      xlong_t *tpeak);
/* get memory usage information */

#define lib_err_msg _glp_lib_err_msg
void lib_err_msg(const char *msg);

#define xerrmsg _glp_lib_xerrmsg
const char *xerrmsg(void);

#define xfopen _glp_lib_xfopen
XFILE *xfopen(const char *fname, const char *mode);

#define xferror _glp_lib_xferror
int xferror(XFILE *file);

#define xfeof _glp_lib_xfeof
int xfeof(XFILE *file);

#define xfgetc _glp_lib_xfgetc
int xfgetc(XFILE *file);

#define xfputc _glp_lib_xfputc
int xfputc(int c, XFILE *file);

#define xfflush _glp_lib_xfflush
int xfflush(XFILE *fp);

#define xfclose _glp_lib_xfclose
int xfclose(XFILE *file);

#define lib_doprnt _glp_lib_doprnt
int lib_doprnt(int (*func)(void *info, int c), void *info, const char
      *fmt, va_list arg);
/* perform formatted output (basic routine) */

#define xfprintf _glp_lib_xfprintf
int xfprintf(XFILE *file, const char *fmt, ...);

#define lib_open_log _glp_lib_open_log
int lib_open_log(const char *fname);
/* open hardcopy file */

#define lib_close_log _glp_lib_close_log
int lib_close_log(void);
/* close hardcopy file */

#if 0
#define xtime1 _glp_lib_xtime1
glp_ulong xtime1(void);
/* determine the current universal time */
#endif

#if 0
#define xdifftime1 _glp_lib_xdifftime1
double xdifftime1(glp_ulong t1, glp_ulong t0);
/* compute the difference between two time values */
#endif

#define xtime _glp_lib_xtime
xlong_t xtime(void);
/* determine the current universal time */

#define xdifftime _glp_lib_xdifftime
double xdifftime(xlong_t t1, xlong_t t0);

#define str2int _glp_lib_str2int
int str2int(const char *str, int *val);
/* convert character string to value of int type */

#define str2num _glp_lib_str2num
int str2num(const char *str, double *val);
/* convert character string to value of double type */

#define strspx _glp_lib_strspx
char *strspx(char *str);
/* remove all spaces from character string */

#define strtrim _glp_lib_strtrim
char *strtrim(char *str);
/* remove trailing spaces from character string */

#define strrev _glp_lib_strrev
char *strrev(char *s);
/* reverse character string */

#define fp2rat _glp_lib_fp2rat
int fp2rat(double x, double eps, double *p, double *q);
/* convert floating-point number to rational number */

#define jday _glp_lib_jday
int jday(int d, int m, int y);
/* convert calendar date to Julian day number */

#define jdate _glp_lib_jdate
void jdate(int j, int *d, int *m, int *y);
/* convert Julian day number to calendar date */

#define xdlopen _glp_xdlopen
void *xdlopen(const char *module);

#define xdlsym _glp_xdlsym
void *xdlsym(void *h, const char *symbol);

#define xdlclose _glp_xdlclose
void xdlclose(void *h);

#endif

/* eof */
