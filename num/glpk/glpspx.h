/* glpspx.h (simplex method) */

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

#ifndef _GLPSPX_H
#define _GLPSPX_H

#include "glpbfd.h"

typedef struct SPX SPX;

struct SPX
{     /* data block used by simplex method routines */
      /*--------------------------------------------------------------*/
      /* LP problem data */
      int m;
      /* number of rows (auxiliary variables), m > 0 */
      int n;
      /* number of columns (structural variables), n > 0 */
      int *type; /* int type[1+m+n]; */
      /* type[k], 1 <= k <= m+n, is the type of variable x[k]:
         GLP_FR - free variable
         GLP_LO - variable with lower bound
         GLP_UP - variable with upper bound
         GLP_DB - double-bounded variable
         GLP_FX - fixed variable */
      double *lb; /* double lb[1+m+n]; */
      /* lb[k], 1 <= k <= m+n, is an lower bound of variable x[k];
         if x[k] has no lower bound, lb[k] is zero */
      double *ub; /* double ub[1+m+n]; */
      /* ub[k], 1 <= k <= m+n, is an upper bound of variable x[k];
         if x[k] has no upper bound, ub[k] is zero;
         if x[k] is of fixed type, ub[k] is equal to lb[k] */
      int dir;
      /* optimization direction flag (objective "sense"):
         GLP_MIN - minimization
         GLP_MAX - maximization */
      double *coef; /* double coef[1+m+n]; */
      /* coef[0] is a constant term of the objective function;
         coef[k], 1 <= k <= m+n, is a coefficient of the objective
         function at variable x[k] (note that auxiliary variables also
         may have non-zero objective coefficients) */
      /*--------------------------------------------------------------*/
      /* constraint matrix (has m rows and n columns) */
      int *A_ptr; /* int A_ptr[1+m+1]; */
      int *A_ind; /* int A_ind[A_ptr[m+1]]; */
      double *A_val; /* double A_val[A_ptr[m+1]]; */
      /* constraint matrix in storage-by-rows format */
      int *AT_ptr; /* int AT_ptr[1+n+1]; */
      int *AT_ind; /* int AT_ind[AT_ptr[n+1]]; */
      double *AT_val; /* double AT_val[AT_ptr[n+1]]; */
      /* constraint matrix in storage-by-columns format */
      double *col; /* double col[1+m]; */
      /* working array */
      /*--------------------------------------------------------------*/
      /* basis factorization */
      int valid;
      /* the factorization is valid only if this flag is set */
      int *posx; /* int posx[1+m+n]; */
      /* posx[k], 1 <= k <= m+n, is the position of variable x[k] in
         the vector of basic variables xB or non-basic variables xN:
         posx[k] = i   means that x[k] = xB[i], 1 <= i <= m
         posx[k] = m+j means that x[k] = xN[j], 1 <= j <= n
         (valid only if the basis factorization is valid) */
      int *indx; /* int indx[1+m+n]; */
      /* indx[i], 1 <= i <= m, is the original number of basic variable
         xB[i], i.e. indx[i] = k means that posx[k] = i
         indx[m+j], 1 <= j <= n, is the original number of non-basic
         variable xN[j], i.e. indx[m+j] = k means that posx[k] = m+j
         (valid only if the basis factorization is valid) */
      BFD *bfd; /* BFD bfd[1:m,1:m]; */
      /* basis factorization driver */
      /*--------------------------------------------------------------*/
      /* basic solution */
      int p_stat;
      /* primal basic solution status:
         GLP_UNDEF  - primal solution is undefined
         GLP_FEAS   - primal solution is feasible
         GLP_INFEAS - primal solution is infeasible
         GLP_NOFEAS - no primal feasible solution exists */
      int d_stat;
      /* dual basic solution status:
         GLP_UNDEF  - dual solution is undefined
         GLP_FEAS   - dual solution is feasible
         GLP_INFEAS - dual solution is infeasible
         GLP_NOFEAS - no dual feasible solution exists */
      int *stat; /* int stat[1+m+n]; */
      /* stat[k], 1 <= k <= m+n, is the status of variable x[k]:
         GLP_BS - basic variable
         GLP_NL - non-basic variable on lower bound
         GLP_NU - non-basic variable on upper bound
         GLP_NF - non-basic free variable
         GLP_NS - non-basic fixed variable */
      double *bbar; /* double bbar[1+m]; */
      /* bbar[0] is not used;
         bbar[i], 1 <= i <= m, is a value of basic variable xB[i] */
      double *pi; /* double pi[1+m]; */
      /* pi[0] is not used;
         pi[i], 1 <= i <= m, is a simplex (Lagrange) multiplier, which
         corresponds to the i-th row (equality constraint) */
      double *cbar; /* double cbar[1+n]; */
      /* cbar[0] is not used;
         cbar[j], 1 <= j <= n, is a reduced cost of non-basic variable
         xN[j] */
      int some;
      /* ordinal number of some auxiliary or structural variable which
         has certain property, 1 <= some <= m+n */
      /*--------------------------------------------------------------*/
      /* control parameters and statistics */
      int msg_lev;
      /* level of messages output by the solver:
         0 - no output
         1 - error messages only
         2 - normal output
         3 - full output (includes informational messages) */
      int dual;
      /* dual simplex option:
         0 - do not use the dual simplex
         1 - if the initial basic solution being primal infeasible is
             dual feasible, use the dual simplex */
      int price;
      /* pricing option (for both primal and dual simplex):
         0 - textbook pricing
         1 - steepest edge pricing */
      double relax;
      /* relaxation parameter used in the ratio test; if it is zero,
         the textbook ratio test is used; if it is non-zero (should be
         positive), Harris' two-pass ratio test is used; in the latter
         case on the first pass basic variables (in the case of primal
         simplex) or reduced costs of non-basic variables (in the case
         of dual simplex) are allowed to slightly violate their bounds,
         but not more than (relax * tol_bnd) or (relax * tol_dj) (thus,
         relax is a percentage of tol_bnd or tol_dj) */
      double tol_bnd;
      /* relative tolerance used to check if the current basic solution
         is primal feasible */
      double tol_dj;
      /* absolute tolerance used to check if the current basic solution
         is dual feasible */
      double tol_piv;
      /* relative tolerance used to choose eligible pivotal elements of
         the simplex table in the ratio test */
      double obj_ll;
      /* lower limit of the objective function; if on the phase II the
         objective function reaches this limit and continues decreasing,
         the solver stops the search */
      double obj_ul;
      /* upper limit of the objective function; if on the phase II the
         objective function reaches this limit and continues increasing,
         the solver stops the search */
      int it_lim;
      /* simplex iterations limit; if this value is positive, it is
         decreased by one each time when one simplex iteration has been
         performed, and reaching zero value signals the solver to stop
         the search; negative value means no iterations limit */
      int it_cnt;
      /* simplex iterations count; this count is increased by one each
         time when one simplex iteration has been performed */
      double tm_lim;
      /* searching time limit, in seconds; if this value is positive,
         it is decreased each time when one simplex iteration has been
         performed by the amount of time spent for the iteration, and
         reaching zero value signals the solver to stop the search;
         negative value means no time limit */
      int out_frq;
      /* output frequency, in iterations; this parameter specifies how
         frequently the solver sends information about the solution to
         the standard output */
      double out_dly;
      /* output delay, in seconds; this parameter specifies how long
         the solver should delay sending information about the solution
         to the standard output; zero value means no delay */
      /*--------------------------------------------------------------*/
      /* working segment */
      int meth;
      /* which method is used:
         'P' - primal simplex
         'D' - dual simplex */
      int p;
      /* the number of basic variable xB[p], 1 <= p <= m, chosen to
         leave the basis; the special case p < 0 means that non-basic
         double-bounded variable xN[q] just goes to its opposite bound,
         and the basis remains unchanged; p = 0 means that no choice
         can be made (in the case of primal simplex non-basic variable
         xN[q] can infinitely change, in the case of dual simplex the
         current basis is primal feasible) */
      int p_tag;
      /* if 1 <= p <= m, p_tag is a non-basic tag, which should be set
         for the variable xB[p] after it has left the basis */
      int q;
      /* the number of non-basic variable xN[q], 1 <= q <= n, chosen to
         enter the basis; q = 0 means that no choice can be made (in
         the case of primal simplex the current basis is dual feasible,
         in the case of dual simplex the dual variable that corresponds
         to xB[p] can infinitely change) */
      double *zeta; /* double zeta[1+m]; */
      /* the p-th row of the inverse inv(B) */
      double *ap; /* double ap[1+n]; */
      /* the p-th row of the current simplex table:
         ap[0] is not used;
         ap[j], 1 <= j <= n, is an influence coefficient, which defines
         how the non-basic variable xN[j] affects on the basic variable
         xB[p] = ... + ap[j] * xN[j] + ... */
      double *aq; /* double aq[1+m]; */
      /* the q-th column of the current simplex table;
         aq[0] is not used;
         aq[i], 1 <= i <= m, is an influence coefficient, which defines
         how the non-basic variable xN[q] affects on the basic variable
         xB[i] = ... + aq[i] * xN[q] + ... */
      double *gvec; /* double gvec[1+n]; */
      /* gvec[0] is not used;
         gvec[j], 1 <= j <= n, is a weight of non-basic variable xN[j];
         this vector is used to price non-basic variables in the primal
         simplex (for example, using the steepest edge technique) */
      double *dvec; /* double dvec[1+m]; */
      /* dvec[0] is not used;
         dvec[i], 1 <= i <= m, is a weight of basic variable xB[i]; it
         is used to price basic variables in the dual simplex */
      int *refsp; /* int refsp[1+m+n]; */
      /* the current reference space (used in the projected steepest
         edge technique); the flag refsp[k], 1 <= k <= m+n, is set if
         the variable x[k] belongs to the current reference space */
      int count;
      /* if this count (used in the projected steepest edge technique)
         gets zero, the reference space is automatically redefined */
      double *work; /* double work[1+m+n]; */
      /* working array (used for various purposes) */
      int *orig_type; /* orig_type[1+m+n]; */
      /* is used to save the original types of variables */
      double *orig_lb; /* orig_lb[1+m+n]; */
      /* is used to save the original lower bounds of variables */
      double *orig_ub; /* orig_ub[1+m+n]; */
      /* is used to save the original upper bounds of variables */
      int orig_dir;
      /* is used to save the original optimization direction */
      double *orig_coef; /* orig_coef[1+m+n]; */
      /* is used to save the original objective coefficients */
};

#define spx_invert            _glp_spx_invert
#define spx_ftran             _glp_spx_ftran
#define spx_btran             _glp_spx_btran
#define spx_update            _glp_spx_update
#define spx_eval_xn_j         _glp_spx_eval_xn_j
#define spx_eval_bbar         _glp_spx_eval_bbar
#define spx_eval_pi           _glp_spx_eval_pi
#define spx_eval_cbar         _glp_spx_eval_cbar
#define spx_eval_obj          _glp_spx_eval_obj
#define spx_eval_col          _glp_spx_eval_col
#define spx_eval_rho          _glp_spx_eval_rho
#define spx_eval_row          _glp_spx_eval_row
#define spx_check_bbar        _glp_spx_check_bbar
#define spx_check_cbar        _glp_spx_check_cbar
#define spx_prim_chuzc        _glp_spx_prim_chuzc
#define spx_prim_chuzr        _glp_spx_prim_chuzr
#define spx_dual_chuzr        _glp_spx_dual_chuzr
#define spx_dual_chuzc        _glp_spx_dual_chuzc
#define spx_update_bbar       _glp_spx_update_bbar
#define spx_update_pi         _glp_spx_update_pi
#define spx_update_cbar       _glp_spx_update_cbar
#define spx_change_basis      _glp_spx_change_basis
#define spx_err_in_bbar       _glp_spx_err_in_bbar
#define spx_err_in_pi         _glp_spx_err_in_pi
#define spx_err_in_cbar       _glp_spx_err_in_cbar
#define spx_reset_refsp       _glp_spx_reset_refsp
#define spx_update_gvec       _glp_spx_update_gvec
#define spx_err_in_gvec       _glp_spx_err_in_gvec
#define spx_update_dvec       _glp_spx_update_dvec
#define spx_err_in_dvec       _glp_spx_err_in_dvec

#define spx_warm_up           _glp_spx_warm_up
#define spx_prim_opt          _glp_spx_prim_opt
#define spx_prim_feas         _glp_spx_prim_feas
#define spx_dual_opt          _glp_spx_dual_opt
#define spx_simplex           _glp_spx_simplex

/* simplex method generic routines -----------------------------------*/

int spx_invert(SPX *spx);
/* reinvert the basis matrix */

void spx_ftran(SPX *spx, double x[]);
/* perform forward transformation (FTRAN) */

void spx_btran(SPX *spx, double x[]);
/* perform backward transformation (BTRAN) */

int spx_update(SPX *spx, int j);
/* update factorization for adjacent basis matrix */

double spx_eval_xn_j(SPX *spx, int j);
/* determine value of non-basic variable */

void spx_eval_bbar(SPX *spx);
/* compute values of basic variables */

void spx_eval_pi(SPX *spx);
/* compute simplex multipliers */

void spx_eval_cbar(SPX *spx);
/* compute reduced costs of non-basic variables */

double spx_eval_obj(SPX *spx);
/* compute value of the objective function */

void spx_eval_col(SPX *spx, int j, double col[], int save);
/* compute column of the simplex table */

void spx_eval_rho(SPX *spx, int i, double rho[]);
/* compute row of the inverse */

void spx_eval_row(SPX *spx, double rho[], double row[]);
/* compute row of the simplex table */

double spx_check_bbar(SPX *spx, double tol);
/* check primal feasibility */

double spx_check_cbar(SPX *spx, double tol);
/* check dual feasibility */

int spx_prim_chuzc(SPX *spx, double tol);
/* choose non-basic variable (primal simplex) */

int spx_prim_chuzr(SPX *spx, double relax);
/* choose basic variable (primal simplex) */

void spx_dual_chuzr(SPX *spx, double tol);
/* choose basic variable (dual simplex) */

int spx_dual_chuzc(SPX *spx, double relax);
/* choose non-basic variable (dual simplex) */

void spx_update_bbar(SPX *spx, double *obj);
/* update values of basic variables */

void spx_update_pi(SPX *spx);
/* update simplex multipliers */

void spx_update_cbar(SPX *spx, int all);
/* update reduced costs of non-basic variables */

int spx_change_basis(SPX *spx);
/* change basis and update the factorization */

double spx_err_in_bbar(SPX *spx);
/* compute maximal absolute error in bbar */

double spx_err_in_pi(SPX *spx);
/* compute maximal absolute error in pi */

double spx_err_in_cbar(SPX *spx, int all);
/* compute maximal absolute error in cbar */

void spx_reset_refsp(SPX *spx);
/* reset the reference space */

void spx_update_gvec(SPX *spx);
/* update the vector gamma for adjacent basis */

double spx_err_in_gvec(SPX *spx);
/* compute maximal absolute error in gvec */

void spx_update_dvec(SPX *spx);
/* update the vector delta for adjacent basis */

double spx_err_in_dvec(SPX *spx);
/* compute maximal absolute error in dvec */

/* simplex method solver routines ------------------------------------*/

void spx_warm_up(SPX *spx);
/* "warm up" the initial basis */

int spx_prim_opt(SPX *spx);
/* find optimal solution (primal simplex) */

int spx_prim_feas(SPX *spx);
/* find primal feasible solution (primal simplex) */

int spx_dual_opt(SPX *spx);
/* find optimal solution (dual simplex) */

int spx_simplex(SPX *spx);
/* base driver to the simplex method */

#endif

/* eof */
