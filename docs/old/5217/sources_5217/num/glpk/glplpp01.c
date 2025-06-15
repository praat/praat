/* glplpp01.c */

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
#include "glplpp.h"
#define dmp_create_poolx(size) dmp_create_pool()
#define dmp_get_atomv dmp_get_atom

/*----------------------------------------------------------------------
-- lpp_create_wksp - create LP presolver workspace.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- LPP *lpp_create_wksp(void);
--
-- *Description*
--
-- The routine lpp_create_wksp creates an empty workspace used by the
-- LP presolver routines.
--
-- *Returns*
--
-- The routine returns a pointer to the LP workspace created. */

LPP *lpp_create_wksp(void)
{     LPP *lpp;
      lpp = xmalloc(sizeof(LPP));
      lpp->orig_m = 0;
      lpp->orig_n = 0;
      lpp->orig_nnz = 0;
      lpp->orig_dir = LPX_MIN;
      lpp->nrows = 0;
      lpp->ncols = 0;
      lpp->row_pool = dmp_create_poolx(sizeof(LPPROW));
      lpp->col_pool = dmp_create_poolx(sizeof(LPPCOL));
      lpp->aij_pool = dmp_create_poolx(sizeof(LPPAIJ));
      lpp->row_ptr = NULL;
      lpp->col_ptr = NULL;
      lpp->row_que = NULL;
      lpp->col_que = NULL;
      lpp->c0 = 0.0;
      lpp->tqe_pool = dmp_create_poolx(0);
      lpp->tqe_list = NULL;
      lpp->m = 0;
      lpp->n = 0;
      lpp->nnz = 0;
      lpp->row_ref = NULL;
      lpp->col_ref = NULL;
      lpp->row_stat = NULL;
      lpp->row_prim = NULL;
      lpp->row_dual = NULL;
      lpp->col_stat = NULL;
      lpp->col_prim = NULL;
      lpp->col_dual = NULL;
      return lpp;
}

/*----------------------------------------------------------------------
-- lpp_add_row - add new row to the transformed problem.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- LPPROW *lpp_add_row(LPP *lpp, double lb, double ub);
--
-- *Description*
--
-- The routine lpp_add_row adds a new empty row to the transformed
-- problem.
--
-- The parameter lb is an lower bound of the new row (-DBL_MAX means
-- the row has no lower bound).
--
-- The parameter ub is an upper bound of the new row (+DBL_MAX means
-- the row has no upper bound).
--
-- *Returns*
--
-- The routine returns a pointer to the created row. */

LPPROW *lpp_add_row(LPP *lpp, double lb, double ub)
{     LPPROW *row;
      row = dmp_get_atom(lpp->row_pool, sizeof(LPPROW));
      row->i = ++(lpp->nrows);
      row->lb = lb;
      row->ub = ub;
      row->ptr = NULL;
      row->temp = 0;
      row->prev = NULL;
      row->next = lpp->row_ptr;
      row->q_flag = 0;
      row->q_prev = row->q_next = NULL;
      if (lpp->row_ptr != NULL) lpp->row_ptr->prev = row;
      lpp->row_ptr = row;
      lpp_enque_row(lpp, row);
      return row;
}

/*----------------------------------------------------------------------
-- lpp_add_col - add new column to the transformed problem.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- LPPCOL *lpp_add_col(LPP *lpp, double lb, double ub, double c);
--
-- *Description*
--
-- The routine lpp_add_col adds a new empty column to the transformed
-- problem.
--
-- The parameter lb is an lower bound of the new column (-DBL_MAX means
-- the column has no lower bound).
--
-- The parameter ub is an upper bound of the new column (+DBL_MAX means
-- the column has no upper bound).
--
-- The parameter c is an objective coefficient at the new column.
--
-- *Returns*
--
-- The routine returns a pointer to the created column. */

LPPCOL *lpp_add_col(LPP *lpp, double lb, double ub, double c)
{     LPPCOL *col;
      col = dmp_get_atom(lpp->col_pool, sizeof(LPPCOL));
      col->j = ++(lpp->ncols);
      col->lb = lb;
      col->ub = ub;
      col->c = c;
      col->ptr = NULL;
      col->prev = NULL;
      col->next = lpp->col_ptr;
      col->q_flag = 0;
      col->q_prev = col->q_next = NULL;
      if (lpp->col_ptr != NULL) lpp->col_ptr->prev = col;
      lpp->col_ptr = col;
      lpp_enque_col(lpp, col);
      return col;
}

/*----------------------------------------------------------------------
-- lpp_add_aij - add new element to the constraint matrix.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- LPPAIJ *lpp_add_aij(LPP *lpp, LPPROW *row, LPPCOL *col, double val);
--
-- *Description*
--
-- The routine lpp_add_aij adds a new element to the constraint matrix
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
-- *Returns*
--
-- The routine returns a pointer to the created element. */

LPPAIJ *lpp_add_aij(LPP *lpp, LPPROW *row, LPPCOL *col, double val)
{     LPPAIJ *aij;
      xassert(val != 0.0);
      aij = dmp_get_atom(lpp->aij_pool, sizeof(LPPAIJ));
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
-- lpp_remove_row - remove row from the transformed problem.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_remove_row(LPP *lpp, LPPROW *row);
--
-- *Description*
--
-- The routine lpp_remove_row removes a row, which the parameter row
-- points to, from the transformed problem. */

void lpp_remove_row(LPP *lpp, LPPROW *row)
{     LPPAIJ *aij;
      /* remove the row from the active queue */
      lpp_deque_row(lpp, row);
      /* remove elements of the row from the constraint matrix */
      while (row->ptr != NULL)
      {  /* get a next element in the row */
         aij = row->ptr;
         /* activate the corresponding column */
         lpp_enque_col(lpp, aij->col);
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
         dmp_free_atom(lpp->aij_pool, aij, sizeof(LPPAIJ));
      }
      /* remove the row from the linked list */
      if (row->prev == NULL)
         lpp->row_ptr = row->next;
      else
         row->prev->next = row->next;
      if (row->next == NULL)
         ;
      else
         row->next->prev = row->prev;
      /* and return the row to its pool */
      dmp_free_atom(lpp->row_pool, row, sizeof(LPPROW));
      return;
}

/*----------------------------------------------------------------------
-- lpp_remove_col - remove column from the transformed problem.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_remove_col(LPP *lpp, LPPCOL *col);
--
-- *Description*
--
-- The routine lpp_remove_col removes a column, which the parameter col
-- points to, from the transformed problem. */

void lpp_remove_col(LPP *lpp, LPPCOL *col)
{     LPPAIJ *aij;
      /* remove the column from the active queue */
      lpp_deque_col(lpp, col);
      /* remove elements of the column from the constraint matrix */
      while (col->ptr != NULL)
      {  /* get a next element in the column */
         aij = col->ptr;
         /* activate the corresponding row */
         lpp_enque_row(lpp, aij->row);
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
         dmp_free_atom(lpp->aij_pool, aij, sizeof(LPPAIJ));
      }
      /* remove the column from the linked list */
      if (col->prev == NULL)
         lpp->col_ptr = col->next;
      else
         col->prev->next = col->next;
      if (col->next == NULL)
         ;
      else
         col->next->prev = col->prev;
      /* and return the column to its pool */
      dmp_free_atom(lpp->col_pool, col, sizeof(LPPCOL));
      return;
}

/*----------------------------------------------------------------------
-- lpp_enque_row - place row in the active queue.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_enque_row(LPP *lpp, LPPROW *row);
--
-- *Description*
--
-- The routine lpp_enque_row places the specified row to the queue of
-- active rows. */

void lpp_enque_row(LPP *lpp, LPPROW *row)
{     if (!row->q_flag)
      {  row->q_flag = 1;
         row->q_prev = NULL;
         row->q_next = lpp->row_que;
         if (lpp->row_que != NULL) lpp->row_que->q_prev = row;
         lpp->row_que = row;
      }
      return;
}

/*----------------------------------------------------------------------
-- lpp_deque_row - remove row from the active queue.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_deque_row(LPP *lpp, LPPROW *row);
--
-- *Description*
--
-- The routine lpp_deque_row removes the specified row from the queue
-- of active rows. */

void lpp_deque_row(LPP *lpp, LPPROW *row)
{     if (row->q_flag)
      {  row->q_flag = 0;
         if (row->q_prev == NULL)
            lpp->row_que = row->q_next;
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
-- lpp_enque_col - place column in the active queue.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_enque_col(LPP *lpp, LPPCOL *col);
--
-- *Description*
--
-- The routine lpp_enque_col places the specified column to the queue of
-- active columns. */

void lpp_enque_col(LPP *lpp, LPPCOL *col)
{     if (!col->q_flag)
      {  col->q_flag = 1;
         col->q_prev = NULL;
         col->q_next = lpp->col_que;
         if (lpp->col_que != NULL) lpp->col_que->q_prev = col;
         lpp->col_que = col;
      }
      return;
}

/*----------------------------------------------------------------------
-- lpp_deque_col - remove column from the active queue.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_deque_col(LPP *lpp, LPPCOL *col);
--
-- *Description*
--
-- The routine lpp_deque_col removes the specified column from the queue
-- of active columns. */

void lpp_deque_col(LPP *lpp, LPPCOL *col)
{     if (col->q_flag)
      {  col->q_flag = 0;
         if (col->q_prev == NULL)
            lpp->col_que = col->q_next;
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
-- lpp_load_orig - load original problem into LP presolver workspace.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_load_orig(LPP *lpp, LPX *orig);
--
-- *Description*
--
-- The routine lpp_load_orig loads an original LP problem, which the
-- parameter orig points to, into the LP presolver workspace.
--
-- On exit from the routine the tranformed problem in the workspace is
-- identical to the original problem. */

void lpp_load_orig(LPP *lpp, LPX *orig)
{     LPPROW *row;
      LPPCOL *col, **map;
      int i, j, t, len, typx, *ndx;
      double lb, ub, temp, *c, *val;
      /* save some information about the original problem */
      lpp->orig_m = lpx_get_num_rows(orig);
      lpp->orig_n = lpx_get_num_cols(orig);
      lpp->orig_nnz = lpx_get_num_nz(orig);
      lpp->orig_dir = lpx_get_obj_dir(orig);
      /* allocate working arrays */
      c = xcalloc(1+lpp->orig_n, sizeof(double));
      ndx = xcalloc(1+lpp->orig_n, sizeof(int));
      val = xcalloc(1+lpp->orig_n, sizeof(double));
      /* auxiliary variables (i.e. rows) in the original problem may
         have non-zero objective coefficients; so, we substitute these
         auxiliary variables into the objective function in order that
         it depends only on structural variables (i.e. columns); the
         resultant vector of objective coefficients is accumulated in
         the working array c */
      for (j = 1; j <= lpp->orig_n; j++)
         c[j] = lpx_get_obj_coef(orig, j);
      for (i = 1; i <= lpp->orig_m; i++)
      {  /* obtain an objective coefficient at i-th row */
#if 0
         temp = lpx_get_row_coef(orig, i);
#else
         temp = 0.0;
#endif
         /* substitute i-th row into the objective function */
         if (temp != 0.0)
         {  len = lpx_get_mat_row(orig, i, ndx, val);
            for (t = 1; t <= len; t++) c[ndx[t]] += val[t] * temp;
         }
      }
      /* copy rows of the original problem into the workspace; each
         row created in the workspace is assigned a reference number,
         which is its ordinal number in the original problem */
      for (i = 1; i <= lpp->orig_m; i++)
      {  lpx_get_row_bnds(orig, i, &typx, &lb, &ub);
         if (typx == LPX_FR || typx == LPX_UP) lb = -DBL_MAX;
         if (typx == LPX_FR || typx == LPX_LO) ub = +DBL_MAX;
         if (typx == LPX_FX) ub = lb;
         lpp_add_row(lpp, lb, ub);
      }
      /* copy columns of the original problem into the workspace; each
         column created in the workspace is assigned a reference number,
         which its ordinal number in the original problem */
      for (j = 1; j <= lpp->orig_n; j++)
      {  lpx_get_col_bnds(orig, j, &typx, &lb, &ub);
         if (typx == LPX_FR || typx == LPX_UP) lb = -DBL_MAX;
         if (typx == LPX_FR || typx == LPX_LO) ub = +DBL_MAX;
         if (typx == LPX_FX) ub = lb;
         lpp_add_col(lpp, lb, ub, c[j]);
      }
      /* copy the constant term of the original objective function */
      lpp->c0 = lpx_get_obj_coef(orig, 0);
      /* if the original problem is maximization, change the sign of
         the objective function, because the transformed problem to be
         processed by the presolver must be minimization */
      if (lpp->orig_dir == LPX_MAX)
      {  for (col = lpp->col_ptr; col != NULL; col = col->next)
            col->c = - col->c;
         lpp->c0 = - lpp->c0;
      }
      /* build an auxiliary array to map column ordinal numbers to the
         corresponding pointers */
      xassert(sizeof(LPPCOL *) <= sizeof(double));
      map = (LPPCOL **)c;
      for (col = lpp->col_ptr; col != NULL; col = col->next)
         map[col->j] = col;
      /* copy the original constraint matrix into the workspace */
      for (row = lpp->row_ptr; row != NULL; row = row->next)
#if 1
      {  len = lpx_get_mat_row(orig, row->i, ndx, val);
         for (t = 1; t <= len; t++)
            lpp_add_aij(lpp, row, map[ndx[t]], val[t]);
      }
#else /* 27/XI-2003 (the problem persists) */
      {  double big, eps;
         len = lpx_get_mat_row(orig, row->i, ndx, val);
         big = 0.0;
         for (t = 1; t <= len; t++)
            if (big < fabs(val[t])) big = fabs(val[t]);
         eps = 1e-10 * big;
         for (t = 1; t <= len; t++)
         {  if (fabs(val[t]) < eps) continue;
            lpp_add_aij(lpp, row, map[ndx[t]], val[t]);
         }
      }
#endif
      /* free working arrays */
      xfree(c);
      xfree(ndx);
      xfree(val);
      return;
}

/*----------------------------------------------------------------------
-- lpp_append_tqe - append new transformation queue entry.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void *lpp_append_tqe(LPP *lpp, int type, int size);
--
-- *Description*
--
-- The routine lpp_append_tqe appends a new transformation queue entry
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

void *lpp_append_tqe(LPP *lpp, int type, int size)
{     LPPTQE *tqe;
      tqe = dmp_get_atomv(lpp->tqe_pool, sizeof(LPPTQE));
      tqe->type = type;
      tqe->info = dmp_get_atomv(lpp->tqe_pool, size);
      tqe->next = lpp->tqe_list;
      lpp->tqe_list = tqe;
      return tqe->info;
}

/*----------------------------------------------------------------------
-- lpp_build_prob - build resultant problem.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- LPX *lpp_build_prob(LPP *lpp);
--
-- *Description*
--
-- The routine lpp_build_prob converts the resultant LP problem from an
-- internal format, in which the problem is stored in the workspace, to
-- the LPX format.
--
-- *Returns*
--
-- The routine returns a pointer to the LP problem object. */

LPX *lpp_build_prob(LPP *lpp)
{     LPX *prob;
      LPPROW *row;
      LPPCOL *col;
      int i, j, typx;
      /* count number of rows and columns in the resultant problem */
      lpp->m = lpp->n = 0;
      for (row = lpp->row_ptr; row != NULL; row = row->next) lpp->m++;
      for (col = lpp->col_ptr; col != NULL; col = col->next) lpp->n++;
      /* allocate two arrays to save reference numbers assigned to rows
         and columns of the resultant problem */
      lpp->row_ref = xcalloc(1+lpp->m, sizeof(int));
      lpp->col_ref = xcalloc(1+lpp->n, sizeof(int));
      /* create LP problem object */
      prob = lpx_create_prob();
      /* the resultant problem should have the same optimization sense
         as the original problem */
      lpx_set_obj_dir(prob, lpp->orig_dir);
      /* set the constant term of the objective function */
      lpx_set_obj_coef(prob, 0,
         lpp->orig_dir == LPX_MIN ? + lpp->c0 : - lpp->c0);
      /* create rows of the resultant problem */
      xassert(lpp->m > 0);
      lpx_add_rows(prob, lpp->m);
      for (i = 1, row = lpp->row_ptr; i <= lpp->m; i++, row = row->next)
      {  xassert(row != NULL);
         lpp->row_ref[i] = row->i;
         row->i = i;
         if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
            typx = LPX_FR;
         else if (row->ub == +DBL_MAX)
            typx = LPX_LO;
         else if (row->lb == -DBL_MAX)
            typx = LPX_UP;
         else if (row->lb != row->ub)
            typx = LPX_DB;
         else
            typx = LPX_FX;
         lpx_set_row_bnds(prob, i, typx, row->lb, row->ub);
      }
      xassert(row == NULL);
      /* create columns of the resultant problem */
      xassert(lpp->n > 0);
      lpx_add_cols(prob, lpp->n);
      for (j = 1, col = lpp->col_ptr; j <= lpp->n; j++, col = col->next)
      {  xassert(col != NULL);
         lpp->col_ref[j] = col->j;
         col->j = j;
         if (col->lb == -DBL_MAX && col->ub == +DBL_MAX)
            typx = LPX_FR;
         else if (col->ub == +DBL_MAX)
            typx = LPX_LO;
         else if (col->lb == -DBL_MAX)
            typx = LPX_UP;
         else if (col->lb != col->ub)
            typx = LPX_DB;
         else
            typx = LPX_FX;
         lpx_set_col_bnds(prob, j, typx, col->lb, col->ub);
         lpx_set_obj_coef(prob, j,
            lpp->orig_dir == LPX_MIN ? + col->c : - col->c);
      }
      xassert(col == NULL);
      /* create the constraint matrix of the resultant problem */
#if 0
      info.lpp = lpp;
      info.row = NULL;
      info.aij = NULL;
      lpx_load_mat(prob, &info, next_aij);
#else
      {  LPPAIJ *aij;
         int len, *ind;
         double *val;
         ind = xcalloc(1+lpp->n, sizeof(int));
         val = xcalloc(1+lpp->n, sizeof(double));
         for (row = lpp->row_ptr; row != NULL; row = row->next)
         {  len = 0;
            for (aij = row->ptr; aij != NULL; aij = aij->r_next)
               len++, ind[len] = aij->col->j, val[len] = aij->val;
            lpx_set_mat_row(prob, row->i, len, ind, val);
         }
         xfree(ind);
         xfree(val);
      }
#endif
      /* count number of non-zeros in the resultant problem */
      lpp->nnz = lpx_get_num_nz(prob);
      /* internal data structures that represnts the resultant problem
         are no longer needed, so free them */
      dmp_delete_pool(lpp->row_pool), lpp->row_pool = NULL;
      dmp_delete_pool(lpp->col_pool), lpp->col_pool = NULL;
      dmp_delete_pool(lpp->aij_pool), lpp->aij_pool = NULL;
      lpp->row_ptr = NULL, lpp->col_ptr = NULL;
      lpp->row_que = NULL, lpp->col_que = NULL;
      /* return a pointer to the built LP problem object */
      return prob;
}

/*----------------------------------------------------------------------
-- lpp_alloc_sol - allocate recovered solution segment.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_alloc_sol(LPP *lpp);
--
-- *Description*
--
-- The routine lpp_alloc_sol allocates and initializes the recovered
-- solution segment. */

void lpp_alloc_sol(LPP *lpp)
{     int i, j;
      lpp->row_stat = xcalloc(1+lpp->nrows, sizeof(int));
      lpp->row_prim = xcalloc(1+lpp->nrows, sizeof(double));
      lpp->row_dual = xcalloc(1+lpp->nrows, sizeof(double));
      lpp->col_stat = xcalloc(1+lpp->ncols, sizeof(int));
      lpp->col_prim = xcalloc(1+lpp->ncols, sizeof(double));
      lpp->col_dual = xcalloc(1+lpp->ncols, sizeof(double));
      for (i = 1; i <= lpp->nrows; i++) lpp->row_stat[i] = 0;
      for (j = 1; j <= lpp->ncols; j++) lpp->col_stat[j] = 0;
      return;
}

/*----------------------------------------------------------------------
-- lpp_load_sol - load basic solution into LP presolver workspace.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_load_sol(LPP *lpp, LPX *prob);
--
-- *Description*
--
-- The routine lpp_load_sol loads a basic solution of the resultant LP
-- problem into the LP presolver workspace. */

void lpp_load_sol(LPP *lpp, LPX *prob)
{     int i, j, ref, stat;
      double prim, dual;
      xassert(lpp->m == lpx_get_num_rows(prob));
      xassert(lpp->n == lpx_get_num_cols(prob));
      xassert(lpp->orig_dir == lpx_get_obj_dir(prob));
      xassert(lpx_get_status(prob) != LPX_UNDEF);
      for (i = 1; i <= lpp->m; i++)
      {  lpx_get_row_info(prob, i, &stat, &prim, &dual);
         ref = lpp->row_ref[i];
         xassert(1 <= ref && ref <= lpp->nrows);
         xassert(lpp->row_stat[ref] == 0);
         lpp->row_stat[ref] = stat;
         lpp->row_prim[ref] = prim;
         lpp->row_dual[ref] =
            (lpp->orig_dir == LPX_MIN ? + dual : - dual);
      }
      for (j = 1; j <= lpp->n; j++)
      {  lpx_get_col_info(prob, j, &stat, &prim, &dual);
         ref = lpp->col_ref[j];
         xassert(1 <= ref && ref <= lpp->ncols);
         xassert(lpp->col_stat[ref] == 0);
         lpp->col_stat[ref] = stat;
         lpp->col_prim[ref] = prim;
         lpp->col_dual[ref] =
            (lpp->orig_dir == LPX_MIN ? + dual : - dual);
      }
      xfree(lpp->row_ref), lpp->row_ref = NULL;
      xfree(lpp->col_ref), lpp->col_ref = NULL;
      return;
}

/*----------------------------------------------------------------------
-- lpp_unload_sol - unload basic solution from LP presolver workspace.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_unload_sol(LPP *lpp, LPX *orig);
--
-- *Description*
--
-- The routine lpp_unload_sol unloads a recovered basic solution from
-- the LP presolver workspace into the original problem object, which
-- the parameter orig points to. */

void lpp_unload_sol(LPP *lpp, LPX *orig)
{     int i, j, k, m, n, typx, tagx, p_stat, d_stat;
      double sum;
      m = lpp->orig_m;
      n = lpp->orig_n;
      xassert(m == lpx_get_num_rows(orig));
      xassert(n == lpx_get_num_cols(orig));
      xassert(lpp->orig_dir == lpx_get_obj_dir(orig));
      /* check row and column statuses */
      xassert(m <= lpp->nrows);
      xassert(n <= lpp->ncols);
      for (k = 1; k <= m+n; k++)
      {  tagx = (k <= m ? lpp->row_stat[k] : lpp->col_stat[k-m]);
         if (tagx != LPX_BS)
         {  if (k <= m)
               lpx_get_row_bnds(orig, k, &typx, NULL, NULL);
            else
               lpx_get_col_bnds(orig, k-m, &typx, NULL, NULL);
            switch (typx)
            {  case LPX_FR:
                  xassert(tagx == LPX_NF);
                  break;
               case LPX_LO:
                  xassert(tagx == LPX_NL);
                  break;
               case LPX_UP:
                  xassert(tagx == LPX_NU);
                  break;
               case LPX_DB:
                  xassert(tagx == LPX_NL || tagx == LPX_NU);
                  break;
               case LPX_FX:
                  xassert(tagx == LPX_NS);
                  break;
               default:
                  xassert(orig != orig);
            }
         }
      }
      /* if the original problem is maximization, change signs of dual
         values */
      if (lpp->orig_dir == LPX_MAX)
      {  for (i = 1; i <= m; i++) lpp->row_dual[i] = -lpp->row_dual[i];
         for (j = 1; j <= n; j++) lpp->col_dual[j] = -lpp->col_dual[j];
      }
      /* store solution components into the original problem object (it
         is assumed that the recovered solution is optimal) */
      p_stat = d_stat = GLP_FEAS;
      for (i = 1; i <= m; i++)
         lpp->row_stat[i] = lpp->row_stat[i] - LPX_BS + GLP_BS;
      for (j = 1; j <= n; j++)
         lpp->col_stat[j] = lpp->col_stat[j] - LPX_BS + GLP_BS;
      sum = lpx_get_obj_coef(orig, 0);
      for (j = 1; j <= n; j++)
         sum += lpx_get_obj_coef(orig, j) * lpp->col_prim[j];
      glp_put_solution(orig, 1, &p_stat, &d_stat, &sum,
         lpp->row_stat, lpp->row_prim, lpp->row_dual,
         lpp->col_stat, lpp->col_prim, lpp->col_dual);
      for (i = 1; i <= m; i++)
         lpp->row_stat[i] = lpp->row_stat[i] - GLP_BS + LPX_BS;
      for (j = 1; j <= n; j++)
         lpp->col_stat[j] = lpp->col_stat[j] - GLP_BS + LPX_BS;
      return;
}

/*----------------------------------------------------------------------
-- lpp_delete_wksp - delete LP presolver workspace.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_delete_wksp(LPP *lpp);
--
-- *Description*
--
-- The routine lpp_delete_wksp deletes an LP presolver workspace, which
-- the parameter lpp points to, freeing all the memory allocated to this
-- object. */

void lpp_delete_wksp(LPP *lpp)
{     if (lpp->row_pool != NULL) dmp_delete_pool(lpp->row_pool);
      if (lpp->col_pool != NULL) dmp_delete_pool(lpp->col_pool);
      if (lpp->aij_pool != NULL) dmp_delete_pool(lpp->aij_pool);
      if (lpp->tqe_pool != NULL) dmp_delete_pool(lpp->tqe_pool);
      if (lpp->row_ref != NULL) xfree(lpp->row_ref);
      if (lpp->col_ref != NULL) xfree(lpp->col_ref);
      if (lpp->row_stat != NULL) xfree(lpp->row_stat);
      if (lpp->row_prim != NULL) xfree(lpp->row_prim);
      if (lpp->row_dual != NULL) xfree(lpp->row_dual);
      if (lpp->col_stat != NULL) xfree(lpp->col_stat);
      if (lpp->col_prim != NULL) xfree(lpp->col_prim);
      if (lpp->col_dual != NULL) xfree(lpp->col_dual);
      xfree(lpp);
      return;
}

/* eof */
