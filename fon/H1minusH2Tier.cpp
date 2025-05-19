/* H1minusH2Tier.cpp
 *
 * Copyright (C) 2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#include "H1minusH2Tier.h"

Thing_implement (H1minusH2Tier, RealTier, 0);

autoH1minusH2Tier H1minusH2Tier_create (double tmin, double tmax) {
	try {
		autoH1minusH2Tier me = Thing_new (H1minusH2Tier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"H1minusH2Tier not created.");
	}
}

void H1minusH2Tier_draw (H1minusH2Tier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, U"H1-H2 (dB)");
}

autoH1minusH2Tier RealTier_to_H1minusH2Tier (RealTier me) {
	try {
		autoH1minusH2Tier thee = Thing_new (H1minusH2Tier);
		my structRealTier :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to H1minusH2Tier.");
	}
}

static double Sound_getHarmonicStrength (Sound me, double tmin, double tmax, integer harmonic) {
	integer imin, imax;
	const integer n = Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	if (n < 3)
		return undefined;
	autoVEC cosine = raw_VEC (n), sine = raw_VEC (n);
	for (integer i = 1; i <= n; i ++) {
		const double phase = harmonic * NUM2pi * (i - 0.5) / n;
		cosine [i] = cos (phase);
		sine [i] = sin (phase);
	}
	longdouble re = 0.0, im = 0.0;
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		re += NUMinner (my z.row (ichan).part (imin, imax), cosine.all());
		im += NUMinner (my z.row (ichan).part (imin, imax), sine.all());
	}
	return sqr (double (re)) + sqr (double (im));
}
autoH1minusH2Tier PointProcess_Sound_to_H1minusH2Tier (PointProcess me, Sound you, double tmin, double tmax,
	double pmin, double pmax, double maximumPeriodFactor)
{
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		const MelderIntegerRange peaks = PointProcess_getWindowPoints (me, tmin, tmax);
		if (peaks.size() < 3)
			Melder_throw (U"Too few pulses between ", tmin, U" and ", tmax, U" seconds.");
		autoH1minusH2Tier him = H1minusH2Tier_create (tmin, tmax);
		for (integer i = peaks.first + 1; i <= peaks.last; i ++) {
			const double period = my t [i] - my t [i - 1];
			if (pmin == pmax || (period >= pmin && period <= pmax)) {
				const double h1_Pa2 = Sound_getHarmonicStrength (you, my t [i - 1], my t [i], 1);
				const double h2_Pa2 = Sound_getHarmonicStrength (you, my t [i - 1], my t [i], 2);
				const double h1minusH2 = 10.0 * log10 (h1_Pa2 / h2_Pa2);
				if (isdefined (h1minusH2))
					RealTier_addPoint (him.get(), 0.5 * (my t [i - 1] + my t [i]), h1minusH2);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", you, U": not converted to H1minusH2Tier.");
	}
}

/* End of file H1minusH2Tier.cpp */
