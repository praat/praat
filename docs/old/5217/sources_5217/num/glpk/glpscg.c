/* glpscg.c */

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

#include "glpscg.h"

#define _GLPSCG_DEBUG 0

/**********************************************************************/

SCG *scg_create_graph(int n)
{     /* create cliqued graph */
      SCG *g;
      xassert(n >= 0);
      g = xmalloc(sizeof(SCG));
      g->pool = dmp_create_pool();
      g->n_max = 50;
      g->nc_max = 10;
      g->n = g->nc = 0;
      g->i_ptr = xcalloc(1+g->n_max, sizeof(SCGRIB *));
      g->j_ptr = xcalloc(1+g->n_max, sizeof(SCGRIB *));
      g->c_ptr = xcalloc(1+g->nc_max, sizeof(SCGCQE *));
      g->v_ptr = xcalloc(1+g->n_max, sizeof(SCGCQE *));
      g->flag = xcalloc(1+g->n_max, sizeof(char));
      if (n > 0) scg_add_nodes(g, n);
      return g;
}

/**********************************************************************/

int scg_add_nodes(SCG *g, int num)
{     /* add new nodes to cliqued graph */
      int n_new, i;
      xassert(num > 0);
      /* determine new number of nodes */
      n_new = g->n + num;
      xassert(n_new > 0);
      /* enlarge the room, if necessary */
      if (g->n_max < n_new)
      {  void **save;
         while (g->n_max < n_new)
         {  g->n_max += g->n_max;
            xassert(g->n_max > 0);
         }
         save = (void **)g->i_ptr;
         g->i_ptr = xcalloc(1+g->n_max, sizeof(SCGRIB *));
         memcpy(&g->i_ptr[1], &save[1], g->n * sizeof(SCGRIB *));
         xfree(save);
         save = (void **)g->j_ptr;
         g->j_ptr = xcalloc(1+g->n_max, sizeof(SCGRIB *));
         memcpy(&g->j_ptr[1], &save[1], g->n * sizeof(SCGRIB *));
         xfree(save);
         save = (void **)g->v_ptr;
         g->v_ptr = xcalloc(1+g->n_max, sizeof(SCGCQE *));
         memcpy(&g->v_ptr[1], &save[1], g->n * sizeof(SCGCQE *));
         xfree(save);
         xfree(g->flag);
         g->flag = xcalloc(1+g->n_max, sizeof(char));
         memset(&g->flag[1], 0, g->n);
      }
      /* add new nodes to the end of the node list */
      for (i = g->n+1; i <= n_new; i++)
      {  g->i_ptr[i] = NULL;
         g->j_ptr[i] = NULL;
         g->v_ptr[i] = NULL;
         g->flag[i] = 0;
      }
      /* set new number of nodes */
      g->n = n_new;
      /* return number of first node added */
      return n_new - num + 1;
}

/**********************************************************************/

SCGRIB *scg_add_edge(SCG *g, int i, int j)
{     /* add new edge (i,j) to cliqued graph */
      SCGRIB *e;
      int t;
      xassert(1 <= i && i <= g->n);
      xassert(1 <= j && j <= g->n);
      if (i > j) t = i, i = j, j = t;
      xassert(i < j);
      e = dmp_get_atom(g->pool, sizeof(SCGRIB));
      e->i = i;
      e->j = j;
      e->i_prev = NULL;
      e->i_next = g->i_ptr[i];
      e->j_prev = NULL;
      e->j_next = g->j_ptr[j];
      if (e->i_next != NULL) e->i_next->i_prev = e;
      if (e->j_next != NULL) e->j_next->j_prev = e;
      g->i_ptr[i] = g->j_ptr[j] = e;
      return e;
}

/***********************************************************************
*  NAME
*
*  scg_adj_list - get adjacency list for a node of cliqued graph
*
*  SYNOPSIS
*
*  #include "glpscg.h"
*  int scg_adj_list(SCG *g, int i, int adj[]);
*
*  DESCRIPTION
*
*  For a given node i of the graph g the routine scg_adj_list stores
*  numbers of adjacent nodes to locations adj[1], ..., adj[nadj], where
*  0 <= nadj <= n-1 is the number of adjacent nodes, n is the number of
*  nodes in the graph. Note that the list of adjacent nodes does not
*  include node i.
*
*  RETURNS
*
*  The routine scg_adj_list returns nadj, which is the number of nodes
*  adjacent to node i. */

int scg_adj_list(SCG *g, int i, int adj[])
{     int n = g->n;
      char *flag = g->flag;
      SCGRIB *e;
      SCGCQE *p, *q;
      int j, c, nadj = 0;
      xassert(1 <= i && i <= n);
#if _GLPSCG_DEBUG
      for (j = 1; j <= n; j++) xassert(!flag[j]);
#endif
      /* go through the list of edges (i,j) and add node j to the
         adjacency list */
      for (e = g->i_ptr[i]; e != NULL; e = e->i_next)
      {  j = e->j;
#if _GLPSCG_DEBUG
         xassert(i < j && j <= n);
#endif
         if (!flag[j]) adj[++nadj] = j, flag[j] = 1;
      }
      /* go through the list of edges (j,i) and add node j to the
         adjacency list */
      for (e = g->j_ptr[i]; e != NULL; e = e->j_next)
      {  j = e->i;
#if _GLPSCG_DEBUG
         xassert(1 <= j && j < i);
#endif
         if (!flag[j]) adj[++nadj] = j, flag[j] = 1;
      }
#if 1
      /* not tested yet */
      xassert(g->v_ptr[i] == NULL);
#endif
      /* go through the list of cliques containing node i */
      for (p = g->v_ptr[i]; p != NULL; p = p->v_next)
      {  c = p->c;
         /* clique c contains node i */
#if _GLPSCG_DEBUG
         xassert(1 <= c && c <= g->nc);
#endif
         /* go through the list of nodes of clique c and add them
            (except node i) to the adjacency list */
         for (q = g->c_ptr[c]; q != NULL; q = q->c_next)
         {  j = q->i;
            /* clique c contains node j */
#if _GLPSCG_DEBUG
            xassert(1 <= j && j <= n);
#endif
            if (j != i && !flag[j]) adj[++nadj] = j, flag[j] = 1;
         }
      }
      /* reset the working array */
      for (j = 1; j <= nadj; j++) flag[adj[j]] = 0;
#if _GLPSCG_DEBUG
      for (j = 1; j <= n; j++) xassert(!flag[j]);
#endif
      return nadj;
}

/***********************************************************************
*  MAXIMUM WEIGHT CLIQUE
*
*  Two subroutines sub and wclique below are used to find a maximum
*  weight clique in a given undirected graph. These subroutines are
*  slightly modified version of the program WCLIQUE developed by Patric
*  Ostergard <http://www.tcs.hut.fi/~pat/wclique.html> and based on
*  ideas from the article "P. R. J. Ostergard, A new algorithm for the
*  maximum-weight clique problem, submitted for publication", which, in
*  turn, is a generalization of the algorithm for unweighted graphs
*  presented in "P. R. J. Ostergard, A fast algorithm for the maximum
*  clique problem, submitted for publication".
*
*  USED WITH PERMISSION OF THE AUTHOR OF THE ORIGINAL CODE. */

struct dsa
{     /* dynamic storage area */
      SCG *g;
      /* given (cliqued) graph */
      int i;
      /* node number, for which the adjacency list is built and stored
         in the arrays adj and flag */
      int nadj;
      /* size of the adjacency list, 0 <= nadj <= n-1 */
      int *adj; /* int list[1+n]; */
      /* the adjacency list: adj[1], ..., adj[nadj] are nodes adjacent
         to node i */
      char *flag; /* int flag[1+n]; */
      /* flag[j] means that there is edge (i,j) in the graph */
      const int *wt; /* int wt[0:n-1]; */
      /* weights */
      int record;
      /* weight of best clique */
      int rec_level;
      /* number of vertices in best clique */
      int *rec; /* int rec[0:n-1]; */
      /* best clique so far */
      int *clique; /* int clique[0:n-1]; */
      /* table for pruning */
      int *set; /* int set[0:n-1]; */
      /* current clique */
};

static int is_edge(struct dsa *dsa, int i, int j)
{     /* returns non-zero if there is edge (i,j) in the graph */
      SCG *g = dsa->g;
      int n = g->n;
      int *adj = dsa->adj;
      char *flag = dsa->flag;
      int k;
      i++, j++;
      xassert(1 <= i && i <= n);
      xassert(1 <= j && j <= n);
      /* build the adjacency list, if necessary */
      if (dsa->i != i)
      {  for (k = dsa->nadj; k >= 1; k--) flag[adj[k]] = 0;
         dsa->i = i;
         dsa->nadj = scg_adj_list(g, i, adj);
         for (k = dsa->nadj; k >= 1; k--) flag[adj[k]] = 1;
      }
      return flag[j];
}

static void sub(struct dsa *dsa, int ct, int table[], int level,
      int weight, int l_weight)
{     int n = dsa->g->n;
      const int *wt = dsa->wt;
      int *rec = dsa->rec;
      int *clique = dsa->clique;
      int *set = dsa->set;
      int i, j, k, curr_weight, left_weight, *p1, *p2, *newtable;
      newtable = xcalloc(n, sizeof(int));
      if (ct <= 0)
      {  /* 0 or 1 elements left; include these */
         if (ct == 0)
         {  set[level++] = table[0];
            weight += l_weight;
         }
         if (weight > dsa->record)
         {  dsa->record = weight;
            dsa->rec_level = level;
            for (i = 0; i < level; i++) rec[i] = set[i];
         }
         goto done;
      }
      for (i = ct; i >= 0; i--)
      {  if ((level == 0) && (i < ct)) goto done;
         k = table[i];
         if ((level > 0) && (clique[k] <= (dsa->record - weight)))
            goto done; /* prune */
         set[level] = k;
         curr_weight = weight + wt[k];
         l_weight -= wt[k];
         if (l_weight <= (dsa->record - curr_weight))
            goto done; /* prune */
         p1 = newtable;
         p2 = table;
         left_weight = 0;
         while (p2 < table + i)
         {  j = *p2++;
#if 0
            if (is_edge(dsa, j, k))
#else
            /* to minimize building adjacency lists (by mao) */
            if (is_edge(dsa, k, j))
#endif
            {  *p1++ = j;
               left_weight += wt[j];
            }
         }
         if (left_weight <= (dsa->record - curr_weight)) continue;
         sub(dsa, p1 - newtable - 1, newtable, level + 1, curr_weight,
            left_weight);
      }
done: xfree(newtable);
      return;
}

static int wclique(SCG *g, const int w[], int sol[])
{     int n = g->n;
      const *wt = &w[1];
      struct dsa _dsa, *dsa = &_dsa;
      int i, j, p, max_wt, max_nwt, wth, *used, *nwt, *pos;
      xlong_t timer;
      xassert(n > 0);
      dsa->g = g;
      dsa->i = 0;
      dsa->nadj = 0;
      dsa->adj = xcalloc(1+n, sizeof(int));
      dsa->flag = xcalloc(1+n, sizeof(char));
      memset(&dsa->flag[1], 0, n);
      dsa->wt = wt;
      dsa->record = 0;
      dsa->rec_level = 0;
      dsa->rec = &sol[1];
      dsa->clique = xcalloc(n, sizeof(int));
      dsa->set = xcalloc(n, sizeof(int));
      used = xcalloc(n, sizeof(int));
      nwt = xcalloc(n, sizeof(int));
      pos = xcalloc(n, sizeof(int));
      /* start timer */
      timer = xtime();
      /* order vertices */
      for (i = 0; i < n; i++)
      {  nwt[i] = 0;
         for (j = 0; j < n; j++)
            if (is_edge(dsa, i, j)) nwt[i] += wt[j];
      }
      for (i = 0; i < n; i++)
         used[i] = 0;
      for (i = n-1; i >= 0; i--)
      {  max_wt = -1;
         max_nwt = -1;
         for (j = 0; j < n; j++)
         {  if ((!used[j]) && ((wt[j] > max_wt) || (wt[j] == max_wt
               && nwt[j] > max_nwt)))
            {  max_wt = wt[j];
               max_nwt = nwt[j];
               p = j;
            }
         }
         pos[i] = p;
         used[p] = 1;
         for (j = 0; j < n; j++)
            if ((!used[j]) && (j != p) && (is_edge(dsa, p, j)))
               nwt[j] -= wt[p];
      }
      /* main routine */
      wth = 0;
      for (i = 0; i < n; i++)
      {  wth += wt[pos[i]];
         sub(dsa, i, pos, 0, 0, wth);
         dsa->clique[pos[i]] = dsa->record;
#if _GLPSCG_DEBUG
         ;
#else
         if (xdifftime(xtime(), timer) >= 5.0 - 0.001)
#endif
         {  /* print current record and reset timer */
            xprintf("level = %d (%d); best = %d\n", i+1, n,
               dsa->record);
            timer = xtime();
         }
      }
      xfree(dsa->adj);
      xfree(dsa->flag);
      xfree(dsa->clique);
      xfree(dsa->set);
      xfree(used);
      xfree(nwt);
      xfree(pos);
      /* return the solution found */
      for (i = 1; i <= dsa->rec_level; i++) sol[i]++;
      return dsa->rec_level;
}

/***********************************************************************
*  NAME
*
*  scg_max_clique - find maximum weight clique in given graph
*
*  SYNOPSIS
*
*  #include "glpscg.h"
*  int scg_max_clique(SCG *g, const int w[], int list[]);
*
*  DESCRIPTION
*
*  The routine scg_max_clique finds an exact maximum weight clique in
*  the given (cliqued) graph.
*
*  On entry the array w specifies node weights in locations w[1], ...
*  w[n], where n is the number of nodes in the graph.
*
*  On exit the routine stores numbers of nodes included in the clique
*  in locations list[1], ..., list[size], where 0 <= size <= n is the
*  clique size.
*
*  RETURNS
*
*  The routine scg_max_clique returns the size of the clique found. */

int scg_max_clique(SCG *g, const int w[], int list[])
{     int size;
      if (g->n == 0)
         size = 0;
      else
         size = wclique(g, w, list);
      return size;
}

/**********************************************************************/

void scg_delete_graph(SCG *g)
{     /* delete cliqued graph */
      dmp_delete_pool(g->pool);
      xfree(g->i_ptr);
      xfree(g->j_ptr);
      xfree(g->c_ptr);
      xfree(g->v_ptr);
      xfree(g->flag);
      xfree(g);
      return;
}

/* eof */
