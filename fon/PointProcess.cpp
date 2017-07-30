/* PointProcess.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PointProcess.h"
#include "VoiceAnalysis.h"

#include "oo_DESTROY.h"
#include "PointProcess_def.h"
#include "oo_COPY.h"
#include "PointProcess_def.h"
#include "oo_EQUAL.h"
#include "PointProcess_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "PointProcess_def.h"
#include "oo_WRITE_TEXT.h"
#include "PointProcess_def.h"
#include "oo_READ_TEXT.h"
#include "PointProcess_def.h"
#include "oo_WRITE_BINARY.h"
#include "PointProcess_def.h"
#include "oo_READ_BINARY.h"
#include "PointProcess_def.h"
#include "oo_DESCRIPTION.h"
#include "PointProcess_def.h"

Thing_implement (PointProcess, Function, 0);

static void infoPeriods (PointProcess me, double shortestPeriod, double longestPeriod, double maximumPeriodFactor, int precision) {
	long numberOfPeriods = PointProcess_getNumberOfPeriods (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double meanPeriod = PointProcess_getMeanPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double stdevPeriod = PointProcess_getStdevPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_local = PointProcess_getJitter_local (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_local_absolute = PointProcess_getJitter_local_absolute (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_rap = PointProcess_getJitter_rap (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_ppq5 = PointProcess_getJitter_ppq5 (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_ddp = PointProcess_getJitter_ddp (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	MelderInfo_writeLine (U"     Number of periods: ", numberOfPeriods);
	MelderInfo_writeLine (U"     Mean period: ", meanPeriod, U" seconds");
	MelderInfo_writeLine (U"     Stdev period: ", stdevPeriod, U" seconds");
	MelderInfo_writeLine (U"     Jitter (local): ", Melder_percent (jitter_local, precision));
	MelderInfo_writeLine (U"     Jitter (local, absolute): ", Melder_fixedExponent (jitter_local_absolute, -6, precision), U" seconds");
	MelderInfo_writeLine (U"     Jitter (rap): ", Melder_percent (jitter_rap, precision));
	MelderInfo_writeLine (U"     Jitter (ppq5): ", Melder_percent (jitter_ppq5, precision));
	MelderInfo_writeLine (U"     Jitter (ddp): ", Melder_percent (jitter_ddp, precision));
}

void structPointProcess :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Number of times: ", nt);
	if (nt) {
		MelderInfo_writeLine (U"First time: ", t [1], U" seconds");
		MelderInfo_writeLine (U"Last time: ", t [nt], U" seconds");
	}
	MelderInfo_writeLine (U"Periods between 0.1 ms and 20 ms (pitch between 50 and 10000 Hz),");
	MelderInfo_writeLine (U"with a maximum \"period factor\" of 1.3:");
	infoPeriods (this, 1e-4, 20e-3, 1.3, 3);
	MelderInfo_writeLine (U"All periods:");
	infoPeriods (this, 0.0, 0.0, 1e308, 6);
}

void structPointProcess :: v_shiftX (double xfrom, double xto) {
	PointProcess_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= nt; i ++) {
		NUMshift (& t [i], xfrom, xto);
	}
}

void structPointProcess :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	PointProcess_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= nt; i ++) {
		NUMscale (& t [i], xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

void PointProcess_init (PointProcess me, double tmin, double tmax, long initialMaxnt) {
	Function_init (me, tmin, tmax);
	if (initialMaxnt < 1) initialMaxnt = 1;
	my maxnt = initialMaxnt;
	my nt = 0;
	my t = NUMvector <double> (1, my maxnt);
}

autoPointProcess PointProcess_create (double tmin, double tmax, long initialMaxnt) {
	try {
		autoPointProcess me = Thing_new (PointProcess);
		PointProcess_init (me.get(), tmin, tmax, initialMaxnt);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess not created.");
	}
}

autoPointProcess PointProcess_createPoissonProcess (double startingTime, double finishingTime, double density) {
	try {
		long nt = NUMrandomPoisson ((finishingTime - startingTime) * density);
		autoPointProcess me = PointProcess_create (startingTime, finishingTime, nt);
		my nt = nt;
		for (long i = 1; i <= nt; i ++)
			my t [i] = NUMrandomUniform (startingTime, finishingTime);
		NUMsort_d (my nt, my t);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess (Poisson process) not created.");
	}
}

long PointProcess_getLowIndex (PointProcess me, double t) {
	if (my nt == 0 || t < my t [1])
		return 0;
	if (t >= my t [my nt])   // special case that often occurs in practice
		return my nt;
	Melder_assert (my nt != 1);   // may fail if t or my t [1] is NaN
	/* Start binary search. */
	long left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t >= my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return left;
}

long PointProcess_getHighIndex (PointProcess me, double t) {
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t > my t [my nt])
		return my nt + 1;
	/* Start binary search. */
	long left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t > my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return right;
}

long PointProcess_getNearestIndex (PointProcess me, double t) {
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t >= my t [my nt])
		return my nt;
	/* Start binary search. */
	long left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t >= my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return t - my t [left] < my t [right] - t ? left : right;
}

void PointProcess_addPoint (PointProcess me, double t) {
	try {
		if (isundef (t))
			Melder_throw (U"Cannot add a point at an undefined time.");
		if (my nt >= my maxnt) {
			/*
			 * Create without change.
			 */
			autoNUMvector <double> dum (1, 2 * my maxnt);
			NUMvector_copyElements (my t, dum.peek(), 1, my nt);
			/*
			 * Change without error.
			 */
			NUMvector_free (my t, 1);
			my t = dum.transfer();
			my maxnt *= 2;
		}
		if (my nt == 0 || t >= my t [my nt]) {   // special case that often occurs in practice
			my t [++ my nt] = t;
		} else {
			long left = PointProcess_getLowIndex (me, t);
			if (left == 0 || my t [left] != t) {
				for (long i = my nt; i > left; i --) my t [i + 1] = my t [i];
				my nt ++;
				my t [left + 1] = t;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": point not added.");
	}
}

void PointProcess_removePoint (PointProcess me, long pointNumber) {
	if (pointNumber < 1 || pointNumber > my nt) return;
	for (long i = pointNumber; i < my nt; i ++)
		my t [i] = my t [i + 1];
	my nt --;
}

void PointProcess_removePointNear (PointProcess me, double time) {
	PointProcess_removePoint (me, PointProcess_getNearestIndex (me, time));
}

void PointProcess_removePoints (PointProcess me, long first, long last) {
	if (first < 1) first = 1;
	if (last > my nt) last = my nt;
	long distance = last - first + 1;
	if (distance <= 0) return;
	for (long i = first + distance; i <= my nt; i ++)
		my t [i - distance] = my t [i];
	my nt -= distance;
}

void PointProcess_removePointsBetween (PointProcess me, double tmin, double tmax) {
	PointProcess_removePoints (me, PointProcess_getHighIndex (me, tmin), PointProcess_getLowIndex (me, tmax));
}

void PointProcess_draw (PointProcess me, Graphics g, double tmin, double tmax, bool garnish) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Graphics_setWindow (g, tmin, tmax, -1.0, 1.0);
	if (my nt) {
		long imin = PointProcess_getHighIndex (me, tmin);
		long imax = PointProcess_getLowIndex  (me, tmax);
		int lineType = Graphics_inqLineType (g);
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_setInner (g);
		for (long i = imin; i <= imax; i ++) {
			Graphics_line (g, my t [i], -1.0, my t [i], 1.0);
		}
		Graphics_setLineType (g, lineType);
		Graphics_unsetInner (g);
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

double PointProcess_getInterval (PointProcess me, double t) {
	long ileft = PointProcess_getLowIndex (me, t);
	if (ileft <= 0 || ileft >= my nt) return undefined;
	return my t [ileft + 1] - my t [ileft];
}

autoPointProcess PointProcesses_union (PointProcess me, PointProcess thee) {
	try {
		autoPointProcess him = Data_copy (me);
		if (thy xmin < my xmin) his xmin = thy xmin;
		if (thy xmax > my xmax) his xmax = thy xmax;
		for (long i = 1; i <= thy nt; i ++) {
			PointProcess_addPoint (him.get(), thy t [i]);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": union not computed.");
	}
}

long PointProcess_findPoint (PointProcess me, double t) {
	long left = 1, right = my nt;
	if (my nt == 0) return 0;
	if (t < my t [left] || t > my t [right]) return 0;
	while (left < right - 1) {
		long mid = (left + right) / 2;   // tleft <= t <= tright
		if (t == my t [mid]) return mid;
		if (t > my t [mid])
			left = mid;
		else
			right = mid;
	}
	if (t == my t [left]) return left;
	if (t == my t [right]) return right;
	return 0;
}

autoPointProcess PointProcesses_intersection (PointProcess me, PointProcess thee) {
	try {
		autoPointProcess him = Data_copy (me);
		if (thy xmin > my xmin) his xmin = thy xmin;
		if (thy xmax < my xmax) his xmax = thy xmax;
		for (long i = my nt; i >= 1; i --)
			if (! PointProcess_findPoint (thee, my t [i]))
				PointProcess_removePoint (him.get(), i);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": intersection not computed.");
	}
}

autoPointProcess PointProcesses_difference (PointProcess me, PointProcess thee) {
	try {
		autoPointProcess him = Data_copy (me);
		for (long i = my nt; i >= 1; i --)
			if (PointProcess_findPoint (thee, my t [i]))
				PointProcess_removePoint (him.get(), i);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": difference not computed.");
	}
}

void PointProcess_fill (PointProcess me, double tmin, double tmax, double period) {
	try {
		if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
		long n = (long) floor ((tmax - tmin) / period);
		double t = 0.5 * (tmin + tmax - n * period);
		for (long i = 1; i <= n; i ++, t += period) {
			PointProcess_addPoint (me, t);
		}
	} catch (MelderError) {
		Melder_throw (me, U": not filled.");
	}
}

void PointProcess_voice (PointProcess me, double period, double maxT) {
	try {
		long ipointright;
		double beginVoiceless = my xmin, endVoiceless;
		for (long ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
			endVoiceless = my t [ipointleft];
			PointProcess_fill (me, beginVoiceless, endVoiceless, period);
			for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
				if (my t [ipointright] - my t [ipointright - 1] > maxT)
					break;
			ipointright --;
			beginVoiceless = my t [ipointright] + 0.005;
		}
		endVoiceless = my xmax;
		PointProcess_fill (me, beginVoiceless, endVoiceless, period);
	} catch (MelderError) {
		Melder_throw (me, U": not voiced.");
	}
}

long PointProcess_getWindowPoints (PointProcess me, double tmin, double tmax, long *p_imin, long *p_imax) {
	long imin = PointProcess_getHighIndex (me, tmin);
	long imax = PointProcess_getLowIndex (me, tmax);
	if (p_imin) *p_imin = imin;
	if (p_imax) *p_imax = imax;
	return imax - imin + 1;
}

static bool PointProcess_isPeriod (PointProcess me, long ileft, double minimumPeriod, double maximumPeriod, double maximumPeriodFactor) {
	/*
	 * This function answers the question: is the interval from point 'ileft' to point 'ileft+1' a period?
	 */
	long iright = ileft + 1;
	/*
	 * Period condition 1: both 'ileft' and 'iright' have to be within the point process.
	 */
	if (ileft < 1 || iright > my nt) {
		return false;
	} else {
		/*
		 * Period condition 2: the interval has to be within the boundaries, if specified.
		 */
		if (minimumPeriod == maximumPeriod) {
			return true;   // all intervals count as periods, irrespective of absolute size and relative size
		} else {
			double interval = my t [iright] - my t [ileft];
			if (interval <= 0.0 || interval < minimumPeriod || interval > maximumPeriod) {
				return false;
			} else if (isundef (maximumPeriodFactor) || maximumPeriodFactor < 1.0) {
				return true;
			} else {
				/*
				 * Period condition 3: the interval cannot be too different from both of its neigbours, if any.
				 */
				double previousInterval = ( ileft <= 1 ? undefined : my t [ileft] - my t [ileft - 1] );
				double nextInterval = ( iright >= my nt ? undefined : my t [iright + 1] - my t [iright] );
				double previousIntervalFactor =
					( isdefined (previousInterval) && previousInterval > 0.0 ? interval / previousInterval : undefined );
				double nextIntervalFactor =
					( isdefined (nextInterval) && nextInterval > 0.0 ? interval / nextInterval : undefined );
				if (isundef (previousIntervalFactor) && isundef (nextIntervalFactor)) {
					return true;   // no neighbours: this is a period
				}
				if (isdefined (previousIntervalFactor) && previousIntervalFactor > 0.0 && previousIntervalFactor < 1.0) {
					previousIntervalFactor = 1.0 / previousIntervalFactor;
				}
				if (isdefined (nextIntervalFactor) && nextIntervalFactor > 0.0 && nextIntervalFactor < 1.0) {
					nextIntervalFactor = 1.0 / nextIntervalFactor;
				}
				if (isdefined (previousIntervalFactor) && previousIntervalFactor > maximumPeriodFactor &&
					isdefined (nextIntervalFactor) && nextIntervalFactor > maximumPeriodFactor)
				{
					return false;
				}
			}
		}
	}
	return true;
}

long PointProcess_getNumberOfPeriods (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 1) return 0;
	for (long i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			(void) 0;   // this interval counts as a period
		} else {
			numberOfPeriods --;   // this interval does not count as a period
		}
	}
	return numberOfPeriods;
}

double PointProcess_getMeanPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 1) return undefined;
	double sum = 0.0;
	for (long i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			sum += my t [i + 1] - my t [i];   // this interval counts as a period
		} else {
			numberOfPeriods --;   // this interval does not count as a period
		}
	}
	return numberOfPeriods > 0 ? sum / numberOfPeriods : undefined;
}

double PointProcess_getStdevPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   // autowindowing
	long imin, imax;
	long numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 2) return undefined;
	/*
	 * Compute mean.
	 */
	double sum = 0.0;
	for (long i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			sum += my t [i + 1] - my t [i];   // this interval counts as a period
		} else {
			numberOfPeriods --;   // this interval does not count as a period
		}
	}
	if (numberOfPeriods < 2) return undefined;
	double mean = sum / numberOfPeriods;
	/*
	 * Compute variance.
	 */
	double sum2 = 0.0;
	for (long i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			double dperiod = my t [i + 1] - my t [i] - mean;
			sum2 += dperiod * dperiod;
		}
	}
	/*
	 * Compute standard deviation.
	 */
	return sqrt (sum2 / (numberOfPeriods - 1));
}

/* End of file PointProcess.cpp */
