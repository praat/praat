/* glpipm.c */

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

#include "glpipm.h"
#include "glplib.h"
#include "glpmat.h"

/*----------------------------------------------------------------------
-- ipm_main - solve LP with primal-dual interior-point method.
--
-- *Synopsis*
--
-- #include "glpipm.h"
-- int ipm_main(int m, int n, int A_ptr[], int A_ind[], double A_val[],
--    double b[], double c[], double x[], double y[], double z[]);
--
-- *Description*
--
-- The routine ipm_main is a *tentative* implementation of primal-dual
-- interior point method for solving linear programming problems.
--
-- The routine ipm_main assumes the following *standard* formulation of
-- LP problem to be solved:
--
--    minimize
--
--       F = c[0] + c[1]*x[1] + c[2]*x[2] + ... + c[n]*x[n]
--
--    subject to linear constraints
--
--       a[1,1]*x[1] + a[1,2]*x[2] + ... + a[1,n]*x[n] = b[1]
--       a[2,1]*x[1] + a[2,2]*x[2] + ... + a[2,n]*x[n] = b[2]
--             . . . . . .
--       a[m,1]*x[1] + a[m,2]*x[2] + ... + a[m,n]*x[n] = b[m]
--
--    and non-negative variables
--
--       x[1] >= 0, x[2] >= 0, ..., x[n] >= 0
--
-- where:
-- F                    is objective function;
-- x[1], ..., x[n]      are (structural) variables;
-- c[0]                 is constant term of the objective function;
-- c[1], ..., c[n]      are objective coefficients;
-- a[1,1], ..., a[m,n]  are constraint coefficients;
-- b[1], ..., b[n]      are right-hand sides.
--
-- The parameter m is the number of rows (constraints).
--
-- The parameter n is the number of columns (variables).
--
-- The arrays A_ptr, A_ind, and A_val specify the mxn constraint matrix
-- A in storage-by-rows format. These arrays are not changed on exit.
--
-- The array b specifies the vector of right-hand sides b, which should
-- be stored in locations b[1], ..., b[m]. This array is not changed on
-- exit.
--
-- The array c specifies the vector of objective coefficients c, which
-- should be stored in locations c[1], ..., c[n], and the constant term
-- of the objective function, which should be stored in location c[0].
-- This array is not changed on exit.
--
-- The solution is three vectors x, y, and z, which are stored by the
-- routine in the arrays x, y, and z, respectively. These vectors
-- correspond to the best primal-dual point found during optimization.
-- They are approximate solution of the following system (which is the
-- Karush-Kuhn-Tucker optimality conditions):
--
--    A*x      = b      (primal feasibility condition)
--    A'*y + z = c      (dual feasibility condition)
--    x'*z     = 0      (primal-dual complementarity condition)
--    x >= 0, z >= 0    (non-negativity condition)
--
-- where:
-- x[1], ..., x[n]      are primal (structural) variables;
-- y[1], ..., y[m]      are dual variables (Lagrange multipliers) for
--                      equality constraints;
-- z[1], ..., z[n]      are dual variables (Lagrange multipliers) for
--                      non-negativity constraints.
--
-- *Returns*
--
-- The routine ipm_main returns one of the following codes:
--
-- 0 - optimal solution found;
-- 1 - problem has no feasible (primal or dual) solution;
-- 2 - no convergence;
-- 3 - iterations limit exceeded;
-- 4 - numeric instability on solving Newtonian system.
--
-- In case of non-zero return code the routine returns the best point,
-- which has been reached during optimization. */

#define ITER_MAX 100
/* maximal number of iterations */

struct dsa
{     /* working area used by interior-point routines */
      int m;
      /* number of rows (equality constraints) */
      int n;
      /* number of columns (structural variables) */
      int *A_ptr; /* int A_ptr[1+m+1]; */
      int *A_ind; /* int A_ind[A_ptr[m+1]]; */
      double *A_val; /* double A_val[A_ptr[m+1]]; */
      /* mxn matrix A in storage-by-rows format */
      double *b; /* double b[1+m]; */
      /* m-vector b of right hand-sides */
      double *c; /* double c[1+n]; */
      /* n-vector c of objective coefficients; c[0] is constant term of
         the objective function */
      double *x; /* double x[1+n]; */
      double *y; /* double y[1+m]; */
      double *z; /* double z[1+n]; */
      /* current point in primal-dual space; the best point on exit */
      double *D; /* double D[1+n]; */
      /* nxn diagonal matrix D = X*inv(Z), where X = diag(x[j]) and
         Z = diag(z[j]) */
      int *P; /* int P[1+m+m]; */
      /* mxm permutation matrix P used to minimize fill-in in Cholesky
         factorization */
      int *S_ptr; /* int S_ptr[1+m+1]; */
      int *S_ind; /* int S_ind[S_ptr[m+1]]; */
      double *S_val; /* double S_val[S_ptr[m+1]]; */
      double *S_diag; /* double S_diag[1+m]; */
      /* mxm symmetric matrix S = P*A*D*A'*P' whose upper triangular
         part without diagonal elements is stored in S_ptr, S_ind, and
         S_val in storage-by-rows format, diagonal elements are stored
         in S_diag */
      int *U_ptr; /* int U_ptr[1+m+1]; */
      int *U_ind; /* int U_ind[U_ptr[m+1]]; */
      double *U_val; /* double U_val[U_ptr[m+1]]; */
      double *U_diag; /* double U_diag[1+m]; */
      /* mxm upper triangular matrix U defining Cholesky factorization
         S = U'*U; its non-diagonal elements are stored in U_ptr, U_ind,
         U_val in storage-by-rows format, diagonal elements are stored
         in U_diag */
      int iter;
      /* iteration number (0, 1, 2, ...); iter = 0 corresponds to the
         initial point */
      double obj;
      /* current value of the objective function */
      double rpi;
      /* relative primal infeasibility rpi = ||A*x-b||/(1+||b||) */
      double rdi;
      /* relative dual infeasibility rdi = ||A'*y+z-c||/(1+||c||) */
      double gap;
      /* primal-dual gap = |c'*x-b'*y|/(1+|c'*x|) which is a relative
         difference between primal and dual objective functions */
      double phi;
      /* merit function phi = ||A*x-b||/max(1,||b||) +
                            + ||A'*y+z-c||/max(1,||c||) +
                            + |c'*x-b'*y|/max(1,||b||,||c||) */
      double mu;
      /* duality measure mu = x'*z/n (used as barrier parameter) */
      double rmu;
      /* rmu = max(||A*x-b||,||A'*y+z-c||)/mu */
      double rmu0;
      /* the initial value of rmu on iteration 0 */
      double *phi_min; /* double phi_min[1+ITER_MAX]; */
      /* phi_min[k] = min(phi[k]), where phi[k] is the value of phi on
         k-th iteration, 0 <= k <= iter */
      int best_iter;
      /* iteration number, on which the value of phi reached its best
         (minimal) value */
      double *best_x; /* double best_x[1+n]; */
      double *best_y; /* double best_y[1+m]; */
      double *best_z; /* double best_z[1+n]; */
      /* best point (in the sense of the merit function phi) which has
         been reached on iteration iter_best */
      double best_obj;
      /* objective value at the best point */
      double *dx_aff; /* double dx_aff[1+n]; */
      double *dy_aff; /* double dy_aff[1+m]; */
      double *dz_aff; /* double dz_aff[1+n]; */
      /* affine scaling direction */
      double alfa_aff_p, alfa_aff_d;
      /* maximal primal and dual stepsizes in affine scaling direction,
         on which x and z are still non-negative */
      double mu_aff;
      /* duality measure mu_aff = x_aff'*z_aff/n in the boundary point
         x_aff' = x+alfa_aff_p*dx_aff, z_aff' = z+alfa_aff_d*dz_aff */
      double sigma;
      /* Mehrotra's heuristic parameter (0 <= sigma <= 1) */
      double *dx_cc; /* double dx_cc[1+n]; */
      double *dy_cc; /* double dy_cc[1+m]; */
      double *dz_cc; /* double dz_cc[1+n]; */
      /* centering corrector direction */
      double *dx; /* double dx[1+n]; */
      double *dy; /* double dy[1+m]; */
      double *dz; /* double dz[1+n]; */
      /* final combined direction dx = dx_aff+dx_cc, dy = dy_aff+dy_cc,
         dz = dz_aff+dz_cc */
      double alfa_max_p;
      double alfa_max_d;
      /* maximal primal and dual stepsizes in combined direction, on
         which x and z are still non-negative */
};

/*----------------------------------------------------------------------
-- initialize - allocate and initialize working area.
--
-- This routine allocates and initializes the working area used by all
-- interior-point method routines. */

static void initialize(struct dsa *dsa, int m, int n, int A_ptr[],
      int A_ind[], double A_val[], double b[], double c[], double x[],
      double y[], double z[])
{     int i;
      dsa->m = m;
      dsa->n = n;
      dsa->A_ptr = A_ptr;
      dsa->A_ind = A_ind;
      dsa->A_val = A_val;
      xprintf("lpx_interior: A has %d non-zeros\n", A_ptr[m+1]-1);
      dsa->b = b;
      dsa->c = c;
      dsa->x = x;
      dsa->y = y;
      dsa->z = z;
      dsa->D = xcalloc(1+n, sizeof(double));
      /* P := I */
      dsa->P = xcalloc(1+m+m, sizeof(int));
      for (i = 1; i <= m; i++) dsa->P[i] = dsa->P[m+i] = i;
      /* S := A*A', symbolically */
      dsa->S_ptr = xcalloc(1+m+1, sizeof(int));
      dsa->S_ind = adat_symbolic(m, n, dsa->P, dsa->A_ptr, dsa->A_ind,
         dsa->S_ptr);
      xprintf("lpx_interior: S has %d non-zeros (upper triangle)\n",
         dsa->S_ptr[m+1]-1 + m);
      /* determine P using minimal degree algorithm */
      xprintf("lpx_interior: minimal degree ordering...\n");
      min_degree(m, dsa->S_ptr, dsa->S_ind, dsa->P);
      /* S = P*A*A'*P', symbolically */
      xfree(dsa->S_ind);
      dsa->S_ind = adat_symbolic(m, n, dsa->P, dsa->A_ptr, dsa->A_ind,
         dsa->S_ptr);
      dsa->S_val = xcalloc(dsa->S_ptr[m+1], sizeof(double));
      dsa->S_diag = xcalloc(1+m, sizeof(double));
      /* compute Cholesky factorization S = U'*U, symbolically */
      xprintf("lpx_interior: computing Cholesky factorization...\n");
      dsa->U_ptr = xcalloc(1+m+1, sizeof(int));
      dsa->U_ind = chol_symbolic(m, dsa->S_ptr, dsa->S_ind, dsa->U_ptr);
      xprintf("lpx_interior: U has %d non-zeros\n",
         dsa->U_ptr[m+1]-1 + m);
      dsa->U_val = xcalloc(dsa->U_ptr[m+1], sizeof(double));
      dsa->U_diag = xcalloc(1+m, sizeof(double));
      dsa->iter = 0;
      dsa->obj = 0.0;
      dsa->rpi = 0.0;
      dsa->rdi = 0.0;
      dsa->gap = 0.0;
      dsa->phi = 0.0;
      dsa->mu = 0.0;
      dsa->rmu = 0.0;
      dsa->rmu0 = 0.0;
      dsa->phi_min = xcalloc(1+ITER_MAX, sizeof(double));
      dsa->best_iter = 0;
      dsa->best_x = xcalloc(1+n, sizeof(double));
      dsa->best_y = xcalloc(1+m, sizeof(double));
      dsa->best_z = xcalloc(1+n, sizeof(double));
      dsa->best_obj = 0.0;
      dsa->dx_aff = xcalloc(1+n, sizeof(double));
      dsa->dy_aff = xcalloc(1+m, sizeof(double));
      dsa->dz_aff = xcalloc(1+n, sizeof(double));
      dsa->alfa_aff_p = 0.0;
      dsa->alfa_aff_d = 0.0;
      dsa->mu_aff = 0.0;
      dsa->sigma = 0.0;
      dsa->dx_cc = xcalloc(1+n, sizeof(double));
      dsa->dy_cc = xcalloc(1+m, sizeof(double));
      dsa->dz_cc = xcalloc(1+n, sizeof(double));
      dsa->dx = dsa->dx_aff;
      dsa->dy = dsa->dy_aff;
      dsa->dz = dsa->dz_aff;
      dsa->alfa_max_p = 0.0;
      dsa->alfa_max_d = 0.0;
      return;
}

/*----------------------------------------------------------------------
-- A_by_vec - compute y = A*x.
--
-- This routine computes the matrix-vector product y = A*x, where A is
-- the constraint matrix. */

static void A_by_vec(struct dsa *dsa, double x[], double y[])
{     /* compute y = A*x */
      int m = dsa->m;
      int *A_ptr = dsa->A_ptr;
      int *A_ind = dsa->A_ind;
      double *A_val = dsa->A_val;
      int i, t, beg, end;
      double temp;
      for (i = 1; i <= m; i++)
      {  temp = 0.0;
         beg = A_ptr[i], end = A_ptr[i+1];
         for (t = beg; t < end; t++) temp += A_val[t] * x[A_ind[t]];
         y[i] = temp;
      }
      return;
}

/*----------------------------------------------------------------------
-- AT_by_vec - compute y = A'*x.
--
-- This routine computes the matrix-vector product y = A'*x, where A' is
-- a matrix transposed to the constraint matrix A. */

static void AT_by_vec(struct dsa *dsa, double x[], double y[])
{     /* compute y = A'*x, where A' is transposed to A */
      int m = dsa->m;
      int n = dsa->n;
      int *A_ptr = dsa->A_ptr;
      int *A_ind = dsa->A_ind;
      double *A_val = dsa->A_val;
      int i, j, t, beg, end;
      double temp;
      for (j = 1; j <= n; j++) y[j] = 0.0;
      for (i = 1; i <= m; i++)
      {  temp = x[i];
         if (temp == 0.0) continue;
         beg = A_ptr[i], end = A_ptr[i+1];
         for (t = beg; t < end; t++) y[A_ind[t]] += A_val[t] * temp;
      }
      return;
}

/*----------------------------------------------------------------------
-- decomp_NE - numeric factorization of matrix S = P*A*D*A'*P'.
--
-- This routine implements numeric phase of Cholesky factorization of
-- the matrix S = P*A*D*A'*P', which is a permuted matrix of the normal
-- equation system. Matrix D is assumed to be already computed */

static void decomp_NE(struct dsa *dsa)
{     adat_numeric(dsa->m, dsa->n, dsa->P, dsa->A_ptr, dsa->A_ind,
         dsa->A_val, dsa->D, dsa->S_ptr, dsa->S_ind, dsa->S_val,
         dsa->S_diag);
      chol_numeric(dsa->m, dsa->S_ptr, dsa->S_ind, dsa->S_val,
         dsa->S_diag, dsa->U_ptr, dsa->U_ind, dsa->U_val, dsa->U_diag);
      return;
}

/*----------------------------------------------------------------------
-- solve_NE - solve normal equation system.
--
-- This routine solves the normal equation system
--
--    A*D*A'*y = h.
--
-- It is assumed that the matrix A*D*A' has been previously factorized
-- by the routine decomp_NE.
--
-- On entry the array y contains the vector of right-hand sides h. On
-- exit this array contains the computed vector of unknowns y.
--
-- Once the vector y has been computed the routine checks for numeric
-- stability. If the residual vector
--
--    r = A*D*A'*y - h
--
-- is relatively small, the routine returns zero, otherwise non-zero is
-- returned. */

static int solve_NE(struct dsa *dsa, double y[])
{     int m = dsa->m;
      int n = dsa->n;
      int *P = dsa->P;
      int i, j, ret = 0;
      double *h, *r, *w;
      /* save vector of right-hand sides h */
      h = xcalloc(1+m, sizeof(double));
      for (i = 1; i <= m; i++) h[i] = y[i];
      /* solve normal equation system (A*D*A')*y = h */
      /* since S = P*A*D*A'*P' = U'*U, then A*D*A' = P'*U'*U*P, so we
         have inv(A*D*A') = P'*inv(U)*inv(U')*P */
      /* w := P*h */
      w = xcalloc(1+m, sizeof(double));
      for (i = 1; i <= m; i++) w[i] = y[P[i]];
      /* w := inv(U')*w */
      ut_solve(m, dsa->U_ptr, dsa->U_ind, dsa->U_val, dsa->U_diag, w);
      /* w := inv(U)*w */
      u_solve(m, dsa->U_ptr, dsa->U_ind, dsa->U_val, dsa->U_diag, w);
      /* y := P'*w */
      for (i = 1; i <= m; i++) y[i] = w[P[m+i]];
      xfree(w);
      /* compute residual vector r = A*D*A'*y - h */
      r = xcalloc(1+m, sizeof(double));
      /* w := A'*y */
      w = xcalloc(1+n, sizeof(double));
      AT_by_vec(dsa, y, w);
      /* w := D*w */
      for (j = 1; j <= n; j++) w[j] *= dsa->D[j];
      /* r := A*w */
      A_by_vec(dsa, w, r);
      xfree(w);
      /* r := r - h */
      for (i = 1; i <= m; i++) r[i] -= h[i];
      /* check for numeric stability */
      for (i = 1; i <= m; i++)
      {  if (fabs(r[i]) / (1.0 + fabs(h[i])) > 1e-4)
         {  ret = 1;
            break;
         }
      }
      xfree(h);
      xfree(r);
      return ret;
}

/*----------------------------------------------------------------------
-- solve_NS - solve Newtonian system.
--
-- This routine solves the Newtonian system:
--
--    A*dx               = p
--          A'*dy +   dz = q
--    Z*dx        + X*dz = r
--
-- where X = diag(x[j]), Z = diag(z[j]), by reducing it to the normal
-- equation system:
--
--    (A*inv(Z)*X*A')*dy = A*inv(Z)*(X*q-r)+p
--
-- (it is assumed that the matrix A*inv(Z)*X*A' has been factorized by
-- means of the decomp_NE routine).
--
-- Once vector dy has been computed the routine computes vectors dx and
-- dz as follows:
--
--    dx = inv(Z)*(X*(A'*dy-q)+r)
--
--    dz = inv(X)*(r-Z*dx)
--
-- The routine solve_NS returns a code reported by the routine solve_NE
-- which solves the normal equation system. */

static int solve_NS(struct dsa *dsa, double p[], double q[], double r[],
      double dx[], double dy[], double dz[])
{     int m = dsa->m;
      int n = dsa->n;
      double *x = dsa->x;
      double *z = dsa->z;
      int i, j, ret;
      double *w = dx;
      /* compute the vector of right-hand sides A*inv(Z)*(X*q-r)+p for
         the normal equation system */
      for (j = 1; j <= n; j++)
         w[j] = (x[j] * q[j] - r[j]) / z[j];
      A_by_vec(dsa, w, dy);
      for (i = 1; i <= m; i++) dy[i] += p[i];
      /* solve the normal equation system to compute vector dy */
      ret = solve_NE(dsa, dy);
      /* compute vectors dx and dz */
      AT_by_vec(dsa, dy, dx);
      for (j = 1; j <= n; j++)
      {  dx[j] = (x[j] * (dx[j] - q[j]) + r[j]) / z[j];
         dz[j] = (r[j] - z[j] * dx[j]) / x[j];
      }
      return ret;
}

/*----------------------------------------------------------------------
-- initial_point - choose initial point using Mehrotra's heuristic.
--
-- This routine chooses a starting point using a heuristic proposed in
-- the paper:
--
-- S. Mehrotra. On the implementation of a primal-dual interior point
-- method. SIAM J. on Optim., 2(4), pp. 575-601, 1992.
--
-- The starting point x in the primal space is chosen as a solution of
-- the following least squares problem:
--
--    minimize    ||x||
--
--    subject to  A*x = b
--
-- which can be computed explicitly as follows:
--
--    x = A'*inv(A*A')*b
--
-- Similarly, the starting point (y, z) in the dual space is chosen as
-- a solution of the following least squares problem:
--
--    minimize    ||z||
--
--    subject to  A'*y + z = c
--
-- which can be computed explicitly as follows:
--
--    y = inv(A*A')*A*c
--
--    z = c - A'*y
--
-- However, some components of the vectors x and z may be non-positive
-- or close to zero, so the routine uses a Mehrotra's heuristic to find
-- a more appropriate starting point. */

static void initial_point(struct dsa *dsa)
{     int m = dsa->m;
      int n = dsa->n;
      double *b = dsa->b;
      double *c = dsa->c;
      double *x = dsa->x;
      double *y = dsa->y;
      double *z = dsa->z;
      double *D = dsa->D;
      int i, j;
      double dp, dd, ex, ez, xz;
      /* factorize A*A' */
      for (j = 1; j <= n; j++) D[j] = 1.0;
      decomp_NE(dsa);
      /* x~ = A'*inv(A*A')*b */
      for (i = 1; i <= m; i++) y[i] = b[i];
      solve_NE(dsa, y);
      AT_by_vec(dsa, y, x);
      /* y~ = inv(A*A')*A*c */
      A_by_vec(dsa, c, y);
      solve_NE(dsa, y);
      /* z~ = c - A'*y~ */
      AT_by_vec(dsa, y,z);
      for (j = 1; j <= n; j++) z[j] = c[j] - z[j];
      /* use Mehrotra's heuristic in order to choose more appropriate
         starting point with positive components of vectors x and z */
      dp = dd = 0.0;
      for (j = 1; j <= n; j++)
      {  if (dp < -1.5 * x[j]) dp = -1.5 * x[j];
         if (dd < -1.5 * z[j]) dd = -1.5 * z[j];
      }
      /* note that b = 0 involves x = 0, and c = 0 involves y = 0 and
         z = 0, so we need to be careful */
      if (dp == 0.0) dp = 1.5;
      if (dd == 0.0) dd = 1.5;
      ex = ez = xz = 0.0;
      for (j = 1; j <= n; j++)
      {  ex += (x[j] + dp);
         ez += (z[j] + dd);
         xz += (x[j] + dp) * (z[j] + dd);
      }
      dp += 0.5 * (xz / ez);
      dd += 0.5 * (xz / ex);
      for (j = 1; j <= n; j++)
      {  x[j] += dp;
         z[j] += dd;
         xassert(x[j] > 0.0 && z[j] > 0.0);
      }
      return;
}

/*----------------------------------------------------------------------
-- basic_info - perform basic computations at the current point.
--
-- This routine computes the following quantities at the current point:
--
-- value of the objective function:
--
--    F = c'*x + c[0]
--
-- relative primal infeasibility:
--
--    rpi = ||A*x-b|| / (1+||b||)
--
-- relative dual infeasibility:
--
--    rdi = ||A'*y+z-c|| / (1+||c||)
--
-- primal-dual gap (a relative difference between the primal and the
-- dual objective function values):
--
--    gap = |c'*x-b'*y| / (1+|c'*x|)
--
-- merit function:
--
--    phi = ||A*x-b|| / max(1,||b||) + ||A'*y+z-c|| / max(1,||c||) +
--
--        + |c'*x-b'*y| / max(1,||b||,||c||)
--
-- duality measure:
--
--    mu = x'*z / n
--
-- the ratio of infeasibility to mu:
--
--    rmu = max(||A*x-b||,||A'*y+z-c||) / mu
--
-- where ||*|| denotes euclidian norm, *' denotes transposition */

static void basic_info(struct dsa *dsa)
{     int m = dsa->m;
      int n = dsa->n;
      double *b = dsa->b;
      double *c = dsa->c;
      double *x = dsa->x;
      double *y = dsa->y;
      double *z = dsa->z;
      int i, j;
      double norm1, bnorm, norm2, cnorm, cx, by, *work, temp;
      /* compute value of the objective function */
      temp = c[0];
      for (j = 1; j <= n; j++) temp += c[j] * x[j];
      dsa->obj = temp;
      /* norm1 = ||A*x-b|| */
      work = xcalloc(1+m, sizeof(double));
      A_by_vec(dsa, x, work);
      norm1 = 0.0;
      for (i = 1; i <= m; i++)
         norm1 += (work[i] - b[i]) * (work[i] - b[i]);
      norm1 = sqrt(norm1);
      xfree(work);
      /* bnorm = ||b|| */
      bnorm = 0.0;
      for (i = 1; i <= m; i++) bnorm += b[i] * b[i];
      bnorm = sqrt(bnorm);
      /* compute relative primal infeasibility */
      dsa->rpi = norm1 / (1.0 + bnorm);
      /* norm2 = ||A'*y+z-c|| */
      work = xcalloc(1+n, sizeof(double));
      AT_by_vec(dsa, y, work);
      norm2 = 0.0;
      for (j = 1; j <= n; j++)
         norm2 += (work[j] + z[j] - c[j]) * (work[j] + z[j] - c[j]);
      norm2 = sqrt(norm2);
      xfree(work);
      /* cnorm = ||c|| */
      cnorm = 0.0;
      for (j = 1; j <= n; j++) cnorm += c[j] * c[j];
      cnorm = sqrt(cnorm);
      /* compute relative dual infeasibility */
      dsa->rdi = norm2 / (1.0 + cnorm);
      /* by = b'*y */
      by = 0.0;
      for (i = 1; i <= m; i++) by += b[i] * y[i];
      /* cx = c'*x */
      cx = 0.0;
      for (j = 1; j <= n; j++) cx += c[j] * x[j];
      /* compute primal-dual gap */
      dsa->gap = fabs(cx - by) / (1.0 + fabs(cx));
      /* compute merit function */
      dsa->phi = 0.0;
      dsa->phi += norm1 / (bnorm > 1.0 ? bnorm : 1.0);
      dsa->phi += norm2 / (cnorm > 1.0 ? cnorm : 1.0);
      temp = 1.0;
      if (temp < bnorm) temp = bnorm;
      if (temp < cnorm) temp = cnorm;
      dsa->phi += fabs(cx - by) / temp;
      /* compute duality measure */
      temp = 0.0;
      for (j = 1; j <= n; j++) temp += x[j] * z[j];
      dsa->mu = temp / (double)n;
      /* compute the ratio of infeasibility to mu */
      dsa->rmu = (norm1 > norm2 ? norm1 : norm2) / dsa->mu;
      return;
}

/*----------------------------------------------------------------------
-- make_step - compute next point using Mehrotra's technique.
--
-- This routine computes the next point using the predictor-corrector
-- technique proposed in the paper:
--
-- S. Mehrotra. On the implementation of a primal-dual interior point
-- method. SIAM J. on Optim., 2(4), pp. 575-601, 1992.
--
-- At first the routine computes so called affine scaling (predictor)
-- direction (dx_aff,dy_aff,dz_aff) which is a solution of the system:
--
--    A*dx_aff                       = b - A*x
--              A'*dy_aff +   dz_aff = c - A'*y - z
--    Z*dx_aff            + X*dz_aff = - X*Z*e
--
-- where (x,y,z) is the current point, X = diag(x[j]), Z = diag(z[j]),
-- e = (1,...,1)'.
--
-- Then the routine computes the centering parameter sigma, using the
-- following Mehrotra's heuristic:
--
--    alfa_aff_p = inf{0 <= alfa <= 1 | x+alfa*dx_aff >= 0}
--
--    alfa_aff_d = inf{0 <= alfa <= 1 | z+alfa*dz_aff >= 0}
--
--    mu_aff = (x+alfa_aff_p*dx_aff)'*(z+alfa_aff_d*dz_aff)/n
--
--    sigma = (mu_aff/mu)^3
--
-- where alfa_aff_p is the maximal stepsize along the affine scaling
-- direction in the primal space, alfa_aff_d is the maximal stepsize
-- along the same direction in the dual space.
--
-- After determining sigma the routine computes so called centering
-- (corrector) direction (dx_cc,dy_cc,dz_cc) which is the solution of
-- the system:
--
--    A*dx_cc                     = 0
--             A'*dy_cc +   dz_cc = 0
--    Z*dx_cc           + X*dz_cc = sigma*mu*e - X*Z*e
--
-- Finally, the routine computes the combined direction
--
--    (dx,dy,dz) = (dx_aff,dy_aff,dz_aff) + (dx_cc,dy_cc,dz_cc)
--
-- and determines maximal primal and dual stepsizes along the combined
-- direction:
--
--    alfa_max_p = inf{0 <= alfa <= 1 | x+alfa*dx >= 0}
--
--    alfa_max_d = inf{0 <= alfa <= 1 | z+alfa*dz >= 0}
--
-- In order to prevent the next point to be too close to the boundary
-- of the positive ortant, the routine decreases maximal stepsizes:
--
--    alfa_p = gamma_p * alfa_max_p
--
--    alfa_d = gamma_d * alfa_max_d
--
-- where gamma_p and gamma_d are scaling factors, and computes the next
-- point:
--
--    x_new = x + alfa_p * dx
--
--    y_new = y + alfa_d * dy
--
--    z_new = z + alfa_d * dz
--
-- which becomes the current point on the next iteration. */

static int make_step(struct dsa *dsa)
{     int m = dsa->m;
      int n = dsa->n;
      double *b = dsa->b;
      double *c = dsa->c;
      double *x = dsa->x;
      double *y = dsa->y;
      double *z = dsa->z;
      double *dx_aff = dsa->dx_aff;
      double *dy_aff = dsa->dy_aff;
      double *dz_aff = dsa->dz_aff;
      double *dx_cc = dsa->dx_cc;
      double *dy_cc = dsa->dy_cc;
      double *dz_cc = dsa->dz_cc;
      double *dx = dsa->dx;
      double *dy = dsa->dy;
      double *dz = dsa->dz;
      int i, j, ret = 0;
      double temp, gamma_p, gamma_d, *p, *q, *r;
      /* allocate working arrays */
      p = xcalloc(1+m, sizeof(double));
      q = xcalloc(1+n, sizeof(double));
      r = xcalloc(1+n, sizeof(double));
      /* p = b - A*x */
      A_by_vec(dsa, x, p);
      for (i = 1; i <= m; i++) p[i] = b[i] - p[i];
      /* q = c - A'*y - z */
      AT_by_vec(dsa, y,q);
      for (j = 1; j <= n; j++) q[j] = c[j] - q[j] - z[j];
      /* r = - X * Z * e */
      for (j = 1; j <= n; j++) r[j] = - x[j] * z[j];
      /* solve the first Newtonian system */
      if (solve_NS(dsa, p, q, r, dx_aff, dy_aff, dz_aff))
      {  ret = 1;
         goto done;
      }
      /* alfa_aff_p = inf{0 <= alfa <= 1 | x + alfa*dx_aff >= 0} */
      /* alfa_aff_d = inf{0 <= alfa <= 1 | z + alfa*dz_aff >= 0} */
      dsa->alfa_aff_p = dsa->alfa_aff_d = 1.0;
      for (j = 1; j <= n; j++)
      {  if (dx_aff[j] < 0.0)
         {  temp = - x[j] / dx_aff[j];
            if (dsa->alfa_aff_p > temp) dsa->alfa_aff_p = temp;
         }
         if (dz_aff[j] < 0.0)
         {  temp = - z[j] / dz_aff[j];
            if (dsa->alfa_aff_d > temp) dsa->alfa_aff_d = temp;
         }
      }
      /* mu_aff = (x+alfa_aff_p*dx_aff)' * (z+alfa_aff_d*dz_aff) / n */
      temp = 0.0;
      for (j = 1; j <= n; j++)
         temp += (x[j] + dsa->alfa_aff_p * dx_aff[j]) *
                 (z[j] + dsa->alfa_aff_d * dz_aff[j]);
      dsa->mu_aff = temp / (double)n;
      /* sigma = (mu_aff/mu)^3 */
      temp = dsa->mu_aff / dsa->mu;
      dsa->sigma = temp * temp * temp;
      /* p = 0 */
      for (i = 1; i <= m; i++) p[i] = 0.0;
      /* q = 0 */
      for (j = 1; j <= n; j++) q[j] = 0.0;
      /* r = sigma * mu * e - X * Z * e */
      for (j = 1; j <= n; j++)
         r[j] = dsa->sigma * dsa->mu - dx_aff[j] * dz_aff[j];
      /* solve the second Newtonian system with the same coefficients
         but with altered right-hand sides */
      if (solve_NS(dsa, p, q, r, dx_cc, dy_cc, dz_cc))
      {  ret = 1;
         goto done;
      }
      /* (dx,dy,dz) = (dx_aff,dy_aff,dz_aff) + (dx_cc,dy_cc,dz_cc) */
      for (j = 1; j <= n; j++) dx[j] = dx_aff[j] + dx_cc[j];
      for (i = 1; i <= m; i++) dy[i] = dy_aff[i] + dy_cc[i];
      for (j = 1; j <= n; j++) dz[j] = dz_aff[j] + dz_cc[j];
      /* alfa_max_p = inf{0 <= alfa <= 1 | x + alfa*dx >= 0} */
      /* alfa_max_d = inf{0 <= alfa <= 1 | z + alfa*dz >= 0} */
      dsa->alfa_max_p = dsa->alfa_max_d = 1.0;
      for (j = 1; j <= n; j++)
      {  if (dx[j] < 0.0)
         {  temp = - x[j] / dx[j];
            if (dsa->alfa_max_p > temp) dsa->alfa_max_p = temp;
         }
         if (dz[j] < 0.0)
         {  temp = - z[j] / dz[j];
            if (dsa->alfa_max_d > temp) dsa->alfa_max_d = temp;
         }
      }
      /* determine scale factors (not implemented yet) */
      gamma_p = 0.90;
      gamma_d = 0.90;
      /* compute the next point */
      for (j = 1; j <= n; j++)
      {  x[j] += gamma_p * dsa->alfa_max_p * dx[j];
         xassert(x[j] > 0.0);
      }
      for (i = 1; i <= m; i++)
         y[i] += gamma_d * dsa->alfa_max_d * dy[i];
      for (j = 1; j <= n; j++)
      {  z[j] += gamma_d * dsa->alfa_max_d * dz[j];
         xassert(z[j] > 0.0);
      }
done: /* free working arrays */
      xfree(p);
      xfree(q);
      xfree(r);
      return ret;
}

/*----------------------------------------------------------------------
-- terminate - deallocate working area.
--
-- This routine frees all memory allocated to the working area used by
-- all interior-point method routines. */

static void terminate(struct dsa *dsa)
{     xfree(dsa->D);
      xfree(dsa->P);
      xfree(dsa->S_ptr);
      xfree(dsa->S_ind);
      xfree(dsa->S_val);
      xfree(dsa->S_diag);
      xfree(dsa->U_ptr);
      xfree(dsa->U_ind);
      xfree(dsa->U_val);
      xfree(dsa->U_diag);
      xfree(dsa->phi_min);
      xfree(dsa->best_x);
      xfree(dsa->best_y);
      xfree(dsa->best_z);
      xfree(dsa->dx_aff);
      xfree(dsa->dy_aff);
      xfree(dsa->dz_aff);
      xfree(dsa->dx_cc);
      xfree(dsa->dy_cc);
      xfree(dsa->dz_cc);
      return;
}

/*----------------------------------------------------------------------
-- ipm_main - main interior-point method routine.
--
-- This is a main routine of the primal-dual interior-point method. */

int ipm_main(int m, int n, int A_ptr[], int A_ind[], double A_val[],
      double b[], double c[], double x[], double y[], double z[])
{     struct dsa _dsa, *dsa = &_dsa;
      int i, j, status;
      double temp;
      /* allocate and initialize working area */
      xassert(m > 0);
      xassert(n > 0);
      initialize(dsa, m, n, A_ptr, A_ind, A_val, b, c, x, y, z);
      /* choose initial point using Mehrotra's heuristic */
      xprintf("lpx_interior: guessing initial point...\n");
      initial_point(dsa);
      /* main loop starts here */
      xprintf("Optimization begins...\n");
      for (;;)
      {  /* perform basic computations at the current point */
         basic_info(dsa);
         /* save initial value of rmu */
         if (dsa->iter == 0) dsa->rmu0 = dsa->rmu;
         /* accumulate values of min(phi[k]) and save the best point */
         xassert(dsa->iter <= ITER_MAX);
         if (dsa->iter == 0 || dsa->phi_min[dsa->iter-1] > dsa->phi)
         {  dsa->phi_min[dsa->iter] = dsa->phi;
            dsa->best_iter = dsa->iter;
            for (j = 1; j <= n; j++) dsa->best_x[j] = dsa->x[j];
            for (i = 1; i <= m; i++) dsa->best_y[i] = dsa->y[i];
            for (j = 1; j <= n; j++) dsa->best_z[j] = dsa->z[j];
            dsa->best_obj = dsa->obj;
         }
         else
            dsa->phi_min[dsa->iter] = dsa->phi_min[dsa->iter-1];
         /* display information at the current point */
         xprintf("%3d: obj = %17.9e; rpi = %8.1e; rdi = %8.1e; gap = %8"
            ".1e\n", dsa->iter, dsa->obj, dsa->rpi, dsa->rdi, dsa->gap);
         /* check if the current point is optimal */
         if (dsa->rpi < 1e-8 && dsa->rdi < 1e-8 && dsa->gap < 1e-8)
         {  xprintf("OPTIMAL SOLUTION FOUND\n");
            status = 0;
            break;
         }
         /* check if the problem has no feasible solutions */
         temp = 1e5 * dsa->phi_min[dsa->iter];
         if (temp < 1e-8) temp = 1e-8;
         if (dsa->phi >= temp)
         {  xprintf("PROBLEM HAS NO FEASIBLE PRIMAL/DUAL SOLUTION\n");
            status = 1;
            break;
         }
         /* check for very slow convergence or divergence */
         if (((dsa->rpi >= 1e-8 || dsa->rdi >= 1e-8) && dsa->rmu /
               dsa->rmu0 >= 1e6) ||
               (dsa->iter >= 30 && dsa->phi_min[dsa->iter] >= 0.5 *
               dsa->phi_min[dsa->iter - 30]))
         {  xprintf("NO CONVERGENCE; SEARCH TERMINATED\n");
            status = 2;
            break;
         }
         /* check for maximal number of iterations */
         if (dsa->iter == ITER_MAX)
         {  xprintf("ITERATIONS LIMIT EXCEEDED; SEARCH TERMINATED\n");
            status = 3;
            break;
         }
         /* start the next iteration */
         dsa->iter++;
         /* factorize normal equation system */
         for (j = 1; j <= n; j++) dsa->D[j] = dsa->x[j] / dsa->z[j];
         decomp_NE(dsa);
         /* compute the next point using Mehrotra's predictor-corrector
            technique */
         if (make_step(dsa))
         {  xprintf("NUMERIC INSTABILITY; SEARCH TERMINATED\n");
            status = 4;
            break;
         }
      }
      /* restore the best point */
      if (status != 0)
      {  for (j = 1; j <= n; j++) dsa->x[j] = dsa->best_x[j];
         for (i = 1; i <= m; i++) dsa->y[i] = dsa->best_y[i];
         for (j = 1; j <= n; j++) dsa->z[j] = dsa->best_z[j];
         xprintf("The best point %17.9e was reached on iteration %d\n",
            dsa->best_obj, dsa->best_iter);
      }
      /* deallocate working area */
      terminate(dsa);
      /* return to the calling program */
      return status;
}

/* eof */
