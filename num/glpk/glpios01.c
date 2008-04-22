/* glpios01.c */

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

/***********************************************************************
*  NAME
*
*  ios_create_tree - create branch-and-bound tree
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  glp_tree *ios_create_tree(glp_prob *mip, const glp_iocp *parm);
*
*  DESCRIPTION
*
*  The routine ios_create_tree creates the branch-and-bound tree.
*
*  Being created the tree consists of the only root subproblem whose
*  reference number is 1. Note that initially the root subproblem is in
*  frozen state and therefore needs to be revived.
*
*  RETURNS
*
*  The routine returns a pointer to the tree created. */

static IOSNPD *new_node(glp_tree *tree, IOSNPD *parent);

glp_tree *ios_create_tree(glp_prob *mip, const glp_iocp *parm)
{     int m = mip->m;
      int n = mip->n;
      glp_tree *tree;
      int i, j;
      xassert(mip->tree == NULL);
      mip->tree = tree = xmalloc(sizeof(glp_tree));
      tree->pool = dmp_create_pool();
      tree->n = n;
      /* save original problem components */
      tree->orig_m = m;
      tree->orig_type = xcalloc(1+m+n, sizeof(int));
      tree->orig_lb = xcalloc(1+m+n, sizeof(double));
      tree->orig_ub = xcalloc(1+m+n, sizeof(double));
      tree->orig_stat = xcalloc(1+m+n, sizeof(int));
      tree->orig_prim = xcalloc(1+m+n, sizeof(double));
      tree->orig_dual = xcalloc(1+m+n, sizeof(double));
      for (i = 1; i <= m; i++)
      {  GLPROW *row = mip->row[i];
         tree->orig_type[i] = row->type;
         tree->orig_lb[i] = row->lb;
         tree->orig_ub[i] = row->ub;
         tree->orig_stat[i] = row->stat;
         tree->orig_prim[i] = row->prim;
         tree->orig_dual[i] = row->dual;
      }
      for (j = 1; j <= n; j++)
      {  GLPCOL *col = mip->col[j];
         tree->orig_type[m+j] = col->type;
         tree->orig_lb[m+j] = col->lb;
         tree->orig_ub[m+j] = col->ub;
         tree->orig_stat[m+j] = col->stat;
         tree->orig_prim[m+j] = col->prim;
         tree->orig_dual[m+j] = col->dual;
      }
      tree->orig_obj = mip->obj_val;
      /* initialize the branch-and-bound tree */
      tree->nslots = 0;
      tree->avail = 0;
      tree->slot = NULL;
      tree->head = tree->tail = NULL;
      tree->a_cnt = tree->n_cnt = tree->t_cnt = 0;
      /* the root subproblem is not solved yet, so its final components
         are unknown so far */
      tree->root_m = 0;
      tree->root_type = NULL;
      tree->root_lb = tree->root_ub = NULL;
      tree->root_stat = NULL;
      /* the current subproblem does not exist yet */
      tree->curr = NULL;
      tree->mip = mip;
      tree->solved = 0;
      tree->non_int = xcalloc(1+n, sizeof(int));
      memset(&tree->non_int[1], 0, n * sizeof(int));
      /* arrays to save parent subproblem components will be allocated
         later */
      tree->pred_m = tree->pred_max = 0;
      tree->pred_type = NULL;
      tree->pred_lb = tree->pred_ub = NULL;
      tree->pred_stat = NULL;
      /* cut generator */
      tree->first_attempt = 1;
      tree->max_added_cuts = 0;
      tree->min_eff = 0.0;
      tree->miss = 0;
      tree->just_selected = 0;
      tree->mir_gen = NULL;
      tree->round = 0;
      /* create the conflict graph */
      tree->n_ref = xcalloc(1+n, sizeof(int));
      memset(&tree->n_ref[1], 0, n * sizeof(int));
      tree->c_ref = xcalloc(1+n, sizeof(int));
      memset(&tree->c_ref[1], 0, n * sizeof(int));
      tree->g = scg_create_graph(0);
      tree->j_ref = xcalloc(1+tree->g->n_max, sizeof(int));
      /* initialize control parameters */
      tree->parm = parm;
      tree->tm_beg = xtime();
      tree->tm_lag = xlset(0);
      tree->sol_cnt = 0;
      /* initialize advanced solver interface */
      tree->reason = 0;
      tree->reopt = 0;
      tree->br_var = 0;
      tree->br_sel = 0;
      tree->btrack = NULL;
      tree->terminate = 0;
      /* create the root subproblem, which initially is identical to
         the original MIP */
      new_node(tree, NULL);
      return tree;
}

/***********************************************************************
*  NAME
*
*  ios_revive_node - revive specified subproblem
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  void ios_revive_node(glp_tree *tree, int p);
*
*  DESCRIPTION
*
*  The routine ios_revive_node revives the specified subproblem, whose
*  reference number is p, and thereby makes it the current subproblem.
*  Note that the specified subproblem must be active. Besides, if the
*  current subproblem already exists, it must be frozen before reviving
*  another subproblem. */

void ios_revive_node(glp_tree *tree, int p)
{     glp_prob *mip = tree->mip;
      IOSNPD *node, *root;
      /* obtain pointer to the specified subproblem */
      xassert(1 <= p && p <= tree->nslots);
      node = tree->slot[p].node;
      xassert(node != NULL);
      /* the specified subproblem must be active */
      xassert(node->count == 0);
      /* the current subproblem must not exist */
      xassert(tree->curr == NULL);
      /* the specified subproblem becomes current */
      tree->curr = node;
      tree->solved = 0;
      /* obtain pointer to the root subproblem */
      root = tree->slot[1].node;
      xassert(root != NULL);
      /* at this point problem object components correspond to the root
         subproblem, so if the root subproblem should be revived, there
         is nothing more to do */
      if (node == root) goto done;
      xassert(mip->m == tree->root_m);
      /* build path from the root to the current node */
      node->temp = NULL;
      for (node = node; node != NULL; node = node->up)
      {  if (node->up == NULL)
            xassert(node == root);
         else
            node->up->temp = node;
      }
      /* go down from the root to the current node and make necessary
         changes to restore components of the current subproblem */
      for (node = root; node != NULL; node = node->temp)
      {  int m = mip->m;
         int n = mip->n;
         /* if the current node is reached, the problem object at this
            point corresponds to its parent, so save attributes of rows
            and columns for the parent subproblem */
         if (node->temp == NULL)
         {  int i, j;
            tree->pred_m = m;
            /* allocate/reallocate arrays, if necessary */
            if (tree->pred_max < m + n)
            {  int new_size = m + n + 100;
               if (tree->pred_type != NULL) xfree(tree->pred_type);
               if (tree->pred_lb != NULL) xfree(tree->pred_lb);
               if (tree->pred_ub != NULL) xfree(tree->pred_ub);
               if (tree->pred_stat != NULL) xfree(tree->pred_stat);
               tree->pred_max = new_size;
               tree->pred_type = xcalloc(1+new_size, sizeof(int));
               tree->pred_lb = xcalloc(1+new_size, sizeof(double));
               tree->pred_ub = xcalloc(1+new_size, sizeof(double));
               tree->pred_stat = xcalloc(1+new_size, sizeof(int));
            }
            /* save row attributes */
            for (i = 1; i <= m; i++)
            {  GLPROW *row = mip->row[i];
               tree->pred_type[i] = row->type;
               tree->pred_lb[i] = row->lb;
               tree->pred_ub[i] = row->ub;
               tree->pred_stat[i] = row->stat;
            }
            /* save column attributes */
            for (j = 1; j <= n; j++)
            {  GLPCOL *col = mip->col[j];
               tree->pred_type[mip->m+j] = col->type;
               tree->pred_lb[mip->m+j] = col->lb;
               tree->pred_ub[mip->m+j] = col->ub;
               tree->pred_stat[mip->m+j] = col->stat;
            }
         }
         /* change bounds of rows and columns */
         {  IOSBND *b;
            for (b = node->b_ptr; b != NULL; b = b->next)
            {  if (b->k <= m)
                  glp_set_row_bnds(mip, b->k, b->type, b->lb, b->ub);
               else
                  glp_set_col_bnds(mip, b->k-m, b->type, b->lb, b->ub);
            }
         }
         /* change statuses of rows and columns */
         {  IOSTAT *s;
            for (s = node->s_ptr; s != NULL; s = s->next)
            {  if (s->k <= m)
                  glp_set_row_stat(mip, s->k, s->stat);
               else
                  glp_set_col_stat(mip, s->k-m, s->stat);
            }
         }
         /* add new rows */
         if (node->r_ptr != NULL)
         {  IOSROW *r;
            IOSAIJ *a;
            int i, len, *ind;
            double *val;
            ind = xcalloc(1+n, sizeof(int));
            val = xcalloc(1+n, sizeof(double));
            for (r = node->r_ptr; r != NULL; r = r->next)
            {  i = glp_add_rows(mip, 1);
               glp_set_row_name(mip, i, r->name);
               glp_set_row_bnds(mip, i, r->type, r->lb, r->ub);
               len = 0;
               for (a = r->ptr; a != NULL; a = a->next)
                  len++, ind[len] = a->j, val[len] = a->val;
               glp_set_mat_row(mip, i, len, ind, val);
               glp_set_rii(mip, i, r->rii);
               glp_set_row_stat(mip, i, r->stat);
            }
            xfree(ind);
            xfree(val);
         }
#if 1
         /* add new edges to the conflict graph */
         /* add new cliques to the conflict graph */
         /* (not implemented yet) */
         xassert(node->own_nn == 0);
         xassert(node->own_nc == 0);
         xassert(node->e_ptr == NULL);
#endif
      }
      /* the specified subproblem has been revived */
      node = tree->curr;
      /* delete its bound change list */
      while (node->b_ptr != NULL)
      {  IOSBND *b;
         b = node->b_ptr;
         node->b_ptr = b->next;
         dmp_free_atom(tree->pool, b, sizeof(IOSBND));
      }
      /* delete its status change list */
      while (node->s_ptr != NULL)
      {  IOSTAT *s;
         s = node->s_ptr;
         node->s_ptr = s->next;
         dmp_free_atom(tree->pool, s, sizeof(IOSTAT));
      }
#if 1
      /* delete its row addition list (additional rows may appear, for
         example, due to branching on GUB constraints */
      /* (not implemented yet) */
      xassert(node->r_ptr == NULL);
#endif
done: return;
}

/***********************************************************************
*  NAME
*
*  ios_freeze_node - freeze current subproblem
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  void ios_freeze_node(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine ios_freeze_node freezes the current subproblem. */

void ios_freeze_node(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int m = mip->m;
      int n = mip->n;
      IOSNPD *node;
      /* obtain pointer to the current subproblem */
      node = tree->curr;
      xassert(node != NULL);
      if (node->up == NULL)
      {  /* freeze the root subproblem */
         int k;
         xassert(node->p == 1);
         xassert(tree->root_m == 0);
         xassert(tree->root_type == NULL);
         xassert(tree->root_lb == NULL);
         xassert(tree->root_ub == NULL);
         xassert(tree->root_stat == NULL);
         tree->root_m = m;
         tree->root_type = xcalloc(1+m+n, sizeof(int));
         tree->root_lb = xcalloc(1+m+n, sizeof(double));
         tree->root_ub = xcalloc(1+m+n, sizeof(double));
         tree->root_stat = xcalloc(1+m+n, sizeof(int));
         for (k = 1; k <= m+n; k++)
         {  if (k <= m)
            {  GLPROW *row = mip->row[k];
               tree->root_type[k] = row->type;
               tree->root_lb[k] = row->lb;
               tree->root_ub[k] = row->ub;
               tree->root_stat[k] = row->stat;
            }
            else
            {  GLPCOL *col = mip->col[k-m];
               tree->root_type[k] = col->type;
               tree->root_lb[k] = col->lb;
               tree->root_ub[k] = col->ub;
               tree->root_stat[k] = col->stat;
            }
         }
      }
      else
      {  /* freeze non-root subproblem */
         int root_m = tree->root_m;
         int pred_m = tree->pred_m;
         int i, j, k;
         xassert(pred_m <= m);
         /* build change lists for rows and columns which exist in the
            parent subproblem */
         xassert(node->b_ptr == NULL);
         xassert(node->s_ptr == NULL);
         for (k = 1; k <= pred_m + n; k++)
         {  int pred_type, pred_stat, type, stat;
            double pred_lb, pred_ub, lb, ub;
            /* determine attributes in the parent subproblem */
            pred_type = tree->pred_type[k];
            pred_lb = tree->pred_lb[k];
            pred_ub = tree->pred_ub[k];
            pred_stat = tree->pred_stat[k];
            /* determine attributes in the current subproblem */
            if (k <= pred_m)
            {  GLPROW *row = mip->row[k];
               type = row->type;
               lb = row->lb;
               ub = row->ub;
               stat = row->stat;
            }
            else
            {  GLPCOL *col = mip->col[k - pred_m];
               type = col->type;
               lb = col->lb;
               ub = col->ub;
               stat = col->stat;
            }
            /* save type and bounds of a row/column, if changed */
            if (!(pred_type == type && pred_lb == lb && pred_ub == ub))
            {  IOSBND *b;
               b = dmp_get_atom(tree->pool, sizeof(IOSBND));
               b->k = k;
               b->type = type;
               b->lb = lb;
               b->ub = ub;
               b->next = node->b_ptr;
               node->b_ptr = b;
            }
            /* save status of a row/column, if changed */
            if (pred_stat != stat)
            {  IOSTAT *s;
               s = dmp_get_atom(tree->pool, sizeof(IOSTAT));
               s->k = k;
               s->stat = stat;
               s->next = node->s_ptr;
               node->s_ptr = s;
            }
         }
         /* save new rows added to the current subproblem */
         xassert(node->r_ptr == NULL);
         if (pred_m < m)
         {  int i, len, *ind;
            double *val;
            ind = xcalloc(1+n, sizeof(int));
            val = xcalloc(1+n, sizeof(double));
            for (i = m; i > pred_m; i--)
            {  GLPROW *row = mip->row[i];
               IOSROW *r;
               const char *name;
               r = dmp_get_atom(tree->pool, sizeof(IOSROW));
               name = glp_get_row_name(mip, i);
               if (name == NULL)
                  r->name = NULL;
               else
               {  r->name = dmp_get_atom(tree->pool, strlen(name)+1);
                  strcpy(r->name, name);
               }
               r->type = row->type;
               r->lb = row->lb;
               r->ub = row->ub;
               r->ptr = NULL;
               len = glp_get_mat_row(mip, i, ind, val);
               for (k = 1; k <= len; k++)
               {  IOSAIJ *a;
                  a = dmp_get_atom(tree->pool, sizeof(IOSAIJ));
                  a->j = ind[k];
                  a->val = val[k];
                  a->next = r->ptr;
                  r->ptr = a;
               }
               r->rii = row->rii;
               r->stat = row->stat;
               r->next = node->r_ptr;
               node->r_ptr = r;
            }
            xfree(ind);
            xfree(val);
         }
         /* remove all rows missing in the root subproblem */
         if (m != root_m)
         {  int nrs, *num;
            nrs = m - root_m;
            xassert(nrs > 0);
            num = xcalloc(1+nrs, sizeof(int));
            for (i = 1; i <= nrs; i++) num[i] = root_m + i;
            glp_del_rows(mip, nrs, num);
            xfree(num);
         }
         m = mip->m;
         /* and restore attributes of all rows and columns for the root
            subproblem */
         xassert(m == root_m);
         for (i = 1; i <= m; i++)
         {  glp_set_row_bnds(mip, i, tree->root_type[i],
               tree->root_lb[i], tree->root_ub[i]);
            glp_set_row_stat(mip, i, tree->root_stat[i]);
         }
         for (j = 1; j <= n; j++)
         {  glp_set_col_bnds(mip, j, tree->root_type[m+j],
               tree->root_lb[m+j], tree->root_ub[m+j]);
            glp_set_col_stat(mip, j, tree->root_stat[m+j]);
         }
#if 1
         /* remove all edges and cliques missing in the conflict graph
            for the root subproblem */
         /* (not implemented yet) */
#endif
      }
      /* the current subproblem has been frozen */
      tree->curr = NULL;
      return;
}

/***********************************************************************
*  NAME
*
*  ios_clone_node - clone specified subproblem
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  void ios_clone_node(glp_tree *tree, int p, int nnn, int ref[]);
*
*  DESCRIPTION
*
*  The routine ios_clone_node clones the specified subproblem, whose
*  reference number is p, creating its nnn exact copies. Note that the
*  specified subproblem must be active and must be in the frozen state
*  (i.e. it must not be the current subproblem).
*
*  Each clone, an exact copy of the specified subproblem, becomes a new
*  active subproblem added to the end of the active list. After cloning
*  the specified subproblem becomes inactive.
*
*  The reference numbers of clone subproblems are stored to locations
*  ref[1], ..., ref[nnn]. */

static int get_slot(glp_tree *tree)
{     int p;
      /* if no free slots are available, increase the room */
      if (tree->avail == 0)
      {  int nslots = tree->nslots;
         IOSLOT *save = tree->slot;
         if (nslots == 0)
            tree->nslots = 20;
         else
         {  tree->nslots = nslots + nslots;
            xassert(tree->nslots > nslots);
         }
         tree->slot = xcalloc(1+tree->nslots, sizeof(IOSLOT));
         if (save != NULL)
         {  memcpy(&tree->slot[1], &save[1], nslots * sizeof(IOSLOT));
            xfree(save);
         }
         /* push more free slots into the stack */
         for (p = tree->nslots; p > nslots; p--)
         {  tree->slot[p].node = NULL;
            tree->slot[p].next = tree->avail;
            tree->avail = p;
         }
      }
      /* pull a free slot from the stack */
      p = tree->avail;
      tree->avail = tree->slot[p].next;
      xassert(tree->slot[p].node == NULL);
      tree->slot[p].next = 0;
      return p;
}

static IOSNPD *new_node(glp_tree *tree, IOSNPD *parent)
{     IOSNPD *node;
      int p;
      /* pull a free slot for the new node */
      p = get_slot(tree);
      /* create descriptor of the new subproblem */
      node = dmp_get_atom(tree->pool, sizeof(IOSNPD));
      tree->slot[p].node = node;
      node->p = p;
      node->up = parent;
      node->level = (parent == NULL ? 0 : parent->level + 1);
      node->count = 0;
      node->b_ptr = NULL;
      node->s_ptr = NULL;
      node->r_ptr = NULL;
      node->own_nn = node->own_nc = 0;
      node->e_ptr = NULL;
      node->bound = (parent == NULL ? (tree->mip->dir == GLP_MIN ?
         -DBL_MAX : +DBL_MAX) : parent->bound);
      node->ii_cnt = 0;
      node->ii_sum = 0.0;
      if (tree->parm->cb_size == 0)
         node->data = NULL;
      else
      {  node->data = dmp_get_atom(tree->pool, tree->parm->cb_size);
         memset(node->data, 0, tree->parm->cb_size);
      }
      node->temp = NULL;
      node->prev = tree->tail;
      node->next = NULL;
      /* add the new subproblem to the end of the active list */
      if (tree->head == NULL)
         tree->head = node;
      else
         tree->tail->next = node;
      tree->tail = node;
      tree->a_cnt++;
      tree->n_cnt++;
      tree->t_cnt++;
      /* increase the number of child subproblems */
      if (parent == NULL)
         xassert(p == 1);
      else
         parent->count++;
      return node;
}

void ios_clone_node(glp_tree *tree, int p, int nnn, int ref[])
{     IOSNPD *node;
      int k;
      /* obtain pointer to the subproblem to be cloned */
      xassert(1 <= p && p <= tree->nslots);
      node = tree->slot[p].node;
      xassert(node != NULL);
      /* the specified subproblem must be active */
      xassert(node->count == 0);
      /* and must be in the frozen state */
      xassert(tree->curr != node);
      /* remove the specified subproblem from the active list, because
         it becomes inactive */
      if (node->prev == NULL)
         tree->head = node->next;
      else
         node->prev->next = node->next;
      if (node->next == NULL)
         tree->tail = node->prev;
      else
         node->next->prev = node->prev;
      node->prev = node->next = NULL;
      tree->a_cnt--;
      /* create clone subproblems */
      xassert(nnn > 0);
      for (k = 1; k <= nnn; k++)
         ref[k] = new_node(tree, node)->p;
      return;
}

/***********************************************************************
*  NAME
*
*  ios_delete_node - delete specified subproblem
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  void ios_delete_node(glp_tree *tree, int p);
*
*  DESCRIPTION
*
*  The routine ios_delete_node deletes the specified subproblem, whose
*  reference number is p. The subproblem must be active and must be in
*  the frozen state (i.e. it must not be the current subproblem).
*
*  Note that deletion is performed recursively, i.e. if a subproblem to
*  be deleted is the only child of its parent, the parent subproblem is
*  also deleted, etc. */

void ios_delete_node(glp_tree *tree, int p)
{     IOSNPD *node, *temp;
      /* obtain pointer to the subproblem to be deleted */
      xassert(1 <= p && p <= tree->nslots);
      node = tree->slot[p].node;
      xassert(node != NULL);
      /* the specified subproblem must be active */
      xassert(node->count == 0);
      /* and must be in the frozen state */
      xassert(tree->curr != node);
      /* remove the specified subproblem from the active list, because
         it is gone from the tree */
      if (node->prev == NULL)
         tree->head = node->next;
      else
         node->prev->next = node->next;
      if (node->next == NULL)
         tree->tail = node->prev;
      else
         node->next->prev = node->prev;
      node->prev = node->next = NULL;
      tree->a_cnt--;
loop: /* recursive deletion starts here */
      /* delete the bound change list */
      {  IOSBND *b;
         while (node->b_ptr != NULL)
         {  b = node->b_ptr;
            node->b_ptr = b->next;
            dmp_free_atom(tree->pool, b, sizeof(IOSBND));
         }
      }
      /* delete the status change list */
      {  IOSTAT *s;
         while (node->s_ptr != NULL)
         {  s = node->s_ptr;
            node->s_ptr = s->next;
            dmp_free_atom(tree->pool, s, sizeof(IOSTAT));
         }
      }
      /* delete the row addition list */
      while (node->r_ptr != NULL)
      {  IOSROW *r;
         r = node->r_ptr;
         if (r->name != NULL)
            dmp_free_atom(tree->pool, r->name, strlen(r->name)+1);
         while (r->ptr != NULL)
         {  IOSAIJ *a;
            a = r->ptr;
            r->ptr = a->next;
            dmp_free_atom(tree->pool, a, sizeof(IOSAIJ));
         }
         node->r_ptr = r->next;
         dmp_free_atom(tree->pool, r, sizeof(IOSROW));
      }
#if 1
      /* delete the edge addition list */
      /* delete the clique addition list */
      /* (not implemented yet) */
      xassert(node->own_nn == 0);
      xassert(node->own_nc == 0);
      xassert(node->e_ptr == NULL);
#endif
      /* free application-specific data */
      if (tree->parm->cb_size == 0)
         xassert(node->data == NULL);
      else
         dmp_free_atom(tree->pool, node->data, tree->parm->cb_size);
      /* free the corresponding node slot */
      p = node->p;
      xassert(tree->slot[p].node == node);
      tree->slot[p].node = NULL;
      tree->slot[p].next = tree->avail;
      tree->avail = p;
      /* save pointer to the parent subproblem */
      temp = node->up;
      /* delete the subproblem descriptor */
      dmp_free_atom(tree->pool, node, sizeof(IOSNPD));
      tree->n_cnt--;
      /* take pointer to the parent subproblem */
      node = temp;
      if (node != NULL)
      {  /* the parent subproblem exists; decrease the number of its
            child subproblems */
         xassert(node->count > 0);
         node->count--;
         /* if now the parent subproblem has no childs, it also must be
            deleted */
         if (node->count == 0) goto loop;
      }
      return;
}

/***********************************************************************
*  NAME
*
*  ios_delete_tree - delete branch-and-bound tree
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  void ios_delete_tree(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine ios_delete_tree deletes the branch-and-bound tree, which
*  the parameter tree points to, and frees all the memory allocated to
*  this program object.
*
*  On exit components of the problem object are restored to correspond
*  to the original MIP passed to the routine ios_create_tree. */

void ios_delete_tree(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int i, j;
      int m = mip->m;
      int n = mip->n;
      xassert(mip->tree == tree);
      /* remove all additional rows */
      if (m != tree->orig_m)
      {  int nrs, *num;
         nrs = m - tree->orig_m;
         xassert(nrs > 0);
         num = xcalloc(1+nrs, sizeof(int));
         for (i = 1; i <= nrs; i++) num[i] = tree->orig_m + i;
         glp_del_rows(mip, nrs, num);
         xfree(num);
      }
      m = tree->orig_m;
      /* restore original attributes of rows and columns */
      xassert(m == tree->orig_m);
      xassert(n == tree->n);
      for (i = 1; i <= m; i++)
      {  glp_set_row_bnds(mip, i, tree->orig_type[i],
            tree->orig_lb[i], tree->orig_ub[i]);
         glp_set_row_stat(mip, i, tree->orig_stat[i]);
         mip->row[i]->prim = tree->orig_prim[i];
         mip->row[i]->dual = tree->orig_dual[i];
      }
      for (j = 1; j <= n; j++)
      {  glp_set_col_bnds(mip, j, tree->orig_type[m+j],
            tree->orig_lb[m+j], tree->orig_ub[m+j]);
         glp_set_col_stat(mip, j, tree->orig_stat[m+j]);
         mip->col[j]->prim = tree->orig_prim[m+j];
         mip->col[j]->dual = tree->orig_dual[m+j];
      }
      mip->pbs_stat = mip->dbs_stat = GLP_FEAS;
      mip->obj_val = tree->orig_obj;
      /* delete the branch-and-bound tree */
      dmp_delete_pool(tree->pool);
      xfree(tree->orig_type);
      xfree(tree->orig_lb);
      xfree(tree->orig_ub);
      xfree(tree->orig_stat);
      xfree(tree->orig_prim);
      xfree(tree->orig_dual);
      xfree(tree->slot);
      if (tree->root_type != NULL) xfree(tree->root_type);
      if (tree->root_lb != NULL) xfree(tree->root_lb);
      if (tree->root_ub != NULL) xfree(tree->root_ub);
      if (tree->root_stat != NULL) xfree(tree->root_stat);
      xfree(tree->non_int);
      xfree(tree->n_ref);
      xfree(tree->c_ref);
      xfree(tree->j_ref);
      scg_delete_graph(tree->g);
      if (tree->pred_type != NULL) xfree(tree->pred_type);
      if (tree->pred_lb != NULL) xfree(tree->pred_lb);
      if (tree->pred_ub != NULL) xfree(tree->pred_ub);
      if (tree->pred_stat != NULL) xfree(tree->pred_stat);
#if 0
      xassert(tree->cut_gen == NULL);
#endif
      xassert(tree->mir_gen == NULL);
      xfree(tree);
      mip->tree = NULL;
      return;
}

/***********************************************************************
*  NAME
*
*  ios_best_node - find active node with best local bound
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  int ios_best_node(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine ios_best_node finds an active node whose local bound is
*  best among other active nodes.
*
*  It is understood that the integer optimal solution of the original
*  mip problem cannot be better than the best bound, so the best bound
*  is an lower (minimization) or upper (maximization) global bound for
*  the original problem.
*
*  RETURNS
*
*  The routine ios_best_node returns the subproblem reference number
*  for the best node. However, if the tree is empty, it returns zero. */

int ios_best_node(glp_tree *tree)
{     IOSNPD *node, *best = NULL;
      switch (tree->mip->dir)
      {  case GLP_MIN:
            /* minimization */
            for (node = tree->head; node != NULL; node = node->next)
               if (best == NULL || best->bound > node->bound)
                  best = node;
            break;
         case GLP_MAX:
            /* maximization */
            for (node = tree->head; node != NULL; node = node->next)
               if (best == NULL || best->bound < node->bound)
                  best = node;
            break;
         default:
            xassert(tree != tree);
      }
      return best == NULL ? 0 : best->p;
}

/***********************************************************************
*  NAME
*
*  ios_relative_gap - compute relative mip gap
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  double ios_relative_gap(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine ios_relative_gap computes the relative mip gap using the
*  formula:
*
*     gap = |best_mip - best_bnd| / (|best_mip| + DBL_EPSILON),
*
*  where best_mip is the best integer feasible solution found so far,
*  best_bnd is the best (global) bound. If no integer feasible solution
*  has been found yet, rel_gap is set to DBL_MAX.
*
*  RETURNS
*
*  The routine ios_relative_gap returns the relative mip gap. */

double ios_relative_gap(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      int p;
      double best_mip, best_bnd, gap;
      if (mip->mip_stat == GLP_FEAS)
      {  best_mip = mip->mip_obj;
         p = ios_best_node(tree);
         if (p == 0)
         {  /* the tree is empty */
            gap = 0.0;
         }
         else
         {  best_bnd = tree->slot[p].node->bound;
            gap = fabs(best_mip - best_bnd) / (fabs(best_mip) +
               DBL_EPSILON);
         }
      }
      else
      {  /* no integer feasible solution has been found yet */
         gap = DBL_MAX;
      }
      return gap;
}

/***********************************************************************
*  NAME
*
*  ios_solve_node - solve LP relaxation of current subproblem
*
*  SYNOPSIS
*
*  #include "glpios.h"
*  int ios_solve_node(glp_tree *tree);
*
*  DESCRIPTION
*
*  The routine ios_solve_node re-optimizes LP relaxation of the current
*  subproblem using the dual simplex method.
*
*  RETURNS
*
*  The routine returns the code which is reported by glp_simplex. */

int ios_solve_node(glp_tree *tree)
{     glp_prob *mip = tree->mip;
      glp_smcp parm;
      int ret;
      /* the current subproblem must exist */
      xassert(tree->curr != NULL);
      /* set some control parameters */
      glp_init_smcp(&parm);
      switch (tree->parm->msg_lev)
      {  case GLP_MSG_OFF:
            parm.msg_lev = GLP_MSG_OFF; break;
         case GLP_MSG_ERR:
            parm.msg_lev = GLP_MSG_ERR; break;
         case GLP_MSG_ON:
         case GLP_MSG_ALL:
            parm.msg_lev = GLP_MSG_ON; break;
         case GLP_MSG_DBG:
            parm.msg_lev = GLP_MSG_ALL; break;
         default:
            xassert(tree != tree);
      }
      parm.meth = GLP_DUALP;
      if (tree->parm->msg_lev < GLP_MSG_DBG)
         parm.out_dly = tree->parm->out_dly;
      else
         parm.out_dly = 0;
      /* if the incumbent objective value is already known, use it to
         prematurely terminate the dual simplex search */
      if (mip->mip_stat == GLP_FEAS)
      {  switch (tree->mip->dir)
         {  case GLP_MIN:
               parm.obj_ul = mip->mip_obj;
               break;
            case GLP_MAX:
               parm.obj_ll = mip->mip_obj;
               break;
            default:
               xassert(mip != mip);
         }
      }
      /* try to solve/re-optimize the LP relaxation */
      ret = glp_simplex(mip, &parm);
      return ret;
}

/**********************************************************************/

IOSPOOL *ios_create_pool(glp_tree *tree)
{     /* create cut pool */
      IOSPOOL *pool;
      pool = dmp_get_atom(tree->pool, sizeof(IOSPOOL));
      pool->size = 0;
      pool->head = pool->tail = NULL;
      return pool;
}

IOSCUT *ios_add_cut_row(glp_tree *tree, IOSPOOL *pool, int len,
      int ind[], double val[], int type, double rhs)
{     /* add cut row to the cut pool */
      int n = tree->n;
      IOSCUT *cut;
      IOSAIJ *aij;
      int k;
      cut = dmp_get_atom(tree->pool, sizeof(IOSCUT));
      cut->ptr = NULL;
      xassert(0 <= len && len <= n);
      for (k = len; k >= 1; k--)
      {  aij = dmp_get_atom(tree->pool, sizeof(IOSAIJ));
         xassert(1 <= ind[k] && ind[k] <= n);
         aij->j = ind[k];
         aij->val = val[k];
         aij->next = cut->ptr;
         cut->ptr = aij;
      }
      xassert(type == GLP_LO || type == GLP_UP || type == GLP_FX);
      cut->type = type;
      cut->rhs = rhs;
      cut->prev = pool->tail;
      cut->next = NULL;
      if (cut->prev == NULL)
         pool->head = cut;
      else
         cut->prev->next = cut;
      pool->tail = cut;
      pool->size++;
      return cut;
}

void ios_del_cut_row(glp_tree *tree, IOSPOOL *pool, IOSCUT *cut)
{     /* remove cut row from the cut pool */
      xassert(pool->size > 0);
      if (cut->prev == NULL)
      {  xassert(pool->head == cut);
         pool->head = cut->next;
      }
      else
      {  xassert(cut->prev->next == cut);
         cut->prev->next = cut->next;
      }
      if (cut->next == NULL)
      {  xassert(pool->tail == cut);
         pool->tail = cut->prev;
      }
      else
      {  xassert(cut->next->prev == cut);
         cut->next->prev = cut->prev;
      }
      while (cut->ptr != NULL)
      {  IOSAIJ *aij = cut->ptr;
         cut->ptr = aij->next;
         dmp_free_atom(tree->pool, aij, sizeof(IOSAIJ));
      }
      dmp_free_atom(tree->pool, cut, sizeof(IOSCUT));
      pool->size--;
      return;
}

void ios_delete_pool(glp_tree *tree, IOSPOOL *pool)
{     /* delete cut pool */
      while (pool->head != NULL)
      {  IOSCUT *cut = pool->head;
         pool->head = cut->next;
         while (cut->ptr != NULL)
         {  IOSAIJ *aij = cut->ptr;
            cut->ptr = aij->next;
            dmp_free_atom(tree->pool, aij, sizeof(IOSAIJ));
         }
         dmp_free_atom(tree->pool, cut, sizeof(IOSCUT));
      }
      dmp_free_atom(tree->pool, pool, sizeof(IOSPOOL));
      return;
}

/**********************************************************************/

static int refer_to_node(glp_tree *tree, int j)
{     /* determine node number corresponding to binary variable x[j] or
         its complement */
      glp_prob *mip = tree->mip;
      int n = mip->n;
      int *ref;
      if (j > 0)
         ref = tree->n_ref;
      else
         ref = tree->c_ref, j = - j;
      xassert(1 <= j && j <= n);
      if (ref[j] == 0)
      {  /* new node is needed */
         SCG *g = tree->g;
         int n_max = g->n_max;
         ref[j] = scg_add_nodes(g, 1);
         if (g->n_max > n_max)
         {  int *save = tree->j_ref;
            tree->j_ref = xcalloc(1+g->n_max, sizeof(int));
            memcpy(&tree->j_ref[1], &save[1], g->n * sizeof(int));
            xfree(save);
         }
         xassert(ref[j] == g->n);
         tree->j_ref[ref[j]] = j;
         xassert(tree->curr != NULL);
         if (tree->curr->level > 0) tree->curr->own_nn++;
      }
      return ref[j];
}

void ios_add_edge(glp_tree *tree, int j1, int j2)
{     /* add new edge to the conflict graph */
      glp_prob *mip = tree->mip;
      int n = mip->n;
      SCGRIB *e;
      int first, i1, i2;
      xassert(-n <= j1 && j1 <= +n && j1 != 0);
      xassert(-n <= j2 && j2 <= +n && j2 != 0);
      xassert(j1 != j2);
      /* determine number of the first node, which was added for the
         current subproblem */
      xassert(tree->curr != NULL);
      first = tree->g->n - tree->curr->own_nn + 1;
      /* determine node numbers for both endpoints */
      i1 = refer_to_node(tree, j1);
      i2 = refer_to_node(tree, j2);
      /* add edge (i1,i2) to the conflict graph */
      e = scg_add_edge(tree->g, i1, i2);
      /* if the current subproblem is not the root and both endpoints
         were created on some previous levels, save the edge */
      if (tree->curr->level > 0 && i1 < first && i2 < first)
      {  IOSRIB *rib;
         rib = dmp_get_atom(tree->pool, sizeof(IOSRIB));
         rib->j1 = j1;
         rib->j2 = j2;
         rib->e = e;
         rib->next = tree->curr->e_ptr;
         tree->curr->e_ptr = rib;
      }
      return;
}

/* eof */
