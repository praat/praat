/* Sound_and_LPC_robust.cpp
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
 djmw 20030814 First version
 djmw 20061218 To Melder_information<x> format
 djmw 20070103 Sound interface changes
 djmw 20080122 float -> double
 djmw 20101008 New LPC_Frame_filterInverse interface.
 djmw 20110302 Corrected a number of pointer initialisations
 djmw 20111027 +Sound_to_Formant_robust
*/

#include "LPC_and_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound_and_LPC_robust.h"
#include "Sound_extensions.h"
#include "SVD.h"
#include "Vector.h"
#include "NUM2.h"

struct huber_struct {
	autoSound e;
	double k, tol, tol_svd;
	integer iter, itermax;
	int wantlocation, wantscale;
	double location, scale;
	integer n, p;
	double *w, *work;
	double *a;
	double **covar, *c;
	autoSVD svd;
};

static void huber_struct_init (struct huber_struct *hs, double windowDuration, integer p, double samplingFrequency, double location, int wantlocation) {
	hs -> w = hs -> work = hs -> a = hs -> c = nullptr;
	hs -> covar = nullptr;
	hs -> svd = autoSVD();
	hs -> e = Sound_createSimple (1, windowDuration, samplingFrequency);
	integer n = hs -> e -> nx;
	hs -> n = n;
	hs -> p = p;
	hs -> w = NUMvector<double> (1, n);
	hs -> work = NUMvector<double> (1, n);
	hs -> a = NUMvector<double> (1, p);
	hs -> covar = NUMmatrix<double> (1, p, 1, p);
	hs -> c = NUMvector<double> (1, p);
	hs -> svd = SVD_create (p, p);
	hs -> wantlocation = wantlocation;
	if (! wantlocation) {
		hs -> location = location;
	}
	hs -> wantscale = 1;
}

static void huber_struct_destroy (struct huber_struct *hs) {
	hs -> e.reset();
	hs -> svd.reset();
	NUMvector_free<double> (hs -> w, 1);
	NUMvector_free<double> (hs -> work, 1);
	NUMvector_free<double> (hs -> a, 1);
	NUMmatrix_free<double> (hs -> covar, 1, 1);
	NUMvector_free<double> (hs -> c, 1);
}

static void huber_struct_getWeights (struct huber_struct *hs, double *e) {
	double ks = hs -> k * hs -> scale;
	double *w = hs -> w;

	for (integer i = 1 ; i <= hs -> n; i ++) {
		double ei = e [i] - hs -> location;
		w [i] = ei > -ks && ei < ks ? 1.0 : ks / fabs (ei);
	}
}

static void huber_struct_getWeightedCovars (struct huber_struct *hs, double *s) {
	integer p = hs -> p, n = hs -> n;
	double *w = hs -> w, **covar = hs -> covar, *c = hs -> c;

	for (integer i = 1; i <= p; i ++) {
		for (integer j = i; j <= p; j ++) {
			double tmp = 0;
			for (integer k = p + 1; k <= n; k ++) {
				tmp += s [k - j] * s [k - i] * w [k];
			}
			covar [i] [j] = covar [j] [i] = tmp;
		}

		double tmp = 0;
		for (integer k = p + 1; k <= n; k ++) {
			tmp += s [k - i] * s [k] * w [k];
		}
		c [i] = -tmp;
	}
}

static void huber_struct_solvelpc (struct huber_struct *hs) {
	SVD me = hs -> svd.get();
	double **covar = hs -> covar;

	for (integer i = 1; i <= my numberOfRows; i ++) {
		for (integer j = 1; j <= my numberOfColumns; j ++) {
			my u [i] [j] = covar [i] [j];
		}
	}

	SVD_setTolerance (me, hs -> tol_svd);
	SVD_compute (me);

	//integer nzeros = SVD_zeroSmallSingularValues (me, 0);

	SVD_solve (me, hs -> c, hs -> a);
}

void LPC_Frames_Sound_huber (LPC_Frame me, Sound thee, LPC_Frame him, struct huber_struct *hs) {
	integer p = my nCoefficients > his nCoefficients ? his nCoefficients : my nCoefficients;
	integer n = hs -> e -> nx > thy nx ? thy nx : hs -> e -> nx;
	double *e = hs -> e -> z [1], *s = thy z [1];

	hs -> iter = 0;
	hs -> scale = 1e308;
	hs -> p = p;

	double s0;
	do {
		Sound hse = hs -> e.get();
		for (integer i = 1; i <= thy nx; i ++) {
			hse -> z [1] [i] = thy z [1] [i];
		}
		LPC_Frame_Sound_filterInverse (him, hse, 1);

		s0 = hs -> scale;

		NUMstatistics_huber (e, n, & (hs -> location), hs -> wantlocation, & (hs -> scale), hs -> wantscale, hs -> k, hs -> tol, hs -> work);

		huber_struct_getWeights (hs, e);
		huber_struct_getWeightedCovars (hs, s);

		// Solve C a = [-] c */
		try {
			huber_struct_solvelpc (hs);
		} catch (MelderError) {
			// Copy the starting lpc coeffs */
			for (integer i = 1; i <= p; i ++) {
				his a [i] = my a [i];
			}
			throw MelderError();
		}
		for (integer i = 1; i <= p; i ++) {
			his a [i] = hs -> a [i];
		}

		(hs -> iter) ++;
	} while ( (hs -> iter < hs -> itermax) && (fabs (s0 - hs -> scale) > hs -> tol * s0));
}

#if 0
	autoSound e;
	double k, tol, tol_svd;
	integer iter, itermax;
	bool wantlocation, wantscale;
	double location, scale;
	integer n, p;
	double *w, *work;
	double *a;
	double **covar, *c;
	autoSVD svd;
#endif
	
autoLPC LPC_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth, double preEmphasisFrequency, double k,
	int itermax, double tol, bool wantlocation) {
	struct huber_struct struct_huber { autoSound(), 0.0, 0.0, 0.0, 0, 0, 0, 0, 0.0, 0.0, 0, 0, nullptr, nullptr, nullptr, nullptr, nullptr, autoSVD() };
	try {
		double t1, samplingFrequency = 1.0 / my dx, tol_svd = 0.000001;
		double location = 0, windowDuration = 2 * analysisWidth; /* Gaussian window */
		integer numberOfFrames, frameErrorCount = 0, iter = 0;
		integer p = thy maxnCoefficients;
		Melder_require (my xmin == thy xmin && my xmax == thy xmax, U"Time domains should be equal.");
		Melder_require (my dx == thy samplingPeriod, U"Sampling intervals should be equal.");
		Melder_require (Melder_roundDown (windowDuration / my dx) > p, U"Analysis window too short.");
		
		Sampled_shortTermAnalysis (me, windowDuration, thy dx, & numberOfFrames, & t1);
		Melder_require (numberOfFrames == thy nx && t1 == thy x1, U"Incorrect retrieved analysis width.");

		autoSound sound = Data_copy (me);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoLPC him = Data_copy (thee);
		huber_struct_init (&struct_huber, windowDuration, p, samplingFrequency, location, wantlocation);

		struct_huber.k = k;
		struct_huber.tol = tol;
		struct_huber.tol_svd = tol_svd;
		struct_huber.itermax = itermax;

		autoMelderProgress progess (U"LPC analysis");

		Sound_preEmphasis (sound.get(), preEmphasisFrequency);

		for (integer i = 1; i <= numberOfFrames; i ++) {
			LPC_Frame lpc = (LPC_Frame) & thy d_frames [i];
			LPC_Frame lpcto = (LPC_Frame) & his d_frames [i];
			double t = Sampled_indexToX (thee, i);

			Sound_into_Sound (sound.get(), sframe.get(), t - windowDuration / 2);
			Vector_subtractMean (sframe.get());
			Sounds_multiply (sframe.get(), window.get());

			try {
				LPC_Frames_Sound_huber (lpc, sframe.get(), lpcto, & struct_huber);
			} catch (MelderError) {
				frameErrorCount ++;
			}

			iter += struct_huber.iter;

			if ( (i % 10) == 1) {
				Melder_progress ((double) i / numberOfFrames, U"LPC analysis of frame ", i, U" out of ", numberOfFrames, U".");
			}
		}

		if (frameErrorCount) Melder_warning (U"Results of ", frameErrorCount,
			U" frame(s) out of ", numberOfFrames, U" could not be optimised.");
		MelderInfo_writeLine (U"Number of iterations: ", iter,
			U"\n   Average per frame: ", (double) iter / numberOfFrames);
		huber_struct_destroy (&struct_huber);
		return him;
	} catch (MelderError) {
		huber_struct_destroy (&struct_huber);
		Melder_throw (me, U": no robust LPC created.");
	}
}

autoFormant Sound_to_Formant_robust (Sound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double halfdt_window, double preEmphasisFrequency, double safetyMargin, double k, int itermax, double tol, bool wantlocation)
{
	double dt = dt_in > 0.0 ? dt_in : halfdt_window / 4.0;
	double nyquist = 0.5 / my dx;
	integer predictionOrder = Melder_ifloor (2 * numberOfFormants);
	try {
		autoSound sound;
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1.0) < 1.0e-12) {
			sound = Data_copy (me);   // will be modified
		} else {
			sound = Sound_resample (me, maximumFrequency * 2.0, 50);
		}

		autoLPC lpc = Sound_to_LPC_auto (sound.get(), predictionOrder, halfdt_window, dt, preEmphasisFrequency);
		autoLPC lpcr = LPC_Sound_to_LPC_robust (lpc.get(), sound.get(), halfdt_window, preEmphasisFrequency, k, itermax, tol, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcr.get(), safetyMargin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}

/* End of file Sound_and_LPC_robust.cpp */
