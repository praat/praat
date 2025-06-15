/* glplpx15.c (bounds sensitivity analysis routine) */

/***********************************************************************
*  This code is part of GLPK (GNU Linear Programming Kit).
*
*  Author: Brady Hunsaker <bkh@member.fsf.org>.
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

#define _GLPSTD_ERRNO
#define _GLPSTD_STDIO
#include "glpapi.h"
#include "glplib.h"

/*----------------------------------------------------------------------
-- lpx_print_sens_bnds - write bounds sensitivity information.
--
-- *Synopsis*
--
-- #include "glplpx.h"
-- int lpx_print_sens_bnds(LPX *lp, char *fname);
--
-- *Description*
--
-- The routine lpx_print_sens_bnds writes the bounds for objective
-- coefficients, right-hand-sides of constraints, and variable bounds
-- for which the current optimal basic solution remains optimal (for LP
-- only).
--
-- The LP is given by the pointer lp, and the output is written to the
-- file specified by fname.  The current contents of the file will be
-- overwritten.
--
-- Information reported by the routine lpx_print_sens_bnds is intended
-- mainly for visual analysis.
--
-- *Returns*
--
-- If the operation was successful, the routine returns zero. Otherwise
-- the routine prints an error message and returns non-zero. */

int lpx_print_sens_bnds(LPX *lp, const char *fname)
{     FILE *fp = NULL;
      int what, round;
      xprintf("lpx_print_sens_bnds: writing LP problem solution bounds "
         "to `%s'...\n", fname);
#if 1
      /* added by mao */
      /* this routine needs factorization of the current basis matrix
         which, however, does not exist if the basic solution was
         obtained by the lp presolver; therefore we should warm up the
         basis to be sure that the factorization is valid (note that if
         the factorization exists, lpx_warm_up does nothing) */
      lpx_warm_up(lp);
#endif
#if 0 /* 21/XII-2003 by mao */
      if (lp->b_stat == LPX_B_UNDEF)
#else
      if (!lpx_is_b_avail(lp))
#endif
      {  xprintf("lpx_print_sens_bnds: basis information not available "
            "(may be a presolve issue)\n");
         goto fail;
      }
      fp = fopen(fname, "w");
      if (fp == NULL)
      {  xprintf("lpx_print_sens_bnds: can't create `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      /* problem name */
      {  const char *name;
         name = lpx_get_prob_name(lp);
         if (name == NULL) name = "";
         fprintf(fp, "%-12s%s\n", "Problem:", name);
      }
      /* number of rows (auxiliary variables) */
      {  int nr;
         nr = lpx_get_num_rows(lp);
         fprintf(fp, "%-12s%d\n", "Rows:", nr);
      }
      /* number of columns (structural variables) */
      {  int nc;
         nc = lpx_get_num_cols(lp);
         fprintf(fp, "%-12s%d\n", "Columns:", nc);
      }
      /* number of non-zeros (constraint coefficients) */
      {  int nz;
         nz = lpx_get_num_nz(lp);
         fprintf(fp, "%-12s%d\n", "Non-zeros:", nz);
      }
      /* solution status */
      {  int status;
         status = lpx_get_status(lp);
         fprintf(fp, "%-12s%s\n", "Status:",
            status == LPX_OPT    ? "OPTIMAL" :
            status == LPX_FEAS   ? "FEASIBLE" :
            status == LPX_INFEAS ? "INFEASIBLE (INTERMEDIATE)" :
            status == LPX_NOFEAS ? "INFEASIBLE (FINAL)" :
            status == LPX_UNBND  ? "UNBOUNDED" :
            status == LPX_UNDEF  ? "UNDEFINED" : "???");
      }
      /* explanation/warning */
      {  fprintf(fp, "\nExplanation:  This file presents amounts by whi"
            "ch objective coefficients,\n");
         fprintf(fp, "constraint bounds, and variable bounds may be cha"
            "nged in the original problem\n");
         fprintf(fp, "while the optimal basis remains the same.  Note t"
            "hat the optimal solution\n");
         fprintf(fp, "and objective value may change even though the ba"
            "sis remains the same.\n");
         fprintf(fp, "These bounds assume that all parameters remain fi"
            "xed except the one in\n");
         fprintf(fp, "question.  If more than one parameter is changed,"
            " it is possible for the\n");
         fprintf(fp, "optimal basis to change even though each paramete"
            "r stays within its bounds.\n");
         fprintf(fp, "For more details, consult a text on linear progra"
            "mming.\n");
      }
      /* Sensitivity ranges if solution was optimal */
      {  int status;
         status = lpx_get_status(lp);
         if (status == LPX_OPT)
         {  int i,j,k,m,n;
            int dir;
            double max_inc, max_dec;
            int *index;
            double *val;
            fprintf(fp, "\nObjective Coefficient Analysis\n");
            fprintf(fp, "   No.  Column name St    Value       Max incr"
               "ease  Max decrease\n");
            fprintf(fp, "------ ------------ -- ------------- ---------"
               "---- ------------- \n");
            n = lpx_get_num_cols(lp);
            m = lpx_get_num_rows(lp);
            dir = lpx_get_obj_dir(lp);
            /* allocate memory for index and val arrays */
            index = xcalloc(1+n+m, sizeof(int));
            val   = xcalloc(1+n+m, sizeof(double));
            for (j = 1; j <= n; j++)
            {  const char *name;
               int typx, tagx;
               double lb, ub, vx, dx;
               name = lpx_get_col_name(lp, j);
               if (name == NULL) name = "";
               lpx_get_col_bnds(lp, j, &typx, &lb, &ub);
#if 0 /* 21/XII-2003 by mao */
               round = lp->round, lp->round = 1;
               lpx_get_col_info(lp, j, &tagx, &vx, &dx);
               lp->round = round;
#else
               round = lpx_get_int_parm(lp, LPX_K_ROUND);
               lpx_set_int_parm(lp, LPX_K_ROUND, 1);
               lpx_get_col_info(lp, j, &tagx, &vx, &dx);
               lpx_set_int_parm(lp, LPX_K_ROUND, round);
#endif
               /* row/column ordinal number */
               fprintf(fp, "%6d ", j);
               /* row column/name */
               if (strlen(name) <= 12)
                  fprintf(fp, "%-12s ", name);
               else
                  fprintf(fp, "%s\n%20s", name, "");
               /* row/column status */
               fprintf(fp, "%s ",
                  tagx == LPX_BS ? "B " :
                  tagx == LPX_NL ? "NL" :
                  tagx == LPX_NU ? "NU" :
                  tagx == LPX_NF ? "NF" :
                  tagx == LPX_NS ? "NS" : "??");
               /* objective coefficient */
               fprintf(fp, "%13.6g ", lpx_get_obj_coef(lp, j));
               if (tagx == LPX_NL)
               {  if (dir==LPX_MIN)
                  {  /* reduced cost must be positive */
                     max_inc = DBL_MAX; /* really represents infinity */
                     max_dec = dx;
                  }
                  else
                  {  /* reduced cost must be negative */
                     max_inc = -dx;
                     max_dec = DBL_MAX; /* means infinity */
                  }
               }
               if (tagx == LPX_NU)
               {  if (dir==LPX_MIN)
                  {  /* reduced cost must be negative */
                     max_inc = -dx;
                     max_dec = DBL_MAX;
                  }
                  else
                  {  max_inc = DBL_MAX;
                     max_dec = dx;
                  }
               }
               if (tagx == LPX_NF)
               {  /* can't change nonbasic free variables' cost */
                  max_inc = 0.0;
                  max_dec = 0.0;
               }
               if (tagx == LPX_NS)
               {  /* doesn't matter what happens to the cost */
                  max_inc = DBL_MAX;
                  max_dec = DBL_MAX;
               }
               if (tagx == LPX_BS)
               {  int len;
                  /* We need to see how this objective coefficient
                     affects reduced costs of other variables */
                  len = lpx_eval_tab_row(lp, m+j, index, val);
                  max_inc = DBL_MAX;
                  max_dec = DBL_MAX;
                  for (i = 1; i <= len; i++)
                  {  /*int stat;*/
                     int tagx2;
                     double vx2, dx2;
                     double delta;
                     if (index[i]>m)
                        lpx_get_col_info(lp, index[i]-m, &tagx2, &vx2,
                           &dx2);
                     else
                        lpx_get_row_info(lp, index[i], &tagx2, &vx2,
                           &dx2);
                     if (tagx2 == LPX_NL)
                     {  if (val[i] != 0.0)
                        {  delta = dx2 / val[i];
                           if (delta < 0 && -delta < max_inc)
                              max_inc = -delta;
                           else if (delta >0 && delta < max_dec)
                              max_dec = delta;
                        }
                     }
                     if (tagx2 == LPX_NU)
                     {  if (val[i] != 0.0)
                        {  delta = dx2 / val[i];
                           if (delta < 0 && -delta < max_inc)
                              max_inc = -delta;
                           else if (delta > 0 && delta < max_dec)
                              max_dec = delta;
                        }
                     }
                     if (tagx2 == LPX_NF)
                     {  if (val[i] != 0.0)
                        {  max_inc = 0.0;
                           max_dec = 0.0;
                        }
                     }
                  }
               }
               if (max_inc == -0.0) max_inc = 0.0;
               if (max_dec == -0.0) max_dec = 0.0;
               if (max_inc == DBL_MAX)
                  fprintf(fp, "%13s ", "infinity");
               else if (max_inc < 1.0e-12 && max_inc > 0)
                  fprintf(fp, "%13s ", "< eps");
               else
                  fprintf(fp, "%13.6g ", max_inc);
               if (max_dec == DBL_MAX)
                  fprintf(fp, "%13s ", "infinity");
               else if (max_dec < 1.0e-12 && max_dec > 0)
                  fprintf(fp, "%13s ", "< eps");
               else
                  fprintf(fp, "%13.6g ", max_dec);
               fprintf(fp, "\n");
            }
            for (what = 1; what <= 2; what++)
            {  int ij, mn;
               fprintf(fp, "\n");
               fprintf(fp, "%s Analysis\n",
                  what==1? "Constraint Bounds":"Variable Bounds");
               fprintf(fp, "   No. %12s St    Value       Max increase "
                  " Max decrease\n",
                  what==1 ? " Row name":"Column name");
               fprintf(fp, "------ ------------ -- ------------- ------"
                  "------- ------------- \n");
               mn = what==1 ? m : n;
               for (ij = 1; ij <= mn; ij++)
               {  const char *name;
                  int typx, tagx;
                  double lb, ub, vx, dx;
                  if (what==1)
                     name = lpx_get_row_name(lp, ij);
                  else
                     name = lpx_get_col_name(lp, ij);
                  if (name == NULL) name = "";
#if 0 /* 21/XII-2003 by mao */
                  if (what==1)
                  {  lpx_get_row_bnds(lp, ij, &typx, &lb, &ub);
                     round = lp->round, lp->round = 1;
                     lpx_get_row_info(lp, ij, &tagx, &vx, &dx);
                     lp->round = round;
                  }
                  else
                  {  lpx_get_col_bnds(lp, ij, &typx, &lb, &ub);
                     round = lp->round, lp->round = 1;
                     lpx_get_col_info(lp, ij, &tagx, &vx, &dx);
                     lp->round = round;
                  }
#else
                  round = lpx_get_int_parm(lp, LPX_K_ROUND);
                  lpx_set_int_parm(lp, LPX_K_ROUND, 1);
                  if (what==1)
                  {  lpx_get_row_bnds(lp, ij, &typx, &lb, &ub);
                     lpx_get_row_info(lp, ij, &tagx, &vx, &dx);
                  }
                  else
                  {  lpx_get_col_bnds(lp, ij, &typx, &lb, &ub);
                     lpx_get_col_info(lp, ij, &tagx, &vx, &dx);
                  }
                  lpx_set_int_parm(lp, LPX_K_ROUND, round);
#endif
                  /* row/column ordinal number */
                  fprintf(fp, "%6d ", ij);
                  /* row column/name */
                  if (strlen(name) <= 12)
                     fprintf(fp, "%-12s ", name);
                  else
                     fprintf(fp, "%s\n%20s", name, "");
                  /* row/column status */
                  fprintf(fp, "%s ",
                     tagx == LPX_BS ? "B " :
                     tagx == LPX_NL ? "NL" :
                     tagx == LPX_NU ? "NU" :
                     tagx == LPX_NF ? "NF" :
                     tagx == LPX_NS ? "NS" : "??");
                  fprintf(fp, "\n");
                  /* first check lower bound */
                  if (typx == LPX_LO || typx == LPX_DB ||
                      typx == LPX_FX)
                  {  int at_lower;
                     at_lower = 0;
                     if (tagx == LPX_BS || tagx == LPX_NU)
                     {  max_inc = vx - lb;
                        max_dec = DBL_MAX;
                     }
                     if (tagx == LPX_NS)
                     {  max_inc = 0.0;
                        max_dec = 0.0;
                        if (dir == LPX_MIN && dx > 0) at_lower = 1;
                        if (dir == LPX_MAX && dx < 0) at_lower = 1;
                     }
                     if (tagx == LPX_NL || at_lower == 1)
                     {  int len;
                        /* we have to see how it affects basic
                           variables */
                        len = lpx_eval_tab_col(lp, what==1?ij:ij+m,
                           index, val);
                        k = lpx_prim_ratio_test(lp, len, index, val, 1,
                           10e-7);
                        max_inc = DBL_MAX;
                        if (k != 0)
                        {  /*int stat;*/
                           int tagx2, typx2;
                           double vx2, dx2, lb2, ub2;
                           /*double delta;*/
                           double alpha;
                           int l;
                           for (l = 1; l <= len; l++)
                              if (index[l] == k) alpha = val[l];
                           if (k>m)
                           {  lpx_get_col_info(lp, k-m, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_col_bnds(lp, k-m, &typx2, &lb2,
                                 &ub2);
                           }
                           else
                           {  lpx_get_row_info(lp, k, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_row_bnds(lp, k, &typx2, &lb2,
                                 &ub2);
                           }
                           /* Check which direction;
                              remember this is upper bound */
                           if (alpha > 0)
                              max_inc = (ub2 - vx2)/ alpha;
                           else
                              max_inc = (lb2 - vx2)/ alpha;
                        }
                        /* now check lower bound */
                        k = lpx_prim_ratio_test(lp, len, index, val, -1,
                           10e-7);
                        max_dec = DBL_MAX;
                        if (k != 0)
                        {  /*int stat;*/
                           int tagx2, typx2;
                           double vx2, dx2, lb2, ub2;
                           /*double delta;*/
                           double alpha;
                           int l;
                           for (l = 1; l <= len; l++)
                              if (index[l] == k) alpha = val[l];
                           if (k>m)
                           {  lpx_get_col_info(lp, k-m, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_col_bnds(lp, k-m, &typx2, &lb2,
                                 &ub2);
                           }
                           else
                           {  lpx_get_row_info(lp, k, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_row_bnds(lp, k, &typx2, &lb2,
                                 &ub2);
                           }
                           /* Check which direction;
                              remember this is lower bound */
                           if (alpha > 0)
                              max_dec = (vx2 - lb2)/ alpha;
                           else
                              max_dec = (vx2 - ub2)/ alpha;
                        }
                     }
                     /* bound */
                     if (typx == LPX_DB || typx == LPX_FX)
                     {  if (max_inc > ub - lb)
                           max_inc = ub - lb;
                     }
                     fprintf(fp, "         LOWER         %13.6g ", lb);
                     if (max_inc == -0.0) max_inc = 0.0;
                     if (max_dec == -0.0) max_dec = 0.0;
                     if (max_inc == DBL_MAX)
                        fprintf(fp, "%13s ", "infinity");
                     else if (max_inc < 1.0e-12 && max_inc > 0)
                        fprintf(fp, "%13s ", "< eps");
                     else
                        fprintf(fp, "%13.6g ", max_inc);
                     if (max_dec == DBL_MAX)
                        fprintf(fp, "%13s ", "infinity");
                     else if (max_dec < 1.0e-12 && max_dec > 0)
                        fprintf(fp, "%13s ", "< eps");
                     else
                        fprintf(fp, "%13.6g ", max_dec);
                     fprintf(fp, "\n");
                  }
                  /* now check upper bound */
                  if (typx == LPX_UP || typx == LPX_DB ||
                     typx == LPX_FX)
                  {  int at_upper;
                     at_upper = 0;
                     if (tagx == LPX_BS || tagx == LPX_NL)
                     {  max_inc = DBL_MAX;
                        max_dec = ub - vx;
                     }
                     if (tagx == LPX_NS)
                     {  max_inc = 0.0;
                        max_dec = 0.0;
                        if (dir == LPX_MIN && dx < 0) at_upper = 1;
                        if (dir == LPX_MAX && dx > 0) at_upper = 1;
                     }
                     if (tagx == LPX_NU || at_upper == 1)
                     {  int len;
                        /* we have to see how it affects basic
                           variables */
                        len = lpx_eval_tab_col(lp, what==1?ij:ij+m,
                           index, val);
                        k = lpx_prim_ratio_test(lp, len, index, val, 1,
                           10e-7);
                        max_inc = DBL_MAX;
                        if (k != 0)
                        {  /*int stat;*/
                           int tagx2, typx2;
                           double vx2, dx2, lb2, ub2;
                           /*double delta;*/
                           double alpha;
                           int l;
                           for (l = 1; l <= len; l++)
                              if (index[l] == k) alpha = val[l];
                           if (k>m)
                           {  lpx_get_col_info(lp, k-m, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_col_bnds(lp, k-m, &typx2, &lb2,
                                 &ub2);
                           }
                           else
                           {  lpx_get_row_info(lp, k, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_row_bnds(lp, k, &typx2, &lb2,
                                 &ub2);
                           }
                           /* Check which direction;
                              remember this is upper bound */
                           if (alpha > 0)
                              max_inc = (ub2 - vx2)/ alpha;
                           else
                              max_inc = (lb2 - vx2)/ alpha;
                        }
                        /* now check lower bound */
                        k = lpx_prim_ratio_test(lp, len, index, val, -1,
                           10e-7);
                        max_dec = DBL_MAX;
                        if (k != 0)
                        {  /*int stat;*/
                           int tagx2, typx2;
                           double vx2, dx2, lb2, ub2;
                           /*double delta;*/
                           double alpha;
                           int l;
                           for (l = 1; l <= len; l++)
                              if (index[l] == k) alpha = val[l];
                           if (k>m)
                           {  lpx_get_col_info(lp, k-m, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_col_bnds(lp, k-m, &typx2, &lb2,
                                 &ub2);
                           }
                           else
                           {  lpx_get_row_info(lp, k, &tagx2, &vx2,
                                 &dx2);
                              lpx_get_row_bnds(lp, k, &typx2, &lb2,
                                 &ub2);
                           }
                           /* Check which direction;
                              remember this is lower bound */
                           if (alpha > 0)
                              max_dec = (vx2 - lb2)/ alpha;
                           else
                              max_dec = (vx2 - ub2)/ alpha;
                        }
                     }
                     if (typx == LPX_DB || typx == LPX_FX)
                     {  if (max_dec > ub - lb)
                           max_dec = ub - lb;
                     }
                     /* bound */
                     fprintf(fp, "         UPPER         %13.6g ", ub);
                     if (max_inc == -0.0) max_inc = 0.0;
                     if (max_dec == -0.0) max_dec = 0.0;
                     if (max_inc == DBL_MAX)
                        fprintf(fp, "%13s ", "infinity");
                     else if (max_inc < 1.0e-12 && max_inc > 0)
                        fprintf(fp, "%13s ", "< eps");
                     else
                        fprintf(fp, "%13.6g ", max_inc);
                     if (max_dec == DBL_MAX)
                        fprintf(fp, "%13s ", "infinity");
                     else if (max_dec < 1.0e-12 && max_dec > 0)
                        fprintf(fp, "%13s ", "< eps");
                     else
                        fprintf(fp, "%13.6g ", max_dec);
                     fprintf(fp, "\n");
                  }
               }
            }
            /* free the memory we used */
            xfree(index);
            xfree(val);
         }
         else fprintf(fp, "No range information since solution is not o"
            "ptimal.\n");
      }
      fprintf(fp, "\n");
      fprintf(fp, "End of output\n");
      fflush(fp);
      if (ferror(fp))
      {  xprintf("lpx_print_sens_bnds: can't write to `%s' - %s\n",
            fname, strerror(errno));
         goto fail;
      }
      fclose(fp);
      return 0;
fail: if (fp != NULL) fclose(fp);
      return 1;
}

/* eof */
