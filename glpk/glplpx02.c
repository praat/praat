/* glplpx02.c */

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

/*----------------------------------------------------------------------
-- lpx_order_matrix - order rows and columns of the constraint matrix.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_order_matrix(glp_prob *lp);
--
-- *Description*
--
-- The routine lpx_order_matrix rebuilds row and column linked lists of
-- the constraint matrix of the specified problem object.
--
-- On exit the constraint matrix is not changed, however, elements in
-- the row linked lists are ordered in ascending their column indices,
-- and elements in the column linked are ordered in ascending their row
-- indices. */

void lpx_order_matrix(glp_prob *lp)
{     GLPAIJ *aij;
      int i, j;
      /* rebuild row lists */
      for (i = lp->m; i >= 1; i--)
         lp->row[i]->ptr = NULL;
      for (j = lp->n; j >= 1; j--)
      {  for (aij = lp->col[j]->ptr; aij != NULL; aij = aij->c_next)
         {  i = aij->row->i;
            aij->r_prev = NULL;
            aij->r_next = lp->row[i]->ptr;
            if (aij->r_next != NULL) aij->r_next->r_prev = aij;
            lp->row[i]->ptr = aij;
         }
      }
      /* rebuild column lists */
      for (j = lp->n; j >= 1; j--)
         lp->col[j]->ptr = NULL;
      for (i = lp->m; i >= 1; i--)
      {  for (aij = lp->row[i]->ptr; aij != NULL; aij = aij->r_next)
         {  j = aij->col->j;
            aij->c_prev = NULL;
            aij->c_next = lp->col[j]->ptr;
            if (aij->c_next != NULL) aij->c_next->c_prev = aij;
            lp->col[j]->ptr = aij;
         }
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_put_solution - store basic solution components
*
*  SYNOPSIS
*
*  void glp_put_solution(glp_prob *lp, int inval, const int *p_stat,
*     const int *d_stat, const double *obj_val, const int r_stat[],
*     const double r_prim[], const double r_dual[], const int c_stat[],
*     const double c_prim[], const double c_dual[])
*
*  DESCRIPTION
*
*  The routine glp_put_solution stores basic solution components to the
*  specified problem object.
*
*  The parameter inval is the basis factorization invalidity flag.
*  If this flag is clear, the current status of the basis factorization
*  remains unchanged. If this flag is set, the routine invalidates the
*  basis factorization.
*
*  The parameter p_stat is a pointer to the status of primal basic
*  solution, which should be specified as follows:
*
*  GLP_UNDEF  - primal solution is undefined;
*  GLP_FEAS   - primal solution is feasible;
*  GLP_INFEAS - primal solution is infeasible;
*  GLP_NOFEAS - no primal feasible solution exists.
*
*  If the parameter p_stat is NULL, the current status of primal basic
*  solution remains unchanged.
*
*  The parameter d_stat is a pointer to the status of dual basic
*  solution, which should be specified as follows:
*
*  GLP_UNDEF  - dual solution is undefined;
*  GLP_FEAS   - dual solution is feasible;
*  GLP_INFEAS - dual solution is infeasible;
*  GLP_NOFEAS - no dual feasible solution exists.
*
*  If the parameter d_stat is NULL, the current status of dual basic
*  solution remains unchanged.
*
*  The parameter obj_val is a pointer to the objective function value.
*  If it is NULL, the current value of the objective function remains
*  unchanged.
*
*  The array element r_stat[i], 1 <= i <= m (where m is the number of
*  rows in the problem object), specifies the status of i-th auxiliary
*  variable, which should be specified as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable on lower bound;
*  GLP_NU - non-basic variable on upper bound;
*  GLP_NF - non-basic free variable;
*  GLP_NS - non-basic fixed variable.
*
*  If the parameter r_stat is NULL, the current statuses of auxiliary
*  variables remain unchanged.
*
*  The array element r_prim[i], 1 <= i <= m (where m is the number of
*  rows in the problem object), specifies a primal value of i-th
*  auxiliary variable. If the parameter r_prim is NULL, the current
*  primal values of auxiliary variables remain unchanged.
*
*  The array element r_dual[i], 1 <= i <= m (where m is the number of
*  rows in the problem object), specifies a dual value (reduced cost)
*  of i-th auxiliary variable. If the parameter r_dual is NULL, the
*  current dual values of auxiliary variables remain unchanged.
*
*  The array element c_stat[j], 1 <= j <= n (where n is the number of
*  columns in the problem object), specifies the status of j-th
*  structural variable, which should be specified as follows:
*
*  GLP_BS - basic variable;
*  GLP_NL - non-basic variable on lower bound;
*  GLP_NU - non-basic variable on upper bound;
*  GLP_NF - non-basic free variable;
*  GLP_NS - non-basic fixed variable.
*
*  If the parameter c_stat is NULL, the current statuses of structural
*  variables remain unchanged.
*
*  The array element c_prim[j], 1 <= j <= n (where n is the number of
*  columns in the problem object), specifies a primal value of j-th
*  structural variable. If the parameter c_prim is NULL, the current
*  primal values of structural variables remain unchanged.
*
*  The array element c_dual[j], 1 <= j <= n (where n is the number of
*  columns in the problem object), specifies a dual value (reduced cost)
*  of j-th structural variable. If the parameter c_dual is NULL, the
*  current dual values of structural variables remain unchanged. */

void glp_put_solution(glp_prob *lp, int inval, const int *p_stat,
      const int *d_stat, const double *obj_val, const int r_stat[],
      const double r_prim[], const double r_dual[], const int c_stat[],
      const double c_prim[], const double c_dual[])
{     GLPROW *row;
      GLPCOL *col;
      int i, j;
      /* invalidate the basis factorization, if required */
      if (inval) lp->valid = 0;
      /* store primal status */
      if (p_stat != NULL)
      {  if (!(*p_stat == GLP_UNDEF  || *p_stat == GLP_FEAS ||
               *p_stat == GLP_INFEAS || *p_stat == GLP_NOFEAS))
            xfault("glp_put_solution: p_stat = %d; invalid primal statu"
               "s\n", *p_stat);
         lp->pbs_stat = *p_stat;
      }
      /* store dual status */
      if (d_stat != NULL)
      {  if (!(*d_stat == GLP_UNDEF  || *d_stat == GLP_FEAS ||
               *d_stat == GLP_INFEAS || *d_stat == GLP_NOFEAS))
            xfault("glp_put_solution: d_stat = %d; invalid dual status "
               "\n", *d_stat);
         lp->dbs_stat = *d_stat;
      }
      /* store objective function value */
      if (obj_val != NULL) lp->obj_val = *obj_val;
      /* store row solution components */
      for (i = 1; i <= lp->m; i++)
      {  row = lp->row[i];
         if (r_stat != NULL)
         {  if (!(r_stat[i] == GLP_BS ||
                  row->type == GLP_FR && r_stat[i] == GLP_NF ||
                  row->type == GLP_LO && r_stat[i] == GLP_NL ||
                  row->type == GLP_UP && r_stat[i] == GLP_NU ||
                  row->type == GLP_DB && r_stat[i] == GLP_NL ||
                  row->type == GLP_DB && r_stat[i] == GLP_NU ||
                  row->type == GLP_FX && r_stat[i] == GLP_NS))
               xfault("glp_put_solution: r_stat[%d] = %d; invalid row s"
                  "tatus\n", i, r_stat[i]);
            row->stat = r_stat[i];
         }
         if (r_prim != NULL) row->prim = r_prim[i];
         if (r_dual != NULL) row->dual = r_dual[i];
      }
      /* store column solution components */
      for (j = 1; j <= lp->n; j++)
      {  col = lp->col[j];
         if (c_stat != NULL)
         {  if (!(c_stat[j] == GLP_BS ||
                  col->type == GLP_FR && c_stat[j] == GLP_NF ||
                  col->type == GLP_LO && c_stat[j] == GLP_NL ||
                  col->type == GLP_UP && c_stat[j] == GLP_NU ||
                  col->type == GLP_DB && c_stat[j] == GLP_NL ||
                  col->type == GLP_DB && c_stat[j] == GLP_NU ||
                  col->type == GLP_FX && c_stat[j] == GLP_NS))
               xfault("glp_put_solution: c_stat[%d] = %d; invalid colum"
                  "n status\n", j, c_stat[j]);
            col->stat = c_stat[j];
         }
         if (c_prim != NULL) col->prim = c_prim[j];
         if (c_dual != NULL) col->dual = c_dual[j];
      }
      return;
}

/*----------------------------------------------------------------------
-- lpx_put_lp_basis - store LP basis information.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_put_lp_basis(glp_prob *lp, int b_stat, int basis[],
--    BFI *b_inv);
--
-- *Description*
--
-- The routine lpx_put_lp_basis stores an LP basis information into the
-- specified problem object.
--
-- NOTE: This routine is intended for internal use only. */

void lpx_put_lp_basis(glp_prob *lp, int valid, int basis[], BFD *b_inv)
{     GLPROW *row;
      GLPCOL *col;
      int i, k;
      /* store basis status */
      lp->valid = valid;
      /* store basis header */
      if (basis != NULL)
         for (i = 1; i <= lp->m; i++) lp->bhead[i] = basis[i];
      /* store factorization of the basis matrix */
      xassert(lp->bfd == b_inv);
      /* if the basis is claimed to be valid, check it */
      if (valid)
      {  for (k = 1; k <= lp->m; k++) lp->row[k]->bind = 0;
         for (k = 1; k <= lp->n; k++) lp->col[k]->bind = 0;
         for (i = 1; i <= lp->m; i++)
         {  k = lp->bhead[i];
            if (!(1 <= k && k <= lp->m+lp->n))
               xfault("lpx_put_lp_basis: basis[%d] = %d; invalid refere"
                  "nce to basic variable\n", i, k);
            if (k <= lp->m)
            {  row = lp->row[k];
               if (row->stat != GLP_BS)
                  xfault("lpx_put_lp_basis: basis[%d] = %d; invalid ref"
                     "erence to non-basic row\n", i, k);
               if (row->bind != 0)
                  xfault("lpx_put_lp_basis: basis[%d] = %d; duplicate r"
                     "eference to basic row\n", i, k);
               row->bind = i;
            }
            else
            {  col = lp->col[k-lp->m];
               if (col->stat != GLP_BS)
                  xfault("lpx_put_lp_basis: basis[%d] = %d; invalid ref"
                     "erence to non-basic column\n", i, k);
               if (col->bind != 0)
                  xfault("lpx_put_lp_basis: basis[%d] = %d; duplicate r"
                     "eference to basic column\n", i, k);
               col->bind = i;
            }
         }
      }
      return;
}

/*----------------------------------------------------------------------
-- lpx_put_ray_info - store row/column which causes unboundness.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_put_ray_info(glp_prob *lp, int k);
--
-- *Description*
--
-- The routine lpx_put_ray_info stores the number of row/column, which
-- causes primal unboundness.
--
-- NOTE: This routine is intended for internal use only. */

void lpx_put_ray_info(glp_prob *lp, int k)
{     if (!(0 <= k && k <= lp->m+lp->n))
         xfault("lpx_put_ray_info: ray = %d; row/column number out of r"
            "ange\n", k);
      lp->some = k;
      return;
}

/*----------------------------------------------------------------------
-- lpx_put_ipt_soln - store interior-point solution components.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_put_ipt_soln(glp_prob *lp, int t_stat, double row_pval[],
--    double row_dval[], double col_pval[], double col_dval[]);
--
-- *Description*
--
-- The routine lpx_put_ipt_soln stores solution components obtained by
-- interior-point solver into the specified problem object.
--
-- NOTE: This routine is intended for internal use only. */

void lpx_put_ipt_soln(glp_prob *lp, int t_stat, double row_pval[],
      double row_dval[], double col_pval[], double col_dval[])
{     GLPROW *row;
      GLPCOL *col;
      double sum;
      int i, j;
      /* store interior-point status */
      if (!(t_stat == LPX_T_UNDEF || t_stat == LPX_T_OPT))
         xfault("lpx_put_ipm_soln: t_stat = %d; invalid interior-point "
            "status\n", t_stat);
      lp->ipt_stat = (t_stat == LPX_T_UNDEF ? GLP_UNDEF : GLP_OPT);
      /* store row solution components */
      for (i = 1; i <= lp->m; i++)
      {  row = lp->row[i];
         if (row_pval != NULL) row->pval = row_pval[i];
         if (row_dval != NULL) row->dval = row_dval[i];
      }
      /* store column solution components */
      for (j = 1; j <= lp->n; j++)
      {  col = lp->col[j];
         if (col_pval != NULL) col->pval = col_pval[j];
         if (col_dval != NULL) col->dval = col_dval[j];
      }
      /* compute the objective function value */
      sum = lp->c0;
      for (j = 1; j <= lp->n; j++)
      {  col = lp->col[j];
         sum += col->coef * col->pval;
      }
      lp->ipt_obj = sum;
      return;
}

/*----------------------------------------------------------------------
-- lpx_put_mip_soln - store mixed integer solution components.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_put_mip_soln(glp_prob *lp, int i_stat, double row_mipx[],
--    double col_mipx[]);
--
-- *Description*
--
-- The routine lpx_put_mip_soln stores solution components obtained by
-- branch-and-bound solver into the specified problem object.
--
-- NOTE: This routine is intended for internal use only. */

void lpx_put_mip_soln(glp_prob *lp, int i_stat, double row_mipx[],
      double col_mipx[])
{     GLPROW *row;
      GLPCOL *col;
      int i, j;
      double sum;
      /* store mixed integer status */
#if 0
      if (!(i_stat == LPX_I_UNDEF || i_stat == LPX_I_OPT ||
            i_stat == LPX_I_FEAS  || i_stat == LPX_I_NOFEAS))
         fault("lpx_put_mip_soln: i_stat = %d; invalid mixed integer st"
            "atus", i_stat);
      lp->i_stat = i_stat;
#else
      switch (i_stat)
      {  case LPX_I_UNDEF:
            lp->mip_stat = GLP_UNDEF; break;
         case LPX_I_OPT:
            lp->mip_stat = GLP_OPT;  break;
         case LPX_I_FEAS:
            lp->mip_stat = GLP_FEAS; break;
         case LPX_I_NOFEAS:
            lp->mip_stat = GLP_NOFEAS; break;
         default:
            xfault("lpx_put_mip_soln: i_stat = %d; invalid mixed intege"
               "r status\n", i_stat);
      }
#endif
      /* store row solution components */
      if (row_mipx != NULL)
      {  for (i = 1; i <= lp->m; i++)
         {  row = lp->row[i];
            row->mipx = row_mipx[i];
         }
      }
      /* store column solution components */
      if (col_mipx != NULL)
      {  for (j = 1; j <= lp->n; j++)
         {  col = lp->col[j];
            col->mipx = col_mipx[j];
         }
      }
      /* if the solution is claimed to be integer feasible, check it */
      if (lp->mip_stat == GLP_OPT || lp->mip_stat == GLP_FEAS)
      {  for (j = 1; j <= lp->n; j++)
         {  col = lp->col[j];
            if (col->kind == GLP_IV && col->mipx != floor(col->mipx))
               xfault("lpx_put_mip_soln: col_mipx[%d] = %.*g; must be i"
                  "ntegral\n", j, DBL_DIG, col->mipx);
         }
      }
      /* compute the objective function value */
      sum = lp->c0;
      for (j = 1; j <= lp->n; j++)
      {  col = lp->col[j];
         sum += col->coef * col->mipx;
      }
      lp->mip_obj = sum;
      return;
}

/*----------------------------------------------------------------------
-- lpx_get_ray_info - retrieve row/column which causes unboundness.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- int lpx_get_ray_info(glp_prob *lp);
--
-- *Returns*
--
-- The routine lpx_get_ray_info returns the number k of some non-basic
-- variable x[k], which causes primal unboundness. If such a variable
-- cannot be identified, the routine returns zero.
--
-- If 1 <= k <= m, x[k] is the auxiliary variable associated with k-th
-- row, if m+1 <= k <= m+n, x[k] is the structural variable associated
-- with (k-m)-th column, where m is the number of rows, n is the number
-- of columns in the LP problem object. */

int lpx_get_ray_info(glp_prob *lp)
{     int k;
      k = lp->some;
      return k;
}

/* eof */
