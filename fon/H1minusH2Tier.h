#ifndef _H1minusH2Tier_h_
#define _H1minusH2Tier_h_
/* H1minusH2Tier.h
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

#include "RealTier.h"
#include "Sound.h"

/********** class H1minusH2Tier **********/

Thing_define (H1minusH2Tier, RealTier) {
	int v_domainQuantity () const
		override { return MelderQuantity_TIME_SECONDS; }
};

autoH1minusH2Tier H1minusH2Tier_create (double tmin, double tmax);

void H1minusH2Tier_draw (H1minusH2Tier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish);

autoH1minusH2Tier PointProcess_upto_H1minusH2Tier (PointProcess me, double soundPressure);
autoH1minusH2Tier RealTier_to_H1minusH2Tier (RealTier me);

autoH1minusH2Tier PointProcess_Sound_to_H1minusH2Tier (PointProcess me, Sound thee,
	double tmin, double tmax, double shortestPeriod, double longestPeriod, double maximumPeriodFactor);

/* End of file H1minusH2Tier.h */
#endif
