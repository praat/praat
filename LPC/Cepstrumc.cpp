/* Cepstrumc.c
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

void Cepstrumc_Frame_init (Cepstrumc_Frame me, int nCoefficients) {
	my c = NUMvector<double> (0, nCoefficients);
	my nCoefficients = nCoefficients;
}

void Cepstrumc_init (Cepstrumc me, double tmin, double tmax, integer nt, double dt, double t1,
                     int nCoefficients, double samplingFrequency) {
	my samplingFrequency = samplingFrequency;
	my maxnCoefficients = nCoefficients;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my frame = NUMvector<structCepstrumc_Frame> (1, nt);
}

autoCepstrumc Cepstrumc_create (double tmin, double tmax, integer nt, double dt, double t1,
                            int nCoefficients, double samplingFrequency) {
	try {
		autoCepstrumc me = Thing_new (Cepstrumc);
		Cepstrumc_init (me.get(), tmin, tmax, nt, dt, t1, nCoefficients, samplingFrequency);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Cepstrum not created.");
	}
}

static void regression (Cepstrumc me, integer frame, double r [], integer nr) {
	integer nc = 1e6; double sumsq = 0;
	for (integer i = 0; i <= my maxnCoefficients; i ++) {
		r [i] = 0;
	}
	if (frame <= nr / 2 || frame >= my nx - nr / 2) {
		return;
	}
	for (integer j = -nr / 2; j <= nr / 2; j ++) {
		Cepstrumc_Frame f = & my frame [frame + j];
		if (f->nCoefficients < nc) {
			nc = f->nCoefficients;
		}
		sumsq += j * j;
	}
	for (integer i = 0; i <= nc; i ++) {
		for (integer j = -nr / 2; j <= nr / 2; j ++) {
			Cepstrumc_Frame f = & my frame [frame + j];
			r [i] += f->c [i] * j / sumsq / my dx;
		}
	}
}

autoDTW Cepstrumc_to_DTW (Cepstrumc me, Cepstrumc thee, double wc, double wle, double wr, double wer, double dtr, int matchStart, int matchEnd, int constraint) {
	try {
		integer nr = Melder_ifloor (dtr / my dx);

		if (my maxnCoefficients != thy maxnCoefficients) {
			Melder_throw (U"Cepstrumc orders should be equal.");
		}
		if (wr != 0 && nr < 2) {
			Melder_throw (U"Time window for regression coefficients too small.");
		}
		if (nr % 2 == 0) {
			nr ++;
		}
		if (wr != 0) {
			Melder_casual (U"Number of frames used for regression coefficients ", nr);
		}
		autoDTW him = DTW_create (my xmin, my xmax, my nx, my dx, my x1, thy xmin, thy xmax, thy nx, thy dx, thy x1);
		autoNUMvector <double> ri ((integer) 0, my maxnCoefficients);
		autoNUMvector <double> rj ((integer) 0, my maxnCoefficients);

		// Calculate distance matrix

		autoMelderProgress progress (U"");
		for (integer i = 1; i <= my nx; i ++) {
			Cepstrumc_Frame fi = & my frame [i];
			regression (me, i, ri.peek(), nr);
			for (integer j = 1; j <= thy nx; j ++) {
				Cepstrumc_Frame fj = & thy frame [j];
				real80 d, dist = 0, distr = 0;
				if (wc != 0) { /* cepstral distance */
					for (integer k = 1; k <= fj -> nCoefficients; k ++) {
						d = fi -> c [k] - fj -> c [k];
						dist += d * d;
					}
					dist *= wc;
				}
				// log energy distance
				d = fi -> c [0] - fj -> c [0];
				dist += wle * d * d;
				if (wr != 0) {   // regression distance
					regression (thee, j, rj.peek(), nr);
					for (integer k = 1; k <= fj -> nCoefficients; k ++) {
						d = ri [k] - rj [k];
						distr += d * d;
					}
					dist += wr * distr;
				}
				if (wer != 0) {   // regression on c [0]: log(energy)
					if (wr == 0) {
						regression (thee, j, rj.peek(), nr);
					}
					d = ri [0] - rj [0];
					dist += wer * d * d;
				}
				dist /= wc + wle + wr + wer;
				his z [i] [j] = sqrt ((real) dist);   // prototype along y-direction
			}
			Melder_progress ( (double) i / my nx, U"Calculate distances: frame ",
			                   i, U" from ", my nx, U".");
		}
		DTW_findPath (him.get(), matchStart, matchEnd, constraint);
		return him;
	} catch (MelderError) {
		Melder_throw (U"DTW not created.");
	}
}

autoMatrix Cepstrumc_to_Matrix (Cepstrumc me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		                                 0, my maxnCoefficients, my maxnCoefficients + 1, 1, 0);

		for (integer i = 1; i <= my nx; i ++) {
			Cepstrumc_Frame him = & my frame [i];
			for (integer j = 1; j <= his nCoefficients + 1; j ++) {
				thy z [j] [i] = his c [j - 1];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix created.");
	}
}

/* End of file Cepstrumc.cpp */
