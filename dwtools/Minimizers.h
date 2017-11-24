#ifndef _Minimizers_h_
#define _Minimizers_h_
/* Minimizers.h
 *
 * Copyright (C) 1993-2017 David Weenink, 2015,2017 Paul Boersma
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

/*
 djmw 20020813 GPL header
 djmw 20030701 Removed non-GPL minimizations
 djmw 20110414 Latest modification.
*/

#include "Data.h"
#include "Graphics.h"

/*********** deferred class Minimizer **********************************/

Thing_define (Minimizer, Thing) {
	integer nParameters;	/* the number of parameters */
	double *p;			/* the parameters */
	double minimum;		/* current minimum */
	double *history;	/* previous minima */
	double tolerance;	/* stop criterium */
	Daata object;		/* reference to the object that uses this Minimizer */
	integer funcCalls;		/* the number of times 'func' has been called */
	integer success;		/* indicates whether I'm done */
	integer start;			/* start iteration series */
	integer maxNumOfIterations; /* the current maximum number of iterations */
	integer iteration;     /* the total number of iterations */
	void (*afterHook) (Minimizer me, Thing boss); /* to be called after each iteration */
	Thing afterBoss;
	Graphics gmonitor;		/* graphics to monitor the minimization process */

	void v_destroy () noexcept
		override;
	void v_info ()
		override { }

	virtual void v_minimize () { }  /* does the work */
	virtual void v_reset () { } /* reset the minimizer */
};

void Minimizer_init (Minimizer me, integer nParameters, Daata object);
/*
	Preconditions:
		nParameters > 0;
	Postconditions:
		if (gmonitor) gmonitor != NULL
*/

void Minimizer_reset (Minimizer me, const double guess[]);
/* reset the start values for the minimizer
 * Preconditions:
 *    guess != NULL;
 * Post conditions:
 *    p[] = guess[];
 *    my minimum = 1.0e30;
 *    success = maxNumOfIterations = iteration = funcCalls = 0;
 *    reset (me);
 */

void Minimizer_minimize (Minimizer me, integer maxNumOfIterations, double tolerance, int monitor);
/* Minimizes during maximally maxNumOfIterations. The gmonitor is initialized
 * before minimization and cleared afterwards.
 * Preconditions:
 *    maxNumOfIterations >= 0;
 *    tolerance > 0;
 * Postconditions:
 *    if (reset) Minimizer_reset called with xopt as initial guess.
 *    after each function call: funcCalls++
 *    after each iteration: iteration++
 */

void Minimizer_minimizeManyTimes (Minimizer me, integer numberOfTimes, integer maxIterationsPerTime, double tolerance);

void Minimizer_drawHistory (Minimizer me, Graphics g, integer itmin, integer itmax, double minimum, double maximum, int garnish);

double Minimizer_getMinimum (Minimizer me);

/********** deferred class LineMinimizer ************************************/

Thing_define (LineMinimizer, Minimizer) {
	/* the function to be minimized */
	double (*func) (Daata object, const double p[]);
	double maxLineStep;	/*maximum step in line search direction */
	double *direction;	/* search direction vector */
	double *ptry;		/* point in search direction */

	void v_destroy () noexcept
		override;

	//virtual void v_linmin (double p[], double fp, double direction[], double *fret);	 // David, is dit correct? ja
};

void LineMinimizer_init (LineMinimizer me, integer nParameters, Daata object, double (*func) (Daata object, const double p[]));

/******************  class SteepestDescentMinimizer**************************/

typedef struct structSteepestDescentMinimizer_parameters {
	double eta, momentum;
} *SteepestDescentMinimizer_parameters;

Thing_define (SteepestDescentMinimizer, Minimizer) {
	double eta, momentum;
	double (*func) (Daata object, const double p[]);
	void  (*dfunc) (Daata object, const double p[], double dp[]);
	/* calculates gradient at position p */

	void v_minimize ()
		override;
};

autoSteepestDescentMinimizer SteepestDescentMinimizer_create (integer nParameters, Daata object, double (*func) (Daata object, const double p[]), void (*dfunc) (Daata object, const double p[], double dp[]));


/**********  class VDSmagtMinimizer ********************************/

typedef struct structVDSmagtMinimizer_parameters {
	double lineSearchGradient;
	integer lineSearchMaxNumOfIterations;
} *VDSmagtMinimizer_parameters;

Thing_define (VDSmagtMinimizer, Minimizer) {
	double (*func) (Daata object, const double p[]);
	void  (*dfunc) (Daata object, const double p[], double dp[]);
	double *dp;
	double lineSearchGradient;
	integer lineSearchMaxNumOfIterations;
	double gr0, gropt, df, alplim, alpha, dalpha, alphamin;
	double *pc;	/* position of current point */
	double *gc;	/* gradient of current point */
	double *g0;	/* gradient at beginning of line search */
	double *s;	/* search direction for line search */
	double *srst;/* search direction for first iteration after restart */
	double *grst; /* gradient for first iteration after restart */
	double fc, grc, fch, gr2s, temp, grs, beta, gcg0;
	double gamma, gamma_in, f0, gsq, gopt_sq;
	integer lineSearch_iteration, flag, again, one_up, restart;
	integer restart_flag;

	void v_destroy () noexcept
		override;
	void v_minimize ()
		override;
	void v_reset ()
		override;
};

autoVDSmagtMinimizer VDSmagtMinimizer_create (integer dimension, Daata object, double (*func) (Daata object, const double p[]), void (*dfunc) (Daata object, const double p[], double dp[]));

#endif /* _Minimizer_h_ */
