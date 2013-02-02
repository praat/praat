/* Sound_and_LPC_robust.cpp
 *
 * Copyright (C) 1994-2013 David Weenink
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
	Sound e;
	double k, tol, tol_svd;
	long iter, itermax;
	int wantlocation, wantscale;
	double location, scale;
	long n, p;
	double *w, *work;
	double *a;
	double **covar, *c;
	SVD svd;
};

static void huber_struct_init (struct huber_struct *hs, double windowDuration,
                               long p, double samplingFrequency, double location, int wantlocation) {
	hs -> w = hs -> work = hs -> a = hs -> c = 0;
	hs -> covar = 0; hs -> svd = 0;
	hs -> e = Sound_createSimple (1, windowDuration, samplingFrequency);
	long n = hs -> e -> nx;
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
	forget (hs -> e);
	forget (hs -> svd);
	NUMvector_free<double> (hs -> w, 1);
	NUMvector_free<double> (hs -> work, 1);
	NUMvector_free<double> (hs -> a, 1);
	NUMmatrix_free<double> (hs -> covar, 1, 1);
	NUMvector_free<double> (hs -> c, 1);
}

static void huber_struct_getWeights (struct huber_struct *hs, double *e) {
	double ks = hs -> k * hs -> scale;
	double *w = hs -> w;

	for (long i = 1 ; i <= hs -> n; i++) {
		double ei = e[i] - hs -> location;
		w[i] = ei > -ks && ei < ks ? 1 : ks / fabs (ei);
	}
}

static void huber_struct_getWeightedCovars (struct huber_struct *hs, double *s) {
	long p = hs -> p, n = hs -> n;
	double *w = hs -> w, **covar = hs -> covar, *c = hs -> c;

	for (long i = 1; i <= p; i++) {
		for (long j = i; j <= p; j++) {
			double tmp = 0;
			for (long k = p + 1; k <= n; k++) {
				tmp += s[k - j] * s[k - i] * w[k];
			}
			covar[i][j] = covar[j][i] = tmp;
		}

		double tmp = 0;
		for (long k = p + 1; k <= n; k++) {
			tmp += s[k - i] * s[k] * w[k];
		}
		c[i] = -tmp;
	}
}

static void huber_struct_solvelpc (struct huber_struct *hs) {
	SVD me = hs -> svd;
	double **covar = hs -> covar;

	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= my numberOfColumns; j++) {
			my u[i][j] = covar[i][j];
		}
	}

	SVD_setTolerance (me, hs -> tol_svd);
	SVD_compute (me);

	//long nzeros = SVD_zeroSmallSingularValues (me, 0);

	SVD_solve (me, hs -> c, hs -> a);
}

void LPC_Frames_and_Sound_huber (LPC_Frame me, Sound thee, LPC_Frame him, struct huber_struct *hs) {
	long p = my nCoefficients > his nCoefficients ? his nCoefficients : my nCoefficients;
	long n = hs -> e -> nx > thy nx ? thy nx : hs -> e -> nx;
	double *e = hs -> e -> z[1], *s = thy z[1];

	hs -> iter = 0;
	hs -> scale = 1e38;
	hs -> p = p;

	double s0;
	do {
		Sound hse = hs -> e;
		for (long i = 1; i <= thy nx; i++) {
			hse -> z[1][i] = thy z[1][i];
		}
		LPC_Frame_and_Sound_filterInverse (him, hse, 1);

		s0 = hs -> scale;

		NUMstatistics_huber (e, n, & (hs -> location), hs -> wantlocation, & (hs -> scale), hs -> wantscale,
		                     hs -> k, hs -> tol, hs -> work);

		huber_struct_getWeights (hs, e);
		huber_struct_getWeightedCovars (hs, s);

		// Solve C a = [-] c */
		try {
			huber_struct_solvelpc (hs);
		} catch (MelderError) {
			// Copy the starting lpc coeffs */
			for (long i = 1; i <= p; i++) {
				his a[i] = my a[i];
			}
			throw MelderError();
		}
		for (long i = 1; i <= p; i++) {
			his a[i] = hs -> a[i];
		}

		(hs -> iter) ++;
	} while ( (hs -> iter < hs -> itermax) && (fabs (s0 - hs -> scale) > hs -> tol * s0));
}


LPC LPC_and_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth, double preEmphasisFrequency, double k,
	int itermax, double tol, int wantlocation) {
	struct huber_struct struct_huber = { 0 };
	try {
		double t1, samplingFrequency = 1.0 / my dx, tol_svd = 0.000001;
		double location = 0, windowDuration = 2 * analysisWidth; /* Gaussian window */
		long nFrames, frameErrorCount = 0, iter = 0;
		long p = thy maxnCoefficients;

		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("Time domains differ.");
		}
		if (my dx != thy samplingPeriod) {
			Melder_throw ("Sampling intervals differ.");
		}
		if (floor (windowDuration / my dx) < p + 1) {
			Melder_throw ("Analysis window too short.");
		}
		Sampled_shortTermAnalysis (me, windowDuration, thy dx, & nFrames, & t1);
		if (nFrames != thy nx || t1 != thy x1) {
			Melder_throw ("Incorrect retrieved analysis width");
		}

		autoSound sound = Data_copy (me);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoLPC him = Data_copy (thee);
		huber_struct_init (&struct_huber, windowDuration, p, samplingFrequency, location, wantlocation);

		struct_huber.k = k;
		struct_huber.tol = tol;
		struct_huber.tol_svd = tol_svd;
		struct_huber.itermax = itermax;

		autoMelderProgress progess (L"LPC analysis");

		Sound_preEmphasis (sound.peek(), preEmphasisFrequency);

		for (long i = 1; i <= nFrames; i++) {
			LPC_Frame lpc = (LPC_Frame) & thy d_frames[i];
			LPC_Frame lpcto = (LPC_Frame) & his d_frames[i];
			double t = Sampled_indexToX (thee, i);

			Sound_into_Sound (sound.peek(), sframe.peek(), t - windowDuration / 2);
			Vector_subtractMean (sframe.peek());
			Sounds_multiply (sframe.peek(), window.peek());

			try {
				LPC_Frames_and_Sound_huber (lpc, sframe.peek(), lpcto, & struct_huber);
			} catch (MelderError) {
				frameErrorCount++;
			}

			iter += struct_huber.iter;

			if ( (i % 10) == 1) {
				Melder_progress ( (double) i / nFrames, L"LPC analysis of frame ",
				                   Melder_integer (i), L" out of ", Melder_integer (nFrames), L".");
			}
		}

		if (frameErrorCount) Melder_warning (L"Results of ", Melder_integer (frameErrorCount),
			L" frame(s) out of ", Melder_integer (nFrames), L" could not be optimised.");
		MelderInfo_writeLine (L"Number of iterations: ", Melder_integer (iter),
			L"\n   Average per frame: ", Melder_double (((double) iter) / nFrames));
		huber_struct_destroy (&struct_huber);
		return him.transfer();
	} catch (MelderError) {
		huber_struct_destroy (&struct_huber);
		Melder_throw (me, ": no robust LPC created.");
	}
}

Formant Sound_to_Formant_robust (Sound me, double dt_in, int numberOfPoles, double maximumFrequency,
	double halfdt_window, double preEmphasisFrequency, double safetyMargin, double k, int itermax, double tol, int wantlocation)
{
	double dt = dt_in > 0.0 ? dt_in : halfdt_window / 4.0;
	double nyquist = 0.5 / my dx;
	long predictionOrder = 2 * numberOfPoles;
	try {
		autoSound sound = NULL;
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1) < 1.0e-12) {
			sound.reset (Data_copy (me));   // will be modified
		} else {
			sound.reset (Sound_resample (me, maximumFrequency * 2, 50));
		}

		autoLPC lpc = Sound_to_LPC_auto (sound.peek(), predictionOrder, halfdt_window, dt, preEmphasisFrequency);
		autoLPC lpcr = LPC_and_Sound_to_LPC_robust (lpc.peek(), sound.peek(), halfdt_window, preEmphasisFrequency, k, itermax, tol, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcr.peek(), safetyMargin);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no robust Formant created.");
	}
}

/* End of file Sound_and_LPC_robust.cpp */
