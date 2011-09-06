#ifndef _Minimizers_h_
#define _Minimizers_h_
/* Minimizers.h
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
	// new data:
	public:
		long nParameters;	/* the number of parameters */
		double *p;			/* the parameters */
		double minimum;		/* current minimum */
		double *history;	/* previous minima */
		double tolerance;	/* stop criterium */
		Data object;		/* reference to the object that uses this Minimizer */
		long funcCalls;		/* the number of times 'func' has been called */
		long success;		/* indicates whether I'm done */
		long start;			/* start iteration series */
		long maxNumOfIterations; /* the current maximum number of iterations */
		long iteration;     /* the total number of iterations */
		void (*after) (I, Any aclosure); /* to be called after each iteration */
		Any aclosure;
		Graphics gmonitor;		/* graphics to monitor the minimization process */
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_info () { }
	// new methods:
		virtual void v_minimize () { }  /* does the work */
		virtual void v_reset () { } /* reset the minimizer */
		virtual void v_setParameters (Any parameters) { (void) parameters; }
};

void Minimizer_init (I, long nParameters, Data object);
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

void Minimizer_setAfterEachIteration (Minimizer me, int (*after) (I, Any aclosure),
	Any aclosure);
/* set the procedure that is executed after each iteration. */

void Minimizer_setParameters (Minimizer me, Any parameters); /* for inheritors */

void Minimizer_minimize (Minimizer me, long maxNumOfIterations, double tolerance,
	int monitor);
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

void Minimizer_minimizeManyTimes (Minimizer me, long numberOfTimes, long maxIterationsPerTime,
	double tolerance);

void Minimizer_drawHistory (Minimizer me, Graphics g, long itmin, long itmax,
    double minimum, double maximum, int garnish);

double Minimizer_getMinimum (Minimizer me);

/********** deferred class LineMinimizer ************************************/

Thing_define (LineMinimizer, Minimizer) {
	// new data:
	public:
		/* the function to be minimized */
		double (*func) (Data object, const double p[]);
		double maxLineStep;	/*maximum step in line search direction */
		double *direction;	/* search direction vector */
		double *ptry;		/* point in search direction */
	// overridden methods:
		virtual void v_destroy ();
	// new methods:
		//virtual void v_linmin (double p[], double fp, double direction[], double *fret);	 // David, is dit correct? ja
};

void LineMinimizer_init (I, long nParameters, Data object, double (*func)
	(Data object, const double p[]));


/******************  class SteepestDescentMinimizer**************************/

typedef struct structSteepestDescentMinimizer_parameters {
	double eta, momentum;
} *SteepestDescentMinimizer_parameters;

Thing_define (SteepestDescentMinimizer, Minimizer) {
	// new data:
	public:
		double eta, momentum;
		double (*func) (Data object, const double p[]);
		void  (*dfunc) (Data object, const double p[], double dp[]);
		/* calculates gradient at position p */
	// overridden methods:
		virtual void v_minimize ();
		virtual void v_setParameters (Any parameters);
};

SteepestDescentMinimizer SteepestDescentMinimizer_create (long nParameters, Data object, double (*func)
	(Data object, const double p[]), void (*dfunc) (Data object, const double p[],
	double dp[]));


/**********  class VDSmagtMinimizer ********************************/

typedef struct structVDSmagtMinimizer_parameters {
	double lineSearchGradient;
	long lineSearchMaxNumOfIterations;
} *VDSmagtMinimizer_parameters;

Thing_define (VDSmagtMinimizer, Minimizer) {
	// new data:
	public:
		double (*func) (Data object, const double p[]);
		void  (*dfunc) (Data object, const double p[], double dp[]);
		double *dp;
		double lineSearchGradient;
		long lineSearchMaxNumOfIterations;
		double gr0, gropt, df, alplim, alpha, dalpha, alphamin;
		double *pc;	/* position of current point */
		double *gc;	/* gradient of current point */
		double *g0;	/* gradient at beginning of line search */
		double *s;	/* search direction for line search */
		double *srst;/* search direction for first iteration after restart */
		double *grst; /* gradient for first iteration after restart */
		double fc, grc, fch, gr2s, temp, grs, beta, gcg0;
		double gamma, gamma_in, f0, gsq, gopt_sq;
		long lineSearch_iteration, flag, again, one_up, restart;
		long restart_flag;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_minimize ();
		virtual void v_reset ();
		virtual void v_setParameters (Any parameters);
};

VDSmagtMinimizer VDSmagtMinimizer_create (long dimension, Data object, double (*func)
	(Data object, const double p[]), void (*dfunc) (Data object, const double p[],
	double dp[]));

#endif /* _Minimizer_h_ */
