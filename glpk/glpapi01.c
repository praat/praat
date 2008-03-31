/* glpapi01.c (problem creating and modifying routines) */

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

#include "glpios.h"
#define xfault xerror

/* CAUTION: DO NOT CHANGE THE LIMITS BELOW */

#define M_MAX 100000000 /* = 100*10^6 */
/* maximal number of rows in the problem object */

#define N_MAX 100000000 /* = 100*10^6 */
/* maximal number of columns in the problem object */

#define NNZ_MAX 500000000 /* = 500*10^6 */
/* maximal number of constraint coefficients in the problem object */

/***********************************************************************
*  NAME
*
*  glp_create_prob - create problem object
*
*  SYNOPSIS
*
*  glp_prob *glp_create_prob(void);
*
*  DESCRIPTION
*
*  The routine glp_create_prob creates a new problem object, which is
*  initially "empty", i.e. has no rows and columns.
*
*  RETURNS
*
*  The routine returns a pointer to the object created, which should be
*  used in any subsequent operations on this object. */

glp_prob *glp_create_prob(void)
{     glp_prob *lp;
      lp = xmalloc(sizeof(glp_prob));
      lp->pool = dmp_create_pool();
      lp->cps = xmalloc(sizeof(struct LPXCPS));
      lpx_reset_parms(lp);
#if 0
      lp->str_buf = xmalloc(255+1);
#endif
      lp->tree = NULL;
      /* LP/MIP data */
      lp->name = NULL;
      lp->obj = NULL;
      lp->dir = GLP_MIN;
      lp->c0 = 0.0;
      lp->m_max = 100;
      lp->n_max = 200;
      lp->m = lp->n = 0;
      lp->nnz = 0;
      lp->row = xcalloc(1+lp->m_max, sizeof(GLPROW *));
      lp->col = xcalloc(1+lp->n_max, sizeof(GLPCOL *));
      lp->r_tree = lp->c_tree = NULL;
      /* basis factorization */
      lp->valid = 0;
      lp->bhead = xcalloc(1+lp->m_max, sizeof(int));
      lp->bfcp = NULL;
      lp->bfd = NULL;
      /* basic solution (LP) */
      lp->pbs_stat = lp->dbs_stat = GLP_UNDEF;
      lp->obj_val = 0.0;
      lp->it_cnt = 0;
      lp->some = 0;
      /* interior-point solution (LP) */
      lp->ipt_stat = GLP_UNDEF;
      lp->ipt_obj = 0.0;
      /* integer solution (MIP) */
      lp->mip_stat = GLP_UNDEF;
      lp->mip_obj = 0.0;
      return lp;
}

/***********************************************************************
*  NAME
*
*  glp_set_prob_name - assign (change) problem name
*
*  SYNOPSIS
*
*  void glp_set_prob_name(glp_prob *lp, const char *name);
*
*  DESCRIPTION
*
*  The routine glp_set_prob_name assigns a given symbolic name (1 up to
*  255 characters) to the specified problem object.
*
*  If the parameter name is NULL or empty string, the routine erases an
*  existing symbolic name of the problem object. */

void glp_set_prob_name(glp_prob *lp, const char *name)
{     if (lp->name != NULL)
      {  dmp_free_atom(lp->pool, lp->name, strlen(lp->name)+1);
         lp->name = NULL;
      }
      if (!(name == NULL || name[0] == '\0'))
      {  if (strlen(name) > 255)
            xfault("glp_set_prob_name: problem name too long\n");
         lp->name = dmp_get_atom(lp->pool, strlen(name)+1);
         strcpy(lp->name, name);
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_obj_name - assign (change) objective function name
*
*  SYNOPSIS
*
*  void glp_set_obj_name(glp_prob *lp, const char *name);
*
*  DESCRIPTION
*
*  The routine glp_set_obj_name assigns a given symbolic name (1 up to
*  255 characters) to the objective function of the specified problem
*  object.
*
*  If the parameter name is NULL or empty string, the routine erases an
*  existing name of the objective function. */

void glp_set_obj_name(glp_prob *lp, const char *name)
{     if (lp->obj != NULL)
      {  dmp_free_atom(lp->pool, lp->obj, strlen(lp->obj)+1);
         lp->obj = NULL;
      }
      if (!(name == NULL || name[0] == '\0'))
      {  if (strlen(name) > 255)
            xfault("glp_set_obj_name: objective name too long\n");
         lp->obj = dmp_get_atom(lp->pool, strlen(name)+1);
         strcpy(lp->obj, name);
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_obj_dir - set (change) optimization direction flag
*
*  SYNOPSIS
*
*  void glp_set_obj_dir(glp_prob *lp, int dir);
*
*  DESCRIPTION
*
*  The routine glp_set_obj_dir sets (changes) optimization direction
*  flag (i.e. "sense" of the objective function) as specified by the
*  parameter dir:
*
*  GLP_MIN - minimization;
*  GLP_MAX - maximization. */

void glp_set_obj_dir(glp_prob *lp, int dir)
{     if (!(dir == GLP_MIN || dir == GLP_MAX))
         xfault("glp_set_obj_dir: dir = %d; invalid direction flag\n",
            dir);
      lp->dir = dir;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_add_rows - add new rows to problem object
*
*  SYNOPSIS
*
*  int glp_add_rows(glp_prob *lp, int nrs);
*
*  DESCRIPTION
*
*  The routine glp_add_rows adds nrs rows (constraints) to the specified
*  problem object. New rows are always added to the end of the row list,
*  so the ordinal numbers of existing rows remain unchanged.
*
*  Being added each new row is initially free (unbounded) and has empty
*  list of the constraint coefficients.
*
*  RETURNS
*
*  The routine glp_add_rows returns the ordinal number of the first new
*  row added to the problem object. */

int glp_add_rows(glp_prob *lp, int nrs)
{     glp_tree *tree = lp->tree;
      GLPROW *row;
      int m_new, i;
      /* determine new number of rows */
      if (nrs < 1)
         xfault("glp_add_rows: nrs = %d; invalid number of rows\n",
            nrs);
      if (nrs > M_MAX - lp->m)
         xfault("glp_add_rows: nrs = %d; too many rows\n", nrs);
      m_new = lp->m + nrs;
      /* increase the room, if necessary */
      if (lp->m_max < m_new)
      {  GLPROW **save = lp->row;
         while (lp->m_max < m_new)
         {  lp->m_max += lp->m_max;
            xassert(lp->m_max > 0);
         }
         lp->row = xcalloc(1+lp->m_max, sizeof(GLPROW *));
         memcpy(&lp->row[1], &save[1], lp->m * sizeof(GLPROW *));
         xfree(save);
         /* do not forget about the basis header */
         xfree(lp->bhead);
         lp->bhead = xcalloc(1+lp->m_max, sizeof(int));
      }
      /* add new rows to the end of the row list */
      for (i = lp->m+1; i <= m_new; i++)
      {  /* create row descriptor */
         lp->row[i] = row = dmp_get_atom(lp->pool, sizeof(GLPROW));
         row->i = i;
         row->name = NULL;
         row->node = NULL;
         row->type = GLP_FR;
         row->lb = row->ub = 0.0;
         row->ptr = NULL;
         row->rii = 1.0;
         row->stat = GLP_BS;
#if 0
         row->bind = -1;
#else
         row->bind = 0;
#endif
         row->prim = row->dual = 0.0;
         row->pval = row->dval = 0.0;
         row->mipx = 0.0;
      }
      /* set new number of rows */
      lp->m = m_new;
      /* invalidate the basis factorization */
      lp->valid = 0;
#if 1
      if (tree != NULL && tree->reason != 0) tree->reopt = 1;
#endif
      /* return the ordinal number of the first row added */
      return m_new - nrs + 1;
}

/***********************************************************************
*  NAME
*
*  glp_add_cols - add new columns to problem object
*
*  SYNOPSIS
*
*  int glp_add_cols(glp_prob *lp, int ncs);
*
*  DESCRIPTION
*
*  The routine glp_add_cols adds ncs columns (structural variables) to
*  the specified problem object. New columns are always added to the end
*  of the column list, so the ordinal numbers of existing columns remain
*  unchanged.
*
*  Being added each new column is initially fixed at zero and has empty
*  list of the constraint coefficients.
*
*  RETURNS
*
*  The routine glp_add_cols returns the ordinal number of the first new
*  column added to the problem object. */

int glp_add_cols(glp_prob *lp, int ncs)
{     GLPCOL *col;
      int n_new, j;
      /* determine new number of columns */
      if (ncs < 1)
         xfault("glp_add_cols: ncs = %d; invalid number of columns\n",
            ncs);
      if (ncs > N_MAX - lp->n)
         xfault("glp_add_cols: ncs = %d; too many columns\n", ncs);
      n_new = lp->n + ncs;
      /* increase the room, if necessary */
      if (lp->n_max < n_new)
      {  GLPCOL **save = lp->col;
         while (lp->n_max < n_new)
         {  lp->n_max += lp->n_max;
            xassert(lp->n_max > 0);
         }
         lp->col = xcalloc(1+lp->n_max, sizeof(GLPCOL *));
         memcpy(&lp->col[1], &save[1], lp->n * sizeof(GLPCOL *));
         xfree(save);
      }
      /* add new columns to the end of the column list */
      for (j = lp->n+1; j <= n_new; j++)
      {  /* create column descriptor */
         lp->col[j] = col = dmp_get_atom(lp->pool, sizeof(GLPCOL));
         col->j = j;
         col->name = NULL;
         col->node = NULL;
         col->kind = GLP_CV;
         col->type = GLP_FX;
         col->lb = col->ub = 0.0;
         col->coef = 0.0;
         col->ptr = NULL;
         col->sjj = 1.0;
         col->stat = GLP_NS;
#if 0
         col->bind = -1;
#else
         col->bind = 0; /* the basis may remain valid */
#endif
         col->prim = col->dual = 0.0;
         col->pval = col->dval = 0.0;
         col->mipx = 0.0;
      }
      /* set new number of columns */
      lp->n = n_new;
      /* return the ordinal number of the first column added */
      return n_new - ncs + 1;
}

/***********************************************************************
*  NAME
*
*  glp_set_row_name - assign (change) row name
*
*  SYNOPSIS
*
*  void glp_set_row_name(glp_prob *lp, int i, const char *name);
*
*  DESCRIPTION
*
*  The routine glp_set_row_name assigns a given symbolic name (1 up to
*  255 characters) to i-th row (auxiliary variable) of the specified
*  problem object.
*
*  If the parameter name is NULL or empty string, the routine erases an
*  existing name of i-th row. */

void glp_set_row_name(glp_prob *lp, int i, const char *name)
{     GLPROW *row;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_set_row_name: i = %d; row number out of range\n",
            i);
      row = lp->row[i];
      if (row->name != NULL)
      {  if (row->node != NULL)
         {  xassert(lp->r_tree != NULL);
            avl_delete_node(lp->r_tree, row->node);
            row->node = NULL;
         }
         dmp_free_atom(lp->pool, row->name, strlen(row->name)+1);
         row->name = NULL;
      }
      if (!(name == NULL || name[0] == '\0'))
      {  if (strlen(name) > 255)
            xfault("glp_set_row_name: i = %d; row name too long\n", i);
         row->name = dmp_get_atom(lp->pool, strlen(name)+1);
         strcpy(row->name, name);
         if (lp->r_tree != NULL)
         {  xassert(row->node == NULL);
            row->node = avl_insert_node(lp->r_tree, row->name);
            avl_set_node_link(row->node, row);
         }
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_col_name - assign (change) column name
*
*  SYNOPSIS
*
*  void glp_set_col_name(glp_prob *lp, int j, const char *name);
*
*  DESCRIPTION
*
*  The routine glp_set_col_name assigns a given symbolic name (1 up to
*  255 characters) to j-th column (structural variable) of the specified
*  problem object.
*
*  If the parameter name is NULL or empty string, the routine erases an
*  existing name of j-th column. */

void glp_set_col_name(glp_prob *lp, int j, const char *name)
{     GLPCOL *col;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_set_col_name: j = %d; column number out of range\n"
            , j);
      col = lp->col[j];
      if (col->name != NULL)
      {  if (col->node != NULL)
         {  xassert(lp->c_tree != NULL);
            avl_delete_node(lp->c_tree, col->node);
            col->node = NULL;
         }
         dmp_free_atom(lp->pool, col->name, strlen(col->name)+1);
         col->name = NULL;
      }
      if (!(name == NULL || name[0] == '\0'))
      {  if (strlen(name) > 255)
            xfault("glp_set_col_name: j = %d; column name too long\n",
               j);
         col->name = dmp_get_atom(lp->pool, strlen(name)+1);
         strcpy(col->name, name);
         if (lp->c_tree != NULL && col->name != NULL)
         {  xassert(col->node == NULL);
            col->node = avl_insert_node(lp->c_tree, col->name);
            avl_set_node_link(col->node, col);
         }
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_row_bnds - set (change) row bounds
*
*  SYNOPSIS
*
*  void glp_set_row_bnds(glp_prob *lp, int i, int type, double lb,
*     double ub);
*
*  DESCRIPTION
*
*  The routine glp_set_row_bnds sets (changes) the type and bounds of
*  i-th row (auxiliary variable) of the specified problem object.
*
*  Parameters type, lb, and ub specify the type, lower bound, and upper
*  bound, respectively, as follows:
*
*     Type           Bounds        Comments
*     ------------------------------------------------------
*     GLP_FR   -inf <  x <  +inf   Free variable
*     GLP_LO     lb <= x <  +inf   Variable with lower bound
*     GLP_UP   -inf <  x <=  ub    Variable with upper bound
*     GLP_DB     lb <= x <=  ub    Double-bounded variable
*     GLP_FX           x  =  lb    Fixed variable
*
*  where x is the auxiliary variable associated with i-th row.
*
*  If the row has no lower bound, the parameter lb is ignored. If the
*  row has no upper bound, the parameter ub is ignored. If the row is
*  an equality constraint (i.e. the corresponding auxiliary variable is
*  of fixed type), only the parameter lb is used while the parameter ub
*  is ignored. */

void glp_set_row_bnds(glp_prob *lp, int i, int type, double lb,
      double ub)
{     GLPROW *row;
      if (!(1 <= i && i <= lp->m))
         xfault("glp_set_row_bnds: i = %d; row number out of range\n",
            i);
      row = lp->row[i];
      row->type = type;
      switch (type)
      {  case GLP_FR:
            row->lb = row->ub = 0.0;
            if (row->stat != GLP_BS) row->stat = GLP_NF;
            break;
         case GLP_LO:
            row->lb = lb, row->ub = 0.0;
            if (row->stat != GLP_BS) row->stat = GLP_NL;
            break;
         case GLP_UP:
            row->lb = 0.0, row->ub = ub;
            if (row->stat != GLP_BS) row->stat = GLP_NU;
            break;
         case GLP_DB:
            row->lb = lb, row->ub = ub;
            if (!(row->stat == GLP_BS ||
                  row->stat == GLP_NL || row->stat == GLP_NU))
               row->stat = (fabs(lb) <= fabs(ub) ? GLP_NL : GLP_NU);
            break;
         case GLP_FX:
            row->lb = row->ub = lb;
            if (row->stat != GLP_BS) row->stat = GLP_NS;
            break;
         default:
            xfault("glp_set_row_bnds: i = %d; type = %d; invalid row ty"
               "pe\n", i, type);
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_col_bnds - set (change) column bounds
*
*  SYNOPSIS
*
*  void glp_set_col_bnds(glp_prob *lp, int j, int type, double lb,
*     double ub);
*
*  DESCRIPTION
*
*  The routine glp_set_col_bnds sets (changes) the type and bounds of
*  j-th column (structural variable) of the specified problem object.
*
*  Parameters type, lb, and ub specify the type, lower bound, and upper
*  bound, respectively, as follows:
*
*     Type           Bounds        Comments
*     ------------------------------------------------------
*     GLP_FR   -inf <  x <  +inf   Free variable
*     GLP_LO     lb <= x <  +inf   Variable with lower bound
*     GLP_UP   -inf <  x <=  ub    Variable with upper bound
*     GLP_DB     lb <= x <=  ub    Double-bounded variable
*     GLP_FX           x  =  lb    Fixed variable
*
*  where x is the structural variable associated with j-th column.
*
*  If the column has no lower bound, the parameter lb is ignored. If the
*  column has no upper bound, the parameter ub is ignored. If the column
*  is of fixed type, only the parameter lb is used while the parameter
*  ub is ignored. */

void glp_set_col_bnds(glp_prob *lp, int j, int type, double lb,
      double ub)
{     GLPCOL *col;
      if (!(1 <= j && j <= lp->n))
         xfault("glp_set_col_bnds: j = %d; column number out of range\n"
            , j);
      col = lp->col[j];
      col->type = type;
      switch (type)
      {  case GLP_FR:
            col->lb = col->ub = 0.0;
            if (col->stat != GLP_BS) col->stat = GLP_NF;
            break;
         case GLP_LO:
            col->lb = lb, col->ub = 0.0;
            if (col->stat != GLP_BS) col->stat = GLP_NL;
            break;
         case GLP_UP:
            col->lb = 0.0, col->ub = ub;
            if (col->stat != GLP_BS) col->stat = GLP_NU;
            break;
         case GLP_DB:
            col->lb = lb, col->ub = ub;
            if (!(col->stat == GLP_BS ||
                  col->stat == GLP_NL || col->stat == GLP_NU))
               col->stat = (fabs(lb) <= fabs(ub) ? GLP_NL : GLP_NU);
            break;
         case GLP_FX:
            col->lb = col->ub = lb;
            if (col->stat != GLP_BS) col->stat = GLP_NS;
            break;
         default:
            xfault("glp_set_col_bnds: j = %d; type = %d; invalid column"
               " type\n", j, type);
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_obj_coef - set (change) obj. coefficient or constant term
*
*  SYNOPSIS
*
*  void glp_set_obj_coef(glp_prob *lp, int j, double coef);
*
*  DESCRIPTION
*
*  The routine glp_set_obj_coef sets (changes) objective coefficient at
*  j-th column (structural variable) of the specified problem object.
*
*  If the parameter j is 0, the routine sets (changes) the constant term
*  ("shift") of the objective function. */

void glp_set_obj_coef(glp_prob *lp, int j, double coef)
{     if (!(0 <= j && j <= lp->n))
         xfault("glp_set_obj_coef: j = %d; column number out of range\n"
            , j);
      if (j == 0)
         lp->c0 = coef;
      else
         lp->col[j]->coef = coef;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_mat_row - set (replace) row of the constraint matrix
*
*  SYNOPSIS
*
*  void glp_set_mat_row(glp_prob *lp, int i, int len, const int ind[],
*     const double val[]);
*
*  DESCRIPTION
*
*  The routine glp_set_mat_row stores (replaces) the contents of i-th
*  row of the constraint matrix of the specified problem object.
*
*  Column indices and numeric values of new row elements must be placed
*  in locations ind[1], ..., ind[len] and val[1], ..., val[len], where
*  0 <= len <= n is the new length of i-th row, n is the current number
*  of columns in the problem object. Elements with identical column
*  indices are not allowed. Zero elements are allowed, but they are not
*  stored in the constraint matrix.
*
*  If the parameter len is zero, the parameters ind and/or val can be
*  specified as NULL. */

void glp_set_mat_row(glp_prob *lp, int i, int len, const int ind[],
      const double val[])
{     GLPROW *row;
      GLPCOL *col;
      GLPAIJ *aij, *next;
      int j, k;
      /* obtain pointer to i-th row */
      if (!(1 <= i && i <= lp->m))
         xfault("glp_set_mat_row: i = %d; row number out of range\n",
            i);
      row = lp->row[i];
      /* remove all existing elements from i-th row */
      while (row->ptr != NULL)
      {  /* take next element in the row */
         aij = row->ptr;
         /* remove the element from the row list */
         row->ptr = aij->r_next;
         /* obtain pointer to corresponding column */
         col = aij->col;
         /* remove the element from the column list */
         if (aij->c_prev == NULL)
            col->ptr = aij->c_next;
         else
            aij->c_prev->c_next = aij->c_next;
         if (aij->c_next == NULL)
            ;
         else
            aij->c_next->c_prev = aij->c_prev;
         /* return the element to the memory pool */
         dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
         /* if the corresponding column is basic, invalidate the basis
            factorization */
         if (col->stat == GLP_BS) lp->valid = 0;
      }
      /* store new contents of i-th row */
      if (!(0 <= len && len <= lp->n))
         xfault("glp_set_mat_row: i = %d; len = %d; invalid row length "
            "\n", i, len);
      if (len > NNZ_MAX - lp->nnz)
         xfault("glp_set_mat_row: i = %d; len = %d; too many constraint"
            " coefficients\n", i, len);
      for (k = 1; k <= len; k++)
      {  /* take number j of corresponding column */
         j = ind[k];
         /* obtain pointer to j-th column */
         if (!(1 <= j && j <= lp->n))
            xfault("glp_set_mat_row: i = %d; ind[%d] = %d; column index"
               " out of range\n", i, k, j);
         col = lp->col[j];
         /* if there is element with the same column index, it can only
            be found in the beginning of j-th column list */
         if (col->ptr != NULL && col->ptr->row->i == i)
            xfault("glp_set_mat_row: i = %d; ind[%d] = %d; duplicate co"
               "lumn indices not allowed\n", i, k, j);
         /* create new element */
         aij = dmp_get_atom(lp->pool, sizeof(GLPAIJ)), lp->nnz++;
         aij->row = row;
         aij->col = col;
         aij->val = val[k];
         /* add the new element to the beginning of i-th row and j-th
            column lists */
         aij->r_prev = NULL;
         aij->r_next = row->ptr;
         aij->c_prev = NULL;
         aij->c_next = col->ptr;
         if (aij->r_next != NULL) aij->r_next->r_prev = aij;
         if (aij->c_next != NULL) aij->c_next->c_prev = aij;
         row->ptr = col->ptr = aij;
         /* if the corresponding column is basic, invalidate the basis
            factorization */
         if (col->stat == GLP_BS && aij->val != 0.0) lp->valid = 0;
      }
      /* remove zero elements from i-th row */
      for (aij = row->ptr; aij != NULL; aij = next)
      {  next = aij->r_next;
         if (aij->val == 0.0)
         {  /* remove the element from the row list */
            if (aij->r_prev == NULL)
               row->ptr = next;
            else
               aij->r_prev->r_next = next;
            if (next == NULL)
               ;
            else
               next->r_prev = aij->r_prev;
            /* remove the element from the column list */
            xassert(aij->c_prev == NULL);
            aij->col->ptr = aij->c_next;
            if (aij->c_next != NULL) aij->c_next->c_prev = NULL;
            /* return the element to the memory pool */
            dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
         }
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_set_mat_col - set (replace) column of the constraint matrix
*
*  SYNOPSIS
*
*  void glp_set_mat_col(glp_prob *lp, int j, int len, const int ind[],
*     const double val[]);
*
*  DESCRIPTION
*
*  The routine glp_set_mat_col stores (replaces) the contents of j-th
*  column of the constraint matrix of the specified problem object.
*
*  Row indices and numeric values of new column elements must be placed
*  in locations ind[1], ..., ind[len] and val[1], ..., val[len], where
*  0 <= len <= m is the new length of j-th column, m is the current
*  number of rows in the problem object. Elements with identical column
*  indices are not allowed. Zero elements are allowed, but they are not
*  stored in the constraint matrix.
*
*  If the parameter len is zero, the parameters ind and/or val can be
*  specified as NULL. */

void glp_set_mat_col(glp_prob *lp, int j, int len, const int ind[],
      const double val[])
{     GLPROW *row;
      GLPCOL *col;
      GLPAIJ *aij, *next;
      int i, k;
      /* obtain pointer to j-th column */
      if (!(1 <= j && j <= lp->n))
         xfault("glp_set_mat_col: j = %d; column number out of range\n",
            j);
      col = lp->col[j];
      /* remove all existing elements from j-th column */
      while (col->ptr != NULL)
      {  /* take next element in the column */
         aij = col->ptr;
         /* remove the element from the column list */
         col->ptr = aij->c_next;
         /* obtain pointer to corresponding row */
         row = aij->row;
         /* remove the element from the row list */
         if (aij->r_prev == NULL)
            row->ptr = aij->r_next;
         else
            aij->r_prev->r_next = aij->r_next;
         if (aij->r_next == NULL)
            ;
         else
            aij->r_next->r_prev = aij->r_prev;
         /* return the element to the memory pool */
         dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
      }
      /* store new contents of j-th column */
      if (!(0 <= len && len <= lp->m))
         xfault("glp_set_mat_col: j = %d; len = %d; invalid column leng"
            "th\n", j, len);
      if (len > NNZ_MAX - lp->nnz)
         xfault("glp_set_mat_col: j = %d; len = %d; too many constraint"
            " coefficients\n", j, len);
      for (k = 1; k <= len; k++)
      {  /* take number i of corresponding row */
         i = ind[k];
         /* obtain pointer to i-th row */
         if (!(1 <= i && i <= lp->m))
            xfault("glp_set_mat_col: j = %d; ind[%d] = %d; row index ou"
               "t of range\n", j, k, i);
         row = lp->row[i];
         /* if there is element with the same row index, it can only be
            found in the beginning of i-th row list */
         if (row->ptr != NULL && row->ptr->col->j == j)
            xfault("glp_set_mat_col: j = %d; ind[%d] = %d; duplicate ro"
               "w indices not allowed\n", j, k, i);
         /* create new element */
         aij = dmp_get_atom(lp->pool, sizeof(GLPAIJ)), lp->nnz++;
         aij->row = row;
         aij->col = col;
         aij->val = val[k];
         /* add the new element to the beginning of i-th row and j-th
            column lists */
         aij->r_prev = NULL;
         aij->r_next = row->ptr;
         aij->c_prev = NULL;
         aij->c_next = col->ptr;
         if (aij->r_next != NULL) aij->r_next->r_prev = aij;
         if (aij->c_next != NULL) aij->c_next->c_prev = aij;
         row->ptr = col->ptr = aij;
      }
      /* remove zero elements from j-th column */
      for (aij = col->ptr; aij != NULL; aij = next)
      {  next = aij->c_next;
         if (aij->val == 0.0)
         {  /* remove the element from the row list */
            xassert(aij->r_prev == NULL);
            aij->row->ptr = aij->r_next;
            if (aij->r_next != NULL) aij->r_next->r_prev = NULL;
            /* remove the element from the column list */
            if (aij->c_prev == NULL)
               col->ptr = next;
            else
               aij->c_prev->c_next = next;
            if (next == NULL)
               ;
            else
               next->c_prev = aij->c_prev;
            /* return the element to the memory pool */
            dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
         }
      }
      /* if j-th column is basic, invalidate the basis factorization */
      if (col->stat == GLP_BS) lp->valid = 0;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_load_matrix - load (replace) the whole constraint matrix
*
*  SYNOPSIS
*
*  void glp_load_matrix(glp_prob *lp, int ne, const int ia[],
*     const int ja[], const double ar[]);
*
*  DESCRIPTION
*
*  The routine glp_load_matrix loads the constraint matrix passed in
*  the arrays ia, ja, and ar into the specified problem object. Before
*  loading the current contents of the constraint matrix is destroyed.
*
*  Constraint coefficients (elements of the constraint matrix) must be
*  specified as triplets (ia[k], ja[k], ar[k]) for k = 1, ..., ne,
*  where ia[k] is the row index, ja[k] is the column index, ar[k] is a
*  numeric value of corresponding constraint coefficient. The parameter
*  ne specifies the total number of (non-zero) elements in the matrix
*  to be loaded. Coefficients with identical indices are not allowed.
*  Zero coefficients are allowed, however, they are not stored in the
*  constraint matrix.
*
*  If the parameter ne is zero, the parameters ia, ja, and ar can be
*  specified as NULL. */

void glp_load_matrix(glp_prob *lp, int ne, const int ia[],
      const int ja[], const double ar[])
{     GLPROW *row;
      GLPCOL *col;
      GLPAIJ *aij, *next;
      int i, j, k;
      /* clear the constraint matrix */
      for (i = 1; i <= lp->m; i++)
      {  row = lp->row[i];
         while (row->ptr != NULL)
         {  aij = row->ptr;
            row->ptr = aij->r_next;
            dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
         }
      }
      xassert(lp->nnz == 0);
      for (j = 1; j <= lp->n; j++) lp->col[j]->ptr = NULL;
      /* load the new contents of the constraint matrix and build its
         row lists */
      if (ne < 0)
         xfault("glp_load_matrix: ne = %d; invalid number of constraint"
            " coefficients\n", ne);
      if (ne > NNZ_MAX)
         xfault("glp_load_matrix: ne = %d; too many constraint coeffici"
            "ents\n", ne);
      for (k = 1; k <= ne; k++)
      {  /* take indices of new element */
         i = ia[k], j = ja[k];
         /* obtain pointer to i-th row */
         if (!(1 <= i && i <= lp->m))
            xfault("glp_load_matrix: ia[%d] = %d; row index out of rang"
               "e\n", k, i);
         row = lp->row[i];
         /* obtain pointer to j-th column */
         if (!(1 <= j && j <= lp->n))
            xfault("glp_load_matrix: ja[%d] = %d; column index out of r"
               "ange\n", k, j);
         col = lp->col[j];
         /* create new element */
         aij = dmp_get_atom(lp->pool, sizeof(GLPAIJ)), lp->nnz++;
         aij->row = row;
         aij->col = col;
         aij->val = ar[k];
         /* add the new element to the beginning of i-th row list */
         aij->r_prev = NULL;
         aij->r_next = row->ptr;
         if (aij->r_next != NULL) aij->r_next->r_prev = aij;
         row->ptr = aij;
      }
      xassert(lp->nnz == ne);
      /* build column lists of the constraint matrix and check elements
         with identical indices */
      for (i = 1; i <= lp->m; i++)
      {  for (aij = lp->row[i]->ptr; aij != NULL; aij = aij->r_next)
         {  /* obtain pointer to corresponding column */
            col = aij->col;
            /* if there is element with identical indices, it can only
               be found in the beginning of j-th column list */
            if (col->ptr != NULL && col->ptr->row->i == i)
            {  for (k = 1; k <= ne; k++)
                  if (ia[k] == i && ja[k] == col->j) break;
               xfault("lpx_load_mat: ia[%d] = %d; ja[%d] = %d; duplicat"
                  "e indices not allowed\n", k, i, k, col->j);
            }
            /* add the element to the beginning of j-th column list */
            aij->c_prev = NULL;
            aij->c_next = col->ptr;
            if (aij->c_next != NULL) aij->c_next->c_prev = aij;
            col->ptr = aij;
         }
      }
      /* remove zero elements from the constraint matrix */
      for (i = 1; i <= lp->m; i++)
      {  row = lp->row[i];
         for (aij = row->ptr; aij != NULL; aij = next)
         {  next = aij->r_next;
            if (aij->val == 0.0)
            {  /* remove the element from the row list */
               if (aij->r_prev == NULL)
                  row->ptr = next;
               else
                  aij->r_prev->r_next = next;
               if (next == NULL)
                  ;
               else
                  next->r_prev = aij->r_prev;
               /* remove the element from the column list */
               if (aij->c_prev == NULL)
                  aij->col->ptr = aij->c_next;
               else
                  aij->c_prev->c_next = aij->c_next;
               if (aij->c_next == NULL)
                  ;
               else
                  aij->c_next->c_prev = aij->c_prev;
               /* return the element to the memory pool */
               dmp_free_atom(lp->pool, aij, sizeof(GLPAIJ)), lp->nnz--;
            }
         }
      }
      /* invalidate the basis factorization */
      lp->valid = 0;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_del_rows - delete rows from problem object
*
*  SYNOPSIS
*
*  void glp_del_rows(glp_prob *lp, int nrs, const int num[]);
*
*  DESCRIPTION
*
*  The routine glp_del_rows deletes rows from the specified problem
*  object. Ordinal numbers of rows to be deleted should be placed in
*  locations num[1], ..., num[nrs], where nrs > 0.
*
*  Note that deleting rows involves changing ordinal numbers of other
*  rows remaining in the problem object. New ordinal numbers of the
*  remaining rows are assigned under the assumption that the original
*  order of rows is not changed. */

void glp_del_rows(glp_prob *lp, int nrs, const int num[])
{     GLPROW *row;
      int i, k, m_new;
      /* mark rows to be deleted */
      if (!(1 <= nrs && nrs <= lp->m))
         xfault("glp_del_rows: nrs = %d; invalid number of rows\n",
            nrs);
      for (k = 1; k <= nrs; k++)
      {  /* take the number of row to be deleted */
         i = num[k];
         /* obtain pointer to i-th row */
         if (!(1 <= i && i <= lp->m))
            xfault("glp_del_rows: num[%d] = %d; row number out of range"
               "\n", k, i);
         row = lp->row[i];
         /* check that the row is not marked yet */
         if (row->i == 0)
            xfault("glp_del_rows: num[%d] = %d; duplicate row numbers n"
               "ot allowed\n", k, i);
         /* erase symbolic name assigned to the row */
         glp_set_row_name(lp, i, NULL);
         xassert(row->node == NULL);
         /* erase corresponding row of the constraint matrix */
         glp_set_mat_row(lp, i, 0, NULL, NULL);
         xassert(row->ptr == NULL);
         /* mark the row to be deleted */
         row->i = 0;
      }
      /* delete all marked rows from the row list */
      m_new = 0;
      for (i = 1; i <= lp->m; i++)
      {  /* obtain pointer to i-th row */
         row = lp->row[i];
         /* check if the row is marked */
         if (row->i == 0)
         {  /* it is marked, delete it */
            dmp_free_atom(lp->pool, row, sizeof(GLPROW));
         }
         else
         {  /* it is not marked; keep it */
            row->i = ++m_new;
            lp->row[row->i] = row;
         }
      }
      /* set new number of rows */
      lp->m = m_new;
      /* invalidate the basis factorization */
      lp->valid = 0;
      return;
}

/***********************************************************************
*  NAME
*
*  glp_del_cols - delete columns from problem object
*
*  SYNOPSIS
*
*  void glp_del_cols(glp_prob *lp, int ncs, const int num[]);
*
*  DESCRIPTION
*
*  The routine glp_del_cols deletes columns from the specified problem
*  object. Ordinal numbers of columns to be deleted should be placed in
*  locations num[1], ..., num[ncs], where ncs > 0.
*
*  Note that deleting columns involves changing ordinal numbers of
*  other columns remaining in the problem object. New ordinal numbers
*  of the remaining columns are assigned under the assumption that the
*  original order of columns is not changed. */

void glp_del_cols(glp_prob *lp, int ncs, const int num[])
{     GLPCOL *col;
      int j, k, n_new;
      /* mark columns to be deleted */
      if (!(1 <= ncs && ncs <= lp->n))
         xfault("glp_del_cols: ncs = %d; invalid number of columns\n",
            ncs);
      for (k = 1; k <= ncs; k++)
      {  /* take the number of column to be deleted */
         j = num[k];
         /* obtain pointer to j-th column */
         if (!(1 <= j && j <= lp->n))
            xfault("glp_del_cols: num[%d] = %d; column number out of ra"
               "nge", k, j);
         col = lp->col[j];
         /* check that the column is not marked yet */
         if (col->j == 0)
            xfault("glp_del_cols: num[%d] = %d; duplicate column number"
               "s not allowed\n", k, j);
         /* erase symbolic name assigned to the column */
         glp_set_col_name(lp, j, NULL);
         xassert(col->node == NULL);
         /* erase corresponding column of the constraint matrix */
         glp_set_mat_col(lp, j, 0, NULL, NULL);
         xassert(col->ptr == NULL);
         /* mark the column to be deleted */
         col->j = 0;
         /* if it is basic, invalidate the basis factorization */
         if (col->stat == GLP_BS) lp->valid = 0;
      }
      /* delete all marked columns from the column list */
      n_new = 0;
      for (j = 1; j <= lp->n; j++)
      {  /* obtain pointer to j-th column */
         col = lp->col[j];
         /* check if the column is marked */
         if (col->j == 0)
         {  /* it is marked; delete it */
            dmp_free_atom(lp->pool, col, sizeof(GLPCOL));
         }
         else
         {  /* it is not marked; keep it */
            col->j = ++n_new;
            lp->col[col->j] = col;
         }
      }
      /* set new number of columns */
      lp->n = n_new;
      /* if the basis header is still valid, adjust it */
      if (lp->valid)
      {  int m = lp->m;
         int *bhead = lp->bhead;
         for (j = 1; j <= n_new; j++)
         {  k = lp->col[j]->bind;
            if (k != 0)
            {  xassert(1 <= k && k <= m);
               bhead[k] = m + j;
            }
         }
      }
      return;
}

/***********************************************************************
*  NAME
*
*  glp_delete_prob - delete problem object
*
*  SYNOPSIS
*
*  void glp_delete_prob(glp_prob *lp);
*
*  DESCRIPTION
*
*  The routine glp_delete_prob deletes the specified problem object and
*  frees all the memory allocated to it. */

void glp_delete_prob(glp_prob *lp)
{     dmp_delete_pool(lp->pool);
      xfree(lp->cps);
#if 0
      xfree(lp->str_buf);
#endif
      xassert(lp->tree == NULL);
      xfree(lp->row);
      xfree(lp->col);
      if (lp->r_tree != NULL) avl_delete_tree(lp->r_tree);
      if (lp->c_tree != NULL) avl_delete_tree(lp->c_tree);
      xfree(lp->bhead);
      if (lp->bfcp != NULL) xfree(lp->bfcp);
      if (lp->bfd != NULL) bfd_delete_it(lp->bfd);
      xfree(lp);
      return;
}

/* eof */
