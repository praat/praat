#ifndef _Cochleagram_h_
#define _Cochleagram_h_
/* Cochleagram.h
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

#include "Matrix.h"

Thing_define (Cochleagram, Matrix) {
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

/* Normally, the attributes will meet the following:
	xmin;			// Start time (seconds).
	xmax;			// End time (seconds).
	nx;				// Number of time slices.
	dx;				// Time step (seconds).
	x1;				// Centre of first time sample (seconds).
	ymin = 0.0;			// Minimum frequency (Bark).
	ymax = 25.6;		// Maximum frequency (Bark).
	ny;				// Number of frequencies.
	dy = 25.6 / ny;		// Frequency step (Bark).
	y1 = 0.5 * dy;		// Centre of first frequency band (Bark).
	z;				// Basilar filter output (milliVolt), or firing rate (Hz), or intensity (phon).
*/

autoCochleagram Cochleagram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double df, integer nf);
/*
	Function:
		return a new instance of Cochleagram.
	Preconditions:
		dt > 0.0;						df > 0.0;
		nt >= 1;						nf >= 1;
	Postconditions:
		result -> xmin == tmin;			result -> ymin == 0.0;
		result -> xmax == tmax;			result -> ymax == 25.6;
		result -> nx == nt;				result -> ny == nf;
		result -> dx == dt;				result -> dy == df;
		result -> x1 == t1;				result -> y1 == 0.5 * df;
		result -> z [1..nf] [1..nt] == 0.0;
*/

void Cochleagram_paint (Cochleagram me, Graphics g, double tmin, double tmax, bool garnish);

double Cochleagram_difference (Cochleagram me, Cochleagram thee, double tmin, double tmax);

autoCochleagram Matrix_to_Cochleagram (Matrix me);
/*
	Function:
		create a Cochleagram from a Matrix,
		with deep copy of all its attributes, except class information and methods.
	Fail if out of memory.
*/

autoMatrix Cochleagram_to_Matrix (Cochleagram me);
/*
	Function:
		create a Matrix from a Cochleagram,
		with deep copy of all its attributes, except class information and methods.
	Fail if out of memory.
*/

/* End of file Cochleagram.h */
#endif
