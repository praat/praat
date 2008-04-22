/* glplpx06.c */

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
#define lpx_get_rii glp_get_rii
#define lpx_get_sjj glp_get_sjj

/*----------------------------------------------------------------------
-- lpx_check_kkt - check Karush-Kuhn-Tucker conditions.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- void lpx_check_kkt(LPX *lp, int scaled, LPXKKT *kkt);
--
-- *Description*
--
-- The routine lpx_check_kkt checks Karush-Kuhn-Tucker conditions for
-- the current basic solution specified by an LP problem object, which
-- the parameter lp points to. Both primal and dual components of the
-- basic solution should be defined.
--
-- If the parameter scaled is zero, the conditions are checked for the
-- original, unscaled LP problem. Otherwise, if the parameter scaled is
-- non-zero, the routine checks the conditions for an internally scaled
-- LP problem.
--
-- The parameter kkt is a pointer to the structure LPXKKT, to which the
-- routine stores the results of checking (for details see below).
--
-- The routine performs all computations using only components of the
-- given LP problem and the current basic solution.
--
-- *Background*
--
-- The first condition checked by the routine is:
--
--    xR - A * xS = 0,                                          (KKT.PE)
--
-- where xR is the subvector of auxiliary variables (rows), xS is the
-- subvector of structural variables (columns), A is the constraint
-- matrix. This condition expresses the requirement that all primal
-- variables must satisfy to the system of equality constraints of the
-- original LP problem. In case of exact arithmetic this condition is
-- satisfied for any basic solution; however, if the arithmetic is
-- inexact, it shows how accurate the primal basic solution is, that
-- depends on accuracy of a representation of the basis matrix.
--
-- The second condition checked by the routines is:
--
--    l[k] <= x[k] <= u[k]  for all k = 1, ..., m+n,            (KKT.PB)
--
-- where x[k] is auxiliary or structural variable, l[k] and u[k] are,
-- respectively, lower and upper bounds of the variable x[k] (including
-- cases of infinite bounds). This condition expresses the requirement
-- that all primal variables must satisfy to bound constraints of the
-- original LP problem. Since in case of basic solution all non-basic
-- variables are placed on their bounds, actually the condition (KKT.PB)
-- is checked for basic variables only. If the primal basic solution
-- has sufficient accuracy, this condition shows primal feasibility of
-- the solution.
--
-- The third condition checked by the routine is:
--
--    grad Z = c = (A~)' * pi + d,
--
-- where Z is the objective function, c is the vector of objective
-- coefficients, (A~)' is a matrix transposed to the expanded constraint
-- matrix A~ = (I | -A), pi is a vector of Lagrange multiplers that
-- correspond to equality constraints of the original LP problem, d is
-- a vector of Lagrange multipliers that correspond to bound constraints
-- for all (i.e. auxiliary and structural) variables of the original LP
-- problem. Geometrically the third condition expresses the requirement
-- that the gradient of the objective function must belong to the
-- orthogonal complement of a linear subspace defined by the equality
-- and active bound constraints, i.e. that the gradient must be a linear
-- combination of normals to the constraint planes, where Lagrange
-- multiplers pi and d are coefficients of that linear combination. To
-- eliminate the vector pi the third condition can be rewritten as:
--
--    (  I  )      ( dR )   ( cR )
--    (     ) pi + (    ) = (    ),
--    ( -A' )      ( dS )   ( cS )
--
-- or, equivalently:
--
--          pi + dR = cR,
--
--    -A' * pi + dS = cS.
--
-- Substituting the vector pi from the first equation into the second
-- one we have:
--
--    A' * (dR - cR) + (dS - cS) = 0,                           (KKT.DE)
--
-- where dR is the subvector of reduced costs of auxiliary variables
-- (rows), dS is the subvector of reduced costs of structural variables
-- (columns), cR and cS are, respectively, subvectors of objective
-- coefficients at auxiliary and structural variables, A' is a matrix
-- transposed to the constraint matrix of the original LP problem. In
-- case of exact arithmetic this condition is satisfied for any basic
-- solution; however, if the arithmetic is inexact, it shows how
-- accurate the dual basic solution is, that depends on accuracy of a
-- representation of the basis matrix.
--
-- The last, fourth condition checked by the routine is:
--
--           d[k] = 0,    if x[k] is basic or free non-basic
--
--      0 <= d[k] < +inf, if x[k] is non-basic on its lower
--                        (minimization) or upper (maximization)
--                        bound
--                                                              (KKT.DB)
--    -inf < d[k] <= 0,   if x[k] is non-basic on its upper
--                        (minimization) or lower (maximization)
--                        bound
--
--    -inf < d[k] < +inf, if x[k] is non-basic fixed
--
-- for all k = 1, ..., m+n, where d[k] is a reduced cost (i.e. Lagrange
-- multiplier) of auxiliary or structural variable x[k]. Geometrically
-- this condition expresses the requirement that constraints of the
-- original problem must "hold" the point preventing its movement along
-- the antigradient (in case of minimization) or the gradient (in case
-- of maximization) of the objective function. Since in case of basic
-- solution reduced costs of all basic variables are placed on their
-- (zero) bounds, actually the condition (KKT.DB) is checked for
-- non-basic variables only. If the dual basic solution has sufficient
-- accuracy, this condition shows dual feasibility of the solution.
--
-- Should note that the complete set of Karush-Kuhn-Tucker conditions
-- also includes the fifth, so called complementary slackness condition,
-- which expresses the requirement that at least either a primal
-- variable x[k] or its dual conterpart d[k] must be on its bound for
-- all k = 1, ..., m+n. However, being always satisfied for any basic
-- solution by definition that condition is not checked by the routine.
--
-- To check the first condition (KKT.PE) the routine computes a vector
-- of residuals
--
--    g = xR - A * xS,
--
-- determines components of this vector that correspond to largest
-- absolute and relative errors:
--
--    pe_ae_max = max |g[i]|,
--
--    pe_re_max = max |g[i]| / (1 + |xR[i]|),
--
-- and stores these quantities and corresponding row indices to the
-- structure LPXKKT.
--
-- To check the second condition (KKT.PB) the routine computes a vector
-- of residuals
--
--           ( 0,            if lb[k] <= x[k] <= ub[k]
--           |
--    h[k] = < x[k] - lb[k], if x[k] < lb[k]
--           |
--           ( x[k] - ub[k], if x[k] > ub[k]
--
-- for all k = 1, ..., m+n, determines components of this vector that
-- correspond to largest absolute and relative errors:
--
--    pb_ae_max = max |h[k]|,
--
--    pb_re_max = max |h[k]| / (1 + |x[k]|),
--
-- and stores these quantities and corresponding variable indices to
-- the structure LPXKKT.
--
-- To check the third condition (KKT.DE) the routine computes a vector
-- of residuals
--
--    u = A' * (dR - cR) + (dS - cS),
--
-- determines components of this vector that correspond to largest
-- absolute and relative errors:
--
--    de_ae_max = max |u[j]|,
--
--    de_re_max = max |u[j]| / (1 + |dS[j] - cS[j]|),
--
-- and stores these quantities and corresponding column indices to the
-- structure LPXKKT.
--
-- To check the fourth condition (KKT.DB) the routine computes a vector
-- of residuals
--
--           ( 0,    if d[k] has correct sign
--    v[k] = <
--           ( d[k], if d[k] has wrong sign
--
-- for all k = 1, ..., m+n, determines components of this vector that
-- correspond to largest absolute and relative errors:
--
--    db_ae_max = max |v[k]|,
--
--    db_re_max = max |v[k]| / (1 + |d[k] - c[k]|),
--
-- and stores these quantities and corresponding variable indices to
-- the structure LPXKKT. */

void lpx_check_kkt(LPX *lp, int scaled, LPXKKT *kkt)
{     int m = lpx_get_num_rows(lp);
      int n = lpx_get_num_cols(lp);
#if 0 /* 21/XII-2003 */
      int *typx = lp->typx;
      double *lb = lp->lb;
      double *ub = lp->ub;
      double *rs = lp->rs;
#else
      int typx, tagx;
      double lb, ub;
#endif
      int dir = lpx_get_obj_dir(lp);
#if 0 /* 21/XII-2003 */
      double *coef = lp->coef;
#endif
#if 0 /* 22/XII-2003 */
      int *A_ptr = lp->A->ptr;
      int *A_len = lp->A->len;
      int *A_ndx = lp->A->ndx;
      double *A_val = lp->A->val;
#endif
      int *A_ndx;
      double *A_val;
#if 0 /* 21/XII-2003 */
      int *tagx = lp->tagx;
      int *posx = lp->posx;
      int *indx = lp->indx;
      double *bbar = lp->bbar;
      double *cbar = lp->cbar;
#endif
      int beg, end, i, j, k, t;
      double cR_i, cS_j, c_k, xR_i, xS_j, x_k, dR_i, dS_j, d_k;
      double g_i, h_k, u_j, v_k, temp, rii, sjj;
      if (lpx_get_prim_stat(lp) == LPX_P_UNDEF)
         xfault("lpx_check_kkt: primal basic solution is undefined\n");
      if (lpx_get_dual_stat(lp) == LPX_D_UNDEF)
         xfault("lpx_check_kkt: dual basic solution is undefined\n");
      /*--------------------------------------------------------------*/
      /* compute largest absolute and relative errors and corresponding
         row indices for the condition (KKT.PE) */
      kkt->pe_ae_max = 0.0, kkt->pe_ae_row = 0;
      kkt->pe_re_max = 0.0, kkt->pe_re_row = 0;
      A_ndx = xcalloc(1+n, sizeof(int));
      A_val = xcalloc(1+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  /* determine xR[i] */
#if 0 /* 21/XII-2003 */
         if (tagx[i] == LPX_BS)
            xR_i = bbar[posx[i]];
         else
            xR_i = spx_eval_xn_j(lp, posx[i] - m);
#else
         lpx_get_row_info(lp, i, NULL, &xR_i, NULL);
         xR_i *= lpx_get_rii(lp, i);
#endif
         /* g[i] := xR[i] */
         g_i = xR_i;
         /* g[i] := g[i] - (i-th row of A) * xS */
         beg = 1;
         end = lpx_get_mat_row(lp, i, A_ndx, A_val);
         for (t = beg; t <= end; t++)
         {  j = m + A_ndx[t]; /* a[i,j] != 0 */
            /* determine xS[j] */
#if 0 /* 21/XII-2003 */
            if (tagx[j] == LPX_BS)
               xS_j = bbar[posx[j]];
            else
               xS_j = spx_eval_xn_j(lp, posx[j] - m);
#else
            lpx_get_col_info(lp, j-m, NULL, &xS_j, NULL);
            xS_j /= lpx_get_sjj(lp, j-m);
#endif
            /* g[i] := g[i] - a[i,j] * xS[j] */
            rii = lpx_get_rii(lp, i);
            sjj = lpx_get_sjj(lp, j-m);
            g_i -= (rii * A_val[t] * sjj) * xS_j;
         }
         /* unscale xR[i] and g[i] (if required) */
         if (!scaled)
         {  rii = lpx_get_rii(lp, i);
            xR_i /= rii, g_i /= rii;
         }
         /* determine absolute error */
         temp = fabs(g_i);
         if (kkt->pe_ae_max < temp)
            kkt->pe_ae_max = temp, kkt->pe_ae_row = i;
         /* determine relative error */
         temp /= (1.0 + fabs(xR_i));
         if (kkt->pe_re_max < temp)
            kkt->pe_re_max = temp, kkt->pe_re_row = i;
      }
      xfree(A_ndx);
      xfree(A_val);
      /* estimate the solution quality */
      if (kkt->pe_re_max <= 1e-9)
         kkt->pe_quality = 'H';
      else if (kkt->pe_re_max <= 1e-6)
         kkt->pe_quality = 'M';
      else if (kkt->pe_re_max <= 1e-3)
         kkt->pe_quality = 'L';
      else
         kkt->pe_quality = '?';
      /*--------------------------------------------------------------*/
      /* compute largest absolute and relative errors and corresponding
         variable indices for the condition (KKT.PB) */
      kkt->pb_ae_max = 0.0, kkt->pb_ae_ind = 0;
      kkt->pb_re_max = 0.0, kkt->pb_re_ind = 0;
      for (k = 1; k <= m+n; k++)
      {  /* determine x[k] */
         if (k <= m)
         {  lpx_get_row_bnds(lp, k, &typx, &lb, &ub);
            rii = lpx_get_rii(lp, k);
            lb *= rii;
            ub *= rii;
            lpx_get_row_info(lp, k, &tagx, &x_k, NULL);
            x_k *= rii;
         }
         else
         {  lpx_get_col_bnds(lp, k-m, &typx, &lb, &ub);
            sjj = lpx_get_sjj(lp, k-m);
            lb /= sjj;
            ub /= sjj;
            lpx_get_col_info(lp, k-m, &tagx, &x_k, NULL);
            x_k /= sjj;
         }
         /* skip non-basic variable */
         if (tagx != LPX_BS) continue;
         /* compute h[k] */
         h_k = 0.0;
         switch (typx)
         {  case LPX_FR:
               break;
            case LPX_LO:
               if (x_k < lb) h_k = x_k - lb;
               break;
            case LPX_UP:
               if (x_k > ub) h_k = x_k - ub;
               break;
            case LPX_DB:
            case LPX_FX:
               if (x_k < lb) h_k = x_k - lb;
               if (x_k > ub) h_k = x_k - ub;
               break;
            default:
               xassert(typx != typx);
         }
         /* unscale x[k] and h[k] (if required) */
         if (!scaled)
         {  if (k <= m)
            {  rii = lpx_get_rii(lp, k);
               x_k /= rii, h_k /= rii;
            }
            else
            {  sjj = lpx_get_sjj(lp, k-m);
               x_k *= sjj, h_k *= sjj;
            }
         }
         /* determine absolute error */
         temp = fabs(h_k);
         if (kkt->pb_ae_max < temp)
            kkt->pb_ae_max = temp, kkt->pb_ae_ind = k;
         /* determine relative error */
         temp /= (1.0 + fabs(x_k));
         if (kkt->pb_re_max < temp)
            kkt->pb_re_max = temp, kkt->pb_re_ind = k;
      }
      /* estimate the solution quality */
      if (kkt->pb_re_max <= 1e-9)
         kkt->pb_quality = 'H';
      else if (kkt->pb_re_max <= 1e-6)
         kkt->pb_quality = 'M';
      else if (kkt->pb_re_max <= 1e-3)
         kkt->pb_quality = 'L';
      else
         kkt->pb_quality = '?';
      /*--------------------------------------------------------------*/
      /* compute largest absolute and relative errors and corresponding
         column indices for the condition (KKT.DE) */
      kkt->de_ae_max = 0.0, kkt->de_ae_col = 0;
      kkt->de_re_max = 0.0, kkt->de_re_col = 0;
      A_ndx = xcalloc(1+m, sizeof(int));
      A_val = xcalloc(1+m, sizeof(double));
      for (j = m+1; j <= m+n; j++)
      {  /* determine cS[j] */
#if 0 /* 21/XII-2003 */
         cS_j = coef[j];
#else
         sjj = lpx_get_sjj(lp, j-m);
         cS_j = lpx_get_obj_coef(lp, j-m) * sjj;
#endif
         /* determine dS[j] */
#if 0 /* 21/XII-2003 */
         if (tagx[j] == LPX_BS)
            dS_j = 0.0;
         else
            dS_j = cbar[posx[j] - m];
#else
         lpx_get_col_info(lp, j-m, NULL, NULL, &dS_j);
         dS_j *= sjj;
#endif
         /* u[j] := dS[j] - cS[j] */
         u_j = dS_j - cS_j;
         /* u[j] := u[j] + (j-th column of A) * (dR - cR) */
         beg = 1;
         end = lpx_get_mat_col(lp, j-m, A_ndx, A_val);
         for (t = beg; t <= end; t++)
         {  i = A_ndx[t]; /* a[i,j] != 0 */
            /* determine cR[i] */
#if 0 /* 21/XII-2003 */
            cR_i = coef[i];
#else
            cR_i = 0.0;
#endif
            /* determine dR[i] */
#if 0 /* 21/XII-2003 */
            if (tagx[i] == LPX_BS)
               dR_i = 0.0;
            else
               dR_i = cbar[posx[i] - m];
#else
            lpx_get_row_info(lp, i, NULL, NULL, &dR_i);
            rii = lpx_get_rii(lp, i);
            dR_i /= rii;
#endif
            /* u[j] := u[j] + a[i,j] * (dR[i] - cR[i]) */
            rii = lpx_get_rii(lp, i);
            sjj = lpx_get_sjj(lp, j-m);
            u_j += (rii * A_val[t] * sjj) * (dR_i - cR_i);
         }
         /* unscale cS[j], dS[j], and u[j] (if required) */
         if (!scaled)
         {  sjj = lpx_get_sjj(lp, j-m);
            cS_j /= sjj, dS_j /= sjj, u_j /= sjj;
         }
         /* determine absolute error */
         temp = fabs(u_j);
         if (kkt->de_ae_max < temp)
            kkt->de_ae_max = temp, kkt->de_ae_col = j - m;
         /* determine relative error */
         temp /= (1.0 + fabs(dS_j - cS_j));
         if (kkt->de_re_max < temp)
            kkt->de_re_max = temp, kkt->de_re_col = j - m;
      }
      xfree(A_ndx);
      xfree(A_val);
      /* estimate the solution quality */
      if (kkt->de_re_max <= 1e-9)
         kkt->de_quality = 'H';
      else if (kkt->de_re_max <= 1e-6)
         kkt->de_quality = 'M';
      else if (kkt->de_re_max <= 1e-3)
         kkt->de_quality = 'L';
      else
         kkt->de_quality = '?';
      /*--------------------------------------------------------------*/
      /* compute largest absolute and relative errors and corresponding
         variable indices for the condition (KKT.DB) */
      kkt->db_ae_max = 0.0, kkt->db_ae_ind = 0;
      kkt->db_re_max = 0.0, kkt->db_re_ind = 0;
      for (k = 1; k <= m+n; k++)
      {  /* determine c[k] */
#if 0 /* 21/XII-2003 */
         c_k = coef[k];
#else
         if (k <= m)
            c_k = 0.0;
         else
         {  sjj = lpx_get_sjj(lp, k-m);
            c_k = lpx_get_obj_coef(lp, k-m) / sjj;
         }
#endif
         /* determine d[k] */
#if 0 /* 21/XII-2003 */
         d_k = cbar[j-m];
#else
         if (k <= m)
         {  lpx_get_row_info(lp, k, &tagx, NULL, &d_k);
            rii = lpx_get_rii(lp, k);
            d_k /= rii;
         }
         else
         {  lpx_get_col_info(lp, k-m, &tagx, NULL, &d_k);
            sjj = lpx_get_sjj(lp, k-m);
            d_k *= sjj;
         }
#endif
         /* skip basic variable */
         if (tagx == LPX_BS) continue;
         /* compute v[k] */
         v_k = 0.0;
         switch (tagx)
         {  case LPX_NL:
               switch (dir)
               {  case LPX_MIN:
                     if (d_k < 0.0) v_k = d_k;
                     break;
                  case LPX_MAX:
                     if (d_k > 0.0) v_k = d_k;
                     break;
                  default:
                     xassert(dir != dir);
               }
               break;
            case LPX_NU:
               switch (dir)
               {  case LPX_MIN:
                     if (d_k > 0.0) v_k = d_k;
                     break;
                  case LPX_MAX:
                     if (d_k < 0.0) v_k = d_k;
                     break;
                  default:
                     xassert(dir != dir);
               }
               break;
            case LPX_NF:
               v_k = d_k;
               break;
            case LPX_NS:
               break;
            default:
               xassert(tagx != tagx);
         }
         /* unscale c[k], d[k], and v[k] (if required) */
         if (!scaled)
         {  if (k <= m)
            {  rii = lpx_get_rii(lp, k);
               c_k *= rii, d_k *= rii, v_k *= rii;
            }
            else
            {  sjj = lpx_get_sjj(lp, k-m);
               c_k /= sjj, d_k /= sjj, v_k /= sjj;
            }
         }
         /* determine absolute error */
         temp = fabs(v_k);
         if (kkt->db_ae_max < temp)
            kkt->db_ae_max = temp, kkt->db_ae_ind = k;
         /* determine relative error */
         temp /= (1.0 + fabs(d_k - c_k));
         if (kkt->db_re_max < temp)
            kkt->db_re_max = temp, kkt->db_re_ind = k;
      }
      /* estimate the solution quality */
      if (kkt->db_re_max <= 1e-9)
         kkt->db_quality = 'H';
      else if (kkt->db_re_max <= 1e-6)
         kkt->db_quality = 'M';
      else if (kkt->db_re_max <= 1e-3)
         kkt->db_quality = 'L';
      else
         kkt->db_quality = '?';
      /* complementary slackness is always satisfied by definition for
         any basic solution, so not checked */
      kkt->cs_ae_max = 0.0, kkt->cs_ae_ind = 0;
      kkt->cs_re_max = 0.0, kkt->cs_re_ind = 0;
      kkt->cs_quality = 'H';
      return;
}

/* eof */
