/* NUMinterpol.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018,2020 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2003/06/19 ridders3 replaced with ridders
 * pb 2003/07/09 gsl
 * pb 2007/01/27 use #defines for value interpolation
 * pb 2007/08/20 built a "weird value" check into NUMviterbi (bug report by Adam Jacks)
 * pb 2011/03/29 C++
 */

#include "melder.h"
#include "../dwsys/NUM2.h"

#if defined (__POWERPC__)||1
	#define RECOMPUTE_SINES  0
#else
	#define RECOMPUTE_SINES  1
#endif
double NUM_interpolate_sinc (constVEC const& y, double x, integer maxDepth) {
	const integer midleft = (integer) floor (x), midright = midleft + 1;
	double result = 0.0;
	if (y.size < 1)
		return undefined;   // there exists no best guess
	if (x < 1)
		return y [1];   // offleft: constant extrapolation
	if (x > y.size)
		return y [y.size];   // offright: constant extrapolation
	if (x == midleft)
		return y [midleft];   // the interpolated curve goes through the points
	/*
		1 < x < y.size && x not integer: interpolate.
	*/
	Melder_clipRight (& maxDepth, midright - 1);
	Melder_clipRight (& maxDepth, y.size - midleft);
	if (maxDepth <= NUM_VALUE_INTERPOLATE_NEAREST)
		return y [(integer) floor (x + 0.5)];
	if (maxDepth == NUM_VALUE_INTERPOLATE_LINEAR)
		return y [midleft] + (x - midleft) * (y [midright] - y [midleft]);
	if (maxDepth == NUM_VALUE_INTERPOLATE_CUBIC) {
		const double yl = y [midleft], yr = y [midright];
		const double dyl = 0.5 * (yr - y [midleft - 1]), dyr = 0.5 * (y [midright + 1] - yl);
		const double fil = x - midleft, fir = midright - x;
		return yl * fir + yr * fil - fil * fir * (0.5 * (dyr - dyl) + (fil - 0.5) * (dyl + dyr - 2 * (yr - yl)));
	}
	/*
		maxDepth >= 3: sinc interpolation
	*/
	const integer left = midright - maxDepth;
	const integer right = midleft + maxDepth;
	double a = NUMpi * (x - midleft);
	double halfsina = 0.5 * sin (a);
	double aa = a / (x - left + 1.0);
	double daa = NUMpi / (x - left + 1.0);
	#if ! RECOMPUTE_SINES
		double cosaa = cos (aa);
		double sinaa = sin (aa);
		double cosdaa = cos (daa);
		double sindaa = sin (daa);
	#endif
	for (integer ix = midleft; ix >= left; ix --) {
		#if RECOMPUTE_SINES
			const double d = halfsina / a * (1.0 + cos (aa));
		#else
			const double d = halfsina / a * (1.0 + cosaa);
		#endif
		result += y [ix] * d;
		a += NUMpi;
		#if RECOMPUTE_SINES
			aa += daa;
		#else
			const double help = cosaa * cosdaa - sinaa * sindaa;
			sinaa = cosaa * sindaa + sinaa * cosdaa;
			cosaa = help;
		#endif
		halfsina = - halfsina;
	}
	a = NUMpi * (midright - x);
	halfsina = 0.5 * sin (a);
	aa = a / (right - x + 1.0);
	daa = NUMpi / (right - x + 1.0);
	#if ! RECOMPUTE_SINES
		cosaa = cos (aa);
		sinaa = sin (aa);
		cosdaa = cos (daa);
		sindaa = sin (daa);
	#endif
	for (integer ix = midright; ix <= right; ix ++) {
		#if RECOMPUTE_SINES
			const double d = halfsina / a * (1.0 + cos (aa));
		#else
			const double d = halfsina / a * (1.0 + cosaa);
		#endif
		result += y [ix] * d;
		a += NUMpi;
		#if RECOMPUTE_SINES
			aa += daa;
		#else
			const double help = cosaa * cosdaa - sinaa * sindaa;
			sinaa = cosaa * sindaa + sinaa * cosdaa;
			cosaa = help;
		#endif
		halfsina = - halfsina;
	}
	return result;
}

/********** Improving extrema **********/
#pragma mark Improving extrema

struct improve_params {
	integer depth;
	constVEC y;
	bool isMaximum;
};

static double improve_evaluate (double x, void *closure) {
	struct improve_params *me = (struct improve_params *) closure;
	const double y = NUM_interpolate_sinc (my y, x, my depth);
	return my isMaximum ? - y : y;
}

double NUMimproveExtremum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real, bool isMaximum) {
	struct improve_params params;
	double result;
	if (ixmid <= 1) {
		*ixmid_real = double (1);
		return y [1];
	}
	if (ixmid >= y.size) {
		*ixmid_real = double (y.size);
		return y [y.size];
	}
	if (interpolationDepth <= NUM_PEAK_INTERPOLATE_NONE) {
		*ixmid_real = double (ixmid);
		return y [ixmid];
	}
	if (interpolationDepth == NUM_PEAK_INTERPOLATE_PARABOLIC) {
		const double dy = 0.5 * (y [ixmid + 1] - y [ixmid - 1]);
		const double d2y = 2 * y [ixmid] - y [ixmid - 1] - y [ixmid + 1];
		*ixmid_real = ixmid + dy / d2y;
		return y [ixmid] + 0.5 * dy * dy / d2y;
	}
	/*
		Cubic or sinc interpolation.
	*/
	params. depth = (
		interpolationDepth == NUM_PEAK_INTERPOLATE_CUBIC ? NUM_VALUE_INTERPOLATE_CUBIC :
		interpolationDepth == NUM_PEAK_INTERPOLATE_SINC70 ? NUM_VALUE_INTERPOLATE_SINC70 :
		NUM_VALUE_INTERPOLATE_SINC700
	);
	params. y = y;
	params. isMaximum = isMaximum;
	*ixmid_real = NUMminimize_brent (improve_evaluate, ixmid - 1, ixmid + 1, & params, 1e-10, & result);
	return isMaximum ? - result : result;
}

double NUMimproveMinimum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real) {
	return NUMimproveExtremum (y, ixmid, interpolationDepth, ixmid_real, false);
}
double NUMimproveMaximum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real) {
	return NUMimproveExtremum (y, ixmid, interpolationDepth, ixmid_real, true);
}

/********** Viterbi **********/

void NUM_viterbi (
	integer numberOfFrames, integer maxnCandidates,
	integer (*getNumberOfCandidates) (integer iframe, void *closure),
	double (*getLocalCost) (integer iframe, integer icand, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, void *closure),
	void (*putResult) (integer iframe, integer place, void *closure),
	void *closure)
{
	autoMAT delta = raw_MAT (numberOfFrames, maxnCandidates);
	autoINTMAT psi = raw_INTMAT (numberOfFrames, maxnCandidates);
	autoINTVEC numberOfCandidates = raw_INTVEC (numberOfFrames);
	for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
		numberOfCandidates [iframe] = getNumberOfCandidates (iframe, closure);
		for (integer icand = 1; icand <= numberOfCandidates [iframe]; icand ++)
			delta [iframe] [icand] = - getLocalCost (iframe, icand, closure);
	}
	for (integer iframe = 2; iframe <= numberOfFrames; iframe ++) {
		for (integer icand2 = 1; icand2 <= numberOfCandidates [iframe]; icand2 ++) {
			double maximum = -1e308;
			integer place = 0;
			for (integer icand1 = 1; icand1 <= numberOfCandidates [iframe - 1]; icand1 ++) {
				const double value = delta [iframe - 1] [icand1] + delta [iframe] [icand2]
						- getTransitionCost (iframe, icand1, icand2, closure);
				if (value > maximum) {
					maximum = value;
					place = icand1;
				}
			}
			if (place == 0)
				Melder_throw (U"Viterbi algorithm cannot compute a track because of weird values.");
			delta [iframe] [icand2] = maximum;
			psi [iframe] [icand2] = place;
		}
	}
	/*
		Find the end of the most probable path.
	*/
	integer place;
	double maximum = delta [numberOfFrames] [place = 1];
	for (integer icand = 2; icand <= numberOfCandidates [numberOfFrames]; icand ++) {
		if (delta [numberOfFrames] [icand] > maximum)
			maximum = delta [numberOfFrames] [place = icand];
	}
	/*
		Backtrack.
	*/
	for (integer iframe = numberOfFrames; iframe >= 1; iframe --) {
		putResult (iframe, place, closure);
		place = psi [iframe] [place];
	}
}

/******************/

struct parm2 {
	integer ntrack;
	integer ncomb;
	INTMAT indices;
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure);
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure);
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure);
	void *closure;
};

static integer getNumberOfCandidates_n (integer iframe, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	(void) iframe;
	return my ncomb;
}
static double getLocalCost_n (integer iframe, integer jcand, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double localCost = 0.0;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		localCost += my getLocalCost (iframe, my indices [jcand] [itrack], itrack, my closure);
	return localCost;
}
static double getTransitionCost_n (integer iframe, integer jcand1, integer jcand2, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	double transitionCost = 0.0;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		transitionCost += my getTransitionCost (iframe,
			my indices [jcand1] [itrack], my indices [jcand2] [itrack], itrack, my closure);
	return transitionCost;
}
static void putResult_n (integer iframe, integer jplace, void *closure) {
	struct parm2 *me = (struct parm2 *) closure;
	for (integer itrack = 1; itrack <= my ntrack; itrack ++)
		my putResult (iframe, my indices [jplace] [itrack], itrack, my closure);
}

void NUM_viterbi_multi (
	integer nframe, integer ncand, integer ntrack,
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure),
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure),
	void *closure)
{
	struct parm2 parm;

	if (ntrack > ncand) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Number of tracks (", ntrack, U") should not exceed number of candidates (", ncand, U").");
	const integer ncomb = Melder_iround (NUMcombinations (ncand, ntrack));
	if (ncomb > 10'000'000) Melder_throw (U"(NUM_viterbi_multi:) "
		U"Unrealistically high number of combinations (", ncomb, U").");
	parm. ntrack = ntrack;
	parm. ncomb = ncomb;

	/*
		For ncand == 5 and ntrack == 3, parm.indices is going to contain:
			1 2 3
			1 2 4
			1 2 5
			1 3 4
			1 3 5
			1 4 5
			2 3 4
			2 3 5
			2 4 5
			3 4 5
	*/
	autoINTMAT indices = zero_INTMAT (ncomb, ntrack);
	autoINTVEC icand = to_INTVEC (ntrack);   // start out with "1 2 3"
	integer jcomb = 0;
	for (;;) {
		jcomb ++;
		for (integer itrack = 1; itrack <= ntrack; itrack ++)
			indices [jcomb] [itrack] = icand [itrack];
		integer itrack = ntrack;
		for (; itrack >= 1; itrack --) {
			if (++ icand [itrack] <= ncand - (ntrack - itrack)) {
				for (integer jtrack = itrack + 1; jtrack <= ntrack; jtrack ++)
					icand [jtrack] = icand [itrack] + jtrack - itrack;
				break;
			}
		}
		if (itrack == 0) break;
	}
	Melder_assert (jcomb == ncomb);
	parm. indices = indices.get();
	parm. getLocalCost = getLocalCost;
	parm. getTransitionCost = getTransitionCost;
	parm. putResult = putResult;
	parm. closure = closure;
	NUM_viterbi (nframe, ncomb, getNumberOfCandidates_n, getLocalCost_n, getTransitionCost_n, putResult_n, & parm);
}

/* End of file NUMinterpol.cpp */
