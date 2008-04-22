/* glplpp.h (LP presolver) */

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

#ifndef _GLPLPP_H
#define _GLPLPP_H

#include "glpdmp.h"

typedef struct LPP LPP;
typedef struct LPPROW LPPROW;
typedef struct LPPCOL LPPCOL;
typedef struct LPPAIJ LPPAIJ;
typedef struct LPPTQE LPPTQE;
typedef struct LPPLFE LPPLFE;
typedef struct LPPLFX LPPLFX;

struct LPP
{     /* LP presolver workspace */
      /*--------------------------------------------------------------*/
      /* original problem segment */
      int orig_m;
      /* number of rows in the original problem */
      int orig_n;
      /* number of columns in the original problem */
      int orig_nnz;
      /* number of non-zeros in the original problem */
      int orig_dir;
      /* optimization direction for the original problem:
         LPX_MIN - minimization
         LPX_MAX - maximization */
      /*--------------------------------------------------------------*/
      /* transformed problem segment (always minimization) */
      int nrows;
      /* number of rows introduced into the transformed problem;
         this count increases by one each time when a new row is added
         to the transformed problem and never decreases; thus, actual
         number of rows may be less than nrows due to row deletions */
      int ncols;
      /* number of columns introduced into the transformed problem;
         this count increases by one each time when a new column is
         added to the transformed problem and never decreases; thus,
         actual number of columns may be less than ncols due to column
         deletions */
      DMP *row_pool;
      /* memory pool to hold LPPROW instances */
      DMP *col_pool;
      /* memory pool to hold LPPCOL instances */
      DMP *aij_pool;
      /* memory pool to hold LPPAIJ instances */
      LPPROW *row_ptr;
      /* initial pointer to the doubly linked list of rows */
      LPPCOL *col_ptr;
      /* initial pointer to the doubly linked list of columns */
      LPPROW *row_que;
      /* initial pointer to the queue of active rows */
      LPPCOL *col_que;
      /* initial pointer to the queue of active columns */
      double c0;
      /* constant term of the objective function */
      /*--------------------------------------------------------------*/
      /* transformation history segment */
      DMP *tqe_pool;
      /* memory pool to hold instances of data structures that describe
         transformations performed by the presolver routines */
      LPPTQE *tqe_list;
      /* pointer to the first transformation queue entry; each time
         when the presolver applies some transformation to the problem,
         the corresponding entry is built and added to the beginning of
         this linked list */
      /*--------------------------------------------------------------*/
      /* resultant problem segment */
      int m;
      /* number of rows in the resultant problem */
      int n;
      /* number of columns in the resultant problem */
      int nnz;
      /* number of non-zeros in the resultant problem */
      int *row_ref; /* int row_ref[1+m]; */
      /* row_ref[0] is not used;
         row_ref[i], i = 1, ..., m, is the reference number assigned to
         a row, which is i-th row of the resultant problem */
      int *col_ref; /* int col_ref[1+n]; */
      /* col_ref[0] is not used;
         col_ref[j], j = 1, ..., n, is the reference number assigned to
         a column, which is j-th column of the resultant problem */
      /*--------------------------------------------------------------*/
      /* recovered solution segment */
      int *row_stat; /* int row_stat[1+nrows]; */
      /* row_stat[0] is not used;
         row_stat[i], i = 1, ..., nrows, is the status of i-th row:
         0      - row is not recovered yet
         LPX_BS - inactive constraint
         LPX_NL - active constraint on lower bound
         LPX_NU - active constraint on upper bound
         LPX_NF - (can never be)
         LPX_NS - active equality constraint */
      double *row_prim; /* double row_prim[1+nrows]; */
      /* row_prim[0] is not used;
         row_prim[i] is a primal value of i-th auxiliary variable */
      double *row_dual; /* double row_dual[1+nrows]; */
      /* row_dual[0] is not used;
         row_dual[i] is a dual value of i-th auxiliary variable */
      int *col_stat; /* int col_stat[1+ncols]; */
      /* col_stat[0] is not used;
         col_stat[j], j = 1, ..., ncols, is the status of j-th column:
         0      - column is not recovered yet
         LPX_BS - basic variable
         LPX_NL - non-basic variable on lower bound
         LPX_NU - non-basic variable on upper bound
         LPX_NF - non-basic free variable
         LPX_NS - non-basic fixed variable */
      double *col_prim; /* double col_prim[1+ncols]; */
      /* col_prim[0] is not used;
         col_prim[j] is a primal value of j-th structural variable */
      double *col_dual; /* double col_dual[1+ncols]; */
      /* col_dual[0] is not used;
         col_dual[j] is a dual value of j-th structural variable */
};

struct LPPROW
{     /* row (constraint) */
      int i;
      /* reference number assigned to this row, 1 <= i <= nrows;
         rows of the original problem are assigned the numbers from 1
         to orig_m */
      double lb;
      /* lower bound or -DBL_MAX, if this row has no lower bound */
      double ub;
      /* upper bound or +DBL_MAX, if this row has no upper bound */
      LPPAIJ *ptr;
      /* initial pointer to the linked list of constraint coefficients
         for the row */
      int temp;
      /* auxiliary attribute used by some presolver routines */
      LPPROW *prev;
      /* pointer to the previous row in the linked list */
      LPPROW *next;
      /* pointer to the next row in the linked list */
      int q_flag;
      /* if this flag is set, the row is in the active queue */
      LPPROW *q_prev;
      /* pointer to the previous row in the active queue */
      LPPROW *q_next;
      /* pointer to the next row in the active queue */
};

struct LPPCOL
{     /* column (variable) */
      int j;
      /* reference number assigned to the column, 1 <= j <= ncols;
         columns of the original problem are assigned the numbers from
         1 to orig_n */
      double lb;
      /* lower bound or -DBL_MAX, if the column has no lower bound */
      double ub;
      /* upper bound or +DBL_MAX, if the column has no upper bound */
      double c;
      /* objective coefficient at the column */
      LPPAIJ *ptr;
      /* initial pointer to the linked list of constraint coefficients
         for the column */
      LPPCOL *prev;
      /* pointer to the previous column in the linked list */
      LPPCOL *next;
      /* pointer to the next column in the linked list */
      int q_flag;
      /* if this flag is set, the column is in the active queue */
      LPPCOL *q_prev;
      /* pointer to the previous column in the active queue */
      LPPCOL *q_next;
      /* pointer to the next column in the active queue */
};

struct LPPAIJ
{     /* element of the constraint matrix */
      LPPROW *row;
      /* pointer to the corresponding row */
      LPPCOL *col;
      /* pointer to the corresponding column */
      double val;
      /* numerical value of this element */
      LPPAIJ *r_prev;
      /* pointer to the previous element in the same row */
      LPPAIJ *r_next;
      /* pointer to the next element in the same row */
      LPPAIJ *c_prev;
      /* pointer to the previous element in the same column */
      LPPAIJ *c_next;
      /* pointer to the next element in the same column */
};

struct LPPTQE
{     /* transformation queue entry */
      int type;
      /* entry type: */
#define LPP_EMPTY_ROW   0x01
#define LPP_EMPTY_COL   0x02
#define LPP_FREE_ROW    0x03
#define LPP_FIXED_COL   0x04
#define LPP_ROW_SNGTON1 0x05
#define LPP_ROW_SNGTON2 0x06
#define LPP_COL_SNGTON1 0x07
#define LPP_COL_SNGTON2 0x08
#define LPP_FORCING_ROW 0x09
      void *info;
      /* pointer to specific part of this entry (depends on the entry
         type) */
      LPPTQE *next;
      /* pointer to an entry, which was created *before* this entry */
};

struct LPPLFE
{     /* linear form element */
      int ref;
      /* row/column reference number */
      double val;
      /* numerical value */
      LPPLFE *next;
      /* pointer to the next element */
};

struct LPPLFX
{     /* extended linear form element */
      int ref;
      /* row/column reference number */
      int flag;
      /* row/column flag */
      double val;
      /* numerical value */
      LPPLFX *next;
      /* pointer to the next element */
};

#define lpp_create_wksp       _glp_lpp_create_wksp
#define lpp_add_row           _glp_lpp_add_row
#define lpp_add_col           _glp_lpp_add_col
#define lpp_add_aij           _glp_lpp_add_aij
#define lpp_remove_row        _glp_lpp_remove_row
#define lpp_remove_col        _glp_lpp_remove_col
#define lpp_enque_row         _glp_lpp_enque_row
#define lpp_deque_row         _glp_lpp_deque_row
#define lpp_enque_col         _glp_lpp_enque_col
#define lpp_deque_col         _glp_lpp_deque_col
#define lpp_load_orig         _glp_lpp_load_orig
#define lpp_append_tqe        _glp_lpp_append_tqe
#define lpp_build_prob        _glp_lpp_build_prob
#define lpp_alloc_sol         _glp_lpp_alloc_sol
#define lpp_load_sol          _glp_lpp_load_sol
#define lpp_unload_sol        _glp_lpp_unload_sol
#define lpp_delete_wksp       _glp_lpp_delete_wksp

#define lpp_presolve          _glp_lpp_presolve
#define lpp_postsolve         _glp_lpp_postsolve

LPP *lpp_create_wksp(void);
/* create LP presolver workspace */

LPPROW *lpp_add_row(LPP *lpp, double lb, double ub);
/* add new row to the transformed problem */

LPPCOL *lpp_add_col(LPP *lpp, double lb, double ub, double c);
/* add new column to the transformed problem */

LPPAIJ *lpp_add_aij(LPP *lpp, LPPROW *row, LPPCOL *col, double val);
/* add new element to the constraint matrix */

void lpp_remove_row(LPP *lpp, LPPROW *row);
/* remove row from the transformed problem */

void lpp_remove_col(LPP *lpp, LPPCOL *col);
/* remove column from the transformed problem */

void lpp_enque_row(LPP *lpp, LPPROW *row);
/* place row in the active queue */

void lpp_deque_row(LPP *lpp, LPPROW *row);
/* remove row from the active queue */

void lpp_enque_col(LPP *lpp, LPPCOL *col);
/* place column in the active queue */

void lpp_deque_col(LPP *lpp, LPPCOL *col);
/* remove column from the active queue */

void lpp_load_orig(LPP *lpp, LPX *orig);
/* load original problem into LP presolver workspace */

void *lpp_append_tqe(LPP *lpp, int type, int size);
/* append new transformation queue entry */

LPX *lpp_build_prob(LPP *lpp);
/* build resultant problem */

void lpp_alloc_sol(LPP *lpp);
/* allocate recovered solution segment */

void lpp_load_sol(LPP *lpp, LPX *prob);
/* load basic solution into LP presolver workspace */

void lpp_unload_sol(LPP *lpp, LPX *orig);
/* unload basic solution from LP presolver workspace */

void lpp_delete_wksp(LPP *lpp);
/* delete LP presolver workspace */

int lpp_presolve(LPP *lpp);
/* LP presolve analysis */

void lpp_postsolve(LPP *lpp);
/* LP postsolve processing */

#endif

/* eof */
