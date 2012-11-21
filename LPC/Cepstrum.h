#ifndef _Cepstrum_h_
#define _Cepstrum_h_
/* Cepstrum.h
 *
 * Copyright (C) 1994-2012 David Weenink
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
 djmw 20010111
 djmw 20020812 GPL header
 djmw 20121117 Latest modification.
*/

/*
	The Cepstrum is a sequence of REAL numbers.
	It is the power spectrum of the power spectrum of a (sound) signal.
*/

#include "Matrix.h"

Thing_define (Cepstrum, Matrix) {
};
/*
	xmin		// Lowest quefrency.
	xmax		// Highest quefrency.
	nx			// Number of quefrencies.
	dx			// Quefrency step.
	x1			// First quefrency.

	ymin = ymax = dy = y1 = 1
	ny = 1
*/

Cepstrum Cepstrum_create (double qmin, double qmax, long nq);
/* Preconditions:
		qmin < qmax;
		nq >= 2;
	Postconditions:
		my xmin = qmin;				my ymin = 1;
		my xmax = qmax;				my ymax = 1;
		my nx = nq;					my ny = 1;
		my dx = qmax / nx;			my dy = 1;
		my x1 = my dx / 2;			my y1 = 1;
		my z [1..ny] [1..nx] = 0.0;
*/

void Cepstrum_draw (Cepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish);
void Cepstrum_drawLinear (Cepstrum me, Graphics g, double qmin, double qmax, double minimum, double maximum, int garnish);
void Cepstrum_drawTiltLine (Cepstrum me, Graphics g, double qmin, double qmax, double dBminimum, double dBmaximum, double qstart, double qend, int method);
/*
	Function:
		Draw a Cepstrum
	Preconditions:
		maximum > minimum;
	Arguments:
		[qmin, qmax]: quefrencies; x domain of drawing;
		Autowindowing: if qmax <= qmin, x domain of drawing is
			[my xmin, my xmax].
		[minimum, maximum]: amplitude; y range of drawing.
*/

double Cepstrum_getPeakProminence (Cepstrum me, double search_lowestQuefrency, double search_highestQuefrency, int interpolation, double fit_lowestFrequency, double fit_highestFrequency, int fitmethod, double *qpeak);
void Cepstrum_fitTiltLine (Cepstrum me, double qmin, double qmax, double *a, double *intercept, int method);

Matrix Cepstrum_to_Matrix (Cepstrum me);
Cepstrum Matrix_to_Cepstrum (Matrix me, long row);

#endif /* _Cepstrum_h_ */
