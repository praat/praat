/* glpapi07.c (interior-point method routines) */

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
*  glp_ipt_status - retrieve status of interior-point solution
*
*  SYNOPSIS
*
*  int glp_ipt_status(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_ipt_status reports the status of solution found by
*  the interior-point solver as follows:
*
*  GLP_UNDEF  - interior-point solution is undefined;
*  GLP_OPT    - interior-point solution is optimal. */

int glp_ipt_status(glp_prob *lp)
{     int ipt_stat = lp->ipt_stat;
      return ipt_stat;
}

/***********************************************************************
*  NAME
*
*  glp_ipt_obj_val - retrieve objective value (interior point)
*
*  SYNOPSIS
*
*  double glp_ipt_obj_val(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_ipt_obj_val returns value of the objective function
*  for interior-point solution. */

double glp_ipt_obj_val(glp_prob *lp)
{     struct LPXCPS *cps = lp->cps;
      double z;
      z = lp->ipt_obj;
      if (cps->round && fabs(z) < 1e-9) z = 0.0;
      return z;
}

/***********************************************************************
*  NAME
*
*  glp_ipt_row_prim - retrieve row primal value (interior point)
*
*  SYNOPSIS
*
*  double glp_ipt_row_prim(glp_prob *lp, int i);
*
*  RETURNS
*
*  The routine glp_ipt_row_prim returns primal value of the auxiliary
*  variable associated with i-th row. */

double glp_ipt_row_prim(glp_prob *lp, int i)
{     struct LPXCPS *cps = lp->cps;
      double pval;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_ipt_row_prim: i = %d; row number out of range\n",
            i);
      pval = lp->row[i]->pval;
      if (cps->round && fabs(pval) < 1e-9) pval = 0.0;
      return pval;
}

/***********************************************************************
*  NAME
*
*  glp_ipt_row_dual - retrieve row dual value (interior point)
*
*  SYNOPSIS
*
*  double glp_ipt_row_dual(glp_prob *lp, int i);
*
*  RETURNS
*
*  The routine glp_ipt_row_dual returns dual value (i.e. reduced cost)
*  of the auxiliary variable associated with i-th row. */

double glp_ipt_row_dual(glp_prob *lp, int i)
{     struct LPXCPS *cps = lp->cps;
      double dval;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_ipt_row_dual: i = %d; row number out of range\n",
            i);
      dval = lp->row[i]->dval;
      if (cps->round && fabs(dval) < 1e-9) dval = 0.0;
      return dval;
}

/***********************************************************************
*  NAME
*
*  glp_ipt_col_prim - retrieve column primal value (interior point)
*
*  SYNOPSIS
*
*  double glp_ipt_col_prim(glp_prob *lp, int j);
*
*  RETURNS
*
*  The routine glp_ipt_col_prim returns primal value of the structural
*  variable associated with j-th column. */

double glp_ipt_col_prim(glp_prob *lp, int j)
{     struct LPXCPS *cps = lp->cps;
      double pval;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_ipt_col_prim: j = %d; column number out of range\n"
            , j);
      pval = lp->col[j]->pval;
      if (cps->round && fabs(pval) < 1e-9) pval = 0.0;
      return pval;
}

/***********************************************************************
*  NAME
*
*  glp_ipt_col_dual - retrieve column dual value (interior point)
*
*  SYNOPSIS
*
*  #include "glplpx.h"
*  double glp_ipt_col_dual(glp_prob *lp, int j);
*
*  RETURNS
*
*  The routine glp_ipt_col_dual returns dual value (i.e. reduced cost)
*  of the structural variable associated with j-th column. */

double glp_ipt_col_dual(glp_prob *lp, int j)
{     struct LPXCPS *cps = lp->cps;
      double dval;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_ipt_col_dual: j = %d; column number out of range\n"
            , j);
      dval = lp->col[j]->dval;
      if (cps->round && fabs(dval) < 1e-9) dval = 0.0;
      return dval;
}

/* eof */
