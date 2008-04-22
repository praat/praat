/* glpspm.h (general sparse matrix) */

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

#ifndef _GLPSPM_H
#define _GLPSPM_H

#include "glpdmp.h"

typedef struct SPM SPM;
typedef struct SPME SPME;

struct SPM
{     /* general sparse matrix */
      int m;
      /* number of rows, m >= 0 */
      int n;
      /* number of columns, n >= 0 */
      DMP *pool;
      /* memory pool to store matrix elements */
      SPME **row; /* SPME *row[1+m]; */
      /* row[i], 1 <= i <= m, is a pointer to i-th row list */
      SPME **col; /* SPME *col[1+n]; */
      /* col[j], 1 <= j <= n, is a pointer to j-th column list */
};

struct SPME
{     /* sparse matrix element */
      int i;
      /* row number */
      int j;
      /* column number */
      double val;
      /* element value */
      SPME *r_prev;
      /* pointer to previous element in the same row */
      SPME *r_next;
      /* pointer to next element in the same row */
      SPME *c_prev;
      /* pointer to previous element in the same column */
      SPME *c_next;
      /* pointer to next element in the same column */
};

#define spm_create_mat _glp_spm_create_mat
SPM *spm_create_mat(int m, int n);
/* create general sparse matrix */

#define spm_new_elem _glp_spm_new_elem
SPME *spm_new_elem(SPM *A, int i, int j, double val);
/* add new element to sparse matrix */

#define spm_delete_mat _glp_spm_delete_mat
void spm_delete_mat(SPM *A);
/* delete general sparse matrix */

#define spm_test_mat_e _glp_spm_test_mat_e
SPM *spm_test_mat_e(int n, int c);
/* create test sparse matrix of E(n,c) class */

#define spm_test_mat_d _glp_spm_test_mat_d
SPM *spm_test_mat_d(int n, int c);
/* create test sparse matrix of D(n,c) class */

#define spm_show_mat _glp_spm_show_mat
int spm_show_mat(const SPM *A, const char *fname);
/* write sparse matrix pattern in BMP file format */

#define spm_read_hbm _glp_spm_read_hbm
SPM *spm_read_hbm(const char *fname);
/* read sparse matrix in Harwell-Boeing format */

#define spm_count_nnz _glp_spm_count_nnz
int spm_count_nnz(const SPM *A);
/* determine number of non-zeros in sparse matrix */

#define spm_drop_zeros _glp_spm_drop_zeros
int spm_drop_zeros(SPM *A, double eps);
/* remove zero elements from sparse matrix */

#define spm_read_mat _glp_spm_read_mat
SPM *spm_read_mat(const char *fname);
/* read sparse matrix from text file */

#define spm_write_mat _glp_spm_write_mat
int spm_write_mat(const SPM *A, const char *fname);
/* write sparse matrix to text file */

#endif

/* eof */
