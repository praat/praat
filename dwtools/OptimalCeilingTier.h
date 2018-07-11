#ifndef _OptimalCeilingTier_h_
#define _OptimalCeilingTier_h_
/* OptimalCeilingTier.h
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

#include "IntensityTier.h"
#include "TableOfReal.h"
#include "Sound.h"

/********** class OptimalCeilingTier **********/

Thing_define (OptimalCeilingTier, RealTier) {
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoOptimalCeilingTier OptimalCeilingTier_create (double tmin, double tmax);

void OptimalCeilingTier_draw (OptimalCeilingTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish);

autoTableOfReal OptimalCeilingTier_downto_TableOfReal (OptimalCeilingTier me);

/* End of file OptimalCeilingTier.h */
#endif
