/* glpapi.h (application program interface) */

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

#ifndef _GLPAPI_H
#define _GLPAPI_H

#include "glpavl.h"
#include "glpbfd.h"

#define _GLP_PROB
#define _GLP_BFCP

typedef struct glp_prob glp_prob;
typedef struct GLPROW GLPROW;
typedef struct GLPCOL GLPCOL;
typedef struct GLPAIJ GLPAIJ;
typedef struct glp_bfcp glp_bfcp;

struct glp_prob
{     /* LP/MIP problem object */
      DMP *pool;
      /* memory pool to store problem object components */
      void *cps; /* struct LPXCPS *cps; */
      /* reserved for downward compatibility */
#if 0
      char *str_buf; /* char str_buf[255+1]; */
      /* working buffer to store null-terminated character strings */
#endif
      void *tree; /* glp_tree *tree */
      /* pointer to the branch-and-bound tree; set by the MIP solver
         when this object is used in the tree as a core MIP object */
      /*--------------------------------------------------------------*/
      /* LP/MIP data */
      char *name;
      /* problem name (1 to 255 chars); NULL means no name is assigned
         to the problem */
      char *obj;
      /* objective function name (1 to 255 chars); NULL means no name
         is assigned to the objective function */
      int dir;
      /* optimization direction flag (objective "sense"):
         GLP_MIN - minimization
         GLP_MAX - maximization */
      double c0;
      /* constant term of the objective function ("shift") */
      int m_max;
      /* length of the array of rows (enlarged automatically) */
      int n_max;
      /* length of the array of columns (enlarged automatically) */
      int m;
      /* number of rows, 0 <= m <= m_max */
      int n;
      /* number of columns, 0 <= n <= n_max */
      int nnz;
      /* number of non-zero constraint coefficients, nnz >= 0 */
      GLPROW **row; /* GLPROW *row[1+m_max]; */
      /* row[i], 1 <= i <= m, is a pointer to i-th row */
      GLPCOL **col; /* GLPCOL *col[1+n_max]; */
      /* col[j], 1 <= j <= n, is a pointer to j-th column */
      AVL *r_tree;
      /* row index to find rows by their names; NULL means this index
         does not exist */
      AVL *c_tree;
      /* column index to find columns by their names; NULL means this
         index does not exist */
      /*--------------------------------------------------------------*/
      /* basis factorization (LP) */
      int valid;
      /* the factorization is valid only if this flag is set */
      int *bhead; /* int bhead[1+m_max]; */
      /* basis header (valid only if the factorization is valid);
         bhead[i] = k is the ordinal number of auxiliary (1 <= k <= m)
         or structural (m+1 <= k <= m+n) variable which corresponds to
         i-th basic variable xB[i], 1 <= i <= m */
      glp_bfcp *bfcp;
      /* basis factorization control parameters; may be NULL */
      BFD *bfd; /* BFD bfd[1:m,1:m]; */
      /* basis factorization driver; may be NULL */
      /*--------------------------------------------------------------*/
      /* basic solution (LP) */
      int pbs_stat;
      /* primal basic solution status:
         GLP_UNDEF  - primal solution is undefined
         GLP_FEAS   - primal solution is feasible
         GLP_INFEAS - primal solution is infeasible
         GLP_NOFEAS - no primal feasible solution exists */
      int dbs_stat;
      /* dual basic solution status:
         GLP_UNDEF  - dual solution is undefined
         GLP_FEAS   - dual solution is feasible
         GLP_INFEAS - dual solution is infeasible
         GLP_NOFEAS - no dual feasible solution exists */
      double obj_val;
      /* objective function value */
      int it_cnt;
      /* simplex method iteration count; increased by one on performing
         one simplex iteration */
      int some;
      /* ordinal number of some auxiliary or structural variable having
         certain property, 0 <= some <= m+n */
      /*--------------------------------------------------------------*/
      /* interior-point solution (LP) */
      int ipt_stat;
      /* interior-point solution status:
         GLP_UNDEF  - interior solution is undefined
         GLP_OPT    - interior solution is optimal */
      double ipt_obj;
      /* objective function value */
      /*--------------------------------------------------------------*/
      /* integer solution (MIP) */
      int mip_stat;
      /* integer solution status:
         GLP_UNDEF  - integer solution is undefined
         GLP_OPT    - integer solution is optimal
         GLP_FEAS   - integer solution is feasible
         GLP_NOFEAS - no integer solution exists */
      double mip_obj;
      /* objective function value */
};

struct GLPROW
{     /* LP/MIP row (auxiliary variable) */
      int i;
      /* ordinal number (1 to m) assigned to this row */
      char *name;
      /* row name (1 to 255 chars); NULL means no name is assigned to
         this row */
      AVLNODE *node;
      /* pointer to corresponding node in the row index; NULL means
         that either the row index does not exist or this row has no
         name assigned */
      int type;
      /* type of the auxiliary variable:
         GLP_FR - free variable
         GLP_LO - variable with lower bound
         GLP_UP - variable with upper bound
         GLP_DB - double-bounded variable
         GLP_FX - fixed variable */
      double lb; /* non-scaled */
      /* lower bound; if the row has no lower bound, lb is zero */
      double ub; /* non-scaled */
      /* upper bound; if the row has no upper bound, ub is zero */
      /* if the row type is GLP_FX, ub is equal to lb */
      GLPAIJ *ptr; /* non-scaled */
      /* pointer to doubly linked list of constraint coefficients which
         are placed in this row */
      double rii;
      /* diagonal element r[i,i] of scaling matrix R for this row;
         if the scaling is not used, r[i,i] is 1 */
      int stat;
      /* status of the auxiliary variable:
         GLP_BS - basic variable
         GLP_NL - non-basic variable on lower bound
         GLP_NU - non-basic variable on upper bound
         GLP_NF - non-basic free variable
         GLP_NS - non-basic fixed variable */
      int bind;
      /* if the auxiliary variable is basic, bhead[bind] refers to this
         row, otherwise, bind is 0; this attribute is valid only if the
         basis factorization is valid */
      double prim; /* non-scaled */
      /* primal value of the auxiliary variable in basic solution */
      double dual; /* non-scaled */
      /* dual value of the auxiliary variable in basic solution */
      double pval; /* non-scaled */
      /* primal value of the auxiliary variable in interior solution */
      double dval; /* non-scaled */
      /* dual value of the auxiliary variable in interior solution */
      double mipx; /* non-scaled */
      /* primal value of the auxiliary variable in integer solution */
};

struct GLPCOL
{     /* LP/MIP column (structural variable) */
      int j;
      /* ordinal number (1 to n) assigned to this column */
      char *name;
      /* column name (1 to 255 chars); NULL means no name is assigned
         to this column */
      AVLNODE *node;
      /* pointer to corresponding node in the column index; NULL means
         that either the column index does not exist or the column has
         no name assigned */
      int kind;
      /* kind of the structural variable:
         GLP_CV - continuous variable
         GLP_IV - integer or binary variable */
      int type;
      /* type of the structural variable:
         GLP_FR - free variable
         GLP_LO - variable with lower bound
         GLP_UP - variable with upper bound
         GLP_DB - double-bounded variable
         GLP_FX - fixed variable */
      double lb; /* non-scaled */
      /* lower bound; if the column has no lower bound, lb is zero */
      double ub; /* non-scaled */
      /* upper bound; if the column has no upper bound, ub is zero */
      /* if the column type is GLP_FX, ub is equal to lb */
      double coef; /* non-scaled */
      /* objective coefficient at the structural variable */
      GLPAIJ *ptr; /* non-scaled */
      /* pointer to doubly linked list of constraint coefficients which
         are placed in this column */
      double sjj;
      /* diagonal element s[j,j] of scaling matrix S for this column;
         if the scaling is not used, s[j,j] is 1 */
      int stat;
      /* status of the structural variable:
         GLP_BS - basic variable
         GLP_NL - non-basic variable on lower bound
         GLP_NU - non-basic variable on upper bound
         GLP_NF - non-basic free variable
         GLP_NS - non-basic fixed variable */
      int bind;
      /* if the structural variable is basic, bhead[bind] refers to
         this column; otherwise, bind is 0; this attribute is valid only
         if the basis factorization is valid */
      double prim; /* non-scaled */
      /* primal value of the structural variable in basic solution */
      double dual; /* non-scaled */
      /* dual value of the structural variable in basic solution */
      double pval; /* non-scaled */
      /* primal value of the structural variable in interior solution */
      double dval; /* non-scaled */
      /* dual value of the structural variable in interior solution */
      double mipx; /* non-scaled */
      /* primal value of the structural variable in integer solution */
};

struct GLPAIJ
{     /* constraint coefficient a[i,j] */
      GLPROW *row;
      /* pointer to row, where this coefficient is placed */
      GLPCOL *col;
      /* pointer to column, where this coefficient is placed */
      double val;
      /* numeric (non-zero) value of this coefficient */
      GLPAIJ *r_prev;
      /* pointer to previous coefficient in the same row */
      GLPAIJ *r_next;
      /* pointer to next coefficient in the same row */
      GLPAIJ *c_prev;
      /* pointer to previous coefficient in the same column */
      GLPAIJ *c_next;
      /* pointer to next coefficient in the same column */
};

#include "glpk.h"

/**********************************************************************/

struct LPXCPS
{     /* control parameters and statistics */
      int msg_lev;
      /* level of messages output by the solver:
         0 - no output
         1 - error messages only
         2 - normal output
         3 - full output (includes informational messages) */
      int scale;
      /* scaling option:
         0 - no scaling
         1 - equilibration scaling
         2 - geometric mean scaling
         3 - geometric mean scaling, then equilibration scaling */
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
      int round;
      /* solution rounding option:
         0 - report all computed values and reduced costs "as is"
         1 - if possible (allowed by the tolerances), replace computed
             values and reduced costs which are close to zero by exact
             zeros */
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
      int branch; /* MIP */
      /* branching heuristic:
         0 - branch on first variable
         1 - branch on last variable
         2 - branch using heuristic by Driebeck and Tomlin
         3 - branch on most fractional variable */
      int btrack; /* MIP */
      /* backtracking heuristic:
         0 - select most recent node (depth first search)
         1 - select earliest node (breadth first search)
         2 - select node using the best projection heuristic
         3 - select node with best local bound */
      double tol_int; /* MIP */
      /* absolute tolerance used to check if the current basic solution
         is integer feasible */
      double tol_obj; /* MIP */
      /* relative tolerance used to check if the value of the objective
         function is not better than in the best known integer feasible
         solution */
      int mps_info; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps outputs several
         comment cards that contains some information about the problem;
         otherwise the routine outputs no comment cards */
      int mps_obj; /* lpx_write_mps */
      /* this parameter tells the routine lpx_write_mps how to output
         the objective function row:
         0 - never output objective function row
         1 - always output objective function row
         2 - output objective function row if and only if the problem
             has no free rows */
      int mps_orig; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps uses original
         row and column symbolic names; otherwise the routine generates
         plain names using ordinal numbers of rows and columns */
      int mps_wide; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps uses all data
         fields; otherwise the routine keeps fields 5 and 6 empty */
      int mps_free; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps omits column
         and vector names everytime if possible (free style); otherwise
         the routine never omits these names (pedantic style) */
      int mps_skip; /* lpx_write_mps */
      /* if this flag is set, the routine lpx_write_mps skips empty
         columns (i.e. which has no constraint coefficients); otherwise
         the routine outputs all columns */
      int lpt_orig; /* lpx_write_lpt */
      /* if this flag is set, the routine lpx_write_lpt uses original
         row and column symbolic names; otherwise the routine generates
         plain names using ordinal numbers of rows and columns */
      int presol; /* lpx_simplex */
      /* LP presolver option:
         0 - do not use LP presolver
         1 - use LP presolver */
      int binarize; /* lpx_intopt */
      /* if this flag is set, the routine lpx_intopt replaces integer
         columns by binary ones */
      int use_cuts; /* lpx_intopt */
      /* if this flag is set, the routine lpx_intopt tries generating
         cutting planes:
         LPX_C_COVER  - mixed cover cuts
         LPX_C_CLIQUE - clique cuts
         LPX_C_GOMORY - Gomory's mixed integer cuts
         LPX_C_ALL    - all cuts */
      double mip_gap; /* MIP */
      /* relative MIP gap tolerance */
};

#define lpx_order_matrix _glp_lpx_order_matrix
void lpx_order_matrix(LPX *lp);
/* order rows and columns of the constraint matrix */

void glp_put_solution(glp_prob *lp, int inval, const int *p_stat,
      const int *d_stat, const double *obj_val, const int r_stat[],
      const double r_prim[], const double r_dual[], const int c_stat[],
      const double c_prim[], const double c_dual[]);
/* store basic solution components */

#define lpx_put_lp_basis _glp_lpx_put_lp_basis
void lpx_put_lp_basis(LPX *lp, int b_stat, int basis[], BFD *b_inv);
/* store LP basis information */

#define lpx_put_ray_info _glp_lpx_put_ray_info
void lpx_put_ray_info(LPX *lp, int k);
/* store row/column which causes unboundness */

#define lpx_put_ipt_soln _glp_lpx_put_ipt_soln
void lpx_put_ipt_soln(LPX *lp, int t_stat, double row_pval[],
      double row_dval[], double col_pval[], double col_dval[]);
/* store interior-point solution components */

#define lpx_put_mip_soln _glp_lpx_put_mip_soln
void lpx_put_mip_soln(LPX *lp, int i_stat, double row_mipx[],
      double col_mipx[]);
/* store mixed integer solution components */

BFD *_glp_access_bfd(glp_prob *lp);
/* access the basis factorization object */

#define lpx_eval_b_prim _glp_lpx_eval_b_prim
void lpx_eval_b_prim(LPX *lp, double row_prim[], double col_prim[]);
/* compute primal basic solution components */

#define lpx_eval_b_dual _glp_lpx_eval_b_dual
void lpx_eval_b_dual(LPX *lp, double row_dual[], double col_dual[]);
/* compute dual basic solution components */

#define lpx_remove_tiny _glp_lpx_remove_tiny
int lpx_remove_tiny(int ne, int ia[], int ja[], double ar[],
      double eps);
/* remove zero and tiny elements */

#define lpx_reduce_form _glp_lpx_reduce_form
int lpx_reduce_form(LPX *lp, int len, int ind[], double val[],
      double work[]);
/* reduce linear form */

#define lpx_eval_row _glp_lpx_eval_row
double lpx_eval_row(LPX *lp, int len, int ind[], double val[]);
/* compute explicitly specified row */

#define lpx_eval_degrad _glp_lpx_eval_degrad
double lpx_eval_degrad(LPX *lp, int len, int ind[], double val[],
      int type, double rhs);
/* compute degradation of the objective function */

#define lpx_gomory_cut _glp_lpx_gomory_cut
int lpx_gomory_cut(LPX *lp, int len, int ind[], double val[],
      double work[]);
/* generate Gomory's mixed integer cut */

#define lpx_cover_cut _glp_lpx_cover_cut
int lpx_cover_cut(LPX *lp, int len, int ind[], double val[],
      double work[]);
/* generate mixed cover cut */

#define lpx_create_cog _glp_lpx_create_cog
void *lpx_create_cog(LPX *lp);
/* create the conflict graph */

#define lpx_add_cog_edge _glp_lpx_add_cog_edge
void lpx_add_cog_edge(void *cog, int i, int j);
/* add edge to the conflict graph */

#define lpx_clique_cut _glp_lpx_clique_cut
int lpx_clique_cut(LPX *lp, void *cog, int ind[], double val[]);
/* generate clique cut */

#define lpx_delete_cog _glp_lpx_delete_cog
void lpx_delete_cog(void *cog);
/* delete the conflict graph */

#define lpx_extract_prob _glp_lpx_extract_prob
LPX *lpx_extract_prob(void *mpl);
/* extract problem instance from MathProg model */

#define lpx_read_prob _glp_lpx_read_prob
LPX *lpx_read_prob(char *fname);
/* read problem data in GNU LP format */

#define lpx_write_prob _glp_lpx_write_prob
int lpx_write_prob(LPX *lp, char *fname);
/* write problem data in GNU LP format */

#endif

/* eof */
