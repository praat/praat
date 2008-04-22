/* glpipp01.c */

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

#include "glpipp.h"
#define dmp_create_poolx(size) dmp_create_pool()
#define dmp_get_atomv dmp_get_atom

/*----------------------------------------------------------------------
-- ipp_create_wksp - create MIP presolver workspace.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- IPP *ipp_create_wksp(void);
--
-- DESCRIPTION
--
-- The routine ipp_create_wksp creates an empty workspace used by the
-- MIP presolver routines.
--
-- RETURNS
--
-- The routine returns a pointer to the MIP workspace created. */

IPP *ipp_create_wksp(void)
{     IPP *ipp;
      ipp = xmalloc(sizeof(IPP));
      ipp->orig_m = 0;
      ipp->orig_n = 0;
      ipp->orig_nnz = 0;
      ipp->orig_dir = LPX_MIN;
      ipp->ncols = 0;
      ipp->row_pool = dmp_create_poolx(sizeof(IPPROW));
      ipp->col_pool = dmp_create_poolx(sizeof(IPPCOL));
      ipp->aij_pool = dmp_create_poolx(sizeof(IPPAIJ));
      ipp->row_ptr = NULL;
      ipp->col_ptr = NULL;
      ipp->row_que = NULL;
      ipp->col_que = NULL;
      ipp->c0 = 0.0;
      ipp->tqe_pool = dmp_create_poolx(0);
      ipp->tqe_list = NULL;
      ipp->col_stat = NULL;
      ipp->col_mipx = NULL;
      return ipp;
}

/*----------------------------------------------------------------------
-- ipp_add_row - add new row to the transformed problem.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- IPPROW *ipp_add_row(IPP *ipp, double lb, double ub);
--
-- DESCRIPTION
--
-- The routine ipp_add_row adds a new empty row to the transformed
-- problem.
--
-- The parameter lb is an lower bound of the new row (-DBL_MAX means
-- the row has no lower bound).
--
-- The parameter ub is an upper bound of the new row (+DBL_MAX means
-- the row has no upper bound).
--
-- RETURNS
--
-- The routine returns a pointer to the created row. */

IPPROW *ipp_add_row(IPP *ipp, double lb, double ub)
{     IPPROW *row;
      /* perform sanity checks */
      xassert(lb <= ub);
      /* create new row */
      row = dmp_get_atom(ipp->row_pool, sizeof(IPPROW));
      row->lb = lb;
      row->ub = ub;
      row->ptr = NULL;
      row->temp = 0;
      row->prev = NULL;
      row->next = ipp->row_ptr;
      row->q_flag = 0;
      row->q_prev = NULL;
      row->q_next = NULL;
      /* add the row to the linked list of rows */
      if (row->next != NULL) row->next->prev = row;
      ipp->row_ptr = row;
      return row;
}

/*----------------------------------------------------------------------
-- ipp_add_col - add new column to the transformed problem.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- IPPCOL *ipp_add_col(IPP *ipp, int i_flag, double lb, double ub,
--    double c);
--
-- DESCRIPTION
--
-- The routine ipp_add_col adds a new empty column to the transformed
-- problem.
--
-- The parameter i_flag is a column integrality flag. If it is set, the
-- new column is integer, otherwise the new column is continuous.
--
-- The parameter lb is an lower bound of the new column (-DBL_MAX means
-- the column has no lower bound).
--
-- The parameter ub is an upper bound of the new column (+DBL_MAX means
-- the column has no upper bound).
--
-- The parameter c is an objective coefficient at the new column.
--
-- RETURNS
--
-- The routine returns a pointer to the created column. */

IPPCOL *ipp_add_col(IPP *ipp, int i_flag, double lb, double ub,
      double c)
{     IPPCOL *col;
      /* perform sanity checks */
      xassert(lb <= ub);
      if (i_flag)
      {  if (lb != -DBL_MAX) xassert(lb == floor(lb));
         if (ub != +DBL_MAX) xassert(ub == floor(ub));
      }
      /* create new column */
      col = dmp_get_atom(ipp->col_pool, sizeof(IPPCOL));
      col->j = ++(ipp->ncols);
      col->i_flag = i_flag;
      col->lb = lb;
      col->ub = ub;
      col->c = c;
      col->ptr = NULL;
      col->temp = 0;
      col->prev = NULL;
      col->next = ipp->col_ptr;
      col->q_flag = 0;
      col->q_prev = NULL;
      col->q_next = NULL;
      /* add the column to the linked list of columns */
      if (col->next != NULL) col->next->prev = col;
      ipp->col_ptr = col;
      return col;
}

/*----------------------------------------------------------------------
-- ipp_add_aij - add new element to the constraint matrix.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- IPPAIJ *ipp_add_aij(IPP *ipp, IPPROW *row, IPPCOL *col, double val);
--
-- DESCRIPTION
--
-- The routine ipp_add_aij adds a new element to the constraint matrix
-- of the transformed problem.
--
-- The parameter row is a pointer to a row, in which the new element
-- should be placed.
--
-- The parameter col is a pointer to a column, in which the new element
-- should be placed.
--
-- The parameter val is a numeric value of the new element.
--
-- RETURNS
--
-- The routine returns a pointer to the created element. */

IPPAIJ *ipp_add_aij(IPP *ipp, IPPROW *row, IPPCOL *col, double val)
{     IPPAIJ *aij;
      xassert(val != 0.0);
      aij = dmp_get_atom(ipp->aij_pool, sizeof(IPPAIJ));
      aij->row = row;
      aij->col = col;
      aij->val = val;
      aij->r_prev = NULL;
      aij->r_next = row->ptr;
      aij->c_prev = NULL;
      aij->c_next = col->ptr;
      if (row->ptr != NULL) row->ptr->r_prev = aij;
      if (col->ptr != NULL) col->ptr->c_prev = aij;
      row->ptr = col->ptr = aij;
      return aij;
}

/*----------------------------------------------------------------------
-- ipp_remove_row - remove row from the transformed problem.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_remove_row(IPP *ipp, IPPROW *row);
--
-- DESCRIPTION
--
-- The routine ipp_remove_row removes a row, which the parameter row
-- points to, from the transformed problem. */

void ipp_remove_row(IPP *ipp, IPPROW *row)
{     IPPAIJ *aij;
      /* remove the row from the active queue */
      ipp_deque_row(ipp, row);
      /* remove elements of the row from the constraint matrix */
      while (row->ptr != NULL)
      {  /* get a next element in the row */
         aij = row->ptr;
         /* remove the element from the row list */
         row->ptr = aij->r_next;
         /* remove the element from the column list */
         if (aij->c_prev == NULL)
            aij->col->ptr = aij->c_next;
         else
            aij->c_prev->c_next = aij->c_next;
         if (aij->c_next == NULL)
            ;
         else
            aij->c_next->c_prev = aij->c_prev;
         /* and return it to its pool */
         dmp_free_atom(ipp->aij_pool, aij, sizeof(IPPAIJ));
      }
      /* remove the row from the linked list */
      if (row->prev == NULL)
         ipp->row_ptr = row->next;
      else
         row->prev->next = row->next;
      if (row->next == NULL)
         ;
      else
         row->next->prev = row->prev;
      /* and return the row to its pool */
      dmp_free_atom(ipp->row_pool, row, sizeof(IPPROW));
      return;
}

/*----------------------------------------------------------------------
-- ipp_remove_col - remove column from the transformed problem.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_remove_col(IPP *ipp, IPPCOL *col);
--
-- DESCRIPTION
--
-- The routine ipp_remove_col removes a column, which the parameter col
-- points to, from the transformed problem. */

void ipp_remove_col(IPP *ipp, IPPCOL *col)
{     IPPAIJ *aij;
      /* remove the column from the active queue */
      ipp_deque_col(ipp, col);
      /* remove elements of the column from the constraint matrix */
      while (col->ptr != NULL)
      {  /* get a next element in the column */
         aij = col->ptr;
         /* remove the element from the column list */
         col->ptr = aij->c_next;
         /* remove the element from the row list */
         if (aij->r_prev == NULL)
            aij->row->ptr = aij->r_next;
         else
            aij->r_prev->r_next = aij->r_next;
         if (aij->r_next == NULL)
            ;
         else
            aij->r_next->r_prev = aij->r_prev;
         /* and return it to its pool */
         dmp_free_atom(ipp->aij_pool, aij, sizeof(IPPAIJ));
      }
      /* remove the column from the linked list */
      if (col->prev == NULL)
         ipp->col_ptr = col->next;
      else
         col->prev->next = col->next;
      if (col->next == NULL)
         ;
      else
         col->next->prev = col->prev;
      /* and return the column to its pool */
      dmp_free_atom(ipp->col_pool, col, sizeof(IPPCOL));
      return;
}

/*----------------------------------------------------------------------
-- ipp_enque_row - place row in the active queue.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_enque_row(IPP *ipp, IPPROW *row);
--
-- DESCRIPTION
--
-- The routine ipp_enque_row places the specified row to the queue of
-- active rows. */

void ipp_enque_row(IPP *ipp, IPPROW *row)
{     if (!row->q_flag)
      {  row->q_flag = 1;
         row->q_prev = NULL;
         row->q_next = ipp->row_que;
         if (ipp->row_que != NULL) ipp->row_que->q_prev = row;
         ipp->row_que = row;
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_deque_row - remove row from the active queue.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_deque_row(IPP *ipp, IPPROW *row);
--
-- DESCRIPTION
--
-- The routine ipp_deque_row removes the specified row from the queue
-- of active rows. */

void ipp_deque_row(IPP *ipp, IPPROW *row)
{     if (row->q_flag)
      {  row->q_flag = 0;
         if (row->q_prev == NULL)
            ipp->row_que = row->q_next;
         else
            row->q_prev->q_next = row->q_next;
         if (row->q_next == NULL)
            ;
         else
            row->q_next->q_prev = row->q_prev;
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_enque_col - place column in the active queue.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_enque_col(IPP *ipp, IPPCOL *col);
--
-- DESCRIPTION
--
-- The routine ipp_enque_col places the specified column to the queue of
-- active columns. */

void ipp_enque_col(IPP *ipp, IPPCOL *col)
{     if (!col->q_flag)
      {  col->q_flag = 1;
         col->q_prev = NULL;
         col->q_next = ipp->col_que;
         if (ipp->col_que != NULL) ipp->col_que->q_prev = col;
         ipp->col_que = col;
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_deque_col - remove column from the active queue.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_deque_col(IPP *ipp, IPPCOL *col);
--
-- DESCRIPTION
--
-- The routine ipp_deque_col removes the specified column from the queue
-- of active columns. */

void ipp_deque_col(IPP *ipp, IPPCOL *col)
{     if (col->q_flag)
      {  col->q_flag = 0;
         if (col->q_prev == NULL)
            ipp->col_que = col->q_next;
         else
            col->q_prev->q_next = col->q_next;
         if (col->q_next == NULL)
            ;
         else
            col->q_next->q_prev = col->q_prev;
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_append_tqe - append new transformation queue entry.
--
-- *Synopsis*
--
-- #include "glpipp.h"
-- void *ipp_append_tqe(IPP *ipp, int type, int size);
--
-- *Description*
--
-- The routine ipp_append_tqe appends a new transformation queue entry
-- to the list.
--
-- The parameter type is the entry type.
--
-- The parameter size is the size of a specific part, in bytes.
--
-- *Returns*
--
-- The routine returns a pointer to a specific part, which is allocated
-- and attached to the entry. */

void *ipp_append_tqe(IPP *ipp, int type, int size)
{     IPPTQE *tqe;
      tqe = dmp_get_atomv(ipp->tqe_pool, sizeof(IPPTQE));
      tqe->type = type;
      tqe->info = dmp_get_atomv(ipp->tqe_pool, size);
      tqe->next = ipp->tqe_list;
      ipp->tqe_list = tqe;
      return tqe->info;
}

/*----------------------------------------------------------------------
-- ipp_load_orig - load original problem into MIP presolver workspace.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_load_orig(IPP *ipp, LPX *orig);
--
-- DESCRIPTION
--
-- The routine ipp_load_orig loads an original MIP problem, which the
-- parameter orig points to, into the MIP presolver workspace.
--
-- On exit from the routine the tranformed problem in the workspace is
-- identical to the original problem. */

void ipp_load_orig(IPP *ipp, LPX *orig)
{     IPPROW **row;
      IPPCOL *col;
      int i, j, k, type, len, *ind;
      double lb, ub, *val;
      /* save some information about the original problem */
      ipp->orig_m = lpx_get_num_rows(orig);
      ipp->orig_n = lpx_get_num_cols(orig);
      ipp->orig_nnz = lpx_get_num_nz(orig);
      ipp->orig_dir = lpx_get_obj_dir(orig);
      /* allocate working arrays */
      row = xcalloc(1+ipp->orig_m, sizeof(IPPROW *));
      ind = xcalloc(1+ipp->orig_m, sizeof(int));
      val = xcalloc(1+ipp->orig_m, sizeof(double));
      /* copy rows of the original problem into the workspace */
      for (i = 1; i <= ipp->orig_m; i++)
      {  type = lpx_get_row_type(orig, i);
         if (type == LPX_FR || type == LPX_UP)
            lb = -DBL_MAX;
         else
            lb = lpx_get_row_lb(orig, i);
         if (type == LPX_FR || type == LPX_LO)
            ub = +DBL_MAX;
         else
            ub = lpx_get_row_ub(orig, i);
         row[i] = ipp_add_row(ipp, lb, ub);
      }
      /* copy columns of the original problem into the workspace; each
         column created in the workspace is assigned a reference number
         which is its ordinal number in the original problem */
      for (j = 1; j <= ipp->orig_n; j++)
      {  type = lpx_get_col_type(orig, j);
         if (type == LPX_FR || type == LPX_UP)
            lb = -DBL_MAX;
         else
            lb = lpx_get_col_lb(orig, j);
         if (type == LPX_FR || type == LPX_LO)
            ub = +DBL_MAX;
         else
            ub = lpx_get_col_ub(orig, j);
         col = ipp_add_col(ipp, lpx_get_col_kind(orig, j) == LPX_IV,
            lb, ub, lpx_get_obj_coef(orig, j));
         len = lpx_get_mat_col(orig, j, ind, val);
         for (k = 1; k <= len; k++)
            ipp_add_aij(ipp, row[ind[k]], col, val[k]);
      }
      /* copy the constant term of the original objective function */
      ipp->c0 = lpx_get_obj_coef(orig, 0);
      /* if the original problem is maximization, change the sign of
         the objective function, because the transformed problem to be
         processed by the presolver must be minimization */
      if (ipp->orig_dir == LPX_MAX)
      {  for (col = ipp->col_ptr; col != NULL; col = col->next)
            col->c = - col->c;
         ipp->c0 = - ipp->c0;
      }
      /* free working arrays */
      xfree(row);
      xfree(ind);
      xfree(val);
      return;
}

/*----------------------------------------------------------------------
-- ipp_tight_bnds - tight current column bounds using implied bounds.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- int ipp_tight_bnds(IPP *ipp, IPPCOL *col, double lb, double ub);
--
-- DESCRIPTION
--
-- The routines ipp_tight_bnds replaces current bounds of the column,
-- which the parameter col points to:
--
--    l <= x <= u,                                                   (1)
--
-- by new bounds:
--
--    max(l, l') <= x <= min(u, u'),                                 (2)
--
-- where l' and u' are specified implied bounds of the column.
--
-- RETURNS
--
-- 0 - bounds remain unchanged
-- 1 - bounds have been changed
-- 2 - new bounds are primal infeasible */

int ipp_tight_bnds(IPP *ipp, IPPCOL *col, double lb, double ub)
{     int ret = 0;
      double eps;
      xassert(ipp == ipp);
      /* if the column is integral, round implied bounds */
      if (col->i_flag)
      {  eps = 1e-5 * (1.0 + fabs(lb));
         if (fabs(lb - floor(lb + 0.5)) <= eps)
            lb = floor(lb + 0.5);
         else
            lb = ceil(lb);
         eps = 1e-5 * (1.0 + fabs(ub));
         if (fabs(ub - floor(ub + 0.5)) <= eps)
            ub = floor(ub + 0.5);
         else
            ub = floor(ub);
      }
      /* check for primal infeasibility */
      if (col->lb != -DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->lb));
         if (ub < col->lb - eps)
         {  ret = 2;
            goto done;
         }
      }
      if (col->ub != +DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->ub));
         if (lb > col->ub + eps)
         {  ret = 2;
            goto done;
         }
      }
#if 1
      if (col->i_flag && lb > ub + 0.5)
      {  /* this may happen due to rounding implied bounds */
         ret = 2;
         goto done;
      }
#endif
      /* replace current bounds by implied ones, if necessary */
      if (lb != -DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(lb));
         if (col->lb < lb - eps)
         {  col->lb = lb;
            ret = 1;
         }
      }
      if (ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(ub));
         if (col->ub > ub + eps)
         {  col->ub = ub;
            ret = 1;
         }
      }
      /* if new bounds of the column are close to each other, the
         column can be fixed */
      if (ret == 1 && col->lb != -DBL_MAX && col->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(col->lb));
         if (col->lb >= col->ub - eps)
         {  if (fabs(col->lb) <= fabs(col->ub))
               col->ub = col->lb;
            else
               col->lb = col->ub;
         }
      }
done: xassert(col->lb <= col->ub);
      return ret;
}

/*----------------------------------------------------------------------
-- ipp_build_prob - build resultant problem.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- LPX *ipp_build_prob(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_build_prob converts the resultant MIP problem from
-- an internal format, in which the problem is stored in the workspace,
-- to the standard problem object.
--
-- RETURNS
--
-- The routine returns a pointer to the problem object. */

LPX *ipp_build_prob(IPP *ipp)
{     LPX *prob;
      IPPROW *row;
      IPPCOL *col;
      IPPAIJ *aij;
      int i, j, type, len, *ind;
      double *val;
      /* create problem object */
      prob = lpx_create_prob();
#if 0
      lpx_set_class(prob, LPX_MIP);
#endif
      /* the resultant problem should have the same optimization sense
         as the original problem */
      lpx_set_obj_dir(prob, ipp->orig_dir);
      /* set the constant term of the objective function */
      lpx_set_obj_coef(prob, 0,
         ipp->orig_dir == LPX_MIN ? + ipp->c0 : - ipp->c0);
      /* copy rows of the resultant problem */
      for (row = ipp->row_ptr; row != NULL; row = row->next)
      {  i = lpx_add_rows(prob, 1);
         if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
            type = LPX_FR;
         else if (row->ub == +DBL_MAX)
            type = LPX_LO;
         else if (row->lb == -DBL_MAX)
            type = LPX_UP;
         else if (row->lb != row->ub)
            type = LPX_DB;
         else
            type = LPX_FX;
         lpx_set_row_bnds(prob, i, type, row->lb, row->ub);
         row->temp = i;
      }
      /* copy columns of the resultant problem */
      ind = xcalloc(1+lpx_get_num_rows(prob), sizeof(int));
      val = xcalloc(1+lpx_get_num_rows(prob), sizeof(double));
      for (col = ipp->col_ptr; col != NULL; col = col->next)
      {  j = lpx_add_cols(prob, 1);
         if (col->i_flag) lpx_set_col_kind(prob, j, LPX_IV);
         if (col->lb == -DBL_MAX && col->ub == +DBL_MAX)
            type = LPX_FR;
         else if (col->ub == +DBL_MAX)
            type = LPX_LO;
         else if (col->lb == -DBL_MAX)
            type = LPX_UP;
         else if (col->lb != col->ub)
            type = LPX_DB;
         else
            type = LPX_FX;
         lpx_set_col_bnds(prob, j, type, col->lb, col->ub);
         lpx_set_obj_coef(prob, j,
            ipp->orig_dir == LPX_MIN ? + col->c : - col->c);
         /* copy constraint coefficients */
         len = 0;
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
         {  len++;
            ind[len] = aij->row->temp;
            val[len] = aij->val;
         }
         lpx_set_mat_col(prob, j, len, ind, val);
      }
      xfree(ind);
      xfree(val);
      return prob;
}

/*----------------------------------------------------------------------
-- ipp_load_sol - load solution into MIP presolver workspace.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_load_sol(IPP *ipp, LPX *prob);
--
-- DESCRIPTION
--
-- The routine ipp_load_sol loads an integer solution of the resultant
-- MIP problem into the MIP presolver workspace. */

void ipp_load_sol(IPP *ipp, LPX *prob)
{     IPPCOL *col;
      int j;
      xassert(lpx_mip_status(prob) != LPX_I_UNDEF);
      ipp->col_stat = xcalloc(1+ipp->ncols, sizeof(int));
      ipp->col_mipx = xcalloc(1+ipp->ncols, sizeof(double));
      for (j = 1; j <= ipp->ncols; j++) ipp->col_stat[j] = 0;
      /* columns in the problem object follow in the same order as in
         the column list (see ipp_build_prob) */
      j = 0;
      for (col = ipp->col_ptr; col != NULL; col = col->next)
      {  j++;
         ipp->col_stat[col->j] = 1;
         ipp->col_mipx[col->j] = lpx_mip_col_val(prob, j);
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_unload_sol - unload solution from MIP presolver workspace.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_unload_sol(IPP *ipp, LPX *orig, int i_stat);
--
-- DESCRIPTION
--
-- The routine ipp_unload_sol unloads a recovered solution from the
-- MIP presolver workspace into the original problem object, which the
-- parameter orig points to. */

void ipp_unload_sol(IPP *ipp, LPX *orig, int i_stat)
{     int i, j, k, len, *ind;
      double temp, *row_mipx, *val;
      xassert(ipp->orig_m == lpx_get_num_rows(orig));
      xassert(ipp->orig_n == lpx_get_num_cols(orig));
      xassert(ipp->orig_dir == lpx_get_obj_dir(orig));
      /* all columns must be computed/recovered */
      xassert(ipp->orig_n <= ipp->ncols);
      for (j = 1; j <= ipp->ncols; j++) xassert(ipp->col_stat[j]);
      /* compute values of auxiliary variables using known values of
         structural variables (columns) */
      row_mipx = xcalloc(1+ipp->orig_m, sizeof(double));
      ind = xcalloc(1+ipp->orig_n, sizeof(int));
      val = xcalloc(1+ipp->orig_n, sizeof(double));
      for (i = 1; i <= ipp->orig_m; i++)
      {  len = lpx_get_mat_row(orig, i, ind, val);
         temp = 0.0;
         for (k = 1; k <= len; k++)
            temp += val[k] * ipp->col_mipx[ind[k]];
         row_mipx[i] = temp;
      }
      xfree(ind);
      xfree(val);
      /* store solution components into the original problem object */
      lpx_put_mip_soln(orig, i_stat, row_mipx, ipp->col_mipx);
      xfree(row_mipx);
      return;
}

/*----------------------------------------------------------------------
-- ipp_delete_wksp - delete MIP presolver workspace.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_delete_wksp(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_delete_wksp deletes a MIP presolver workspace, which
-- the parameter ipp points to, freeing all the memory allocated to this
-- object. */

void ipp_delete_wksp(IPP *ipp)
{     if (ipp->row_pool != NULL) dmp_delete_pool(ipp->row_pool);
      if (ipp->col_pool != NULL) dmp_delete_pool(ipp->col_pool);
      if (ipp->aij_pool != NULL) dmp_delete_pool(ipp->aij_pool);
      if (ipp->tqe_pool != NULL) dmp_delete_pool(ipp->tqe_pool);
      if (ipp->col_stat != NULL) xfree(ipp->col_stat);
      if (ipp->col_mipx != NULL) xfree(ipp->col_mipx);
      xfree(ipp);
      return;
}

/* eof */
