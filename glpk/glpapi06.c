/* glpapi06.c (simplex method routines) */

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
#include "glpspx.h"
#define xfault xerror

/***********************************************************************
*  NAME
*
*  glp_simplex - solve LP problem with the simplex method
*
*  SYNOPSIS
*
*  int glp_simplex(glp_prob *lp, const glp_smcp *parm);
*
*  DESCRIPTION
*
*  The routine glp_simplex is a driver to the LP solver based on the
*  simplex method. This routine retrieves problem data from the
*  specified problem object, calls the solver to solve the problem
*  instance, and stores results of computations back into the problem
*  object.
*
*  The simplex solver has a set of control parameters. Values of the
*  control parameters can be passed in a structure glp_smcp, which the
*  parameter parm points to.
*
*  The parameter parm can be specified as NULL, in which case the LP
*  solver uses default settings.
*
*  RETURNS
*
*  0  The LP problem instance has been successfully solved. This code
*     does not necessarily mean that the solver has found optimal
*     solution. It only means that the solution process was successful.
*
*  GLP_EBADB
*     Unable to start the search, because the initial basis specified
*     in the problem object is invalid--the number of basic (auxiliary
*     and structural) variables is not the same as the number of rows in
*     the problem object.
*
*  GLP_ESING
*     Unable to start the search, because the basis matrix correspodning
*     to the initial basis is singular within the working precision.
*
*  GLP_ECOND
*     Unable to start the search, because the basis matrix correspodning
*     to the initial basis is ill-conditioned, i.e. its condition number
*     is too large.
*
*  GLP_EBOUND
*     Unable to start the search, because some double-bounded variables
*     have incorrect bounds.
*
*  GLP_EFAIL
*     The search was prematurely terminated due to the solver failure.
*
*  GLP_EOBJLL
*     The search was prematurely terminated, because the objective
*     function being maximized has reached its lower limit and continues
*     decreasing (dual simplex only).
*
*  GLP_EOBJUL
*     The search was prematurely terminated, because the objective
*     function being minimized has reached its upper limit and continues
*     increasing (dual simplex only).
*
*  GLP_EITLIM
*     The search was prematurely terminated, because the simplex
*     iteration limit has been exceeded.
*
*  GLP_ETMLIM
*     The search was prematurely terminated, because the time limit has
*     been exceeded.
*
*  GLP_ENOPFS
*     The LP problem instance has no primal feasible solution (only if
*     the LP presolver is used).
*
*  GLP_ENODFS
*     The LP problem instance has no dual feasible solution (only if the
*     LP presolver is used). */

static void trivial1(glp_prob *lp, const glp_smcp *parm)
{     /* solve trivial LP problem which has no rows */
      int j;
      double dir;
      xassert(lp->m == 0);
      switch (lp->dir)
      {  case GLP_MIN: dir = +1.0; break;
         case GLP_MAX: dir = -1.0; break;
         default:      xassert(lp != lp);
      }
      lp->pbs_stat = lp->dbs_stat = GLP_FEAS;
      lp->obj_val = lp->c0;
      lp->some = 0;
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         col->dual = col->coef;
         switch (col->type)
         {  case GLP_FR:
               if (dir * col->dual < -1e-7)
                  lp->dbs_stat = GLP_NOFEAS;
               if (dir * col->dual > +1e-7)
                  lp->dbs_stat = GLP_NOFEAS;
               col->stat = GLP_NF;
               col->prim = 0.0;
               break;
            case GLP_LO:
               if (dir * col->dual < -1e-7)
                  lp->dbs_stat = GLP_NOFEAS;
lo:            col->stat = GLP_NL;
               col->prim = col->lb;
               break;
            case GLP_UP:
               if (dir * col->dual > +1e-7)
                  lp->dbs_stat = GLP_NOFEAS;
up:            col->stat = GLP_NU;
               col->prim = col->ub;
               break;
            case GLP_DB:
               if (dir * col->dual < 0.0) goto up;
               if (dir * col->dual > 0.0) goto lo;
               if (fabs(col->lb) <= fabs(col->ub))
                  goto lo;
               else
                  goto up;
            case GLP_FX:
               col->stat = GLP_NS;
               col->prim = col->lb;
               break;
            default:
               xassert(lp != lp);
         }
         lp->obj_val += col->coef * col->prim;
         if (lp->dbs_stat == GLP_NOFEAS && lp->some == 0)
            lp->some = j;
      }
      if (parm->msg_lev >= GLP_MSG_ON && parm->out_dly == 0)
      {  xprintf("~%6d:   objval = %17.9e   infeas = %17.9e\n",
            lp->it_cnt, lp->obj_val, 0.0);
         if (parm->msg_lev >= GLP_MSG_ALL)
         {  if (lp->dbs_stat == GLP_FEAS)
               xprintf("OPTIMAL SOLUTION FOUND\n");
            else
               xprintf("PROBLEM HAS UNBOUNDED SOLUTION\n");
         }
      }
      return;
}

static void trivial2(glp_prob *lp, const glp_smcp *parm)
{     /* solve trivial LP problem which has no columns */
      int i;
      xassert(lp->n == 0);
      lp->pbs_stat = lp->dbs_stat = GLP_FEAS;
      lp->obj_val = lp->c0;
      lp->some = 0;
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         row->stat = GLP_BS;
         row->prim = row->dual = 0.0;
         switch (row->type)
         {  case GLP_FR:
               break;
            case GLP_LO:
               if (row->lb > +1e-8)
                  lp->pbs_stat = GLP_NOFEAS;
               break;
            case GLP_UP:
               if (row->ub < -1e-8)
                  lp->pbs_stat = GLP_NOFEAS;
               break;
            case GLP_DB:
            case GLP_FX:
               if (row->lb > +1e-8)
                  lp->pbs_stat = GLP_NOFEAS;
               if (row->ub < -1e-8)
                  lp->pbs_stat = GLP_NOFEAS;
               break;
            default:
               xassert(lp != lp);
         }
      }
      if (parm->msg_lev >= GLP_MSG_ON && parm->out_dly == 0)
      {  xprintf("~%6d:   objval = %17.9e   infeas = %17.9e\n",
            lp->it_cnt, lp->obj_val);
         if (parm->msg_lev >= GLP_MSG_ALL)
         {  if (lp->pbs_stat == GLP_FEAS)
               xprintf("OPTIMAL SOLUTION FOUND\n");
            else
               xprintf("PROBLEM HAS NO FEASIBLE SOLUTION\n");
         }
      }
      return;
}

static int simplex1(glp_prob *lp, const glp_smcp *parm)
{     /* base driver which does not use LP presolver */
      GLPROW **row = lp->row;
      GLPCOL **col = lp->col;
      GLPAIJ *aij;
      SPX _spx, *spx = &_spx;
      int i, j, k, m, n, t, nnz, loc, ret;
      double prim, dual;
      m = spx->m = lp->m;
      n = spx->n = lp->n;
      xassert(m > 0 && n > 0);
      if (!glp_bf_exists(lp))
      {  ret = glp_factorize(lp);
         switch (ret)
         {  case 0:
               break;
            case GLP_EBADB:
               if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_simplex: initial basis is invalid\n");
               goto done;
            case GLP_ESING:
               if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_simplex: initial basis is singular\n");
               goto done;
            case GLP_ECOND:
               if (parm->msg_lev >= GLP_MSG_ERR)
                  xprintf("glp_simplex: initial basis is ill-conditione"
                     "d\n");
               goto done;
            default:
               xassert(ret != ret);
         }
      }
      spx->type = xcalloc(1+m+n, sizeof(int));
      spx->lb = xcalloc(1+m+n, sizeof(double));
      spx->ub = xcalloc(1+m+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  spx->type[i] = row[i]->type;
         spx->lb[i] = row[i]->lb * row[i]->rii;
         spx->ub[i] = row[i]->ub * row[i]->rii;
      }
      for (j = 1; j <= n; j++)
      {  spx->type[m+j] = col[j]->type;
         spx->lb[m+j] = col[j]->lb / col[j]->sjj;
         spx->ub[m+j] = col[j]->ub / col[j]->sjj;
      }
      for (k = 1; k <= m+n; k++)
      {  switch (spx->type[k])
         {  case GLP_FR:
               spx->type[k] = GLP_FR;
               xassert(spx->lb[k] == 0.0);
               xassert(spx->ub[k] == 0.0);
               break;
            case GLP_LO:
               spx->type[k] = GLP_LO;
               xassert(spx->ub[k] == 0.0);
               break;
            case GLP_UP:
               spx->type[k] = GLP_UP;
               xassert(spx->lb[k] == 0.0);
               break;
            case GLP_DB:
               spx->type[k] = GLP_DB;
               break;
            case GLP_FX:
               spx->type[k] = GLP_FX;
               break;
            default:
               xassert(lp != lp);
         }
      }
      switch (lp->dir)
      {  case GLP_MIN:
            spx->dir = GLP_MIN;
            break;
         case GLP_MAX:
            spx->dir = GLP_MAX;
            break;
         default:
            xassert(lp != lp);
      }
      spx->coef = xcalloc(1+m+n, sizeof(double));
      spx->coef[0] = lp->c0;
      for (i = 1; i <= m; i++)
         spx->coef[i] = 0.0;
      for (j = 1; j <= n; j++)
         spx->coef[m+j] = col[j]->coef * col[j]->sjj;
      nnz = glp_get_num_nz(lp);
      spx->A_ptr = xcalloc(1+m+1, sizeof(int));
      spx->A_ind = xcalloc(1+nnz, sizeof(int));
      spx->A_val = xcalloc(1+nnz, sizeof(double));
      loc = 1;
      for (i = 1; i <= m; i++)
      {  spx->A_ptr[i] = loc;
         for (aij = row[i]->ptr; aij != NULL; aij = aij->r_next)
         {  spx->A_ind[loc] = aij->col->j;
            spx->A_val[loc] = aij->row->rii * aij->val * aij->col->sjj;
            loc++;
         }
      }
      spx->A_ptr[m+1] = loc;
      xassert(loc - 1 == nnz);
      spx->AT_ptr = xcalloc(1+n+1, sizeof(int));
      spx->AT_ind = xcalloc(1+nnz, sizeof(int));
      spx->AT_val = xcalloc(1+nnz, sizeof(double));
      loc = 1;
      for (j = 1; j <= n; j++)
      {  spx->AT_ptr[j] = loc;
         for (aij = col[j]->ptr; aij != NULL; aij = aij->c_next)
         {  spx->AT_ind[loc] = aij->row->i;
            spx->AT_val[loc] = aij->row->rii * aij->val * aij->col->sjj;
            loc++;
         }
      }
      spx->AT_ptr[n+1] = loc;
      xassert(loc - 1 == nnz);
      spx->col = xcalloc(1+m, sizeof(double));
      xassert(glp_bf_exists(lp));
      spx->valid = 1;
      spx->p_stat = spx->d_stat = GLP_UNDEF;
      spx->stat = xcalloc(1+m+n, sizeof(int));
      for (i = 1; i <= m; i++)
         spx->stat[i] = row[i]->stat;
      for (j = 1; j <= n; j++)
         spx->stat[m+j] = col[j]->stat;
      for (k = 1; k <= m+n; k++)
      {  switch (spx->stat[k])
         {  case GLP_BS:
               spx->stat[k] = GLP_BS;
               break;
            case GLP_NL:
               spx->stat[k] = GLP_NL;
               xassert(spx->type[k] == GLP_LO || spx->type[k] == GLP_DB)
                  ;
               break;
            case GLP_NU:
               spx->stat[k] = GLP_NU;
               xassert(spx->type[k] == GLP_UP || spx->type[k] == GLP_DB)
                  ;
               break;
            case GLP_NF:
               spx->stat[k] = GLP_NF;
               xassert(spx->type[k] == GLP_FR);
               break;
            case GLP_NS:
               spx->stat[k] = GLP_NS;
               xassert(spx->type[k] == GLP_FX);
               break;
            default:
               xassert(lp != lp);
         }
      }
      spx->posx = xcalloc(1+m+n, sizeof(int));
      spx->indx = xcalloc(1+m+n, sizeof(int));
      spx->bfd = _glp_access_bfd(lp);
      for (k = 1; k <= m+n; k++)
         spx->posx[k] = spx->indx[k] = 0;
      for (i = 1; i <= m; i++)
      {  k = glp_get_bhead(lp, i); /* xB[i] = x[k] */
         xassert(1 <= k && k <= m+n);
         xassert(spx->posx[k] == 0);
         xassert(spx->indx[i] == 0);
         spx->posx[k] = i, spx->indx[i] = k;
      }
      j = 0;
      for (k = 1; k <= m+n; k++)
      {  if (spx->posx[k] == 0)
         {  j++;
            spx->posx[k] = m+j, spx->indx[m+j] = k;
         }
      }
      xassert(j == n);
      spx->bbar = xcalloc(1+m, sizeof(double));
      spx->pi = xcalloc(1+m, sizeof(double));
      spx->cbar = xcalloc(1+n, sizeof(double));
      spx->some = 0;
      switch (parm->msg_lev)
      {  case GLP_MSG_OFF: spx->msg_lev = 0; break;
         case GLP_MSG_ERR: spx->msg_lev = 1; break;
         case GLP_MSG_ON:  spx->msg_lev = 2; break;
         case GLP_MSG_ALL: spx->msg_lev = 3; break;
         default:          xassert(parm != parm);
      }
      switch (parm->meth)
      {  case GLP_PRIMAL:  spx->dual = 0;    break;
         case GLP_DUALP:   spx->dual = 1;    break;
         default:          xassert(parm != parm);
      }
      switch (parm->pricing)
      {  case GLP_PT_STD:  spx->price = 0;   break;
         case GLP_PT_PSE:  spx->price = 1;   break;
         default:          xassert(parm != parm);
      }
      switch (parm->r_test)
      {  case GLP_RT_STD:  spx->relax = .00; break;
         case GLP_RT_HAR:  spx->relax = .07; break;
         default:          xassert(parm != parm);
      }
      spx->tol_bnd = parm->tol_bnd;
      spx->tol_dj  = parm->tol_dj;
      spx->tol_piv = parm->tol_piv;
      spx->obj_ll  = parm->obj_ll;
      spx->obj_ul  = parm->obj_ul;
      spx->it_lim  = parm->it_lim;
      spx->it_cnt  = lp->it_cnt;
      spx->tm_lim  = 0.001 * (double)parm->tm_lim;
      spx->out_frq = parm->out_frq;
      spx->out_dly = 0.001 * (double)parm->out_dly;
      spx->meth = 0;
      ret = spx_simplex(spx);
      lp->it_cnt = spx->it_cnt;
      switch (ret)
      {  case 0:           ret = 0;          break;
         case GLP_EOBJLL:  ret = GLP_EOBJLL; break;
         case GLP_EOBJUL:  ret = GLP_EOBJUL; break;
         case GLP_EITLIM:  ret = GLP_EITLIM; break;
         case GLP_ETMLIM:  ret = GLP_ETMLIM; break;
         case GLP_EFAIL:   ret = GLP_EFAIL;  break;
         default:          xassert(ret != ret);
      }
      if (ret == GLP_EFAIL) goto skip;
      xassert(spx->valid);
      switch (spx->p_stat)
      {  case GLP_UNDEF:   lp->pbs_stat = GLP_UNDEF;  break;
         case GLP_FEAS:    lp->pbs_stat = GLP_FEAS;   break;
         case GLP_INFEAS:  lp->pbs_stat = GLP_INFEAS; break;
         case GLP_NOFEAS:  lp->pbs_stat = GLP_NOFEAS; break;
         default: xassert(spx != spx);
      }
      switch (spx->d_stat)
      {  case GLP_UNDEF:   lp->dbs_stat = GLP_UNDEF;  break;
         case GLP_FEAS:    lp->dbs_stat = GLP_FEAS;   break;
         case GLP_INFEAS:  lp->dbs_stat = GLP_INFEAS; break;
         case GLP_NOFEAS:  lp->dbs_stat = GLP_NOFEAS; break;
         default: xassert(spx != spx);
      }
      lp->obj_val = spx_eval_obj(spx);
      if (lp->pbs_stat == GLP_FEAS && lp->dbs_stat == GLP_NOFEAS)
         lp->some = spx->some;
      else
         lp->some = 0;
      for (k = 1; k <= m+n; k++)
      {  /* status */
         switch (spx->stat[k])
         {  case GLP_BS: t = GLP_BS; break;
            case GLP_NL: t = GLP_NL; break;
            case GLP_NU: t = GLP_NU; break;
            case GLP_NF: t = GLP_NF; break;
            case GLP_NS: t = GLP_NS; break;
            default: xassert(spx != spx);
         }
         if (k <= m)
            row[k]->stat = t;
         else
            col[k-m]->stat = t;
         /* primal and dual values */
         t = spx->posx[k];
         if (t <= m)
         {  prim = spx->bbar[t];
            dual = 0.0;
         }
         else
         {  prim = spx_eval_xn_j(spx, t-m);
            dual = spx->cbar[t-m];
         }
         if (k <= m)
         {  row[k]->prim = prim / row[k]->rii;
            row[k]->dual = dual * row[k]->rii;
         }
         else
         {  col[k-m]->prim = prim * col[k-m]->sjj;
            col[k-m]->dual = dual / col[k-m]->sjj;
         }
      }
      xassert(spx->valid);
      lpx_put_lp_basis(lp, spx->valid, &spx->indx[0], spx->bfd);
skip: xfree(spx->type);
      xfree(spx->lb);
      xfree(spx->ub);
      xfree(spx->coef);
      xfree(spx->A_ptr);
      xfree(spx->A_ind);
      xfree(spx->A_val);
      xfree(spx->AT_ptr);
      xfree(spx->AT_ind);
      xfree(spx->AT_val);
      xfree(spx->col);
      xfree(spx->stat);
      xfree(spx->posx);
      xfree(spx->indx);
      xfree(spx->bbar);
      xfree(spx->pi);
      xfree(spx->cbar);
      xassert(spx->meth == 0);
done: return ret;
}

static int simplex2(glp_prob *orig, const glp_smcp *parm)
{     /* extended driver which uses LP presolver */
      LPP *lpp;
      glp_prob *prob;
      glp_bfcp bfcp;
      int orig_m, orig_n, orig_nnz, ret;
      orig_m = glp_get_num_rows(orig);
      orig_n = glp_get_num_cols(orig);
      orig_nnz = glp_get_num_nz(orig);
      if (parm->msg_lev >= GLP_MSG_ALL)
      {  xprintf("glp_simplex: original LP has %d row%s, %d column%s, %"
            "d non-zero%s\n",
            orig_m, orig_m == 1 ? "" : "s",
            orig_n, orig_n == 1 ? "" : "s",
            orig_nnz, orig_nnz == 1 ? "" : "s");
      }
      /* the problem must have at least one row and one column */
      xassert(orig_m > 0 && orig_n > 0);
      /* create LP presolver workspace */
      lpp = lpp_create_wksp();
      /* load the original problem into LP presolver workspace */
      lpp_load_orig(lpp, orig);
      /* perform LP presolve analysis */
      ret = lpp_presolve(lpp);
      switch (ret)
      {  case 0:
            /* presolving has been successfully completed */
            break;
         case 1:
            /* the original problem is primal infeasible */
            if (parm->msg_lev >= GLP_MSG_ALL)
               xprintf("PROBLEM HAS NO PRIMAL FEASIBLE SOLUTION\n");
            lpp_delete_wksp(lpp);
            return GLP_ENOPFS;
         case 2:
            /* the original problem is dual infeasible */
            if (parm->msg_lev >= GLP_MSG_ALL)
               xprintf("PROBLEM HAS NO DUAL FEASIBLE SOLUTION\n");
            lpp_delete_wksp(lpp);
            return GLP_ENODFS;
         default:
            xassert(ret != ret);
      }
      /* if the resultant problem is empty, it has an empty solution,
         which is optimal */
      if (lpp->row_ptr == NULL || lpp->col_ptr == NULL)
      {  xassert(lpp->row_ptr == NULL);
         xassert(lpp->col_ptr == NULL);
         if (parm->msg_lev >= GLP_MSG_ALL)
         {  xprintf("Objective value = %.10g\n",
               lpp->orig_dir == LPX_MIN ? + lpp->c0 : - lpp->c0);
            xprintf("OPTIMAL SOLUTION FOUND BY LP PRESOLVER\n");
         }
         /* allocate recovered solution segment */
         lpp_alloc_sol(lpp);
         goto post;
      }
      /* build resultant LP problem object */
      prob = lpp_build_prob(lpp);
      if (parm->msg_lev >= GLP_MSG_ALL)
      {  int m = glp_get_num_rows(prob);
         int n = glp_get_num_cols(prob);
         int nnz = glp_get_num_nz(prob);
         xprintf("glp_simplex: presolved LP has %d row%s, %d column%s, "
            "%d non-zero%s\n", m, m == 1 ? "" : "s",
            n, n == 1 ? "" : "s", nnz, nnz == 1 ? "" : "s");
      }
      /* inherit basis factorization control parameters */
      glp_get_bfcp(orig, &bfcp);
      glp_set_bfcp(prob, &bfcp);
      /* scale the resultant problem */
      lpx_scale_prob(prob);
      /* build advanced initial basis */
      {  LIBENV *env = lib_link_env();
         int term_out = env->term_out;
         if (!term_out || parm->msg_lev < GLP_MSG_ALL)
            env->term_out = GLP_OFF;
         else
            env->term_out = GLP_ON;
         lpx_adv_basis(prob);
         env->term_out = term_out;
      }
      /* try to solve the resultant problem */
      prob->it_cnt = orig->it_cnt;
      ret = simplex1(prob, parm);
      orig->it_cnt = prob->it_cnt;
      /* check if the optimal solution has been found */
      if (glp_get_status(prob) != GLP_OPT)
      {  if (parm->msg_lev >= GLP_MSG_ERR)
            xprintf("glp_simplex: cannot recover undefined or non-optim"
               "al solution\n");
         if (ret == 0)
         {  if (glp_get_prim_stat(prob) == GLP_NOFEAS)
               ret = GLP_ENOPFS;
            else if (glp_get_dual_stat(prob) == GLP_NOFEAS)
               ret = GLP_ENODFS;
         }
         glp_delete_prob(prob);
         lpp_delete_wksp(lpp);
         return ret;
      }
      /* allocate recovered solution segment */
      lpp_alloc_sol(lpp);
      /* load basic solution of the resultant problem into LP presolver
         workspace */
      lpp_load_sol(lpp, prob);
      /* the resultant problem object is no longer needed */
      glp_delete_prob(prob);
post: /* perform LP postsolve processing */
      lpp_postsolve(lpp);
      /* unload recovered basic solution and store it into the original
         problem object */
      lpp_unload_sol(lpp, orig);
      /* delete LP presolver workspace */
      lpp_delete_wksp(lpp);
      /* the original problem has been successfully solved */
      return 0;
}

int glp_simplex(glp_prob *lp, const glp_smcp *parm)
{     glp_smcp _parm;
      int i, j, ret;
      if (parm == NULL)
         parm = &_parm, glp_init_smcp((glp_smcp *)parm);
      /* check control parameters */
      if (!(parm->msg_lev == GLP_MSG_OFF ||
            parm->msg_lev == GLP_MSG_ERR ||
            parm->msg_lev == GLP_MSG_ON  ||
            parm->msg_lev == GLP_MSG_ALL))
         xfault("glp_simplex: msg_lev = %d; invalid parameter\n",
            parm->msg_lev);
      if (!(parm->meth == GLP_PRIMAL ||
            parm->meth == GLP_DUALP))
         xfault("glp_simplex: meth = %d; invalid parameter\n",
            parm->meth);
      if (!(parm->pricing == GLP_PT_STD ||
            parm->pricing == GLP_PT_PSE))
         xfault("glp_simplex: pricing = %d; invalid parameter\n",
            parm->pricing);
      if (!(parm->r_test == GLP_RT_STD ||
            parm->r_test == GLP_RT_HAR))
         xfault("glp_simplex: r_test = %d; invalid parameter\n",
            parm->r_test);
      if (!(0.0 < parm->tol_bnd && parm->tol_bnd < 1.0))
         xfault("glp_simplex: tol_bnd = %g; invalid parameter\n",
            parm->tol_bnd);
      if (!(0.0 < parm->tol_dj && parm->tol_dj < 1.0))
         xfault("glp_simplex: tol_dj = %g; invalid parameter\n",
            parm->tol_dj);
      if (!(0.0 < parm->tol_piv && parm->tol_piv < 1.0))
         xfault("glp_simplex: tol_piv = %g; invalid parameter\n",
            parm->tol_piv);
      if (parm->it_lim < 0)
         xfault("glp_simplex: it_lim = %d; invalid parameter\n",
            parm->it_lim);
      if (parm->tm_lim < 0)
         xfault("glp_simplex: tm_lim = %d; invalid parameter\n",
            parm->tm_lim);
      if (parm->out_frq < 1)
         xfault("glp_simplex: out_frq = %d; invalid parameter\n",
            parm->out_frq);
      if (parm->out_dly < 0)
         xfault("glp_simplex: out_dly = %d; invalid parameter\n",
            parm->out_dly);
      if (!(parm->presolve == GLP_ON || parm->presolve == GLP_OFF))
         xfault("glp_simplex: presolve = %d; invalid parameter\n",
            parm->presolve);
      /* basic solution is currently undefined */
      lp->pbs_stat = lp->dbs_stat = GLP_UNDEF;
      lp->obj_val = 0.0;
      lp->some = 0;
      /* check bounds of double-bounded variables */
      for (i = 1; i <= lp->m; i++)
      {  GLPROW *row = lp->row[i];
         if (row->type == GLP_DB && row->lb >= row->ub)
         {  if (parm->msg_lev >= GLP_MSG_ERR)
               xprintf("glp_simplex: row %d: lb = %g, ub = %g; incorrec"
                  "t bounds\n", i, row->lb, row->ub);
            ret = GLP_EBOUND;
            goto done;
         }
      }
      for (j = 1; j <= lp->n; j++)
      {  GLPCOL *col = lp->col[j];
         if (col->type == GLP_DB && col->lb >= col->ub)
         {  if (parm->msg_lev >= GLP_MSG_ERR)
               xprintf("glp_simplex: column %d: lb = %g, ub = %g; incor"
                  "rect bounds\n", j, col->lb, col->ub);
            ret = GLP_EBOUND;
            goto done;
         }
      }
      /* solve LP problem */
      if (lp->m == 0)
         trivial1(lp, parm), ret = 0;
      else if (lp->n == 0)
         trivial2(lp, parm), ret = 0;
      else if (!parm->presolve)
         ret = simplex1(lp, parm);
      else
         ret = simplex2(lp, parm);
done: /* return to the application program */
      return ret;
}

/***********************************************************************
*  NAME
*
*  glp_init_smcp - initialize simplex method control parameters
*
*  SYNOPSIS
*
*  void glp_init_smcp(glp_smcp *parm);
*
*  DESCRIPTION
*
*  The routine glp_init_smcp initializes control parameters, which are
*  used by the simplex solver, with default values.
*
*  Default values of the control parameters are stored in a glp_smcp
*  structure, which the parameter parm points to. */

void glp_init_smcp(glp_smcp *parm)
{     parm->msg_lev = GLP_MSG_ALL;
      parm->meth = GLP_PRIMAL;
      parm->pricing = GLP_PT_PSE;
      parm->r_test = GLP_RT_HAR;
      parm->tol_bnd = 1e-7;
      parm->tol_dj = 1e-7;
      parm->tol_piv = 1e-10;
      parm->obj_ll = -DBL_MAX;
      parm->obj_ul = +DBL_MAX;
      parm->it_lim = INT_MAX;
      parm->tm_lim = INT_MAX;
      parm->out_frq = 200;
      parm->out_dly = 0;
      parm->presolve = GLP_OFF;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_get_status - retrieve generic status of basic solution
*
*  SYNOPSIS
*
*  int glp_get_status(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_get_status reports the generic status of the basic
*  solution for the specified problem object as follows:
*
*  GLP_OPT    - solution is optimal;
*  GLP_FEAS   - solution is feasible;
*  GLP_INFEAS - solution is infeasible;
*  GLP_NOFEAS - problem has no feasible solution;
*  GLP_UNBND  - problem has unbounded solution;
*  GLP_UNDEF  - solution is undefined. */

int glp_get_status(glp_prob *lp)
{     int status;
      status = glp_get_prim_stat(lp);
      switch (status)
      {  case GLP_FEAS:
            switch (glp_get_dual_stat(lp))
            {  case GLP_FEAS:
                  status = GLP_OPT;
                  break;
               case GLP_NOFEAS:
                  status = GLP_UNBND;
                  break;
               case GLP_UNDEF:
               case GLP_INFEAS:
                  status = status;
                  break;
               default:
                  xassert(lp != lp);
            }
            break;
         case GLP_UNDEF:
         case GLP_INFEAS:
         case GLP_NOFEAS:
            status = status;
            break;
         default:
            xassert(lp != lp);
      }
      return status;
}

/***********************************************************************
*  NAME
*
*  glp_get_prim_stat - retrieve status of primal basic solution
*
*  SYNOPSIS
*
*  int glp_get_prim_stat(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_get_prim_stat reports the status of the primal basic
*  solution for the specified problem object as follows:
*
*  GLP_UNDEF  - primal solution is undefined;
*  GLP_FEAS   - primal solution is feasible;
*  GLP_INFEAS - primal solution is infeasible;
*  GLP_NOFEAS - no primal feasible solution exists. */

int glp_get_prim_stat(glp_prob *lp)
{     int pbs_stat = lp->pbs_stat;
      return pbs_stat;
}

/***********************************************************************
*  NAME
*
*  glp_get_dual_stat - retrieve status of dual basic solution
*
*  SYNOPSIS
*
*  int glp_get_dual_stat(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_get_dual_stat reports the status of the dual basic
*  solution for the specified problem object as follows:
*
*  GLP_UNDEF  - dual solution is undefined;
*  GLP_FEAS   - dual solution is feasible;
*  GLP_INFEAS - dual solution is infeasible;
*  GLP_NOFEAS - no dual feasible solution exists. */

int glp_get_dual_stat(glp_prob *lp)
{     int dbs_stat = lp->dbs_stat;
      return dbs_stat;
}

/***********************************************************************
*  NAME
*
*  glp_get_obj_val - retrieve objective value (basic solution)
*
*  SYNOPSIS
*
*  double glp_get_obj_val(glp_prob *lp);
*
*  RETURNS
*
*  The routine glp_get_obj_val returns value of the objective function
*  for basic solution. */

double glp_get_obj_val(glp_prob *lp)
{     struct LPXCPS *cps = lp->cps;
      double z;
      z = lp->obj_val;
      if (cps->round && fabs(z) < 1e-9) z = 0.0;
      return z;
}

/***********************************************************************
*  NAME
*
*  glp_get_row_prim - retrieve row primal value (basic solution)
*
*  SYNOPSIS
*
*  double glp_get_row_prim(glp_prob *lp, int i);
*
*  RETURNS
*
*  The routine glp_get_row_prim returns primal value of the auxiliary
*  variable associated with i-th row. */

double glp_get_row_prim(glp_prob *lp, int i)
{     struct LPXCPS *cps = lp->cps;
      double prim;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_get_row_prim: i = %d; row number out of range\n",
            i);
      prim = lp->row[i]->prim;
      if (cps->round && fabs(prim) < 1e-9) prim = 0.0;
      return prim;
}

/***********************************************************************
*  NAME
*
*  glp_get_row_dual - retrieve row dual value (basic solution)
*
*  SYNOPSIS
*
*  double glp_get_row_dual(glp_prob *lp, int i);
*
*  RETURNS
*
*  The routine glp_get_row_dual returns dual value (i.e. reduced cost)
*  of the auxiliary variable associated with i-th row. */

double glp_get_row_dual(glp_prob *lp, int i)
{     struct LPXCPS *cps = lp->cps;
      double dual;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_get_row_dual: i = %d; row number out of range\n",
            i);
      dual = lp->row[i]->dual;
      if (cps->round && fabs(dual) < 1e-9) dual = 0.0;
      return dual;
}

/***********************************************************************
*  NAME
*
*  glp_get_col_prim - retrieve column primal value (basic solution)
*
*  SYNOPSIS
*
*  double glp_get_col_prim(glp_prob *lp, int j);
*
*  RETURNS
*
*  The routine glp_get_col_prim returns primal value of the structural
*  variable associated with j-th column. */

double glp_get_col_prim(glp_prob *lp, int j)
{     struct LPXCPS *cps = lp->cps;
      double prim;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_get_col_prim: j = %d; column number out of range\n"
            , j);
      prim = lp->col[j]->prim;
      if (cps->round && fabs(prim) < 1e-9) prim = 0.0;
      return prim;
}

/***********************************************************************
*  NAME
*
*  glp_get_col_dual - retrieve column dual value (basic solution)
*
*  SYNOPSIS
*
*  double glp_get_col_dual(glp_prob *lp, int j);
*
*  RETURNS
*
*  The routine glp_get_col_dual returns dual value (i.e. reduced cost)
*  of the structural variable associated with j-th column. */

double glp_get_col_dual(glp_prob *lp, int j)
{     struct LPXCPS *cps = lp->cps;
      double dual;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_get_col_dual: j = %d; column number out of range\n"
            , j);
      dual = lp->col[j]->dual;
      if (cps->round && fabs(dual) < 1e-9) dual = 0.0;
      return dual;
}

/* eof */
