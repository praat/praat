/* glplpx12.c */

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
-- lpx_remove_tiny - remove zero and tiny elements.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- int lpx_remove_tiny(int ne, int ia[], int ja[], double ar[],
--    double eps);
--
-- DESCRIPTION
--
-- The routine lpx_remove_tiny removes zero and tiny elements from a
-- sparse matrix specified as triplets (ia[k], ja[k], ar[k]), for
-- k = 1, ..., ne, where ia[k] is the row index, ja[k] is the column
-- index, ar[k] is a numeric value of corresponding matrix element. The
-- parameter ne specifies the total number of elements in the matrix.
--
-- The parameter ia or ja can specified as NULL that allows using this
-- routine to remove zero and tiny elements from a sparse vector.
--
-- An element ar[k] is considered as tiny, if
--
--    |ar[k]| < eps * max(1, |ar[1]|, ..., |ar[ne]|),
--
-- where eps is a specified relative threshold. If eps = 0, only zero
-- elements are removed.
--
-- RETURNS
--
-- The routine lpx_remove_tiny returns the number of remaining elements
-- in the matrix (vector) after removal. */

int lpx_remove_tiny(int ne, int ia[], int ja[], double ar[],
      double eps)
{     int k, newne;
      double big;
      if (ne < 0)
         xfault("lpx_remove_tiny: ne = %d; invalid number of elements\n"
            , ne);
      if (eps < 0.0)
         xfault("lpx_remove_tiny: eps = %g; invalid threshold\n", eps);
      /* big := max(1, |ar[1]|, ..., |ar[ne]|) */
      big = 1.0;
      for (k = 1; k <= ne; k++)
         if (big < fabs(ar[k])) big = fabs(ar[k]);
      /* remove zero and tiny elements */
      newne = 0;
      for (k = 1; k <= ne; k++)
      {  if (ar[k] == 0.0) continue;
         if (fabs(ar[k]) < eps * big) continue;
         newne++;
         if (ia != NULL) ia[newne] = ia[k];
         if (ja != NULL) ja[newne] = ja[k];
         ar[newne] = ar[k];
      }
      return newne;
}

/*----------------------------------------------------------------------
-- lpx_reduce_form - reduce linear form.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- int lpx_reduce_form(LPX *lp, int len, int ind[], double val[],
--    double work[]);
--
-- DESCRIPTION
--
-- The routine lpx_reduce_form reduces the specified linear form:
--
--    f = a[1]*x[1] + a[2]*x[2] + ... + a[m+n]*x[m+n],               (1)
--
-- which includes auxiliary and structural variables, by substituting
-- auxiliary variables x[1], ..., x[m] from equality constraints of the
-- specified LP object, so the resultant linear form becomes expressed
-- only through structural variables:
--
--    f' = a'[1]*x[m+1] + a'[2]*x[m+2] + ... + a'[n]*x[m+n].         (2)
--
-- On entry indices and numerical values of the coefficients of the
-- specified linear form (1) should be placed in locations ndx[1], ...,
-- ndx[len] and val[1], ..., val[len], respectively, where indices from
-- 1 to m denote auxiliary variables, indices from m+1 to m+n denote
-- structural variables.
--
-- On exit the routine stores column indices and coefficients of the
-- resultant linear form (2) in locations ndx[1], ..., ndx[len'] and
-- val[1], ..., val[len'], respectively, where 0 <= len' <= n is the
-- number of non-zero coefficients returned by the routine.
--
-- The working array work should have at least 1+m+n locations, where
-- m and n is, respectively, the number of rows and columns in the LP
-- object. If work is NULL, the working array is allocated and freed
-- internally by the routine.
--
-- RETURNS
--
-- The routine returns len', the number of non-zero coefficients in the
-- resultant linear form. */

int lpx_reduce_form(LPX *lp, int len, int ind[], double val[],
      double _work[])
{     int m = lpx_get_num_rows(lp);
      int n = lpx_get_num_cols(lp);
      int i, j, k, t;
      double *work = _work;
      /* allocate working array */
      if (_work == NULL) work = xcalloc(1+m+n, sizeof(double));
      /* convert the original linear form to dense format */
      for (k = 1; k <= m+n; k++)
         work[k] = 0.0;
      for (t = 1; t <= len; t++)
      {  k = ind[t];
         if (!(1 <= k && k <= m+n))
            xfault("lpx_reduce_form: ind[%d] = %d; ordinal number out o"
               "f range\n", t, k);
         work[k] += val[t];
      }
      /* perform substitution */
      for (i = 1; i <= m; i++)
      {  /* substitute x[i] = a[i,1]*x[m+1] + ... + a[i,n]*x[m+n] */
         if (work[i] == 0.0) continue;
         len = lpx_get_mat_row(lp, i, ind, val);
         for (t = 1; t <= len; t++)
         {  j = ind[t];
            work[m+j] += work[i] * val[t];
         }
      }
      /* convert the resultant linear form to sparse format */
      len = 0;
      for (j = 1; j <= n; j++)
      {  if (work[m+j] == 0.0) continue;
         len++;
         ind[len] = j;
         val[len] = work[m+j];
      }
      /* free working array */
      if (_work == NULL) xfree(work);
      return len;
}

/*----------------------------------------------------------------------
-- lpx_eval_row - compute explictily specified row.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- double lpx_eval_row(LPX *lp, int len, int ind[], double val[]);
--
-- DESCRIPTION
--
-- The routine lpx_eval_row computes the primal value of an explicitly
-- specified row using current values of structural variables.
--
-- The explicitly specified row may be thought as a linear form:
--
--    y = a[1]*x[m+1] + a[2]*x[m+2] + ... + a[n]*x[m+n],
--
-- where y is an auxiliary variable for this row, a[j] are coefficients
-- of the linear form, x[m+j] are structural variables.
--
-- On entry column indices and numerical values of non-zero elements of
-- the row should be stored in locations ind[1], ..., ind[len] and
-- val[1], ..., val[len], where len is the number of non-zero elements.
-- The array ind and val are not changed on exit.
--
-- RETURNS
--
-- The routine returns a computed value of y, the auxiliary variable of
-- the specified row. */

double lpx_eval_row(LPX *lp, int len, int ind[], double val[])
{     int n = lpx_get_num_cols(lp);
      int j, k;
      double sum = 0.0;
      if (len < 0)
         xfault("lpx_eval_row: len = %d; invalid row length\n", len);
      for (k = 1; k <= len; k++)
      {  j = ind[k];
         if (!(1 <= j && j <= n))
            xfault("lpx_eval_row: j = %d; column number out of range\n",
               j);
         sum += val[k] * lpx_get_col_prim(lp, j);
      }
      return sum;
}

/*----------------------------------------------------------------------
-- lpx_eval_degrad - compute degradation of the objective function.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- double lpx_eval_degrad(LPX *lp, int len, int ind[], double val[],
--    int type, double rhs);
--
-- DESCRIPTION
--
-- The routine lpx_eval_degrad computes a degradation (worsening) of
-- the objective function if a specified inequality constraint, which
-- is assumed to be violated at the current point, would be introduced
-- in the problem. The degradation computed by the routine is a change
-- in the objective function obtained by one step of the dual simplex,
-- so it gives only the lower bound of the actual objective change.
--
-- The inequality constraint is specified as a row (linear form):
--
--    y = a[1]*x[m+1] + a[2]*x[m+2] + ... + a[n]*x[m+n] <rho> b,
--
-- where y is an auxiliary variable for this row, a[j] are coefficients
-- of the row, x[m+j] are structural variables, <rho> is a relation sign
-- '>=' or '<=', b is a right-hand side.
--
-- On entry column indices and numerical values of non-zero elements of
-- the row should be stored in locations ind[1], ..., ind[len] and
-- val[1], ..., val[len], where len is the number of non-zero elements.
--
-- NOTE: The arrays ind and val must have at least 1+n locations, where
--       n is the number of columns in the problem. The contents of the
--       arrays ind and val are DESTROYED on exit.
--
-- The parameter type specifies the relation sign <rho>: LPX_LO means
-- '>=', LPX_UP means '<='. The right-hand side b should be specified by
-- the parameter rhs.
--
-- NOTE: On entry to the routine the current basic solution must be
--       dual feasible. It is also assumed that the specified inequality
--       constraint is violated at the current point, i.e. y < b in the
--       case of '>=', or y > b in the case of '<='.
--
-- RETURNS
--
-- The routine lpx_eval_degrad returns the change delta = Z' - Z, where
-- Z is the objective value in the current vertex, and Z' is the
-- objective value in the adjacent vertex (which remains dual feasible).
-- So, if the problem is minimization, delta >= 0, and if the problem is
-- maximization, delta <= 0. If no adjacent dual feasible vertex exist,
-- the routine returns +DBL_MAX (in the case of minization) or -DBL_MAX
-- (in the case of maximization) */

double lpx_eval_degrad(LPX *lp, int len, int ind[], double val[],
      int type, double rhs)
{     int m = lpx_get_num_rows(lp);
      int n = lpx_get_num_cols(lp);
      int dir = lpx_get_obj_dir(lp);
      int q, k;
      double y, delta;
      if (lpx_get_dual_stat(lp) != LPX_D_FEAS)
         xfault("lpx_eval_degrad: LP basis is not dual feasible\n");
      if (!(0 <= len && len <= n))
         xfault("lpx_eval_degrad: len = %d; invalid row length\n", len);
      if (!(type == LPX_LO || type == LPX_UP))
         xfault("lpx_eval_degrad: type = %d; invalid row type\n", type);
      /* compute value of the row auxiliary variable */
      y = lpx_eval_row(lp, len, ind, val);
      /* the inequalitiy constraint is assumed to be violated */
      if (!(type == LPX_LO && y < rhs || type == LPX_UP && y > rhs))
         xfault("lpx_eval_degrad: y = %g, rhs = %g; constraint is not v"
            "iolated\n", y, rhs);
      /* transform the row in order to express y through only non-basic
         variables: y = alfa[1] * xN[1] + ... + alfa[n] * xN[n] */
      len = lpx_transform_row(lp, len, ind, val);
      /* in the adjacent basis y would become non-basic, so in case of
         '>=' it increases and in case of '<=' it decreases; determine
         which non-basic variable x[q], 1 <= q <= m+n, should leave the
         basis to keep dual feasibility */
      q = lpx_dual_ratio_test(lp, len, ind, val,
         type == LPX_LO ? +1 : -1, 1e-7);
      /* q = 0 means no adjacent dual feasible basis exist */
      if (q == 0) return dir == LPX_MIN ? +DBL_MAX : -DBL_MAX;
      /* find the entry corresponding to x[q] in the list */
      for (k = 1; k <= len; k++)
         if (ind[k] == q) break;
      xassert(k <= len);
      /* dy = alfa[q] * dx[q], so dx[q] = dy / alfa[q], where dy is a
         change in y, dx[q] is a change in x[q] */
      delta = (rhs - y) / val[k];
#if 0
      /* Tomlin noticed that if the variable x[q] is of integer kind,
         its change cannot be less than one in the magnitude */
      if (q > m && lpx_get_col_kind(lp, q-m) == LPX_IV)
      {  /* x[q] is structural integer variable */
         if (fabs(delta - floor(delta + 0.5)) > 1e-3)
         {  if (delta > 0.0)
               delta = ceil(delta);  /* +3.14 -> +4 */
            else
               delta = floor(delta); /* -3.14 -> -4 */
         }
      }
#endif
      /* dz = lambda[q] * dx[q], where dz is a change in the objective
         function, lambda[q] is a reduced cost of x[q] */
      if (q <= m)
         delta *= lpx_get_row_dual(lp, q);
      else
         delta *= lpx_get_col_dual(lp, q-m);
      /* delta must be >= 0 (in case of minimization) or <= 0 (in case
         of minimization); however, due to round-off errors and finite
         tolerance used to choose x[q], this condition can be slightly
         violated */
      switch (dir)
      {  case LPX_MIN: if (delta < 0.0) delta = 0.0; break;
         case LPX_MAX: if (delta > 0.0) delta = 0.0; break;
         default: xassert(dir != dir);
      }
      return delta;
}

/*----------------------------------------------------------------------
-- lpx_gomory_cut - generate Gomory's mixed integer cut.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- int lpx_gomory_cut(LPX *lp, int len, int ind[], double val[],
--    double work[]);
--
-- DESCRIPTION
--
-- The routine lpx_mixed_gomory generates a Gomory's mixed integer cut
-- for a given row of the simplex table.
--
-- The given row of the simplex table should be explicitly specified in
-- the form:
--
--    y = alfa[1]*xN[1] + alfa[2]*xN[2] + ... + alfa[n]*xN[n]        (1)
--
-- where y is some basic structural variable of integer kind, whose
-- value in the current basic solution is assumed to be fractional,
-- xN[1], ..., xN[n] are non-basic variables, alfa[1], ..., alfa[n] are
-- influence coefficient.
--
-- On entry indices (ordinal numbers) of non-basic variables, which
-- have non-zero influence coefficients, should be placed in locations
-- ind[1], ..., ind[len], where indices from 1 to m denote auxiliary
-- variables and indices from m+1 to m+n denote structural variables,
-- and the corresponding influence coefficients should be placed in
-- locations val[1], ..., val[len]. These arrays can be computed using
-- the routine lpx_eval_tab_row.
--
-- The routine generates the Gomory's mixed integer cut in the form of
-- an inequality constraint:
--
--    a[1]*x[m+1] + a[2]*x[m+2] + ... + a[n]*x[m+n] >= b,            (2)
--
-- where x[m+1], ..., x[m+n] are structural variables, a[1], ..., a[n]
-- are constraint coefficients, b is a right-hand side.
--
-- On exit the routine stores indices of structural variables and the
-- corresponding non-zero constraint coefficients for the inequality
-- constraint (2) in locations ind[1], ..., ind[len'] and val[1], ...,
-- val[len'], where 0 <= len' <= n is returned by the routine. The
-- right-hand side b is stored to the location val[0], and the location
-- ind[0] is set to 0. (Should note than on exit indices of structural
-- variables stored in the array ind are in the range from 1 to n.)
--
-- The working array work should have at least 1+m+n locations, where
-- m and n is, respectively, the number of rows and columns in the LP
-- object. If work is NULL, the working array is allocated and freed
-- internally by the routine.
--
-- RETURNS
--
-- If the cutting plane has been successfully generated, the routine
-- returns 0 <= len' <= n, which is the number of non-zero coefficients
-- in the inequality constraint (2). In case of errors the routine
-- returns one of the following negative codes:
--
-- -1 - in the row (1) there is a free non-basic variable with non-zero
--      influence coefficient;
-- -2 - value of the basic variable y defined by the row (1) is near to
--      a closest integer point, so the cutting plane may be unreliable.
--
-- REFERENCES
--
-- 1. R.E.Gomory. An algorithm for the mixed integer problem. Tech.Rep.
--    RM-2597, The RAND Corp. (1960).
--
-- 2. H.Marchand, A.Martin, R.Weismantel, L.Wolsey. Cutting Planes in
--    Integer and Mixed Integer Programming. CORE, October 1999. */

int lpx_gomory_cut(LPX *lp, int len, int ind[], double val[],
      double work[])
{     int m = lpx_get_num_rows(lp);
      int n = lpx_get_num_cols(lp);
      int k, t, stat;
      double lb, ub, *alfa, beta, alfa_j, f0, fj, *a, b, a_j;
      /* on entry the specified row of the simplex table has the form:
         y = alfa[1]*xN[1] + ... + alfa[n]*xN[n];
         convert this row to the form:
         y + alfa'[1]*xN'[1] + ... + alfa'[n]*xN'[n] = beta,
         where all new (stroked) non-basic variables are non-negative
         (this is not needed for y, because it has integer bounds and
         only fractional part of beta is used); note that beta is the
         value of y in the current basic solution */
      alfa = val;
      beta = 0.0;
      for (t = 1; t <= len; t++)
      {  /* get index of some non-basic variable x[k] = xN[j] */
         k = ind[t];
         if (!(1 <= k && k <= m+n))
            xfault("lpx_gomory_cut: ind[%d] = %d; variable number out o"
               "f range\n", t, k);
         /* get the original influence coefficient alfa[j] */
         alfa_j = alfa[t];
         /* obtain status and bounds of x[k] = xN[j] */
         if (k <= m)
         {  stat = lpx_get_row_stat(lp, k);
            lb = lpx_get_row_lb(lp, k);
            ub = lpx_get_row_ub(lp, k);
         }
         else
         {  stat = lpx_get_col_stat(lp, k-m);
            lb = lpx_get_col_lb(lp, k-m);
            ub = lpx_get_col_ub(lp, k-m);
         }
         /* perform conversion */
         if (stat == LPX_BS)
            xfault("lpx_gomory_cut: ind[%d] = %d; variable must be non-"
               "basic\n", t, k);
         switch (stat)
         {  case LPX_NL:
               /* xN[j] is on its lower bound */
               /* substitute xN[j] = lb[k] + xN'[j] */
               alfa[t] = - alfa_j;
               beta += alfa_j * lb;
               break;
            case LPX_NU:
               /* xN[j] is on its upper bound */
               /* substitute xN[j] = ub[k] - xN'[j] */
               alfa[t] = + alfa_j;
               beta += alfa_j * ub;
               break;
            case LPX_NF:
               /* xN[j] is free non-basic variable */
               return -1;
            case LPX_NS:
               /* xN[j] is fixed non-basic variable */
               /* substitute xN[j] = lb[k] */
               alfa[t] = 0.0;
               beta += alfa_j * lb;
               break;
            default:
               xassert(stat != stat);
         }
      }
      /* now the converted row of the simplex table has the form:
         y + alfa'[1]*xN'[1] + ... + alfa'[n]*xN'[n] = beta,
         where all xN'[j] >= 0; generate Gomory's mixed integer cut in
         the form of inequality:
         a'[1]*xN'[1] + ... + a'[n]*xN'[n] >= b' */
      a = val;
      /* f0 is fractional part of beta, where beta is the value of the
         variable y in the current basic solution; if f0 is close to
         zero or to one, i.e. if y is near to a closest integer point,
         the corresponding cutting plane may be unreliable */
      f0 = beta - floor(beta);
      if (!(0.00001 <= f0 && f0 <= 0.99999)) return -2;
      for (t = 1; t <= len; t++)
      {  alfa_j = alfa[t];
         if (alfa_j == 0.0)
         {  a[t] = 0.0;
            continue;
         }
         k = ind[t];
         xassert(1 <= k && k <= m+n);
         if (k > m && lpx_get_col_kind(lp, k-m) == LPX_IV)
         {  /* xN[j] is integer */
            fj = alfa_j - floor(alfa_j);
            if (fj <= f0)
               a[t] = fj;
            else
               a[t] = (f0 / (1.0 - f0)) * (1.0 - fj);
         }
         else
         {  /* xN[j] is continuous */
            if (alfa_j > 0.0)
               a[t] = alfa_j;
            else
               a[t] = - (f0 / (1.0 - f0)) * alfa_j;
         }
      }
      b = f0;
      /* now the generated cutting plane has the form of an inequality:
         a'[1]*xN'[1] + ... + a'[n]*xN'[n] >= b';
         convert this inequality back to the form expressed through the
         original non-basic variables:
         a[1]*xN[1] + ... + a[n]*xN[n] >= b */
      for (t = 1; t <= len; t++)
      {  a_j = a[t];
         if (a_j == 0.0) continue;
         k = ind[t]; /* x[k] = xN[j] */
         /* obtain status and bounds of x[k] = xN[j] */
         if (k <= m)
         {  stat = lpx_get_row_stat(lp, k);
            lb = lpx_get_row_lb(lp, k);
            ub = lpx_get_row_ub(lp, k);
         }
         else
         {  stat = lpx_get_col_stat(lp, k-m);
            lb = lpx_get_col_lb(lp, k-m);
            ub = lpx_get_col_ub(lp, k-m);
         }
         /* perform conversion */
         switch (stat)
         {  case LPX_NL:
               /* xN[j] is on its lower bound */
               /* substitute xN'[j] = xN[j] - lb[k] */
               val[t] = + a_j;
               b += a_j * lb;
               break;
            case LPX_NU:
               /* xN[j] is on its upper bound */
               /* substitute xN'[j] = ub[k] - xN[j] */
               val[t] = - a_j;
               b -= a_j * ub;
               break;
            default:
               xassert(stat != stat);
         }
      }
      /* substitute auxiliary (non-basic) variables to the generated
         inequality constraint a[1]*xN[1] + ... + a[n]*xN[n] >= b using
         the equality constraints of the specified LP problem object in
         order to express the generated constraint through structural
         variables only */
      len = lpx_reduce_form(lp, len, ind, val, work);
      /* store the right-hand side */
      ind[0] = 0, val[0] = b;
      /* return to the calling program */
      return len;
}

/*----------------------------------------------------------------------
-- COVER INEQUALITIES
--
-- Consider the set of feasible solutions to 0-1 knapsack problem:
--
--    sum a[j]*x[j] <= b,                                            (1)
--  j in J
--
--    x[j] is binary,                                                (2)
--
-- where, wlog, we assume that a[j] > 0 (since 0-1 variables can be
-- complemented) and a[j] <= b (since a[j] > b implies x[j] = 0).
--
-- A set C within J is called a cover if
--
--    sum a[j] > b.                                                  (3)
--  j in C
--
-- For any cover C the inequality
--
--    sum x[j] <= |C| - 1                                            (4)
--  j in C
--
-- is called a cover inequality and is valid for (1)-(2).
--
-- MIXED COVER INEQUALITIES
--
-- Consider the set of feasible solutions to mixed knapsack problem:
--
--    sum a[j]*x[j] + y <= b,                                        (5)
--  j in J
--
--    x[j] is binary,                                                (6)
--
--    0 <= y <= u is continuous,                                     (7)
--
-- where again we assume that a[j] > 0.
--
-- Let C within J be some set. From (1)-(4) it follows that
--
--    sum a[j] > b - y                                               (8)
--  j in C
--
-- implies
--
--    sum x[j] <= |C| - 1.                                           (9)
--  j in C
--
-- Thus, we need to modify the inequality (9) in such a way that it be
-- a constraint only if the condition (8) is satisfied.
--
-- Consider the following inequality:
--
--    sum x[j] <= |C| - t.                                          (10)
--  j in C
--
-- If 0 < t <= 1, then (10) is equivalent to (9), because all x[j] are
-- binary variables. On the other hand, if t <= 0, (10) being satisfied
-- for any values of x[j] is not a constraint.
--
-- Let
--
--    t' = sum a[j] + y - b.                                        (11)
--       j in C
--
-- It is understood that the condition t' > 0 is equivalent to (8).
-- Besides, from (6)-(7) it follows that t' has an implied upper bound:
--
--    t'max = sum a[j] + u - b.                                     (12)
--          j in C
--
-- This allows to express the parameter t having desired properties:
--
--    t = t' / t'max.                                               (13)
--
-- In fact, t <= 1 by definition, and t > 0 being equivalent to t' > 0
-- is equivalent to (8).
--
-- Thus, the inequality (10), where t is given by formula (13) is valid
-- for (5)-(7).
--
-- Note that if u = 0, then y = 0, so t = 1, and the conditions (8) and
-- (10) is transformed to the conditions (3) and (4).
--
-- GENERATING MIXED COVER CUTS
--
-- To generate a mixed cover cut in the form (10) we need to find such
-- set C which satisfies to the inequality (8) and for which, in turn,
-- the inequality (10) is violated in the current point.
--
-- Substituting t from (13) to (10) gives:
--
--                        1
--    sum x[j] <= |C| - -----  (sum a[j] + y - b),                  (14)
--  j in C              t'max j in C
--
-- and finally we have the cut inequality in the standard form:
--
--    sum x[j] + alfa * y <= beta,                                  (15)
--  j in C
--
-- where:
--
--    alfa = 1 / t'max,                                             (16)
--
--    beta = |C| - alfa *  (sum a[j] - b).                          (17)
--                        j in C                                      */

#define MAXTRY 10000

static int cover2(int n, double a[], double b, double u, double x[],
      double y, int cov[], double *_alfa, double *_beta)
{     /* try to generate mixed cover cut using two-element cover */
      int i, j, try = 0, ret = 0;
      double eps, alfa, beta, temp, rmax = 0.001;
      eps = 0.001 * (1.0 + fabs(b));
      for (i = 0+1; i <= n; i++)
      for (j = i+1; j <= n; j++)
      {  /* C = {i, j} */
         try++;
         if (try > MAXTRY) goto done;
         /* check if condition (8) is satisfied */
         if (a[i] + a[j] + y > b + eps)
         {  /* compute parameters for inequality (15) */
            temp = a[i] + a[j] - b;
            alfa = 1.0 / (temp + u);
            beta = 2.0 - alfa * temp;
            /* compute violation of inequality (15) */
            temp = x[i] + x[j] + alfa * y - beta;
            /* choose C providing maximum violation */
            if (rmax < temp)
            {  rmax = temp;
               cov[1] = i;
               cov[2] = j;
               *_alfa = alfa;
               *_beta = beta;
               ret = 1;
            }
         }
      }
done: return ret;
}

static int cover3(int n, double a[], double b, double u, double x[],
      double y, int cov[], double *_alfa, double *_beta)
{     /* try to generate mixed cover cut using three-element cover */
      int i, j, k, try = 0, ret = 0;
      double eps, alfa, beta, temp, rmax = 0.001;
      eps = 0.001 * (1.0 + fabs(b));
      for (i = 0+1; i <= n; i++)
      for (j = i+1; j <= n; j++)
      for (k = j+1; k <= n; k++)
      {  /* C = {i, j, k} */
         try++;
         if (try > MAXTRY) goto done;
         /* check if condition (8) is satisfied */
         if (a[i] + a[j] + a[k] + y > b + eps)
         {  /* compute parameters for inequality (15) */
            temp = a[i] + a[j] + a[k] - b;
            alfa = 1.0 / (temp + u);
            beta = 3.0 - alfa * temp;
            /* compute violation of inequality (15) */
            temp = x[i] + x[j] + x[k] + alfa * y - beta;
            /* choose C providing maximum violation */
            if (rmax < temp)
            {  rmax = temp;
               cov[1] = i;
               cov[2] = j;
               cov[3] = k;
               *_alfa = alfa;
               *_beta = beta;
               ret = 1;
            }
         }
      }
done: return ret;
}

static int cover4(int n, double a[], double b, double u, double x[],
      double y, int cov[], double *_alfa, double *_beta)
{     /* try to generate mixed cover cut using four-element cover */
      int i, j, k, l, try = 0, ret = 0;
      double eps, alfa, beta, temp, rmax = 0.001;
      eps = 0.001 * (1.0 + fabs(b));
      for (i = 0+1; i <= n; i++)
      for (j = i+1; j <= n; j++)
      for (k = j+1; k <= n; k++)
      for (l = k+1; l <= n; l++)
      {  /* C = {i, j, k, l} */
         try++;
         if (try > MAXTRY) goto done;
         /* check if condition (8) is satisfied */
         if (a[i] + a[j] + a[k] + a[l] + y > b + eps)
         {  /* compute parameters for inequality (15) */
            temp = a[i] + a[j] + a[k] + a[l] - b;
            alfa = 1.0 / (temp + u);
            beta = 4.0 - alfa * temp;
            /* compute violation of inequality (15) */
            temp = x[i] + x[j] + x[k] + x[l] + alfa * y - beta;
            /* choose C providing maximum violation */
            if (rmax < temp)
            {  rmax = temp;
               cov[1] = i;
               cov[2] = j;
               cov[3] = k;
               cov[4] = l;
               *_alfa = alfa;
               *_beta = beta;
               ret = 1;
            }
         }
      }
done: return ret;
}

static int cover(int n, double a[], double b, double u, double x[],
      double y, int cov[], double *alfa, double *beta)
{     /* try to generate mixed cover cut;
         input (see (5)):
         n        is the number of binary variables;
         a[1:n]   are coefficients at binary variables;
         b        is the right-hand side;
         u        is upper bound of continuous variable;
         x[1:n]   are values of binary variables at current point;
         y        is value of continuous variable at current point;
         output (see (15), (16), (17)):
         cov[1:r] are indices of binary variables included in cover C,
                  where r is the set cardinality returned on exit;
         alfa     coefficient at continuous variable;
         beta     is the right-hand side; */
      int j;
      /* perform some sanity checks */
      xassert(n >= 2);
      for (j = 1; j <= n; j++) xassert(a[j] > 0.0);
      xassert(b > 0.0);
      xassert(u >= 0.0);
      for (j = 1; j <= n; j++) xassert(0.0 <= x[j] && x[j] <= 1.0);
      xassert(0.0 <= y && y <= u);
      /* try to generate mixed cover cut */
      if (cover2(n, a, b, u, x, y, cov, alfa, beta)) return 2;
      if (cover3(n, a, b, u, x, y, cov, alfa, beta)) return 3;
      if (cover4(n, a, b, u, x, y, cov, alfa, beta)) return 4;
      return 0;
}

/*----------------------------------------------------------------------
-- lpx_cover_cut - generate mixed cover cut.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- int lpx_cover_cut(LPX *lp, int len, int ind[], double val[],
--    double work[]);
--
-- DESCRIPTION
--
-- The routine lpx_cover_cut generates a mixed cover cut for a given
-- row of the MIP problem.
--
-- The given row of the MIP problem should be explicitly specified in
-- the form:
--
--    sum{j in J} a[j]*x[j] <= b.                                    (1)
--
-- On entry indices (ordinal numbers) of structural variables, which
-- have non-zero constraint coefficients, should be placed in locations
-- ind[1], ..., ind[len], and corresponding constraint coefficients
-- should be placed in locations val[1], ..., val[len]. The right-hand
-- side b should be stored in location val[0].
--
-- The working array work should have at least nb locations, where nb
-- is the number of binary variables in (1).
--
-- The routine generates a mixed cover cut in the same form as (1) and
-- stores the cut coefficients and right-hand side in the same way as
-- just described above.
--
-- RETURNS
--
-- If the cutting plane has been successfully generated, the routine
-- returns 1 <= len' <= n, which is the number of non-zero coefficients
-- in the inequality constraint. Otherwise, the routine returns zero. */

int lpx_cover_cut(LPX *lp, int len, int ind[], double val[],
      double work[])
{     int cov[1+4], j, k, nb, newlen, r;
      double f_min, f_max, alfa, beta, u, *x = work, y;
      /* substitute and remove fixed variables */
      newlen = 0;
      for (k = 1; k <= len; k++)
      {  j = ind[k];
         if (lpx_get_col_type(lp, j) == LPX_FX)
            val[0] -= val[k] * lpx_get_col_lb(lp, j);
         else
         {  newlen++;
            ind[newlen] = ind[k];
            val[newlen] = val[k];
         }
      }
      len = newlen;
      /* move binary variables to the beginning of the list so that
         elements 1, 2, ..., nb correspond to binary variables, and
         elements nb+1, nb+2, ..., len correspond to rest variables */
      nb = 0;
      for (k = 1; k <= len; k++)
      {  j = ind[k];
         if (lpx_get_col_kind(lp, j) == LPX_IV &&
             lpx_get_col_type(lp, j) == LPX_DB &&
             lpx_get_col_lb(lp, j) == 0.0 &&
             lpx_get_col_ub(lp, j) == 1.0)
         {  /* binary variable */
            int ind_k;
            double val_k;
            nb++;
            ind_k = ind[nb], val_k = val[nb];
            ind[nb] = ind[k], val[nb] = val[k];
            ind[k] = ind_k, val[k] = val_k;
         }
      }
      /* now the specified row has the form:
         sum a[j]*x[j] + sum a[j]*y[j] <= b,
         where x[j] are binary variables, y[j] are rest variables */
      /* at least two binary variables are needed */
      if (nb < 2) return 0;
      /* compute implied lower and upper bounds for sum a[j]*y[j] */
      f_min = f_max = 0.0;
      for (k = nb+1; k <= len; k++)
      {  j = ind[k];
         /* both bounds must be finite */
         if (lpx_get_col_type(lp, j) != LPX_DB) return 0;
         if (val[k] > 0.0)
         {  f_min += val[k] * lpx_get_col_lb(lp, j);
            f_max += val[k] * lpx_get_col_ub(lp, j);
         }
         else
         {  f_min += val[k] * lpx_get_col_ub(lp, j);
            f_max += val[k] * lpx_get_col_lb(lp, j);
         }
      }
      /* sum a[j]*x[j] + sum a[j]*y[j] <= b ===>
         sum a[j]*x[j] + (sum a[j]*y[j] - f_min) <= b - f_min ===>
         sum a[j]*x[j] + y <= b - f_min,
         where y = sum a[j]*y[j] - f_min;
         note that 0 <= y <= u, u = f_max - f_min */
      /* determine upper bound of y */
      u = f_max - f_min;
      /* determine value of y at the current point */
      y = 0.0;
      for (k = nb+1; k <= len; k++)
      {  j = ind[k];
         y += val[k] * lpx_get_col_prim(lp, j);
      }
      y -= f_min;
      if (y < 0.0) y = 0.0;
      if (y > u) y = u;
      /* modify the right-hand side b */
      val[0] -= f_min;
      /* now the transformed row has the form:
         sum a[j]*x[j] + y <= b, where 0 <= y <= u */
      /* determine values of x[j] at the current point */
      for (k = 1; k <= nb; k++)
      {  j = ind[k];
         x[k] = lpx_get_col_prim(lp, j);
         if (x[k] < 0.0) x[k] = 0.0;
         if (x[k] > 1.0) x[k] = 1.0;
      }
      /* if a[j] < 0, replace x[j] by its complement 1 - x'[j] */
      for (k = 1; k <= nb; k++)
      {  if (val[k] < 0.0)
         {  ind[k] = - ind[k];
            val[k] = - val[k];
            val[0] += val[k];
            x[k] = 1.0 - x[k];
         }
      }
      /* try to generate a mixed cover cut for the transformed row */
      r = cover(nb, val, val[0], u, x, y, cov, &alfa, &beta);
      if (r == 0) return 0;
      xassert(2 <= r && r <= 4);
      /* now the cut is in the form:
         sum{j in C} x[j] + alfa * y <= beta */
      /* store the right-hand side beta */
      ind[0] = 0, val[0] = beta;
      /* restore the original ordinal numbers of x[j] */
      for (j = 1; j <= r; j++) cov[j] = ind[cov[j]];
      /* store cut coefficients at binary variables complementing back
         the variables having negative row coefficients */
      xassert(r <= nb);
      for (k = 1; k <= r; k++)
      {  if (cov[k] > 0)
         {  ind[k] = +cov[k];
            val[k] = +1.0;
         }
         else
         {  ind[k] = -cov[k];
            val[k] = -1.0;
            val[0] -= 1.0;
         }
      }
      /* substitute y = sum a[j]*y[j] - f_min */
      for (k = nb+1; k <= len; k++)
      {  r++;
         ind[r] = ind[k];
         val[r] = alfa * val[k];
      }
      val[0] += alfa * f_min;
      xassert(r <= len);
      len = r;
      return len;
}

/* eof */
