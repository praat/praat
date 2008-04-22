/* glpapi05.c (LP basis constructing routines) */

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
*  glp_set_row_stat - set (change) row status
*
*  SYNOPSIS
*
*  void glp_set_row_stat(glp_prob *lp, int i, int stat);
*
*  DESCRIPTION
*
*  The routine glp_set_row_stat sets (changes) status of the auxiliary
*  variable associated with i-th row.
*
*  The new status of the auxiliary variable should be specified by the
*  parameter stat as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable;
*  GLP_NU - non-basic variable on its upper bound; if the variable is
*           not double-bounded, this means the same as GLP_NL (only in
*           case of this routine);
*  GLP_NF - the same as GLP_NL (only in case of this routine);
*  GLP_NS - the same as GLP_NL (only in case of this routine). */

void glp_set_row_stat(glp_prob *lp, int i, int stat)
{     GLPROW *row;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_set_row_stat: i = %d; row number out of range\n",
            i);
      if (!(stat == GLP_BS || stat == GLP_NL || stat == GLP_NU ||
            stat == GLP_NF || stat == GLP_NS))
         xfault("glp_set_row_stat: i = %d; stat = %d; invalid status\n",
            i, stat);
      row = lp->row[i];
      if (stat != GLP_BS)
      {  switch (row->type)
         {  case GLP_FR: stat = GLP_NF; break;
            case GLP_LO: stat = GLP_NL; break;
            case GLP_UP: stat = GLP_NU; break;
            case GLP_DB: if (stat != GLP_NU) stat = GLP_NL; break;
            case GLP_FX: stat = GLP_NS; break;
            default: xassert(row != row);
         }
      }
      if (row->stat == GLP_BS && stat != GLP_BS ||
          row->stat != GLP_BS && stat == GLP_BS)
      {  /* invalidate the basis factorization */
         lp->valid = 0;
      }
      row->stat = stat;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_col_stat - set (change) column status
*
*  SYNOPSIS
*
*  void glp_set_col_stat(glp_prob *lp, int j, int stat);
*
*  DESCRIPTION
*
*  The routine glp_set_col_stat sets (changes) status of the structural
*  variable associated with j-th column.
*
*  The new status of the structural variable should be specified by the
*  parameter stat as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable;
*  GLP_NU - non-basic variable on its upper bound; if the variable is
*           not double-bounded, this means the same as GLP_NL (only in
*           case of this routine);
*  GLP_NF - the same as GLP_NL (only in case of this routine);
*  GLP_NS - the same as GLP_NL (only in case of this routine). */

void glp_set_col_stat(glp_prob *lp, int j, int stat)
{     GLPCOL *col;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_set_col_stat: j = %d; column number out of range\n"
            , j);
      if (!(stat == GLP_BS || stat == GLP_NL || stat == GLP_NU ||
            stat == GLP_NF || stat == GLP_NS))
         xfault("glp_set_col_stat: j = %d; stat = %d; invalid status\n",
            j, stat);
      col = lp->col[j];
      if (stat != GLP_BS)
      {  switch (col->type)
         {  case GLP_FR: stat = GLP_NF; break;
            case GLP_LO: stat = GLP_NL; break;
            case GLP_UP: stat = GLP_NU; break;
            case GLP_DB: if (stat != GLP_NU) stat = GLP_NL; break;
            case GLP_FX: stat = GLP_NS; break;
            default: xassert(col != col);
         }
      }
      if (col->stat == GLP_BS && stat != GLP_BS ||
          col->stat != GLP_BS && stat == GLP_BS)
      {  /* invalidate the basis factorization */
         lp->valid = 0;
      }
      col->stat = stat;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_get_row_stat - retrieve row status
*
*  SYNOPSIS
*
*  int glp_get_row_stat(glp_prob *lp, int i);
*
*  RETURNS
*
*  The routine glp_get_row_stat returns current status assigned to the
*  auxiliary variable associated with i-th row as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable on its lower bound;
*  GLP_NU - non-basic variable on its upper bound;
*  GLP_NF - non-basic free (unbounded) variable;
*  GLP_NS - non-basic fixed variable. */

int glp_get_row_stat(glp_prob *lp, int i)
{     if (!(1 <= i && i <= lp->m))
         xfault("glp_get_row_stat: i = %d; row number out of range\n",
            i);
      return lp->row[i]->stat;
}

/***********************************************************************
*  NAME
*
*  glp_get_col_stat - retrieve column status
*
*  SYNOPSIS
*
*  int glp_get_col_stat(glp_prob *lp, int j);
*
*  RETURNS
*
*  The routine glp_get_col_stat returns current status assigned to the
*  structural variable associated with j-th column as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable on its lower bound;
*  GLP_NU - non-basic variable on its upper bound;
*  GLP_NF - non-basic free (unbounded) variable;
*  GLP_NS - non-basic fixed variable. */

int glp_get_col_stat(glp_prob *lp, int j)
{     if (!(1 <= j && j <= lp->n))
         xfault("glp_get_col_stat: j = %d; column number out of range\n"
            , j);
      return lp->col[j]->stat;
}

/* eof */
