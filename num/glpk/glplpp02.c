/* glplpp02.c */

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
#define dmp_get_atomv dmp_get_atom

/*----------------------------------------------------------------------
-- EMPTY ROW
--
-- *Processing*
--
-- Let row p be empty, i.e. all its coefficients are zero:
--
--    l[p] <= y[p] = 0 <= u[p].                                      (1)
--
-- If l[p] <= 0 and u[p] >= 0, the row is redundant and therefore can
-- be removed from the problem. Otherwise, if l[p] > 0 or u[p] < 0, the
-- row is primal infeasible.
--
-- *Recovering*
--
-- Being redundant the empty row is non-active constraint.
--
-- Primal value y[p] is zero as it follows from (1).
--
-- Dual value pi[p] is zero, because the constraint is non-active. */

typedef struct
{     /* empty row */
      int p;
      /* reference number of empty row */
} EMPTY_ROW;

static int process_empty_row(LPP *lpp, LPPROW *row)
{     EMPTY_ROW *info;
      double eps;
      /* the row must be empty */
      xassert(row->ptr == NULL);
      /* check for primal infeasibility */
      eps = 1e-5;
      if (row->lb > +eps || row->ub < -eps) return 1;
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_EMPTY_ROW, sizeof(EMPTY_ROW));
      info->p = row->i;
      /* remove the row from the problem */
      lpp_remove_row(lpp, row);
      return 0;
}

static void recover_empty_row(LPP *lpp, EMPTY_ROW *info)
{     xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == 0);
      /* the empty row is non-active */
      lpp->row_stat[info->p] = LPX_BS;
      /* both primal and dual values are zero */
      lpp->row_prim[info->p] = 0.0;
      lpp->row_dual[info->p] = 0.0;
      return;
}

/*----------------------------------------------------------------------
-- EMPTY COLUMN
--
-- *Processing*
--
-- Let column q be empty, i.e. it has zero coefficients in all rows.
-- Then its dual value is equal to its objective coefficient:
--
--    lambda[q] = c[q].                                              (1)
--
-- So, if c[q] > 0 (c[q] < 0), the variable x[q] should be fixed at its
-- lower (upper) bound, and if c[q] = 0, the variable x[q] can be fixed
-- at any value. And being fixed the variable x[q] can be removed from
-- the problem. If the variable x[q] cannot be properly fixed, i.e. if
-- c[q] > 0 (c[q] < 0) and x[q] has no lower (upper) bound, the column
-- is dual infeasible.
--
-- *Recovering*
--
-- Being fixed on its bound the empty column is non-basic.
--
-- Primal value x[q] is a value, at which the column is fixed.
--
-- Dual value lambda[q] is the objective coefficient c[q] as it follows
-- from (1). */

typedef struct
{     /* empty column */
      int q;
      /* reference number of empty column */
      int stat;
      /* column status:
         LPX_NL - non-basic variable on lower bound
         LPX_NU - non-basic variable on upper bound
         LPX_NF - non-basic free variable
         LPX_NS - non-basic fixed variable */
      double prim;
      /* primal value */
      double dual;
      /* dual value */
} EMPTY_COL;

static int process_empty_col(LPP *lpp, LPPCOL *col)
{     EMPTY_COL *info;
      double eps;
      /* the column must be empty */
      xassert(col->ptr == NULL);
      /* check for dual infeasibility */
      eps = 1e-5;
      if (col->c > +eps && col->lb == -DBL_MAX ||
          col->c < -eps && col->ub == +DBL_MAX) return 1;
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_EMPTY_COL, sizeof(EMPTY_COL));
      info->q = col->j;
      if (col->lb == -DBL_MAX && col->ub == +DBL_MAX)
      {  /* free variable */
         info->stat = LPX_NF;
         info->prim = 0.0;
      }
      else if (col->ub == +DBL_MAX)
lo:   {  /* variable with lower bound */
         info->stat = LPX_NL;
         info->prim = col->lb;
      }
      else if (col->lb == -DBL_MAX)
up:   {  /* variable with upper bound */
         info->stat = LPX_NU;
         info->prim = col->ub;
      }
      else if (col->lb != col->ub)
      {  /* double bounded variable */
         if (col->c > 0.0) goto lo;
         if (col->c < 0.0) goto up;
         if (fabs(col->lb) <= fabs(col->ub)) goto lo; else goto up;
      }
      else
      {  /* fixed variable */
         info->stat = LPX_NS;
         info->prim = col->lb;
      }
      info->dual = col->c;
      /* update the constant term of the objective function */
      lpp->c0 += col->c * info->prim;
      /* remove the column from the problem */
      lpp_remove_col(lpp, col);
      return 0;
}

static void recover_empty_col(LPP *lpp, EMPTY_COL *info)
{     xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] == 0);
      lpp->col_stat[info->q] = info->stat;
      lpp->col_prim[info->q] = info->prim;
      lpp->col_dual[info->q] = info->dual;
      return;
}

/*----------------------------------------------------------------------
-- FREE ROW
--
-- *Processing*
--
-- Let row p be free, i.e. it has no bounds:
--
--    -inf < y[p] = sum a[p,j] * x[j] < +inf.                        (1)
--
-- The corresponding "constraint" can never be active, so the free row
-- is redundant and can be removed from the problem.
--
-- *Recovering*
--
-- Being redundant the free row is non-active constraint.
--
-- Primal value y[p] is computed using the formula (1).
--
-- Dual value pi[p] is zero, because the constraint is non-active. */

typedef struct
{     /* free row */
      int p;
      /* reference number of a free row */
      LPPLFE *ptr;
      /* list of non-zero coefficients a[p,j] */
} FREE_ROW;

static void process_free_row(LPP *lpp, LPPROW *row)
{     FREE_ROW *info;
      LPPCOL *col;
      LPPAIJ *aij;
      LPPLFE *lfe;
      /* the row must be free */
      xassert(row->lb == -DBL_MAX && row->ub == +DBL_MAX);
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_FREE_ROW, sizeof(FREE_ROW));
      info->p = row->i;
      info->ptr = NULL;
      /* save coefficients of the row */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         lfe = dmp_get_atomv(lpp->tqe_pool, sizeof(LPPLFE));
         lfe->ref = col->j;
         lfe->val = aij->val;
         lfe->next = info->ptr;
         info->ptr = lfe;
      }
      /* remove the row from the problem */
      lpp_remove_row(lpp, row);
      return;
}

static void recover_free_row(LPP *lpp, FREE_ROW *info)
{     LPPLFE *lfe;
      double sum;
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == 0);
      /* the free row is non-active */
      lpp->row_stat[info->p] = LPX_BS;
      /* compute primal value y[p] using the formula (1) */
      sum = 0.0;
      for (lfe = info->ptr; lfe != NULL; lfe = lfe->next)
      {  xassert(1 <= lfe->ref && lfe->ref <= lpp->ncols);
         xassert(lpp->col_stat[lfe->ref] != 0);
         sum += lfe->val * lpp->col_prim[lfe->ref];
      }
      lpp->row_prim[info->p] = sum;
      /* set dual value pi[p] to zero */
      lpp->row_dual[info->p] = 0.0;
      return;
}

/*----------------------------------------------------------------------
-- FIXED COLUMN
--
-- Let column q be fixed:
--
--    x[q] = s[q],                                                   (1)
--
-- where s[q] is a given value. Then it can be substituted to all rows
-- and the objective function and removed from the problem.
--
-- Consider a row i:
--
--    lb[i] <= y[i] =  sum a[i,j] * x[j] + a[i,q] * x[q] <= ub[i].   (2)
--                   j != q
--
-- Substituting x[q] from (1) to (2) transforms the row i to:
--
--    lb'[i] <= y'[i] =  sum a[i,j] * x[j] <= ub'[i],                (3)
--                     j != q
--
-- with new lower and upper bounds
--
--    lb'[i] = lb[i] - a[i,q] * s[q]                                 (4)
--
--    ub'[i] = ub[i] - a[i,q] * s[q]                                 (5)
--
-- of a new auxiliary variable y'[i] for the row i.
--
-- *Recovering*
--
-- The fixed column is non-basic.
--
-- Primal value x[q] is a value s[q], at which the column is fixed.
--
-- Dual value can be computed using the dual equality constraint:
--
--    sum a[i,q] * pi[i] + lambda[q] = c[q],                         (6)
--     i
--
-- from which it follows that:
--
--    lambda[q] = c[q] - sum a[i,q] * pi[i],                         (7)
--                        i
--
-- where c[q] is an objective coefficient of the column, a[i,q] are
-- constraints coefficents, pi[i] are dual values of the corresponding
-- rows.
--
-- After recovering the fixed column its primal value s[q] should be
-- also substututed to all rows in order to recover the corresponding
-- primal values (see (2) and (3)):
--
--    y[i] = y'[i] + a[i,q] * s[q].                                  (8)
--
-- Note that the fixed column doesn't affect dual values pi[i], so they
-- are not changed on recovering. */

typedef struct
{     /* fixed column */
      int q;
      /* reference number of a fixed column */
      double val;
      /* value, at which the column is fixed */
      double c;
      /* objective coefficient of the column */
      LPPLFE *ptr;
      /* list of non-zero coefficients a[i,q] */
} FIXED_COL;

static void process_fixed_col(LPP *lpp, LPPCOL *col)
{     FIXED_COL *info;
      LPPROW *row;
      LPPAIJ *aij;
      LPPLFE *lfe;
      /* the column must be fixed */
      xassert(col->lb == col->ub);
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_FIXED_COL, sizeof(FIXED_COL));
      info->q = col->j;
      info->val = col->lb;
      info->c = col->c;
      info->ptr = NULL;
      /* save coefficients of the column and update bounds of rows */
      for (aij = col->ptr; aij != NULL; aij = aij->c_next)
      {  row = aij->row;
         lfe = dmp_get_atomv(lpp->tqe_pool, sizeof(LPPLFE));
         lfe->ref = row->i;
         lfe->val = aij->val;
         lfe->next = info->ptr;
         info->ptr = lfe;
         /* see (4) and (5) */
         if (row->lb == row->ub)
            row->ub = (row->lb -= aij->val * col->lb);
         else
         {  if (row->lb != -DBL_MAX) row->lb -= aij->val * col->lb;
            if (row->ub != +DBL_MAX) row->ub -= aij->val * col->lb;
         }
      }
      /* update the constant term of the objective function */
      lpp->c0 += col->c * col->lb;
      /* remove the column from the problem */
      lpp_remove_col(lpp, col);
      return;
}

static void recover_fixed_col(LPP *lpp, FIXED_COL *info)
{     LPPLFE *lfe;
      double dual;
      xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] == 0);
      /* the fixed column is non-basic */
      lpp->col_stat[info->q] = LPX_NS;
      /* set primal value x[q] */
      lpp->col_prim[info->q] = info->val;
      /* compute dual value lambda[q] using the formula (7) and update
         primal values of rows using the formula (8) */
      dual = info->c;
      for (lfe = info->ptr; lfe != NULL; lfe = lfe->next)
      {  xassert(1 <= lfe->ref && lfe->ref <= lpp->nrows);
         xassert(lpp->row_stat[lfe->ref] != 0);
         dual -= lfe->val * lpp->row_dual[lfe->ref];
         lpp->row_prim[lfe->ref] += lfe->val * info->val;
      }
      lpp->col_dual[info->q] = dual;
      return;
}

/*----------------------------------------------------------------------
-- ROW SINGLETON (EQUALITY CONSTRAINT)
--
-- *Processing*
--
-- Let row p be an equality constraint that has the only column:
--
--    y[p] = a[p,q] * x[q] = b[p],                                   (1)
--
-- in which case it implies fixing x[q]:
--
--    x[q] = b[p] / a[p,q].                                          (2)
--
-- So, if (2) doesn't conflict with bounds of x[q], the column q can be
-- fixed and therefore removed from the problem. Then the row p becomes
-- redundant and therefore can be removed from the problem. Otherwise,
-- if (2) conflicts with bounds of x[q], the row is primal infeasible.
--
-- Note that at first the row p is removed. Then the column q is fixed
-- and processed as a fixed column (see section "Fixed Column").
--
-- *Recovering*
--
-- On entry to the recovering routine the column q is already recovered
-- as if it were a fixed column, i.e. it is non-basic with primal value
-- x[q] and dual value lambda[q]. The routine makes it basic with the
-- same primal value and zero dual value.
--
-- Then the recovering routine makes the row p non-basic, and computes
-- its primal value y[p] using the formula (1) and its dual value pi[p]
-- using the formula:
--
--    pi[p] = lambda[q] / a[p,q],                                    (3)
--
-- where lambda[q] is a dual value, which the column q has on entry to
-- the recovering routine. */

typedef struct
{     /* row singleton (equality constraint) */
      int p;
      /* row reference number */
      int q;
      /* column reference number */
      double apq;
      /* constraint coefficient */
} ROW_SNGTON1;

static int process_row_sngton1(LPP *lpp, LPPROW *row)
{     ROW_SNGTON1 *info;
      LPPCOL *col;
      LPPAIJ *aij;
      double val, eps;
      /* the row must have the only non-zero constraint coefficient and
         be an equality constraint */
      xassert(row->ptr != NULL && row->ptr->r_next == NULL);
      xassert(row->lb == row->ub);
      /* compute the implied value; see (2) */
      aij = row->ptr;
      val = row->lb / aij->val;
      /* check for primal infeasibility */
      col = aij->col;
      if (col->lb != -DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->lb));
         if (val < col->lb - eps) return 1;
      }
      if (col->ub != +DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->ub));
         if (val > col->ub + eps) return 1;
      }
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_ROW_SNGTON1, sizeof(ROW_SNGTON1));
      info->p = row->i;
      info->q = col->j;
      info->apq = aij->val;
      /* remove the row from the problem */
      lpp_remove_row(lpp, row);
      /* fix the column */
      col->lb = col->ub = val;
      /* and also remove it from the problem */
      process_fixed_col(lpp, col);
      return 0;
}

static void recover_row_sngton1(LPP *lpp, ROW_SNGTON1 *info)
{     /* the row is not recovered yet */
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == 0);
      /* while the column is already recovered; it was removed as if it
         were fixed, so currently it must be non-basic */
      xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] == LPX_NS);
      /* the row is actually active */
      lpp->row_stat[info->p] = LPX_NS;
      /* compute primal value y[p] using the formula (1) */
      lpp->row_prim[info->p] = info->apq * lpp->col_prim[info->q];
      /* compute dual value pi[p] using the formula (3) */
      lpp->row_dual[info->p] = lpp->col_dual[info->q] / info->apq;
      /* the column is actually basic */
      lpp->col_stat[info->q] = LPX_BS;
      lpp->col_dual[info->q] = 0.0;
      return;
}

/*----------------------------------------------------------------------
-- ROW SINGLETON (INEQUALITY CONSTRAINT)
--
-- *Processing*
--
-- Let row p be an inequality constraint that has the only column:
--
--    l[p] <= y[p] = a[p,q] * x[q] <= u[p],                          (1)
--
-- where l[p] != u[p] and at least one of the bounds l[p] and u[p] is
-- finite. In this case the row implies bounds of x[q]:
--
--    l' <= x[q] <= u',                                              (2)
--
-- where:
--
--    if a[p,q] > 0:   l' = l[p] / a[p,q],   u' = u[p] / a[p,q];     (3)
--
--    if a[p,q] < 0:   l' = u[p] / a[p,q],   u' = l[p] / a[p,q].     (4)
--
-- If the bounds l' and u' conflict with own bounds of x[q], i.e. if
-- l' > u[q] or u' < l[q], the row is primal infeasible. Otherwise the
-- range of x[q] can be tightened as follows:
--
--    max(l[q], l') <= x[q] <= min(u[q], u'),                        (5)
--
-- in which case the row becomes redundant and therefore can be removed
-- from the problem.
--
-- *Recovering*
--
-- On entry to the recovering routine the column q is already recovered
-- as if it had the modified bounds (5). Then the row p is recovered as
-- follows.
--
-- Let the column q be basic. In this case the row p is inactive, so
-- its primal value y[p] is computed with the formula (1) and its dual
-- value pi[p] is set to zero.
--
-- Let the column q be non-basic on lower bound. If l' <= l[q], the row
-- p is inactive and recovered in the same way as above. Otherwise, if
-- l' > l[q], the column q is actually basic while the row p is active
-- on its lower bound (a[p,q] > 0) or on its upper bound (a[p,q] < 0).
-- In the latter case the row's primal value y[p] is computed using the
-- formula (1) and the dual value pi[p] is computed using the formula:
--
--    pi[p] = lambda[q] / a[p,q],                                    (6)
--
-- where lambda[q] is a dual value, which the column q has on entry to
-- the recovering routine.
--
-- Let the column q be non-basic on upper bound. If u' >= u[q], the row
-- p is inactive and recovered in the same way as above. Otherwise, if
-- u' < u[q], the column q is actually basic while the row p is active
-- on its lower bound (a[p,q] < 0) or on its upper bound (a[p,q] > 0).
-- Then the row's primal value y[p] is computed using the formula (1)
-- and the dual value pi[p] is computed using the formula (6). */

typedef struct
{     /* row singleton (inequality constraint) */
      int p;
      /* row reference number */
      int q;
      /* column reference number */
      double apq;
      /* constraint coefficient */
      int lb_changed;
      /* this flag is set if the lower bound of the column was changed,
         i.e. if l' > l[q]; see (2)--(5) */
      int ub_changed;
      /* this flag is set if the upper bound of the column was changed,
         i.e. if u' < u[q]; see (2)--(5) */
} ROW_SNGTON2;

static int process_row_sngton2(LPP *lpp, LPPROW *row)
{     ROW_SNGTON2 *info;
      LPPCOL *col;
      LPPAIJ *aij;
      double lb, ub, eps;
      /* the row must have the only non-zero constraint coefficient and
         be an inequality constraint */
      xassert(row->ptr != NULL && row->ptr->r_next == NULL);
      xassert(row->lb != row->ub);
      /* if the row is free, just remove it from the problem */
      if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
      {  process_free_row(lpp, row);
         goto done;
      }
      /* compute the impled bounds l' and u'; see (2)--(4) */
      aij = row->ptr;
      if (aij->val > 0.0)
      {  lb = (row->lb == -DBL_MAX ? -DBL_MAX : row->lb / aij->val);
         ub = (row->ub == +DBL_MAX ? +DBL_MAX : row->ub / aij->val);
      }
      else
      {  lb = (row->ub == +DBL_MAX ? -DBL_MAX : row->ub / aij->val);
         ub = (row->lb == -DBL_MAX ? +DBL_MAX : row->lb / aij->val);
      }
      /* check for primal infeasibility */
      col = aij->col;
      if (col->lb != -DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->lb));
         if (ub < col->lb - eps) return 1;
      }
      if (col->ub != +DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(col->ub));
         if (lb > col->ub + eps) return 1;
      }
      /* if the column q is fixed, it can be substituted and removed,
         in which case the row p becomes empty and being feasible (as
         already checked above) can be also removed */
      if (col->lb == col->ub)
      {  /* substitute and remove the column q */
         process_fixed_col(lpp, col);
         /* now the row p became empty and redundant */
         xassert(row->ptr == NULL);
         /* remove the row p */
         row->lb = -DBL_MAX, row->ub = +DBL_MAX;
         xassert(process_empty_row(lpp, row) == 0);
         goto done;
      }
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_ROW_SNGTON2, sizeof(ROW_SNGTON2));
      info->p = row->i;
      info->q = col->j;
      info->apq = aij->val;
      info->lb_changed = (lb != -DBL_MAX && lb > col->lb);
      info->ub_changed = (ub != +DBL_MAX && ub < col->ub);
      /* tighten bounds of the column, if necessary */
      if (info->lb_changed) col->lb = lb;
      if (info->ub_changed) col->ub = ub;
      /* the row is now redundant; remove it from the problem */
      lpp_remove_row(lpp, row);
      /* if modified bounds of the column are close to each other, the
         column can be fixed, substituted, and therefore removed */
      if (col->lb != -DBL_MAX && col->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(col->lb));
         if (fabs(col->lb - col->ub) <= eps)
         {  if (fabs(col->lb) <= fabs(col->ub))
               col->ub = col->lb;
            else
               col->lb = col->ub;
            process_fixed_col(lpp, col);
         }
      }
done: return 0;
}

static void recover_row_sngton2(LPP *lpp, ROW_SNGTON2 *info)
{     /* the row is not recovered yet */
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == 0);
      /* while the column is already recovered */
      xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] != 0);
      /* analyze the column status */
      switch (lpp->col_stat[info->q])
      {  case LPX_BS:
            /* the column is basic, so the row is inactive */
            lpp->row_stat[info->p] = LPX_BS;
            lpp->row_prim[info->p] =
               info->apq * lpp->col_prim[info->q];
            lpp->row_dual[info->p] = 0.0;
            break;
         case LPX_NL:
nl:         /* the column is non-basic on lower bound */
            if (info->lb_changed)
            {  /* it is not its own lower bound, so actually the row is
                  active */
               if (info->apq > 0.0)
                  lpp->row_stat[info->p] = LPX_NL;
               else
                  lpp->row_stat[info->p] = LPX_NU;
               lpp->row_prim[info->p] =
                  info->apq * lpp->col_prim[info->q];
               lpp->row_dual[info->p] =
                  lpp->col_dual[info->q] / info->apq;
               /* and the column is basic */
               lpp->col_stat[info->q] = LPX_BS;
               lpp->col_dual[info->q] = 0.0;
            }
            else
            {  /* it is its own lower bound, so the row is incative */
               lpp->row_stat[info->p] = LPX_BS;
               lpp->row_prim[info->p] =
                  info->apq * lpp->col_prim[info->q];
               lpp->row_dual[info->p] = 0.0;
            }
            break;
         case LPX_NU:
nu:         /* the column is non-basic on upper bound */
            if (info->ub_changed)
            {  /* it is not its own upper bound, so actually the row is
                  active */
               if (info->apq > 0.0)
                  lpp->row_stat[info->p] = LPX_NU;
               else
                  lpp->row_stat[info->p] = LPX_NL;
               lpp->row_prim[info->p] =
                  info->apq * lpp->col_prim[info->q];
               lpp->row_dual[info->p] =
                  lpp->col_dual[info->q] / info->apq;
               /* and the column is basic */
               lpp->col_stat[info->q] = LPX_BS;
               lpp->col_dual[info->q] = 0.0;
            }
            else
            {  /* it is its own upper bound, so the row is inactive */
               lpp->row_stat[info->p] = LPX_BS;
               lpp->row_prim[info->p] =
                  info->apq * lpp->col_prim[info->q];
               lpp->row_dual[info->p] = 0.0;
            }
            break;
         case LPX_NF:
            /* the column cannot be free, since the row is always has
               at least one finite bound; see (5) */
            xassert(lpp != lpp);
            /* no break */
         case LPX_NS:
            /* the column is non-basic and fixed; it cannot be fixed
               before tightening its bounds, because in that case this
               transformation entry is not created; therefore we need
               to consider the column as double-bounded whose lower and
               upper bounds are equal to each other, and choose a bound
               using its dual value lambda[q] */
            if (lpp->col_dual[info->q] >= 0.0)
            {  /* lambda[q] >= 0; set the column on lower bound */
               lpp->col_stat[info->q] = LPX_NL;
               goto nl;
            }
            else
            {  /* lambda[q] < 0; set the column on upper bound */
               lpp->col_stat[info->q] = LPX_NU;
               goto nu;
            }
            /* no break */
         default:
            xassert(0);
      }
      return;
}

/*----------------------------------------------------------------------
-- COLUMN SINGLETON (IMPLIED SLACK VARIABLE)
--
-- *Processing*
--
-- Let column q have the only non-zero constraint coefficient in row p,
-- which is an equality constraint:
--
--    y[p] =  sum a[p,j] * x[j] + a[p,q] * x[q] = b[p],              (1)
--           j!=q
--
--    l[q] <= x[q] <= u[q].                                          (2)
--
-- The term a[p,q] * x[q] can be considered as a slack variable of the
-- row p, that allows removing the column q from the problem.
--
-- From (1) it follows that:
--
--    sum a[p,j] * x[j] = b[p] - a[p,q] * x[q].                      (3)
--   j!=q
--
-- So, (1) can be replaced by the following equivalent constraint:
--
--    l' <= y' =  sum a[p,j] * x[j] <= u',                           (4)
--               j!=q
--
-- where y' is an auxiliary variable of this new constraint, and
--
--    if a[p,q] > 0:   l' = b[p] - a[p,q] * u[q],                    (5)
--                     u' = b[p] - a[p,q] * l[q],
--
--    if a[p,q] < 0:   l' = b[p] - a[p,q] * l[q],                    (6)
--                     u' = b[p] - a[p,q] * u[q].
--
-- On removing x[q] from the problem it also should be substituted in
-- the objective function. From (3) it follows that:
--
--    x[q] = - sum (a[p,j] / a[p,q]) * x[j] + b[p] / a[p,q],         (7)
--            j!=q
--
-- and substituting x[q] in the objective function gives:
--
--    Z = sum c[j] * x[j] + c[0] =
--         j
--
--      = sum c[j] * x[j] + c[q] * x[q] + c[0] =                     (8)
--       j!=q
--
--      = sum c'[j] * x[j] + c'[0],
--       j!=q
--
-- where
--
--    c'[j] = c[j] - c[q] * (a[p,j] / a[p,q]),                       (9)
--
--    c'[0] = c[0] + c[q] * (b[p] / a[p,q]).                        (10)
--
-- *Recovering*
--
-- On entry to the recovering routine the row (4) that corresponds to
-- the row p is already recovered, i.e. its status, primal value y' and
-- dual value pi' are known.
--
-- From (3) and (4) it follows that
--
--    y' = b[p] - a[p,q] * x[q].                                    (11)
--
-- Therefore the status of the column q is determined as follows:
--
-- if y' is basic, x[q] is basic;
--
-- if y' is non-basic on lower bound, x[q] is non-basic on upper (if
-- a[p,q] > 0) or lower (if a[p,q] < 0) bound;
--
-- if y' is non-basic on upper bound, x[q] is non-basic on lower (if
-- a[p,q] > 0) or upper (of a[p,q] < 0) bound.
--
-- The primal and dual values of the column q are computed as follows:
--
--    x[q] = (b[p] - y') / a[p,q],                                  (12)
--
--    lambda[q] = - a[p,q] * pi',                                   (13)
--
-- where y' and pi' are primal and dual values of the row (4), resp.
--
-- Being an equality constraint the row p is active.
--
-- From (1) it follows that
--
--    y[p] = y' + a[p,q] * x[q] = b[p],                             (14)
--
-- which is the formula to compute the primal value of the row p.
--
-- The dual equality constraint for the row p is:
--
--    a[p,q] * pi[p] + lambda[q] = c[q],                            (15)
--
-- therefore
--
--    pi[p] = (c[q] - lambda[q]) / a[p,q],                          (16)
--
-- which is the formula to compute the dual value of the row p. */

typedef struct
{     /* column singleton (implied slack variable) */
      int p;
      /* row reference number */
      int q;
      /* column reference number */
      double rhs;
      /* b[p], right-hand side of the row */
      double c;
      /* c[q], objective coefficient of the column */
      double apq;
      /* a[p,q], constraint coefficient */
} COL_SNGTON1;

static void process_col_sngton1(LPP *lpp, LPPCOL *col)
{     COL_SNGTON1 *info;
      LPPROW *row;
      LPPAIJ *aij;
      double lb, ub, eps;
      /* the column must have the only non-zero constraint coefficient
         in the corresponding row */
      xassert(col->ptr != NULL && col->ptr->c_next == NULL);
      /* the corresponding row must be equality constraint */
      aij = col->ptr;
      row = aij->row;
      xassert(row->lb == row->ub);
      /* if the column is fixed, it can be just substituted and removed
         from the problem */
      if (col->lb == col->ub)
      {  process_fixed_col(lpp, col);
         goto done;
      }
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_COL_SNGTON1, sizeof(COL_SNGTON1));
      info->p = row->i;
      info->q = col->j;
      info->rhs = row->lb;
      info->c = col->c;
      info->apq = aij->val;
      /* compute new bounds l' and u' of the row; see (4)--(6) */
      if (info->apq > 0.0)
      {  lb = (col->ub ==
            +DBL_MAX ? -DBL_MAX : info->rhs - info->apq * col->ub);
         ub = (col->lb ==
            -DBL_MAX ? +DBL_MAX : info->rhs - info->apq * col->lb);
      }
      else
      {  lb = (col->lb ==
            -DBL_MAX ? -DBL_MAX : info->rhs - info->apq * col->lb);
         ub = (col->ub ==
            +DBL_MAX ? +DBL_MAX : info->rhs - info->apq * col->ub);
      }
      row->lb = lb;
      row->ub = ub;
      /* if new bounds of the row are close to each other, the row can
         be treated as equality constraint */
      if (row->lb != -DBL_MAX && row->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(row->lb));
         if (fabs(row->lb - row->ub) <= eps)
         {  if (fabs(row->lb) <= fabs(row->ub))
               row->ub = row->lb;
            else
               row->lb = row->ub;
         }
      }
      /* remove the column from the problem */
      lpp_remove_col(lpp, col);
      /* substitute x[q] into the objective function; see (7)--(10) */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
         aij->col->c -= info->c * (aij->val / info->apq);
      lpp->c0 += info->c * (info->rhs / info->apq);
done: return;
}

static void recover_col_sngton1(LPP *lpp, COL_SNGTON1 *info)
{     /* the (modified) row is already recovered */
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] != 0);
      /* while the column is not recovered yet */
      xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] == 0);
      /* recover the column status */
      switch (lpp->row_stat[info->p])
      {  case LPX_BS:
            /* y' is basic */
            lpp->col_stat[info->q] = LPX_BS;
            break;
         case LPX_NL:
nl:         /* y' is non-basic on lower bound */
            lpp->col_stat[info->q] =
               (info->apq > 0.0 ? LPX_NU : LPX_NL);
            break;
         case LPX_NU:
nu:         /* y' is non-basic on upper bound */
            lpp->col_stat[info->q] =
               (info->apq > 0.0 ? LPX_NL : LPX_NU);
            break;
         case LPX_NF:
            /* y' is non-basic free */
            xassert(lpp != lpp /* this case is not tested yet */);
            lpp->col_stat[info->q] = LPX_NF;
            break;
         case LPX_NS:
            /* y' is non-basic fixed; this can only mean the row was
               turned to equality constraint, because if the column is
               of fixed type, this transformation entry is not created;
               thus, we need to consider the row as ranged, whose lower
               and upper bounds are equal to each other, and choose an
               appropriate bound using the dual value pi[p] */
#if 0
            xassert(lpp != lpp /* this case is not tested yet */);
#endif
            if (lpp->row_dual[info->p] >= 0.0) goto nl; else goto nu;
            /* no break */
         default:
            xassert(lpp != lpp);
      }
      /* recover primal value x[q] using the formula (12) */
      lpp->col_prim[info->q] =
         (info->rhs - lpp->row_prim[info->p]) / info->apq;
      /* recover dual value lambda[q] using the formula (13) */
      lpp->col_dual[info->q] = - info->apq * lpp->row_dual[info->p];
      /* the row is active equality constraint */
      lpp->row_stat[info->p] = LPX_NS;
      /* recover primal value y[p] using the formula (14) */
      lpp->row_prim[info->p] = info->rhs;
      /* recover dual value pi[p] using the formula (15) */
      lpp->row_dual[info->p] =
         (info->c - lpp->col_dual[info->q]) / info->apq;
      return;
}

/*----------------------------------------------------------------------
-- COLUMN SINGLETON (IMPLIED FREE VARIABLE)
--
-- *Processing*
--
-- Let column q have the only non-zero constraint coefficient in row p,
-- which is an inequality constraint:
--
--    l[p] <= y[p] =  sum a[p,j] * x[j] + a[p,q] * x[q] <= u[p],     (1)
--                   j!=q
--
--    l[q] <= x[q] <= u[q].                                          (2)
--
-- Resolving (1) through x[q] we have:
--
--    x[q] = a'[p,q] * y[p] + sum a'[p,j] * x[j],                    (3)
--                           j!=q
--
-- where
--
--    a'[p,q] = 1 / a[p,q],                                          (4)
--
--    a'[p,j] = - a[p,j] / a[p,q],                                   (5)
--
-- The formula (3) allows computing implied lower and upper bounds of
-- x[q] using explicit bounds of y[p] and x[j]. Let these impled bounds
-- of x[q] are l' and u'. If l' >= l[q] and u' <= u[q], own bounds of
-- x[q] are never reached, in which case x[q] can be considered as an
-- implied free variable.
--
-- Let x[q] be a free variable. Then it is always basic, and its dual
-- value lambda[q] is zero. If x[q] is basic, y[p] must be non-basic
-- (otherwise there were two linearly dependent columns in the basic
-- matrix). The dual value pi[p] can be found from the dual equality
-- constraint for the column q:
--
--    a[p,q] * pi[p] + lambda[q] = c[q],                             (6)
--
-- that gives:
--
--    pi[p] = (c[q] - lambda[q]) / a[p,q] = c[q] / a[p,q].           (7)
--
-- If pi[p] > 0, the row p must be active on its lower bound l[p], and
-- if the row p has no lower bound, it is dual infeasible. Analogously,
-- if pi[p] < 0, the row p must be active on its upper bound u[p], and
-- if the row p has no upper bound, it is dual infeasible. Thus, in both
-- cases the row p becomes an equality constraint, whose right-hand side
-- is l[p] or u[p] depending on the sign of pi[p], while the column q
-- becomes an implied slack variable. If pi[p] = 0 (i.e. if c[q] = 0),
-- the row p can be fixed at any bound.
--
-- *Recovering*
--
-- The only thing needed on recovering is to properly set the status of
-- the row p, because on entry to the recovering routine this row is an
-- active equality constraint while actually it is an active inequality
-- constraint. */

typedef struct
{     /* column singleton (implied free variable) */
      int p;
      /* row reference number */
      int q;
      /* column reference number */
      int stat;
      /* row status:
         LPX_NL - active constraint on lower bound
         LPX_NU - active constraint on upper bound */
} COL_SNGTON2;

static int process_col_sngton2(LPP *lpp, LPPCOL *col)
{     COL_SNGTON2 *info;
      LPPROW *row;
      LPPAIJ *apq, *aij;
      double lb, ub, eps, temp;
      /* the column must have the only non-zero constraint coefficient
         in the corresponding row */
      xassert(col->ptr != NULL && col->ptr->c_next == NULL);
      /* the corresponding row must be inequality constraint */
      apq = col->ptr;
      row = apq->row;
      xassert(row->lb != row->ub);
      /* if the column is fixed, it can be just substituted and removed
         from the problem */
      if (col->lb == col->ub)
      {  process_fixed_col(lpp, col);
         goto done;
      }
      /* if the row is free, it can be just removed from the problem */
      if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
      {  process_free_row(lpp, row);
         goto done;
      }
      /* compute l', an implied lower bound of x[q]; see (3)--(5) */
      temp = 1.0 / apq->val;
      if (temp > 0.0)
         lb = (row->lb == -DBL_MAX ? -DBL_MAX : temp * row->lb);
      else
         lb = (row->ub == +DBL_MAX ? -DBL_MAX : temp * row->ub);
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  if (lb == -DBL_MAX) break;
         if (aij == apq) continue;
         temp = - aij->val / apq->val;
         if (temp > 0.0)
         {  if (aij->col->lb == -DBL_MAX)
               lb = -DBL_MAX;
            else
               lb += temp * aij->col->lb;
         }
         else
         {  if (aij->col->ub == +DBL_MAX)
               lb = -DBL_MAX;
            else
               lb += temp * aij->col->ub;
         }
      }
      /* compute u', an implied upper bound of x[q]; see (3)--(5) */
      temp = 1.0 / apq->val;
      if (temp > 0.0)
         ub = (row->ub == +DBL_MAX ? +DBL_MAX : temp * row->ub);
      else
         ub = (row->lb == -DBL_MAX ? +DBL_MAX : temp * row->lb);
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  if (ub == +DBL_MAX) break;
         if (aij == apq) continue;
         temp = - aij->val / apq->val;
         if (temp > 0.0)
         {  if (aij->col->ub == +DBL_MAX)
               ub = +DBL_MAX;
            else
               ub += temp * aij->col->ub;
         }
         else
         {  if (aij->col->lb == -DBL_MAX)
               ub = +DBL_MAX;
            else
               ub += temp * aij->col->lb;
         }
      }
      /* check if x[q] can reach its own lower bound */
      if (col->lb != -DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(col->lb));
         if (lb < col->lb - eps) goto done; /* yes, it can */
      }
      /* check if x[q] can reach its own upper bound */
      if (col->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 * fabs(col->ub));
         if (ub > col->ub + eps) goto done; /* yes, it can */
      }
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_COL_SNGTON2, sizeof(COL_SNGTON2));
      info->p = row->i;
      info->q = col->j;
      info->stat = 0;
      /* x[q] is implied free variable */
      col->lb = -DBL_MAX, col->ub = +DBL_MAX;
      /* since x[q] is always basic, the row p must be active; compute
         its dual value pi[p]; see (7) */
      temp = col->c / apq->val;
      /* analyze the dual value pi[p] */
      if (temp > 0.0)
      {  /* pi[p] > 0; the row p must be active on its lower bound */
         if (row->lb == -DBL_MAX) return 1; /* dual infeasibility */
         info->stat = LPX_NL;
         row->ub = row->lb;
      }
      else if (temp < 0.0)
      {  /* pi[p] < 0; the row p must be active on its upper bound */
         if (row->ub == +DBL_MAX) return 1; /* dual infeasibility */
         info->stat = LPX_NU;
         row->lb = row->ub;
      }
      else
      {  /* pi[p] = 0; the row must be active on any bound */
         if (row->lb != -DBL_MAX)
         {  info->stat = LPX_NL;
            row->ub = row->lb;
         }
         else
         {  xassert(row->ub != +DBL_MAX);
            info->stat = LPX_NU;
            row->lb = row->ub;
         }
      }
      /* now x[q] can be considered as an implied slack variable */
      process_col_sngton1(lpp, col);
done: return 0;
}

static void recover_col_sngton2(LPP *lpp, COL_SNGTON2 *info)
{     /* the (modified) row is already recovered and must be an active
         equality constraint */
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == LPX_NS);
      /* the (modified) column is already recovered and must be a basic
         variable */
      xassert(1 <= info->q && info->q <= lpp->ncols);
      xassert(lpp->col_stat[info->q] == LPX_BS);
      /* set proper status of the row */
      lpp->row_stat[info->p] = info->stat;
      return;
}

/*----------------------------------------------------------------------
-- FORCING ROW
--
-- *Processing*
--
-- Let row p be of general kind:
--
--    l[p] <= y[p] = sum a[p,j] * x[j] <= u[p],                      (1)
--                    j
--
--    l[j] <= x[j] <= u[j].                                          (2)
--
-- If l[p] = u' or u[p] = l', where l' and u' are implied lower and
-- upper bounds of the row (see (3) and (4) in comments to the routine
-- analyze_row), the constraint forces the corresponding variables x[j]
-- to be set on their bounds in order to provide primal feasibility:
--
-- if l[p] = u' and a[p,j] > 0 or if u[p] = l' and a[p,j] < 0, x[j] can
-- only be set on its upper bound u[j], and
--
-- if l[p] = u' and a[p,j] < 0 or if u[p] = l' and a[p,j] > 0, x[j] can
-- only be set on its lower bound l[j].
--
-- Being set on their bounds the variables x[j] become fixed and can be
-- removed from the problem. At the same time the row becomes redundant
-- and therefore can also be removed from the problem.
--
-- *Recovering*
--
-- On entry to the recovering routine all x[j] are non-basic and fixed,
-- while the row p is still removed.
--
-- Since the corresponding basic solution is assumed to be optimal, the
-- following dual equality constraints are satisfied:
--
--    sum a[i,j] * pi[i] + lambda'[j] = c[j],                        (3)
--   i!=p
--
-- where lambda'[j] is a dual value of the column j on entry to the
-- recovering routine.
--
-- On recovering the row p an additional term that corresponds to this
-- row appears in the dual constraints (3):
--
--    sum a[i,j] * pi[i] + a[p,j] * pi[p] + lambda[j] = c[j],        (4)
--   i!=p
--
-- where pi[p] is recovered dual value of the row p, and lambda[j] is
-- recovered dual value of the column j.
--
-- From (3) and (4) it follows that:
--
--    lambda[j] = lambda'[j] - a[p,j] * pi[p].                       (5)
--
-- Now note that actually x[j] are non-fixed. Therefore, the following
-- dual feasibility condition must be satisfied for all x[j]:
--
--    lambda[j] >= 0, if x[j] was forced on its lower bound l[j],    (6)
--
--    lambda[j] <= 0, if x[j] was forced on its upper bound u[j].    (7)
--
-- Let the row p is non-active (i.e. y[p] is basic). Then pi[p] = 0 and
-- therefore lambda[j] = lambda'[j]. If the conditions (6)--(7) are
-- satisfied for all x[j], recovering is finished. Otherwise, we should
-- change (increase or decrease) pi[p] while at least one lambda[j] in
-- (5) has wrong sign. (Note that it is *always* possible to attain dual
-- feasibility by changing pi[p].) Once signs of all lambda[j] become
-- correct, there is always some lambda[q], which reaches zero last. In
-- this case the row p is active (i.e. y[p] is non-basic) with non-zero
-- dual value pi[p], all columns (except the column q) are non-basic
-- with dual values lambda[j], which are computed using the formula (5),
-- and the column q is basic with zero dual value lambda[q]. Should also
-- note that due to primal degeneracy changing pi[p] doesn't affect any
-- primal values y[p] and x[j]. */

typedef struct
{     /* forcing row */
      int p;
      /* reference number of a forcing row */
      int stat;
      /* status assigned to the row if it becomes active constraint:
         LPX_NS - equality constraint
         LPX_NL - inequality constraint on lower bound
         LPX_NU - inequality constraint on upper bound */
      double bnd;
      /* primal value of the row (one of its bounds) */
      LPPLFX *ptr;
      /* list of non-zero coefficients a[p,j] with additional flags of
         the corresponding variables x[j]:
         LPX_NL - x[j] is forced on its lower bound
         LPX_NU - x[j] is forced on its upper bound */
} FORCING_ROW;

static void process_forcing_row(LPP *lpp, LPPROW *row, int at)
{     FORCING_ROW *info;
      LPPCOL *col;
      LPPAIJ *aij, *next_aij;
      LPPLFX *lfx;
      /* if there are fixed columns in the row, they can be substituted
         and thus removed from the problem */
      for (aij = row->ptr; aij != NULL; aij = next_aij)
      {  col = aij->col;
         next_aij = aij->r_next;
         if (col->lb == col->ub) process_fixed_col(lpp, col);
      }
      /* the row may be empty; in this case it being redundant can just
         be removed from the problem */
      if (row->ptr == NULL)
      {  row->lb = -DBL_MAX, row->ub = +DBL_MAX;
         xassert(process_empty_row(lpp, row) == 0);
         goto done;
      }
      /* create transformation queue entry */
      info = lpp_append_tqe(lpp, LPP_FORCING_ROW, sizeof(FORCING_ROW));
      info->p = row->i;
      if (row->lb == row->ub)
      {  /* equality constraint */
         info->stat = LPX_NS;
         info->bnd = row->lb;
      }
      else if (at == 0)
      {  /* inequality constraint; the case l[p] = u' */
         info->stat = LPX_NL;
         xassert(row->lb != -DBL_MAX);
         info->bnd = row->lb;
      }
      else
      {  /* inequality constraint; the case u[p] = l' */
         info->stat = LPX_NU;
         xassert(row->ub != +DBL_MAX);
         info->bnd = row->ub;
      }
      info->ptr = NULL;
      /* formally the row p is removed from the problem at this point;
         however, its constraint coefficients are still needed, so its
         physical removal will be performed later */
      /* walk through the list of constraint coefficients of the row,
         save them, and fix the corresponding columns at appropriate
         bounds */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         /* save the constraint coefficient a[p,j] */
         lfx = dmp_get_atomv(lpp->tqe_pool, sizeof(LPPLFX));
         lfx->ref = col->j;
         lfx->flag = 0; /* will be set a bit later */
         lfx->val = aij->val;
         lfx->next = info->ptr;
         info->ptr = lfx;
         /* there must be no fixed columns */
         xassert(col->lb != col->ub);
         /* fix the column j at its lower or upper bound */
         if (at == 0 && aij->val < 0.0 || at != 0 && aij->val > 0.0)
         {  /* fix at lower bound */
            lfx->flag = LPX_NL;
            xassert(col->lb != -DBL_MAX);
            col->ub = col->lb;
         }
         else
         {  /* fix at upper bound */
            lfx->flag = LPX_NU;
            xassert(col->ub != +DBL_MAX);
            col->lb = col->ub;
         }
         /* before removing the column j from the problem we need to
            remove a[p,j] from the column list, because the row p is
            formally removed */
         if (aij->c_prev == NULL)
            aij->col->ptr = aij->c_next;
         else
            aij->c_prev->c_next = aij->c_next;
         if (aij->c_next == NULL)
            ;
         else
            aij->c_next->c_prev = aij->c_prev;
         /* remove the column j from the problem */
         process_fixed_col(lpp, col);
      }
      /* remove all a[p,j] from the row p */
      while (row->ptr != NULL)
      {  /* get a next element in the row */
         aij = row->ptr;
         /* remove it from the row list */
         row->ptr = aij->r_next;
         /* and return it to its pool */
         dmp_free_atom(lpp->aij_pool, aij, sizeof(LPPAIJ));
      }
      /* physically remove the row p (now it is empty) */
      lpp_remove_row(lpp, row);
done: return;
}

static void recover_forcing_row(LPP *lpp, FORCING_ROW *info)
{     LPPLFX *lfx, *that;
      double big, lambda, pi, temp;
      /* the row is not recovered yet */
      xassert(1 <= info->p && info->p <= lpp->nrows);
      xassert(lpp->row_stat[info->p] == 0);
      /* while all the corresponding columns are already recovered;
         they were fixed, so currently all they must be non-basic */
      for (lfx = info->ptr; lfx != NULL; lfx = lfx->next)
      {  xassert(1 <= lfx->ref && lfx->ref <= lpp->ncols);
         xassert(lpp->col_stat[lfx->ref] == LPX_NS);
      }
      /* choose a column q, whose dual value lambda[q] has wrong sign
         and reaches zero value last on changing pi[p] */
      that = NULL, big = 0.0;
      for (lfx = info->ptr; lfx != NULL; lfx = lfx->next)
      {  lambda = lpp->col_dual[lfx->ref];
         temp = fabs(lambda / lfx->val);
         switch (lfx->flag)
         {  case LPX_NL:
               /* x[j] >= l[j], therefore lambda[j] >= 0 */
               if (lambda < 0.0 && big < temp) that = lfx, big = temp;
               break;
            case LPX_NU:
               /* x[j] <= u[j], therefore lambda[j] <= 0 */
               if (lambda > 0.0 && big < temp) that = lfx, big = temp;
               break;
            default:
               xassert(lfx != lfx);
         }
      }
      /* recover the row p and all the corresponding columns */
      if (that == NULL)
      {  /* the row p is inactive; all columns are non-basic */
         lpp->row_stat[info->p] = LPX_BS;
         lpp->row_prim[info->p] = info->bnd;
         lpp->row_dual[info->p] = 0.0;
         for (lfx = info->ptr; lfx != NULL; lfx = lfx->next)
            lpp->col_stat[lfx->ref] = lfx->flag;
      }
      else
      {  /* the row p is active, the column q is basic, and all other
            columns are non-basic */
         /* compute dual value pi[p] = lambda'[q] / a[p,q]; see (5) */
         pi = lpp->col_dual[that->ref] / that->val;
         /* recover the row p */
         lpp->row_stat[info->p] = info->stat;
         lpp->row_prim[info->p] = info->bnd;
         lpp->row_dual[info->p] = pi;
         /* recover the corresponding columns */
         for (lfx = info->ptr; lfx != NULL; lfx = lfx->next)
         {  if (lfx == that)
            {  /* recover the column q, which is basic */
               lpp->col_stat[lfx->ref] = LPX_BS;
               lpp->col_dual[lfx->ref] = 0.0;
            }
            else
            {  /* recover some column j, which is non-basic */
               lpp->col_stat[lfx->ref] = lfx->flag;
               lpp->col_dual[lfx->ref] -= lfx->val * pi;
            }
         }
      }
      return;
}

/*----------------------------------------------------------------------
-- GENERAL ROW ANALYSIS
--
-- Let row p be of general kind:
--
--    l[p] <= y[p] = sum a[p,j] * x[j] <= u[p],                      (1)
--                    j
--
--    l[j] <= x[j] <= u[j].                                          (2)
--
-- The analysis is based on implied lower and upper bounds l' and u' of
-- the primal value y[p]:
--
--                      ( l[j], if a[p,j] > 0 )
--    l' = sum a[p,j] * <                     > ,                    (3)
--          j           ( u[j], if a[p,j] < 0 )
--
--                      ( u[j], if a[p,j] > 0 )
--    u' = sum a[p,j] * <                     > .                    (4)
--          j           ( l[j], if a[p,j] < 0 )
--
-- If l' > u[p] or u' < l[p], the row is primal infeasible.
--
-- If l' = u[p], all the variables x[j] with non-zero coefficients in
-- the row p can be fixed on their bounds as follows:
--
--    if a[p,j] > 0, x[j] is fixed on l[j];                          (5)
--
--    if a[p,j] < 0, x[j] is fixed on u[j].                          (6)
--
-- If u' = l[p], all the variables x[j] with non-zero coefficients in
-- the row p can be fixed on their bounds as follows:
--
--    if a[p,j] > 0, x[j] is fixed on u[j];                          (7)
--
--    if a[p,j] < 0, x[j] is fixed on l[j].                          (8)
--
-- In both cases l' = u[p] and u' = l[p] after fixing variables the row
-- becomes redundant and therefore can be removed from the problem.
--
-- If l' > l[p], the row cannot be active on its lower bound, so the
-- lower bound can be removed. Analogously, If u' < u[p], the row cannot
-- be active on its upper bound, so the upper bound can be removed. If
-- both lower and upper bounds have been removed, the row becomes free
-- and therefore itself can be removed from the problem. */

static int analyze_row(LPP *lpp, LPPROW *row)
{     LPPCOL *col;
      LPPAIJ *aij;
      double lb, ub, eps;
      /* compute the implied lower bound l'; see (3) */
      lb = 0.0;
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  if (lb == -DBL_MAX) break;
         col = aij->col;
         if (aij->val > 0.0)
         {  if (col->lb == -DBL_MAX)
               lb = -DBL_MAX;
            else
               lb += aij->val * col->lb;
         }
         else
         {  if (col->ub == +DBL_MAX)
               lb = -DBL_MAX;
            else
               lb += aij->val * col->ub;
         }
      }
      /* compute the implied upper bound u'; see (4) */
      ub = 0.0;
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  if (ub == +DBL_MAX) break;
         col = aij->col;
         if (aij->val > 0.0)
         {  if (col->ub == +DBL_MAX)
               ub = +DBL_MAX;
            else
               ub += aij->val * col->ub;
         }
         else
         {  if (col->lb == -DBL_MAX)
               ub = +DBL_MAX;
            else
               ub += aij->val * col->lb;
         }
      }
      /* check for primal infeasibility */
      if (row->lb != -DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(row->lb));
         if (ub < row->lb - eps) return 1;
      }
      if (row->ub != +DBL_MAX)
      {  eps = 1e-5 * (1.0 + fabs(row->ub));
         if (lb > row->ub + eps) return 1;
      }
      /* check if the row is implicitly fixed on its lower bound */
      if (row->lb != -DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(row->lb));
         if (ub <= row->lb + eps)
         {  process_forcing_row(lpp, row, 0);
            goto done;
         }
      }
      /* check if the row is implicitly fixed on its upper bound */
      if (row->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(row->ub));
         if (lb >= row->ub - eps)
         {  process_forcing_row(lpp, row, 1);
            goto done;
         }
      }
      /* check whether the row can reach its lower bound */
      if (row->lb != -DBL_MAX)
      {  eps = 1.001e-7 * (1.0 + fabs(row->lb));
         if (lb >= row->lb - eps)
         {  /* the row cannot reach its lower bound, so the lower bound
               is redundant and therefore can be removed (note that the
               row cannot be an equality constraint at this point, since
               the case l' > u[p] = l[p] would be processed above */
            xassert(row->lb != row->ub);
            row->lb = -DBL_MAX;
            lpp_enque_row(lpp, row);
         }
      }
      /* check whether the row can reach its upper bound */
      if (row->ub != +DBL_MAX)
      {  eps = 1.001e-7 * (1.0 + fabs(row->ub));
         if (ub <= row->ub + eps)
         {  /* the row cannot reach its upper bound, so the upper bound
               is redundant and therefore can be removed (note that the
               row cannot be an equality constraint at this point, since
               the case u' < l[p] = u[p] would be processed above */
            xassert(row->lb != row->ub);
            row->ub = +DBL_MAX;
            lpp_enque_row(lpp, row);
         }
      }
done: return 0;
}

/*----------------------------------------------------------------------
-- lpp_presolve - LP presolve analysis.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- int lpp_presolve(LPP *lpp);
--
-- *Description*
--
-- The routine lpp_presolve performs a presolve analysis and transforms
-- the original problem to a resultant problem.
--
-- *Returns*
--
-- The routine returns one of the following codes:
--
-- 0 - neither primal nor dual infeasibility have been detected;
-- 1 - the original problem is primal infeasible;
-- 2 - the original problem is dual infeasible.
--
-- If the return code is non-zero, the resultant problem is invalid. */

int lpp_presolve(LPP *lpp)
{     LPPROW *row;
      LPPCOL *col;
      /* the presolver uses two queues (organized in the form of doubly
         liked lists): one for active rows and other for active columns;
         initially these queues contain all rows and columns of the
         original problem; once a next active row or column has been
         selected for processing, it is removed from the corresponding
         queue; if a transformation applied to the selected row/column
         may affect other rows/columns, the latters are placed in the
         queue for the repeated processing */
      while (lpp->row_que != NULL || lpp->col_que != NULL)
      {  /* process active rows */
         while (lpp->row_que != NULL)
         {  /* select a next active row */
            row = lpp->row_que;
            /* and remove it from the queue */
            lpp_deque_row(lpp, row);
            /* process the row */
            if (row->ptr == NULL)
            {  /* remove empty row */
               if (process_empty_row(lpp, row)) return 1;
            }
            else if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
            {  /* remove free row */
               process_free_row(lpp, row);
            }
            else if (row->ptr != NULL && row->ptr->r_next == NULL &&
                     row->lb == row->ub)
            {  /* remove row singleton (equality constraint) */
               if (process_row_sngton1(lpp, row)) return 1;
            }
            else if (row->ptr != NULL && row->ptr->r_next == NULL &&
                     row->lb != row->ub)
            {  /* remove row singleton (inequality constraint) */
               if (process_row_sngton2(lpp, row)) return 1;
            }
            else
            {  /* general row analysis */
               if (analyze_row(lpp, row)) return 1;
            }
         }
         /* process active columns */
         while (lpp->col_que != NULL)
         {  /* select a next active column */
            col = lpp->col_que;
            /* and remove it from the queue */
            lpp_deque_col(lpp, col);
            /* process the column */
            if (col->ptr == NULL)
            {  /* remove empty column */
               if (process_empty_col(lpp, col)) return 2;
            }
            else if (col->lb == col->ub)
            {  /* remove fixed column */
               process_fixed_col(lpp, col);
            }
            else if (col->ptr != NULL && col->ptr->c_next == NULL &&
                     col->ptr->row->lb == col->ptr->row->ub)
            {  /* remove column singleton (implied slack variable) */
               process_col_sngton1(lpp, col);
            }
            else if (col->ptr != NULL && col->ptr->c_next == NULL &&
                     col->ptr->row->lb != col->ptr->row->ub)
            {  /* remove column singleton (implied free variable) */
               if (process_col_sngton2(lpp, col)) return 2;
            }
            else
            {  /* general column analysis */
               /* (not implemented yet) */
            }
         }
      }
      return 0;
}

/*----------------------------------------------------------------------
-- lpp_postsolve - LP postsolve processing.
--
-- *Synopsis*
--
-- #include "glplpp.h"
-- void lpp_postsolve(LPP *lpp);
--
-- *Description*
--
-- The routine lpp_postsolve performs a postsolve processing to recover
-- a solution of the original problem. It is assumed that a solution of
-- the resultant problem is loaded into the presolver workspace. */

void lpp_postsolve(LPP *lpp)
{     LPPTQE *tqe;
      for (tqe = lpp->tqe_list; tqe != NULL; tqe = tqe->next)
      {  switch (tqe->type)
         {  case LPP_EMPTY_ROW:
               recover_empty_row(lpp, tqe->info);
               break;
            case LPP_EMPTY_COL:
               recover_empty_col(lpp, tqe->info);
               break;
            case LPP_FREE_ROW:
               recover_free_row(lpp, tqe->info);
               break;
            case LPP_FIXED_COL:
               recover_fixed_col(lpp, tqe->info);
               break;
            case LPP_ROW_SNGTON1:
               recover_row_sngton1(lpp, tqe->info);
               break;
            case LPP_ROW_SNGTON2:
               recover_row_sngton2(lpp, tqe->info);
               break;
            case LPP_COL_SNGTON1:
               recover_col_sngton1(lpp, tqe->info);
               break;
            case LPP_COL_SNGTON2:
               recover_col_sngton2(lpp, tqe->info);
               break;
            case LPP_FORCING_ROW:
               recover_forcing_row(lpp, tqe->info);
               break;
            default:
               xassert(tqe->type != tqe->type);
         }
      }
      return;
}

/* eof */
