/* LPC.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20020812 GPL header
 djmw 20030612 Removed LPC_Frame_free
 djmw 20060510 LPC_drawPoles error cleared if something goes wrong.
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20071017 oo_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 float -> double
 djmw 20081223 Corrected a bug in Matrix LPC_to_Matrix (last coefficient was not copied)
 djmw 20110304 Thing_new
*/

#include "LPC_and_Polynomial.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "LPC_def.h"
#include "oo_COPY.h"
#include "LPC_def.h"
#include "oo_EQUAL.h"
#include "LPC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPC_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPC_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPC_def.h"
#include "oo_READ_TEXT.h"
#include "LPC_def.h"
#include "oo_READ_BINARY.h"
#include "LPC_def.h"
#include "oo_DESCRIPTION.h"
#include "LPC_def.h"

Thing_implement (LPC, Sampled, 1);

void structLPC :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine5 (L"Time domain: ", Melder_double (xmin), L" to ", Melder_double (xmax),
	                       L" (s).");
	MelderInfo_writeLine2 (L"Prediction order: ", Melder_integer (maxnCoefficients));
	MelderInfo_writeLine2 (L"Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine3 (L"Time step: ", Melder_double (dx), L" (s).");
	MelderInfo_writeLine3 (L"First frame at: ", Melder_double (x1), L" (s).");
}

void LPC_Frame_init (LPC_Frame me, int nCoefficients) {
	if (nCoefficients != 0) {
		my a = NUMvector<double> (1, nCoefficients);
	}
	my nCoefficients = nCoefficients;
}

void LPC_init (LPC me, double tmin, double tmax, long nt, double dt, double t1,
               int predictionOrder, double samplingPeriod) {
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = predictionOrder;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my d_frames = NUMvector<structLPC_Frame> (1, nt);
}

LPC LPC_create (double tmin, double tmax, long nt, double dt, double t1,
                int predictionOrder, double samplingPeriod) {
	try {
		autoLPC me = Thing_new (LPC);
		LPC_init (me.peek(), tmin, tmax, nt, dt, t1, predictionOrder, samplingPeriod);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("LPC not created.");
	}
}

void LPC_drawGain (LPC me, Graphics g, double tmin, double tmax, double gmin, double gmax, int garnish) {
	if (tmax <= tmin) {
		tmin = my xmin;
		tmax = my xmax;
	}
	long itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) {
		return;
	}
	autoNUMvector<double> gain (itmin, itmax);

	for (long iframe = itmin; iframe <= itmax; iframe++) {
		gain[iframe] = my d_frames[iframe].gain;
	}
	if (gmax <= gmin) {
		NUMvector_extrema (gain.peek(), itmin, itmax, & gmin, & gmax);
	}
	if (gmax == gmin) {
		gmin = 0;
		gmax += 0.5;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, gmin, gmax);
	for (long iframe = itmin; iframe <= itmax; iframe++) {
		double x = Sampled_indexToX (me, iframe);
		Graphics_fillCircle_mm (g, x, gain[iframe], 1.0);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (seconds)");
		Graphics_textLeft (g, 1, L"Gain");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
	}
}

void LPC_drawPoles (LPC me, Graphics g, double time, int garnish) {
	autoPolynomial p = LPC_to_Polynomial (me, time);
	autoRoots r = Polynomial_to_Roots (p.peek());
	Roots_draw (r.peek(), g, -1, 1, -1, 1, L"+", 12, garnish);
}


Matrix LPC_to_Matrix (LPC me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		                                 1, my maxnCoefficients, my maxnCoefficients, 1, 1);

		for (long i = 1; i <= my nx; i++) {
			LPC_Frame frame = & my d_frames[i];
			for (long j = 1; j <= frame -> nCoefficients; j++) {
				thy z[j][i] = frame -> a[j];
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix created.");
	}
}

/* End of file LPC.cpp */
