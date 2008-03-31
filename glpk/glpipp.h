/* glpipp.h (MIP presolver) */

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

#ifndef _GLPIPP_H
#define _GLPIPP_H

#include "glpapi.h"
#include "glpdmp.h"

typedef struct IPP IPP;
typedef struct IPPROW IPPROW;
typedef struct IPPCOL IPPCOL;
typedef struct IPPAIJ IPPAIJ;
typedef struct IPPTQE IPPTQE;
typedef struct IPPLFE IPPLFE;

struct IPP
{     /* MIP presolver workspace */
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
      int ncols;
      /* number of columns introduced into the transformed problem;
         this count increases by one whenever a new column is included
         into the transformed problem and never decreases; thus, actual
         number of columns may be less than ncols due to deletions of
         columns */
      DMP *row_pool;
      /* memory pool to hold IPPROW instances */
      DMP *col_pool;
      /* memory pool to hold IPPCOL instances */
      DMP *aij_pool;
      /* memory pool to hold IPPAIJ instances */
      IPPROW *row_ptr;
      /* initial pointer to the linked list of rows */
      IPPCOL *col_ptr;
      /* initial pointer to the linked list of columns */
      IPPROW *row_que;
      /* initial pointer to the queue of active rows */
      IPPCOL *col_que;
      /* initial pointer to the queue of active columns */
      double c0;
      /* constant term of the objective function */
      /*--------------------------------------------------------------*/
      /* transformation history segment */
      DMP *tqe_pool;
      /* memory pool to hold instances of data structures to describe
         transformations performed by the presolver routines */
      IPPTQE *tqe_list;
      /* pointer to the first transformation queue entry; whenever the
         presolver applies some transformation, the corresponding entry
         is created and added to the beginning of this linked list */
      /*--------------------------------------------------------------*/
      /* recovered solution segment */
      int *col_stat; /* int col_stat[1+ncols]; */
      /* col_stat[0] is not used;
         col_stat[j], j = 1,...,ncols, is the status of j-th column:
         0 - column is not recovered yet
         1 - column has been recovered */
      double *col_mipx; /* double col_mipx[1+ncols]; */
      /* col_prim[0] is not used;
         col_prim[j] is a numeric value of j-th structural variable */
};

struct IPPROW
{     /* row (constraint) */
      double lb;
      /* lower bound; -DBL_MAX means the row has no lower bound */
      double ub;
      /* upper bound; +DBL_MAX means the row has no upper bound */
      IPPAIJ *ptr;
      /* initial pointer to the linked list of constraint coefficients
         for the row */
      int temp;
      /* auxiliary attribute used by some presolver routines */
      IPPROW *prev;
      /* pointer to the previous row in the linked list */
      IPPROW *next;
      /* pointer to the next row in the linked list */
      int q_flag;
      /* if this flag is set, the row is in the active queue */
      IPPROW *q_prev;
      /* pointer to the previous row in the active queue */
      IPPROW *q_next;
      /* pointer to the next row in the active queue */
};

struct IPPCOL
{     /* column (variable) */
      int j;
      /* reference number assigned to this column, 1 <= j <= ncols;
         columns of the original problem are assigned the numbers from
         1 to orig_n */
      int i_flag;
      /* integrality flag */
      double lb;
      /* lower bound; -DBL_MAX means the column has no lower bound */
      double ub;
      /* upper bound; +DBL_MAX means the column has no upper bound */
      double c;
      /* objective coefficient at the column */
      IPPAIJ *ptr;
      /* initial pointer to the linked list of constraint coefficients
         for the column */
      int temp;
      /* auxiliary attribute used by some presolver routines */
      IPPCOL *prev;
      /* pointer to the previous column in the linked list */
      IPPCOL *next;
      /* pointer to the next column in the linked list */
      int q_flag;
      /* if this flag is set, the column is in the active queue */
      IPPCOL *q_prev;
      /* pointer to the previous column in the active queue */
      IPPCOL *q_next;
      /* pointer to the next column in the active queue */
};

struct IPPAIJ
{     /* element of the constraint matrix */
      IPPROW *row;
      /* pointer to the corresponding row */
      IPPCOL *col;
      /* pointer to the corresponding column */
      double val;
      /* numerical value of this element */
      IPPAIJ *r_prev;
      /* pointer to the previous element in the same row */
      IPPAIJ *r_next;
      /* pointer to the next element in the same row */
      IPPAIJ *c_prev;
      /* pointer to the previous element in the same column */
      IPPAIJ *c_next;
      /* pointer to the next element in the same column */
};

struct IPPTQE
{     /* transformation queue entry */
      int type;
      /* entry type: */
#define IPP_FIXED_COL   0x01  /* fixed column */
#define IPP_SHIFT_COL   0x02  /* shifted column */
#define IPP_NONBIN_COL  0x03  /* non-binary column */
      void *info;
      /* pointer to specific part of this entry (depends on the entry
         type) */
      IPPTQE *next;
      /* pointer to an entry, which was created *before* this entry */
};

struct IPPLFE
{     /* linear form element */
      int ref;
      /* column reference number */
      double val;
      /* numeric value */
      IPPLFE *next;
      /* pointer to the next element */
};

#define ipp_create_wksp       _glp_ipp_create_wksp
#define ipp_add_row           _glp_ipp_add_row
#define ipp_add_col           _glp_ipp_add_col
#define ipp_add_aij           _glp_ipp_add_aij
#define ipp_remove_row        _glp_ipp_remove_row
#define ipp_remove_col        _glp_ipp_remove_col
#define ipp_enque_row         _glp_ipp_enque_row
#define ipp_deque_row         _glp_ipp_deque_row
#define ipp_enque_col         _glp_ipp_enque_col
#define ipp_deque_col         _glp_ipp_deque_col
#define ipp_append_tqe        _glp_ipp_append_tqe
#define ipp_load_orig         _glp_ipp_load_orig
#define ipp_tight_bnds        _glp_ipp_tight_bnds
#define ipp_build_prob        _glp_ipp_build_prob
#define ipp_load_sol          _glp_ipp_load_sol
#define ipp_unload_sol        _glp_ipp_unload_sol
#define ipp_delete_wksp       _glp_ipp_delete_wksp

#define ipp_free_row          _glp_ipp_free_row
#define ipp_fixed_col         _glp_ipp_fixed_col
#define ipp_fixed_col_r       _glp_ipp_fixed_col_r
#define ipp_empty_row         _glp_ipp_empty_row
#define ipp_empty_col         _glp_ipp_empty_col
#define ipp_row_sing          _glp_ipp_row_sing
#define ipp_analyze_row       _glp_ipp_analyze_row
#define ipp_analyze_col       _glp_ipp_analyze_col
#define ipp_basic_tech        _glp_ipp_basic_tech
#define ipp_reduce_bnds       _glp_ipp_reduce_bnds
#define ipp_shift_col         _glp_ipp_shift_col
#define ipp_shift_col_r       _glp_ipp_shift_col_r
#define ipp_nonbin_col        _glp_ipp_nonbin_col
#define ipp_nonbin_col_r      _glp_ipp_nonbin_col_r
#define ipp_reduce_coef       _glp_ipp_reduce_coef
#define ipp_binarize          _glp_ipp_binarize
#define ipp_reduction         _glp_ipp_reduction
#define ipp_postsolve         _glp_ipp_postsolve

IPP *ipp_create_wksp(void);
/* create MIP presolver workspace */

IPPROW *ipp_add_row(IPP *ipp, double lb, double ub);
/* add new row to the transformed problem */

IPPCOL *ipp_add_col(IPP *ipp, int i_flag, double lb, double ub,
      double c);
/* add new column to the transformed problem */

IPPAIJ *ipp_add_aij(IPP *ipp, IPPROW *row, IPPCOL *col, double val);
/* add new element to the constraint matrix */

void ipp_remove_row(IPP *ipp, IPPROW *row);
/* remove row from the transformed problem */

void ipp_remove_col(IPP *ipp, IPPCOL *col);
/* remove column from the transformed problem */

void ipp_enque_row(IPP *ipp, IPPROW *row);
/* place row in the active queue */

void ipp_deque_row(IPP *ipp, IPPROW *row);
/* remove row from the active queue */

void ipp_enque_col(IPP *ipp, IPPCOL *col);
/* place column in the active queue */

void ipp_deque_col(IPP *ipp, IPPCOL *col);
/* remove column from the active queue */

void *ipp_append_tqe(IPP *ipp, int type, int size);
/* append new transformation queue entry */

void ipp_load_orig(IPP *ipp, LPX *orig);
/* load original problem into MIP presolver workspace */

int ipp_tight_bnds(IPP *ipp, IPPCOL *col, double lb, double ub);
/* tight current column bounds using implied bounds */

LPX *ipp_build_prob(IPP *ipp);
/* build resultant problem */

void ipp_load_sol(IPP *ipp, LPX *prob);
/* load solution into MIP presolver workspace */

void ipp_unload_sol(IPP *ipp, LPX *orig, int i_stat);
/* unload solution from MIP presolver workspace */

void ipp_delete_wksp(IPP *ipp);
/* delete MIP presolver workspace */

void ipp_free_row(IPP *ipp, IPPROW *row);
/* process free row */

void ipp_fixed_col(IPP *ipp, IPPCOL *col);
/* process fixed column */

void ipp_fixed_col_r(IPP *ipp, void *info);
/* recover fixed column */

int ipp_empty_row(IPP *ipp, IPPROW *row);
/* process empty row */

int ipp_empty_col(IPP *ipp, IPPCOL *col);
/* process empty column */

int ipp_row_sing(IPP *ipp, IPPROW *row);
/* process row singleton */

int ipp_analyze_row(IPP *ipp, IPPROW *row);
/* general row analysis */

int ipp_analyze_col(IPP *ipp, IPPCOL *col);
/* general column analysis */

int ipp_basic_tech(IPP *ipp);
/* basic MIP presolve analysis */

int ipp_reduce_bnds(IPP *ipp);
/* reduce column bounds */

void ipp_shift_col(IPP *ipp, IPPCOL *col);
/* process shifted column */

void ipp_shift_col_r(IPP *ipp, void *info);
/* recover shifted column */

int ipp_nonbin_col(IPP *ipp, IPPCOL *col);
/* process non-binary column */

void ipp_nonbin_col_r(IPP *ipp, void *info);
/* recover non-binary column */

void ipp_reduce_coef(IPP *ipp);
/* reduce constraint coefficients */

void ipp_binarize(IPP *ipp);
/* replace general integer variables by binary ones */

void ipp_reduction(IPP *ipp);
/* perform coefficient reduction */

void ipp_postsolve(IPP *ipp);
/* MIP postsolve processing */

#endif

/* eof */
