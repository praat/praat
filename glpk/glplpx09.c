/* glplpx09.c (advanced branch-and-bound solver) */

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
#include "glpapi.h"
#include "glpipp.h"
#include "glplib.h"

/*----------------------------------------------------------------------
-- show_status - display current status of the problem.
--
-- This routine displays some information about current status of the
-- problem.
--
-- This information includes:
--
-- the number of iterations performed by the simplex solver;
--
-- the current objective value;
--
-- the number of integral columns whose current values are fractional;
--
-- the number of cutting planes generated;
--
-- the number of non-zero coefficients in all cut inequalities. */

static void show_status(LPX *prob, int prob_m, int prob_nz)
{     int n, j, count;
      double x, tol_int;
      /* determine the number of structural variables of integer kind
         whose current values are still fractional */
      n = lpx_get_num_cols(prob);
      tol_int = lpx_get_real_parm(prob, LPX_K_TOLINT);
      count = 0;
      for (j = 1; j <= n; j++)
      {  if (lpx_get_col_kind(prob, j) != LPX_IV) continue;
         x = lpx_get_col_prim(prob, j);
         if (fabs(x - floor(x + 0.5)) <= tol_int) continue;
         count++;
      }
      xprintf("&%6d: obj = %17.9e   frac = %5d   cuts = %5d (%d)\n",
         lpx_get_int_parm(prob, LPX_K_ITCNT),
         lpx_get_obj_val(prob), count,
         lpx_get_num_rows(prob) - prob_m,
         lpx_get_num_nz(prob) - prob_nz);
      return;
}

/*----------------------------------------------------------------------
-- gen_cover_cut - try to generate mixed cover cut.
--
-- This routine generates mixed cover cuts by one for each appropriate
-- row for the specified problem and chooses the cut providing maximal
-- degradation or maximal residual. Once the cut has been chosen, the
-- routine adds it to the problem. */

static void gen_cover_cut(LPX *prob)
{     int m = lpx_get_num_rows(prob);
      int n = lpx_get_num_cols(prob);
      int i, k, type, kase, cut_i, len, *ind;
      double d, r, cut_d, cut_r, *val, *work;
      xassert(lpx_get_status(prob) == LPX_OPT);
      /* allocate working arrays */
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      work = xcalloc(1+n, sizeof(double));
      /* nothing is chosen so far */
      cut_i = 0; cut_d = 0.0; cut_r = 0.0;
      /* look through all rows */
      for (i = 1; i <= m; i++)
      for (kase = 1; kase <= 2; kase++)
      {  type = lpx_get_row_type(prob, i);
         if (kase == 1)
         {  /* consider rows of '<=' type */
            if (!(type == LPX_UP || type == LPX_DB)) continue;
            len = lpx_get_mat_row(prob, i, ind, val);
            val[0] = lpx_get_row_ub(prob, i);
         }
         else
         {  /* consider rows of '>=' type */
            if (!(type == LPX_LO || type == LPX_DB)) continue;
            len = lpx_get_mat_row(prob, i, ind, val);
            for (k = 1; k <= len; k++) val[k] = - val[k];
            val[0] = - lpx_get_row_lb(prob, i);
         }
         /* generate mixed cover cut:
            sum{j in J} a[j] * x[j] <= b */
         len = lpx_cover_cut(prob, len, ind, val, work);
         if (len == 0) continue;
         /* at the current point the cut inequality is violated, i.e.
            sum{j in J} a[j] * x[j] - b > 0 */
         r = lpx_eval_row(prob, len, ind, val) - val[0];
         if (r < 1e-5) continue;
         /* estimate degradation (worsening) of the objective function
            by one dual simplex step if the cut row would be introduced
            in the problem */
         d = lpx_eval_degrad(prob, len, ind, val, LPX_UP, val[0]);
         /* ignore the sign of degradation */
         d = fabs(d);
         /* if both degradation and scaled residual are small, skip the
            cut */
         if (d < 0.001 && r < 0.001)
            continue;
         /* if there is no cut chosen, choose this cut */
         else if (cut_i == 0)
            ;
         /* if this cut provides stronger degradation, choose it */
         else if (cut_d != 0.0 && cut_d < d)
            ;
         /* if this cut provides larger residual, choose it */
         else if (cut_d == 0.0 && cut_r < r)
            ;
         /* otherwise skip the cut */
         else
            continue;
         /* save attributes of the cut chosen */
         cut_i = (kase == 1 ? +i : -i), cut_r = r, cut_d = d;
      }
      /* if a cut has been chosen, include it in the problem */
      if (cut_i != 0)
      {  if (cut_i > 0)
         {  i = +cut_i;
            len = lpx_get_mat_row(prob, i, ind, val);
            val[0] = lpx_get_row_ub(prob, i);
         }
         else
         {  i = -cut_i;
            len = lpx_get_mat_row(prob, i, ind, val);
            for (k = 1; k <= len; k++) val[k] = - val[k];
            val[0] = - lpx_get_row_lb(prob, i);
         }
         /* generate the cut */
         len = lpx_cover_cut(prob, len, ind, val, work);
         /* include the corresponding row in the problem */
         i = lpx_add_rows(prob, 1);
         lpx_set_row_bnds(prob, i, LPX_UP, 0.0, val[0]);
         lpx_set_mat_row(prob, i, len, ind, val);
      }
      /* free working arrays */
      xfree(ind);
      xfree(val);
      xfree(work);
}

/*----------------------------------------------------------------------
-- gen_clique_cut - try to generate clique cut.
--
-- This routine generates a most violated clique cut. Once the cut has
-- been generated, the routine adds it to the problem. */

static void gen_clique_cut(LPX *lp, void *cog)
{     int n = lpx_get_num_cols(lp);
      int i, len, *ind;
      double *val;
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      len = lpx_clique_cut(lp, cog, ind, val);
      if (len > 0)
      {  i = lpx_add_rows(lp, 1);
         lpx_set_mat_row(lp, i, len, ind, val);
         lpx_set_row_bnds(lp, i, LPX_UP, 0.0, val[0]);
      }
      xfree(ind);
      xfree(val);
      return;
}

#if 0
/*----------------------------------------------------------------------
-- gen_gomory_cut - try to generate Gomory's mixed integer cut.
--
-- This routine generates Gomory's mixed integer cuts by one for each
-- integer variable having fractional value in the current solution and
-- chooses the cut providing either maximal degradation of the objective
-- or maximal scaled residual. Once the cut has been chosen, the routine
-- adds it to the problem. */

static void gen_gomory_cut(LPX *prob, int maxlen)
{     int m = lpx_get_num_rows(prob);
      int n = lpx_get_num_cols(prob);
      int i, j, k, len, cut_j, *ind;
      double x, d, r, temp, cut_d, cut_r, *val, *work;
      xassert(lpx_get_status(prob) == LPX_OPT);
      /* allocate working arrays */
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      work = xcalloc(1+m+n, sizeof(double));
      /* nothing is chosen so far */
      cut_j = 0; cut_d = 0.0; cut_r = 0.0;
      /* look through all structural variables */
      for (j = 1; j <= n; j++)
      {  /* if the variable is continuous, skip it */
         if (lpx_get_col_kind(prob, j) != LPX_IV) continue;
         /* if the variable is non-basic, skip it */
         if (lpx_get_col_stat(prob, j) != LPX_BS) continue;
         /* if the variable is fixed, skip it */
         if (lpx_get_col_type(prob, j) == LPX_FX) continue;
         /* obtain current primal value of the variable */
         x = lpx_get_col_prim(prob, j);
         /* if the value is close enough to nearest integer, skip the
            variable */
         if (fabs(x - floor(x + 0.5)) < 1e-4) continue;
         /* compute the row of the simplex table corresponding to the
            variable */
         len = lpx_eval_tab_row(prob, m+j, ind, val);
         len = lpx_remove_tiny(len, ind, NULL, val, 1e-10);
         /* generate Gomory's mixed integer cut:
            a[1]*x[1] + ... + a[n]*x[n] >= b */
         len = lpx_gomory_cut(prob, len, ind, val, work);
         if (len < 0) continue;
         xassert(0 <= len && len <= n);
         len = lpx_remove_tiny(len, ind, NULL, val, 1e-10);
         if (fabs(val[0]) < 1e-10) val[0] = 0.0;
         /* if the cut is too long, skip it */
         if (len > maxlen) continue;
         /* if the cut contains coefficients with too large magnitude,
            do not use it to prevent numeric instability */
         for (k = 0; k <= len; k++) /* including rhs */
            if (fabs(val[k]) > 1e+6) break;
         if (k <= len) continue;
         /* at the current point the cut inequality is violated, i.e.
            the residual b - (a[1]*x[1] + ... + a[n]*x[n]) > 0; note
            that for Gomory's cut the residual is less than 1.0 */
         /* in order not to depend on the magnitude of coefficients we
            use scaled residual:
            r = [b - (a[1]*x[1] + ... + a[n]*x[n])] / max(1, |a[j]|) */
         temp = 1.0;
         for (k = 1; k <= len; k++)
            if (temp < fabs(val[k])) temp = fabs(val[k]);
         r = (val[0] - lpx_eval_row(prob, len, ind, val)) / temp;
         if (r < 1e-5) continue;
         /* estimate degradation (worsening) of the objective function
            by one dual simplex step if the cut row would be introduced
            in the problem */
         d = lpx_eval_degrad(prob, len, ind, val, LPX_LO, val[0]);
         /* ignore the sign of degradation */
         d = fabs(d);
         /* which cut should be used? there are two basic cases:
            1) if the degradation is non-zero, we are interested in a
               cut providing maximal degradation;
            2) if the degradation is zero (i.e. a non-basic variable
               which would enter the basis in the adjacent vertex has
               zero reduced cost), we are interested in a cut providing
               maximal scaled residual;
            in both cases it is desired that the cut length (the number
            of inequality coefficients) is possibly short */
         /* if both degradation and scaled residual are small, skip the
            cut */
         if (d < 0.001 && r < 0.001)
            continue;
         /* if there is no cut chosen, choose this cut */
         else if (cut_j == 0)
            ;
         /* if this cut provides stronger degradation and has shorter
            length, choose it */
         else if (cut_d != 0.0 && cut_d < d)
            ;
         /* if this cut provides larger scaled residual and has shorter
            length, choose it */
         else if (cut_d == 0.0 && cut_r < r)
            ;
         /* otherwise skip the cut */
         else
            continue;
         /* save attributes of the cut choosen */
         cut_j = j, cut_r = r, cut_d = d;
      }
      /* if a cut has been chosen, include it to the problem */
      if (cut_j != 0)
      {  j = cut_j;
         /* compute the row of the simplex table */
         len = lpx_eval_tab_row(prob, m+j, ind, val);
         len = lpx_remove_tiny(len, ind, NULL, val, 1e-10);
         /* generate the cut */
         len = lpx_gomory_cut(prob, len, ind, val, work);
         xassert(0 <= len && len <= n);
         len = lpx_remove_tiny(len, ind, NULL, val, 1e-10);
         if (fabs(val[0]) < 1e-10) val[0] = 0.0;
         /* include the corresponding row in the problem */
         i = lpx_add_rows(prob, 1);
         lpx_set_row_bnds(prob, i, LPX_LO, val[0], 0.0);
         lpx_set_mat_row(prob, i, len, ind, val);
      }
      /* free working arrays */
      xfree(ind);
      xfree(val);
      xfree(work);
      return;
}
#endif

/*----------------------------------------------------------------------
-- generate_cuts - generate cutting planes.
--
-- This routine generates cutting planes and add them to the specified
-- problem object to improve LP relaxation. */

static int generate_cuts(LPX *prob, int use_cuts)
{     void *cog = NULL;
      int prob_m, prob_nz, msg_lev, dual, nrows, it_cnt, ret,
         cover = 0, clique = 0, gomory = 0;
      double out_dly, tm_lim;
      xlong_t tm_lag = xlset(0), tm_beg = xtime();
      /* generating clique cuts needs the conflict graph */
      if (use_cuts & LPX_C_CLIQUE) cog = lpx_create_cog(prob);
      xprintf("Generating cutting planes...\n");
      /* determine the number of rows, columns, and non-zeros on entry
         to the routine */
      prob_m = lpx_get_num_rows(prob);
#if 0
      prob_n = lpx_get_num_cols(prob);
#endif
      prob_nz = lpx_get_num_nz(prob);
      /* save some control parameters */
      msg_lev = lpx_get_int_parm(prob, LPX_K_MSGLEV);
      dual = lpx_get_int_parm(prob, LPX_K_DUAL);
      out_dly = lpx_get_real_parm(prob, LPX_K_OUTDLY);
      tm_lim = lpx_get_real_parm(prob, LPX_K_TMLIM);
      /* and set their new values needed for re-optimization */
      lpx_set_int_parm(prob, LPX_K_MSGLEV, 2);
      lpx_set_int_parm(prob, LPX_K_DUAL, 1);
      lpx_set_real_parm(prob, LPX_K_OUTDLY, 10.0);
      lpx_set_real_parm(prob, LPX_K_TMLIM, -1.0);
loop: /* main loop starts here */
      /* display current status of the problem */
#if 0
      if (utime() - tm_lag >= 5.0 - 0.001)
#else
      if (xdifftime(xtime(), tm_lag) >= 5.0 - 0.001)
#endif
         show_status(prob, prob_m, prob_nz), tm_lag = xtime();
      /* check if the patience has been exhausted */
#if 0
      if (tm_lim >= 0.0 && tm_lim <= utime() - tm_beg)
#else
      if (tm_lim >= 0.0 && tm_lim <= xdifftime(xtime(), tm_beg))
#endif
      {  ret = LPX_E_TMLIM;
         goto done;
      }
      /* not more than 300 cut inequalities are allowed */
      if (lpx_get_num_rows(prob) - prob_m >= 300)
      {  ret = LPX_E_OK;
         goto done;
      }
      /* not more than 30,000 cut coefficients are allowed */
      if (lpx_get_num_nz(prob) - prob_nz >= 30000)
      {  ret = LPX_E_OK;
         goto done;
      }
      /* try to generate some cut */
      nrows = lpx_get_num_rows(prob);
      if (nrows == lpx_get_num_rows(prob))
      {  /* try to generate mixed cover cut */
         if (use_cuts & LPX_C_COVER)
         {  gen_cover_cut(prob);
            cover += lpx_get_num_rows(prob) - nrows;
         }
      }
      if (nrows == lpx_get_num_rows(prob))
      {  /* try to generate clique cut */
         if (use_cuts & LPX_C_CLIQUE)
         {  if (cog != NULL) gen_clique_cut(prob, cog);
            clique += lpx_get_num_rows(prob) - nrows;
         }
      }
#if 0
      if (nrows == lpx_get_num_rows(prob))
      {  /* try to generate Gomory's mixed integer cut */
         if (use_cuts & LPX_C_GOMORY)
         {  gen_gomory_cut(prob, prob_n);
            gomory += lpx_get_num_rows(prob) - nrows;
         }
      }
#endif
      if (nrows == lpx_get_num_rows(prob))
      {  /* nothing has been generated */
         ret = LPX_E_OK;
         goto done;
      }
      /* re-optimize current LP relaxation using dual simplex */
      it_cnt = lpx_get_int_parm(prob, LPX_K_ITCNT);
      switch (lpx_simplex(prob))
      {  case LPX_E_OK:
            break;
         case LPX_E_ITLIM:
            ret = LPX_E_ITLIM;
            goto done;
         default:
            ret = LPX_E_SING;
            goto done;
      }
      if (it_cnt == lpx_get_int_parm(prob, LPX_K_ITCNT))
      {  ret = LPX_E_OK;
         goto done;
      }
      /* analyze status of the basic solution */
      switch (lpx_get_status(prob))
      {  case LPX_OPT:
            break;
         case LPX_NOFEAS:
            ret = LPX_E_NOPFS;
            goto done;
         default:
            xassert(prob != prob);
      }
      /* continue generating cutting planes */
      goto loop;
done: /* display final status of the problem */
      show_status(prob, prob_m, prob_nz);
      if (cover)
         xprintf("%d mixed cover cut(s) added\n", cover);
      if (clique)
         xprintf("%d clique cut(s) added\n", clique);
      if (gomory)
         xprintf("%d Gomory's mixed integer cut(s) added\n", gomory);
      /* the conflict graph is no longer needed, so delete it */
      if (cog != NULL) lpx_delete_cog(cog);
      switch (ret)
      {  case LPX_E_OK:
            break;
         case LPX_E_NOPFS:
            xprintf("PROBLEM HAS NO INTEGER FEASIBLE SOLUTION\n");
            break;
         case LPX_E_ITLIM:
            xprintf("ITERATIONS LIMIT EXCEEDED; SEARCH TERMINATED\n");
            break;
         case LPX_E_TMLIM:
            xprintf("TIME LIMIT EXCEEDED; SEARCH TERMINATED\n");
            break;
         case LPX_E_SING:
            xprintf("lpx_intopt: cannot re-optimize LP relaxation\n");
            break;
         default:
            xassert(ret != ret);
      }
      /* decrease the time limit by spent amount of the time */
      if (tm_lim >= 0.0)
#if 0
      {  tm_lim -= (utime() - tm_beg);
#else
      {  tm_lim -= xdifftime(xtime(), tm_beg);
#endif
         if (tm_lim < 0.0) tm_lim = 0.0;
      }
      /* restore some control parameters and update statistics */
      lpx_set_int_parm(prob, LPX_K_MSGLEV, msg_lev);
      lpx_set_int_parm(prob, LPX_K_DUAL, dual);
      lpx_set_real_parm(prob, LPX_K_OUTDLY, out_dly);
      lpx_set_real_parm(prob, LPX_K_TMLIM, tm_lim);
      return ret;
}

/*----------------------------------------------------------------------
-- lpx_intopt - advanced branch-and-bound solver.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- int lpx_intopt(LPX *mip);
--
-- DESCRIPTION
--
-- The routine lpx_intopt is intended for solving a MIP problem, which
-- is specified by the parameter mip.
--
-- Unlike the routine lpx_integer this routine needs no LP relaxation.
--
-- RETURNS
--
-- The routine lpx_intopt returns one of the following exit codes:
--
-- LPX_E_OK       the MIP problem has been successfully solved.
--
-- LPX_E_FAULT    the solver cannot start the search because either
--                the problem is not of MIP class, or
--                some integer variable has non-integer lower or upper
--                bound.
--
-- LPX_E_NOPFS    the MIP problem has no primal feasible solution.
--
-- LPX_E_NODFS    LP relaxation of the MIP problem has no dual feasible
--                solution.
--
-- LPX_E_ITLIM    iterations limit exceeded.
--
-- LPX_E_TMLIM    time limit exceeded.
--
-- LPX_E_SING     an error occurred on solving LP relaxation of some
--                subproblem during branch-and-bound.
--
-- Should note that additional exit codes may appear in future versions
-- of this routine. */

int lpx_intopt(LPX *_mip)
{     IPP *ipp = NULL;
      LPX *orig = _mip, *prob = NULL;
      int orig_m, orig_n, i, j, ret, i_stat, use_cuts;
#if 0
      /* the problem must be of MIP class */
      if (lpx_get_class(orig) != LPX_MIP)
      {  print("lpx_intopt: problem is not of MIP class");
         ret = LPX_E_FAULT;
         goto done;
      }
#endif
      /* the problem must have at least one row and one column */
      orig_m = lpx_get_num_rows(orig);
      orig_n = lpx_get_num_cols(orig);
      if (!(orig_m > 0 && orig_n > 0))
      {  xprintf("lpx_intopt: problem has no rows/columns\n");
         ret = LPX_E_FAULT;
         goto done;
      }
      /* check that each double-bounded row and column has bounds */
      for (i = 1; i <= orig_m; i++)
      {  if (lpx_get_row_type(orig, i) == LPX_DB)
         {  if (lpx_get_row_lb(orig, i) >= lpx_get_row_ub(orig, i))
            {  xprintf("lpx_intopt: row %d has incorrect bounds\n", i);
               ret = LPX_E_FAULT;
               goto done;
            }
         }
      }
      for (j = 1; j <= orig_n; j++)
      {  if (lpx_get_col_type(orig, j) == LPX_DB)
         {  if (lpx_get_col_lb(orig, j) >= lpx_get_col_ub(orig, j))
            {  xprintf("lpx_intopt: column %d has incorrect bounds\n",
                  j);
               ret = LPX_E_FAULT;
               goto done;
            }
         }
      }
      /* bounds of all integer variables must be integral */
      for (j = 1; j <= orig_n; j++)
      {  int type;
         double lb, ub;
         if (lpx_get_col_kind(orig, j) != LPX_IV) continue;
         type = lpx_get_col_type(orig, j);
         if (type == LPX_LO || type == LPX_DB || type == LPX_FX)
         {  lb = lpx_get_col_lb(orig, j);
            if (lb != floor(lb))
            {  xprintf("lpx_intopt: integer column %d has non-integer l"
                  "ower bound or fixed value %g\n", j, lb);
               ret = LPX_E_FAULT;
               goto done;
            }
         }
         if (type == LPX_UP || type == LPX_DB)
         {  ub = lpx_get_col_ub(orig, j);
            if (ub != floor(ub))
            {  xprintf("lpx_intopt: integer column %d has non-integer u"
                  "pper bound %g\n", j, ub);
               ret = LPX_E_FAULT;
               goto done;
            }
         }
      }
      /* reset the status of MIP solution */
      lpx_put_mip_soln(orig, LPX_I_UNDEF, NULL, NULL);
      /* create MIP presolver workspace */
      ipp = ipp_create_wksp();
      /* load the original problem into the presolver workspace */
      ipp_load_orig(ipp, orig);
      /* perform basic MIP presolve analysis */
      switch (ipp_basic_tech(ipp))
      {  case 0:
            /* no infeasibility is detected */
            break;
         case 1:
nopfs:      /* primal infeasibility is detected */
            xprintf("PROBLEM HAS NO PRIMAL FEASIBLE SOLUTION\n");
            ret = LPX_E_NOPFS;
            goto done;
         case 2:
            /* dual infeasibility is detected */
nodfs:      xprintf("LP RELAXATION HAS NO DUAL FEASIBLE SOLUTION\n");
            ret = LPX_E_NODFS;
            goto done;
         default:
            xassert(ipp != ipp);
      }
      /* reduce column bounds */
      switch (ipp_reduce_bnds(ipp))
      {  case 0:  break;
         case 1:  goto nopfs;
         default: xassert(ipp != ipp);
      }
      /* perform basic MIP presolve analysis */
      switch (ipp_basic_tech(ipp))
      {  case 0:  break;
         case 1:  goto nopfs;
         case 2:  goto nodfs;
         default: xassert(ipp != ipp);
      }
      /* replace general integer variables by sum of binary variables,
         if required */
      if (lpx_get_int_parm(orig, LPX_K_BINARIZE))
         ipp_binarize(ipp);
      /* perform coefficient reduction */
      ipp_reduction(ipp);
      /* if the resultant problem is empty, it has an empty solution,
         which is optimal */
      if (ipp->row_ptr == NULL || ipp->col_ptr == NULL)
      {  xassert(ipp->row_ptr == NULL);
         xassert(ipp->col_ptr == NULL);
         xprintf("Objective value = %.10g\n",
            ipp->orig_dir == LPX_MIN ? +ipp->c0 : -ipp->c0);
         xprintf("INTEGER OPTIMAL SOLUTION FOUND BY MIP PRESOLVER\n");
         /* allocate recovered solution segment */
         ipp->col_stat = xcalloc(1+ipp->ncols, sizeof(int));
         ipp->col_mipx = xcalloc(1+ipp->ncols, sizeof(double));
         for (j = 1; j <= ipp->ncols; j++) ipp->col_stat[j] = 0;
         /* perform MIP postsolve processing */
         ipp_postsolve(ipp);
         /* unload recovered MIP solution and store it in the original
            problem object */
         ipp_unload_sol(ipp, orig, LPX_I_OPT);
         ret = LPX_E_OK;
         goto done;
      }
      /* build resultant MIP problem object */
      prob = ipp_build_prob(ipp);
      /* display some statistics */
      {  int m = lpx_get_num_rows(prob);
         int n = lpx_get_num_cols(prob);
         int nnz = lpx_get_num_nz(prob);
         int ni = lpx_get_num_int(prob);
         int nb = lpx_get_num_bin(prob);
         char s[50];
         xprintf("lpx_intopt: presolved MIP has %d row%s, %d column%s, "
            "%d non-zero%s\n",
            m, m == 1 ? "" : "s", n, n == 1 ? "" : "s",
            nnz, nnz == 1 ? "" : "s");
         if (nb == 0)
            strcpy(s, "none of");
         else if (ni == 1 && nb == 1)
            strcpy(s, "");
         else if (nb == 1)
            strcpy(s, "one of");
         else if (nb == ni)
            strcpy(s, "all of");
         else
            sprintf(s, "%d of", nb);
         xprintf(
            "lpx_intopt: %d integer column%s, %s which %s binary\n",
            ni, ni == 1 ? "" : "s", s, nb == 1 ? "is" : "are");
      }
      /* inherit some control parameters and statistics */
      lpx_set_int_parm(prob, LPX_K_BFTYPE, lpx_get_int_parm(orig,
         LPX_K_BFTYPE));
      lpx_set_int_parm(prob, LPX_K_PRICE, lpx_get_int_parm(orig,
         LPX_K_PRICE));
      lpx_set_real_parm(prob, LPX_K_RELAX, lpx_get_real_parm(orig,
         LPX_K_RELAX));
      lpx_set_real_parm(prob, LPX_K_TOLBND, lpx_get_real_parm(orig,
         LPX_K_TOLBND));
      lpx_set_real_parm(prob, LPX_K_TOLDJ, lpx_get_real_parm(orig,
         LPX_K_TOLDJ));
      lpx_set_real_parm(prob, LPX_K_TOLPIV, lpx_get_real_parm(orig,
         LPX_K_TOLPIV));
      lpx_set_int_parm(prob, LPX_K_ITLIM, lpx_get_int_parm(orig,
         LPX_K_ITLIM));
      lpx_set_int_parm(prob, LPX_K_ITCNT, lpx_get_int_parm(orig,
         LPX_K_ITCNT));
      lpx_set_real_parm(prob, LPX_K_TMLIM, lpx_get_real_parm(orig,
         LPX_K_TMLIM));
      lpx_set_int_parm(prob, LPX_K_BRANCH, lpx_get_int_parm(orig,
         LPX_K_BRANCH));
      lpx_set_int_parm(prob, LPX_K_BTRACK, lpx_get_int_parm(orig,
         LPX_K_BTRACK));
      lpx_set_real_parm(prob, LPX_K_TOLINT, lpx_get_real_parm(orig,
         LPX_K_TOLINT));
      lpx_set_real_parm(prob, LPX_K_TOLOBJ, lpx_get_real_parm(orig,
         LPX_K_TOLOBJ));
      lpx_set_int_parm(prob, LPX_K_USECUTS, lpx_get_int_parm(orig,
         LPX_K_USECUTS));
      lpx_set_real_parm(prob, LPX_K_MIPGAP, lpx_get_real_parm(orig,
         LPX_K_MIPGAP));
      /* build an advanced initial basis */
      lpx_adv_basis(prob);
      /* solve LP relaxation */
      xprintf("Solving LP relaxation...\n");
      switch (lpx_simplex(prob))
      {  case LPX_E_OK:
            break;
         case LPX_E_ITLIM:
            ret = LPX_E_ITLIM;
            goto done;
         case LPX_E_TMLIM:
            ret = LPX_E_TMLIM;
            goto done;
         default:
            xprintf("lpx_intopt: cannot solve LP relaxation\n");
            ret = LPX_E_SING;
            goto done;
      }
      /* analyze status of the basic solution */
      switch (lpx_get_status(prob))
      {  case LPX_OPT:
            break;
         case LPX_NOFEAS:
            ret = LPX_E_NOPFS;
            goto done;
         case LPX_UNBND:
            ret = LPX_E_NODFS;
            goto done;
         default:
            xassert(prob != prob);
      }
      /* generate cutting planes, if necessary */
      use_cuts = lpx_get_int_parm(orig, LPX_K_USECUTS);
      if (use_cuts)
      {  ret =  generate_cuts(prob, use_cuts);
         if (ret != LPX_E_OK) goto done;
      }
      /* call the branch-and-bound solver */
      ret = lpx_integer(prob);
      /* determine status of MIP solution */
      i_stat = lpx_mip_status(prob);
      if (i_stat == LPX_I_OPT || i_stat == LPX_I_FEAS)
      {  /* load MIP solution of the resultant problem into presolver
            workspace */
         ipp_load_sol(ipp, prob);
         /* perform MIP postsolve processing */
         ipp_postsolve(ipp);
         /* unload recovered MIP solution and store it in the original
            problem object */
         ipp_unload_sol(ipp, orig, i_stat);
      }
      else
      {  /* just set the status of MIP solution */
         lpx_put_mip_soln(orig, i_stat, NULL, NULL);
      }
done: /* copy back statistics about spent resources */
      if (prob != NULL)
      {  lpx_set_int_parm(orig, LPX_K_ITLIM, lpx_get_int_parm(prob,
            LPX_K_ITLIM));
         lpx_set_int_parm(orig, LPX_K_ITCNT, lpx_get_int_parm(prob,
            LPX_K_ITCNT));
         lpx_set_real_parm(orig, LPX_K_TMLIM, lpx_get_real_parm(prob,
            LPX_K_TMLIM));
      }
      /* delete the resultant problem object */
      if (prob != NULL) lpx_delete_prob(prob);
      /* delete MIP presolver workspace */
      if (ipp != NULL) ipp_delete_wksp(ipp);
      return ret;
}

/* eof */
