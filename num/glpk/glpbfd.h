/* glpbfd.h (LP basis factorization driver) */

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

#ifndef _GLPBFD_H
#define _GLPBFD_H

#include "glpfhv.h"
#include "glplpf.h"

typedef struct BFD BFD;

struct BFD
{     /* LP basis factorization */
      int valid;
      /* the factorization is valid only if this flag is set */
      int type;
      /* the factorization type: */
#define BFD_TFT      1  /* LUF + Forrest-Tomlin update */
#define BFD_TBG      2  /* LUF + Schur compl. + Bartels-Golub update */
#define BFD_TGR      3  /* LUF + Schur compl. + Givens rot. update */
      FHV *fhv;
      /* LP basis factorization (BFD_TFT) */
      LPF *lpf;
      /* LP basis factorization (BFD_TBG, BFD_TGR) */
      int lu_size;      /* luf.sv_size */
      double piv_tol;   /* luf.piv_tol */
      int piv_lim;      /* luf.piv_lim */
      int suhl;         /* luf.suhl */
      double eps_tol;   /* luf.eps_tol */
      double max_gro;   /* luf.max_gro */
      int nfs_max;      /* fhv.hh_max */
      double upd_tol;   /* fhv.upd_tol */
      int nrs_max;      /* lpf.n_max */
      int rs_size;      /* lpf.v_size */
      /* internal control parameters */
      int upd_lim;
      /* the factorization update limit */
      int upd_cnt;
      /* the factorization update count */
};

/* return codes: */
#define BFD_ESING    1  /* singular matrix */
#define BFD_ECOND    2  /* ill-conditioned matrix */
#define BFD_ECHECK   3  /* insufficient accuracy */
#define BFD_ELIMIT   4  /* update limit reached */
#define BFD_EROOM    5  /* SVA overflow */

#define bfd_create_it _glp_bfd_create_it
BFD *bfd_create_it(void);
/* create LP basis factorization */

#define bfd_factorize _glp_bfd_factorize
int bfd_factorize(BFD *bfd, int m, const int bh[], int (*col)
      (void *info, int j, int ind[], double val[]), void *info);
/* compute LP basis factorization */

#define bfd_ftran _glp_bfd_ftran
void bfd_ftran(BFD *bfd, double x[]);
/* perform forward transformation (solve system B*x = b) */

#define bfd_btran _glp_bfd_btran
void bfd_btran(BFD *bfd, double x[]);
/* perform backward transformation (solve system B'*x = b) */

#define bfd_update_it _glp_bfd_update_it
int bfd_update_it(BFD *bfd, int j, int bh, int len, const int ind[],
      const double val[]);
/* update LP basis factorization */

#define bfd_delete_it _glp_bfd_delete_it
void bfd_delete_it(BFD *bfd);
/* delete LP basis factorization */

#endif

/* eof */
