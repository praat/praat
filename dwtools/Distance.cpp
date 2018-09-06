/* Distance.cpp
 *
 * Copyright (C) 1993-2018 David Weenink
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
	double dmax = 0.0;
	for (integer i = 1; i <= my numberOfRows; i ++) { // symmetric matrix
		for (integer j = i + 1; j <= my numberOfColumns; j ++) {
			if (my data [i] [j] > dmax) {
				dmax = my data [i] [j];
			}
		}
	}
	return dmax;
}

autoDistance Configuration_to_Distance (Configuration me) {
	try {
		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, -1);
		for (integer i = 1; i <= thy numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= thy numberOfColumns; j ++) {
				/*
					first divide distance by maximum to prevent overflow when metric is a large number.
					d = (x^n)^(1/n) may overflow if x>1 & n >>1 even if d would not overflow!
					metric changed 24/11/97
					my w [k] * pow (|i-j|) instead of pow (my w [k] * |i-j|)
				*/
				double dmax = 0.0;
				for (integer k = 1; k <= my numberOfColumns; k ++) {
					double dtmp  = fabs (my data [i] [k] - my data [j] [k]);
					if (dtmp > dmax) dmax = dtmp;
				}
				longdouble d = 0.0;
				if (dmax > 0.0) {
					for (integer k = 1; k <= my numberOfColumns; k ++) {
						double arg = fabs (my data [i] [k] - my data [j] [k]) / dmax;
						d += my w [k] * pow (arg, my metric);
					}
				}
				thy data [i] [j] = thy data [j] [i] = dmax * pow ((double) d, 1.0 / my metric);
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
