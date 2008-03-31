/* glpapi09.c (utility routines) */

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
#include "glpapi.h"
#include "glppds.h"

/***********************************************************************
*  NAME
*
*  glp_read_sol - read basic solution from text file
*
*  SYNOPSIS
*
*  int glp_read_sol(glp_prob *lp, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_read_sol reads basic solution from a text file whose
*  name is specified by the parameter fname into the problem object.
*
*  For the file format see description of the routine glp_write_sol.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero. */

int glp_read_sol(glp_prob *lp, const char *fname)
{     PDS *pds;
      jmp_buf jump;
      int i, j, k, ret = 0;
      xprintf("glp_read_sol: reading basic solution from `%s'...\n",
         fname);
      pds = pds_open_file(fname);
      if (pds == NULL)
      {  xprintf("glp_read_sol: unable to open `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      if (setjmp(jump))
      {  ret = 1;
         goto done;
      }
      pds_set_jump(pds, jump);
      /* number of rows, number of columns */
      k = pds_scan_int(pds);
      if (k != lp->m)
         pds_error(pds, "wrong number of rows\n");
      k = pds_scan_int(pds);
      if (k != lp->n)
         pds_error(pds, "wrong number of columns\n");
      /* primal status, dual status, objective value */
      k = pds_scan_int(pds);
      if (!(k == GLP_UNDEF || k == GLP_FEAS || k == GLP_INFEAS ||
            k == GLP_NOFEAS))
         pds_error(pds, "invalid primal status\n");
      lp->pbs_stat = k;
      k = pds_scan_int(pds);
      if (!(k == GLP_UNDEF || k == GLP_FEAS || k == GLP_INFEAS ||
            k == GLP_NOFEAS))
         pds_error(pds, "invalid dual status\n");
      lp->dbs_stat = k;
      lp->obj_val = pds_scan_num(pds);
      /* rows (auxiliary variables) */
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         /* status, primal value, dual value */
         k = pds_scan_int(pds);
         if (!(k == GLP_BS || k == GLP_NL || k == GLP_NU ||
               k == GLP_NF || k == GLP_NS))
            pds_error(pds, "invalid row status\n");
         glp_set_row_stat(lp, i, k);
         row->prim = pds_scan_num(pds);
         row->dual = pds_scan_num(pds);
      }
      /* columns (structural variables) */
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         /* status, primal value, dual value */
         k = pds_scan_int(pds);
         if (!(k == GLP_BS || k == GLP_NL || k == GLP_NU ||
               k == GLP_NF || k == GLP_NS))
            pds_error(pds, "invalid column status\n");
         glp_set_col_stat(lp, j, k);
         col->prim = pds_scan_num(pds);
         col->dual = pds_scan_num(pds);
      }
      xprintf("glp_read_sol: %d lines were read\n", pds->count);
done: if (ret) lp->pbs_stat = lp->dbs_stat = GLP_UNDEF;
      if (pds != NULL) pds_close_file(pds);
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_write_sol - write basic solution to text file
*
*  SYNOPSIS
*
*  int glp_write_sol(glp_prob *lp, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_write_sol writes the current basic solution to a
*  text file whose name is specified by the parameter fname. This file
*  can be read back with the routine glp_read_sol.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero.
*
*  FILE FORMAT
*
*  The file created by the routine glp_write_sol is a plain text file,
*  which contains the following information:
*
*     m n
*     p_stat d_stat obj_val
*     r_stat[1] r_prim[1] r_dual[1]
*     . . .
*     r_stat[m] r_prim[m] r_dual[m]
*     c_stat[1] c_prim[1] c_dual[1]
*     . . .
*     c_stat[n] c_prim[n] c_dual[n]
*
*  where:
*  m is the number of rows (auxiliary variables);
*  n is the number of columns (structural variables);
*  p_stat is the primal status of the basic solution (GLP_UNDEF = 1,
*     GLP_FEAS = 2, GLP_INFEAS = 3, or GLP_NOFEAS = 4);
*  d_stat is the dual status of the basic solution (GLP_UNDEF = 1,
*     GLP_FEAS = 2, GLP_INFEAS = 3, or GLP_NOFEAS = 4);
*  obj_val is the objective value;
*  r_stat[i], i = 1,...,m, is the status of i-th row (GLP_BS = 1,
*     GLP_NL = 2, GLP_NU = 3, GLP_NF = 4, or GLP_NS = 5);
*  r_prim[i], i = 1,...,m, is the primal value of i-th row;
*  r_dual[i], i = 1,...,m, is the dual value of i-th row;
*  c_stat[j], j = 1,...,n, is the status of j-th column (GLP_BS = 1,
*     GLP_NL = 2, GLP_NU = 3, GLP_NF = 4, or GLP_NS = 5);
*  c_prim[j], j = 1,...,n, is the primal value of j-th column;
*  c_dual[j], j = 1,...,n, is the dual value of j-th column. */

int glp_write_sol(glp_prob *lp, const char *fname)
{     FILE *fp;
      int i, j, ret = 0;
      xprintf("glp_write_sol: writing basic solution to `%s'...\n",
         fname);
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("glp_write_sol: unable to create `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      /* number of rows, number of columns */
      fprintf(fp, "%d %d\n", lp->m, lp->n);
      /* primal status, dual status, objective value */
      fprintf(fp, "%d %d %.*g\n", lp->pbs_stat, lp->dbs_stat, DBL_DIG,
         lp->obj_val);
      /* rows (auxiliary variables) */
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         /* status, primal value, dual value */
         fprintf(fp, "%d %.*g %.*g\n", row->stat, DBL_DIG, row->prim,
            DBL_DIG, row->dual);
      }
      /* columns (structural variables) */
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         /* status, primal value, dual value */
         fprintf(fp, "%d %.*g %.*g\n", col->stat, DBL_DIG, col->prim,
            DBL_DIG, col->dual);
      }
      fflush(fp);
      if (ferror(fp))
      {  xprintf("glp_write_sol: writing error on `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      xprintf("glp_write_sol: %d lines were written\n", 2 + lp->m +
         lp->n);
done: if (fp != NULL) fclose(fp);
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_read_ipt - read interior-point solution from text file
*
*  SYNOPSIS
*
*  int glp_read_ipt(glp_prob *lp, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_read_ipt reads interior-point solution from a text
*  file whose name is specified by the parameter fname into the problem
*  object.
*
*  For the file format see description of the routine glp_write_ipt.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero. */

int glp_read_ipt(glp_prob *lp, const char *fname)
{     PDS *pds;
      jmp_buf jump;
      int i, j, k, ret = 0;
      xprintf("glp_read_ipt: reading interior-point solution from `%s'."
         "..\n", fname);
      pds = pds_open_file(fname);
      if (pds == NULL)
      {  xprintf("glp_read_ipt: unable to open `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      if (setjmp(jump))
      {  ret = 1;
         goto done;
      }
      pds_set_jump(pds, jump);
      /* number of rows, number of columns */
      k = pds_scan_int(pds);
      if (k != lp->m)
         pds_error(pds, "wrong number of rows\n");
      k = pds_scan_int(pds);
      if (k != lp->n)
         pds_error(pds, "wrong number of columns\n");
      /* solution status, objective value */
      k = pds_scan_int(pds);
      if (!(k == GLP_UNDEF || k == GLP_OPT))
         pds_error(pds, "invalid solution status\n");
      lp->ipt_stat = k;
      lp->ipt_obj = pds_scan_num(pds);
      /* rows (auxiliary variables) */
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         /* primal value, dual value */
         row->pval = pds_scan_num(pds);
         row->dval = pds_scan_num(pds);
      }
      /* columns (structural variables) */
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         /* primal value, dual value */
         col->pval = pds_scan_num(pds);
         col->dval = pds_scan_num(pds);
      }
      xprintf("glp_read_ipt: %d lines were read\n", pds->count);
done: if (ret) lp->ipt_stat = GLP_UNDEF;
      if (pds != NULL) pds_close_file(pds);
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_write_ipt - write interior-point solution to text file
*
*  SYNOPSIS
*
*  int glp_write_ipt(glp_prob *lp, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_write_ipt writes the current interior-point solution
*  to a text file whose name is specified by the parameter fname. This
*  file can be read back with the routine glp_read_ipt.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero.
*
*  FILE FORMAT
*
*  The file created by the routine glp_write_ipt is a plain text file,
*  which contains the following information:
*
*     m n
*     stat obj_val
*     r_prim[1] r_dual[1]
*     . . .
*     r_prim[m] r_dual[m]
*     c_prim[1] c_dual[1]
*     . . .
*     c_prim[n] c_dual[n]
*
*  where:
*  m is the number of rows (auxiliary variables);
*  n is the number of columns (structural variables);
*  stat is the solution status (GLP_UNDEF = 1 or GLP_OPT = 5);
*  obj_val is the objective value;
*  r_prim[i], i = 1,...,m, is the primal value of i-th row;
*  r_dual[i], i = 1,...,m, is the dual value of i-th row;
*  c_prim[j], j = 1,...,n, is the primal value of j-th column;
*  c_dual[j], j = 1,...,n, is the dual value of j-th column. */

int glp_write_ipt(glp_prob *lp, const char *fname)
{     FILE *fp;
      int i, j, ret = 0;
      xprintf("glp_write_ipt: writing interior-point solution to `%s'.."
         ".\n", fname);
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("glp_write_ipt: unable to create `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      /* number of rows, number of columns */
      fprintf(fp, "%d %d\n", lp->m, lp->n);
      /* solution status, objective value */
      fprintf(fp, "%d %.*g\n", lp->ipt_stat, DBL_DIG, lp->ipt_obj);
      /* rows (auxiliary variables) */
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         /* primal value, dual value */
         fprintf(fp, "%.*g %.*g\n", DBL_DIG, row->pval, DBL_DIG,
            row->dval);
      }
      /* columns (structural variables) */
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         /* primal value, dual value */
         fprintf(fp, "%.*g %.*g\n", DBL_DIG, col->pval, DBL_DIG,
            col->dval);
      }
      fflush(fp);
      if (ferror(fp))
      {  xprintf("glp_write_ipt: writing error on `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      xprintf("glp_write_ipt: %d lines were written\n", 2 + lp->m +
         lp->n);
done: if (fp != NULL) fclose(fp);
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_read_mip - read MIP solution from text file
*
*  SYNOPSIS
*
*  int glp_read_mip(glp_prob *mip, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_read_mip reads MIP solution from a text file whose
*  name is specified by the parameter fname into the problem object.
*
*  For the file format see description of the routine glp_write_mip.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero. */

int glp_read_mip(glp_prob *mip, const char *fname)
{     PDS *pds;
      jmp_buf jump;
      int i, j, k, ret = 0;
      xprintf("glp_read_mip: reading MIP solution from `%s'...\n",
         fname);
      pds = pds_open_file(fname);
      if (pds == NULL)
      {  xprintf("glp_read_sol: unable to open `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      if (setjmp(jump))
      {  ret = 1;
         goto done;
      }
      pds_set_jump(pds, jump);
      /* number of rows, number of columns */
      k = pds_scan_int(pds);
      if (k != mip->m)
         pds_error(pds, "wrong number of rows\n");
      k = pds_scan_int(pds);
      if (k != mip->n)
         pds_error(pds, "wrong number of columns\n");
      /* solution status, objective value */
      k = pds_scan_int(pds);
      if (!(k == GLP_UNDEF || k == GLP_OPT || k == GLP_FEAS ||
            k == GLP_NOFEAS))
         pds_error(pds, "invalid solution status\n");
      mip->mip_stat = k;
      mip->mip_obj = pds_scan_num(pds);
      /* rows (auxiliary variables) */
      for (i = 1; i <= mip->m; i++)
      {  GLPROW *row = mip->row[i];
         row->mipx = pds_scan_num(pds);
      }
      /* columns (structural variables) */
      for (j = 1; j <= mip->n; j++)
      {  GLPCOL *col = mip->col[j];
         col->mipx = pds_scan_num(pds);
         if (col->kind == GLP_IV && col->mipx != floor(col->mipx))
            pds_error(pds, "non-integer column value");
      }
      xprintf("glp_read_mip: %d lines were read\n", pds->count);
done: if (ret) mip->mip_stat = GLP_UNDEF;
      if (pds != NULL) pds_close_file(pds);
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_write_mip - write MIP solution to text file
*
*  SYNOPSIS
*
*  int glp_write_mip(glp_prob *mip, const char *fname);
*
*  DESCRIPTION
*
*  The routine glp_write_mip writes the current MIP solution to a text
*  file whose name is specified by the parameter fname. This file can
*  be read back with the routine glp_read_mip.
*
*  RETURNS
*
*  On success the routine returns zero, otherwise non-zero.
*
*  FILE FORMAT
*
*  The file created by the routine glp_write_sol is a plain text file,
*  which contains the following information:
*
*     m n
*     stat obj_val
*     r_val[1]
*     . . .
*     r_val[m]
*     c_val[1]
*     . . .
*     c_val[n]
*
*  where:
*  m is the number of rows (auxiliary variables);
*  n is the number of columns (structural variables);
*  stat is the solution status (GLP_UNDEF = 1, GLP_FEAS = 2,
*     GLP_NOFEAS = 4, or GLP_OPT = 5);
*  obj_val is the objective value;
*  r_val[i], i = 1,...,m, is the value of i-th row;
*  c_val[j], j = 1,...,n, is the value of j-th column. */

int glp_write_mip(glp_prob *mip, const char *fname)
{     FILE *fp;
      int i, j, ret = 0;
      xprintf("glp_write_mip: writing MIP solution to `%s'...\n",
         fname);
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("glp_write_mip: unable to create `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      /* number of rows, number of columns */
      fprintf(fp, "%d %d\n", mip->m, mip->n);
      /* solution status, objective value */
      fprintf(fp, "%d %.*g\n", mip->mip_stat, DBL_DIG, mip->mip_obj);
      /* rows (auxiliary variables) */
      for (i = 1; i <= mip->m; i++)
         fprintf(fp, "%.*g\n", DBL_DIG, mip->row[i]->mipx);
      /* columns (structural variables) */
      for (j = 1; j <= mip->n; j++)
         fprintf(fp, "%.*g\n", DBL_DIG, mip->col[j]->mipx);
      fflush(fp);
      if (ferror(fp))
      {  xprintf("glp_write_mip: writing error on `%s' - %s\n", fname,
            strerror(errno));
         ret = 1;
         goto done;
      }
      xprintf("glp_write_mip: %d lines were written\n", 2 + mip->m +
         mip->n);
done: if (fp != NULL) fclose(fp);
      return ret;
}

/* eof */
