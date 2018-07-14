/* OptimalCeilingTier.cpp
 *
 * Copyright (C) 2015 David Weenink
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

#include "OptimalCeilingTier.h"

Thing_implement (OptimalCeilingTier, RealTier, 0);

autoOptimalCeilingTier OptimalCeilingTier_create (double tmin, double tmax) {
	try {
		autoOptimalCeilingTier me = Thing_new (OptimalCeilingTier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"OptimalCeilingTier not created.");
	}
}

void OptimalCeilingTier_draw (OptimalCeilingTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, U"Sound pressure (Pa)");
}

autoTableOfReal OptimalCeilingTier_downto_TableOfReal (OptimalCeilingTier me) {
	autoTableOfReal thee = RealTier_downto_TableOfReal (me, U"Time (s)", U"Frequency (Hz)");
	return thee;
}

/* End of file OptimalCeilingTier.cpp */
