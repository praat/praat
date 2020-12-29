/* Cepstrumc.c
 *
 * Copyright (C) 1994-2020 David Weenink
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
 djmw 20061218 Changed info to Melder_writeLine<x> format.
 djmw 20071017 oo_CAN_WRITE_AS_ENCODING.h
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrumc.h"
#include "DTW.h"

#include "oo_DESTROY.h"
#include "Cepstrumc_def.h"
#include "oo_COPY.h"
#include "Cepstrumc_def.h"
#include "oo_EQUAL.h"
#include "Cepstrumc_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Cepstrumc_def.h"
#include "oo_WRITE_TEXT.h"
#include "Cepstrumc_def.h"
#include "oo_WRITE_BINARY.h"
#include "Cepstrumc_def.h"
#include "oo_READ_TEXT.h"
#include "Cepstrumc_def.h"
#include "oo_READ_BINARY.h"
#include "Cepstrumc_def.h"
#include "oo_DESCRIPTION.h"
#include "Cepstrumc_def.h"

Thing_implement (Cepstrumc, Sampled, 1);

void structCepstrumc :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"  Start time: ", xmin);
	MelderInfo_writeLine (U"  End time: ", xmax);
	MelderInfo_writeLine (U"  Number of frames: ", nx);
	MelderInfo_writeLine (U"  Time step: ", dx);
	MelderInfo_writeLine (U"  First frame at: ", x1);
	MelderInfo_writeLine (U"  Number of coefficients: ", maxnCoefficients);
}

void Cepstrumc_Frame_init (Cepstrumc_Frame me, integer nCoefficients) {
	my c = zero_VEC (nCoefficients);
	my nCoefficients = my c.size; // maintain invariant
}

void Cepstrumc_init (Cepstrumc me, double tmin, double tmax, integer nt, double dt, double t1, integer nCoefficients, double samplingFrequency) {
	my samplingFrequency = samplingFrequency;
	my maxnCoefficients = nCoefficients;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my frame = newvectorzero <structCepstrumc_Frame> (nt);
}

autoCepstrumc Cepstrumc_create (double tmin, double tmax, integer nt, double dt, double t1, integer nCoefficients, double samplingFrequency) {
	try {
		autoCepstrumc me = Thing_new (Cepstrumc);
		Cepstrumc_init (me.get(), tmin, tmax, nt, dt, t1, nCoefficients, samplingFrequency);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrum not created.");
	}
}

static void regression (VEC const& r, Cepstrumc me, integer frameNumber, integer numberOfRegressionFrames) {
	Melder_assert (r.size == my maxnCoefficients + 1);
	r  <<=  0.0;

	if (frameNumber <= numberOfRegressionFrames / 2 || frameNumber > my nx - numberOfRegressionFrames / 2)
		return;
	integer nc = INTEGER_MAX;
	longdouble sumsq = 0.0;
	for (integer j = -numberOfRegressionFrames / 2; j <= numberOfRegressionFrames / 2; j ++) {
		const Cepstrumc_Frame f = & my frame [frameNumber + j];
		if (f -> nCoefficients < nc)
			nc = f -> nCoefficients;
		sumsq += j * j;
	}
	for (integer i = 0; i <= nc; i ++) {
		for (integer j = -numberOfRegressionFrames / 2; j <= numberOfRegressionFrames / 2; j ++) {
			const Cepstrumc_Frame f = & my frame [frameNumber + j];
			r [i + 1] += f -> c [i] * j / (double (sumsq) * my dx);
		}
	}
}

autoDTW Cepstrumc_to_DTW (Cepstrumc me, Cepstrumc thee, double wc, double wle, double wr, double wer, double dtr, int matchStart, int matchEnd, int constraint) {
	try {
		integer numberOfRegressionFrames = Melder_ifloor (dtr / my dx);
		Melder_require (my maxnCoefficients == thy maxnCoefficients,
			U"Cepstrumc orders should be equal.");
		Melder_require (! (wr != 0.0 && numberOfRegressionFrames < 2),
			U"Time window for regression coefficients too small.");
		if (numberOfRegressionFrames % 2 == 0)
			numberOfRegressionFrames += 1;
		if (wr != 0.0)
			Melder_casual (U"Number of frames used for regression coefficients ", numberOfRegressionFrames);
		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoVEC ri = raw_VEC (my maxnCoefficients + 1);
		autoVEC rj = raw_VEC (my maxnCoefficients + 1);
		/*
			Calculate distance matrix.
		*/
		autoMelderProgress progress (U"");
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Cepstrumc_Frame fi = & my frame [iframe];
			regression (ri.get(), me, iframe, numberOfRegressionFrames);
			for (integer jframe = 1; jframe <= thy nx; jframe ++) {
				const Cepstrumc_Frame fj = & thy frame [jframe];
				longdouble dist = 0.0, distr = 0.0;
				/*
					Cepstral distance.
				*/
				if (wc != 0.0) {
					for (integer k = 1; k <= fj -> nCoefficients; k ++) {
						const double dci = fi -> c [k] - fj -> c [k];
						dist += dci * dci;
					}
					dist *= wc;
				}
				/*
					Log energy distance.
				*/
				const double dc0 = fi -> c [0] - fj -> c [0];
				dist += wle * dc0 * dc0;
				/*
					Regression distance.
				*/
				if (wr != 0.0) {
					regression (rj.get(), thee, jframe, numberOfRegressionFrames);
					for (integer k = 1; k <= fj -> nCoefficients; k ++) {
						const double drci = ri [k + 1] - rj [k + 1];
						distr += drci * drci;
					}
					dist += wr * distr;
				}
				/*
					Regression on c [0]: log(energy)
				*/
				if (wer != 0.0) {
					if (wr == 0.0)
						regression (rj.get(), thee, jframe, numberOfRegressionFrames);
					const double drc0 = ri [1] - rj [1];
					dist += wer * drc0 * drc0;
				}
				dist /= wc + wle + wr + wer;
				his z [iframe] [jframe] = sqrt ((double) dist);   // prototype along y-direction
			}
			Melder_progress ( (double) iframe / my nx, U"Calculate distances: frame ", iframe, U" from ", my nx, U".");
		}
		DTW_findPath (him.get(), matchStart, matchEnd, constraint);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created.");
	}
}

autoMatrix Cepstrumc_to_Matrix (Cepstrumc me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0, my maxnCoefficients, my maxnCoefficients + 1, 1, 0);

		for (integer i = 1; i <= my nx; i ++) {
			const Cepstrumc_Frame him = & my frame [i];
			for (integer j = 1; j <= his nCoefficients + 1; j ++)
				thy z [j] [i] = his c [j - 1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

/* End of file Cepstrumc.cpp */
