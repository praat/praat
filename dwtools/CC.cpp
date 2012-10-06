/* CC.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20011016 removed some causes for compiler warnings
 djmw 20020315 GPL header
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 float -> double
 djmw 20080513 CC_getValue
 */

#include "CC.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "CC_def.h"
#include "oo_COPY.h"
#include "CC_def.h"
#include "oo_EQUAL.h"
#include "CC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "CC_def.h"
#include "oo_WRITE_TEXT.h"
#include "CC_def.h"
#include "oo_WRITE_BINARY.h"
#include "CC_def.h"
#include "oo_READ_TEXT.h"
#include "CC_def.h"
#include "oo_READ_BINARY.h"
#include "CC_def.h"
#include "oo_DESCRIPTION.h"
#include "CC_def.h"

Thing_implement (CC, Sampled, 1);

void structCC :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:", Melder_double (xmin), L" to ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine (L"Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"First frame at: ", Melder_double (x1), L" seconds");
	MelderInfo_writeLine (L"Number of coefficients: ", Melder_integer (maximumNumberOfCoefficients));
	MelderInfo_writeLine (L"Minimum frequency: ", Melder_double (fmin), L" Hz");
	MelderInfo_writeLine (L"Maximum frequency: ", Melder_double (fmax), L" Hz");
}

void CC_Frame_init (CC_Frame me, long numberOfCoefficients) {
	my c = NUMvector<double> (1, numberOfCoefficients);
	my numberOfCoefficients = numberOfCoefficients;
}

void CC_init (I, double tmin, double tmax, long nt, double dt, double t1,
              long maximumNumberOfCoefficients, double fmin, double fmax) {
	iam (CC);
	my fmin = fmin;
	my fmax = fmax;
	my maximumNumberOfCoefficients = maximumNumberOfCoefficients;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my frame = NUMvector<structCC_Frame> (1, nt);
}

Matrix CC_to_Matrix (I) {
	iam (CC);
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		                                 1, my maximumNumberOfCoefficients, my maximumNumberOfCoefficients, 1, 1);

		for (long i = 1; i <= my nx; i++) {
			CC_Frame cf = & my frame[i];
			for (long j = 1; j <= cf -> numberOfCoefficients; j++) {
				thy z[j][i] = cf -> c[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

void CC_paint (I, Graphics g, double xmin, double xmax, long cmin,
               long cmax, double minimum, double maximum, int garnish) {
	iam (CC);
	autoMatrix thee = CC_to_Matrix (me);

	Matrix_paintCells (thee.peek(), g, xmin, xmax, cmin, cmax, minimum, maximum);

	if (garnish) {
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Coefficients");
	}
}

void CC_drawC0 (I, Graphics g, double xmin, double xmax, double ymin,
                double ymax, int garnish) {
	iam (CC);
	(void) garnish;

	if (xmin >= xmax) {
		xmin = my xmin; xmax = my xmax;
	}

	long bframe, eframe;
	(void) Sampled_getWindowSamples (me, xmin, xmax, &bframe, &eframe);
	autoNUMvector<double> c (bframe, eframe);
	for (long i = bframe; i <= eframe; i++) {
		CC_Frame cf = & my frame[i];
		c[i] = cf -> c0;
	}
	if (ymin >= ymax) {
		NUMvector_extrema (c.peek(), bframe, eframe, &ymin, &ymax);
		if (ymax <= ymin) {
			ymin -= 1.0;
			ymax += 1.0;
		}
	} else {
		NUMvector_clip (c.peek(), bframe, eframe, ymin, ymax);
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, xmin, xmax, ymin, ymax);
	Graphics_function (g, c.peek(), bframe, eframe, xmin, xmax);
	Graphics_unsetInner (g);
}

void CC_getNumberOfCoefficients_extrema (I, long startframe, long endframe, long *min, long *max) {
	iam (CC);

	Melder_assert (startframe <= endframe);

	if (startframe == 0 && endframe == 0) {
		startframe = 1; endframe = my nx;
	}
	if (startframe < 1) {
		startframe = 1;
	}
	if (endframe > my nx) {
		endframe = my nx;
	}

	*min = my maximumNumberOfCoefficients;
	*max = 0;

	for (long i = startframe; i <= endframe; i++) {
		CC_Frame f = & my frame[i];
		long nc = f -> numberOfCoefficients;

		if (nc < *min) {
			*min = nc;
		} else if (nc > *max) {
			*max = nc;
		}
	}
}

long CC_getMinimumNumberOfCoefficients (I, long startframe, long endframe) {
	iam (CC);
	long min, max;

	CC_getNumberOfCoefficients_extrema (me, startframe, endframe, &min, &max);

	return min;
}

long CC_getMaximumNumberOfCoefficients (I, long startframe, long endframe) {
	iam (CC);
	long min, max;

	CC_getNumberOfCoefficients_extrema (me, startframe, endframe, &min, &max);

	return max;
}

double CC_getValue (I, double t, long index) {
	iam (CC);
	long iframe = Sampled_xToNearestIndex (me, t);
	if (iframe < 1 || iframe > my nx) {
		return NUMundefined;
	}
	CC_Frame cf = & me -> frame[iframe];
	return index > cf -> numberOfCoefficients ? NUMundefined : cf -> c[index];
}

/* End of file CC.cpp */
