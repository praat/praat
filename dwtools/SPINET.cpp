/* SPINET.cpp
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
 djmw 20020813 GPL header
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
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

Thing_implement (SPINET, Sampled2, 0);

void structSPINET :: v_info () {
	structData :: v_info ();
	double miny, maxy, mins, maxs;
	if (! Sampled2_getWindowExtrema_d (this, y, 1, nx, 1, ny, & miny, & maxy) ||
	        ! Sampled2_getWindowExtrema_d (this, s, 1, nx, 1, ny, & mins, & maxs)) {
		return;
	}
	MelderInfo_writeLine (L"Minimum power: ", Melder_double (miny));
	MelderInfo_writeLine (L"Maximum power: ", Melder_double (maxy));
	MelderInfo_writeLine (L"Minimum power rectified: ", Melder_double (mins));
	MelderInfo_writeLine (L"Maximum powerrectified: ", Melder_double (maxs));
}

SPINET SPINET_create (double tmin, double tmax, long nt, double dt, double t1,
                      double minimumFrequency, double maximumFrequency, long nFilters,
                      double excitationErbProportion, double inhibitionErbProportion) {
	try {
		autoSPINET me = Thing_new (SPINET);
		double minErb = NUMhertzToErb (minimumFrequency);
		double maxErb = NUMhertzToErb (maximumFrequency);
		double dErb = (maxErb - minErb) / nFilters;
		Sampled2_init (me.peek(), tmin, tmax, nt, dt, t1,
		               minErb - dErb / 2, maxErb + dErb / 2, nFilters, dErb, minErb);
		my y = NUMmatrix<double> (1, nFilters, 1, nt);
		my s = NUMmatrix<double> (1, nFilters, 1, nt);
		my gamma = 4;
		my excitationErbProportion = excitationErbProportion;
		my inhibitionErbProportion = inhibitionErbProportion;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("SPINET not created.");
	}
}

void SPINET_spectralRepresentation (SPINET me, Graphics g, double fromTime, double toTime,
                                    double fromErb, double toErb, double minimum, double maximum, int enhanced,
                                    int garnish) {
	double **z = enhanced ? my s : my y;
	autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
	                                 my ymin, my ymax, my ny, my dy, my y1);
	for (long j = 1; j <= my ny; j++) {
		for (long i = 1; i <= my nx; i++) {
			thy z[j][i] = z[j][i];
		}
	}
	Matrix_paintCells (thee.peek(), g, fromTime, toTime, fromErb, toErb, minimum, maximum);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Frequency (ERB)");
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textTop (g, 0, enhanced ? L"Cooperative interaction output" :
		                  L"Gammatone filterbank output");
	}
}

void SPINET_drawSpectrum (SPINET me, Graphics g, double time, double fromErb, double toErb,
                          double minimum, double maximum, int enhanced, int garnish) {
	long ifmin, ifmax, icol = Sampled2_xToColumn (me, time);
	double **z = enhanced ? my s : my y;
	if (icol < 1 || icol > my nx) {
		return;
	}
	if (toErb <= fromErb) {
		fromErb = my ymin;
		toErb = my ymax;
	}
	Sampled2_getWindowSamplesY (me, fromErb, toErb, &ifmin, &ifmax);
	autoNUMvector<double> spec (1, my ny);

	for (long i = 1; i <= my ny; i++) {
		spec[i] = z[i][icol];
	}
	if (maximum <= minimum) {
		NUMvector_extrema (spec.peek(), ifmin, ifmax, &minimum, &maximum);
	}
	if (maximum <= minimum) {
		minimum -= 1;
		maximum += 1;
	}
	for (long i = ifmin; i <= ifmax; i++) {
		if (spec[i] > maximum) {
			spec[i] = maximum;
		} else if (spec[i] < minimum) {
			spec[i] = minimum;
		}
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, fromErb, toErb, minimum, maximum);
	Graphics_function (g, spec.peek(), ifmin, ifmax, Sampled2_rowToY (me, ifmin), Sampled2_rowToY (me, ifmax));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Frequency (ERB)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"strength");
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

/* End of file SPINET.cpp */
