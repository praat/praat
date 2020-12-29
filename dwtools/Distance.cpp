/* Distance.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020813 GPL header
 djmw 20110304 Thing_new
*/

#include "Distance.h"
#include "TableOfReal_extensions.h"

Thing_implement (Distance, Proximity, 0);

Thing_implement (DistanceList, ProximityList, 0);

autoDistance Distance_create (integer numberOfPoints) {
	try {
		autoDistance me = Thing_new (Distance);
		Proximity_init (me.get(), numberOfPoints);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Distance not created.");
	}
}

double Distance_getMaximumDistance (Distance me) {
	return NUMmax (my data.get());
}

static void VECpow (VECVU const& v, double power) {
	for (integer i = 1; i <= v.size; i++)
		v [i] = pow (v [i], power);
}

autoDistance Configuration_to_Distance (Configuration me) {
	try {
		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, -1);
		autoVEC dist = raw_VEC (my numberOfColumns);
		for (integer i = 1; i <= thy numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= thy numberOfColumns; j ++) {
				dist.all() <<= my data.row (i)  -  my data.row (j);
				VECabs_inplace (dist.get());
				const double dmax = NUMmax (dist.get());
				double d = 0.0;
				if (dmax > 0.0) {
					dist.all()  /=  dmax;   // prevent overflow
					VECpow (dist.get(), my metric);
					d = NUMinner (my w.all(), dist.get());
					d = dmax * pow (d, 1.0 / my metric);   // scale back
				}
				thy data [i] [j] = thy data [j] [i] = d;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Distance created.");
	}
}

void Distance_drawDendogram (Distance me, Graphics g, int method) {
	(void) me;
	(void) g;
	(void) method;
}

/* End of file Distance.cpp */
