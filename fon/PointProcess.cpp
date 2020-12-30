/* PointProcess.cpp
 *
 * Copyright (C) 1992-2012,2014-2020 Paul Boersma
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
	const integer numberOfPeriods = PointProcess_getNumberOfPeriods (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double meanPeriod = PointProcess_getMeanPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double stdevPeriod = PointProcess_getStdevPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double jitter_local = PointProcess_getJitter_local (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double jitter_local_absolute = PointProcess_getJitter_local_absolute (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double jitter_rap = PointProcess_getJitter_rap (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double jitter_ppq5 = PointProcess_getJitter_ppq5 (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	const double jitter_ddp = PointProcess_getJitter_ddp (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
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
	for (integer i = 1; i <= nt; i ++)
		NUMshift (& t [i], xfrom, xto);
}

void structPointProcess :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	PointProcess_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (integer i = 1; i <= nt; i ++)
		NUMscale (& t [i], xminfrom, xmaxfrom, xminto, xmaxto);
}

void PointProcess_init (PointProcess me, double tmin, double tmax, integer initialMaxnt) {
	Function_init (me, tmin, tmax);
	my t.initWithCapacity (initialMaxnt);
	my nt = 0;   // maintain invariant
}

autoPointProcess PointProcess_create (double tmin, double tmax, integer initialMaxnt) {
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
		autoPointProcess me = PointProcess_create (startingTime, finishingTime, 0);
		const integer numberOfPoints = (integer) NUMrandomPoisson ((finishingTime - startingTime) * density);
		my t = randomUniform_VEC (numberOfPoints, startingTime, finishingTime);
		my nt = numberOfPoints;   // maintain invariant
		sort_VEC_inout (my t.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"PointProcess (Poisson process) not created.");
	}
}

integer PointProcess_getLowIndex (PointProcess me, double t) {
	if (my nt == 0 || t < my t [1])
		return 0;
	if (t >= my t [my nt])   // special case that often occurs in practice
		return my nt;
	Melder_assert (my nt != 1);   // may fail if t or my t [1] is NaN
	/*
		Start binary search.
	*/
	integer left = 1, right = my nt;
	while (left < right - 1) {
		const integer mid = (left + right) / 2;
		if (t >= my t [mid])
			left = mid;
		else
			right = mid;
	}
	Melder_assert (right == left + 1);
	return left;
}

integer PointProcess_getHighIndex (PointProcess me, double t) {
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t > my t [my nt])
		return my nt + 1;
	/*
		Start binary search.
	*/
	integer left = 1, right = my nt;
	while (left < right - 1) {
		const integer mid = (left + right) / 2;
		if (t > my t [mid])
			left = mid;
		else
			right = mid;
	}
	Melder_assert (right == left + 1);
	return right;
}

integer PointProcess_getNearestIndex (PointProcess me, double t) {
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t >= my t [my nt])
		return my nt;
	/*
		Start binary search.
	*/
	integer left = 1, right = my nt;
	while (left < right - 1) {
		const integer mid = (left + right) / 2;
		if (t >= my t [mid])
			left = mid;
		else
			right = mid;
	}
	Melder_assert (right == left + 1);
	return t - my t [left] < my t [right] - t ? left : right;
}

void PointProcess_addPoint (PointProcess me, double t) {
	try {
		Melder_require (isdefined (t),
			U"Cannot add a point at an undefined time.");
		const integer newNumberOfPoints = my nt + 1;
		my t. resize (newNumberOfPoints);
		if (my nt == 0 || t >= my t [my nt]) {   // special case that often occurs in practice
			my nt = newNumberOfPoints;   // maintain invariant
			my t [newNumberOfPoints] = t;
		} else {
			const integer left = PointProcess_getLowIndex (me, t);
			if (left == 0 || my t [left] != t) {
				for (integer i = my nt; i > left; i --)
					my t [i + 1] = my t [i];
				my nt = newNumberOfPoints;   // maintain invariant
				my t [left + 1] = t;
			}
		}
	} catch (MelderError) {
		Melder_throw (me, U": point not added.");
	}
}

void PointProcess_addPoints (PointProcess me, constVECVU const& times) {
	try {
		const integer newNumberOfPoints = my nt + times.size;
		my t. resize (newNumberOfPoints);
		my t.part (my nt + 1, newNumberOfPoints)  <<=  times;
		my nt = newNumberOfPoints;   // maintain invariant
		sort_VEC_inout (my t.get());
	} catch (MelderError) {
		Melder_throw (me, U": points not added.");
	}
}

void PointProcess_removePoint (PointProcess me, integer pointNumber) {
	if (pointNumber < 1 || pointNumber > my nt) return;
	/*
		First shift, then resize.
	*/
	for (integer i = pointNumber; i < my nt; i ++)
		my t [i] = my t [i + 1];
	const integer newNumberOfPoints = my nt - 1;
	my t. resize (newNumberOfPoints);
	my nt = newNumberOfPoints;   // maintain invariant
}

void PointProcess_removePointNear (PointProcess me, double time) {
	PointProcess_removePoint (me, PointProcess_getNearestIndex (me, time));
}

void PointProcess_removePoints (PointProcess me, integer first, integer last) {
	Melder_clipLeft (1_integer, & first);
	Melder_clipRight (& last, my nt);
	const integer distance = last - first + 1;
	if (distance <= 0)
		return;
	for (integer i = first + distance; i <= my nt; i ++)
		my t [i - distance] = my t [i];
	const integer newNumberOfPoints = my nt - distance;
	my t. resize (newNumberOfPoints);
	my nt = newNumberOfPoints;   // maintain invariant
}

void PointProcess_removePointsBetween (PointProcess me, double tmin, double tmax) {
	PointProcess_removePoints (me, PointProcess_getHighIndex (me, tmin), PointProcess_getLowIndex (me, tmax));
}

void PointProcess_draw (PointProcess me, Graphics g, double tmin, double tmax, bool garnish) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setWindow (g, tmin, tmax, -1.0, 1.0);
	if (my nt) {
		const integer imin = PointProcess_getHighIndex (me, tmin);
		const integer imax = PointProcess_getLowIndex  (me, tmax);
		const int lineType = Graphics_inqLineType (g);
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_setInner (g);
		for (integer i = imin; i <= imax; i ++)
			Graphics_line (g, my t [i], -1.0, my t [i], 1.0);
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
	const integer ileft = PointProcess_getLowIndex (me, t);
	if (ileft <= 0 || ileft >= my nt)
		return undefined;
	return my t [ileft + 1] - my t [ileft];
}

autoPointProcess PointProcesses_union (PointProcess me, PointProcess thee) {
	try {
		autoPointProcess him = Data_copy (me);
		if (thy xmin < my xmin)
			his xmin = thy xmin;
		if (thy xmax > my xmax)
			his xmax = thy xmax;
		for (integer i = 1; i <= thy nt; i ++)
			PointProcess_addPoint (him.get(), thy t [i]);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": union not computed.");
	}
}

integer PointProcess_findPoint (PointProcess me, double t) {
	integer left = 1, right = my nt;
	if (my nt == 0)
		return 0;
	if (t < my t [left] || t > my t [right])
		return 0;
	while (left < right - 1) {
		integer mid = (left + right) / 2;   // tleft <= t <= tright
		if (t == my t [mid])
			return mid;
		if (t > my t [mid])
			left = mid;
		else
			right = mid;
	}
	if (t == my t [left])
		return left;
	if (t == my t [right])
		return right;
	return 0;
}

autoPointProcess PointProcesses_intersection (PointProcess me, PointProcess thee) {
	try {
		autoPointProcess him = Data_copy (me);
		if (thy xmin > my xmin)
			his xmin = thy xmin;
		if (thy xmax < my xmax)
			his xmax = thy xmax;
		for (integer i = my nt; i >= 1; i --)
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
		for (integer i = my nt; i >= 1; i --)
			if (PointProcess_findPoint (thee, my t [i]))
				PointProcess_removePoint (him.get(), i);
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": difference not computed.");
	}
}

void PointProcess_fill (PointProcess me, double tmin, double tmax, double period) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		const integer n = Melder_ifloor ((tmax - tmin) / period);
		double t = 0.5 * (tmin + tmax - n * period);
		for (integer i = 1; i <= n; i ++, t += period)
			PointProcess_addPoint (me, t);
	} catch (MelderError) {
		Melder_throw (me, U": not filled.");
	}
}

void PointProcess_voice (PointProcess me, double period, double maxT) {
	try {
		integer ipointright;
		double beginVoiceless = my xmin, endVoiceless;
		for (integer ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
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

MelderIntegerRange PointProcess_getWindowPoints (PointProcess me, double tmin, double tmax) {
	return { PointProcess_getHighIndex (me, tmin), PointProcess_getLowIndex (me, tmax) };
}

static bool PointProcess_isPeriod (PointProcess me, integer ileft, double minimumPeriod, double maximumPeriod, double maximumPeriodFactor) {
	/*
		This function answers the question: is the interval from point 'ileft' to point 'ileft+1' a period?
	*/
	const integer iright = ileft + 1;
	/*
		Period condition 1: both 'ileft' and 'iright' have to be within the point process.
	*/
	if (ileft < 1 || iright > my nt)
		return false;
	/*
		Period condition 2: the interval has to be within the boundaries, if specified.
	*/
	if (minimumPeriod == maximumPeriod)   // special input setting (typically both zero)
		return true;   // all intervals count as periods, irrespective of absolute size and relative size
	const double interval = my t [iright] - my t [ileft];
	if (interval <= 0.0 || interval < minimumPeriod || interval > maximumPeriod)
		return false;
	if (isundef (maximumPeriodFactor) || maximumPeriodFactor < 1.0)
		return true;
	/*
		Period condition 3: the interval cannot be too different from both of its neigbours, if any.
	*/
	const double previousInterval = ( ileft <= 1 ? undefined : my t [ileft] - my t [ileft - 1] );
	const double nextInterval = ( iright >= my nt ? undefined : my t [iright + 1] - my t [iright] );
	double previousIntervalFactor =
		( isdefined (previousInterval) && previousInterval > 0.0 ? interval / previousInterval : undefined );
	double nextIntervalFactor =
		( isdefined (nextInterval) && nextInterval > 0.0 ? interval / nextInterval : undefined );
	if (isundef (previousIntervalFactor) && isundef (nextIntervalFactor))
		return true;   // no neighbours: this is a period
	if (isdefined (previousIntervalFactor) && previousIntervalFactor > 0.0 && previousIntervalFactor < 1.0)
		previousIntervalFactor = 1.0 / previousIntervalFactor;
	if (isdefined (nextIntervalFactor) && nextIntervalFactor > 0.0 && nextIntervalFactor < 1.0)
		nextIntervalFactor = 1.0 / nextIntervalFactor;
	if (isdefined (previousIntervalFactor) && previousIntervalFactor > maximumPeriodFactor &&
			isdefined (nextIntervalFactor) && nextIntervalFactor > maximumPeriodFactor)
		return false;
	return true;
}

integer PointProcess_getNumberOfPeriods (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const MelderIntegerRange pointNumbers = PointProcess_getWindowPoints (me, tmin, tmax);
	integer numberOfPeriods = 0;
	for (integer ipoint = pointNumbers.first; ipoint < pointNumbers.last; ipoint ++)
		numberOfPeriods += PointProcess_isPeriod (me, ipoint, minimumPeriod, maximumPeriod, maximumPeriodFactor);
	return numberOfPeriods;
}

double PointProcess_getMeanPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const MelderIntegerRange pointNumbers = PointProcess_getWindowPoints (me, tmin, tmax);
	integer numberOfPeriods = 0;
	longdouble sum = 0.0;
	for (integer ipoint = pointNumbers.first; ipoint < pointNumbers.last; ipoint ++) {
		if (PointProcess_isPeriod (me, ipoint, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			numberOfPeriods ++;
			sum += my t [ipoint + 1] - my t [ipoint];
		}
	}
	return numberOfPeriods > 0 ? double (sum / numberOfPeriods) : undefined;
}

double PointProcess_getStdevPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const MelderIntegerRange pointNumbers = PointProcess_getWindowPoints (me, tmin, tmax);
	integer numberOfPeriods = 0;
	/*
		Compute mean.
	*/
	longdouble sum = 0.0;
	for (integer ipoint = pointNumbers.first; ipoint < pointNumbers.last; ipoint ++) {
		if (PointProcess_isPeriod (me, ipoint, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			numberOfPeriods ++;
			sum += my t [ipoint + 1] - my t [ipoint];
		}
	}
	constexpr integer minimumNumberOfDatapointsToComputeAStandardDeviation = 2;
	if (numberOfPeriods < minimumNumberOfDatapointsToComputeAStandardDeviation)
		return undefined;
	const double mean = double (sum / numberOfPeriods);
	/*
		Compute sum of squares.
	*/
	longdouble sum2 = 0.0;
	for (integer ipoint = pointNumbers.first; ipoint < pointNumbers.last; ipoint ++) {
		if (PointProcess_isPeriod (me, ipoint, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			const double dperiod = my t [ipoint + 1] - my t [ipoint] - mean;
			sum2 += dperiod * dperiod;
		}
	}
	/*
		Compute standard deviation.
	*/
	return sqrt (double (sum2 / (numberOfPeriods - 1)));
}

MelderCountAndFraction PointProcess_getCountAndFractionOfVoiceBreaks (PointProcess me,
	double tmin, double tmax, double maximumPeriod)
{
	MelderCountAndFraction result;
	const MelderIntegerRange pointNumbers = PointProcess_getWindowPoints (me, tmin, tmax);
	if (pointNumbers.size() > 1) {
		result.denominator = tmax - tmin;
		bool previousPeriodVoiced = true;
		for (integer ipoint = pointNumbers.first + 1; ipoint < pointNumbers.last; ipoint ++) {
			const double period = my t [ipoint] - my t [ipoint - 1];
			if (period > maximumPeriod) {
				result.numerator += period;
				if (previousPeriodVoiced) {
					result.count ++;
					previousPeriodVoiced = false;
				}
			} else {
				previousPeriodVoiced = true;
			}
		}
	}
	return result;
}

/* End of file PointProcess.cpp */
