/* glppds.h (plain data stream) */

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

#ifndef _GLPPDS_H
#define _GLPPDS_H

#include "glplib.h"

typedef struct PDS PDS;

struct PDS
{     /* plain data stream */
      char *fname;
      /* name of input text file */
      void *fp; /* FILE *fp; */
      /* stream assigned to input text file */
      void *jump; /* jmp_buf jump; */
      /* address for non-local go to on error */
      int count;
      /* line count */
      int c;
      /* current character or EOF */
      char item[255+1];
      /* current item */
};

#define pds_open_file _glp_pds_open_file
PDS *pds_open_file(const char *fname);
/* open plain data stream */

#define pds_set_jump _glp_pds_set_jump
void pds_set_jump(PDS *pds, jmp_buf jump);
/* set address for non-local go to on error */

#define pds_error _glp_pds_error
void pds_error(PDS *pds, const char *fmt, ...);
/* print error message and terminate processing */

#define pds_warning _glp_pds_warning
void pds_warning(PDS *pds, const char *fmt, ...);
/* print warning message and terminate processing */

#define pds_skip_line _glp_pds_skip_line
void pds_skip_line(PDS *pds);
/* skip current line */

#define pds_scan_sym _glp_pds_scan_sym
const char *pds_scan_sym(PDS *pds);
/* scan symbol from plain data stream */

#define pds_scan_int _glp_pds_scan_int
int pds_scan_int(PDS *pds);
/* scan integer from plain data stream */

#define pds_scan_num _glp_pds_scan_num
double pds_scan_num(PDS *pds);
/* scan floating-point number from plain data stream */

#define pds_close_file _glp_pds_close_file
void pds_close_file(PDS *pds);
/* close plain data stream */

#endif

/* eof */
