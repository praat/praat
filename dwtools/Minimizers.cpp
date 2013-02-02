/* Minimizers.cpp
 *
 * Copyright (C) 2001-2013 David Weenink
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

Thing_implement (Minimizer, Thing, 0);

void structMinimizer :: v_destroy () {
	NUMvector_free (p, 1);
	NUMvector_free (history, 1);
	Minimizer_Parent :: v_destroy ();
}

static void classMinimizer_after (I, Any aclosure) {
	iam (Minimizer);
	(void) aclosure;

	if (my success || ! my gmonitor) {
		return;
	}

	if (my start == 1) {
		wchar_t s[35];
		Minimizer_drawHistory (me, my gmonitor, 0, my maxNumOfIterations, 0, 1.1 * my history[1], 1);
		swprintf (s, 35, L"Dimension of search space: %6ld", my nParameters);
		Graphics_textTop (my gmonitor, 0, s);
	}
	Graphics_setInner (my gmonitor);
	Graphics_line (my gmonitor, my iteration, my history[my iteration],
	               my iteration, my history[my iteration]);
	Graphics_unsetInner (my gmonitor);
	Melder_monitor ( (double) (my iteration) / my maxNumOfIterations,
	                  L"Iterations: ", Melder_integer (my iteration),
	                  L", Function calls: ", Melder_integer (my funcCalls),
	                  L", Cost: ", Melder_double (my minimum));
}

void Minimizer_init (I, long nParameters, Data object) {
	iam (Minimizer);
	my nParameters = nParameters;
	my p = NUMvector<double> (1, nParameters);
	my object = object;
	my minimum = 1.0e30;
	my after = classMinimizer_after;
	Minimizer_reset (me, 0); /* added 27/11/97 */
}

void Minimizer_setParameters (Minimizer me, Any parameters) {
	my v_setParameters (parameters);
}
static void monitor_off (Minimizer me) {
	Melder_monitor (1.1, NULL);
	if (my gmonitor) {
		Graphics_clearWs (my gmonitor); // DON'T forget (my gmonitor)
		my gmonitor = NULL;
	}
}

void Minimizer_minimize (Minimizer me, long maxNumOfIterations, double tolerance, int monitor) {
	try {

		my tolerance = tolerance;
		if (maxNumOfIterations <= 0) {
			return;
		}

		if (my iteration + maxNumOfIterations > my maxNumOfIterations) {
			double *history;
			my maxNumOfIterations += maxNumOfIterations;
			if (my history) {
				my history++;    /* arrays start at 1 !! */
			}
			history = (double *) Melder_realloc (my history, my maxNumOfIterations *
			                                     sizeof (double));
			my history = --history; /* arrays start at 1 !! */
		}
		if (monitor) {
			my gmonitor = (Graphics) Melder_monitor (0.0, L"Starting...");
		}
		my start = 1; /* for my after() */
		my v_minimize ();
		if (monitor) {
			monitor_off (me);
		}
		if (my success) Melder_casual ("Minimizer_minimize: minimum %f reached \n"
			                               "after %ld iterations and %ld function calls.", my minimum,
			                               my iteration, my funcCalls);
	} catch (MelderError) {
		if (monitor) {
			monitor_off (me);    // temporarily until better monitor facilities
		}
		Melder_clearError(); // memory error in history mechanism is not fatal
	}
}

void Minimizer_minimizeManyTimes (Minimizer me, long numberOfTimes, long maxIterationsPerTime, double tolerance) {
	double fopt = my minimum;
	int monitorSingle = numberOfTimes == 1;

	autoNUMvector<double> popt (NUMvector_copy<double> (my p, 1, my nParameters), 1);

	if (! monitorSingle) {
		Melder_progress (0.0, L"Minimize many times");
	}
	/* on first iteration start with current parameters 27/11/97 */
	for (long i = 1; i <= numberOfTimes; i++) {
		Minimizer_minimize (me, maxIterationsPerTime, tolerance, monitorSingle);
		Melder_casual ("Current %ld: minimum = %.17g", i, my minimum);
		if (my minimum < fopt) {
			NUMvector_copyElements (my p, popt.peek(), 1, my nParameters);
			fopt = my minimum;
		}
		Minimizer_reset (me, 0);
		if (! monitorSingle) {
			try {
				Melder_progress ( (double) i / numberOfTimes, Melder_integer (i), L" from ",
				                   Melder_integer (numberOfTimes));
			} catch (MelderError) {
				Melder_clearError ();   // interrurpt, no error
				break;
			}
		}
	}
	if (! monitorSingle) {
		Melder_progress (1.0, 0);
	}
	Minimizer_reset (me, popt.peek());
}

static void Minimizer_setAfterEachIteration (Minimizer me, void (*after) (I, Any aclosure), Any aclosure) {
	my after = after;
	my aclosure = aclosure;
}

void Minimizer_reset (Minimizer me, const double guess[]) {
	if (guess) {
		for (long i = 1; i <= my nParameters; i++) {
			my p[i] = guess[i];
		}
	} else {
		for (long i = 1; i <= my nParameters; i++) {
			my p[i] = NUMrandomUniform (-1, 1);
		}
	}
	/*
		Don't use NUMdvector_free: realloc in Minimizer_minimize
	*/
	if (my history != 0) {
		my history++;
		Melder_free (my history);
	}
	my maxNumOfIterations = my success = my funcCalls = my iteration = 0;
	my minimum = 1.0e38;
	my v_reset ();
}

void Minimizer_drawHistory (Minimizer me, Graphics g, long iFrom, long iTo, double hmin,
                            double hmax, int garnish) {
	if (my history == 0) {
		return;
	}
	autoNUMvector<double> history (1, my iteration);
	for (long i = 1; i <= my iteration; i++) {
		history[i] = my history[i];
	}
	if (iTo <= iFrom) {
		iFrom = 1; iTo = my iteration;
	}
	long itmin = iFrom, itmax = iTo;
	if (itmin < 1) {
		itmin = 1;
	}
	if (itmax > my iteration) {
		itmax = my iteration;
	}
	if (hmax <= hmin) {
		NUMvector_extrema (history.peek(), itmin, itmax, & hmin, & hmax);
	}
	if (hmax <= hmin) {
		hmin -= 0.5 * fabs (hmin);
		hmax += 0.5 * fabs (hmax);
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, iFrom, iTo, hmin, hmax);
	Graphics_function (g, history.peek(), itmin, itmax, itmin, itmax);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Number of iterations");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

double Minimizer_getMinimum (Minimizer me) {
	return my minimum;
}

/**************  class SteepestDescentMinimizer **********************/

Thing_implement	(SteepestDescentMinimizer, Minimizer, 0);

void structSteepestDescentMinimizer :: v_minimize () {
	autoNUMvector<double> dp (1, nParameters);
	autoNUMvector<double> dpp (1, nParameters);
	double fret = func (object, p);
	while (iteration < maxNumOfIterations) {
		dfunc (object, p, dp.peek());
		for (long i = 1; i <= nParameters; i++) {
			dpp[i] = - eta * dp[i] + momentum * dpp[i];
			p[i] += dpp[i];
		}
		history[++iteration] = minimum = func (object, p);
		success = 2.0 * fabs (fret - minimum) < tolerance * (fabs (fret) + fabs (minimum));
		if (after) {
			try {
				after (this, aclosure);
			} catch (MelderError) {
				Melder_casual ("Interrupted after %ld iterations.", iteration);
				Melder_clearError ();
				break;
			}
		}
		if (success) {
			break;
		}
		fret = minimum;
	}
}

void structSteepestDescentMinimizer :: v_setParameters (Any parameters) {
	if (p) {
		SteepestDescentMinimizer_parameters thee = (SteepestDescentMinimizer_parameters) parameters;
		eta = thy eta;
		momentum = thy momentum;
	} else {
		eta = 0.1; momentum = 0.9;
	}
}

SteepestDescentMinimizer SteepestDescentMinimizer_create (long nParameters, Data object,
        double (*func) (Data object, const double p[]),
        void (*dfunc) (Data object, const double p[], double dp[])) {
	try {
		autoSteepestDescentMinimizer me = Thing_new (SteepestDescentMinimizer);
		Minimizer_init (me.peek(), nParameters, object);
		my func = func;
		my dfunc = dfunc;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SteepestDescentMinimizer not created.");
	}
}

/*****************  class VDSmagtMinimizer ******************************/

Thing_implement (VDSmagtMinimizer, Minimizer, 0);

void structVDSmagtMinimizer :: v_minimize () {
	int decrease_direction_found = 1;
	int l_iteration = 1;   // David, dat is gevaarlijk: een locale variabele met dezelfde naam als een member; daarom hernoemd, maar is het correct? yes, we can iterate in steps, therefore local and global counter
	double rtemp, rtemp2;

	// df is estimate of function reduction obtainable during line search
	// restart = 2 => line search in direction of steepest descent
	// restart = 1 => line search with Powell-restart.
	// flag = 1 => no decrease in function value during previous line search;
	// flag = 2 => line search did not decrease gradient
	//    OK; must restart

	if (restart_flag) {
		minimum = func (object, p);
		dfunc (object, p, dp);
		df = minimum;
		restart = 2;
		one_up = flag = 0;
		gcg0 = gopt_sq = 0.0;
	}
	restart_flag = 1;
	while (++ this -> iteration <= maxNumOfIterations) {
		if (flag & 1) {
			if (one_up) {
				decrease_direction_found = 0;
				this -> iteration --;
				break;
			} else {
				one_up = 1;
			}
		} else {
			one_up = 0;
		}
		if (flag & 2) {
			restart = 2; /* flag & 1 ??? */
		} else if (fabs ( (double) gcg0) > 0.2 * gopt_sq) {
			restart = 1;
		}
		if (restart == 0) {
			rtemp = rtemp2 = 0.0;
			for (long i = 1; i <= nParameters; i++) {
				rtemp += gc[i] * grst[i];
				rtemp2 += gc[i] * srst[i];
			}
			gamma = rtemp / gamma_in;
			if (fabs (beta * gropt - gamma * rtemp2) > 0.2 * gopt_sq) {
				restart = 1;
			} else {
				for (long i = 1; i <= nParameters; i++) {
					s[i] = beta * s[i] + gamma * srst[i] - gc[i];
				}
			}
		}
		if (restart == 2) {
			for (long i = 1; i <= nParameters; i++) {
				s[i] = - dp[i];
			}
			restart = 1;
		} else if (restart == 1) {
			gamma_in = gropt - gr0;
			for (long i = 1; i <= nParameters; i++) {
				srst[i] = s[i];
				s[i] = beta * s[i] - gc[i];
				grst[i] = gc[i] - g0[i];
			}
			restart = 0;
		}

		// Begin line search
		// lineSearch_iteration = #iterations during current line search

		flag = 0;
		lineSearch_iteration = 0;
		rtemp = 0.0;
		for (long i = 1; i <= nParameters; i++) {
			rtemp += dp[i] * s[i];
			g0[i] = dp[i];
		}
		gr0 = gropt = rtemp;
		if (l_iteration == 1) {
			alphamin = fabs (df / gropt);
		}
		if (gr0 > 0) {
			flag = 1;
			restart = 2;
			continue;
		}
		f0 = minimum;

		// alpha = length of step along line;
		// dalpha = change in alpha
		// alphamin = position of min along line

		alplim = -1;
		again = -1;
		rtemp = fabs (df / gropt);
		dalpha = alphamin < rtemp ? alphamin : rtemp;
		alphamin = 0;
		do {
			do {
				if (lineSearch_iteration) {
					if (! (fch == 0)) {
						gr2s += (temp + temp) / dalpha;
					}

					if (alplim < -0.5) {
						dalpha = 9.0 * alphamin;
					} else {
						dalpha = 0.5 * (alplim - alphamin);
					}

					grs = gropt + dalpha * gr2s;
					if (gropt * grs < 0) {
						dalpha *= gropt / (gropt - grs);
					}
				}
				alpha = alphamin + dalpha;
				for (long i = 1; i <= nParameters; i++) {
					pc[i] = p[i] + dalpha * s[i];
				}
				fc = func (object, pc);
				dfunc (object, pc, gc);
				l_iteration ++;
				lineSearch_iteration++;
				gsq = grc = 0.0;
				for (long i = 1; i <= nParameters; i++) {
					gsq += gc[i] * gc[i];
					grc += gc[i] * s[i];
				}
				fch = fc - minimum;
				gr2s = (grc - gropt) / dalpha;
				temp = (fch + fch) / dalpha - grc - gropt;
				if ( (fc < minimum) ||
				        ( (fc == minimum) && (grc / gropt > -1))) {
					double *tmp;
					gopt_sq = gsq;
					history [this ->iteration] = minimum = fc;
					tmp = p; p = pc; pc = tmp;
					tmp = dp; dp = gc; gc = tmp;
					if (grc *gropt <= 0) {
						alplim = alphamin;
					}
					alphamin = alpha;
					gropt = grc;
					dalpha = - dalpha;
					success = gsq < tolerance;
					if (after) {
						try {
							after (this, aclosure);
						} catch (MelderError) {
							Melder_casual ("Interrupted after %ld iterations.", this -> iteration);
							Melder_clearError ();
							break;
						}
					}
					if (success) {
						return;
					}
					if (fabs (gropt / gr0) < lineSearchGradient) {
						break;
					}
				} else {
					alplim = alpha;
				}
			} while (lineSearch_iteration
			         <= lineSearchMaxNumOfIterations);

			fc = history [this -> iteration] = minimum;
			rtemp = 0.0;
			for (long i = 1; i <= nParameters; i++) {
				pc[i] = p[i];
				gc[i] = dp[i];
				rtemp += gc[i] * g0[i];
			}
			gcg0 = rtemp;
			if (fabs (gropt - gr0) > tolerance) {
				beta = (gopt_sq - gcg0) / (gropt - gr0);
				if (fabs (beta * gropt) < 0.2 * gopt_sq) {
					break;
				}
			}
			again++;
			if (again > 0) {
				flag += 2;
			}
		} while (flag < 1);

		if (f0 <= minimum) {
			flag += 1;
		}
		df = gr0 * alphamin;
	}
	if (this -> iteration > maxNumOfIterations) {
		this -> iteration = maxNumOfIterations;
	}
	if (decrease_direction_found) {
		restart_flag = 0;
	}
}

void structVDSmagtMinimizer :: v_destroy () {
	NUMvector_free (dp, 1);
	NUMvector_free (pc, 1);
	NUMvector_free (gc, 1);
	NUMvector_free (g0, 1);
	NUMvector_free (s, 1);
	NUMvector_free (srst, 1);
	NUMvector_free (grst, 1);
	VDSmagtMinimizer_Parent :: v_destroy ();
}

void structVDSmagtMinimizer :: v_reset () {
	restart_flag = 1;
}

void structVDSmagtMinimizer :: v_setParameters (Any parameters) {
	if (parameters) {
		VDSmagtMinimizer_parameters  vdspars = (VDSmagtMinimizer_parameters) parameters;  // David, weer link: dezelfde naam,: Nu niet meer
		lineSearchGradient = vdspars -> lineSearchGradient;
		lineSearchMaxNumOfIterations = vdspars -> lineSearchMaxNumOfIterations;
	}
}

VDSmagtMinimizer VDSmagtMinimizer_create (long nParameters, Data object, double (*func) (Data object, const double x[]),
        void (*dfunc) (Data object, const double x[], double dx[])) {
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
	} catch (MelderError) {
		Melder_throw ("VDSmagtMinimizer not created.");
	}
}

/************ class LineMinimizer *******************************/

void structLineMinimizer :: v_destroy () {
	NUMvector_free (ptry, 1);
	NUMvector_free (direction, 1);
	LineMinimizer_Parent :: v_destroy ();
}

Thing_implement (LineMinimizer, Minimizer, 0);

void LineMinimizer_init (I, long nParameters, Data object, double (*func) (Data, const double [])) {
	iam (LineMinimizer);
	Minimizer_init (me, nParameters, object);
	my direction = NUMvector<double> (1, nParameters);
	my ptry = NUMvector<double> (1, nParameters);
	my func = func;
	my maxLineStep = 100;
}

/* End of file Minimizers.c 657*/
