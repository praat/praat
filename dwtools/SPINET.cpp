/* SPINET.cpp
 *
 * Copyright (C) 1993-2017 David Weenink, 2015 Paul Boersma
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
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: oo_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 float -> double
  djmw 20110304 Thing_new
*/

#include "SPINET.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SPINET_def.h"
#include "oo_COPY.h"
#include "SPINET_def.h"
#include "oo_EQUAL.h"
#include "SPINET_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SPINET_def.h"
#include "oo_WRITE_TEXT.h"
#include "SPINET_def.h"
#include "oo_WRITE_BINARY.h"
#include "SPINET_def.h"
#include "oo_READ_TEXT.h"
#include "SPINET_def.h"
#include "oo_READ_BINARY.h"
#include "SPINET_def.h"
#include "oo_DESCRIPTION.h"
#include "SPINET_def.h"

Thing_implement (SPINET, SampledXY, 0);

static integer SampledXY_getWindowExtrema (SampledXY me, double **z, integer ixmin, integer ixmax, integer iymin, integer iymax, double *minimum, double *maximum) {
/*
	Function:
		compute the minimum and maximum values of z over all samples inside [ixmin, ixmax] * [iymin, iymax].
	Arguments:
		if ixmin = 0, start at first column; if ixmax = 0, end at last column (same for iymin and iymax).
	Return value:
		the number of samples inside the window.
	Postconditions:
		if result == 0, *minimum and *maximum are not changed;
*/

	if (ixmin == 0) {
		ixmin = 1;
	}
	if (ixmax == 0) {
		ixmax = my nx;
	}
	if (iymin == 0) {
		iymin = 1;
	}
	if (iymax == 0) {
		iymax = my ny;
	}
	if (ixmin > ixmax || iymin > iymax) {
		return 0;
	}
	*minimum = *maximum = z [iymin] [ixmin];
	for (integer iy = iymin; iy <= iymax; iy ++)
		for (integer ix = ixmin; ix <= ixmax; ix ++) {
			if (z [iy] [ix] < *minimum) {
				*minimum = z [iy] [ix];
			}
			if (z [iy] [ix] > *maximum) {
				*maximum = z [iy] [ix];
			}
		}
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

void structSPINET :: v_info () {
	structDaata :: v_info ();
	double miny, maxy, mins, maxs;
	if (! SampledXY_getWindowExtrema (this, y, 1, nx, 1, ny, & miny, & maxy) ||
	        ! SampledXY_getWindowExtrema (this, s, 1, nx, 1, ny, & mins, & maxs)) {
		return;
	}
	MelderInfo_writeLine (U"Minimum power: ", miny);
	MelderInfo_writeLine (U"Maximum power: ", maxy);
	MelderInfo_writeLine (U"Minimum power rectified: ", mins);
	MelderInfo_writeLine (U"Maximum powerrectified: ", maxs);
}

autoSPINET SPINET_create (double tmin, double tmax, integer nt, double dt, double t1, double minimumFrequency, double maximumFrequency, integer nFilters, double excitationErbProportion, double inhibitionErbProportion) {
	try {
		autoSPINET me = Thing_new (SPINET);
		double minErb = NUMhertzToErb (minimumFrequency);
		double maxErb = NUMhertzToErb (maximumFrequency);
		double dErb = (maxErb - minErb) / nFilters;
		SampledXY_init (me.get(), tmin, tmax, nt, dt, t1, minErb - dErb / 2.0, maxErb + dErb / 2.0, nFilters, dErb, minErb);
		my y = NUMmatrix<double> (1, nFilters, 1, nt);
		my s = NUMmatrix<double> (1, nFilters, 1, nt);
		my gamma = 4;
		my excitationErbProportion = excitationErbProportion;
		my inhibitionErbProportion = inhibitionErbProportion;
		return me;
	} catch (MelderError) {
		Melder_throw (U"SPINET not created.");
	}
}

void SPINET_spectralRepresentation (SPINET me, Graphics g, double fromTime, double toTime, double fromErb, double toErb, double minimum, double maximum, int enhanced, int garnish) {
	double **z = enhanced ? my s : my y;
	autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
	for (integer j = 1; j <= my ny; j ++) {
		for (integer i = 1; i <= my nx; i ++) {
			thy z [j] [i] = z [j] [i];
		}
	}
	Matrix_paintCells (thee.get(), g, fromTime, toTime, fromErb, toErb, minimum, maximum);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (ERB)");
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textTop (g, false, enhanced ? U"Cooperative interaction output" : U"Gammatone filterbank output");
	}
}

void SPINET_drawSpectrum (SPINET me, Graphics g, double time, double fromErb, double toErb, double minimum, double maximum, int enhanced, int garnish) {
	integer ifmin, ifmax, icol = Sampled_xToLowIndex (me, time);   // ppgb: don't use Sampled2_xToColumn for integer rounding
	double **z = enhanced ? my s : my y;
	if (icol < 1 || icol > my nx) {
		return;
	}
	if (toErb <= fromErb) {
		fromErb = my ymin;
		toErb = my ymax;
	}
	SampledXY_getWindowSamplesY (me, fromErb, toErb, &ifmin, &ifmax);
	autoNUMvector<double> spec (1, my ny);

	for (integer i = 1; i <= my ny; i ++) {
		spec [i] = z [i] [icol];
	}
	if (maximum <= minimum) {
		NUMvector_extrema (spec.peek(), ifmin, ifmax, &minimum, &maximum);
	}
	if (maximum <= minimum) {
		minimum -= 1;
		maximum += 1;
	}
	for (integer i = ifmin; i <= ifmax; i ++) {
		if (spec [i] > maximum) {
			spec [i] = maximum;
		} else if (spec [i] < minimum) {
			spec [i] = minimum;
		}
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, fromErb, toErb, minimum, maximum);
	Graphics_function (g, spec.peek(), ifmin, ifmax, SampledXY_indexToY (me, ifmin), SampledXY_indexToY (me, ifmax));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Frequency (ERB)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"strength");
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

/* End of file SPINET.cpp */
