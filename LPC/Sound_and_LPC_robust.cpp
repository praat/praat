/* Sound_and_LPC_robust.cpp
 *
 * Copyright (C) 1994-2018 David Weenink
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
	double k_stdev, tol, tol_svd;
	integer iter, itermax;
	int wantlocation, wantscale;
	double location, scale;
	integer n, p;
	autoVEC w, work, a, c;
	autoMAT covar;
	autoSVD svd;
};

static void huber_struct_init (struct huber_struct *hs, double windowDuration, integer p, double samplingFrequency, double location, int wantlocation) {
	hs -> e = Sound_createSimple (1, windowDuration, samplingFrequency);
	hs -> k_stdev = hs -> tol = hs -> tol_svd = hs -> scale = 0.0;
	hs -> iter = 1;
	hs -> itermax = 1;
	hs -> wantlocation = wantlocation;
	if (! wantlocation) hs -> location = location;
	hs -> wantscale = 1;
	integer n = hs -> e -> nx;
	hs -> n = n;
	hs -> p = p;
	hs -> w = newVECzero (n);
	hs -> work = newVECraw (n);
	hs -> a = newVECraw (p);
	hs -> c = newVECzero (p);
	hs -> covar = newMATzero (p, p);
	hs -> svd = SVD_create (p, p);
}

static void huber_struct_getWeights (struct huber_struct *hs, constVEC e) {
	Melder_assert (e.size == hs -> n);
	double kstdev = hs -> k_stdev * hs -> scale;

	for (integer i = 1 ; i <= hs -> n; i ++) {
		double ei = e [i] - hs -> location;
		hs -> w [i] = ei > -kstdev && ei < kstdev ? 1.0 : kstdev / fabs (ei);
	}
}

static void huber_struct_getWeightedCovars (struct huber_struct *hs, VEC s) {
	Melder_assert (s.size == hs -> n);
	integer p = hs -> p, n = hs -> n;

	for (integer i = 1; i <= p; i ++) {
		for (integer j = i; j <= p; j ++) {
			longdouble tmp = 0.0;
			for (integer k = p + 1; k <= s.size; k ++)
				tmp += s [k - j] * s [k - i] *  hs -> w [k];
			hs -> covar [i] [j] = hs -> covar [j] [i] = (double) tmp;
		}

		longdouble tmp = 0.0;
		for (integer k = p + 1; k <= n; k ++) {
			tmp += s [k - i] * s [k] *  hs -> w [k];
		}
		hs -> c [i] = - tmp;
	}
}

static void huber_struct_solvelpc (struct huber_struct *hs) {
	SVD me = hs -> svd.get();
	my u.get() <<= hs -> covar.get();
	SVD_setTolerance (me, hs -> tol_svd);
	SVD_compute (me);

	autoVEC x = SVD_solve (me, hs -> c.get());
	hs -> a.all() <<= x.all();
}

void LPC_Frames_Sound_huber (LPC_Frame me, Sound thee, LPC_Frame him, struct huber_struct *hs) {
	integer p = my nCoefficients > his nCoefficients ? his nCoefficients : my nCoefficients;
	integer n = hs -> e -> nx > thy nx ? thy nx : hs -> e -> nx;

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
		VEC work = hs -> work.get();
		NUMstatistics_huber (hs -> e -> z.row (1), & hs -> location, hs -> wantlocation, & hs -> scale, hs -> wantscale, hs -> k_stdev, hs -> tol, & work);

		huber_struct_getWeights (hs, hs -> e -> z.row (1));
		huber_struct_getWeightedCovars (hs, thy z.row (1));

		// Solve C a = [-] c */
		try {
			huber_struct_solvelpc (hs);
		} catch (MelderError) {
			// Copy the starting lpc coeffs */
			his a.part (1, p) <<= my a.part (1, p); 
			throw MelderError();
		}
		his a.part (1, p) <<= hs -> a.part (1, p);
		hs -> iter ++;
	} while (hs -> iter < hs -> itermax && fabs (s0 - hs -> scale) > hs -> tol * s0);
}

autoLPC LPC_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth, double preEmphasisFrequency, double k_stdev,
	int itermax, double tol, bool wantlocation) {
	struct huber_struct struct_huber;
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

		struct_huber.k_stdev = k_stdev;
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

			if (i % 10 == 1)
				Melder_progress ((double) i / numberOfFrames,
					U"LPC analysis of frame ", i, U" out of ", numberOfFrames, U".");
		}

		if (frameErrorCount) Melder_warning (U"Results of ", frameErrorCount,
			U" frame(s) out of ", numberOfFrames, U" could not be optimised.");
		//Melder_casual (U"Number of iterations: ", iter, U"\n   Average per frame: ", (double) iter / numberOfFrames);
		return him;
	} catch (MelderError) {
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
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1.0) < 1.0e-12)
			sound = Data_copy (me);   // will be modified
		else
			sound = Sound_resample (me, maximumFrequency * 2.0, 50);

		autoLPC lpc = Sound_to_LPC_auto (sound.get(), predictionOrder, halfdt_window, dt, preEmphasisFrequency);
		autoLPC lpcr = LPC_Sound_to_LPC_robust (lpc.get(), sound.get(), halfdt_window, preEmphasisFrequency, k, itermax, tol, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcr.get(), safetyMargin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}

/* End of file Sound_and_LPC_robust.cpp */
