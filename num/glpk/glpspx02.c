/* glpspx02.c (simplex method solver routines) */

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
#include "glpspx.h"

#if 0
#define LPX_E_NOFEAS    209   /* no feasible solution */
#define LPX_E_INSTAB    210   /* numerical instability */
#endif

/*----------------------------------------------------------------------
-- spx_warm_up - "warm up" the initial basis.
--
-- *Synopsis*
--
-- #include "glpspx.h"
-- void spx_warm_up(SPX *spx);
--
-- *Description*
--
-- The routine spx_warm_up "warms up" the initial basis specified by
-- the array tagx. "Warming up" includes (if necessary) reinverting
-- (factorizing) the initial basis matrix, computing the initial basic
-- solution components (values of basic variables, simplex multipliers,
-- reduced costs of non-basic variables), and determining primal and
-- dual statuses of the initial basic solution. */

void spx_warm_up(SPX *spx)
{     /* the basis factorization must be valid */
      xassert(spx->valid);
      /* compute the initial primal solution */
      spx_eval_bbar(spx);
      if (spx_check_bbar(spx, spx->tol_bnd) == 0.0)
         spx->p_stat = GLP_FEAS;
      else
         spx->p_stat = GLP_INFEAS;
      /* compute the initial dual solution */
      spx_eval_pi(spx);
      spx_eval_cbar(spx);
      if (spx_check_cbar(spx, spx->tol_dj) == 0.0)
         spx->d_stat = GLP_FEAS;
      else
         spx->d_stat = GLP_INFEAS;
      return;
}

/*----------------------------------------------------------------------
-- spx_prim_opt - find optimal solution (primal simplex).
--
-- *Synopsis*
--
-- #include "glpspx.h"
-- int spx_prim_opt(SPX *spx);
--
-- *Description*
--
-- The routine spx_prim_opt is intended to find optimal solution of an
-- LP problem using the primal simplex method.
--
-- On entry to the routine the initial basis should be "warmed up" and,
-- moreover, the initial basic solution should be primal feasible.
--
-- Structure of this routine can be an example for other variants based
-- on the primal simplex method.
--
-- *Returns*
--
-- 0  optimal solution found.
--
-- LPX_E_NOFEAS
--    the problem has no dual feasible solution, therefore its primal
--    solution is unbounded.
--
-- GLP_EITLIM
--    iterations limit exceeded.
--
-- GLP_ETMLIM
--    time limit exceeded.
--
-- LPX_E_INSTAB
--    numerical instability; the current basic solution became primal
--    infeasible due to excessive round-off errors.
--
-- GLP_EFAIL
--    singular basis; the current basis matrix became singular or
--    ill-conditioned due to improper simplex iteration. */

static void prim_opt_dpy(SPX *spx)
{     /* this auxiliary routine displays information about the current
         basic solution */
      int i, def = 0;
      for (i = 1; i <= spx->m; i++)
         if (spx->type[spx->indx[i]] == GLP_FX) def++;
      xprintf("*%6d:   objval = %17.9e   infeas = %17.9e (%d)\n",
         spx->it_cnt, spx_eval_obj(spx), spx_check_bbar(spx, 0.0), def);
      return;
}

int spx_prim_opt(SPX *spx)
{     /* find optimal solution (primal simplex) */
      int m = spx->m;
      int n = spx->n;
      int ret;
      xlong_t start = xtime();
      double spent = 0.0;
      /* the initial basis should be "warmed up" */
      xassert(spx->valid);
      xassert(spx->p_stat != GLP_UNDEF && spx->d_stat != GLP_UNDEF);
      /* the initial basic solution should be primal feasible */
      xassert(spx->p_stat == GLP_FEAS);
      /* if the initial basic solution is dual feasible, nothing to
         search for */
      if (spx->d_stat == GLP_FEAS)
      {  ret = 0;
         goto done;
      }
      /* allocate the working segment */
      xassert(spx->meth == 0);
      spx->meth = 'P';
      spx->p = 0;
      spx->p_tag = 0;
      spx->q = 0;
      spx->zeta = xcalloc(1+m, sizeof(double));
      spx->ap = xcalloc(1+n, sizeof(double));
      spx->aq = xcalloc(1+m, sizeof(double));
      spx->gvec = xcalloc(1+n, sizeof(double));
      spx->dvec = NULL;
      spx->refsp = (spx->price ? xcalloc(1+m+n, sizeof(int)) : NULL);
      spx->count = 0;
      spx->work = xcalloc(1+m+n, sizeof(double));
      spx->orig_type = NULL;
      spx->orig_lb = spx->orig_ub = NULL;
      spx->orig_dir = 0;
      spx->orig_coef = NULL;
beg:  /* initialize weights of non-basic variables */
      if (!spx->price)
      {  /* textbook pricing will be used */
         int j;
         for (j = 1; j <= n; j++) spx->gvec[j] = 1.0;
      }
      else
      {  /* steepest edge pricing will be used */
         spx_reset_refsp(spx);
      }
      /* display information about the initial basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) prim_opt_dpy(spx);
      /* main loop starts here */
      for (;;)
      {  /* determine the spent amount of time */
#if 0
         spent = utime() - start;
#else
         spent = xdifftime(xtime(), start);
#endif
         /* display information about the current basic solution */
         if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq == 0 &&
             spx->out_dly <= spent) prim_opt_dpy(spx);
         /* check if the iterations limit has been exhausted */
         if (spx->it_lim == 0)
         {  ret = GLP_EITLIM;
            break;
         }
         /* check if the time limit has been exhausted */
         if (spx->tm_lim >= 0.0 && spx->tm_lim <= spent)
         {  ret = GLP_ETMLIM;
            break;
         }
         /* choose non-basic variable xN[q] */
         if (spx_prim_chuzc(spx, spx->tol_dj))
         {  /* basic solution components were recomputed; check primal
               feasibility */
            if (spx_check_bbar(spx, spx->tol_bnd) != 0.0)
            {  /* the current solution became primal infeasible due to
                  round-off errors */
               ret = LPX_E_INSTAB;
               break;
            }
         }
         /* if no xN[q] has been chosen, the current basic solution is
            dual feasible and therefore optimal */
         if (spx->q == 0)
         {  ret = 0;
            break;
         }
         /* compute the q-th column of the current simplex table (later
            this column will enter the basis) */
         spx_eval_col(spx, spx->q, spx->aq, 1);
         /* choose basic variable xB[p] */
         if (spx_prim_chuzr(spx, spx->relax * spx->tol_bnd))
         {  /* the basis matrix should be reinverted, because the q-th
               column of the simplex table is unreliable */
            xassert("not implemented yet" == NULL);
         }
         /* if no xB[p] has been chosen, the problem is unbounded (has
            no dual feasible solution) */
         if (spx->p == 0)
         {  spx->some = spx->indx[m + spx->q];
            ret = LPX_E_NOFEAS;
            break;
         }
         /* update values of basic variables */
         spx_update_bbar(spx, NULL);
         if (spx->p > 0)
         {  /* compute the p-th row of the inverse inv(B) */
            spx_eval_rho(spx, spx->p, spx->zeta);
            /* compute the p-th row of the current simplex table */
            spx_eval_row(spx, spx->zeta, spx->ap);
            /* update simplex multipliers */
            spx_update_pi(spx);
            /* update reduced costs of non-basic variables */
            spx_update_cbar(spx, 0);
            /* update weights of non-basic variables */
            if (spx->price) spx_update_gvec(spx);
         }
         /* jump to the adjacent vertex of the LP polyhedron */
         if (spx_change_basis(spx))
         {  /* the basis matrix should be reinverted */
            if (spx_invert(spx) != 0)
            {  /* numerical problems with the basis matrix */
               spx->p_stat = spx->d_stat = GLP_UNDEF;
               ret = GLP_EFAIL;
               goto done;
            }
            /* compute the current basic solution components */
            spx_eval_bbar(spx);
            spx_eval_pi(spx);
            spx_eval_cbar(spx);
            /* check primal feasibility */
            if (spx_check_bbar(spx, spx->tol_bnd) != 0.0)
            {  /* the current solution became primal infeasible due to
                  round-off errors */
               ret = LPX_E_INSTAB;
               break;
            }
         }
#if 0
         /* check accuracy of main solution components after updating
            (for debugging purposes only) */
         {  double ae_bbar = spx_err_in_bbar(spx);
            double ae_pi   = spx_err_in_pi(spx);
            double ae_cbar = spx_err_in_cbar(spx, 0);
            double ae_gvec = spx->price ? spx_err_in_gvec(spx) : 0.0;
            print("bbar: %g; pi: %g; cbar: %g; gvec: %g",
               ae_bbar, ae_pi, ae_cbar, ae_gvec);
            if (ae_bbar > 1e-7 || ae_pi > 1e-7 || ae_cbar > 1e-7 ||
                ae_gvec > 1e-3) fault("solution accuracy too low");
         }
#endif
      }
      /* compute the final basic solution components */
      spx_eval_bbar(spx);
      spx_eval_pi(spx);
      spx_eval_cbar(spx);
      if (spx_check_bbar(spx, spx->tol_bnd) == 0.0)
         spx->p_stat = GLP_FEAS;
      else
         spx->p_stat = GLP_INFEAS;
      if (spx_check_cbar(spx, spx->tol_dj) == 0.0)
         spx->d_stat = GLP_FEAS;
      else
         spx->d_stat = GLP_INFEAS;
      /* display information about the final basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) prim_opt_dpy(spx);
      /* correct the preliminary diagnosis */
      switch (ret)
      {  case 0:
            /* assumed p_stat = d_stat = GLP_FEAS */
            if (spx->p_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->d_stat != GLP_FEAS)
            {  /* it seems we need to continue the search */
               goto beg;
            }
            break;
         case GLP_EITLIM:
         case GLP_ETMLIM:
            /* assumed p_stat = GLP_FEAS and d_stat = GLP_INFEAS */
            if (spx->p_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->d_stat == GLP_FEAS)
               ret = 0;
            break;
         case LPX_E_NOFEAS:
            /* assumed p_stat = GLP_FEAS and d_stat = GLP_INFEAS */
            if (spx->p_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->d_stat == GLP_FEAS)
               ret = 0;
            else
               spx->d_stat = GLP_NOFEAS;
            break;
         case LPX_E_INSTAB:
            /* assumed p_stat = GLP_INFEAS */
            if (spx->p_stat == GLP_FEAS)
            {  if (spx->d_stat == GLP_FEAS)
                  ret = 0;
               else
               {  /* it seems we need to continue the search */
                  goto beg;
               }
            }
            break;
         default:
            xassert(ret != ret);
      }
done: /* deallocate the working segment */
      if (spx->meth != 0)
      {  spx->meth = 0;
         xfree(spx->zeta);
         xfree(spx->ap);
         xfree(spx->aq);
         xfree(spx->gvec);
         if (spx->price) xfree(spx->refsp);
         xfree(spx->work);
      }
      /* determine the spent amount of time */
#if 0
      spent = utime() - start;
#else
      spent = xdifftime(xtime(), start);
#endif
      /* decrease the time limit by the spent amount */
      if (spx->tm_lim >= 0.0)
      {  spx->tm_lim -= spent;
         if (spx->tm_lim < 0.0) spx->tm_lim = 0.0;
      }
      /* return to the calling program */
      return ret;
}

/*----------------------------------------------------------------------
-- spx_prim_feas - find primal feasible solution (primal simplex).
--
-- *Synopsis*
--
-- #include "glpspx.h"
-- int spx_prim_feas(SPX *spx);
--
-- *Description*
--
-- The routine spx_prim_feas tries to find primal feasible solution of
-- an LP problem using the method of implicit artificial variables that
-- is based on the primal simplex method (see the comments below).
--
-- On entry to the routine the initial basis should be "warmed up".
--
-- *Returns*
--
-- 0  primal feasible solution found.
--
-- LPX_E_NOFEAS
--    the problem has no primal feasible solution.
--
-- GLP_EITLIM
--    iterations limit exceeded.
--
-- GLP_ETMLIM
--    time limit exceeded.
--
-- LPX_E_INSTAB
--    numerical instability; the current basic solution became primal
--    infeasible due to excessive round-off errors.
--
-- GLP_EFAIL
--    singular basis; the current basis matrix became singular or
--    ill-conditioned due to improper simplex iteration. */

static double orig_objval(SPX *spx)
{     /* this auxliary routine computes value of the objective function
         for the original LP problem */
      double objval;
      void *t;
      t = spx->type, spx->type = spx->orig_type, spx->orig_type = t;
      t = spx->lb, spx->lb = spx->orig_lb, spx->orig_lb = t;
      t = spx->ub, spx->ub = spx->orig_ub, spx->orig_ub = t;
      t = spx->coef, spx->coef = spx->orig_coef, spx->orig_coef = t;
      objval = spx_eval_obj(spx);
      t = spx->type, spx->type = spx->orig_type, spx->orig_type = t;
      t = spx->lb, spx->lb = spx->orig_lb, spx->orig_lb = t;
      t = spx->ub, spx->ub = spx->orig_ub, spx->orig_ub = t;
      t = spx->coef, spx->coef = spx->orig_coef, spx->orig_coef = t;
      return objval;
}

static double orig_infsum(SPX *spx, double tol)
{     /* this auxiliary routine computes the sum of infeasibilities for
         the original LP problem */
      double infsum;
      void *t;
      t = spx->type, spx->type = spx->orig_type, spx->orig_type = t;
      t = spx->lb, spx->lb = spx->orig_lb, spx->orig_lb = t;
      t = spx->ub, spx->ub = spx->orig_ub, spx->orig_ub = t;
      t = spx->coef, spx->coef = spx->orig_coef, spx->orig_coef = t;
      infsum = spx_check_bbar(spx, tol);
      t = spx->type, spx->type = spx->orig_type, spx->orig_type = t;
      t = spx->lb, spx->lb = spx->orig_lb, spx->orig_lb = t;
      t = spx->ub, spx->ub = spx->orig_ub, spx->orig_ub = t;
      t = spx->coef, spx->coef = spx->orig_coef, spx->orig_coef = t;
      return infsum;
}

static void prim_feas_dpy(SPX *spx, double sum_0)
{     /* this auxiliary routine displays information about the current
         basic solution */
      int i, def = 0;
      for (i = 1; i <= spx->m; i++)
         if (spx->type[spx->indx[i]] == GLP_FX) def++;
      xprintf(" %6d:   objval = %17.9e   infeas = %17.9e (%d)\n",
         spx->it_cnt, orig_objval(spx), orig_infsum(spx, 0.0) / sum_0,
         def);
      return;
}

int spx_prim_feas(SPX *spx)
{     /* find primal feasible solution (primal simplex) */
      int m = spx->m;
      int n = spx->n;
      int i, k, ret;
      double sum_0;
      xlong_t start = xtime();
      double spent = 0.0;
      /* the initial basis should be "warmed up" */
      xassert(spx->valid);
      xassert(spx->p_stat != GLP_UNDEF && spx->d_stat != GLP_UNDEF);
      /* if the initial basic solution is primal feasible, nothing to
         search for */
      if (spx->p_stat == GLP_FEAS)
      {  ret = 0;
         goto done;
      }
      /* allocate the working segment */
      xassert(spx->meth == 0);
      spx->meth = 'P';
      spx->p = 0;
      spx->p_tag = 0;
      spx->q = 0;
      spx->zeta = xcalloc(1+m, sizeof(double));
      spx->ap = xcalloc(1+n, sizeof(double));
      spx->aq = xcalloc(1+m, sizeof(double));
      spx->gvec = xcalloc(1+n, sizeof(double));
      spx->dvec = NULL;
      spx->refsp = (spx->price ? xcalloc(1+m+n, sizeof(int)) : NULL);
      spx->count = 0;
      spx->work = xcalloc(1+m+n, sizeof(double));
      spx->orig_type = xcalloc(1+m+n, sizeof(int));
      spx->orig_lb = xcalloc(1+m+n, sizeof(double));
      spx->orig_ub = xcalloc(1+m+n, sizeof(double));
      spx->orig_dir = 0;
      spx->orig_coef = xcalloc(1+m+n, sizeof(double));
      /* save components of the original LP problem, which are changed
         by the routine */
      memcpy(spx->orig_type, spx->type, (1+m+n) * sizeof(int));
      memcpy(spx->orig_lb, spx->lb, (1+m+n) * sizeof(double));
      memcpy(spx->orig_ub, spx->ub, (1+m+n) * sizeof(double));
      spx->orig_dir = spx->dir;
      memcpy(spx->orig_coef, spx->coef, (1+m+n) * sizeof(double));
      /* build an artificial basic solution, which is primal feasible,
         and also build an auxiliary objective function to minimize the
         sum of infeasibilities (residuals) for the original problem */
      spx->dir = GLP_MIN;
      for (k = 0; k <= m+n; k++) spx->coef[k] = 0.0;
      for (i = 1; i <= m; i++)
      {  int type_k;
         double lb_k, ub_k, bbar_i;
         double eps = 0.10 * spx->tol_bnd;
         k = spx->indx[i]; /* x[k] = xB[i] */
         type_k = spx->orig_type[k];
         lb_k = spx->orig_lb[k];
         ub_k = spx->orig_ub[k];
         bbar_i = spx->bbar[i];
         if (type_k == GLP_LO || type_k == GLP_DB || type_k == GLP_FX)
         {  /* in the original problem x[k] has an lower bound */
            if (bbar_i < lb_k - eps)
            {  /* and violates it */
               spx->type[k] = GLP_UP;
               spx->lb[k] = 0.0;
               spx->ub[k] = lb_k;
               spx->coef[k] = -1.0; /* x[k] should be increased */
            }
         }
         if (type_k == GLP_UP || type_k == GLP_DB || type_k == GLP_FX)
         {  /* in the original problem x[k] has an upper bound */
            if (bbar_i > ub_k + eps)
            {  /* and violates it */
               spx->type[k] = GLP_LO;
               spx->lb[k] = ub_k;
               spx->ub[k] = 0.0;
               spx->coef[k] = +1.0; /* x[k] should be decreased */
            }
         }
      }
      /* now the initial basic solution should be primal feasible due
         to changes of bounds of some basic variables, which turned to
         implicit artifical variables */
      xassert(spx_check_bbar(spx, spx->tol_bnd) == 0.0);
      /* compute the initial sum of infeasibilities for the original
         problem */
      sum_0 = orig_infsum(spx, 0.0);
      /* it can't be zero, because the initial basic solution is primal
         infeasible */
      xassert(sum_0 != 0.0);
      /* compute simplex multipliers and reduced costs of non-basic
         variables once again (because the objective function has been
         changed) */
      spx_eval_pi(spx);
      spx_eval_cbar(spx);
      /* initialize weights of non-basic variables */
      if (!spx->price)
      {  /* textbook pricing will be used */
         int j;
         for (j = 1; j <= n; j++) spx->gvec[j] = 1.0;
      }
      else
      {  /* steepest edge pricing will be used */
         spx_reset_refsp(spx);
      }
      /* display information about the initial basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) prim_feas_dpy(spx, sum_0);
      /* main loop starts here */
      for (;;)
      {  /* determine the spent amount of time */
#if 0
         spent = utime() - start;
#else
         spent = xdifftime(xtime(), start);
#endif
         /* display information about the current basic solution */
         if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq == 0 &&
             spx->out_dly <= spent) prim_feas_dpy(spx, sum_0);
         /* we needn't to wait until all artificial variables leave the
            basis */
         if (orig_infsum(spx, spx->tol_bnd) == 0.0)
         {  /* the sum of infeasibilities is zero, therefore the current
               solution is primal feasible for the original problem */
            ret = 0;
            break;
         }
         /* check if the iterations limit has been exhausted */
         if (spx->it_lim == 0)
         {  ret = GLP_EITLIM;
            break;
         }
         /* check if the time limit has been exhausted */
         if (spx->tm_lim >= 0.0 && spx->tm_lim <= spent)
         {  ret = GLP_ETMLIM;
            break;
         }
         /* choose non-basic variable xN[q] */
         if (spx_prim_chuzc(spx, spx->tol_dj))
         {  /* basic solution components were recomputed; check primal
               feasibility (of the artificial solution) */
            if (spx_check_bbar(spx, spx->tol_bnd) != 0.0)
            {  /* the current solution became primal infeasible due to
                  round-off errors */
               ret = LPX_E_INSTAB;
               break;
            }
         }
         /* if no xN[q] has been chosen, the sum of infeasibilities is
            minimal but non-zero; therefore the original problem has no
            primal feasible solution */
         if (spx->q == 0)
         {  ret = LPX_E_NOFEAS;
            break;
         }
         /* compute the q-th column of the current simplex table (later
            this column will enter the basis) */
         spx_eval_col(spx, spx->q, spx->aq, 1);
         /* choose basic variable xB[p] */
         if (spx_prim_chuzr(spx, spx->relax * spx->tol_bnd))
         {  /* the basis matrix should be reinverted, because the q-th
               column of the simplex table is unreliable */
            xassert("not implemented yet" == NULL);
         }
         /* the sum of infeasibilities can't be negative, therefore the
            modified problem can't have unbounded solution */
         xassert(spx->p != 0);
         /* update values of basic variables */
         spx_update_bbar(spx, NULL);
         if (spx->p > 0)
         {  /* compute the p-th row of the inverse inv(B) */
            spx_eval_rho(spx, spx->p, spx->zeta);
            /* compute the p-th row of the current simplex table */
            spx_eval_row(spx, spx->zeta, spx->ap);
            /* update simplex multipliers */
            spx_update_pi(spx);
            /* update reduced costs of non-basic variables */
            spx_update_cbar(spx, 0);
            /* update weights of non-basic variables */
            if (spx->price) spx_update_gvec(spx);
         }
         /* xB[p] is leaving the basis; if it is implicit artificial
            variable, the corresponding residual vanishes; therefore
            bounds of this variable should be restored to the original
            ones */
         if (spx->p > 0)
         {  k = spx->indx[spx->p]; /* x[k] = xB[p] */
            if (spx->type[k] != spx->orig_type[k])
            {  /* x[k] is implicit artificial variable */
               spx->type[k] = spx->orig_type[k];
               spx->lb[k] = spx->orig_lb[k];
               spx->ub[k] = spx->orig_ub[k];
               xassert(spx->p_tag == GLP_NL || spx->p_tag == GLP_NU);
               spx->p_tag = (spx->p_tag == GLP_NL ? GLP_NU : GLP_NL);
               if (spx->type[k] == GLP_FX) spx->p_tag = GLP_NS;
               /* nullify the objective coefficient at x[k] */
               spx->coef[k] = 0.0;
               /* since coef[k] has been changed, we need to compute
                  new reduced cost of x[k], which it will have in the
                  adjacent basis */
               /* the formula d[j] = cN[j] - pi' * N[j] is used (note
                  that the vector pi is not changed, because it depends
                  on objective coefficients at basic variables, but in
                  the adjacent basis, for which the vector pi has been
                  just recomputed, x[k] is non-basic) */
               if (k <= m)
               {  /* x[k] is auxiliary variable */
                  spx->cbar[spx->q] = - spx->pi[k];
               }
               else
               {  /* x[k] is structural variable */
                  int ptr = spx->AT_ptr[k-m];
                  int end = spx->AT_ptr[k-m+1];
                  double d = 0.0;
                  for (ptr = ptr; ptr < end; ptr++)
                     d += spx->pi[spx->AT_ind[ptr]] * spx->AT_val[ptr];
                  spx->cbar[spx->q] = d;
               }
            }
         }
         /* jump to the adjacent vertex of the LP polyhedron */
         if (spx_change_basis(spx))
         {  /* the basis matrix should be reinverted */
            if (spx_invert(spx))
            {  /* numerical problems with the basis matrix */
               ret = GLP_EFAIL;
               break;
            }
            /* compute the current basic solution components */
            spx_eval_bbar(spx);
            spx_eval_pi(spx);
            spx_eval_cbar(spx);
            /* check primal feasibility */
            if (spx_check_bbar(spx, spx->tol_bnd) != 0.0)
            {  /* the current solution became primal infeasible due to
                  excessive round-off errors */
               ret = LPX_E_INSTAB;
               break;
            }
         }
#if 0
         /* check accuracy of main solution components after updating
            (for debugging purposes only) */
         {  double ae_bbar = spx_err_in_bbar(spx);
            double ae_pi   = spx_err_in_pi(spx);
            double ae_cbar = spx_err_in_cbar(spx, 0);
            double ae_gvec = spx->price ? spx_err_in_gvec(spx) : 0.0;
            print("bbar: %g; pi: %g; cbar: %g; gvec: %g",
               ae_bbar, ae_pi, ae_cbar, ae_gvec);
            if (ae_bbar > 1e-7 || ae_pi > 1e-7 || ae_cbar > 1e-7 ||
                ae_gvec > 1e-3) fault("solution accuracy too low");
         }
#endif
      }
      /* restore components of the original problem, which were changed
         by the routine */
      memcpy(spx->type, spx->orig_type, (1+m+n) * sizeof(int));
      memcpy(spx->lb, spx->orig_lb, (1+m+n) * sizeof(double));
      memcpy(spx->ub, spx->orig_ub, (1+m+n) * sizeof(double));
      spx->dir = spx->orig_dir;
      memcpy(spx->coef, spx->orig_coef, (1+m+n) * sizeof(double));
      /* if there are numerical problems with the basis matrix, the
         latter must be repaired; mark the basic solution as undefined
         and exit immediately */
      if (ret == GLP_EFAIL)
      {  spx->p_stat = spx->d_stat = GLP_UNDEF;
         goto done;
      }
      /* compute the final basic solution components */
      spx_eval_bbar(spx);
      spx_eval_pi(spx);
      spx_eval_cbar(spx);
      if (spx_check_bbar(spx, spx->tol_bnd) == 0.0)
         spx->p_stat = GLP_FEAS;
      else
         spx->p_stat = GLP_INFEAS;
      if (spx_check_cbar(spx, spx->tol_dj) == 0.0)
         spx->d_stat = GLP_FEAS;
      else
         spx->d_stat = GLP_INFEAS;
      /* display information about the final basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) prim_feas_dpy(spx, sum_0);
      /* correct the preliminary diagnosis */
      switch (ret)
      {  case 0:
            /* assumed p_stat = GLP_FEAS */
            if (spx->p_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            break;
         case GLP_EITLIM:
         case GLP_ETMLIM:
            /* assumed p_stat = GLP_INFEAS */
            if (spx->p_stat == GLP_FEAS)
               ret = 0;
            break;
         case LPX_E_NOFEAS:
            /* assumed p_stat = GLP_INFEAS */
            if (spx->p_stat == GLP_FEAS)
               ret = 0;
            else
               spx->p_stat = GLP_NOFEAS;
            break;
         case LPX_E_INSTAB:
            /* assumed p_stat = GLP_INFEAS */
            if (spx->p_stat == GLP_FEAS)
               ret = 0;
            break;
         default:
            xassert(ret != ret);
      }
done: /* deallocate the working segment */
      if (spx->meth != 0)
      {  spx->meth = 0;
         xfree(spx->zeta);
         xfree(spx->ap);
         xfree(spx->aq);
         xfree(spx->gvec);
         if (spx->price) xfree(spx->refsp);
         xfree(spx->work);
         xfree(spx->orig_type);
         xfree(spx->orig_lb);
         xfree(spx->orig_ub);
         xfree(spx->orig_coef);
      }
      /* determine the spent amount of time */
#if 0
      spent = utime() - start;
#else
      spent = xdifftime(xtime(), start);
#endif
      /* decrease the time limit by the spent amount */
      if (spx->tm_lim >= 0.0)
      {  spx->tm_lim -= spent;
         if (spx->tm_lim < 0.0) spx->tm_lim = 0.0;
      }
      /* return to the calling program */
      return ret;
}

/*----------------------------------------------------------------------
-- spx_dual_opt - find optimal solution (dual simplex).
--
-- *Synopsis*
--
-- #include "glpspx.h"
-- int spx_dual_opt(SPX *spx);
--
-- *Description*
--
-- The routine spx_dual_opt is intended to find optimal solution of an
-- LP problem using the dual simplex method.
--
-- On entry to the routine the initial basis should be "warmed up" and,
-- moreover, the initial basic solution should be dual feasible.
--
-- Structure of this routine can be an example for other variants based
-- on the dual simplex method.
--
-- *Returns*
--
-- 0  optimal solution found.
--
-- LPX_E_NOFEAS
--    the problem has no primal feasible solution.
--
-- GLP_EOBJLL
--    the objective function has reached its lower limit and continues
--    decreasing.
--
-- GLP_EOBJUL
--    the objective function has reached its upper limit and continues
--    increasing.
--
-- GLP_EITLIM
--    iterations limit exceeded.
--
-- GLP_ETMLIM
--    time limit exceeded.
--
-- LPX_E_INSTAB
--    numerical instability; the current basic solution became primal
--    infeasible due to excessive round-off errors.
--
-- GLP_EFAIL
--    singular basis; the current basis matrix became singular or
--    ill-conditioned due to improper simplex iteration. */

static void dual_opt_dpy(SPX *spx)
{     /* this auxiliary routine displays information about the current
         basic solution */
      int i, def = 0;
      for (i = 1; i <= spx->m; i++)
         if (spx->type[spx->indx[i]] == GLP_FX) def++;
      xprintf("|%6d:   objval = %17.9e   infeas = %17.9e (%d)\n",
         spx->it_cnt, spx_eval_obj(spx), spx_check_bbar(spx, 0.0), def);
      return;
}

int spx_dual_opt(SPX *spx)
{     /* find optimal solution (dual simplex) */
      int m = spx->m;
      int n = spx->n;
      int ret;
      xlong_t start = xtime();
      double spent = 0.0, obj;
      /* the initial basis should be "warmed up" */
      xassert(spx->valid);
      xassert(spx->p_stat != GLP_UNDEF && spx->d_stat != GLP_UNDEF);
      /* the initial basic solution should be dual feasible */
      xassert(spx->d_stat == GLP_FEAS);
      /* if the initial basic solution is primal feasible, nothing to
         search for */
      if (spx->p_stat == GLP_FEAS)
      {  ret = 0;
         goto done;
      }
      /* allocate the working segment */
      xassert(spx->meth == 0);
      spx->meth = 'D';
      spx->p = 0;
      spx->p_tag = 0;
      spx->q = 0;
      spx->zeta = xcalloc(1+m, sizeof(double));
      spx->ap = xcalloc(1+n, sizeof(double));
      spx->aq = xcalloc(1+m, sizeof(double));
      spx->gvec = NULL;
      spx->dvec = xcalloc(1+m, sizeof(double));
      spx->refsp = (spx->price ? xcalloc(1+m+n, sizeof(int)) : NULL);
      spx->count = 0;
      spx->work = xcalloc(1+m+n, sizeof(double));
      spx->orig_type = NULL;
      spx->orig_lb = spx->orig_ub = NULL;
      spx->orig_dir = 0;
      spx->orig_coef = NULL;
beg:  /* compute initial value of the objective function */
      obj = spx_eval_obj(spx);
      /* initialize weights of basic variables */
      if (!spx->price)
      {  /* textbook pricing will be used */
         int i;
         for (i = 1; i <= m; i++) spx->dvec[i] = 1.0;
      }
      else
      {  /* steepest edge pricing will be used */
         spx_reset_refsp(spx);
      }
      /* display information about the initial basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) dual_opt_dpy(spx);
      /* main loop starts here */
      for (;;)
      {  /* determine the spent amount of time */
#if 0
         spent = utime() - start;
#else
         spent = xdifftime(xtime(), start);
#endif
         /* display information about the current basic solution */
         if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq == 0 &&
             spx->out_dly <= spent) dual_opt_dpy(spx);
         /* if the objective function should be minimized, check if it
            has reached its upper bound */
         if (spx->dir == GLP_MIN && obj >= spx->obj_ul)
         {  ret = GLP_EOBJUL;
            break;
         }
         /* if the objective function should be maximized, check if it
            has reached its lower bound */
         if (spx->dir == GLP_MAX && obj <= spx->obj_ll)
         {  ret = GLP_EOBJLL;
            break;
         }
         /* check if the iterations limit has been exhausted */
         if (spx->it_lim == 0)
         {  ret = GLP_EITLIM;
            break;
         }
         /* check if the time limit has been exhausted */
         if (spx->tm_lim >= 0.0 && spx->tm_lim <= spent)
         {  ret = GLP_ETMLIM;
            break;
         }
         /* choose basic variable */
         spx_dual_chuzr(spx, spx->tol_bnd);
         /* if no xB[p] has been chosen, the current basic solution is
            primal feasible and therefore optimal */
         if (spx->p == 0)
         {  ret = 0;
            break;
         }
         /* compute the p-th row of the inverse inv(B) */
         spx_eval_rho(spx, spx->p, spx->zeta);
         /* compute the p-th row of the current simplex table */
         spx_eval_row(spx, spx->zeta, spx->ap);
         /* choose non-basic variable xN[q] */
         if (spx_dual_chuzc(spx, spx->relax * spx->tol_dj))
         {  /* the basis matrix should be reinverted, because the p-th
               row of the simplex table is unreliable */
            xassert("not implemented yet" == NULL);
         }
         /* if no xN[q] has been chosen, there is no primal feasible
            solution (the dual problem has unbounded solution) */
         if (spx->q == 0)
         {  ret = LPX_E_NOFEAS;
            break;
         }
         /* compute the q-th column of the current simplex table (later
            this column will enter the basis) */
         spx_eval_col(spx, spx->q, spx->aq, 1);
         /* update values of basic variables and value of the objective
            function */
         spx_update_bbar(spx, &obj);
         /* update simplex multipliers */
         spx_update_pi(spx);
         /* update reduced costs of non-basic variables */
         spx_update_cbar(spx, 0);
         /* update weights of basic variables */
         if (spx->price) spx_update_dvec(spx);
         /* if xB[p] is fixed variable, adjust its non-basic tag */
         if (spx->type[spx->indx[spx->p]] == GLP_FX)
            spx->p_tag = GLP_NS;
         /* jump to the adjacent vertex of the LP polyhedron */
         if (spx_change_basis(spx))
         {  /* the basis matrix should be reinverted */
            if (spx_invert(spx) != 0)
            {  /* numerical problems with the basis matrix */
               spx->p_stat = spx->d_stat = GLP_UNDEF;
               ret = GLP_EFAIL;
               goto done;
            }
            /* compute the current basic solution components */
            spx_eval_bbar(spx);
            obj = spx_eval_obj(spx);
            spx_eval_pi(spx);
            spx_eval_cbar(spx);
            /* check dual feasibility */
            if (spx_check_cbar(spx, spx->tol_dj) != 0.0)
            {  /* the current solution became dual infeasible due to
                  round-off errors */
               ret = LPX_E_INSTAB;
               break;
            }
         }
#if 0
         /* check accuracy of main solution components after updating
            (for debugging purposes only) */
         {  double ae_bbar = spx_err_in_bbar(spx);
            double ae_pi   = spx_err_in_pi(spx);
            double ae_cbar = spx_err_in_cbar(spx, 0);
            double ae_dvec = spx->price ? spx_err_in_dvec(spx) : 0.0;
            print("bbar: %g; pi: %g; cbar: %g; dvec: %g",
               ae_bbar, ae_pi, ae_cbar, ae_dvec);
            if (ae_bbar > 1e-9 || ae_pi > 1e-9 || ae_cbar > 1e-9 ||
                ae_dvec > 1e-3)
               xassert("solution accuracy too low" == NULL);
         }
#endif
      }
      /* compute the final basic solution components */
      spx_eval_bbar(spx);
      obj = spx_eval_obj(spx);
      spx_eval_pi(spx);
      spx_eval_cbar(spx);
      if (spx_check_bbar(spx, spx->tol_bnd) == 0.0)
         spx->p_stat = GLP_FEAS;
      else
         spx->p_stat = GLP_INFEAS;
      if (spx_check_cbar(spx, spx->tol_dj) == 0.0)
         spx->d_stat = GLP_FEAS;
      else
         spx->d_stat = GLP_INFEAS;
      /* display information about the final basic solution */
      if (spx->msg_lev >= 2 && spx->it_cnt % spx->out_frq != 0 &&
          spx->out_dly <= spent) dual_opt_dpy(spx);
      /* correct the preliminary diagnosis */
      switch (ret)
      {  case 0:
            /* assumed p_stat = d_stat = GLP_FEAS */
            if (spx->d_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->p_stat != GLP_FEAS)
            {  /* it seems we need to continue the search */
               goto beg;
            }
            break;
         case GLP_EOBJLL:
         case GLP_EOBJUL:
            /* assumed p_stat = GLP_INFEAS and d_stat = GLP_FEAS */
            if (spx->d_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->p_stat == GLP_FEAS)
               ret = 0;
            else if (spx->dir == GLP_MIN && obj < spx->obj_ul ||
                     spx->dir == GLP_MAX && obj > spx->obj_ll)
            {  /* it seems we need to continue the search */
               goto beg;
            }
            break;
         case GLP_EITLIM:
         case GLP_ETMLIM:
            /* assumed p_stat = GLP_INFEAS and d_stat = GLP_FEAS */
            if (spx->d_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->p_stat == GLP_FEAS)
               ret = 0;
            break;
         case LPX_E_NOFEAS:
            /* assumed p_stat = GLP_INFEAS and d_stat = GLP_FEAS */
            if (spx->d_stat != GLP_FEAS)
               ret = LPX_E_INSTAB;
            else if (spx->p_stat == GLP_FEAS)
               ret = 0;
            else
               spx->p_stat = GLP_NOFEAS;
            break;
         case LPX_E_INSTAB:
            /* assumed d_stat = GLP_INFEAS */
            if (spx->d_stat == GLP_FEAS)
            {  if (spx->p_stat == GLP_FEAS)
                  ret = 0;
               else
               {  /* it seems we need to continue the search */
                  goto beg;
               }
            }
            break;
         default:
            xassert(ret != ret);
      }
done: /* deallocate the working segment */
      if (spx->meth != 0)
      {  spx->meth = 0;
         xfree(spx->zeta);
         xfree(spx->ap);
         xfree(spx->aq);
         xfree(spx->dvec);
         if (spx->price) xfree(spx->refsp);
         xfree(spx->work);
      }
      /* determine the spent amount of time */
#if 0
      spent = utime() - start;
#else
      spent = xdifftime(xtime(), start);
#endif
      /* decrease the time limit by the spent amount */
      if (spx->tm_lim >= 0.0)
      {  spx->tm_lim -= spent;
         if (spx->tm_lim < 0.0) spx->tm_lim = 0.0;
      }
      /* return to the calling program */
      return ret;
}

/*----------------------------------------------------------------------
-- spx_simplex - base driver to the simplex method.
--
-- *Synopsis*
--
-- #include "glpspx.h"
-- int spx_simplex(SPX *spx);
--
-- *Description*
--
-- The routine spx_simplex is a base driver to the simplex method.
--
-- Currently this routine implements an easy variant of the two-phase
-- primal simplex method, where on the phase I the routine searches for
-- a primal feasible solution, and on the phase II for an optimal one.
-- (However, if the initial basic solution is primal infeasible, but
-- dual feasible, the dual simplex method may be used; see the control
-- parameter LPX_K_DUAL.)
--
-- *Returns*
--
-- 0  the LP problem has been successfully solved.
--
-- GLP_EOBJLL
--    the objective function has reached its lower limit and continues
--    decreasing.
--
-- GLP_EOBJUL
--    the objective function has reached its upper limit and continues
--    increasing.
--
-- GLP_EITLIM
--    iterations limit exceeded.
--
-- GLP_ETMLIM
--    time limit exceeded.
--
-- GLP_EFAIL
--    singular basis; the current basis matrix became singular or
--    ill-conditioned due to improper simplex iteration. */

#define prefix "spx_simplex: "

int spx_simplex(SPX *spx)
{     int ret;
      xassert(spx->m > 0 && spx->n > 0);
      /* check that each double-bounded variable has correct lower and
         upper bounds */
      {  int k;
         for (k = 1; k <= spx->m + spx->n; k++)
         {  if (spx->type[k] == GLP_DB)
               xassert(spx->lb[k] < spx->ub[k]);
         }
      }
      /* "warm up" the initial basis */
      spx_warm_up(spx);
      /* if the initial basic solution is optimal (i.e. primal and dual
         feasible), nothing to search for */
      if (spx->p_stat == GLP_FEAS && spx->d_stat == GLP_FEAS)
      {  if (spx->msg_lev >= 2 && spx->out_dly == 0.0)
            xprintf("!%6d:   objval = %17.9e   infeas = %17.9e\n",
               spx->it_cnt, spx_eval_obj(spx), 0.0);
         if (spx->msg_lev >= 3)
            xprintf("OPTIMAL SOLUTION FOUND\n");
         ret = 0;
         goto done;
      }
      /* if the initial basic solution is primal infeasible, but dual
         feasible, the dual simplex method may be used */
      if (spx->d_stat == GLP_FEAS && spx->dual) goto dual;
feas: /* phase I: find a primal feasible basic solution */
      ret = spx_prim_feas(spx);
      switch (ret)
      {  case 0:
            goto opt;
         case LPX_E_NOFEAS:
            if (spx->msg_lev >= 3)
               xprintf("PROBLEM HAS NO FEASIBLE SOLUTION\n");
            ret = 0;
            goto done;
         case GLP_EITLIM:
            if (spx->msg_lev >= 3)
               xprintf("ITERATIONS LIMIT EXCEEDED; SEARCH TERMINATED\n")
                  ;
            goto done;
         case GLP_ETMLIM:
            if (spx->msg_lev >= 3)
               xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
            goto done;
         case LPX_E_INSTAB:
            if (spx->msg_lev >= 2)
               xprintf(prefix "warning: numerical instability (primal s"
                  "implex, phase I)\n");
            goto feas;
         case GLP_EFAIL:
            if (spx->msg_lev >= 1)
            {  xprintf(prefix "numerical problems with basis matrix\n");
               xprintf(prefix "sorry, basis recovery procedure not impl"
                  "emented yet\n");
            }
            goto done;
         default:
            xassert(ret != ret);
      }
opt:  /* phase II: find an optimal basic solution (primal simplex) */
      ret = spx_prim_opt(spx);
      switch (ret)
      {  case 0:
            if (spx->msg_lev >= 3)
               xprintf("OPTIMAL SOLUTION FOUND\n");
            goto done;
         case LPX_E_NOFEAS:
            if (spx->msg_lev >= 3)
               xprintf("PROBLEM HAS UNBOUNDED SOLUTION\n");
            ret = 0;
            goto done;
         case GLP_EITLIM:
            if (spx->msg_lev >= 3)
               xprintf("ITERATIONS LIMIT EXCEEDED; SEARCH TERMINATED\n")
                  ;
            goto done;
         case GLP_ETMLIM:
            if (spx->msg_lev >= 3)
               xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
            goto done;
         case LPX_E_INSTAB:
            if (spx->msg_lev >= 2)
               xprintf(prefix "warning: numerical instability (primal s"
                  "implex, phase II)\n");
            goto feas;
         case GLP_EFAIL:
            if (spx->msg_lev >= 1)
            {  xprintf(prefix "numerical problems with basis matrix\n");
               xprintf(prefix "sorry, basis recovery procedure not impl"
                  "emented yet\n");
            }
            goto done;
         default:
            xassert(ret != ret);
      }
dual: /* phase II: find an optimal basic solution (dual simplex) */
      ret = spx_dual_opt(spx);
      switch (ret)
      {  case 0:
            if (spx->msg_lev >= 3)
               xprintf("OPTIMAL SOLUTION FOUND\n");
            goto done;
         case LPX_E_NOFEAS:
            if (spx->msg_lev >= 3)
               xprintf("PROBLEM HAS NO FEASIBLE SOLUTION\n");
            ret = 0;
            goto done;
         case GLP_EOBJLL:
            if (spx->msg_lev >= 3)
               xprintf("OBJECTIVE LOWER LIMIT REACHED; SEARCH TERMINATE"
                  "D\n");
            goto done;
         case GLP_EOBJUL:
            if (spx->msg_lev >= 3)
               xprintf("OBJECTIVE UPPER LIMIT REACHED; SEARCH TERMINATE"
                  "D\n");
            goto done;
         case GLP_EITLIM:
            if (spx->msg_lev >= 3)
               xprintf("ITERATIONS LIMIT EXCEEDED; SEARCH TERMINATED\n")
                  ;
            goto done;
         case GLP_ETMLIM:
            if (spx->msg_lev >= 3)
               xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
            goto done;
         case LPX_E_INSTAB:
            if (spx->msg_lev >= 2)
               xprintf(prefix "warning: numerical instability (dual sim"
                  "plex)\n");
            goto feas;
         case GLP_EFAIL:
            if (spx->msg_lev >= 1)
            {  xprintf(prefix "numerical problems with basis matrix\n");
               xprintf(prefix "sorry, basis recovery procedure not impl"
                  "emented yet\n");
            }
            goto done;
         default:
            xassert(ret != ret);
      }
done: /* return to the calling program */
      return ret;
}

/* eof */
