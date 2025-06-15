/* glpscg.h (sparse cliqued graph) */

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

#ifndef _GLPSCG_H
#define _GLPSCG_H

#include "glpdmp.h"

typedef struct SCG SCG;
typedef struct SCGRIB SCGRIB;
typedef struct SCGCQE SCGCQE;

struct SCG
{     /* sparse undirected loopless cliqued graph */
      DMP *pool;
      /* memory pool to store graph components */
      int n_max;
      /* maximal number of nodes (enlarged automatically) */
      int nc_max;
      /* maximal number of cliques (enlarged automatically) */
      int n;
      /* number of nodes, n >= 0 */
      int nc;
      /* number of cliques, nc >= 0 */
      SCGRIB **i_ptr; /* SCGRIB *i_ptr[1+n_max]; */
      /* i_ptr[i], 1 <= i <= n, is a pointer to the list of edges,
         where node i is the first endpoint of the edge */
      SCGRIB **j_ptr; /* SCGRIB *j_ptr[1+n_max]; */
      /* j_ptr[j], 1 <= j <= n, is a pointer to the list of edges,
         where node j is the second endpoint of the edge */
      SCGCQE **c_ptr; /* SCGCQE *c_ptr[1+nc_max]; */
      /* c_ptr[c], 1 <= c <= nc, is a pointer to the list of clique
         elements for nodes, which belong to clique c */
      SCGCQE **v_ptr; /* SCGCQE *v_ptr[1+n_max]; */
      /* v_ptr[i], 1 <= i <= n, is a pointer to the list of clique
         elements for cliques, which contain node i */
      char *flag; /* char flag[1+n]; */
      /* working array (must contain binary zeros) */
};

struct SCGRIB
{     /* edge (i,j), where 1 <= i < j <= n */
      int i;
      /* node i is the first endpoint of this edge */
      int j;
      /* node j is the second endpoint of this edge */
      SCGRIB *i_prev;
      /* pointer to previous edge having the same first endpoint */
      SCGRIB *i_next;
      /* pointer to next edge having the same first endpoint */
      SCGRIB *j_prev;
      /* pointer to previous edge having the same second endpoint */
      SCGRIB *j_next;
      /* pointer to next edge having the same second endpoint */
};

struct SCGCQE
{     /* clique element */
      int c;
      /* clique number (1 <= c <= nc) */
      int i;
      /* node number (1 <= i <= n) */
      SCGCQE *c_next;
      /* pointer to next clique element for the same clique */
      SCGCQE *v_next;
      /* pointer to next clique element for the same node */
};

#define scg_create_graph _glp_scg_create_graph
SCG *scg_create_graph(int n);
/* create cliqued graph */

#define scg_add_nodes _glp_scg_add_nodes
int scg_add_nodes(SCG *g, int num);
/* add new nodes to cliqued graph */

#define scg_add_edge _glp_scg_add_edge
SCGRIB *scg_add_edge(SCG *g, int i, int j);
/* add new edge (i,j) to cliqued graph */

#define scg_adj_list _glp_scg_adj_list
int scg_adj_list(SCG *g, int i, int adj[]);
/* get adjacency list for a node of cliqued graph */

#define scg_max_clique _glp_scg_max_clique
int scg_max_clique(SCG *g, const int w[], int list[]);
/* find maximum weight clique in given graph */

#define scg_delete_graph _glp_scg_delete_graph
void scg_delete_graph(SCG *g);
/* delete cliqued graph */

#endif

/* eof */
