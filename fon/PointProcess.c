/* PointProcess.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/03/08 speed up creation of Poisson process
 * pb 2002/03/08 GPL
 * pb 2002/03/08 getMeanPeriod, getStdevPeriod
 * pb 2003/07/20 moved jitter measurements to VoiceAnalysis.c
 * pb 2004/04/16 addPoint tests uncrashingly for undefined
 * pb 2004/04/16 added maximum period factor
 * pb 2004/07/09 better period counting
 * pb 2006/12/10 MelderInfo
 * pb 2007/03/17 domain quantity
 * pb 2007/10/01 can write as encoding
 * pb 2008/09/20 shiftX
 * pb 2008/09/23 scaleX
 * pb 2010/03/24 make sure that addPoint cannot add a point where there is already a point
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

static void infoPeriods (PointProcess me, double shortestPeriod, double longestPeriod, double maximumPeriodFactor, int precision) {
	long numberOfPeriods = PointProcess_getNumberOfPeriods (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double meanPeriod = PointProcess_getMeanPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double stdevPeriod = PointProcess_getStdevPeriod (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_local = PointProcess_getJitter_local (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_local_absolute = PointProcess_getJitter_local_absolute (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_rap = PointProcess_getJitter_rap (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_ppq5 = PointProcess_getJitter_ppq5 (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	double jitter_ddp = PointProcess_getJitter_ddp (me, 0.0, 0.0, shortestPeriod, longestPeriod, maximumPeriodFactor);
	MelderInfo_writeLine2 (L"     Number of periods: ", Melder_integer (numberOfPeriods));
	MelderInfo_writeLine3 (L"     Mean period: ", Melder_double (meanPeriod), L" seconds");
	MelderInfo_writeLine3 (L"     Stdev period: ", Melder_double (stdevPeriod), L" seconds");
	MelderInfo_writeLine2 (L"     Jitter (local): ", Melder_percent (jitter_local, precision));
	MelderInfo_writeLine3 (L"     Jitter (local, absolute): ", Melder_fixedExponent (jitter_local_absolute, -6, precision), L" seconds");
	MelderInfo_writeLine2 (L"     Jitter (rap): ", Melder_percent (jitter_rap, precision));
	MelderInfo_writeLine2 (L"     Jitter (ppq5): ", Melder_percent (jitter_ppq5, precision));
	MelderInfo_writeLine2 (L"     Jitter (ddp): ", Melder_percent (jitter_ddp, precision));
}

static void info (I) {
	iam (PointProcess);
	classData -> info (me);
	MelderInfo_writeLine1 (L"Time domain:");
	MelderInfo_writeLine3 (L"   Start time: ", Melder_double (my xmin), L" seconds");
	MelderInfo_writeLine3 (L"   End time: ", Melder_double (my xmax), L" seconds");
	MelderInfo_writeLine3 (L"   Total duration: ", Melder_double (my xmax - my xmin), L" seconds");
	MelderInfo_writeLine2 (L"Number of times: ", Melder_integer (my nt));
	if (my nt) {
		MelderInfo_writeLine3 (L"First time: ", Melder_double (my t [1]), L" seconds");
		MelderInfo_writeLine3 (L"Last time: ", Melder_double (my t [my nt]), L" seconds");
	}
	MelderInfo_writeLine1 (L"Periods between 0.1 ms and 20 ms (pitch between 50 and 10000 Hz),");
	MelderInfo_writeLine1 (L"with a maximum \"period factor\" of 1.3:");
	infoPeriods (me, 1e-4, 20e-3, 1.3, 3);
	MelderInfo_writeLine1 (L"All periods:");
	infoPeriods (me, 0.0, 0.0, 1e300, 6);
}

static void shiftX (I, double xfrom, double xto) {
	iam (PointProcess);
	inherited (PointProcess) shiftX (me, xfrom, xto);
	for (long i = 1; i <= my nt; i ++) {
		NUMshift (& my t [i], xfrom, xto);
	}
}

static void scaleX (I, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	iam (PointProcess);
	inherited (PointProcess) scaleX (me, xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= my nt; i ++) {
		NUMscale (& my t [i], xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

class_methods (PointProcess, Function) {
	class_method_local (PointProcess, destroy)
	class_method (info)
	class_method_local (PointProcess, description)
	class_method_local (PointProcess, copy)
	class_method_local (PointProcess, equal)
	class_method_local (PointProcess, canWriteAsEncoding)
	class_method_local (PointProcess, writeText)
	class_method_local (PointProcess, readText)
	class_method_local (PointProcess, writeBinary)
	class_method_local (PointProcess, readBinary)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method (shiftX)
	class_method (scaleX)
	class_methods_end
}

int PointProcess_init (I, double tmin, double tmax, long initialMaxnt) {
	iam (PointProcess);
	if (! Function_init (me, tmin, tmax)) return 0;
	if (initialMaxnt < 1) initialMaxnt = 1;
	my maxnt = initialMaxnt;
	my nt = 0;
	if (! (my t = NUMdvector (1, my maxnt))) return 0;
	return 1;
}

PointProcess PointProcess_create (double tmin, double tmax, long initialMaxnt) {
	PointProcess me = new (PointProcess);
	if (! me || ! PointProcess_init (me, tmin, tmax, initialMaxnt)) forget (me);
	return me;
}

PointProcess PointProcess_createPoissonProcess (double startingTime, double finishingTime, double density) {
	long nt = NUMrandomPoisson ((finishingTime - startingTime) * density), i;
	PointProcess me = PointProcess_create (startingTime, finishingTime, nt);
	if (! me) return NULL;
	my nt = nt;
	for (i = 1; i <= nt; i ++)
		my t [i] = NUMrandomUniform (startingTime, finishingTime);
	NUMsort_d (my nt, my t);
	return me;
}

long PointProcess_getLowIndex (PointProcess me, double t) {
	long left, right;
	if (my nt == 0 || t < my t [1])
		return 0;
	if (t >= my t [my nt])   /* Special case that often occurs in practice. */
		return my nt;
	Melder_assert (my nt != 1);   /* May fail if t or my t [1] is NaN. */
	/* Start binary search. */
	left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t >= my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return left;
}

long PointProcess_getHighIndex (PointProcess me, double t) {
	long left, right;
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t > my t [my nt])
		return my nt + 1;
	/* Start binary search. */
	left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t > my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return right;
}

long PointProcess_getNearestIndex (PointProcess me, double t) {
	long left, right;
	if (my nt == 0)
		return 0;
	if (t <= my t [1])
		return 1;
	if (t >= my t [my nt])
		return my nt;
	/* Start binary search. */
	left = 1, right = my nt;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (t >= my t [mid]) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return t - my t [left] < my t [right] - t ? left : right;
}

int PointProcess_addPoint (PointProcess me, double t) {
	if (t == NUMundefined) {
		return Melder_error1 (L"Cannot add a point at an undefined time.");
	}
	if (my nt >= my maxnt) {
		double *dum = NUMdvector (1, 2 * my maxnt);
		if (! dum) return 0;
		NUMdvector_copyElements (my t, dum, 1, my nt);
		NUMdvector_free (my t, 1);   // dangle
		my t = dum;   // undangle
		my maxnt *= 2;
	}
	if (my nt == 0 || t >= my t [my nt]) {   /* Special case that often occurs in practice. */
		my t [++ my nt] = t;
	} else {
		long left = PointProcess_getLowIndex (me, t);
		if (left == 0 || my t [left] != t) {
			for (long i = my nt; i > left; i --) my t [i + 1] = my t [i];
			my nt ++;
			my t [left + 1] = t;
		}
	}
	return 1;
}

void PointProcess_removePoint (PointProcess me, long index) {
	long i;
	if (index < 1 || index > my nt) return;
	for (i = index; i < my nt; i ++)
		my t [i] = my t [i + 1];
	my nt --;
}

void PointProcess_removePointNear (PointProcess me, double t) {
	PointProcess_removePoint (me, PointProcess_getNearestIndex (me, t));
}

void PointProcess_removePoints (PointProcess me, long first, long last) {
	long i, distance;
	if (first < 1) first = 1;
	if (last > my nt) last = my nt;
	if ((distance = last - first + 1) <= 0) return;
	for (i = first + distance; i <= my nt; i ++)
		my t [i - distance] = my t [i];
	my nt -= distance;
}

void PointProcess_removePointsBetween (PointProcess me, double tmin, double tmax) {
	PointProcess_removePoints (me, PointProcess_getHighIndex (me, tmin), PointProcess_getLowIndex (me, tmax));
}

void PointProcess_draw (PointProcess me, Graphics g, double tmin, double tmax, int garnish) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Graphics_setWindow (g, tmin, tmax, -1, 1);
	if (my nt) {
		long imin = PointProcess_getHighIndex (me, tmin), imax = PointProcess_getLowIndex (me, tmax), i;
		int lineType = Graphics_inqLineType (g);
		Graphics_setLineType (g, Graphics_DOTTED);
		Graphics_setInner (g);
		for (i = imin; i <= imax; i ++) {
			Graphics_line (g, my t [i], -1, my t [i], 1);
		}
		Graphics_setLineType (g, lineType);
		Graphics_unsetInner (g);
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
	}
}

double PointProcess_getInterval (PointProcess me, double t) {
	long ileft = PointProcess_getLowIndex (me, t);
	if (ileft <= 0 || ileft >= my nt) return NUMundefined;
	return my t [ileft + 1] - my t [ileft];
}

PointProcess PointProcesses_union (PointProcess me, PointProcess thee) {
	PointProcess him = Data_copy (me);
	long i;
	if (thy xmin < my xmin) his xmin = thy xmin;
	if (thy xmax > my xmax) his xmax = thy xmax;
	for (i = 1; i <= thy nt; i ++)
		if (! PointProcess_addPoint (him, thy t [i])) { forget (him); return NULL; }
	return him;
}

long PointProcess_findPoint (PointProcess me, double t) {
	long left = 1, right = my nt;
	if (my nt == 0) return 0;
	if (t < my t [left] || t > my t [right]) return 0;
	while (left < right - 1) {
		long mid = (left + right) / 2;   /* tleft <= t <= tright */
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

PointProcess PointProcesses_intersection (PointProcess me, PointProcess thee) {
	PointProcess him = Data_copy (me);
	long i;
	if (thy xmin > my xmin) his xmin = thy xmin;
	if (thy xmax < my xmax) his xmax = thy xmax;
	for (i = my nt; i >= 1; i --)
		if (! PointProcess_findPoint (thee, my t [i]))
			PointProcess_removePoint (him, i);
	return him;
}

PointProcess PointProcesses_difference (PointProcess me, PointProcess thee) {
	PointProcess him = Data_copy (me);
	long i;
	for (i = my nt; i >= 1; i --)
		if (PointProcess_findPoint (thee, my t [i]))
			PointProcess_removePoint (him, i);
	return him;
}

int PointProcess_fill (PointProcess me, double tmin, double tmax, double period) {
	long n, i;
	double t;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	n = floor ((tmax - tmin) / period);
	t = 0.5 * (tmin + tmax - n * period);
	for (i = 1, t = 0.5 * (tmin + tmax - n * period); i <= n; i ++, t += period)
		if (! PointProcess_addPoint (me, t)) return 0;
	return 1;
}

int PointProcess_voice (PointProcess me, double period, double maxT) {
	long ipointleft, ipointright;
	double beginVoiceless = my xmin, endVoiceless;
	for (ipointleft = 1; ipointleft <= my nt; ipointleft = ipointright + 1) {
		endVoiceless = my t [ipointleft];
		if (! PointProcess_fill (me, beginVoiceless, endVoiceless, period)) return 0;
		for (ipointright = ipointleft + 1; ipointright <= my nt; ipointright ++)
			if (my t [ipointright] - my t [ipointright - 1] > maxT)
				break;
		ipointright --;
		beginVoiceless = my t [ipointright] + 0.005;
	}
	endVoiceless = my xmax;
	PointProcess_fill (me, beginVoiceless, endVoiceless, period);
	return 1;
}

long PointProcess_getWindowPoints (PointProcess me, double tmin, double tmax, long *pimin, long *pimax) {
	long imin = PointProcess_getHighIndex (me, tmin);
	long imax = PointProcess_getLowIndex (me, tmax);
	if (pimin) *pimin = imin;
	if (pimax) *pimax = imax;
	return imax - imin + 1;
}

static int PointProcess_isPeriod (PointProcess me, long ileft, double minimumPeriod, double maximumPeriod, double maximumPeriodFactor) {
	/*
	 * This function answers the question: is the interval from point 'ileft' to point 'ileft+1' a period?
	 */
	long iright = ileft + 1;
	/*
	 * Period condition 1: both 'ileft' and 'iright' have to be within the point process.
	 */
	if (ileft < 1 || iright > my nt) {
		return FALSE;
	} else {
		/*
		 * Period condition 2: the interval has to be within the boundaries, if specified.
		 */
		if (minimumPeriod == maximumPeriod) {
			return TRUE;   /* All intervals count as periods, irrespective of absolute size and relative size. */
		} else {
			double interval = my t [iright] - my t [ileft];
			if (interval <= 0.0 || interval < minimumPeriod || interval > maximumPeriod) {
				return FALSE;
			} else if (! NUMdefined (maximumPeriodFactor) || maximumPeriodFactor < 1.0) {
				return TRUE;
			} else {
				/*
				 * Period condition 3: the interval cannot be too different from both of its neigbours, if any.
				 */
				double previousInterval = ileft <= 1 ? NUMundefined : my t [ileft] - my t [ileft - 1];
				double nextInterval = iright >= my nt ? NUMundefined : my t [iright + 1] - my t [iright];
				double previousIntervalFactor = NUMdefined (previousInterval) && previousInterval > 0.0 ? interval / previousInterval : NUMundefined;
				double nextIntervalFactor = NUMdefined (nextInterval) && nextInterval > 0.0 ? interval / nextInterval : NUMundefined;
				if (! NUMdefined (previousIntervalFactor) && ! NUMdefined (nextIntervalFactor)) {
					return TRUE;   /* No neighbours: this is a period. */
				}
				if (NUMdefined (previousIntervalFactor) && previousIntervalFactor > 0.0 && previousIntervalFactor < 1.0) {
					previousIntervalFactor = 1.0 / previousIntervalFactor;
				}
				if (NUMdefined (nextIntervalFactor) && nextIntervalFactor > 0.0 && nextIntervalFactor < 1.0) {
					nextIntervalFactor = 1.0 / nextIntervalFactor;
				}
				if (NUMdefined (previousIntervalFactor) && previousIntervalFactor > maximumPeriodFactor &&
					NUMdefined (nextIntervalFactor) && nextIntervalFactor > maximumPeriodFactor)
				{
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

long PointProcess_getNumberOfPeriods (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	long imin, imax, numberOfPeriods, i;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 1) return 0;
	for (i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			(void) 0;   /* This interval counts as a period. */
		} else {
			numberOfPeriods --;   /* This interval does not count as a period. */
		}
	}
	return numberOfPeriods;
}

double PointProcess_getMeanPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	long imin, imax, numberOfPeriods, i;
	double sum = 0.0;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 1) return NUMundefined;
	for (i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			sum += my t [i + 1] - my t [i];   /* This interval counts as a period. */
		} else {
			numberOfPeriods --;   /* This interval does not count as a period. */
		}
	}
	return numberOfPeriods > 0 ? sum / numberOfPeriods : NUMundefined;
}

double PointProcess_getStdevPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor)
{
	long imin, imax, numberOfPeriods, i;
	double sum = 0.0, sum2 = 0.0, mean;
	if (tmax <= tmin) tmin = my xmin, tmax = my xmax;   /* Autowindowing. */
	numberOfPeriods = PointProcess_getWindowPoints (me, tmin, tmax, & imin, & imax) - 1;
	if (numberOfPeriods < 2) return NUMundefined;
	/*
	 * Compute mean.
	 */
	for (i = imin; i < imax; i ++) {
		if (PointProcess_isPeriod (me, i, minimumPeriod, maximumPeriod, maximumPeriodFactor)) {
			sum += my t [i + 1] - my t [i];   /* This interval counts as a period. */
		} else {
			numberOfPeriods --;   /* This interval does not count as a period. */
		}
	}
	if (numberOfPeriods < 2) return NUMundefined;
	mean = sum / numberOfPeriods;
	/*
	 * Compute variance.
	 */
	for (i = imin; i < imax; i ++) {
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

/* End of file PointProcess.c */
