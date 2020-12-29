/* Minimizers.cpp
 *
 * Copyright (C) 2001-2020 David Weenink
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

#include "melder.h"
#include "NUM2.h"
#include "Graphics.h"
#include "Minimizers.h"

Thing_implement (Minimizer, Thing, 0);

static void classMinimizer_afterHook (Minimizer me, Thing /* boss */) {
	if (my success || ! my gmonitor)
		return;
	Graphics_beginMovieFrame (my gmonitor, nullptr);
	Graphics_clearWs (my gmonitor);
	Minimizer_drawHistory (me, my gmonitor, 0, my maximumNumberOfIterations, 0.0, 1.1 * my history [1], 1);
	Graphics_textTop (my gmonitor, false, Melder_cat (U"Dimension of search space: ", my numberOfParameters));
	Graphics_endMovieFrame (my gmonitor, 0.0);
	Melder_monitor ((double) (my iteration) / my maximumNumberOfIterations, U"Iterations: ", my iteration, 
		U", Function calls: ", my numberOfFunctionCalls, U", Cost: ", my minimum);
}

void Minimizer_init (Minimizer me, integer numberOfParameters, Daata object) {
	my numberOfParameters = numberOfParameters;
	my p = zero_VEC (numberOfParameters);
	my object = object;
	my minimum = 1e308;
	my afterHook = classMinimizer_afterHook;
	autoVEC my_p;
	Minimizer_reset (me, my_p.get());   // do weights initialization if my_p.size == 0
}

static void monitor_off (Minimizer me) {
	Melder_monitor (1.0);
	my gmonitor = nullptr;
}

void Minimizer_minimize (Minimizer me, integer maximumNumberOfIterations, double tolerance, int monitor) {
	try {
		my tolerance = tolerance;
		if (maximumNumberOfIterations <= 0)
			return;
		if (my iteration + maximumNumberOfIterations > my maximumNumberOfIterations) {
			my maximumNumberOfIterations += maximumNumberOfIterations;
			my history. resize (my maximumNumberOfIterations);
		}
		if (monitor)
			my gmonitor = (Graphics) Melder_monitor (0.0, U"Starting...");
		my v_minimize ();
		if (monitor)
			monitor_off (me);
		if (my success)
			Melder_casual (U"Minimizer_minimize:", U" minimum ", my minimum, U" reached \nafter ", my iteration,
			U" iterations and ", my numberOfFunctionCalls, U" function calls.");
	} catch (MelderError) {
		if (monitor)
			monitor_off (me);   // temporarily until better monitor facilities
		Melder_clearError();   // memory error in history mechanism is not fatal
	}
}

void Minimizer_minimizeManyTimes (Minimizer me, integer maxIterationsPerTime, integer numberOfTimes, double tolerance) {
	double fopt = my minimum;
	int monitorSingle = numberOfTimes == 1;

	autoVEC popt = raw_VEC (my numberOfParameters);
	popt.get () <<= my p.get();

	if (! monitorSingle)
		Melder_progress (0.0, U"Minimize many times");

	/* on first iteration start with current parameters 27/11/97 */
	for (integer iter = 1; iter <= numberOfTimes; iter ++) {
		Minimizer_minimize (me, maxIterationsPerTime, tolerance, monitorSingle);
		Melder_casual (U"Current ", iter, U": minimum = ", my minimum);
		if (my minimum < fopt) {
			my p.get () <<= popt.get();
			fopt = my minimum;
		}
		VEC p;
		Minimizer_reset (me, p); // do initialization if p.size == 0
		if (! monitorSingle) {
			try {
				Melder_progress ((double) iter / numberOfTimes, iter, U" from ", numberOfTimes);
			} catch (MelderError) {
				Melder_clearError ();   // interrupted, no error
				break;
			}
		}
	}
	if (! monitorSingle)
		Melder_progress (1.0);
	Minimizer_reset (me, popt.get());
}

void Minimizer_reset (Minimizer me, constVEC const& guess) {
	Melder_assert (guess.size == 0 || guess.size >= my numberOfParameters);
	if (guess.size > 0)
		my p.all()  <<=  guess;
	else
		for (integer i = 1; i <= my numberOfParameters; i ++)
			my p [i] = NUMrandomUniform (-1.0, 1.0);

	my history. resize (0);
	my maximumNumberOfIterations = my numberOfFunctionCalls = my iteration = 0;
	my success = false;
	my minimum = 1.0e38;
	my v_reset ();
}

void Minimizer_drawHistory (Minimizer me, Graphics g, integer iFrom, integer iTo, double hmin, double hmax, bool garnish) {
	if (my history.size == 0)
		return;
	if (iTo <= iFrom) {
		iFrom = 1;
		iTo = my iteration;
	}
	integer itmin = iFrom, itmax = iTo;
	Melder_clipLeft (1_integer, & itmin);
	Melder_clipRight (& itmax, my iteration);
	if (hmax <= hmin)
		NUMextrema (my history.part (itmin, itmax), & hmin, & hmax);
	if (hmax <= hmin) {
		hmin -= 0.5 * fabs (hmin);
		hmax += 0.5 * fabs (hmax);
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, iFrom, iTo, hmin, hmax);
	Graphics_function (g, my history.asArgumentToFunctionThatExpectsOneBasedArray(), itmin, itmax, itmin, itmax);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Number of iterations");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

double Minimizer_getMinimum (Minimizer me) {
	return my minimum;
}

/**************  class SteepestDescentMinimizer **********************/

Thing_implement	(SteepestDescentMinimizer, Minimizer, 0);

void structSteepestDescentMinimizer :: v_minimize () {
	autoVEC dp = raw_VEC (numberOfParameters);
	autoVEC dpp = raw_VEC (numberOfParameters);
	double fret = func (object, p.get());
	while (iteration < maximumNumberOfIterations) {
		dfunc (object, p.get(), dp.get());
		for (integer i = 1; i <= numberOfParameters; i ++) {
			dpp [i] = - eta * dp [i] + momentum * dpp [i];
			p [i] += dpp [i];
		}
		history [++ iteration] = minimum = func (object, p.get());
		success = 2.0 * fabs (fret - minimum) < tolerance * (fabs (fret) + fabs (minimum));
		if (our afterHook) {
			try {
				our afterHook (this, our afterBoss);
			} catch (MelderError) {
				Melder_casual (U"Interrupted after ", iteration, U" iterations.");
				Melder_clearError ();
				break;
			}
		}
		if (success) break;
		fret = minimum;
	}
}

autoSteepestDescentMinimizer SteepestDescentMinimizer_create (integer numberOfParameters, Daata object, double (*func) (Daata object, VEC const& p), void (*dfunc) (Daata object, VEC const& p, VEC const&)) {
	try {
		autoSteepestDescentMinimizer me = Thing_new (SteepestDescentMinimizer);
		Minimizer_init (me.get(), numberOfParameters, object);
		my func = func;
		my dfunc = dfunc;
		return me;
	} catch (MelderError) {
		Melder_throw (U"SteepestDescentMinimizer not created.");
	}
}

/*****************  class VDSmagtMinimizer ******************************/

Thing_implement (VDSmagtMinimizer, Minimizer, 0);

void structVDSmagtMinimizer :: v_minimize () {
	int decrease_direction_found = 1;
	int l_iteration = 1;   // yes, we can iterate in steps, therefore local and global counter
	longdouble rtemp, rtemp2;
	/*
		df is estimate of function reduction obtainable during line search
		restart = 2 => line search in direction of steepest descent
		restart = 1 => line search with Powell-restart.
		flag = 1 => no decrease in function value during previous line search;
		flag = 2 => line search did not decrease gradient
			OK; must restart
	*/
	if (restart_flag) {
		minimum = func (object, p.get());
		dfunc (object, p.get(), dp.get());
		df = minimum;
		restart = 2;
		one_up = flag = 0;
		gcg0 = gopt_sq = 0.0;
	}
	restart_flag = true;
	while (++ this -> iteration <= maximumNumberOfIterations) {
		if (flag & 1) {
			if (one_up) {
				decrease_direction_found = 0;
				this -> iteration --;
				break;
			} else
				one_up = 1;
		} else
			one_up = 0;

		if (flag & 2)
			restart = 2; /* flag & 1 ??? */
		else if (fabs ((double) gcg0) > 0.2 * gopt_sq)
			restart = 1;

		if (restart == 0) {
			rtemp = rtemp2 = 0.0;
			for (integer i = 1; i <= numberOfParameters; i ++) {
				rtemp += gc [i] * grst [i];
				rtemp2 += gc [i] * srst [i];
			}
			gamma = rtemp / gamma_in;
			if (fabs (beta * gropt - gamma * rtemp2) > 0.2 * gopt_sq)
				restart = 1;
			else
				for (integer i = 1; i <= numberOfParameters; i ++)
					s [i] = beta * s [i] + gamma * srst [i] - gc [i];
		}
		if (restart == 2) {
			for (integer i = 1; i <= numberOfParameters; i ++)
				s [i] = - dp [i];
			restart = 1;
		} else if (restart == 1) {
			gamma_in = gropt - gr0;
			for (integer i = 1; i <= numberOfParameters; i ++) {
				srst [i] = s [i];
				s [i] = beta * s [i] - gc [i];
				grst [i] = gc [i] - g0 [i];
			}
			restart = 0;
		}
		/*
			Begin line search
			lineSearch_iteration = #iterations during current line search
		*/
		flag = 0;
		lineSearch_iteration = 0;
		rtemp = 0.0;
		for (integer i = 1; i <= numberOfParameters; i ++) {
			rtemp += dp [i] * s [i];
			g0 [i] = dp [i];
		}
		gr0 = gropt = rtemp;
		if (l_iteration == 1)
			alphamin = fabs (df / gropt);
		if (gr0 > 0) {
			flag = 1;
			restart = 2;
			continue;
		}
		f0 = minimum;
		/*
			alpha = length of step along line;
			dalpha = change in alpha
			alphamin = position of min along line
		*/
		alplim = -1;
		again = -1;
		rtemp = fabs (df / gropt);
		dalpha = std::min (alphamin, (double) rtemp);
		alphamin = 0;
		do {
			do {
				if (lineSearch_iteration) {
					if (! (fch == 0))
						gr2s += (temp + temp) / dalpha;

					if (alplim < -0.5)
						dalpha = 9.0 * alphamin;
					else
						dalpha = 0.5 * (alplim - alphamin);

					grs = gropt + dalpha * gr2s;
					if (gropt * grs < 0)
						dalpha *= gropt / (gropt - grs);
				}
				alpha = alphamin + dalpha;
				for (integer i = 1; i <= numberOfParameters; i ++)
					pc [i] = p [i] + dalpha * s [i];
				fc = func (object, pc.get());
				dfunc (object, pc.get(), gc.get());
				l_iteration ++;
				lineSearch_iteration ++;
				gsq = grc = 0.0;
				for (integer i = 1; i <= numberOfParameters; i ++) {
					gsq += gc [i] * gc [i];
					grc += gc [i] * s [i];
				}
				fch = fc - minimum;
				gr2s = (grc - gropt) / dalpha;
				temp = (fch + fch) / dalpha - grc - gropt;
				if ((fc < minimum) || ((fc == minimum) && (grc / gropt > -1))) {
					gopt_sq = gsq;
					history [this -> iteration] = minimum = fc;
					std::swap (p, pc);
					std::swap (dp, gc);
					if (grc * gropt <= 0.0)
						alplim = alphamin;
					alphamin = alpha;
					gropt = grc;
					dalpha = - dalpha;
					success = ( gsq < tolerance );
					if (our afterHook) {
						try {
							our afterHook (this, our afterBoss);
						} catch (MelderError) {
							Melder_casual (U"Interrupted after ", this -> iteration, U" iterations.");
							Melder_clearError ();
							break;
						}
					}
					if (success)
						return;
					if (fabs (gropt / gr0) < lineSearchGradient)
						break;
				} else {
					alplim = alpha;
				}
			} while (lineSearch_iteration <= lineSearchMaxNumOfIterations);

			fc = history [this -> iteration] = minimum;
			rtemp = 0.0;
			for (integer i = 1; i <= numberOfParameters; i ++) {
				pc [i] = p [i];
				gc [i] = dp [i];
				rtemp += gc [i] * g0 [i];
			}
			gcg0 = rtemp;
			if (fabs (gropt - gr0) > tolerance) {
				beta = (gopt_sq - gcg0) / (gropt - gr0);
				if (fabs (beta * gropt) < 0.2 * gopt_sq) break;
			}
			again ++;
			if (again > 0) flag += 2;
		} while (flag < 1);

		if (f0 <= minimum)
			flag += 1;
		df = gr0 * alphamin;
	}
	if (this -> iteration > maximumNumberOfIterations)
		this -> iteration = maximumNumberOfIterations;
	if (decrease_direction_found)
		restart_flag = false;
}

void structVDSmagtMinimizer :: v_reset () {
	restart_flag = true;
}

autoVDSmagtMinimizer VDSmagtMinimizer_create (integer numberOfParameters, Daata object, double (*func) (Daata object, VEC const& x), void (*dfunc) (Daata object, VEC const& x, VEC const& dx)) {
	try {
		autoVDSmagtMinimizer me = Thing_new (VDSmagtMinimizer);
		Minimizer_init (me.get(), numberOfParameters, object);
		my dp = zero_VEC (numberOfParameters);
		my pc = zero_VEC (numberOfParameters);
		my gc = zero_VEC (numberOfParameters);
		my g0 = zero_VEC (numberOfParameters);
		my s = zero_VEC (numberOfParameters);
		my srst = zero_VEC (numberOfParameters);
		my grst = zero_VEC (numberOfParameters);
		my func = func;
		my dfunc = dfunc;
		my lineSearchGradient = 0.9;
		my lineSearchMaxNumOfIterations = 5;
		return me;
	} catch (MelderError) {
		Melder_throw (U"VDSmagtMinimizer not created.");
	}
}

/************ class LineMinimizer *******************************/

Thing_implement (LineMinimizer, Minimizer, 0);

void LineMinimizer_init (LineMinimizer me, integer numberOfParameters, Daata object, double (*func) (Daata, VEC const& p)) {
	Minimizer_init (me, numberOfParameters, object);
	my direction = zero_VEC (numberOfParameters);
	my ptry = zero_VEC (numberOfParameters);
	my func = func;
	my maxLineStep = 100;
}

/* End of file Minimizers.cpp */
