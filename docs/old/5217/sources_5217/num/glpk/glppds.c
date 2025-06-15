/* glppds.c */

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

#define _GLPSTD_ERRNO
#define _GLPSTD_STDIO
#include "glppds.h"
#define xfault xerror

PDS *pds_open_file(const char *fname)
{     /* open plain data stream */
      PDS *pds = NULL;
      FILE *fp;
      fp = fopen(fname, "r");
      if (fp == NULL) goto done;
      pds = xmalloc(sizeof(PDS));
      pds->fname = xmalloc(strlen(fname)+1);
      strcpy(pds->fname, fname);
      pds->fp = fp;
      pds->jump = NULL;
      pds->count = 0;
      pds->c = '\n';
      pds->item[0] = '\0';
done: return pds;
}

void pds_set_jump(PDS *pds, jmp_buf jump)
{     /* set address for non-local go to on error */
      pds->jump = jump;
      return;
}

void pds_error(PDS *pds, const char *fmt, ...)
{     /* print error message and terminate processing */
      va_list arg;
      char msg[1023+1];
      va_start(arg, fmt);
      vsprintf(msg, fmt, arg);
      xassert(strlen(msg) < sizeof(msg));
      va_end(arg);
      if (pds->jump == NULL)
         xfault("%s:%d: %s", pds->fname, pds->count, msg);
      else
      {  xprintf("%s:%d: %s", pds->fname, pds->count, msg);
         longjmp(pds->jump, 1);
      }
      /* no return */
}

void pds_warning(PDS *pds, const char *fmt, ...)
{     /* print warning message and continue processing */
      va_list arg;
      char msg[1023+1];
      va_start(arg, fmt);
      vsprintf(msg, fmt, arg);
      xassert(strlen(msg) < sizeof(msg));
      va_end(arg);
      xprintf("%s:%d: warning: %s", pds->fname, pds->count, msg);
      return;
}

static void scan_char(PDS *pds)
{     /* scan character from plain data stream */
      FILE *fp = pds->fp;
      int c;
      if (pds->c == EOF) goto done;
      if (pds->c == '\n') pds->count++;
      c = fgetc(fp);
      if (ferror(fp))
         pds_error(pds, "read error - %s\n", strerror(errno));
      if (feof(fp))
      {  if (pds->c == '\n')
         {  pds->count--;
            c = EOF;
         }
         else
         {  pds_warning(pds, "missing final end-of-line\n");
            c = '\n';
         }
      }
      else if (c == '\n')
         ;
      else if (isspace(c))
         c = ' ';
      else if (iscntrl(c))
         pds_error(pds, "invalid control character 0x%02X\n", c);
      pds->c = c;
done: return;
}

void pds_skip_line(PDS *pds)
{     /* skip current line */
      while (!(pds->c == '\n' || pds->c == EOF))
         scan_char(pds);
      if (pds->c == '\n') scan_char(pds);
      return;
}

static int check_comment(PDS *pds)
{     /* check if there begins a comment sequence */
      int ret = 0;
      if (pds->c == '/')
      {  scan_char(pds);
         if (pds->c == '*') ret = 1;
         if (pds->c != EOF)
            xassert(ungetc(pds->c, pds->fp) == pds->c);
         pds->c = '/';
      }
      return ret;
}

static void skip_pad(PDS *pds)
{     /* skip non-significant characters and comments */
      for (;;)
      {  while (pds->c == ' ' || pds->c == '\n')
            scan_char(pds);
         if (!check_comment(pds)) break;
         xassert(pds->c == '/');
         scan_char(pds);
         xassert(pds->c == '*');
         scan_char(pds);
         for (;;)
         {  if (pds->c == EOF)
            {  pds_warning(pds, "incomplete comment sequence\n");
               break;
            }
            if (pds->c == '*')
            {  scan_char(pds);
               if (pds->c == '/')
               {  scan_char(pds);
                  break;
               }
            }
            scan_char(pds);
         }
      }
      return;
}

static const char *scan_item(PDS *pds)
{     /* scan data item from plain data stream */
      int len;
      skip_pad(pds);
      len = 0;
      while (!(pds->c == ' ' || pds->c == '\n' || pds->c == EOF))
      {  if (pds->c == '/' && check_comment(pds)) break;
         if (len == 255)
            pds_error(pds, "data item `%.31s...' too long\n",
               pds->item);
         pds->item[len++] = (char)pds->c;
         scan_char(pds);
      }
      pds->item[len] = '\0';
      return pds->item;
}

const char *pds_scan_sym(PDS *pds)
{     /* scan symbol from plain data stream */
      scan_item(pds);
      if (pds->item[0] == '\0')
         pds_error(pds, "unexpected end-of-file\n");
      return pds->item;
}

int pds_scan_int(PDS *pds)
{     /* scan integer from plain data stream */
      int x;
      scan_item(pds);
      if (pds->item[0] == '\0')
         pds_error(pds, "unexpected end-of-file\n");
      switch (str2int(pds->item, &x))
      {  case 0:
            break;
         case 1:
            pds_error(pds, "integer `%s' out of range\n",
               pds->item);
         case 2:
            pds_error(pds, "cannot convert `%s' to integer\n",
               pds->item);
         default:
            xassert(pds != pds);
      }
      return x;
}

double pds_scan_num(PDS *pds)
{     /* scan floating-point number from plain data stream */
      double x;
      scan_item(pds);
      if (pds->item[0] == '\0')
         pds_error(pds, "unexpected end-of-file\n");
      switch (str2num(pds->item, &x))
      {  case 0:
            break;
         case 1:
            pds_error(pds, "number `%s' out of range\n",
               pds->item);
         case 2:
            pds_error(pds, "cannot convert `%s' to number\n",
               pds->item);
         default:
            xassert(pds != pds);
      }
      return x;
}

void pds_close_file(PDS *pds)
{     /* close plain data stream */
      fclose(pds->fp);
      xfree(pds->fname);
      xfree(pds);
      return;
}

/* eof */
