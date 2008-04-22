/* glpios03.c (under construction so far) */

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

#define _GLPSTD_STDIO
#include "glpios.h"

/***********************************************************************
*  show_progress - display progress of the search
*
*  This routine displays some information about progress of the search.
*
*  The information includes:
*
*  the current number of iterations performed by the simplex solver;
*
*  the objective value for the best known integer feasible solution,
*  which is upper (minimization) or lower (maximization) global bound
*  for optimal solution of the original mip problem;
*
*  the best local bound for active nodes, which is lower (minimization)
*  or upper (maximization) global bound for optimal solution of the
*  original mip problem;
*
*  the relative mip gap, in percents;
*
*  the number of open (active) subproblems;
*
*  the number of completely explored subproblems, i.e. whose nodes have
*  been removed from the tree. */

static void show_progress(glp_tree *tree, int bingo)
{     int p;
      double temp;
      char best_mip[50], best_bound[50], *rho, rel_gap[50];
      /* format the best known integer feasible solution */
      if (tree->mip->mip_stat == GLP_FEAS)
         sprintf(best_mip, "%17.9e", tree->mip->mip_obj);
      else
         sprintf(best_mip, "%17s", "not found yet");
      /* determine reference number of an active subproblem whose local
         bound is best */
      p = ios_best_node(tree);
      /* format the best bound */
      if (p == 0)
         sprintf(best_bound, "%17s", "tree is empty");
      else
      {  temp = tree->slot[p].node->bound;
         if (temp == -DBL_MAX)
            sprintf(best_bound, "%17s", "-inf");
         else if (temp == +DBL_MAX)
            sprintf(best_bound, "%17s", "+inf");
         else
            sprintf(best_bound, "%17.9e", temp);
      }
      /* choose the relation sign between global bounds */
      switch (tree->mip->dir)
      {  case GLP_MIN: rho = ">="; break;
         case GLP_MAX: rho = "<="; break;
         default: xassert(tree != tree);
      }
      /* format the relative mip gap */
      temp = ios_relative_gap(tree);
      if (temp == 0.0)
         sprintf(rel_gap, "  0.0%%");
      else if (temp < 0.001)
         sprintf(rel_gap, "< 0.1%%");
      else if (temp <= 9.999)
         sprintf(rel_gap, "%5.1f%%", 100.0 * temp);
      else
         sprintf(rel_gap, "%6s", "");
      /* display progress of the search */
      xprintf("+%6d: %s %s %s %s %s (%d; %d)\n",
         tree->mip->it_cnt, bingo ? ">>>>>" : "mip =", best_mip, rho,
         best_bound, rel_gap, tree->a_cnt, tree->t_cnt - tree->n_cnt);
      tree->tm_lag = xtime();
      return;
}

/***********************************************************************
*  set_local_bound - set local bound for current subproblem
*
*  This routine sets the local bound for integer optimal solution of
*  the current subproblem, which is optimal solution to LP relaxation.
*  If the latter is fractional while the objective function is known to
*  be integral for any integer feasible point, the bound is strengthen
*  by rounding up (minimization) or down (maximization). */

static void set_local_bound(glp_tree *tree)
{     double bound;
      bound = tree->mip->obj_val;
      if (fabs(bound) < 1e-8) bound = 0.0;
#if 0
      if (tree->int_obj)
      {  /* the objective function is known to be integral */
         double temp;
         temp = floor(bound + 0.5);
         if (temp - 1e-5 <= bound && bound <= temp + 1e-5)
            bound = temp;
         else
         {  switch (tree->mip->dir)
            {  case GLP_MIN:
                  bound = ceil(bound); break;
               case GLP_MAX:
                  bound = floor(bound); break;
               default:
                  xassert(tree != tree);
            }
         }
      }
#endif
      xassert(tree->curr != NULL);
      tree->curr->bound = bound;
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
         xprintf("Local bound is %.9e\n", bound);
      return;
}

/***********************************************************************
*  is_branch_hopeful - check if specified branch is hopeful
*
*  This routine checks if the specified subproblem can have an integer
*  optimal solution which is better than the best known one.
*
*  The check is based on comparison of the local objective bound stored
*  in the subproblem descriptor and the incumbent objective value which
*  is the global objective bound.
*
*  If there is a chance that the specified subproblem can have a better
*  integer optimal solution, the routine returns non-zero. Otherwise, if
*  the corresponding branch can pruned, zero is returned. */

static int is_branch_hopeful(glp_tree *tree, int p)
{     int ret = 1;
      if (tree->mip->mip_stat == GLP_FEAS)
      {  double bound, eps;
         xassert(1 <= p && p <= tree->nslots);
         xassert(tree->slot[p].node != NULL);
         bound = tree->slot[p].node->bound;
         eps = tree->parm->tol_obj * (1.0 + fabs(tree->mip->mip_obj));
         switch (tree->mip->dir)
         {  case GLP_MIN:
               if (bound >= tree->mip->mip_obj - eps) ret = 0;
               break;
            case GLP_MAX:
               if (bound <= tree->mip->mip_obj + eps) ret = 0;
               break;
            default:
               xassert(tree != tree);
         }
      }
      return ret;
}

static int is_curr_node_hopeful(glp_tree *tree)
{     int ret = 1;
      xassert(tree->curr != NULL);
      switch (tree->mip->dir)
      {  case GLP_MIN:
            if (tree->curr->bound == +DBL_MAX)
               ret = 0;
            else if (tree->mip->mip_stat == GLP_FEAS)
            {  double eps = tree->parm->tol_obj * (1.0 +
                  fabs(tree->mip->mip_obj));
               if (tree->curr->bound >= tree->mip->mip_obj - eps)
                  ret = 0;
            }
            break;
         case GLP_MAX:
            if (tree->curr->bound == -DBL_MAX)
               ret = 0;
            else if (tree->mip->mip_stat == GLP_FEAS)
            {  double eps = tree->parm->tol_obj * (1.0 +
                  fabs(tree->mip->mip_obj));
               if (tree->curr->bound <= tree->mip->mip_obj + eps)
                  ret = 0;
            }
            break;
         default:
            xassert(tree != tree);
      }
      return ret;
}

/***********************************************************************
*  check_integrality - check integrality of basic solution
*
*  This routine checks if the basic solution of LP relaxation of the
*  current subproblem satisfies to integrality conditions, i.e. that all
*  variables of integer kind have integral primal values. (The solution
*  is assumed to be optimal.)
*
*  For each variable of integer kind the routine computes the following
*  quantity:
*
*     ii(x[j]) = min(x[j] - floor(x[j]), ceil(x[j]) - x[j]),         (1)
*
*  which is a measure of the integer infeasibility (non-integrality) of
*  x[j] (for example, ii(2.1) = 0.1, ii(3.7) = 0.3, ii(5.0) = 0). It is
*  understood that 0 <= ii(x[j]) <= 0.5, and variable x[j] is integer
*  feasible if ii(x[j]) = 0. However, due to floating-point arithmetic
*  the routine checks less restrictive condition:
*
*     ii(x[j]) <= tol_int,                                           (2)
*
*  where tol_int is a given tolerance (small positive number) and marks
*  each variable which does not satisfy to (2) as integer infeasible by
*  setting its fractionality flag.
*
*  In order to characterize integer infeasibility of the basic solution
*  in the whole the routine computes two parameters: ii_cnt, which is
*  the number of variables with the fractionality flag set, and ii_sum,
*  which is the sum of integer infeasibilities (1). */

static void check_integrality(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int j, type, ii_cnt = 0;
      double lb, ub, x, temp1, temp2, ii_sum = 0.0;
      /* walk through the set of columns (structural variables) */
      for (j = 1; j <= mip->n; j++)
      {  GLPCOL *col = mip->col[j];
         tree->non_int[j] = 0;
         /* if the column is not integer, skip it */
         if (col->kind != GLP_IV) continue;
         /* if the column is non-basic, it is integer feasible */
         if (col->stat != GLP_BS) continue;
         /* obtain the type and bounds of the column */
         type = col->type, lb = col->lb, ub = col->ub;
         /* obtain value of the column in optimal basic solution */
         x = col->prim;
         /* if the column's primal value is close to the lower bound,
            the column is integer feasible within given tolerance */
         if (type == GLP_LO || type == GLP_DB || type == GLP_FX)
         {  temp1 = lb - tree->parm->tol_int;
            temp2 = lb + tree->parm->tol_int;
            if (temp1 <= x && x <= temp2) continue;
            /* the lower bound must not be violated */
            xassert(x >= lb);
         }
         /* if the column's primal value is close to the upper bound,
            the column is integer feasible within given tolerance */
         if (type == GLP_UP || type == GLP_DB || type == GLP_FX)
         {  temp1 = ub - tree->parm->tol_int;
            temp2 = ub + tree->parm->tol_int;
            if (temp1 <= x && x <= temp2) continue;
            /* the upper bound must not be violated */
            xassert(x <= ub);
         }
         /* if the column's primal value is close to nearest integer,
            the column is integer feasible within given tolerance */
         temp1 = floor(x + 0.5) - tree->parm->tol_int;
         temp2 = floor(x + 0.5) + tree->parm->tol_int;
         if (temp1 <= x && x <= temp2) continue;
         /* otherwise the column is integer infeasible */
         tree->non_int[j] = 1;
         /* increase the number of fractional-valued columns */
         ii_cnt++;
         /* compute the sum of integer infeasibilities */
         temp1 = x - floor(x);
         temp2 = ceil(x) - x;
         xassert(temp1 > 0.0 && temp2 > 0.0);
         ii_sum += (temp1 <= temp2 ? temp1 : temp2);
      }
      /* store ii_cnt and ii_sum in the current problem descriptor */
      xassert(tree->curr != NULL);
      tree->curr->ii_cnt = ii_cnt;
      tree->curr->ii_sum = ii_sum;
      /* and also display these parameters */
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
      {  if (ii_cnt == 0)
            xprintf("There are no fractional columns\n");
         else if (ii_cnt == 1)
            xprintf("There is one fractional column, integer infeasibil"
               "ity is %.3e\n", ii_sum);
         else
            xprintf("There are %d fractional columns, integer infeasibi"
               "lity is %.3e\n", ii_cnt, ii_sum);
      }
      return;
}

/***********************************************************************
*  record_solution - record better integer feasible solution
*
*  This routine records optimal basic solution of LP relaxation of the
*  current subproblem, which being integer feasible is better than the
*  best known integer feasible solution. */

static void record_solution(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int i, j;
      mip->mip_stat = GLP_FEAS;
      mip->mip_obj = mip->obj_val;
      for (i = 1; i <= mip->m; i++)
      {  GLPROW *row = mip->row[i];
         row->mipx = row->prim;
      }
      for (j = 1; j <= mip->n; j++)
      {  GLPCOL *col = mip->col[j];
         switch (col->kind)
         {  case GLP_CV:
               col->mipx = col->prim;
               break;
            case GLP_IV:
               /* value of the integer column must be integral */
               col->mipx = floor(col->prim + 0.5);
               break;
            default:
               xassert(col != col);
         }
      }
      tree->sol_cnt++;
      return;
}

/***********************************************************************
*  fix_by_red_cost - fix non-basic integer columns by reduced costs
*
*  This routine fixes some non-basic integer columns if their reduced
*  costs indicate that increasing (decreasing) the column at least by
*  one involves the objective value becoming worse than the incumbent
*  objective value (i.e. the global bound). */

static void fix_by_red_cost(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int j, stat, fixed = 0;
      double obj, lb, ub, dj;
      /* the global bound must exist */
      xassert(tree->mip->mip_stat == GLP_FEAS);
      /* basic solution of LP relaxation must be optimal */
      xassert(mip->pbs_stat == GLP_FEAS && mip->dbs_stat == GLP_FEAS);
      /* determine the objective function value */
      obj = mip->obj_val;
      /* walk through the column list */
      for (j = 1; j <= mip->n; j++)
      {  GLPCOL *col = mip->col[j];
         /* if the column is not integer, skip it */
         if (col->kind != GLP_IV) continue;
         /* obtain bounds of j-th column */
         lb = col->lb, ub = col->ub;
         /* and determine its status and reduced cost */
         stat = col->stat, dj = col->dual;
         /* analyze the reduced cost */
         switch (mip->dir)
         {  case GLP_MIN:
               /* minimization */
               if (stat == GLP_NL)
               {  /* j-th column is non-basic on its lower bound */
                  if (dj < 0.0) dj = 0.0;
                  if (obj + dj >= mip->mip_obj)
                     glp_set_col_bnds(mip, j, GLP_FX, lb, lb), fixed++;
               }
               else if (stat == GLP_NU)
               {  /* j-th column is non-basic on its upper bound */
                  if (dj > 0.0) dj = 0.0;
                  if (obj - dj >= mip->mip_obj)
                     glp_set_col_bnds(mip, j, GLP_FX, ub, ub), fixed++;
               }
               break;
            case GLP_MAX:
               /* maximization */
               if (stat == GLP_NL)
               {  /* j-th column is non-basic on its lower bound */
                  if (dj > 0.0) dj = 0.0;
                  if (obj + dj <= mip->mip_obj)
                     glp_set_col_bnds(mip, j, GLP_FX, lb, lb), fixed++;
               }
               else if (stat == GLP_NU)
               {  /* j-th column is non-basic on its upper bound */
                  if (dj < 0.0) dj = 0.0;
                  if (obj - dj <= mip->mip_obj)
                     glp_set_col_bnds(mip, j, GLP_FX, ub, ub), fixed++;
               }
               break;
            default:
               xassert(tree != tree);
         }
      }
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
      {  if (fixed == 0)
            /* nothing to say */;
         else if (fixed == 1)
            xprintf("One column has been fixed by reduced cost\n");
         else
            xprintf("%d columns have been fixed by reduced costs\n",
               fixed);
      }
      /* fixing non-basic columns on their current bounds must not
         change the basic solution */
      xassert(mip->pbs_stat == GLP_FEAS && mip->dbs_stat == GLP_FEAS);
      return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

#define int_col(j) (lpx_get_col_kind(tree->mip, j) == LPX_IV)

/*----------------------------------------------------------------------
-- branch_on - perform branching on specified column.
--
-- This routine performs branching on j-th column (structural variable)
-- of the current subproblem. The specified column must be of integer
-- kind and must have a fractional value in optimal basic solution of
-- LP relaxation of the current subproblem (i.e. only columns for which
-- the flag non_int[j] is set are valid candidates to branch on).
--
-- Let x be j-th structural variable, and beta be its primal fractional
-- value in the current basic solution. Branching on j-th variable is
-- dividing the current subproblem into two new subproblems, which are
-- identical to the current subproblem with the following exception: in
-- the first subproblem that begins the down-branch x has a new upper
-- bound x <= floor(beta), and in the second subproblem that begins the
-- up-branch x has a new lower bound x >= ceil(beta).
--
-- The parameter next specifies which subproblem should be solved next
-- to continue the search:
--
-- -1 means that one which begins the down-branch;
-- +1 means that one which begins the up-branch. */

static void branch_on(glp_tree *tree, int j, int next)
{     glp_prob *lp = tree->mip;
      int p, type, clone[1+2];
      double beta, lb, ub, new_lb, new_ub;
      /* check input parameters for correctness */
      xassert(1 <= j && j <= lp->n);
      xassert(tree->non_int[j]);
      xassert(next == -1 || next == +1 || next == 0);
      /* obtain primal value of j-th column in basic solution */
      beta = lpx_get_col_prim(lp, j);
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
         xprintf("Branching on column %d, primal value is %.9e\n",
            j, beta);
      /* determine the reference number of the current subproblem */
      xassert(tree->curr != NULL);
      p = tree->curr->p;
      /* freeze the current subproblem */
      ios_freeze_node(tree);
      /* create two clones of the current subproblem; the first clone
         begins the down-branch, the second one begins the up-branch */
      ios_clone_node(tree, p, 2, clone);
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
         xprintf(
            "Node %d begins down branch, node %d begins up branch\n",
            clone[1], clone[2]);
      /* set new upper bound of j-th column in the first subproblem */
      ios_revive_node(tree, clone[1]);
      type = lpx_get_col_type(lp, j);
      lb = lpx_get_col_lb(lp, j);
      ub = lpx_get_col_ub(lp, j);
      new_ub = floor(beta);
      switch (type)
      {  case LPX_FR:
            type = LPX_UP;
            break;
         case LPX_LO:
            xassert(lb <= new_ub);
            type = (lb < new_ub ? LPX_DB : LPX_FX);
            break;
         case LPX_UP:
            xassert(new_ub <= ub - 1.0);
            break;
         case LPX_DB:
            xassert(lb <= new_ub && new_ub <= ub - 1.0);
            type = (lb < new_ub ? LPX_DB : LPX_FX);
            break;
         default:
            xassert(type != type);
      }
      lpx_set_col_bnds(lp, j, type, lb, new_ub);
      ios_freeze_node(tree);
      /* set new lower bound of j-th column in the second subproblem */
      ios_revive_node(tree, clone[2]);
      type = lpx_get_col_type(lp, j);
      lb = lpx_get_col_lb(lp, j);
      ub = lpx_get_col_ub(lp, j);
      new_lb = ceil(beta);
      switch (type)
      {  case LPX_FR:
            type = LPX_LO;
            break;
         case LPX_LO:
            xassert(lb + 1.0 <= new_lb);
            break;
         case LPX_UP:
            xassert(new_lb <= ub);
            type = (new_lb < ub ? LPX_DB : LPX_FX);
            break;
         case LPX_DB:
            xassert(lb + 1.0 <= new_lb && new_lb <= ub);
            type = (new_lb < ub ? LPX_DB : LPX_FX);
            break;
         default:
            xassert(type != type);
      }
      lpx_set_col_bnds(lp, j, type, new_lb, ub);
      ios_freeze_node(tree);
      /* revive the subproblem to be solved next */
      if (next != 0)
         ios_revive_node(tree, clone[next < 0 ? 1 : 2]);
      return;
}

/*----------------------------------------------------------------------
-- branch_first - choose first branching variable.
--
-- This routine looks up the list of structural variables and chooses
-- the first one, which is of integer kind and has fractional value in
-- optimal solution of the current LP relaxation.
--
-- This routine also selects the branch to be solved next where integer
-- infeasibility of the chosen variable is less than in other one. */

static void branch_first(glp_tree *tree)
{     glp_prob *lp = tree->mip;
      int n = lp->n;
      int j, next;
      double beta;
      /* choose the column to branch on */
      for (j = 1; j <= n; j++)
         if (tree->non_int[j]) break;
      xassert(1 <= j && j <= n);
      /* select the branch to be solved next */
      beta = lpx_get_col_prim(lp, j);
      if (beta - floor(beta) < ceil(beta) - beta)
         next = -1; /* down branch */
      else
         next = +1; /* up branch */
      /* perform branching */
      branch_on(tree, j, next);
      return;
}

/*----------------------------------------------------------------------
-- branch_last - choose last branching variable.
--
-- This routine looks up the list of structural variables and chooses
-- the last one, which is of integer kind and has fractional value in
-- optimal solution of the current LP relaxation.
--
-- This routine also selects the branch to be solved next where integer
-- infeasibility of the chosen variable is less than in other one. */

static void branch_last(glp_tree *tree)
{     glp_prob *lp = tree->mip;
      int n = lp->n;
      int j, next;
      double beta;
      /* choose the column to branch on */
      for (j = n; j >= 1; j--)
         if (tree->non_int[j]) break;
      xassert(1 <= j && j <= n);
      /* select the branch to be solved next */
      beta = lpx_get_col_prim(lp, j);
      if (beta - floor(beta) < ceil(beta) - beta)
         next = -1; /* down branch */
      else
         next = +1; /* up branch */
      /* perform branching */
      branch_on(tree, j, next);
      return;
}

/*----------------------------------------------------------------------
-- branch_drtom - choose branching variable with Driebeck-Tomlin heur.
--
-- This routine chooses a structural variable, which is required to be
-- integral and has fractional value in optimal solution of the current
-- LP relaxation, using a heuristic proposed by Driebeck and Tomlin.
--
-- The routine also selects the branch to be solved next, again due to
-- Driebeck and Tomlin.
--
-- This routine is based on the heuristic proposed in:
--
-- Driebeck N.J. An algorithm for the solution of mixed-integer
-- programming problems, Management Science, 12: 576-87 (1966)
--
-- and improved in:
--
-- Tomlin J.A. Branch and bound methods for integer and non-convex
-- programming, in J.Abadie (ed.), Integer and Nonlinear Programming,
-- North-Holland, Amsterdam, pp. 437-50 (1970).
--
-- Must note that this heuristic is time-expensive, because computing
-- one-step degradation (see the routine below) requires one BTRAN for
-- every fractional-valued structural variable. */

#if 0
struct col { int j; double f; };

static int fcmp(const void *x1, const void *x2)
{     const struct col *c1 = x1, *c2 = x2;
      if (c1->f > c2->f) return -1;
      if (c1->f < c2->f) return +1;
      return 0;
}
#endif

static void branch_drtom(glp_tree *tree)
{     glp_prob *lp = tree->mip;
      int m = lp->m;
      int n = lp->n;
      int *non_int = tree->non_int;
      int j, jj, k, t, next, kase, len, stat, *ind;
      double x, dk, alfa, delta_j, delta_k, delta_z, dz_dn, dz_up,
         dd_dn, dd_up, degrad, *val;
#if 0
      struct col *list;
      int size;
#endif
      /* basic solution of LP relaxation must be optimal */
      xassert(lpx_get_status(lp) == LPX_OPT);
      /* allocate working arrays */
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
#if 0
      list = xcalloc(1+n, sizeof(struct col));
      /* build the list of fractional-valued columns */
      size = 0;
      for (j = 1; j <= n; j++)
      {  if (non_int[j])
         {  double t1, t2;
            size++;
            list[size].j = j;
            x = lpx_get_col_prim(lp, j);
            t1 = x - floor(x);
            t2 = ceil(x) - x;
            list[size].f = (t1 <= t2 ? t1 : t2);
         }
      }
      /* sort the column list in descending fractionality */
      qsort(&list[1], size, sizeof(struct col), fcmp);
      /* limit the number of columns to be considered */
      if (size > 50) size = 50;
#endif
      /* nothing has been chosen so far */
      jj = 0, degrad = -1.0;
      /* walk through the list of columns (structural variables) */
#if 1
      for (j = 1; j <= n; j++)
      {  /* if j-th column is not marked as fractional, skip it */
         if (!non_int[j]) continue;
#else
      while (size > 0)
      {  j = list[size--].j;
#endif
         /* obtain (fractional) value of j-th column in basic solution
            of LP relaxation */
         x = lpx_get_col_prim(lp, j);
         /* since the value of j-th column is fractional, the column is
            basic; compute corresponding row of the simplex table */
         len = lpx_eval_tab_row(lp, m+j, ind, val);
         /* the following fragment computes a change in the objective
            function: delta Z = new Z - old Z, where old Z is the
            objective value in the current optimal basis, and new Z is
            the objective value in the adjacent basis, for two cases:
            1) if new upper bound ub' = floor(x[j]) is introduced for
               j-th column (down branch);
            2) if new lower bound lb' = ceil(x[j]) is introduced for
               j-th column (up branch);
            since in both cases the solution remaining dual feasible
            becomes primal infeasible, one implicit simplex iteration
            is performed to determine the change delta Z;
            it is obvious that new Z, which is never better than old Z,
            is a lower (minimization) or upper (maximization) bound of
            the objective function for down- and up-branches. */
         for (kase = -1; kase <= +1; kase += 2)
         {  /* if kase < 0, the new upper bound of x[j] is introduced;
               in this case x[j] should decrease in order to leave the
               basis and go to its new upper bound */
            /* if kase > 0, the new lower bound of x[j] is introduced;
               in this case x[j] should increase in order to leave the
               basis and go to its new lower bound */
            /* apply the dual ratio test in order to determine which
               auxiliary or structural variable should enter the basis
               to keep dual feasibility */
            k = lpx_dual_ratio_test(lp, len, ind, val, kase, 1e-8);
            /* if no non-basic variable has been chosen, LP relaxation
               of corresponding branch being primal infeasible and dual
               unbounded has no primal feasible solution; in this case
               the change delta Z is formally set to infinity */
            if (k == 0)
            {  delta_z =
                  (tree->mip->dir == GLP_MIN ? +DBL_MAX : -DBL_MAX);
               goto skip;
            }
            /* row of the simplex table that corresponds to non-basic
               variable x[k] choosen by the dual ratio test is:
                  x[j] = ... + alfa * x[k] + ...
               where alfa is the influence coefficient (an element of
               the simplex table row) */
            /* determine the coefficient alfa */
            for (t = 1; t <= len; t++) if (ind[t] == k) break;
            xassert(1 <= t && t <= len);
            alfa = val[t];
            /* since in the adjacent basis the variable x[j] becomes
               non-basic, knowing its value in the current basis we can
               determine its change delta x[j] = new x[j] - old x[j] */
            delta_j = (kase < 0 ? floor(x) : ceil(x)) - x;
            /* and knowing the coefficient alfa we can determine the
               corresponding change delta x[k] = new x[k] - old x[k],
               where old x[k] is a value of x[k] in the current basis,
               and new x[k] is a value of x[k] in the adjacent basis */
            delta_k = delta_j / alfa;
            /* Tomlin noticed that if the variable x[k] is of integer
               kind, its change cannot be less (eventually) than one in
               the magnitude */
            if (k > m && int_col(k-m))
            {  /* x[k] is structural integer variable */
               if (fabs(delta_k - floor(delta_k + 0.5)) > 1e-3)
               {  if (delta_k > 0.0)
                     delta_k = ceil(delta_k);  /* +3.14 -> +4 */
                  else
                     delta_k = floor(delta_k); /* -3.14 -> -4 */
               }
            }
            /* now determine the status and reduced cost of x[k] in the
               current basis */
            if (k <= m)
            {  stat = lpx_get_row_stat(lp, k);
               dk = lpx_get_row_dual(lp, k);
            }
            else
            {  stat = lpx_get_col_stat(lp, k-m);
               dk = lpx_get_col_dual(lp, k-m);
            }
            /* if the current basis is dual degenerative, some reduced
               costs which are close to zero may have wrong sign due to
               round-off errors, so correct the sign of d[k] */
            switch (tree->mip->dir)
            {  case GLP_MIN:
                  if (stat == LPX_NL && dk < 0.0 ||
                      stat == LPX_NU && dk > 0.0 ||
                      stat == LPX_NF) dk = 0.0;
                  break;
               case GLP_MAX:
                  if (stat == LPX_NL && dk > 0.0 ||
                      stat == LPX_NU && dk < 0.0 ||
                      stat == LPX_NF) dk = 0.0;
                  break;
               default:
                  xassert(tree != tree);
            }
            /* now knowing the change of x[k] and its reduced cost d[k]
               we can compute the corresponding change in the objective
               function delta Z = new Z - old Z = d[k] * delta x[k];
               note that due to Tomlin's modification new Z can be even
               worse than in the adjacent basis */
            delta_z = dk * delta_k;
skip:       /* new Z is never better than old Z, therefore the change
               delta Z is always non-negative (in case of minimization)
               or non-positive (in case of maximization) */
            switch (tree->mip->dir)
            {  case GLP_MIN: xassert(delta_z >= 0.0); break;
               case GLP_MAX: xassert(delta_z <= 0.0); break;
               default: xassert(tree != tree);
            }
            /* save the change in the objective fnction for down- and
               up-branches, respectively */
            if (kase < 0) dz_dn = delta_z; else dz_up = delta_z;
         }
         /* thus, in down-branch no integer feasible solution can be
            better than Z + dz_dn, and in up-branch no integer feasible
            solution can be better than Z + dz_up, where Z is value of
            the objective function in the current basis */
         /* following the heuristic by Driebeck and Tomlin we choose a
            column (i.e. structural variable) which provides largest
            degradation of the objective function in some of branches;
            besides, we select the branch with smaller degradation to
            be solved next and keep other branch with larger degradation
            in the active list hoping to minimize the number of further
            backtrackings */
         if (degrad < fabs(dz_dn) || degrad < fabs(dz_up))
         {  jj = j;
            if (fabs(dz_dn) < fabs(dz_up))
            {  /* select down branch to be solved next */
               next = -1;
               degrad = fabs(dz_up);
            }
            else
            {  /* select up branch to be solved next */
               next = +1;
               degrad = fabs(dz_dn);
            }
            /* save the objective changes for printing */
            dd_dn = dz_dn, dd_up = dz_up;
            /* if down- or up-branch has no feasible solution, we does
               not need to consider other candidates (in principle, the
               corresponding branch could be pruned right now) */
            if (degrad == DBL_MAX) break;
         }
      }
      /* free working arrays */
      xfree(ind);
      xfree(val);
#if 0
      xfree(list);
#endif
      /* something must be chosen */
      xassert(1 <= jj && jj <= n);
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
      {  xprintf("branch_drtom: column %d chosen to branch on\n", jj);
         if (fabs(dd_dn) == DBL_MAX)
            xprintf("branch_drtom: down-branch is infeasible\n");
         else
            xprintf("branch_drtom: down-branch bound is %.9e\n",
               lpx_get_obj_val(lp) + dd_dn);
         if (fabs(dd_up) == DBL_MAX)
            xprintf("branch_drtom: up-branch   is infeasible\n");
         else
            xprintf("branch_drtom: up-branch   bound is %.9e\n",
               lpx_get_obj_val(lp) + dd_up);
      }
      /* perform branching */
      branch_on(tree, jj, next);
      return;
}

/*----------------------------------------------------------------------
-- branch_mostf - choose most fractional branching variable.
--
-- This routine looks up the list of structural variables and chooses
-- that one, which is of integer kind and has most fractional value in
-- optimal solution of the current LP relaxation.
--
-- This routine also selects the branch to be solved next where integer
-- infeasibility of the chosen variable is less than in other one. */

static void branch_mostf(glp_tree *tree)
{     glp_prob *lp = tree->mip;
      int n = lp->n;
      int j, jj, next;
      double beta, most, temp;
      /* choose the column to branch on */
      jj = 0, most = DBL_MAX;
      for (j = 1; j <= n; j++)
      {  if (tree->non_int[j])
         {  beta = lpx_get_col_prim(lp, j);
            temp = floor(beta) + 0.5;
            if (most > fabs(beta - temp))
            {  jj = j, most = fabs(beta - temp);
               if (beta < temp)
                  next = -1; /* down branch */
               else
                  next = +1; /* up branch */
            }
         }
      }
      /* perform branching */
      branch_on(tree, jj, next);
      return;
}

/*----------------------------------------------------------------------
-- cleanup_the_tree - prune hopeless branches from the tree.
--
-- This routine walks through the active list and checks the local
-- bound for every active subproblem. If the local bound indicates that
-- the subproblem cannot have integer optimal solution better than the
-- incumbent objective value, the routine deletes such subproblem that,
-- in turn, involves pruning the corresponding branch of the tree. */

static void cleanup_the_tree(glp_tree *tree)
{     IOSNPD *node, *next_node;
      int count = 0;
      /* the global bound must exist */
      xassert(tree->mip->mip_stat == GLP_FEAS);
      /* walk through the list of active subproblems */
      for (node = tree->head; node != NULL; node = next_node)
      {  /* deleting some active problem node may involve deleting its
            parents recursively; however, all its parents being created
            *before* it are always *precede* it in the node list, so
            the next problem node is never affected by such deletion */
         next_node = node->next;
         /* if the branch is hopeless, prune it */
         if (!is_branch_hopeful(tree, node->p))
            ios_delete_node(tree, node->p), count++;
      }
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
      {  if (count == 1)
            xprintf("One hopeless branch has been pruned\n");
         else if (count > 1)
            xprintf("%d hopeless branches have been pruned\n", count);
      }
      return;
}

/*----------------------------------------------------------------------
-- btrack_most_feas - select "most integer feasible" subproblem.
--
-- This routine selects from the active list a subproblem to be solved
-- next, whose parent has minimal sum of integer infeasibilities. */

static void btrack_most_feas(glp_tree *tree)
{     IOSNPD *node;
      int p;
      double best;
      p = 0, best = DBL_MAX;
      for (node = tree->head; node != NULL; node = node->next)
      {  xassert(node->up != NULL);
         if (best > node->up->ii_sum)
            p = node->p, best = node->up->ii_sum;
      }
      ios_revive_node(tree, p);
      return;
}

/*----------------------------------------------------------------------
-- btrack_best_proj - select subproblem with best projection heur.
--
-- This routine selects from the active list a subproblem to be solved
-- next using the best projection heuristic. */

static void btrack_best_proj(glp_tree *tree)
{     IOSNPD *root, *node;
      int p;
      double best, deg, obj;
      /* the global bound must exist */
      xassert(tree->mip->mip_stat == GLP_FEAS);
      /* obtain pointer to the root node, which must exist */
      root = tree->slot[1].node;
      xassert(root != NULL);
      /* deg estimates degradation of the objective function per unit
         of the sum of integer infeasibilities */
      xassert(root->ii_sum > 0.0);
      deg = (tree->mip->mip_obj - root->bound) / root->ii_sum;
      /* nothing has been selected so far */
      p = 0, best = DBL_MAX;
      /* walk through the list of active subproblems */
      for (node = tree->head; node != NULL; node = node->next)
      {  xassert(node->up != NULL);
         /* obj estimates optimal objective value if the sum of integer
            infeasibilities were zero */
         obj = node->up->bound + deg * node->up->ii_sum;
         if (tree->mip->dir == GLP_MAX) obj = - obj;
         /* select the subproblem which has the best estimated optimal
            objective value */
         if (best > obj) p = node->p, best = obj;
      }
      ios_revive_node(tree, p);
      return;
}

#undef int_col
#undef print

static void btrack_best_node(glp_tree *tree)
{     IOSNPD *node, *best = NULL;
      double bound, eps;
      switch (tree->mip->dir)
      {  case GLP_MIN:
            bound = +DBL_MAX;
            for (node = tree->head; node != NULL; node = node->next)
               if (bound > node->bound) bound = node->bound;
            xassert(bound != +DBL_MAX);
            eps = 0.001 * (1.0 + fabs(bound));
            for (node = tree->head; node != NULL; node = node->next)
            {  if (node->bound <= bound + eps)
               {  xassert(node->up != NULL);
                  if (best == NULL ||
                  best->up->ii_sum > node->up->ii_sum) best = node;
               }
            }
            break;
         case GLP_MAX:
            bound = -DBL_MAX;
            for (node = tree->head; node != NULL; node = node->next)
               if (bound < node->bound) bound = node->bound;
            xassert(bound != -DBL_MAX);
            eps = 0.001 * (1.0 + fabs(bound));
            for (node = tree->head; node != NULL; node = node->next)
            {  if (node->bound >= bound - eps)
               {  xassert(node->up != NULL);
                  if (best == NULL ||
                  best->up->ii_sum > node->up->ii_sum) best = node;
               }
            }
            break;
         default:
            xassert(tree != tree);
      }
      xassert(best != NULL);
      ios_revive_node(tree, best->p);
      return;
}

static void write_sol(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int term_out = lib_link_env()->term_out;
      if (tree->parm->msg_lev < GLP_MSG_ALL)
         glp_term_out(GLP_OFF);
      if (tree->parm->fn_sol != NULL)
      {  int m = mip->m;
         mip->m = tree->orig_m;
         glp_write_mip(mip, tree->parm->fn_sol);
         mip->m = m;
      }
      if (tree->parm->msg_lev < GLP_MSG_ALL)
         glp_term_out(term_out);
      return;
}

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/***********************************************************************
*  NAME
*
*  mip_driver - branch-and-bound driver
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  int ios_driver(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine mip_driver is a branch-and-bound driver that manages the
*  process of solving MIP problem instance.
*
*  RETURNS
*
*  0  The MIP problem instance has been successfully solved. This code
*     does not necessarily mean that the solver has found optimal
*     solution. It only means that the solution process was successful.
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

static void separation(glp_tree *tree);

int ios_driver(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int p, p_stat, d_stat, ret;
      xlong_t ttt = tree->tm_beg;
      /* on entry to the B&B driver it is assumed that the active list
         contains the only active (i.e. root) subproblem, which is the
         original MIP problem to be solved */
back: /* at this point the current subproblem does not exist */
      xassert(tree->curr == NULL);
      /* if the active list is empty, the search is finished */
      if (tree->head == NULL)
      {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("Active list is empty!\n");
         xassert(dmp_in_use(tree->pool).lo == 0);
         ret = 0;
         goto done;
      }
#if 1
      tree->just_selected = 1;
#endif
      /* select some active subproblem to be solved next and make it
         current */
      if (tree->parm->cb_func != NULL)
      {  xassert(tree->reason == 0);
         xassert(tree->btrack == NULL);
         tree->reason = GLP_ISELECT;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
         if (tree->btrack != NULL)
         {  /* revive the subproblem selected */
            ios_revive_node(tree, tree->btrack->p);
            tree->btrack = NULL;
            goto loop;
         }
      }
      /****************************************************************/
      if (tree->a_cnt == 1)
      {  xassert(tree->head->next == NULL);
         ios_revive_node(tree, tree->head->p);
      }
      else
      switch (tree->parm->bt_tech)
      {  case GLP_BT_DFS:
            /* depth first search */
            xassert(tree->tail != NULL);
            ios_revive_node(tree, tree->tail->p);
            break;
         case GLP_BT_BFS:
            /* breadth first search */
            xassert(tree->head != NULL);
            ios_revive_node(tree, tree->head->p);
            break;
         case GLP_BT_BLB:
            /* select node with best local bound */
#if 0
            ios_revive_node(tree, ios_best_node(tree));
#else
            btrack_best_node(tree);
#endif
            break;
         case GLP_BT_BPH:
            if (mip->mip_stat == GLP_UNDEF)
            {  /* "most integer feasible" subproblem */
               btrack_most_feas(tree);
            }
            else
            {  /* best projection heuristic */
               btrack_best_proj(tree);
            }
            break;
         default:
            xassert(tree != tree);
      }
#if 0
      /* revive the root subproblem */
      ios_revive_node(tree, 1);
#endif
loop: /* main loop starts here; at this point some subproblem has been
         just selected from the active list and made current */
      xassert(tree->curr != NULL);
      xassert(tree->solved == 0);
#if 1
      tree->round = 0;
#endif
      /* determine the reference number of the current subproblem */
      p = tree->curr->p;
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
      {  int level = tree->slot[p].node->level;
         xprintf("-----------------------------------------------------"
            "-------------------\n");
         xprintf("Processing node %d at level %d\n", p, level);
      }
      /* check if the time limit has been exhausted */
      if (tree->parm->tm_lim < INT_MAX &&
         (double)(tree->parm->tm_lim - 1) <=
         1000.0 * xdifftime(xtime(), tree->tm_beg))
      {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("Time limit exhausted; search terminated\n");
         ret = GLP_ETMLIM;
         goto done;
      }
#if 1
      /* perform basic preprocessing */
      switch (tree->parm->pp_tech)
      {  case GLP_PP_NONE:
            break;
         case GLP_PP_ROOT:
            if (tree->curr->level == 0)
            {  if (ios_preprocess_node(tree, 99))
                  goto fath;
            }
            break;
         case GLP_PP_ALL:
            if (ios_preprocess_node(tree,
               tree->curr->level == 0 ? 99 : 10)) goto fath;
            break;
         default:
            xassert(tree != tree);
      }
#endif
      /* call the user-defined callback routine for preprocessing */
      if (tree->parm->cb_func != NULL)
      {  xassert(tree->reason == 0);
         tree->reason = GLP_IPREPRO;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
#if 1
         if (!is_curr_node_hopeful(tree))
         {  /*xprintf("fathomed by preprocessing\n");*/
            goto fath;
         }
#endif
      }
more: /* minor loop starts here; at this point the current subproblem
         is either to be solved for the first time or to be reoptimized
         due to reformulation */
      /* display current progress of the search */
      if (tree->parm->msg_lev >= GLP_MSG_DBG ||
          tree->parm->msg_lev >= GLP_MSG_ON &&
        (double)(tree->parm->out_frq - 1) <=
            1000.0 * xdifftime(xtime(), tree->tm_lag))
         show_progress(tree, 0);
      if (tree->parm->msg_lev >= GLP_MSG_ALL &&
            xdifftime(xtime(), ttt) >= 60.0)
      {  xlong_t total;
         lib_mem_usage(NULL, NULL, &total, NULL);
         xprintf("Time used: %.1f secs.  Memory used: %.1f Mb.\n",
            xdifftime(xtime(), tree->tm_beg), xltod(total) / 1048576.0);
         ttt = xtime();
      }
      /* solve LP relaxation of the current subproblem */
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
         xprintf("Solving LP relaxation...\n");
      ret = ios_solve_node(tree);
      tree->solved++;
      switch (ret)
      {  case 0:
         case GLP_EOBJLL:
         case GLP_EOBJUL:
            break;
         default:
            if (tree->parm->msg_lev >= GLP_MSG_ERR)
               xprintf("ios_driver: unable to solve current LP relaxati"
                  "on; glp_simplex returned %d\n", ret);
            ret = GLP_EFAIL;
            goto done;
      }
      /* analyze status of the basic solution */
      p_stat = mip->pbs_stat;
      d_stat = mip->dbs_stat;
      if (p_stat == GLP_FEAS && d_stat == GLP_FEAS)
      {  /* LP relaxation has optimal solution */
         if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("Found optimal solution to LP relaxation\n");
      }
      else if (p_stat == GLP_FEAS && d_stat == GLP_NOFEAS)
      {  /* LP relaxation has unbounded solution */
         /* since the current subproblem cannot have a larger feasible
            region than its parent, there is something wrong */
         if (tree->parm->msg_lev >= GLP_MSG_ERR)
            xprintf("ios_driver: current LP relaxation has unbounded so"
               "lution\n");
         ret = GLP_EFAIL;
         goto done;
      }
      else if (p_stat == GLP_INFEAS && d_stat == GLP_FEAS)
      {  /* LP relaxation has no primal solution which is better than
            the incumbent objective value */
         xassert(mip->mip_stat == GLP_FEAS);
         if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("LP relaxation has no solution better than incumben"
               "t objective value\n");
         /* prune the branch */
         goto fath;
      }
      else if (p_stat == GLP_NOFEAS)
      {  /* LP relaxation has no primal feasible solution */
         if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("LP relaxation has no feasible solution\n");
         /* prune the branch */
         goto fath;
      }
      else
      {  /* other cases cannot appear */
         xassert(mip != mip);
      }
      /* at this point basic solution of LP relaxation of the current
         subproblem is optimal */
      xassert(p_stat == GLP_FEAS && d_stat == GLP_FEAS);
      /* thus, it defines a local bound for integer optimal solution of
         the current subproblem */
      set_local_bound(tree);
      /* if the local bound indicates that integer optimal solution of
         the current subproblem cannot be better than the global bound,
         prune the branch */
      if (!is_branch_hopeful(tree, p))
      {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("Current branch is hopeless and can be pruned\n");
         goto fath;
      }
      /* call the user-defined callback routine to generate additional
         rows ("lazy" constraints) */
      if (tree->parm->cb_func != NULL)
      {  /*int old_m = mip->m;*/
         xassert(tree->reason == 0);
         tree->reason = GLP_IROWGEN;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
#if 0
         if (mip->m != old_m)
         {  int nrs = mip->m - old_m;
            xassert(nrs > 0);
            if (tree->parm->msg_lev >= GLP_MSG_DBG)
            {  if (nrs == 1)
                  xprintf("One row has been generated\n");
               else
                  xprintf("%d rows have been generated\n", nrs);
            }
            goto more;
         }
#else
         if (tree->reopt)
         {  tree->reopt = 0;
            goto more;
         }
#endif
      }
      /* check if the basic solution is integer feasible */
      check_integrality(tree);
      /* if the basic solution satisfies to all integrality conditions,
         it is a new, better integer feasible solution */
      if (tree->curr->ii_cnt == 0)
      {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("New integer feasible solution found\n");
         record_solution(tree);
         if (tree->parm->msg_lev >= GLP_MSG_ON)
            show_progress(tree, 1);
         write_sol(tree);
         /* call the user-defined callback routine to make it happy */
         if (tree->parm->cb_func != NULL)
         {  xassert(tree->reason == 0);
            tree->reason = GLP_IBINGO;
            tree->parm->cb_func(tree, tree->parm->cb_info);
            tree->reason = 0;
            if (tree->terminate)
            {  ret = GLP_ESTOP;
               goto done;
            }
         }
#if 1
         if (tree->parm->mip_gap > 0.0 &&
             ios_relative_gap(tree) <= tree->parm->mip_gap)
         {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
               xprintf("Relative gap tolerance reached; search terminat"
                  "ed\n");
            ret = GLP_ETMLIM;
            goto done;
         }
#endif
         /* the current subproblem is fathomed; prune its branch */
         goto fath;
      }
      /* basic solution of LP relaxation of the current subproblem is
         integer infeasible */
      /* try to fix some non-basic structural variables of integer kind
         on their current bounds using reduced costs */
      if (mip->mip_stat == GLP_FEAS)
         fix_by_red_cost(tree);
      /* call the user-defined callback routine to find solution with
         primal heuristic */
      if (tree->parm->cb_func != NULL)
      {  xassert(tree->reason == 0);
         tree->reason = GLP_IHEUR;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
         /* check if the current branch became hopeless */
         if (!is_branch_hopeful(tree, p))
         {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
               xprintf("Current branch became hopeless and can be prune"
                  "d\n");
            goto fath;
         }
      }
      tree->round++;
#if 1
      if (tree->parm->mir_cuts == GLP_ON ||
          tree->parm->gmi_cuts == GLP_ON)
      {  xassert(tree->reason == 0);
         tree->reason = GLP_ICUTGEN;
         separation(tree);
         tree->reason = 0;
         if (tree->reopt)
         {  tree->reopt = 0;
            goto more;
         }
      }
#endif
      /* call the user-defined callback routine to generate cuts */
      if (tree->parm->cb_func != NULL)
      {  /*int old_m = mip->m;*/
         xassert(tree->reason == 0);
         tree->reason = GLP_ICUTGEN;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
#if 0
         if (mip->m != old_m)
         {  int nrs = mip->m - old_m;
            xassert(nrs > 0);
            if (tree->parm->msg_lev >= GLP_MSG_DBG)
            {  if (nrs == 1)
                  xprintf("One cut has been generated\n");
               else
                  xprintf("%d cuts have been generated\n", nrs);
            }
            goto more;
         }
#else
         if (tree->reopt)
         {  tree->reopt = 0;
            goto more;
         }
#endif
      }
#if 1
      tree->just_selected = 0;
#endif
      /* call the user-defined callback routine to choose branching
         variable */
      if (tree->parm->cb_func != NULL)
      {  xassert(tree->reason == 0);
         xassert(tree->br_var == 0);
         xassert(tree->br_sel == 0);
         tree->reason = GLP_IBRANCH;
         tree->parm->cb_func(tree, tree->parm->cb_info);
         tree->reason = 0;
         if (tree->terminate)
         {  ret = GLP_ESTOP;
            goto done;
         }
         if (tree->br_var != 0)
         {  switch (tree->br_sel)
            {  case 'D':
                  branch_on(tree, tree->br_var, -1); break;
               case 'U':
                  branch_on(tree, tree->br_var, +1); break;
               case 'N':
                  branch_on(tree, tree->br_var,  0); break;
               default:
                  xassert(tree != tree);
            }
            tree->br_var = 0;
            tree->br_sel = 0;
            if (tree->curr == NULL) goto back; else goto loop;
         }
      }
      /* perform branching */
      switch (tree->parm->br_tech)
      {  case GLP_BR_FFV:
            /* branch on first fractional variable */
            branch_first(tree);
            break;
         case GLP_BR_LFV:
            /* branch on last fractional variable */
            branch_last(tree);
            break;
         case GLP_BR_MFV:
            /* branch on most fractional variable */
            branch_mostf(tree);
            break;
         case GLP_BR_DTH:
            /* branch using the heuristic by Dreebeck and Tomlin */
            branch_drtom(tree);
            break;
         default:
            xassert(tree != tree);
      }
      /* continue the search from the subproblem which begins down- or
         up-branch (it has been revived by branching routine) */
      xassert(tree->curr != NULL);
      goto loop;
fath: /* the current subproblem has been fathomed */
      if (tree->parm->msg_lev >= GLP_MSG_DBG)
         xprintf("Node %d fathomed\n", p);
      /* freeze the current subproblem */
      ios_freeze_node(tree);
      /* and prune the corresponding branch of the tree */
      ios_delete_node(tree, p);
      /* if a new integer feasible solution has just been found, other
         branches may become hopeless and therefore should be pruned */
      if (mip->mip_stat == GLP_FEAS) cleanup_the_tree(tree);
#if 1
      goto back;
#else
      /* if the active list is empty, the search is finished */
      if (tree->head == NULL)
      {  if (tree->parm->msg_lev >= GLP_MSG_DBG)
            xprintf("Active list is empty!\n");
         xassert(dmp_in_use(tree->pool).lo == 0);
         ret = 0;
         goto done;
      }
      /* perform backtracking */
      switch (tree->bt_tech)
      {  case GLP_BT_DFS:
            /* depth first search */
            xassert(tree->tail != NULL);
            ios_revive_node(tree, tree->tail->p);
            break;
         case GLP_BT_BFS:
            /* breadth first search */
            xassert(tree->head != NULL);
            ios_revive_node(tree, tree->head->p);
            break;
         case GLP_BT_BLB:
            /* select node with best local bound */
            ios_revive_node(tree, ios_best_node(tree));
            break;
         case GLP_BT_BPH:
            if (mip->mip_stat == GLP_UNDEF)
            {  /* "most integer feasible" subproblem */
               btrack_most_feas(tree);
            }
            else
            {  /* best projection heuristic */
               btrack_best_proj(tree);
            }
            break;
         default:
            xassert(tree != tree);
      }
      /* continue the search from the subproblem selected */
      goto loop;
#endif
done: /* display status of the search on exit from the solver */
      if (tree->parm->msg_lev >= GLP_MSG_ON)
         show_progress(tree, 0);
#if 1
      if (tree->mir_gen != NULL)
         ios_mir_term(tree->mir_gen), tree->mir_gen = NULL;
#endif
      /* return to the calling program */
      return ret;
}

/**********************************************************************/

#define cut_factor 0.30
#define max_pool 10
#define ub_min_eff 0.02
#define max_par 0.1

#define first_attempt (tree->first_attempt)
#define max_added_cuts (tree->max_added_cuts)
#define min_eff (tree->min_eff)
#define miss (tree->miss)
#define just_selected (tree->just_selected)

static double efficacy(glp_tree *tree, IOSCUT *cut)
{     glp_prob *mip = tree->mip;
      IOSAIJ *aij;
      double s = 0.0, t = 0.0, temp;
      for (aij = cut->ptr; aij != NULL; aij = aij->next)
      {  xassert(1 <= aij->j && aij->j <= mip->n);
         s += aij->val * mip->col[aij->j]->prim;
         t += aij->val * aij->val;
      }
      temp = sqrt(t);
      if (temp < DBL_EPSILON) temp = DBL_EPSILON;
      switch (cut->type)
      {  case GLP_LO:
            temp = (s >= cut->rhs ? 0.0 : (cut->rhs - s) / temp);
            break;
         case GLP_UP:
            temp = (s <= cut->rhs ? 0.0 : (s - cut->rhs) / temp);
            break;
         default:
            xassert(cut != cut);
      }
      return temp;
}

static double parallel(IOSCUT *a, IOSCUT *b, double work[])
{     IOSAIJ *aij;
      double s = 0.0, sa = 0.0, sb = 0.0, temp;
      for (aij = a->ptr; aij != NULL; aij = aij->next)
      {  work[aij->j] = aij->val;
         sa += aij->val * aij->val;
      }
      for (aij = b->ptr; aij != NULL; aij = aij->next)
      {  s += work[aij->j] * aij->val;
         sb += aij->val * aij->val;
      }
      for (aij = a->ptr; aij != NULL; aij = aij->next)
         work[aij->j] = 0.0;
      temp = sqrt(sa * sb);
      if (temp < DBL_EPSILON) temp = DBL_EPSILON;
      return s / temp;
}

struct cut { IOSCUT *cut; double eff; };

static int fcmp(const void *x1, const void *x2)
{     const struct cut *c1 = x1, *c2 = x2;
      if (c1->eff > c2->eff) return -1;
      if (c1->eff < c2->eff) return +1;
      return 0;
}

static void sort_pool(glp_tree *tree, IOSPOOL *pool)
{     /* sort pool by decreasing efficacy */
      struct cut *cuts = xcalloc(1+pool->size, sizeof(struct cut));
      IOSCUT *cut;
      int c = 0;
      for (cut = pool->head; cut != NULL; cut = cut->next)
         c++, cuts[c].cut = cut, cuts[c].eff = efficacy(tree, cut);
      xassert(c == pool->size);
      qsort(&cuts[1], pool->size, sizeof(struct cut), fcmp);
      pool->head = pool->tail = NULL;
      for (c = 1; c <= pool->size; c++)
      {  cut = cuts[c].cut;
         cut->prev = pool->tail;
         cut->next = NULL;
         if (cut->prev == NULL)
            pool->head = cut;
         else
            cut->prev->next = cut;
         pool->tail = cut;
      }
      xfree(cuts);
      return;
}

static void separation(glp_tree *tree)
{     /* separation strategy */
      IOSPOOL *pool = NULL;
      if (first_attempt)
         max_added_cuts = tree->mip->n;
      if (tree->curr->level > 0 && !just_selected) goto done;
      /* if added_cuts >= max_added_cuts then return */
      if (tree->mip->m - tree->orig_m >= max_added_cuts) goto done;
      /* add to POOL all the cuts violated by x* */
      pool = ios_create_pool(tree);
      if (tree->parm->gmi_cuts == GLP_ON)
      {  if (tree->curr->level == 0 && tree->round == 1 &&
             tree->parm->msg_lev >= GLP_MSG_ALL)
            xprintf("Gomory's cuts enabled\n");
         if (tree->round <= 6)
            ios_gmi_gen(tree, pool);
      }
      if (tree->parm->mir_cuts == GLP_ON)
      {  if (tree->curr->level == 0 && tree->mir_gen == NULL)
            tree->mir_gen = ios_mir_init(tree);
         xassert(tree->mir_gen != NULL);
#ifndef _GLP_FOOBAR
         ios_mir_gen(tree, tree->mir_gen, pool);
#else
         ios_cov_gen(tree, pool);
#endif
      }
      if (pool->size == 0) goto done;
      /* sort POOL by decreasing efficacy; the first cut is the cut
         with largest efficacy */
      sort_pool(tree, pool);
      /* remove the last |POOL| - max_pool cuts from POOL */
      while (pool->size > max_pool)
         ios_del_cut_row(tree, pool, pool->tail);
      /* define the global variable min_eff */
      if (first_attempt)
      {  min_eff = 0.7 * efficacy(tree, pool->head);
         if (min_eff > ub_min_eff) min_eff = ub_min_eff;
         miss = 0;
      }
      if (efficacy(tree, pool->head) < min_eff)
      {  miss++;
         if (miss == 20) miss = 0, min_eff -= 0.03;
         goto done;
      }
      /* adding cuts */
      {  IOSCUT *cut, *next, *ccc;
         int j;
         double *work = xcalloc(1+tree->mip->n, sizeof(double));
         int *ind = xcalloc(1+tree->mip->n, sizeof(int));
         double *val = xcalloc(1+tree->mip->n, sizeof(double));
         for (j = 1; j <= tree->mip->n; j++) work[j] = 0.0;
         for (cut = pool->head; cut != NULL; cut = next)
         {  next = cut->next;
            if (efficacy(tree, cut) < min_eff) break; /* stop adding */
            /* cut must not be parallel !!! */
            for (ccc = pool->head; ccc != cut; ccc = ccc->next)
               if (parallel(cut, ccc, work) > max_par) break;
            if (ccc == cut)
            {  /* cut accepted */
               IOSAIJ *aij;
               int len = 0, i;
               i = glp_add_rows(tree->mip, 1);
               for (aij = cut->ptr; aij != NULL; aij = aij->next)
                  len++, ind[len] = aij->j, val[len] = aij->val;
               glp_set_mat_row(tree->mip, i, len, ind, val);
               xassert(cut->type == GLP_LO || cut->type == GLP_UP);
               glp_set_row_bnds(tree->mip, i, cut->type, cut->rhs,
                  cut->rhs);
            }
            else
            {  /* cut rejected */
               ios_del_cut_row(tree, pool, cut);
            }
         }
         xfree(work);
         xfree(ind);
         xfree(val);
      }
done: if (pool != NULL) ios_delete_pool(tree, pool);
      first_attempt = 0;
      return;
}

/* eof */
