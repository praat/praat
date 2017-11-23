/* LPC.cpp
 *
 * Copyright (C) 1994-2017 David Weenink
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
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain: ", xmin, U" to ", xmax, U" (s).");
	MelderInfo_writeLine (U"Prediction order: ", maxnCoefficients);
	MelderInfo_writeLine (U"Number of frames: ", nx);
	MelderInfo_writeLine (U"Time step: ", dx, U" (s).");
	MelderInfo_writeLine (U"First frame at: ", x1, U" (s).");
}

void LPC_Frame_init (LPC_Frame me, int nCoefficients) {
	if (nCoefficients != 0) {
		my a = NUMvector<double> (1, nCoefficients);
	}
	my nCoefficients = nCoefficients;
}

void LPC_init (LPC me, double tmin, double tmax, integer nt, double dt, double t1, int predictionOrder, double samplingPeriod) {
	my samplingPeriod = samplingPeriod;
	my maxnCoefficients = predictionOrder;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my d_frames = NUMvector<structLPC_Frame> (1, nt);
}

autoLPC LPC_create (double tmin, double tmax, integer nt, double dt, double t1, int predictionOrder, double samplingPeriod) {
	try {
		autoLPC me = Thing_new (LPC);
		LPC_init (me.get(), tmin, tmax, nt, dt, t1, predictionOrder, samplingPeriod);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LPC not created.");
	}
}

void LPC_drawGain (LPC me, Graphics g, double tmin, double tmax, double gmin, double gmax, bool garnish) {
	if (tmax <= tmin) {
		tmin = my xmin;
		tmax = my xmax;
	}
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) {
		return;
	}
	autoNUMvector<double> gain (itmin, itmax);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		gain [iframe] = my d_frames [iframe]. gain;
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
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		double x = Sampled_indexToX (me, iframe);
		Graphics_speckle (g, x, gain[iframe]);
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (seconds)");
		Graphics_textLeft (g, true, U"Gain");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void LPC_drawPoles (LPC me, Graphics g, double time, int garnish) {
	autoPolynomial p = LPC_to_Polynomial (me, time);
	autoRoots r = Polynomial_to_Roots (p.get());
	Roots_draw (r.get(), g, -1.0, 1.0, -1.0, 1.0, U"+", 12, garnish);
}

autoMatrix LPC_downto_Matrix_lpc (LPC me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, 0.5 + my maxnCoefficients, my maxnCoefficients, 1.0, 1.0);
		for (integer j = 1; j <= my nx; j ++) {
			LPC_Frame lpc = & my d_frames [j];
			for (integer i = 1; i <= lpc -> nCoefficients; i ++) {
				thy z [i] [j] = lpc -> a [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix with linear prediction coefficients created.");
	}
}

autoMatrix LPC_downto_Matrix_rc (LPC me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, 0.5 + my maxnCoefficients, my maxnCoefficients, 1.0, 1.0);
		autoNUMvector<double> rc (1, my maxnCoefficients);
		for (integer j = 1; j <= my nx; j ++) {
			LPC_Frame lpc = & my d_frames [j];
			NUMlpc_lpc_to_rc (lpc -> a, lpc -> nCoefficients, rc.peek());
			for (integer i = 1; i <= lpc -> nCoefficients; i ++) {
				thy z [i] [j] = rc [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix with relection coefficients created.");
	}
}

autoMatrix LPC_downto_Matrix_area (LPC me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, 0.5 + my maxnCoefficients, my maxnCoefficients, 1.0, 1.0);
		autoNUMvector<double> rc (1, my maxnCoefficients);
		autoNUMvector<double> area (1, my maxnCoefficients);
		for (integer j = 1; j <= my nx; j ++) {
			LPC_Frame lpc = & my d_frames [j];
			NUMlpc_lpc_to_rc (lpc -> a, lpc -> nCoefficients, rc.peek());
			NUMlpc_rc_to_area (rc.peek(), lpc -> nCoefficients, area.peek());
			for (integer i = 1; i <= lpc -> nCoefficients; i ++) {
				thy z [i] [j] = area [i];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix with areas created.");
	}
}

/* End of file LPC.cpp */
