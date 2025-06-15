/* glpipp02.c */

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

/*----------------------------------------------------------------------
-- FREE ROW
--
-- Let row p be free, i.e. it has no bounds:
--
--    -inf < y[p] = sum a[p,j] * x[j] < +inf.                        (1)
--
-- The corresponding "constraint" can never be active, so the free row
-- is redundant and can be removed from the problem.
--
-- RETURNS
--
-- None. */

void ipp_free_row(IPP *ipp, IPPROW *row)
{     /* process free row */
      IPPAIJ *aij;
      /* the row must be free */
      xassert(row->lb == -DBL_MAX && row->ub == +DBL_MAX);
      /* activate corresponding columns for further processing */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
         ipp_enque_col(ipp, aij->col);
      /* remove the row from the problem */
      ipp_remove_row(ipp, row);
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
-- RECOVERING
--
-- Value of x[q] is determined using the formula (1).
--
-- RETURNS
--
-- None. */

struct fixed_col
{     /* fixed column */
      int q;
      /* reference number of a fixed column */
      double s;
      /* value at which the column is fixed */
};

void ipp_fixed_col(IPP *ipp, IPPCOL *col)
{     /* process fixed column */
      struct fixed_col *info;
      IPPROW *row;
      IPPAIJ *aij;
      double temp;
      /* the column must be fixed */
      xassert(col->lb == col->ub);
      /* create transformation queue entry */
      info = ipp_append_tqe(ipp, IPP_FIXED_COL, sizeof(*info));
      info->q = col->j;
      info->s = col->lb;
      /* update bounds of corresponding rows and activate them for
         further processing */
      for (aij = col->ptr; aij != NULL; aij = aij->c_next)
      {  row = aij->row;
         /* see (4) and (5) */
         temp = aij->val * info->s;
         if (row->lb == row->ub)
         {  row->lb -= temp;
            row->ub = row->lb;
         }
         else
         {  if (row->lb != -DBL_MAX) row->lb -= temp;
            if (row->ub != +DBL_MAX) row->ub -= temp;
         }
         ipp_enque_row(ipp, row);
      }
      /* update the constant term of the objective function */
      ipp->c0 += col->c * info->s;
      /* remove the column from the problem */
      ipp_remove_col(ipp, col);
      return;
}

void ipp_fixed_col_r(IPP *ipp, void *_info)
{     /* recover fixed column */
      struct fixed_col *info = _info;
      xassert(1 <= info->q && info->q <= ipp->ncols);
      xassert(ipp->col_stat[info->q] == 0);
      ipp->col_stat[info->q] = 1;
      ipp->col_mipx[info->q] = info->s;
      return;
}

/*----------------------------------------------------------------------
-- EMPTY ROW
--
-- Let row p be empty, i.e. all its coefficients are zero:
--
--    l[p] <= y[p] = 0 <= u[p].                                      (1)
--
-- If l[p] <= 0 and u[p] >= 0, the row is redundant and therefore can
-- be removed from the problem. Otherwise, if l[p] > 0 or u[p] < 0, the
-- row is primal infeasible.
--
-- RETURNS
--
-- 0 - the row is primal feasible and has been removed
-- 1 - the row is primal infeasible */

int ipp_empty_row(IPP *ipp, IPPROW *row)
{     /* process empty row */
      double eps;
      /* the row must be empty */
      xassert(row->ptr == NULL);
      /* check for primal infeasibility */
      eps = 1e-5;
      if (row->lb > +eps || row->ub < -eps) return 1;
      /* make the row free */
      row->lb = -DBL_MAX, row->ub = +DBL_MAX;
      /* and activate it for further processing (removing) */
      ipp_enque_row(ipp, row);
      return 0;
}

/*----------------------------------------------------------------------
-- EMPTY COLUMN
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
-- RETURNS
--
-- 0 - the column is dual feasible and has been fixed and removed
-- 1 - the column is dual infeasible */

int ipp_empty_col(IPP *ipp, IPPCOL *col)
{     /* process empty column */
      double eps;
      /* the column must be empty */
      xassert(col->ptr == NULL);
      /* check for dual infeasibility */
      eps = 1e-5;
      if (col->c > +eps && col->lb == -DBL_MAX ||
          col->c < -eps && col->ub == +DBL_MAX) return 1;
      /* fix the column at proper bound */
      if (col->lb == -DBL_MAX && col->ub == +DBL_MAX)
      {  /* free variable */
         col->lb = col->ub = 0.0;
      }
      else if (col->ub == +DBL_MAX)
lo:   {  /* variable with lower bound */
         col->ub = col->lb;
      }
      else if (col->lb == -DBL_MAX)
up:   {  /* variable with upper bound */
         col->lb = col->ub;
      }
      else if (col->lb != col->ub)
      {  /* double bounded variable */
         if (col->c > 0.0) goto lo;
         if (col->c < 0.0) goto up;
         if (fabs(col->lb) <= fabs(col->ub)) goto lo; else goto up;
      }
      else
      {  /* fixed variable */
         /* nop */;
      }
      /* and activate it for further processing (removing) */
      ipp_enque_col(ipp, col);
      return 0;
}

/*----------------------------------------------------------------------
-- ROW SINGLETON
--
-- Let row p be a constraing having only one column:
--
--    l[p] <= y[p] = a[p,q] * x[q] <= u[p].                          (1)
--
-- In this case the row implies bounds of x[q]:
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
-- RETURNS
--
-- 0 - the row is primal feasible
-- 1 - the row is primal infeasible */

int ipp_row_sing(IPP *ipp, IPPROW *row)
{     /* process row singleton */
      IPPCOL *col;
      IPPAIJ *aij;
      double lb, ub;
      /* the row must have only one constraint coefficient */
      xassert(row->ptr != NULL && row->ptr->r_next == NULL);
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
      /* tight current column bounds */
      col = aij->col;
      switch (ipp_tight_bnds(ipp, col, lb, ub))
      {  case 0:
            /* bounds remain unchanged */
            break;
         case 1:
            /* bounds have been changed, therefore activate the column
               for further processing */
            ipp_enque_col(ipp, col);
            break;
         case 2:
            /* implied bounds are in conflict */
            return 1;
         default:
            xassert(ipp != ipp);
      }
      /* make the row free */
      row->lb = -DBL_MAX, row->ub = +DBL_MAX;
      /* and activate it for further processing (removing) */
      ipp_enque_row(ipp, row);
      return 0;
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
-- and therefore itself can be removed from the problem.
--
-- RETURNS
--
-- 0 - the row is primal feasible
-- 1 - the row is primal infeasible */

int ipp_analyze_row(IPP *ipp, IPPROW *row)
{     IPPCOL *col;
      IPPAIJ *aij;
      double lb, ub, eps, s;
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
         {  /* fix all the columns; see (7)-(8) */
            for (aij = row->ptr; aij != NULL; aij = aij->r_next)
            {  col = aij->col;
               s = (aij->val > 0.0 ? col->ub : col->lb);
               switch (ipp_tight_bnds(ipp, col, s, s))
               {  case 0:
                     /* bounds remain unchanged */
                     break;
                  case 1:
                     /* bounds have been changed, therefore activate
                        the column for further processing (removing) */
                     ipp_enque_col(ipp, col);
                     break;
                  case 2:
                     /* implied bounds are in conflict */
                     return 1;
                  default:
                     xassert(ipp != ipp);
               }
            }
            /* make the row free */
            row->lb = -DBL_MAX, row->ub = +DBL_MAX;
            /* and activate it for further processing (removing) */
            ipp_enque_row(ipp, row);
            goto done;
         }
      }
      /* check if the row is implicitly fixed on its upper bound */
      if (row->ub != +DBL_MAX)
      {  eps = 1e-7 * (1.0 + fabs(row->ub));
         if (lb >= row->ub - eps)
         {  /* fix all the columns; see (5)-(6) */
            for (aij = row->ptr; aij != NULL; aij = aij->r_next)
            {  col = aij->col;
               s = (aij->val > 0.0 ? col->lb : col->ub);
               switch (ipp_tight_bnds(ipp, col, s, s))
               {  case 0:
                     /* bounds remain unchanged */
                     break;
                  case 1:
                     /* bounds have been changed, therefore activate
                        the column for further processing (removing) */
                     ipp_enque_col(ipp, col);
                     break;
                  case 2:
                     /* implied bounds are in conflict */
                     return 1;
                  default:
                     xassert(ipp != ipp);
               }
            }
            /* make the row free */
            row->lb = -DBL_MAX, row->ub = +DBL_MAX;
            /* and activate it for further processing (removing) */
            ipp_enque_row(ipp, row);
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
            ipp_enque_row(ipp, row);
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
            ipp_enque_row(ipp, row);
         }
      }
done: return 0;
}

/*----------------------------------------------------------------------
-- GENERAL COLUMN ANALYSIS
--
-- Let column q be of general kind. Then its dual equality constraint
-- is the following:
--
--    sum a[i,q] * pi[i] + lambda[q] = c[q],                         (1)
--     i
--
-- from which it follows that:
--
--    lambda[q] = c[q] - sum a[i,q] * pi[i],                         (2)
--                        i
--
-- where c[q] is an objective coefficient of the column, a[i,q] are
-- constraints coefficents, pi[i] are dual values of the corresponding
-- rows.
--
-- If row i has no lower bound, pi[i] <= 0, and if row i has no upper
-- bound, pi[i] >= 0. This allows determining implied lower and upper
-- bounds of lambda[q].
--
-- If lambda[q] > 0, column q can be fixed on its lower bound, and if
-- lambda[q] < 0, column q can be fixed on its upper bound.
--
-- RETURNS
--
-- 0 - the column is dual feasible
-- 1 - the column is dual infeasible */

int ipp_analyze_col(IPP *ipp, IPPCOL *col)
{     IPPROW *row;
      IPPAIJ *aij;
      if (col->c > +1e-5)
      {  /* check if lambda[q] > 0 */
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
         {  row = aij->row;
            if (aij->val > 0.0)
            {  /* pi[i] <= 0 iff row i has no lower bound */
               if (row->lb != -DBL_MAX) goto done;
            }
            else
            {  /* pi[i] >= 0 iff row i has no upper bound */
               if (row->ub != +DBL_MAX) goto done;
            }
         }
         /* lambda[q] > 0; fix the column on its lower bound */
         if (col->lb == -DBL_MAX) return 1;
         ipp_tight_bnds(ipp, col, col->lb, col->lb);
         /* activate the column for further processing (removing) */
         ipp_enque_col(ipp, col);
      }
      else if (col->c < -1e-5)
      {  /* check if lambda[q] < 0 */
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
         {  row = aij->row;
            if (aij->val > 0.0)
            {  /* pi[i] >= 0 iff row i has no upper bound */
               if (row->ub != +DBL_MAX) goto done;
            }
            else
            {  /* pi[i] <= 0 iff row i has no lower bound */
               if (row->lb != -DBL_MAX) goto done;
            }
         }
         /* lambda[q] < 0; fix the column on its upper bound */
         if (col->ub == +DBL_MAX) return 1;
         ipp_tight_bnds(ipp, col, col->ub, col->ub);
         /* activate the column for further processing (removing) */
         ipp_enque_col(ipp, col);
      }
done: return 0;
}

/*----------------------------------------------------------------------
-- ipp_basic_tech - basic MIP presolve analysis.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- int ipp_basic_tech(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_basic_tech performs a basic MIP presolve analysis.
--
-- RETURNS
--
-- The routine returns one of the following codes:
--
-- 0 - neither primal nor dual infeasibility have been detected;
-- 1 - the original problem is primal infeasible;
-- 2 - the original problem is dual infeasible.
--
-- If the return code is non-zero, the resultant problem is invalid. */

int ipp_basic_tech(IPP *ipp)
{     IPPROW *row;
      IPPCOL *col;
      int nrows, ncols;
      /* the presolver uses two queues (organized in the form of doubly
         liked lists): one for active rows and other for active columns;
         initially these queues contain all rows and columns of the
         original problem; once a next active row or column has been
         selected for processing, it is removed from the corresponding
         queue; if a transformation applied to the selected row/column
         may affect other rows/columns, the latters are placed in the
         queue for the repeated processing */
      /* activate all rows and columns */
      nrows = 0;
      for (row = ipp->row_ptr; row != NULL; row = row->next)
         ipp_enque_row(ipp, row), nrows++;
      ncols = 0;
      for (col = ipp->col_ptr; col != NULL; col = col->next)
         ipp_enque_col(ipp, col), ncols++;
      /* process active rows and columns until both queues are empty */
      while (!(ipp->row_que == NULL && ipp->col_que == NULL))
      {  /* process active rows */
         while (ipp->row_que != NULL)
         {  /* select an active row */
            row = ipp->row_que;
            /* and remove it from the queue */
            ipp_deque_row(ipp, row);
            /* process the row */
            if (row->lb == -DBL_MAX && row->ub == +DBL_MAX)
            {  /* process free row */
               ipp_free_row(ipp, row);
            }
            else if (row->ptr == NULL)
            {  /* process empty row */
               if (ipp_empty_row(ipp, row)) return 1;
            }
            else if (row->ptr != NULL && row->ptr->r_next == NULL)
            {  /* process row singleton */
               if (ipp_row_sing(ipp, row)) return 1;
            }
            else
            {  /* general row analysis */
               if (ipp_analyze_row(ipp, row)) return 1;
            }
         }
         /* process active columns */
         while (ipp->col_que != NULL)
         {  /* select an active column */
            col = ipp->col_que;
            /* and remove it from the queue */
            ipp_deque_col(ipp, col);
            /* process the column */
            if (col->lb == col->ub)
            {  /* process fixed column */
               ipp_fixed_col(ipp, col);
            }
            else if (col->ptr == NULL)
            {  /* process empty column */
               if (ipp_empty_col(ipp, col)) return 2;
            }
            else
            {  /* general column analysis */
               if (ipp_analyze_col(ipp, col)) return 2;
            }
         }
      }
      for (row = ipp->row_ptr; row != NULL; row = row->next) nrows--;
      for (col = ipp->col_ptr; col != NULL; col = col->next) ncols--;
      xprintf("ipp_basic_tech:  %d row(s) and %d column(s) removed\n",
         nrows, ncols);
      return 0;
}

/*----------------------------------------------------------------------
-- REDUCE COLUMN BOUNDS
--
-- Given a row (constraint) the routine reduce_bounds tries to reduce
-- column bounds replacing them by implied bounds.
--
-- To obtain an implied lower/upper bound of a column x[j] the routine
-- solves the following LP relaxation:
--
--    minimize/maximize x[j]                                         (1)
--
--    subject to        L <= sum a[j] * x[j] <= U                    (2)
--                            j
--
--                      l[j] <= x[j] <= u[j]                         (3)
--
-- where (2) is a given row (constraint).
--
-- If a bound of a column has been changed, the column is placed in the
-- active queue for further processing. */

static int reduce_bounds(IPP *ipp, IPPROW *row)
{     IPPCOL *col, *c_min, *c_max;
      IPPAIJ *aij;
      int flag;
      double f_min, f_max, ff_min, ff_max, lb, ub, delta;
      /* compute implied lower bound of the row */
      c_min = NULL, f_min = 0.0;
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         if (aij->val > 0.0 && col->lb == -DBL_MAX ||
             aij->val < 0.0 && col->ub == +DBL_MAX)
         {  if (c_min == NULL)
               c_min = col;
            else
            {  f_min = -DBL_MAX;
               break;
            }
         }
         else
            f_min += aij->val * (aij->val > 0.0 ? col->lb : col->ub);
      }
      /* compute implied upper bound of the row */
      c_max = NULL, f_max = 0.0;
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         if (aij->val > 0.0 && col->ub == +DBL_MAX ||
             aij->val < 0.0 && col->lb == -DBL_MAX)
         {  if (c_max == NULL)
               c_max = col;
            else
            {  f_max = +DBL_MAX;
               break;
            }
         }
         else
            f_max += aij->val * (aij->val > 0.0 ? col->ub : col->lb);
      }
      /* process all columns in the row */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         /* compute implied lower bound of the row minus a[j] * x[j] */
         if (f_min == -DBL_MAX)
            ff_min = -DBL_MAX;
         else if (c_min == NULL)
            ff_min = f_min -
               aij->val * (aij->val > 0.0 ? col->lb : col->ub);
         else if (c_min == col)
            ff_min = f_min;
         else
            ff_min = -DBL_MAX;
         /* compute implied upper bound of the row minus a[j] * x[j] */
         if (f_max == +DBL_MAX)
            ff_max = +DBL_MAX;
         else if (c_max == NULL)
            ff_max = f_max -
               aij->val * (aij->val > 0.0 ? col->ub : col->lb);
         else if (c_max == col)
            ff_max = f_max;
         else
            ff_max = +DBL_MAX;
         /* compute implied lower and upper bounds of x[j] */
#if 1
         /* do not use aij if it has small magnitude to prevent wrong
            implied bounds; for example, 1e-15 * x1 >= x2 + x3, where
            x1 >= -10, x2, x3 >= 0, would lead to wrong conclusion that
            x1 >= 0 */
         if (fabs(aij->val) < 1e-6)
            lb = -DBL_MAX, ub = +DBL_MAX; else
#endif
         if (aij->val > 0.0)
         {  if (row->lb == -DBL_MAX || ff_max == +DBL_MAX)
               lb = -DBL_MAX;
            else
               lb = (row->lb - ff_max) / aij->val;
            if (row->ub == +DBL_MAX || ff_min == -DBL_MAX)
               ub = +DBL_MAX;
            else
               ub = (row->ub - ff_min) / aij->val;
         }
         else
         {  if (row->ub == +DBL_MAX || ff_min == -DBL_MAX)
               lb = -DBL_MAX;
            else
               lb = (row->ub - ff_min) / aij->val;
            if (row->lb == -DBL_MAX || ff_max == +DBL_MAX)
               ub = +DBL_MAX;
            else
               ub = (row->lb - ff_max) / aij->val;
         }
         /* to prevent infinite reducing we replace current bounds of
            x[j] by its implied bounds only if there is a significant
            change (not less than 10%) in the bounds */
         flag = 0;
         /* estimate significance in lower bound */
         if (lb != -DBL_MAX)
         {  if (col->i_flag)
               delta = 0.001; /* even tiny change is significant */
            else
               delta = 0.10 * (1.0 + fabs(lb));
            if (lb - delta >= col->lb) flag = 1;
         }
         /* estimate significance in upper bound */
         if (ub != +DBL_MAX)
         {  if (col->i_flag)
               delta = 0.001; /* even tiny change is significant */
            else
               delta = 0.10 * (1.0 + fabs(ub));
            if (ub + delta <= col->ub) flag = 1;
         }
         /* if the change is significant, perform replacing */
#if 0
         if (flag)
#else
         if (flag && lb < +1e15 && ub > -1e15)
#endif
         {  switch(ipp_tight_bnds(ipp, col, lb, ub))
            {  case 0:
                  /* bounds remain unchanged; can never be */
                  xassert(ipp != ipp);
               case 1:
                  /* bounds have been changed */
                  break;
               case 2:
                  /* new bounds are primal infeasible */
                  return 1;
               default:
                  xassert(ipp != ipp);
            }
            /* activate x[j] for further processing */
            ipp_enque_col(ipp, col);
         }
      }
      return 0;
}

/*----------------------------------------------------------------------
-- ipp_reduce_bnds - reduce column bounds.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- int ipp_reduce_bnds(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_reduce_bnds tries to reduce column bounds replacing
-- them by implied bounds.
--
-- Since changes in bounds of one column may involve changes in bounds
-- of other columns, the routine repeats reducing passes while there is
-- a significant change in bounds of at least one column.
--
-- RETURNS
--
-- 0 - no primal infeasibility is detected
-- 1 - implied bounds of some column are primal infeasible */

int ipp_reduce_bnds(IPP *ipp)
{     IPPROW *row;
      IPPCOL *col;
      IPPAIJ *aij;
      int pass = 0, total = 0, count;
      /* activate all rows */
      for (row = ipp->row_ptr; row != NULL; row = row->next)
         ipp_enque_row(ipp, row);
      /* deactivate all columns */
      for (col = ipp->col_ptr; col != NULL; col = col->next)
         ipp_deque_col(ipp, col);
loop: /* start a next pass for all active rows */
      pass++;
      while (ipp->row_que != NULL)
      {  /* select an active row */
         row = ipp->row_que;
         /* and remove it from the queue */
         ipp_deque_row(ipp, row);
         /* use the row to reduce column bounds */
         if (reduce_bounds(ipp, row)) return 1;
      }
      /* now all rows are inactive while all columns whose bounds were
         reduced are active */
      count = 0;
      while (ipp->col_que != NULL)
      {  count++;
         /* select an active column */
         col = ipp->col_que;
         /* and remove it from the queue */
         ipp_deque_col(ipp, col);
         /* activate corresponding rows for next pass */
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
         {  row = aij->row;
            ipp_enque_row(ipp, row);
         }
      }
      total += count;
      if (count > 0) goto loop;
      xprintf(
         "ipp_reduce_bnds: %d pass(es) made, %d bound(s) reduced\n",
         pass, total);
      return 0;
}

/*----------------------------------------------------------------------
-- SHIFTED COLUMN
--
-- Let column q has a finite non-zero lower bound:
--
--    l[q] <= x[q] <= u[q].                                          (1)
--
-- Then it can be replaced by:
--
--    x[q] = x'[q] + l[q],                                           (2)
--
-- where:
--
--    0 <= x'[q] <= u[q] - l[q]                                      (3)
--
-- is a non-negative column.
--
-- RECOVERING
--
-- Value of x[q] is recovered with formula (2) assuming that value of
-- x'[q] is already recovered. */

struct shift_col
{     /* shifted column */
      int q;
      /* reference number of a shifted column */
      double s;
      /* shifting value = original non-zero lower bound */
};

void ipp_shift_col(IPP *ipp, IPPCOL *col)
{     /* process shifted column */
      struct shift_col *info;
      IPPROW *row;
      IPPAIJ *aij;
      double temp;
      /* the column must have finite non-zero lower bound */
      xassert(col->lb != -DBL_MAX && col->lb != 0.0);
      /* create transformation queue entry */
      info = ipp_append_tqe(ipp, IPP_SHIFT_COL, sizeof(*info));
      info->q = col->j;
      info->s = col->lb;
      /* update bounds of corresponding rows */
      for (aij = col->ptr; aij != NULL; aij = aij->c_next)
      {  row = aij->row;
         temp = aij->val * info->s;
         if (row->lb == row->ub)
         {  row->lb -= temp;
            row->ub = row->lb;
         }
         else
         {  if (row->lb != -DBL_MAX) row->lb -= temp;
            if (row->ub != +DBL_MAX) row->ub -= temp;
         }
      }
      /* update the constant term of the objective function */
      ipp->c0 += col->c * info->s;
      /* set shifted bounds of the column */
      col->lb = 0.0;
      if (col->ub != +DBL_MAX) col->ub -= info->s;
      return;
}

void ipp_shift_col_r(IPP *ipp, void *_info)
{     /* recover shifted column */
      struct shift_col *info = _info;
      xassert(1 <= info->q && info->q <= ipp->ncols);
      xassert(ipp->col_stat[info->q] == 1);
      ipp->col_mipx[info->q] += info->s;
      return;
}

/*----------------------------------------------------------------------
-- NON-BINARY COLUMN
--
-- Let column q is integer non-negative:
--
--    0 <= x[q] <= u[q]                                              (1)
--
-- where u[q] > 1, i.e. the column is non-binary. In this case it can
-- be replaced by:
--
--    x[q] = 2^0 * z[0] + 2^1 * z[1] + ... + 2^(t-1) * z[t-1],       (2)
--
-- where z[0], z[1], ..., z[t-1] are binary variables, and t is minimal
-- number of bits sufficient to represent u[q].
--
-- To keep the upper bound of x[q] the following additional constraint
-- should be added to the problem:
--
--    2^0 * z[0] + 2^1 * z[1] + ... + 2^(t-1) * z[t-1] <= u[q].      (3)
--
-- However, if u[q] = 2^t - 1, the constraint (3) is redundant and may
-- be omitted.
--
-- RECOVERING
--
-- Value of x[q] is recovered with formula (2) assuming that values of
-- z[0], z[1], ..., z[t-1] are already recovered. */

struct nonbin_col
{     /* non-binary column */
      int q;
      /* reference number of a non-binary column */
      IPPLFE *ptr;
      /* 2^0 * z[0] + 2^1 * z[1] + ... + 2^(t-1) * z[t-1] */
};

int ipp_nonbin_col(IPP *ipp, IPPCOL *col)
{     /* process non-binary column */
      struct nonbin_col *info;
      IPPROW *row;
      IPPCOL *bin;
      IPPAIJ *aij;
      IPPLFE *lfe;
      int u, t, two_t, k, two_k;
      /* the column must be integral */
      xassert(col->i_flag);
      /* its lower bound must be zero */
      xassert(col->lb == 0.0);
      /* its upper bound must be greater than one */
      xassert(col->ub >= 2.0);
      /* and must be not greater than 2^15-1 = 32767 (implementation
         restriction) */
      xassert(col->ub <= 32767.0);
      /* create transformation queue entry */
      info = ipp_append_tqe(ipp, IPP_NONBIN_COL, sizeof(*info));
      info->q = col->j;
      info->ptr = NULL;
      /* determine t, minimal number of bits sufficient to represent
         the upper bound */
      u = (int)col->ub;
      xassert(col->ub == (double)u);
      for (t = 2, two_t = 4; t <= 15; t++, two_t += two_t)
         if (u <= two_t - 1) break;
      xassert(t <= 15);
      /* create additional constraint (3), if necessary */
      if (u <= two_t - 2)
         row = ipp_add_row(ipp, -DBL_MAX, (double)u);
      /* create binary columns z[0], z[1], ..., z[t-1] */
      for (k = 0, two_k = 1; k < t; k++, two_k += two_k)
      {  bin = ipp_add_col(ipp, 1, 0.0, 1.0, 0.0);
         lfe = dmp_get_atom(ipp->tqe_pool, sizeof(IPPLFE));
         lfe->ref = bin->j;
         lfe->val = (double)two_k;
         lfe->next = info->ptr;
         info->ptr = lfe;
         /* create constraint coefficients for z[k] */
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
            ipp_add_aij(ipp, aij->row, bin, aij->val * lfe->val);
         /* create objective coefficient for z[k] */
         bin->c = col->c * lfe->val;
         /* include z[k] in additional constraint (3), if necessary */
         if (u <= two_t - 2)
            ipp_add_aij(ipp, row, bin, lfe->val);
      }
      /* remove the original column x[q] from the problem */
      ipp_remove_col(ipp, col);
      return t;
}

void ipp_nonbin_col_r(IPP *ipp, void *_info)
{     /* recover non-binary column */
      struct nonbin_col *info = _info;
      IPPLFE *lfe;
      double temp;
      xassert(1 <= info->q && info->q <= ipp->ncols);
      xassert(ipp->col_stat[info->q] == 0);
      temp = 0.0;
      for (lfe = info->ptr; lfe != NULL; lfe = lfe->next)
      {  xassert(1 <= lfe->ref && lfe->ref <= ipp->ncols);
         xassert(ipp->col_stat[lfe->ref] == 1);
         temp += lfe->val * ipp->col_mipx[lfe->ref];
      }
      ipp->col_stat[info->q] = 1;
      ipp->col_mipx[info->q] = temp;
      return;
}

/*----------------------------------------------------------------------
-- COEFFICIENT REDUCTION
--
-- Let an inequality constraint is the following:
--
--    sum a[j] * x[j] + a[k] * x[k] <= b,                            (1)
--   j!=k
--
-- where x[k] is a binary variable. Let also be known that:
--
--    sum a[j] * x[j] <= u,                                          (2)
--   j!=k
--
-- where u is an implied upper bound of the row.
--
-- Case 1. If a[k] > 0 and b - a[k] < u < b, a[k] can be replaced by
-- (ak + u - b) and b can be replaced by u.
--
-- Case 2. If a[k] < 0 and b < u < b - a[k], a[k] can be replaced by
-- (b - u) and b remains unchanged.
--
-- In both cases magnitude of a[k] is decreased while the sign of a[k]
-- remains unchanged.
--
-- If a[k] has been changed, the corresponding column x[k] is placed in
-- the active queue for further processing. */

static void reduce_coef(IPP *ipp, IPPROW *row)
{     IPPCOL *col, *c_max;
      IPPAIJ *aij;
      double f_max, ff_max, eps;
      /* the row must be '<=' constraint */
      xassert(row->lb == -DBL_MAX && row->ub != +DBL_MAX);
      /* compute implied upper bound of the row */
      c_max = NULL, f_max = 0.0;
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         if (aij->val > 0.0 && col->ub == +DBL_MAX ||
             aij->val < 0.0 && col->lb == -DBL_MAX)
         {  if (c_max == NULL)
               c_max = col;
            else
            {  f_max = +DBL_MAX;
               break;
            }
         }
         else
            f_max += aij->val * (aij->val > 0.0 ? col->ub : col->lb);
      }
      /* process all columns in the row */
      for (aij = row->ptr; aij != NULL; aij = aij->r_next)
      {  col = aij->col;
         /* skip non-binary column */
         if (!col->i_flag) continue;
         if (!(col->lb == 0.0 && col->ub == 1.0)) continue;
         /* compute implied upper bound of the row minus a[k] * a[k] */
         if (f_max == +DBL_MAX)
            ff_max = +DBL_MAX;
         else if (c_max == NULL)
            ff_max = f_max -
               aij->val * (aij->val > 0.0 ? col->ub : col->lb);
         else if (c_max == col)
            ff_max = f_max;
         else
            ff_max = +DBL_MAX;
         /* try to reduce constraint coefficient a[k] */
         if (ff_max == +DBL_MAX) continue;
         eps = 1e-5 * (1.0 + fabs(ff_max));
         if (aij->val > 0.0)
         {  /* case 1 */
            if (row->ub - aij->val + eps <= ff_max &&
                ff_max <= row->ub - eps)
            {  aij->val += ff_max - row->ub;
               row->ub = ff_max;
               ipp_enque_col(ipp, col);
            }
         }
         else
         {  /* case 2 */
            if (row->ub + eps <= ff_max &&
               ff_max <= row->ub - aij->val - eps)
            {  aij->val = row->ub - ff_max;
               row->ub = row->ub;
               ipp_enque_col(ipp, col);
            }
         }
      }
      return;
}

/*----------------------------------------------------------------------
-- ipp_reduce_coef - reduce constraint coefficients.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_reduce_coef(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_reduce_coef tries to reduce constraint coefficients
-- for inequality constraints '<='. */

void ipp_reduce_coef(IPP *ipp)
{     IPPROW *row;
      IPPCOL *col;
      IPPAIJ *aij;
      int pass = 0, total = 0, count;
      /* activate all rows which are '<=' inequalities */
      for (row = ipp->row_ptr; row != NULL; row = row->next)
      {  if (row->lb == -DBL_MAX && row->ub != +DBL_MAX)
            ipp_enque_row(ipp, row);
      }
      /* deactivate all columns */
      for (col = ipp->col_ptr; col != NULL; col = col->next)
         ipp_deque_col(ipp, col);
loop: /* start a next pass for all active rows */
      pass++;
      while (ipp->row_que != NULL)
      {  /* select an active row */
         row = ipp->row_que;
         /* and remove it from the queue */
         ipp_deque_row(ipp, row);
         /* try to reduce constraint coefficients */
         reduce_coef(ipp, row);
      }
      /* now all rows are inactive while all columns whose coefficients
         were reduced are active */
      count = 0;
      while (ipp->col_que != NULL)
      {  count++;
         /* select an active column */
         col = ipp->col_que;
         /* and remove it from the queue */
         ipp_deque_col(ipp, col);
         /* activate corresponding rows for next pass */
         for (aij = col->ptr; aij != NULL; aij = aij->c_next)
         {  row = aij->row;
            if (row->lb == -DBL_MAX && row->ub != +DBL_MAX)
               ipp_enque_row(ipp, row);
         }
      }
      total += count;
      if (count > 0) goto loop;
      xprintf("ipp_reduce_coef: %d pass(es) made, %d coefficient(s) red"
         "uced\n", pass, total);
      return;
}

/*----------------------------------------------------------------------
-- ipp_binarize - replace general integer variables by binary ones.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_binarize(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_binarize replaces all integer variables by binary
-- variables as explained in comments to the routine ipp_nonbin_col.
--
-- It is highly recommended to previously performs bounds reduction in
-- order to minimize the number of binary variables required. */

void ipp_binarize(IPP *ipp)
{     IPPCOL *col;
      int ncols, nbins;
      /* activate all columns (variables) to be replaced */
      for (col = ipp->col_ptr; col != NULL; col = col->next)
      {  ipp_deque_col(ipp, col);
         if (!col->i_flag) continue;
         if (col->lb == col->ub) continue;
         if (col->lb == 0.0 && col->ub == 1.0) continue;
         xassert(col->lb != -DBL_MAX);
         xassert(col->ub != +DBL_MAX);
         if (col->lb == -DBL_MAX || col->ub == +DBL_MAX ||
             col->ub - col->lb > 32767.0)
         {  xprintf("WARNING: BINARIZATION IMPOSSIBLE\n");
            goto done;
         }
         ipp_enque_col(ipp, col);
      }
      /* perform replacement */
      ncols = nbins = 0;
      while (ipp->col_que != NULL)
      {  /* select an active column to be replaced */
         ncols++;
         col = ipp->col_que;
         /* and remove it from the queue */
         ipp_deque_col(ipp, col);
         /* make the lower bound to be zero, if necessary */
         if (col->lb != 0.0) ipp_shift_col(ipp, col);
         /* if the column became binary, drop it */
         if (col->ub == 1.0) continue;
         /* replace the non-binary column */
         nbins += ipp_nonbin_col(ipp, col);
      }
      if (ncols == 0)
         xprintf(
            "ipp_binarize: no general integer variables detected\n");
      else
         xprintf("ipp_binarize: %d integer variable(s) replaced by %d b"
            "inary ones\n", ncols, nbins);
done: return;
}

/*----------------------------------------------------------------------
-- ipp_reduction - perform coefficient reduction.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_reduction(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_reduction tries to reduce constraint coefficients
-- for inequality constraints having binary columns (variables).
--
-- It is highly recommended to previously performs bounds reduction in
-- order to made coefficient reduction much more efficient. */

void ipp_reduction(IPP *ipp)
{     IPPROW *row, *dup;
      IPPCOL *col;
      IPPAIJ *aij;
      int nrows;
      /* activate all double-bounded rows to be replaced by ordinary
         inequality rows */
      for (row = ipp->row_ptr; row != NULL; row = row->next)
      {  ipp_deque_row(ipp, row);
         if (row->lb == -DBL_MAX) continue;
         if (row->ub == +DBL_MAX) continue;
         if (row->lb == row->ub) continue;
         /* skip rows having no binary variables */
         for (aij = row->ptr; aij != NULL; aij = aij->r_next)
         {  col = aij->col;
            if (!(col->i_flag && col->lb == 0.0 && col->ub == 1.0))
               break;
         }
         if (aij != NULL) continue;
         ipp_enque_row(ipp, row);
      }
      /* perform replacement */
      nrows = 0;
      while (ipp->row_que != NULL)
      {  /* select an active row to be replaced */
         nrows++;
         row = ipp->row_que;
         /* and remove it from the queue */
         ipp_deque_row(ipp, row);
         /* duplicate the row */
         dup = ipp_add_row(ipp, -DBL_MAX, row->ub);
         row->ub = +DBL_MAX;
         for (aij = row->ptr; aij != NULL; aij = aij->r_next)
            ipp_add_aij(ipp, dup, aij->col, aij->val);
      }
      if (nrows > 0)
         xprintf("ipp_reduction: %d row(s) splitted into single inequal"
            "ities\n", nrows);
      /* replace all '>=' inequalities by '<=' ones */
      for (row = ipp->row_ptr; row != NULL; row = row->next)
      {  if (row->lb != -DBL_MAX && row->ub == +DBL_MAX)
         {  row->ub = - row->lb;
            row->lb = - DBL_MAX;
            for (aij = row->ptr; aij != NULL; aij = aij->r_next)
               aij->val = - aij->val;
         }
      }
      /* call basic reduction routine */
      ipp_reduce_coef(ipp);
      return;
}

/*----------------------------------------------------------------------
-- ipp_postsolve - MIP postsolve processing.
--
-- SYNOPSIS
--
-- #include "glpipp.h"
-- void ipp_postsolve(IPP *ipp);
--
-- DESCRIPTION
--
-- The routine ipp_postsolve performs a postsolve processing to recover
-- a solution of the original problem. It is assumed that a solution of
-- the resultant problem is loaded into the presolver workspace. */

void ipp_postsolve(IPP *ipp)
{     IPPTQE *tqe;
      for (tqe = ipp->tqe_list; tqe != NULL; tqe = tqe->next)
      {  switch (tqe->type)
         {  case IPP_FIXED_COL:
               ipp_fixed_col_r(ipp, tqe->info);
               break;
            case IPP_SHIFT_COL:
               ipp_shift_col_r(ipp, tqe->info);
               break;
            case IPP_NONBIN_COL:
               ipp_nonbin_col_r(ipp, tqe->info);
               break;
            default:
               xassert(tqe != tqe);
         }
      }
      return;
}

/* eof */
