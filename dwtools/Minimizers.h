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


#ifndef _Thing_h_
	#include "Thing.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

/*********** deferred class Minimizer **********************************/

#define Minimizer_members Thing_members							\
    long nParameters;	/* the number of parameters */			\
    double *p;			/* the parameters */					\
    double minimum;		/* current minimum */					\
    double *history;	/* previous minima */					\
    double tolerance;	/* stop criterium */					\
    Any object;		/* reference to the object that uses this Minimizer */	\
    long funcCalls;		/* the number of times 'func' has been called */	\
    long success;		/* indicates whether I'm done */					\
    long start;			/* start iteration series */						\
    long maxNumOfIterations; /* the current maximum number of iterations */	\
    long iteration;     /* the total number of iterations */				\
    int (*after) (I, Any aclosure); /* to be called after each iteration */	\
    Any aclosure;															\
    Any gmonitor;		/* graphics to monitor the minimization process */
#define Minimizer_methods Thing_methods				\
    int (*minimize) (I);  /* does the work */		\
    void (*reset) (I); /* reset the minimizer */	\
    void (*setParameters) (I, Any parameters);

class_create (Minimizer, Thing);
 
int Minimizer_init (I, long nParameters, Any object);
/*
	Preconditions:
		nParameters > 0;
	Postconditions:
		if (gmonitor) gmonitor != NULL
*/

void Minimizer_reset (I, const double guess[]);
/* reset the start values for the minimizer
 * Preconditions:
 *    guess != NULL;
 * Post conditions:
 *    p[] = guess[];
 *    my minimum = 1.0e30;
 *    success = maxNumOfIterations = iteration = funcCalls = 0;
 *    reset (me);
 */

void Minimizer_setAfterEachIteration (I, int (*after) (I, Any aclosure), 
	Any aclosure);
/* set the procedure that is executed after each iteration. */

void Minimizer_setParameters (I, Any parameters); /* for inheritors */

int Minimizer_minimize (I, long maxNumOfIterations, double tolerance,
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

int Minimizer_minimizeManyTimes (I, long numberOfTimes, long maxIterationsPerTime,
	double tolerance);
   
void Minimizer_drawHistory (I, Any graphics, long itmin, long itmax,
    double minimum, double maximum, int garnish);

double Minimizer_getMinimum (I);

/********** deferred class LineMinimizer ************************************/

#define LineMinimizer_members Minimizer_members						\
	/* the function to be minimized */		\
    double (*func) (Any object, const double p[]);	\
	double maxLineStep;	/*maximum step in line search direction */	\
    double *direction;	/* search direction vector */				\
    double *ptry;		/* point in search direction */
#define LineMinimizer_methods Minimizer_methods \
    void (*linmin) (I, double p[], double fp, double direction[], double *fret);	/* line minimization */
class_create (LineMinimizer, Minimizer);

int LineMinimizer_init (I, long nParameters, Any object, double (*func) 
	(Any object, const double p[]));


/******************  class SteepestDescentMinimizer**************************/

typedef struct structSteepestDescentMinimizer_parameters {
	double eta, momentum;
} *SteepestDescentMinimizer_parameters;

#define SteepestDescentMinimizer_members Minimizer_members	\
	double eta, momentum;									\
    double (*func) (Any object, const double p[]);			\
    void  (*dfunc) (Any object, const double p[], double dp[]);	
	/* calculates gradient at position p */
#define SteepestDescentMinimizer_methods Minimizer_methods
class_create (SteepestDescentMinimizer, Minimizer);

Any SteepestDescentMinimizer_create (long nParameters, Any object, double (*func) 
	(Any object, const double p[]), void (*dfunc) (Any object, const double p[],
	double dp[]));
	 

/**********  class VDSmagtMinimizer ********************************/

typedef struct structVDSmagtMinimizer_parameters {
	double lineSearchGradient;
	long lineSearchMaxNumOfIterations;
} *VDSmagtMinimizer_parameters;

#define VDSmagtMinimizer_members Minimizer_members                          \
    double (*func) (Any object, const double p[]);							\
	void  (*dfunc) (Any object, const double p[], double dp[]);             \
	double *dp;																\
    double lineSearchGradient;      	                                    \
    long lineSearchMaxNumOfIterations;                                      \
    double gr0, gropt, df, alplim, alpha, dalpha, alphamin;					\
    double *pc;	/* position of current point */								\
    double *gc;	/* gradient of current point */								\
    double *g0;	/* gradient at beginning of line search */					\
    double *s;	/* search direction for line search */						\
    double *srst;/* search direction for first iteration after restart */	\
    double *grst; /* gradient for first iteration after restart */			\
    double fc, grc, fch, gr2s, temp, grs, beta, gcg0;                       \
    double gamma, gamma_in, f0, gsq, gopt_sq;                               \
    long lineSearch_iteration, flag, again, one_up, restart;		    	\
    long restart_flag;
#define VDSmagtMinimizer_methods Minimizer_methods
class_create (VDSmagtMinimizer, Minimizer);

Any VDSmagtMinimizer_create (long dimension, void *object, double (*func) 
	(Any object, const double p[]), void (*dfunc) (Any object, const double p[],
	double dp[]));

#ifdef __cplusplus
	}
#endif

#endif /* _Minimizer_h_ */
