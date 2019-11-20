/* PatternList.cpp
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
 djmw 20041203 Added _PatternList_checkElements.
 djmw 20071017 Melder_error<p>
  djmw 20110304 Thing_new
*/

#include "PatternList.h"

Thing_implement (PatternList, Matrix, 2);

bool _PatternList_checkElements (PatternList me) {
	for (integer i = 1; i <= my ny; i ++) {
		for (integer j = 1; j <= my nx; j ++)
			if (my z [i] [j] < 0 || my z [i] [j] > 1)
				return false;
	}
	return true;
}

autoPatternList PatternList_create (integer ny, integer nx) {
	try {
		autoPatternList me = Thing_new (PatternList);
		Matrix_init (me.get(), 1, nx, nx, 1, 1, 1, ny, ny, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PatternList not created.");
	}
}

void PatternList_normalize (PatternList me, int choice, double pmin, double pmax) {
	if (pmin == pmax)
		(void) Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & pmin, & pmax);
	if (pmin == pmax)
		return;

	if (choice == 1) {
		for (integer i = 1; i <= my ny; i ++)
			for (integer j = 1; j <= my nx; j ++)
				my z [i] [j] = (my z [i] [j] - pmin) / (pmax - pmin);
	} else { /* default choice */
		for (integer i = 1; i <= my ny; i ++) {
			double sum = 0;
			for (integer j = 1; j <= my nx; j ++)
				sum += (my z [i] [j] -= pmin);
			for (integer j = 1; j <= my nx; j ++)
				my z [i] [j] *= 1.0 / sum;
		}
	}
}

void PatternList_draw (PatternList me, Graphics g, integer pattern, double xmin, double xmax, double ymin, double ymax, bool garnish) {
	Matrix_drawRows (me, g, xmin, xmax, pattern - 0.5, pattern + 0.5, ymin, ymax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoPatternList Matrix_to_PatternList (Matrix me, integer join) {
	try {
		if (join < 1) join = 1;
		Melder_require (my ny % join == 0,
			U"Number of rows should be a multiple of join factor.");
		autoPatternList thee = PatternList_create (my ny / join, join * my nx);

		integer r = 0, c = 1;
		for (integer i = 1; i <= my ny; i ++) {
			if ( (i - 1) % join == 0) {
				r ++;
				c = 1;
			}
			for (integer j = 1; j <= my nx; j ++)
				thy z [r] [c ++] = my z [i] [j];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PatternList.");
	}
}

autoMatrix PatternList_to_Matrix (PatternList me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPatternList ActivationList_to_PatternList (ActivationList me) {
	try {
		autoPatternList thee = Thing_new (PatternList);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PatternList.");
	}
}

/* End of file PatternList.cpp */
