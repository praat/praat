/* OptimalCeilingTier.cpp
 *
 * Copyright (C) 2015 David Weenink
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

#include "OptimalCeilingTier.h"

Thing_implement (OptimalCeilingTier, RealTier, 0);

OptimalCeilingTier OptimalCeilingTier_create (double tmin, double tmax) {
	try {
		autoOptimalCeilingTier me = Thing_new (OptimalCeilingTier);
		RealTier_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"OptimalCeilingTier not created.");
	}
}

void OptimalCeilingTier_draw (OptimalCeilingTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, const char32 *method, int garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, U"Sound pressure (Pa)");
}

TableOfReal OptimalCeilingTier_downto_TableOfReal (OptimalCeilingTier me) {
	return RealTier_downto_TableOfReal (me, U"Time (s)", U"Frequency (Hz)");
}

/* End of file OptimalCeilingTier.cpp */
