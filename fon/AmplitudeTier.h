#ifndef _AmplitudeTier_h_
#define _AmplitudeTier_h_
/* AmplitudeTier.h
 *
 * Copyright (C) 2003-2005,2007,2010-2012,2015-2018,2021,2023,2025 Paul Boersma
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

#include "IntensityTier.h"
#include "TableOfReal.h"
#include "Sound.h"

/********** class AmplitudeTier **********/

Thing_define (AmplitudeTier, RealTier) {
	int v_domainQuantity () const
		override { return MelderQuantity_TIME_SECONDS; }
};

autoAmplitudeTier AmplitudeTier_create (double tmin, double tmax);

void AmplitudeTier_draw (AmplitudeTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish);

autoAmplitudeTier PointProcess_upto_AmplitudeTier (PointProcess me, double soundPressure);
autoAmplitudeTier RealTier_to_AmplitudeTier (RealTier me);
autoAmplitudeTier IntensityTier_to_AmplitudeTier (IntensityTier me);
autoIntensityTier AmplitudeTier_to_IntensityTier (AmplitudeTier me, double threshold_dB);
autoTableOfReal AmplitudeTier_downto_TableOfReal (AmplitudeTier me);
void Sound_AmplitudeTier_multiply_inplace (Sound me, AmplitudeTier intensity);
autoSound Sound_AmplitudeTier_multiply (Sound me, AmplitudeTier intensity);

autoAmplitudeTier PointProcess_Sound_to_AmplitudeTier_point (PointProcess me, Sound thee);
autoAmplitudeTier PointProcess_Sound_to_AmplitudeTier_period (PointProcess me, Sound thee,
	double tmin, double tmax, double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/*
	The shimmer measurements return `undefined` if there are too few periods.
*/
double AmplitudeTier_getShimmer_local_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);
double AmplitudeTier_getShimmer_local_dB_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);
double AmplitudeTier_getShimmer_apq3_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);
double AmplitudeTier_getShimmer_apq5_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);
double AmplitudeTier_getShimmer_apq11_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);
double AmplitudeTier_getShimmer_dda_u (AmplitudeTier me, double shortestPeriod, double longestPeriod, double maximumAmplitudeFactor);

autoSound AmplitudeTier_to_Sound (AmplitudeTier me, double samplingFrequency, integer interpolationDepth);

/* End of file AmplitudeTier.h */
#endif
