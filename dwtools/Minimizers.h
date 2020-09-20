#ifndef _Minimizers_h_
#define _Minimizers_h_
/* Minimizers.h
 *
 * Copyright (C) 1993-2019 David Weenink, 2015-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Data.h"
#include "Graphics.h"

/*********** deferred class Minimizer **********************************/

Thing_define (Minimizer, Thing) {
	integer numberOfParameters;
	autoVEC p;          /* the parameters */
	double minimum;     /* current minimum */
	autoVEC history;    /* previous minima */
	double tolerance;   /* stopping criterion */
	Daata object;       /* reference to the object that uses this Minimizer */
	integer numberOfFunctionCalls;  /* the number of times 'func' has been called */
	bool success;       /* indicates whether I'm done */
	//integer start;      /* start iteration series */
	integer maximumNumberOfIterations;   /* the current maximum number of iterations */
	integer iteration;       /* the current number of iterations */
	void (*afterHook) (Minimizer me, Thing boss);   /* to be called after each iteration */
	Thing afterBoss;
	Graphics gmonitor;   /* graphics to monitor the minimization process */

	void v_info ()
		override { }

	virtual void v_minimize () { }   /* does the work */
	virtual void v_reset () { }
};

void Minimizer_init (Minimizer me, integer numberOfParameters, Daata object);
/*
	Preconditions:
		numberOfParameters > 0;
*/

void Minimizer_reset (Minimizer me, constVEC const& guess);
/* reset the start values for the minimizer
 * 
 * Post conditions:
 *    p[] = guess[];
 *    my minimum = 1e308;
 *    success = maximumNumberOfIterations = iteration = numberOfFunctionCalls = 0;
 *    reset (me);
 */

void Minimizer_minimize (Minimizer me, integer maximumNumberOfIterations, double tolerance, int monitor);
/* Minimizes during maximally maximumNumberOfIterations. The gmonitor is initialized
 * before minimization and cleared afterwards.
 * Preconditions:
 *    maximumNumberOfIterations >= 0;
 *    tolerance > 0.0;
 * Postconditions:
 *    if (reset) Minimizer_reset called with xopt as initial guess.
 *    after each function call: numberOfFunctionCalls++
 *    after each iteration: iteration++
 */

void Minimizer_minimizeManyTimes (Minimizer me, integer maxIterationsPerTime, integer numberOfTimes, double tolerance);

void Minimizer_drawHistory (Minimizer me, Graphics g, integer itmin, integer itmax, double minimum, double maximum, bool garnish);

double Minimizer_getMinimum (Minimizer me);

/********** deferred class LineMinimizer ************************************/

Thing_define (LineMinimizer, Minimizer) {
	/* the function to be minimized */
	double (*func) (Daata object, VEC const& p);
	double maxLineStep;   // maximum step in line search direction
	autoVEC direction;    // search direction vector
	autoVEC ptry;         // point in search direction

	//virtual void v_linmin (double p[], double fp, double direction[], double *fret);
};

void LineMinimizer_init (LineMinimizer me, integer numberOfParameters, Daata object, double (*func) (Daata object, VEC const& p));

/******************  class SteepestDescentMinimizer**************************/

typedef struct structSteepestDescentMinimizer_parameters {
	double eta, momentum;
} *SteepestDescentMinimizer_parameters;

Thing_define (SteepestDescentMinimizer, Minimizer) {
	double eta, momentum;
	double (*func) (Daata object, VEC const& p);
	void  (*dfunc) (Daata object, VEC const& p, VEC const& dp);
	/* calculates gradient at position p */

	void v_minimize ()
		override;
};

autoSteepestDescentMinimizer SteepestDescentMinimizer_create (integer numberOfParameters, Daata object, double (*func) (Daata object, VEC const& p), void (*dfunc) (Daata object, VEC const& p, VEC const& dp));


/**********  class VDSmagtMinimizer ********************************/

typedef struct structVDSmagtMinimizer_parameters {
	double lineSearchGradient;
	integer lineSearchMaxNumOfIterations;
} *VDSmagtMinimizer_parameters;

Thing_define (VDSmagtMinimizer, Minimizer) {
	double (*func) (Daata object, VEC const& p);
	void  (*dfunc) (Daata object, VEC const& p, VEC const& dp);
	autoVEC dp;
	double lineSearchGradient;
	integer lineSearchMaxNumOfIterations;
	double gr0, gropt, df, alplim, alpha, dalpha, alphamin;
	autoVEC pc;   /* position of current point */
	autoVEC gc;   /* gradient of current point */
	autoVEC g0;   /* gradient at beginning of line search */
	autoVEC s;   /* search direction for line search */
	autoVEC srst;   /* search direction for first iteration after restart */
	autoVEC grst;   /* gradient for first iteration after restart */
	double fc, grc, fch, gr2s, temp, grs, beta, gcg0;
	double gamma, gamma_in, f0, gsq, gopt_sq;
	integer lineSearch_iteration, flag, again, one_up, restart;
	bool restart_flag;

	void v_minimize ()
		override;
	void v_reset ()
		override;
};

autoVDSmagtMinimizer VDSmagtMinimizer_create (integer dimension, Daata object, double (*func) (Daata object, VEC const& p), void (*dfunc) (Daata object, VEC const& p, VEC const& dp));

#endif /* _Minimizer_h_ */
