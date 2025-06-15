/* glplpx08.c (branch-and-bound solver routine) */

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

/*----------------------------------------------------------------------
-- lpx_check_int - check integer feasibility conditions.
--
-- SYNOPSIS
--
-- #include "glplpx.h"
-- void lpx_check_int(LPX *lp, LPXKKT *kkt);
--
-- DESCRIPTION
--
-- The routine lpx_check_int checks integer fasibility conditions for
-- current mip solution specified by a problem object, which the
-- parameter lp points to.
--
-- The parameter kkt is a pointer to the structure LPXKKT, to which the
-- routine stores results of checking.
--
-- The routine lpx_check_int acts like the routine lpx_check_kkt with
-- the difference that:
--
-- the current mip solution is used;
--
-- only (KKT.PE) and (KKT.PB) conditions are checked. */

void lpx_check_int(LPX *lp, LPXKKT *kkt)
{     int m = lpx_get_num_rows(lp);
      int n = lpx_get_num_cols(lp);
      int *ind, i, len, t, j, k, type;
      double *val, xR_i, g_i, xS_j, temp, lb, ub, x_k, h_k;
      /*--------------------------------------------------------------*/
      /* compute largest absolute and relative errors and corresponding
         row indices for the condition (KKT.PE) */
      kkt->pe_ae_max = 0.0, kkt->pe_ae_row = 0;
      kkt->pe_re_max = 0.0, kkt->pe_re_row = 0;
      ind = xcalloc(1+n, sizeof(int));
      val = xcalloc(1+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  /* determine xR[i] */
         xR_i = lpx_mip_row_val(lp, i);
         /* g[i] := xR[i] */
         g_i = xR_i;
         /* g[i] := g[i] - (i-th row of A) * xS */
         len = lpx_get_mat_row(lp, i, ind, val);
         for (t = 1; t <= len; t++)
         {  j = ind[t];
            /* determine xS[j] */
            xS_j = lpx_mip_col_val(lp, j);
            /* g[i] := g[i] - a[i,j] * xS[j] */
            g_i -= val[t] * xS_j;
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
      xfree(ind);
      xfree(val);
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
         {  i = k;
            type = lpx_get_row_type(lp, i);
            lb = lpx_get_row_lb(lp, i);
            ub = lpx_get_row_ub(lp, i);
            x_k = lpx_mip_row_val(lp, i);
         }
         else
         {  j = k - m;
            type = lpx_get_col_type(lp, j);
            lb = lpx_get_col_lb(lp, j);
            ub = lpx_get_col_ub(lp, j);
            x_k = lpx_mip_col_val(lp, j);
         }
         /* compute h[k] */
         h_k = 0.0;
         switch (type)
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
               xassert(type != type);
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
      return;
}

/* eof */
