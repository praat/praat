/* glpapi08.c (mixed integer programming routines) */

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

#include "glpios.h"
#define xfault xerror

/***********************************************************************
*  NAME
*
*  glp_set_col_kind - set (change) column kind
*
*  SYNOPSIS
*
*  void glp_set_col_kind(glp_prob *mip, int j, int kind);
*
*  DESCRIPTION
*
*  The routine glp_set_col_kind sets (changes) the kind of j-th column
*  (structural variable) as specified by the parameter kind:
*
*  GLP_CV - continuous variable;
*  GLP_IV - integer variable;
*  GLP_BV - binary variable. */

void glp_set_col_kind(glp_prob *mip, int j, int kind)
{     GLPCOL *col;
      if (!(1 <= j && j <= mip->n))
         xfault("glp_set_col_kind: j = %d; column number out of range\n"
            , j);
      col = mip->col[j];
      switch (kind)
      {  case GLP_CV:
            col->kind = GLP_CV;
            break;
         case GLP_IV:
            col->kind = GLP_IV;
            break;
         case GLP_BV:
            col->kind = GLP_IV;
            if (!(col->type == GLP_DB && col->lb == 0.0 && col->ub ==
               1.0)) glp_set_col_bnds(mip, j, GLP_DB, 0.0, 1.0);
            break;
         default:
            xfault("glp_set_col_kind: j = %d; kind = %d; invalid column"
               " kind\n", j, kind);
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_get_col_kind - retrieve column kind
*
*  SYNOPSIS
*
*  int glp_get_col_kind(glp_prob *mip, int j);
*
*  RETURNS
*
*  The routine glp_get_col_kind returns the kind of j-th column, i.e.
*  the kind of corresponding structural variable, as follows:
*
*  GLP_CV - continuous variable;
*  GLP_IV - integer variable;
*  GLP_BV - binary variable */

int glp_get_col_kind(glp_prob *mip, int j)
{     GLPCOL *col;
      int kind;
      if (!(1 <= j && j <= mip->n))
         xfault("glp_get_col_kind: j = %d; column number out of range\n"
            , j);
      col = mip->col[j];
      kind = col->kind;
      switch (kind)
      {  case GLP_CV:
            break;
         case GLP_IV:
            if (col->type == GLP_DB && col->lb == 0.0 && col->ub == 1.0)
               kind = GLP_BV;
            break;
         default:
            xassert(kind != kind);
      }
      return kind;
}

/***********************************************************************
*  NAME
*
*  glp_get_num_int - retrieve number of integer columns
*
*  SYNOPSIS
*
*  int glp_get_num_int(glp_prob *mip);
*
*  RETURNS
*
*  The routine glp_get_num_int returns the current number of columns,
*  which are marked as integer. */

int glp_get_num_int(glp_prob *mip)
{     GLPCOL *col;
      int j, count = 0;
      for (j = 1; j <= mip->n; j++)
      {  col = mip->col[j];
         if (col->kind == GLP_IV) count++;
      }
      return count;
}

/***********************************************************************
*  NAME
*
*  glp_get_num_bin - retrieve number of binary columns
*
*  SYNOPSIS
*
*  int glp_get_num_bin(glp_prob *mip);
*
*  RETURNS
*
*  The routine glp_get_num_bin returns the current number of columns,
*  which are marked as binary. */

int glp_get_num_bin(glp_prob *mip)
{     GLPCOL *col;
      int j, count = 0;
      for (j = 1; j <= mip->n; j++)
      {  col = mip->col[j];
         if (col->kind == GLP_IV && col->type == GLP_DB && col->lb ==
            0.0 && col->ub == 1.0) count++;
      }
      return count;
}

/***********************************************************************
*  NAME
*
*  glp_intopt - solve MIP problem with the branch-and-bound method
*
*  SYNOPSIS
*
*  int glp_intopt(glp_prob *mip, const glp_iocp *parm);
*
*  DESCRIPTION
*
*  The routine glp_intopt is a driver to the MIP solver based on the
*  branch-and-bound method.
*
*  On entry the problem object should contain optimal solution to LP
*  relaxation (which can be obtained with the routine glp_simplex).
*
*  The MIP solver has a set of control parameters. Values of the control
*  parameters can be passed in a structure glp_iocp, which the parameter
*  parm points to.
*
*  The parameter parm can be specified as NULL, in which case the MIP
*  solver uses default settings.
*
*  RETURNS
*
*  0  The MIP problem instance has been successfully solved. This code
*     does not necessarily mean that the solver has found optimal
*     solution. It only means that the solution process was successful.
*
*  GLP_EBOUND
*     Unable to start the search, because some double-bounded variables
*     have incorrect bounds or some integer variables have non-integer
*     (fractional) bounds.
*
*  GLP_EROOT
*     Unable to start the search, because optimal basis for initial LP
*     relaxation is not provided.
*
*  GLP_EFAIL
*     The search was prematurely terminated due to the solver failure.
*
*  GLP_ETMLIM
*     The search was prematurely terminated, because the time limit has
*     been exceeded.
*
*  GLP_ESTOP
*     The search was prematurely terminated by application. */

int glp_intopt(glp_prob *mip, const glp_iocp *parm)
{     glp_iocp _parm;
      int m = mip->m;
      int n = mip->n;
      glp_tree *tree;
      int i, j, ret;
      if (mip->tree != NULL)
         xfault("glp_intopt: problem object is already used by the MIP "
            "solver\n");
      if (parm == NULL)
         glp_init_iocp(&_parm);
      else
         memcpy(&_parm, parm, sizeof(glp_iocp));
      parm = &_parm;
      /* check control parameters */
      if (!(parm->msg_lev == GLP_MSG_OFF ||
            parm->msg_lev == GLP_MSG_ERR ||
            parm->msg_lev == GLP_MSG_ON  ||
            parm->msg_lev == GLP_MSG_ALL ||
            parm->msg_lev == GLP_MSG_DBG))
         xfault("glp_intopt: msg_lev = %d; invalid parameter\n",
            parm->msg_lev);
      if (!(parm->br_tech == GLP_BR_FFV ||
            parm->br_tech == GLP_BR_LFV ||
            parm->br_tech == GLP_BR_MFV ||
            parm->br_tech == GLP_BR_DTH))
         xfault("glp_intopt: br_tech = %d; invalid parameter\n",
            parm->br_tech);
      if (!(parm->bt_tech == GLP_BT_DFS ||
            parm->bt_tech == GLP_BT_BFS ||
            parm->bt_tech == GLP_BT_BLB ||
            parm->bt_tech == GLP_BT_BPH))
         xfault("glp_intopt: bt_tech = %d; invalid parameter\n",
            parm->bt_tech);
      if (!(0.0 < parm->tol_int && parm->tol_int < 1.0))
         xfault("glp_intopt: tol_int = %g; invalid parameter\n",
            parm->tol_int);
      if (!(0.0 < parm->tol_obj && parm->tol_obj < 1.0))
         xfault("glp_intopt: tol_obj = %g; invalid parameter\n",
            parm->tol_obj);
      if (parm->tm_lim < 0)
         xfault("glp_intopt: tm_lim = %d; invalid parameter\n",
            parm->tm_lim);
      if (parm->out_frq < 0)
         xfault("glp_intopt: out_frq = %d; invalid parameter\n",
            parm->out_frq);
      if (parm->out_dly < 0)
         xfault("glp_intopt: out_dly = %d; invalid parameter\n",
            parm->out_dly);
      if (!(0 <= parm->cb_size && parm->cb_size <= 256))
         xfault("glp_intopt: cb_size = %d; invalid parameter\n",
            parm->cb_size);
      if (!(parm->pp_tech == GLP_PP_NONE ||
            parm->pp_tech == GLP_PP_ROOT ||
            parm->pp_tech == GLP_PP_ALL))
         xfault("glp_intopt: pp_tech = %d; invalid parameter\n",
            parm->pp_tech);
      if (parm->mip_gap < 0.0)
         xfault("glp_intopt: mip_gap = %g; invalid parameter\n",
            parm->mip_gap);
      if (!(parm->mir_cuts == GLP_ON || parm->mir_cuts == GLP_OFF))
         xfault("glp_intopt: mir_cuts = %d; invalid parameter\n",
            parm->mir_cuts);
      if (!(parm->gmi_cuts == GLP_ON || parm->gmi_cuts == GLP_OFF))
         xfault("glp_intopt: gmi_cuts = %d; invalid parameter\n",
            parm->gmi_cuts);
      /* integer solution is currently undefined */
      mip->mip_stat = GLP_UNDEF;
      mip->mip_obj = 0.0;
      /* check bounds of double-bounded variables */
      for (i = 1; i <= m; i++)
      {  GLPROW *row = mip->row[i];
         if (row->type == GLP_DB && row->lb >= row->ub)
         {  if (parm->msg_lev >= GLP_MSG_ERR)
               xprintf("glp_intopt: row %d: lb = %g, ub = %g; incorrect"
                  " bounds\n", i, row->lb, row->ub);
            ret = GLP_EBOUND;
            goto done;
         }
      }
      for (j = 1; j <= n; j++)
      {  GLPCOL *col = mip->col[j];
         if (col->type == GLP_DB && col->lb >= col->ub)
         {  if (parm->msg_lev >= GLP_MSG_ERR)
               xprintf("glp_intopt: column %d: lb = %g, ub = %g; incorr"
                  "ect bounds\n", j, col->lb, col->ub);
            ret = GLP_EBOUND;
            goto done;
         }
      }
      /* bounds of all integer variables must be integral */
      for (j = 1; j <= n; j++)
      {  GLPCOL *col = mip->col[j];
         if (col->kind != GLP_IV) continue;
         if (col->type == GLP_LO || col->type == GLP_DB)
         {  if (col->lb != floor(col->lb))
            {  if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_intopt: integer column %d has non-intege"
                     "r lower bound %g\n", j, col->lb);
               ret = GLP_EBOUND;
               goto done;
            }
         }
         if (col->type == GLP_UP || col->type == GLP_DB)
         {  if (col->ub != floor(col->ub))
            {  if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_intopt: integer column %d has non-intege"
                     "r upper bound %g\n", j, col->ub);
               ret = GLP_EBOUND;
               goto done;
            }
         }
         if (col->type == GLP_FX)
         {  if (col->lb != floor(col->lb))
            {  if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_intopt: integer column %d has non-intege"
                     "r fixed value %g\n", j, col->lb);
               ret = GLP_EBOUND;
               goto done;
            }
         }
      }
      /* optimal solution to LP relaxation must be known */
      if (glp_get_status(mip) != GLP_OPT)
      {  if (parm->msg_lev >= GLP_MSG_ERR)
            xprintf("glp_intopt: optimal basis to initial LP relaxation"
               " not provided\n");
         ret = GLP_EROOT;
         goto done;
      }
      /* it seems all is ok */
      if (parm->msg_lev >= GLP_MSG_ALL)
         xprintf("Integer optimization begins...\n");
      /* create the branch-and-bound tree */
      tree = ios_create_tree(mip, parm);
#if 0
      if (parm->msg_lev >= GLP_MSG_ALL && tree->int_obj)
         xprintf("Objective function is integral\n");
#endif
      /* try to solve the problem */
      ret = ios_driver(tree);
      /* analyze exit code reported by the mip driver */
      switch (ret)
      {  case 0:
            if (tree->mip->mip_stat == GLP_FEAS)
            {  if (parm->msg_lev >= GLP_MSG_ALL)
                  xprintf("INTEGER OPTIMAL SOLUTION FOUND\n");
               tree->mip->mip_stat = GLP_OPT;
            }
            else
            {  if (parm->msg_lev >= GLP_MSG_ALL)
                  xprintf("PROBLEM HAS NO INTEGER FEASIBLE SOLUTION\n");
               tree->mip->mip_stat = GLP_NOFEAS;
            }
            break;
         case GLP_ETMLIM:
            if (parm->msg_lev >= GLP_MSG_ALL)
               xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
            break;
         case GLP_EFAIL:
            if (parm->msg_lev >= GLP_MSG_ERR)
               xprintf("glp_intopt: cannot solve current LP relaxation "
                  "\n");
            break;
         case GLP_ESTOP:
            if (parm->msg_lev >= GLP_MSG_ALL)
               xprintf("SEARCH TERMINATED BY APPLICATION\n");
            break;
         default:
            xassert(ret != ret);
      }
      /* delete the branch-and-bound tree */
      ios_delete_tree(tree);
done: /* return to the application program */
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_init_iocp - initialize integer optimizer control parameters
*
*  SYNOPSIS
*
*  void glp_init_iocp(glp_iocp *parm);
*
*  DESCRIPTION
*
*  The routine glp_init_iocp initializes control parameters, which are
*  used by the integer optimizer, with default values.
*
*  Default values of the control parameters are stored in a glp_iocp
*  structure, which the parameter parm points to. */

void glp_init_iocp(glp_iocp *parm)
{     parm->msg_lev = GLP_MSG_ALL;
      parm->br_tech = GLP_BR_DTH;
      parm->bt_tech = GLP_BT_BLB;
      parm->tol_int = 1e-5;
      parm->tol_obj = 1e-7;
      parm->tm_lim = INT_MAX;
      parm->out_frq = 5000;
      parm->out_dly = 10000;
      parm->cb_func = NULL;
      parm->cb_info = NULL;
      parm->cb_size = 0;
      parm->pp_tech = GLP_PP_ALL;
      parm->mip_gap = 0.0;
      parm->mir_cuts = GLP_OFF;
      parm->gmi_cuts = GLP_OFF;
      parm->fn_sol = NULL;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_mip_status - retrieve status of MIP solution
*
*  SYNOPSIS
*
*  int glp_mip_status(glp_prob *mip);
*
*  RETURNS
*
*  The routine lpx_mip_status reports the status of MIP solution found
*  by the branch-and-bound solver as follows:
*
*  GLP_UNDEF  - MIP solution is undefined;
*  GLP_OPT    - MIP solution is integer optimal;
*  GLP_FEAS   - MIP solution is integer feasible but its optimality
*               (or non-optimality) has not been proven, perhaps due to
*               premature termination of the search;
*  GLP_NOFEAS - problem has no integer feasible solution (proven by the
*               solver). */

int glp_mip_status(glp_prob *mip)
{     int mip_stat = mip->mip_stat;
      return mip_stat;
}

/***********************************************************************
*  NAME
*
*  glp_mip_obj_val - retrieve objective value (MIP solution)
*
*  SYNOPSIS
*
*  double glp_mip_obj_val(glp_prob *mip);
*
*  RETURNS
*
*  The routine glp_mip_obj_val returns value of the objective function
*  for MIP solution. */

double glp_mip_obj_val(glp_prob *mip)
{     struct LPXCPS *cps = mip->cps;
      double z;
      z = mip->mip_obj;
      if (cps->round && fabs(z) < 1e-9) z = 0.0;
      return z;
}

/***********************************************************************
*  NAME
*
*  glp_mip_row_val - retrieve row value (MIP solution)
*
*  SYNOPSIS
*
*  double glp_mip_row_val(glp_prob *mip, int i);
*
*  RETURNS
*
*  The routine glp_mip_row_val returns value of the auxiliary variable
*  associated with i-th row. */

double glp_mip_row_val(glp_prob *mip, int i)
{     struct LPXCPS *cps = mip->cps;
      double mipx;
      if (!(1 <= i && i <= mip->m))
         xfault("glp_mip_row_val: i = %d; row number out of range\n", i)
            ;
      mipx = mip->row[i]->mipx;
      if (cps->round && fabs(mipx) < 1e-9) mipx = 0.0;
      return mipx;
}

/***********************************************************************
*  NAME
*
*  glp_mip_col_val - retrieve column value (MIP solution)
*
*  SYNOPSIS
*
*  double glp_mip_col_val(glp_prob *mip, int j);
*
*  RETURNS
*
*  The routine glp_mip_col_val returns value of the structural variable
*  associated with j-th column. */

double glp_mip_col_val(glp_prob *mip, int j)
{     struct LPXCPS *cps = mip->cps;
      double mipx;
      if (!(1 <= j && j <= mip->n))
         xfault("glp_mip_col_val: j = %d; column number out of range\n",
            j);
      mipx = mip->col[j]->mipx;
      if (cps->round && fabs(mipx) < 1e-9) mipx = 0.0;
      return mipx;
}

/* eof */
