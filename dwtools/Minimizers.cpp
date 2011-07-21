/* Minimizers.cpp
 *
 * Copyright (C) 2001-2011 David Weenink
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
 David Weenink, 20011016
 djmw 20011016 removed some causes for compiler warnings
 djmw 20030205 Latest modification
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040421 Bug removed: delayed message when learning was interrupted by user.
 djmw 20080122 float -> double
  djmw 20110304 Thing_new
*/

#include "NUM2.h"
#include "Graphics.h"
#include "Minimizers.h"

#define SIGN(x,s) ((s) < 0 ? -fabs (x) : fabs(x))
#define GOLD  1.618034
#define CGOLD 0.3819660
#define ITMAX 100.0
#define TINY 1.0e-20
#define ZEPS 1.0e-10
#define EPS 1.0e-10
#define TOL 2e-4
#define SHFT(a, b, c, d)	(a) = (b); (b) = (c); (c) = (d);
#define MOV3(a, b, c, d, e, f)	(a) = (d); (b) = (e); (c) = (f);
#define FUNC1(fx, x) for (i=1; i <= my nParameters; i++) my ptry[i] = p[i] + (x) * direction[i]; \
	(fx) = my func (my object, my ptry);
#define DFUNC1(df, x) for (i=1; i <= my nParameters; i++) my ptry[i] = p[i] + (x) * direction[i]; \
	my dfunc (my object, my ptry, my dp); for (df=0, i=1; i <= my nParameters; i++) df += my dp[i] * direction[i];

#undef our
#define our ((Minimizer_Table) my methods) ->   // tijdelijk

static void classMinimizer_after (I, Any aclosure)
{
    iam (Minimizer);
	(void) aclosure;

    if (my success || ! my gmonitor) return;

    if (my start == 1)
    {
    	wchar_t s[35];
    	Minimizer_drawHistory (me, my gmonitor, 0, my maxNumOfIterations, 0, 1.1 * my history[1], 1);
    	swprintf (s, 35, L"Dimension of search space: %6ld", my nParameters);
    	Graphics_textTop (my gmonitor, 0, s);
    }
    Graphics_setInner (my gmonitor);
    Graphics_line (my gmonitor, my iteration, my history[my iteration],
    	my iteration, my history[my iteration]);
    Graphics_unsetInner (my gmonitor);
	Melder_monitor6 ((double) (my iteration) / my maxNumOfIterations,
		L"Iterations: ", Melder_integer (my iteration),
		L", Function calls: ", Melder_integer (my funcCalls),
		L", Cost: ", Melder_double (my minimum));
}

static void classMinimizer_info (I) {(void) void_me;}

static void classMinimizer_destroy (I)
{
    iam (Minimizer);
    NUMdvector_free (my p, 1);
    NUMdvector_free (my history, 1);
    inherited (Minimizer) destroy (me);
}

static void classMinimizer_reset (I) {(void) void_me;}

static void classMinimizer_minimize (I) { (void) void_me; }

static void classMinimizer_setParameters (I, Any parameters)
{
	(void) void_me; (void) parameters;
}

class_methods (Minimizer, Thing)
   class_method_local (Minimizer, destroy)
   class_method_local (Minimizer, info)
   class_method_local (Minimizer, minimize)
   class_method_local (Minimizer, reset)
   class_method_local (Minimizer, setParameters)
class_methods_end

void Minimizer_init (I, long nParameters, Data object)
{
	iam (Minimizer);
	my nParameters = nParameters;
	my p = NUMvector<double> (1, nParameters);
	my object = object;
	my minimum = 1.0e30;
	my after = classMinimizer_after;
	Minimizer_reset (me, 0); /* added 27/11/97 */
}

void Minimizer_setParameters (Minimizer me, Any parameters)
{
	if (our setParameters) our setParameters (me, parameters);
}
static void monitor_off (Minimizer me)
{
	Melder_monitor1 (1.1, NULL);
	if (my gmonitor)
	{
		Graphics_clearWs (my gmonitor); // DON'T forget (my gmonitor)
		my gmonitor = NULL;
	}
}

void Minimizer_minimize (Minimizer me, long maxNumOfIterations, double tolerance, int monitor)
{
	try {

		my tolerance = tolerance;
		if (maxNumOfIterations <= 0) return;

		if (my iteration + maxNumOfIterations > my maxNumOfIterations)
		{
			double *history;
			my maxNumOfIterations += maxNumOfIterations;
			if (my history) my history++; /* arrays start at 1 !! */
			history = (double *) Melder_realloc (my history, my maxNumOfIterations *
				sizeof(double));
			my history = --history; /* arrays start at 1 !! */
		}
		if (monitor) my gmonitor = (Graphics) Melder_monitor1 (0.0, L"Starting...");
		my start = 1; /* for my after() */
		our minimize (me);
		if (monitor) monitor_off (me);
		if (my success) Melder_casual("Minimizer_minimize: minimum %f reached \n"
			"after %ld iterations and %ld function calls.", my minimum,
			my iteration, my funcCalls);
	} catch (MelderError) { if (monitor) monitor_off (me); } // temporarily until better monitor facilities
}

void Minimizer_minimizeManyTimes (Minimizer me, long numberOfTimes, long maxIterationsPerTime, double tolerance)
{
	double fopt = my minimum;
	int monitorSingle = numberOfTimes == 1;

	autoNUMvector<double> popt (NUMvector_copy<double> (my p, 1, my nParameters), 1);

	if (! monitorSingle) Melder_progress1 (0.0, L"Minimize many times");
	/* on first iteration start with current parameters 27/11/97 */
	for (long i = 1; i <= numberOfTimes; i++)
	{
		Minimizer_minimize (me, maxIterationsPerTime, tolerance, monitorSingle);
		Melder_casual("Current %ld: minimum = %.17g", i, my minimum);
		if (my minimum < fopt)
		{
			NUMdvector_copyElements (my p, popt.peek(), 1, my nParameters);
			fopt = my minimum;
		}
		Minimizer_reset (me, 0);
		if (! monitorSingle) {
			try { Melder_progress3 ((double)i / numberOfTimes, Melder_integer (i), L" from ",
				Melder_integer (numberOfTimes)); therror
			} catch (MelderError) {
				Melder_clearError ();   // interrurpt, no error
				break;
			}
		}
	}
	if (! monitorSingle) Melder_progress1 (1.0, 0);
	Minimizer_reset (me, popt.peek());
}

void Minimizer_setAfterEachIteration (Minimizer me, void (*after) (I, Any aclosure), Any aclosure)
{
    my after = after;
    my aclosure = aclosure;
}

void Minimizer_reset (Minimizer me, const double guess[])
{
    if (guess)
    {
    	for (long i = 1; i <= my nParameters; i++)
		{
			my p[i] = guess[i];
		}
    }
    else
    {
    	for (long i = 1; i <= my nParameters; i++)
		{
			my p[i] = NUMrandomUniform (-1, 1);
		}
    }
	/*
		Don't use NUMdvector_free: realloc in Minimizer_minimize
	*/
    if (my history != 0)
    {
    	my history++;
		Melder_free (my history);
    }
    my maxNumOfIterations = my success = my funcCalls = my iteration = 0;
    my minimum = 1.0e38;
    our reset (me);
}

void Minimizer_drawHistory (Minimizer me, Graphics g, long iFrom, long iTo, double hmin,
    double hmax, int garnish)
{
    if (my history == 0) return;
	autoNUMvector<double> history (1, my iteration);
    for (long i = 1; i <= my iteration; i++)
	{
		history[i] = my history[i];
	}
    if (iTo <= iFrom)
	{
		iFrom = 1; iTo = my iteration;
	}
    long itmin = iFrom, itmax = iTo;
    if (itmin < 1) itmin = 1;
    if (itmax > my iteration) itmax = my iteration;
    if (hmax <= hmin)
	{
		NUMdvector_extrema (history.peek(), itmin, itmax, & hmin, & hmax);
	}
    if (hmax <= hmin)
	{
		hmin -= 0.5 * fabs (hmin);
		hmax += 0.5 * fabs (hmax);
	}
    Graphics_setInner (g);
    Graphics_setWindow (g, iFrom, iTo, hmin, hmax);
    Graphics_function (g, history.peek(), itmin, itmax, itmin, itmax);
    Graphics_unsetInner (g);
    if (garnish)
    {
    	Graphics_drawInnerBox (g);
   		Graphics_textBottom (g, 1, L"Number of iterations");
		Graphics_marksBottom (g, 2, 1, 1, 0);
    	Graphics_marksLeft (g, 2, 1, 1, 0);
    }
}

double Minimizer_getMinimum (Minimizer me)
{
	return my minimum;
}

/**************  class SteepestDescentMinimizer **********************/

static void classSteepestDescentMinimizer_minimize (I)
{
	iam (SteepestDescentMinimizer);
	autoNUMvector<double> dp  (1, my nParameters);
	autoNUMvector<double> dpp (1, my nParameters);
	double fret = my func (my object, my p);
	while (my iteration < my maxNumOfIterations)
	{
		my dfunc (my object, my p, dp.peek());
		for (long i = 1; i <= my nParameters; i++)
		{
			dpp[i] = - my eta * dp[i] + my momentum * dpp[i];
			my p[i] += dpp[i];
		}
		my history[++my iteration] = my minimum = my func (my object, my p);
		my success = 2.0 * fabs (fret - my minimum) < my tolerance * (fabs (fret) + fabs (my minimum));
		if (my after)
		{
			try { my after (me, my aclosure);
			} catch (MelderError) {
				Melder_casual ("Interrupted after %ld iterations.",my iteration);
				Melder_clearError ();
				break;
			}
		}
		if (my success) break;
		fret = my minimum;
	}
}

static void classSteepestDescentMinimizer_setParameters (I, Any p)
{
	iam (SteepestDescentMinimizer);
	if (p)
	{
		SteepestDescentMinimizer_parameters thee = (SteepestDescentMinimizer_parameters) p;
		my eta = thy eta;
		my momentum = thy momentum;
	}
	else
	{
		my eta = 0.1; my momentum = 0.9;
	}
}

class_methods (SteepestDescentMinimizer, Minimizer)
   class_method_local (SteepestDescentMinimizer, minimize)
   class_method_local (SteepestDescentMinimizer, setParameters)
class_methods_end

SteepestDescentMinimizer SteepestDescentMinimizer_create (long nParameters, Data object,
	double (*func) (Data object, const double p[]),
	 void (*dfunc) (Data object, const double p[], double dp[]))
{
	try {
		autoSteepestDescentMinimizer me = Thing_new (SteepestDescentMinimizer);
		Minimizer_init (me.peek(), nParameters, object);
		my func = func;
		my dfunc = dfunc;
		return me.transfer();
	} catch (MelderError) { Melder_throw ("SteepestDescentMinimizer not created."); }
}

/*****************  class VDSmagtMinimizer ******************************/

static void classVDSmagtMinimizer_minimize (I)
{
    iam (VDSmagtMinimizer);
    int decrease_direction_found = 1, iteration = 1;
    double rtemp, rtemp2;

	// df is estimate of function reduction obtainable during line search
	// restart = 2 => line search in direction of steepest descent
	// restart = 1 => line search with Powell-restart.
	// flag = 1 => no decrease in function value during previous line search;
	// flag = 2 => line search did not decrease gradient
    //    OK; must restart

	if (my restart_flag)
	{
    	my minimum = my func (my object, my p);
    	my dfunc (my object, my p, my dp);
 		my df = my minimum;
    	my restart = 2;
		my one_up = my flag = 0;
		my gcg0 = my gopt_sq = 0.0;
	}
	my restart_flag = 1;
    while (++my iteration <= my maxNumOfIterations)
    {
		if (my flag & 1)
		{
	    	if (my one_up)
			{
				decrease_direction_found = 0;
				my iteration--;
				break;
			}
	    	else
			{
				my one_up = 1;
			}
		}
		else
		{
			my one_up = 0;
		}
		if (my flag & 2)
		{
			my restart = 2; /* my flag & 1 ??? */
		}
		else if (fabs ((double) my gcg0) > 0.2 * my gopt_sq)
		{
			my restart = 1;
		}
		if (my restart == 0)
		{
			rtemp = rtemp2 = 0.0;
	    	for (long i = 1; i <= my nParameters; i++)
	    	{
				rtemp += my gc[i] * my grst[i];
				rtemp2 += my gc[i] * my srst[i];
	    	}
	    	my gamma = rtemp / my gamma_in;
	    	if (fabs (my beta * my gropt - my gamma * rtemp2) > 0.2*my gopt_sq)
			{
				my restart = 1;
			}
	    	else
			{
				for (long i = 1; i <= my nParameters; i++)
				{
	    			my s[i] = my beta * my s[i] + my gamma * my srst[i] - my gc[i];
				}
			}
		}
		if (my restart == 2)
		{
	    	for (long i = 1; i <= my nParameters; i++) my s[i] = - my dp[i];
	    	my restart = 1;
		}
		else if (my restart == 1)
		{
	    	my gamma_in = my gropt - my gr0;
	    	for (long i = 1; i <= my nParameters; i++)
	    	{
				my srst[i] = my s[i];
				my s[i] = my beta * my s[i] - my gc[i];
				my grst[i] = my gc[i] - my g0[i];
	    	}
	    	my restart = 0;
		}

		// Begin line search
		// lineSearch_iteration = #iterations during current line search

		my flag = 0;
		my lineSearch_iteration = 0;
		rtemp = 0.0;
		for (long i = 1; i <= my nParameters; i++)
		{
	    	rtemp += my dp[i] * my s[i];
	    	my g0[i] = my dp[i];
		}
		my gr0 = my gropt = rtemp;
		if (iteration == 1) my alphamin = fabs (my df / my gropt);
		if (my gr0 > 0)
		{
	    	my flag = 1;
	    	my restart = 2;
			continue;
		}
		my f0 = my minimum;

		// alpha = length of step along line;
		// dalpha = change in alpha
		// alphamin = position of min along line

		my alplim = -1;
		my again = -1;
		rtemp = fabs (my df / my gropt);
		my dalpha = my alphamin < rtemp ? my alphamin : rtemp;
		my alphamin = 0;
		do
		{
	    	do
			{
				if (my lineSearch_iteration)
				{
		    		if (! (my fch == 0))
					{
						my gr2s += (my temp + my temp) / my dalpha;
					}

		    		if (my alplim < -0.5)
					{
						my dalpha = 9.0 * my alphamin;
					}
		    		else
					{
						my dalpha = 0.5 * (my alplim - my alphamin);
					}

		    		my grs = my gropt + my dalpha * my gr2s;
		    		if (my gropt * my grs < 0)
					{
						my dalpha *= my  gropt / (my gropt - my grs);
					}
				}
				my alpha = my alphamin + my dalpha;
				for (long i = 1; i <= my nParameters; i++)
				{
					my pc[i] = my p[i] + my dalpha * my s[i];
				}
    			my fc = my func (my object, my pc);
    			my dfunc (my object, my pc, my gc);
				iteration++;
				my lineSearch_iteration++;
				my gsq = my grc = 0.0;
				for (long i = 1; i <= my nParameters; i++)
				{
		    		my gsq += my gc[i] * my gc[i];
		    		my grc += my gc[i] * my s[i];
				}
				my fch = my fc - my minimum;
				my gr2s = (my grc - my gropt) / my dalpha;
				my temp = (my fch + my fch) / my dalpha - my grc - my gropt;
				if ((my fc < my minimum) ||
					((my fc == my minimum) && (my grc / my gropt > -1)))
				{
		    		double *tmp;
		    		my gopt_sq = my gsq;
		    		my history[my iteration] = my minimum = my fc;
		    		tmp = my p; my p = my pc; my pc = tmp;
		    		tmp = my dp; my dp = my gc; my gc = tmp;
		    		if (my grc * my gropt <= 0) my alplim = my alphamin;
		    		my alphamin = my alpha;
		    		my gropt = my grc;
		    		my dalpha = -my dalpha;
		    		my success = my gsq < my tolerance;
					if (my after)
					{
						try { my after (me, my aclosure);
						} catch (MelderError) {
							Melder_casual ("Interrupted after %ld iterations.", my iteration);
							Melder_clearError ();
							break;
						}
					}
					if (my success) return;
		    		if (fabs (my gropt / my gr0) < my lineSearchGradient) break;
				}
				else
				{
					my alplim = my alpha;
				}
	    	} while (my lineSearch_iteration
				<= my lineSearchMaxNumOfIterations);

	    	my fc = my history[my iteration] = my minimum;
			rtemp = 0.0;
	    	for (long i = 1; i <= my nParameters; i++)
	    	{
				my pc[i] = my p[i];
				my gc[i] = my dp[i];
				rtemp += my gc[i] * my g0[i];
	    	}
	    	my gcg0 = rtemp;
	    	if (fabs(my gropt - my gr0) > my tolerance)
	    	{
				my beta = (my gopt_sq - my gcg0) / (my gropt - my gr0);
				if (fabs (my beta * my gropt) < 0.2 * my gopt_sq) break;
	    	}
	    	my again++;
	    	if (my again > 0) my flag += 2;
		} while (my flag < 1);

		if (my f0 <= my minimum) my flag += 1;
		my df = my gr0 * my alphamin;
	}
	if (my iteration > my maxNumOfIterations)
	{
		my iteration = my maxNumOfIterations;
	}
    if (decrease_direction_found) my restart_flag = 0;
}

static void classVDSmagtMinimizer_destroy (I)
{
    iam (VDSmagtMinimizer);
    NUMvector_free (my dp, 1);
    NUMvector_free (my pc, 1);
    NUMvector_free (my gc, 1);
    NUMvector_free (my g0, 1);
    NUMvector_free (my s, 1);
    NUMvector_free (my srst, 1);
    NUMvector_free (my grst, 1);
    inherited (VDSmagtMinimizer) destroy (me);
}

static void classVDSmagtMinimizer_reset (I)
{
    iam (VDSmagtMinimizer);
    my restart_flag = 1;
}

static void classVDSmagtMinimizer_setParameters (I, Any parameters)
{
	iam (VDSmagtMinimizer);
	if (parameters)
	{
		VDSmagtMinimizer_parameters p = (VDSmagtMinimizer_parameters) parameters;
    	my lineSearchGradient = p -> lineSearchGradient;
    	my lineSearchMaxNumOfIterations = p -> lineSearchMaxNumOfIterations;
    }
}

class_methods (VDSmagtMinimizer, Minimizer)
   class_method_local (VDSmagtMinimizer, minimize)
   class_method_local (VDSmagtMinimizer, destroy)
   class_method_local (VDSmagtMinimizer, reset)
   class_method_local (VDSmagtMinimizer, setParameters)
class_methods_end

VDSmagtMinimizer VDSmagtMinimizer_create (long nParameters, Data object, double (*func) (Data object, const double x[]),
    void (*dfunc) (Data object, const double x[], double dx[]))
{
	try {
		autoVDSmagtMinimizer me = Thing_new (VDSmagtMinimizer);
		Minimizer_init (me.peek(), nParameters, object);
		my dp = NUMvector<double> (1, nParameters);
		my pc = NUMvector<double> (1, nParameters);
		my gc = NUMvector<double> (1, nParameters);
		my g0 = NUMvector<double> (1, nParameters);
		my s = NUMvector<double> (1, nParameters);
		my srst = NUMvector<double> (1, nParameters);
		my grst = NUMvector<double> (1, nParameters);
    	my func = func;
		my dfunc = dfunc;
    	my lineSearchGradient = 0.9;
    	my lineSearchMaxNumOfIterations = 5;
		return me.transfer();
	} catch (MelderError) { Melder_throw ("VDSmagtMinimizer not created."); }
}

/************ class LineMinimizer *******************************/

static void classLineMinimizer_destroy (I)
{
    iam (LineMinimizer);
    NUMvector_free (my ptry, 1);
    NUMvector_free (my direction, 1);
    inherited (LineMinimizer) destroy (me);
}

class_methods (LineMinimizer, Minimizer)
   class_method_local (LineMinimizer, destroy)
class_methods_end

void LineMinimizer_init (I, long nParameters, Data object, double (*func)(Data, const double []))
{
	iam (LineMinimizer);
	Minimizer_init (me, nParameters, object);
	my direction = NUMvector<double> (1, nParameters);
	my ptry = NUMvector<double> (1, nParameters);
	my func = func;
	my maxLineStep = 100;
}

/* End of file Minimizers.c 657*/
