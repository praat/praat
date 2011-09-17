/* Pattern.c
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
 djmw 20041203 Added _Pattern_checkElements.
 djmw 20071017 Melder_error<p>
  djmw 20110304 Thing_new
*/

#include "Pattern.h"

Thing_implement (Pattern, Matrix, 2);

int _Pattern_checkElements (Pattern me) {
	for (long i = 1; i <= my ny; i++) {
		for (long j = 1; j <= my nx; j++) {
			if (my z[i][j] < 0 || my z[i][j] > 1) {
				return 0;
			}
		}
	}
	return 1;
}

void Pattern_init (I, long ny, long nx) {
	iam (Pattern);
	my ny = ny;
	my nx = nx;
	Matrix_init (me, 1, nx, nx, 1, 1, 1, ny, ny, 1, 1);
}

Pattern Pattern_create (long ny, long nx) {
	try {
		autoPattern me = Thing_new (Pattern);
		Pattern_init (me.peek(), ny, nx);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Pattern not created.");
	}
}

void Pattern_normalize (I, int choice, double pmin, double pmax) {
	iam (Pattern);

	if (pmin == pmax) {
		(void) Matrix_getWindowExtrema (me, 1, my nx, 1, my ny, & pmin, & pmax);
	}
	if (pmin == pmax) {
		return;
	}

	if (choice == 1) {
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++) {
				my z[i][j] = (my z[i][j] - pmin) / (pmax - pmin);
			}
		}
	} else { /* default choice */
		for (long i = 1; i <= my ny; i++) {
			double sum = 0;
			for (long j = 1; j <= my nx; j++) {
				sum += (my z[i][j] -= pmin);
			}
			for (long j = 1; j <= my nx; j++) {
				my z[i][j] *= 1.0 / sum;
			}
		}
	}
}

void Pattern_draw (I, Graphics g, long pattern, double xmin, double xmax, double ymin, double ymax, int garnish) {
	iam (Pattern);
	Matrix_drawRows (me, g, xmin, xmax, pattern - 0.5, pattern + 0.5, ymin, ymax);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

Pattern Matrix_to_Pattern (I, int join) {
	iam (Matrix);
	try {
		if (join < 1) {
			join = 1;
		}
		if ( (my ny % join) != 0) {
			Melder_throw (L"Number of rows is not a multiple of join factor.");
		}

		autoPattern thee = Pattern_create (my ny / join, join * my nx);

		long r = 0, c = 1;
		for (long i = 1; i <= my ny; i++) {
			if ( (i - 1) % join == 0) {
				r++;
				c = 1;
			}
			for (long j = 1; j <= my nx; j++) {
				thy z[r][c++] = my z[i][j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Pattern created.");
	}
}

Matrix Pattern_to_Matrix (Pattern me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of file Pattern.cpp */
