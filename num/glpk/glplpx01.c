/* glplpx01.c (obsolete api routines) */

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
#include "glplib.h"
#define xfault xerror

LPX *lpx_create_prob(void)
{     /* create problem object */
      return glp_create_prob();
}

void lpx_set_prob_name(LPX *lp, const char *name)
{     /* assign (change) problem name */
      glp_set_prob_name(lp, name);
      return;
}

void lpx_set_obj_name(LPX *lp, const char *name)
{     /* assign (change) objective function name */
      glp_set_obj_name(lp, name);
      return;
}

void lpx_set_obj_dir(LPX *lp, int dir)
{     /* set (change) optimization direction flag */
      glp_set_obj_dir(lp, dir - LPX_MIN + GLP_MIN);
      return;
}

int lpx_add_rows(LPX *lp, int nrs)
{     /* add new rows to problem object */
      return glp_add_rows(lp, nrs);
}

int lpx_add_cols(LPX *lp, int ncs)
{     /* add new columns to problem object */
      return glp_add_cols(lp, ncs);
}

void lpx_set_row_name(LPX *lp, int i, const char *name)
{     /* assign (change) row name */
      glp_set_row_name(lp, i, name);
      return;
}

void lpx_set_col_name(LPX *lp, int j, const char *name)
{     /* assign (change) column name */
      glp_set_col_name(lp, j, name);
      return;
}

void lpx_set_row_bnds(LPX *lp, int i, int type, double lb, double ub)
{     /* set (change) row bounds */
      glp_set_row_bnds(lp, i, type - LPX_FR + GLP_FR, lb, ub);
      return;
}

void lpx_set_col_bnds(LPX *lp, int j, int type, double lb, double ub)
{     /* set (change) column bounds */
      glp_set_col_bnds(lp, j, type - LPX_FR + GLP_FR, lb, ub);
      return;
}

void lpx_set_obj_coef(glp_prob *lp, int j, double coef)
{     /* set (change) obj. coefficient or constant term */
      glp_set_obj_coef(lp, j, coef);
      return;
}

void lpx_set_mat_row(LPX *lp, int i, int len, const int ind[],
      const double val[])
{     /* set (replace) row of the constraint matrix */
      glp_set_mat_row(lp, i, len, ind, val);
      return;
}

void lpx_set_mat_col(LPX *lp, int j, int len, const int ind[],
      const double val[])
{     /* set (replace) column of the constraint matrix */
      glp_set_mat_col(lp, j, len, ind, val);
      return;
}

void lpx_load_matrix(LPX *lp, int ne, const int ia[], const int ja[],
      const double ar[])
{     /* load (replace) the whole constraint matrix */
      glp_load_matrix(lp, ne, ia, ja, ar);
      return;
}

void lpx_del_rows(LPX *lp, int nrs, const int num[])
{     /* delete specified rows from problem object */
      glp_del_rows(lp, nrs, num);
      return;
}

void lpx_del_cols(LPX *lp, int ncs, const int num[])
{     /* delete specified columns from problem object */
      glp_del_cols(lp, ncs, num);
      return;
}

void lpx_delete_prob(LPX *lp)
{     /* delete problem object */
      glp_delete_prob(lp);
      return;
}

const char *lpx_get_prob_name(LPX *lp)
{     /* retrieve problem name */
      return glp_get_prob_name(lp);
}

const char *lpx_get_obj_name(LPX *lp)
{     /* retrieve objective function name */
      return glp_get_obj_name(lp);
}

int lpx_get_obj_dir(LPX *lp)
{     /* retrieve optimization direction flag */
      return glp_get_obj_dir(lp) - GLP_MIN + LPX_MIN;
}

int lpx_get_num_rows(LPX *lp)
{     /* retrieve number of rows */
      return glp_get_num_rows(lp);
}

int lpx_get_num_cols(LPX *lp)
{     /* retrieve number of columns */
      return glp_get_num_cols(lp);
}

const char *lpx_get_row_name(LPX *lp, int i)
{     /* retrieve row name */
      return glp_get_row_name(lp, i);
}

const char *lpx_get_col_name(LPX *lp, int j)
{     /* retrieve column name */
      return glp_get_col_name(lp, j);
}

int lpx_get_row_type(LPX *lp, int i)
{     /* retrieve row type */
      return glp_get_row_type(lp, i) - GLP_FR + LPX_FR;
}

double lpx_get_row_lb(glp_prob *lp, int i)
{     /* retrieve row lower bound */
      double lb;
      lb = glp_get_row_lb(lp, i);
      if (lb == -DBL_MAX) lb = 0.0;
      return lb;
}

double lpx_get_row_ub(glp_prob *lp, int i)
{     /* retrieve row upper bound */
      double ub;
      ub = glp_get_row_ub(lp, i);
      if (ub == +DBL_MAX) ub = 0.0;
      return ub;
}

void lpx_get_row_bnds(glp_prob *lp, int i, int *typx, double *lb,
      double *ub)
{     /* retrieve row bounds */
      if (typx != NULL) *typx = lpx_get_row_type(lp, i);
      if (lb != NULL) *lb = lpx_get_row_lb(lp, i);
      if (ub != NULL) *ub = lpx_get_row_ub(lp, i);
      return;
}

int lpx_get_col_type(LPX *lp, int j)
{     /* retrieve column type */
      return glp_get_col_type(lp, j) - GLP_FR + LPX_FR;
}

double lpx_get_col_lb(glp_prob *lp, int j)
{     /* retrieve column lower bound */
      double lb;
      lb = glp_get_col_lb(lp, j);
      if (lb == -DBL_MAX) lb = 0.0;
      return lb;
}

double lpx_get_col_ub(glp_prob *lp, int j)
{     /* retrieve column upper bound */
      double ub;
      ub = glp_get_col_ub(lp, j);
      if (ub == +DBL_MAX) ub = 0.0;
      return ub;
}

void lpx_get_col_bnds(glp_prob *lp, int j, int *typx, double *lb,
      double *ub)
{     /* retrieve column bounds */
      if (typx != NULL) *typx = lpx_get_col_type(lp, j);
      if (lb != NULL) *lb = lpx_get_col_lb(lp, j);
      if (ub != NULL) *ub = lpx_get_col_ub(lp, j);
      return;
}

double lpx_get_obj_coef(LPX *lp, int j)
{     /* retrieve obj. coefficient or constant term */
      return glp_get_obj_coef(lp, j);
}

int lpx_get_num_nz(LPX *lp)
{     /* retrieve number of constraint coefficients */
      return glp_get_num_nz(lp);
}

int lpx_get_mat_row(LPX *lp, int i, int ind[], double val[])
{     /* retrieve row of the constraint matrix */
      return glp_get_mat_row(lp, i, ind, val);
}

int lpx_get_mat_col(LPX *lp, int j, int ind[], double val[])
{     /* retrieve column of the constraint matrix */
      return glp_get_mat_col(lp, j, ind, val);
}

void lpx_create_index(LPX *lp)
{     /* create the name index */
      glp_create_index(lp);
      return;
}

int lpx_find_row(LPX *lp, const char *name)
{     /* find row by its name */
      return glp_find_row(lp, name);
}

int lpx_find_col(LPX *lp, const char *name)
{     /* find column by its name */
      return glp_find_col(lp, name);
}

void lpx_delete_index(LPX *lp)
{     /* delete the name index */
      glp_delete_index(lp);
      return;
}

void lpx_unscale_prob(LPX *lp)
{     /* unscale problem data */
      glp_unscale_prob(lp);
      return;
}

void lpx_set_row_stat(LPX *lp, int i, int stat)
{     /* set (change) row status */
      glp_set_row_stat(lp, i, stat - LPX_BS + GLP_BS);
      return;
}

void lpx_set_col_stat(LPX *lp, int j, int stat)
{     /* set (change) column status */
      glp_set_col_stat(lp, j, stat - LPX_BS + GLP_BS);
      return;
}

int lpx_simplex(LPX *lp)
{     /* easy-to-use driver to the simplex method */
      glp_smcp parm;
      int ret;
      glp_init_smcp(&parm);
      switch (lpx_get_int_parm(lp, LPX_K_MSGLEV))
      {  case 0:  parm.msg_lev = GLP_MSG_OFF;   break;
         case 1:  parm.msg_lev = GLP_MSG_ERR;   break;
         case 2:  parm.msg_lev = GLP_MSG_ON;    break;
         case 3:  parm.msg_lev = GLP_MSG_ALL;   break;
         default: xassert(lp != lp);
      }
      switch (lpx_get_int_parm(lp, LPX_K_DUAL))
      {  case 0:  parm.meth = GLP_PRIMAL;       break;
         case 1:  parm.meth = GLP_DUALP;        break;
         default: xassert(lp != lp);
      }
      switch (lpx_get_int_parm(lp, LPX_K_PRICE))
      {  case 0:  parm.pricing = GLP_PT_STD;    break;
         case 1:  parm.pricing = GLP_PT_PSE;    break;
         default: xassert(lp != lp);
      }
      if (lpx_get_real_parm(lp, LPX_K_RELAX) == 0.0)
         parm.r_test = GLP_RT_STD;
      else
         parm.r_test = GLP_RT_HAR;
      parm.tol_bnd = lpx_get_real_parm(lp, LPX_K_TOLBND);
      parm.tol_dj  = lpx_get_real_parm(lp, LPX_K_TOLDJ);
      parm.tol_piv = lpx_get_real_parm(lp, LPX_K_TOLPIV);
      parm.obj_ll  = lpx_get_real_parm(lp, LPX_K_OBJLL);
      parm.obj_ul  = lpx_get_real_parm(lp, LPX_K_OBJUL);
      if (lpx_get_int_parm(lp, LPX_K_ITLIM) < 0)
         parm.it_lim = INT_MAX;
      else
         parm.it_lim = lpx_get_int_parm(lp, LPX_K_ITLIM);
      if (lpx_get_real_parm(lp, LPX_K_TMLIM) < 0.0)
         parm.tm_lim = INT_MAX;
      else
         parm.tm_lim  = 1000.0 * lpx_get_real_parm(lp, LPX_K_TMLIM);
      parm.out_frq = lpx_get_int_parm(lp, LPX_K_OUTFRQ);
      parm.out_dly = 1000.0 * lpx_get_real_parm(lp, LPX_K_OUTDLY);
      switch (lpx_get_int_parm(lp, LPX_K_PRESOL))
      {  case 0:  parm.presolve = GLP_OFF;      break;
         case 1:  parm.presolve = GLP_ON;       break;
         default: xassert(lp != lp);
      }
      ret = glp_simplex(lp, &parm);
      switch (ret)
      {  case 0:           ret = LPX_E_OK;      break;
         case GLP_EBADB:
         case GLP_ESING:
         case GLP_ECOND:
         case GLP_EBOUND:  ret = LPX_E_FAULT;   break;
         case GLP_EFAIL:   ret = LPX_E_SING;    break;
         case GLP_EOBJLL:  ret = LPX_E_OBJLL;   break;
         case GLP_EOBJUL:  ret = LPX_E_OBJUL;   break;
         case GLP_EITLIM:  ret = LPX_E_ITLIM;   break;
         case GLP_ETMLIM:  ret = LPX_E_TMLIM;   break;
         case GLP_ENOPFS:  ret = LPX_E_NOPFS;   break;
         case GLP_ENODFS:  ret = LPX_E_NODFS;   break;
         default:          xassert(ret != ret);
      }
      return ret;
}

int lpx_get_status(glp_prob *lp)
{     /* retrieve generic status of basic solution */
      int status;
      switch (glp_get_status(lp))
      {  case GLP_OPT:    status = LPX_OPT;    break;
         case GLP_FEAS:   status = LPX_FEAS;   break;
         case GLP_INFEAS: status = LPX_INFEAS; break;
         case GLP_NOFEAS: status = LPX_NOFEAS; break;
         case GLP_UNBND:  status = LPX_UNBND;  break;
         case GLP_UNDEF:  status = LPX_UNDEF;  break;
         default:         xassert(lp != lp);
      }
      return status;
}

int lpx_get_prim_stat(glp_prob *lp)
{     /* retrieve status of primal basic solution */
      return glp_get_prim_stat(lp) - GLP_UNDEF + LPX_P_UNDEF;
}

int lpx_get_dual_stat(glp_prob *lp)
{     /* retrieve status of dual basic solution */
      return glp_get_dual_stat(lp) - GLP_UNDEF + LPX_D_UNDEF;
}

double lpx_get_obj_val(LPX *lp)
{     /* retrieve objective value (basic solution) */
      return glp_get_obj_val(lp);
}

int lpx_get_row_stat(LPX *lp, int i)
{     /* retrieve row status (basic solution) */
      return glp_get_row_stat(lp, i) - GLP_BS + LPX_BS;
}

double lpx_get_row_prim(LPX *lp, int i)
{     /* retrieve row primal value (basic solution) */
      return glp_get_row_prim(lp, i);
}

double lpx_get_row_dual(LPX *lp, int i)
{     /* retrieve row dual value (basic solution) */
      return glp_get_row_dual(lp, i);
}

void lpx_get_row_info(glp_prob *lp, int i, int *tagx, double *vx,
      double *dx)
{     /* obtain row solution information */
      if (tagx != NULL) *tagx = lpx_get_row_stat(lp, i);
      if (vx != NULL) *vx = lpx_get_row_prim(lp, i);
      if (dx != NULL) *dx = lpx_get_row_dual(lp, i);
      return;
}

int lpx_get_col_stat(LPX *lp, int j)
{     /* retrieve column status (basic solution) */
      return glp_get_col_stat(lp, j) - GLP_BS + LPX_BS;
}

double lpx_get_col_prim(LPX *lp, int j)
{     /* retrieve column primal value (basic solution) */
      return glp_get_col_prim(lp, j);
}

double lpx_get_col_dual(glp_prob *lp, int j)
{     /* retrieve column dual value (basic solution) */
      return glp_get_col_dual(lp, j);
}

void lpx_get_col_info(glp_prob *lp, int j, int *tagx, double *vx,
      double *dx)
{     /* obtain column solution information */
      if (tagx != NULL) *tagx = lpx_get_col_stat(lp, j);
      if (vx != NULL) *vx = lpx_get_col_prim(lp, j);
      if (dx != NULL) *dx = lpx_get_col_dual(lp, j);
      return;
}

int lpx_eval_tab_row(LPX *lp, int k, int ind[], double val[])
{     /* compute row of the simplex tableau */
      return glp_eval_tab_row(lp, k, ind, val);
}

int lpx_eval_tab_col(LPX *lp, int k, int ind[], double val[])
{     /* compute column of the simplex tableau */
      return glp_eval_tab_col(lp, k, ind, val);
}

int lpx_ipt_status(glp_prob *lp)
{     /* retrieve status of interior-point solution */
      int status;
      switch (glp_ipt_status(lp))
      {  case GLP_UNDEF:  status = LPX_T_UNDEF;  break;
         case GLP_OPT:    status = LPX_T_OPT;    break;
         default:         xassert(lp != lp);
      }
      return status;
}

double lpx_ipt_obj_val(LPX *lp)
{     /* retrieve objective value (interior point) */
      return glp_ipt_obj_val(lp);
}

double lpx_ipt_row_prim(LPX *lp, int i)
{     /* retrieve row primal value (interior point) */
      return glp_ipt_row_prim(lp, i);
}

double lpx_ipt_row_dual(LPX *lp, int i)
{     /* retrieve row dual value (interior point) */
      return glp_ipt_row_dual(lp, i);
}

double lpx_ipt_col_prim(LPX *lp, int j)
{     /* retrieve column primal value (interior point) */
      return glp_ipt_col_prim(lp, j);
}

double lpx_ipt_col_dual(LPX *lp, int j)
{     /* retrieve column dual value (interior point) */
      return glp_ipt_col_dual(lp, j);
}

void lpx_set_class(LPX *lp, int klass)
{     /* set problem class */
      xassert(lp == lp);
      if (!(klass == LPX_LP || klass == LPX_MIP))
         xfault("lpx_set_class: invalid problem class\n");
      return;
}

int lpx_get_class(LPX *lp)
{     /* determine problem klass */
      return glp_get_num_int(lp) == 0 ? LPX_LP : LPX_MIP;
}

void lpx_set_col_kind(LPX *lp, int j, int kind)
{     /* set (change) column kind */
      glp_set_col_kind(lp, j, kind - LPX_CV + GLP_CV);
      return;
}

int lpx_get_col_kind(LPX *lp, int j)
{     /* retrieve column kind */
      return glp_get_col_kind(lp, j) == GLP_CV ? LPX_CV : LPX_IV;
}

int lpx_get_num_int(LPX *lp)
{     /* retrieve number of integer columns */
      return glp_get_num_int(lp);
}

int lpx_get_num_bin(LPX *lp)
{     /* retrieve number of binary columns */
      return glp_get_num_bin(lp);
}

int lpx_integer(LPX *lp)
{     /* easy-to-use driver to the branch-and-bound method */
      glp_iocp parm;
      int ret;
      glp_init_iocp(&parm);
      switch (lpx_get_int_parm(lp, LPX_K_MSGLEV))
      {  case 0:  parm.msg_lev = GLP_MSG_OFF;   break;
         case 1:  parm.msg_lev = GLP_MSG_ERR;   break;
         case 2:  parm.msg_lev = GLP_MSG_ON;    break;
         case 3:  parm.msg_lev = GLP_MSG_ALL;   break;
         default: xassert(lp != lp);
      }
      switch (lpx_get_int_parm(lp, LPX_K_BRANCH))
      {  case 0:  parm.br_tech = GLP_BR_FFV;    break;
         case 1:  parm.br_tech = GLP_BR_LFV;    break;
         case 2:  parm.br_tech = GLP_BR_DTH;    break;
         case 3:  parm.br_tech = GLP_BR_MFV;    break;
         default: xassert(lp != lp);
      }
      switch (lpx_get_int_parm(lp, LPX_K_BTRACK))
      {  case 0:  parm.bt_tech = GLP_BT_DFS;    break;
         case 1:  parm.bt_tech = GLP_BT_BFS;    break;
         case 2:  parm.bt_tech = GLP_BT_BPH;    break;
         case 3:  parm.bt_tech = GLP_BT_BLB;    break;
         default: xassert(lp != lp);
      }
      parm.tol_int = lpx_get_real_parm(lp, LPX_K_TOLINT);
      parm.tol_obj = lpx_get_real_parm(lp, LPX_K_TOLOBJ);
      if (lpx_get_real_parm(lp, LPX_K_TMLIM) < 0.0)
         parm.tm_lim = INT_MAX;
      else
         parm.tm_lim = 1000.0 * lpx_get_real_parm(lp, LPX_K_TMLIM);
      if (lpx_get_int_parm(lp, LPX_K_USECUTS) & LPX_C_GOMORY)
         parm.gmi_cuts = GLP_ON;
      else
         parm.gmi_cuts = GLP_OFF;
      if (lpx_get_int_parm(lp, LPX_K_USECUTS) & LPX_C_MIR)
         parm.mir_cuts = GLP_ON;
      else
         parm.mir_cuts = GLP_OFF;
      parm.mip_gap = lpx_get_real_parm(lp, LPX_K_MIPGAP);
      ret = glp_intopt(lp, &parm);
      switch (ret)
      {  case 0:           ret = LPX_E_OK;      break;
         case GLP_EBOUND:
         case GLP_EROOT:   ret = LPX_E_FAULT;   break;
         case GLP_EFAIL:   ret = LPX_E_SING;    break;
         case GLP_ETMLIM:  ret = LPX_E_TMLIM;   break;
         default:          xassert(ret != ret);
      }
      return ret;
}

int lpx_mip_status(glp_prob *lp)
{     /* retrieve status of MIP solution */
      int status;
      switch (glp_mip_status(lp))
      {  case GLP_UNDEF:  status = LPX_I_UNDEF;  break;
         case GLP_OPT:    status = LPX_I_OPT;    break;
         case GLP_FEAS:   status = LPX_I_FEAS;   break;
         case GLP_NOFEAS: status = LPX_I_NOFEAS; break;
         default:         xassert(lp != lp);
      }
      return status;
}

double lpx_mip_obj_val(LPX *lp)
{     /* retrieve objective value (MIP solution) */
      return glp_mip_obj_val(lp);
}

double lpx_mip_row_val(LPX *lp, int i)
{     /* retrieve row value (MIP solution) */
      return glp_mip_row_val(lp, i);
}

double lpx_mip_col_val(LPX *lp, int j)
{     /* retrieve column value (MIP solution) */
      return glp_mip_col_val(lp, j);
}

int lpx_is_b_avail(glp_prob *lp)
{     /* check if LP basis is available */
      return glp_bf_exists(lp);
}

/* eof */
