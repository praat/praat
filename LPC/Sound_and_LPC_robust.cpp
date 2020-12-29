/* Sound_and_LPC_robust.cpp
 *
 * Copyright (C) 1994-2019 David Weenink
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
	autoVEC error;
	double k_stdev, tol, tol_svd;
	integer numberOfSamples, predictionOrder, maximumPredictionOrder;
	integer iter, itermax, huber_iterations = 5;
	bool wantlocation, wantscale;
	double location, scale;
	autoVEC workSpace;
	autoVEC weights, coefficients, covariancesw;
	autoMAT covarmatrixw;
	autoSVD svd;
};

static void huber_struct_init (struct huber_struct *me, integer numberOfSamples, integer maximumPredictionOrder, double location, bool wantlocation) {
	my numberOfSamples = numberOfSamples;
	my error = zero_VEC (numberOfSamples);
	my k_stdev = my tol = my tol_svd = my scale = 0.0;
	my iter = my itermax = 1;
	my wantlocation = wantlocation;
	if (! wantlocation)
		my location = location;
	my wantscale = true;
	my predictionOrder = my maximumPredictionOrder = maximumPredictionOrder;
	my weights = zero_VEC (numberOfSamples);
	my workSpace = raw_VEC (numberOfSamples);
	my coefficients = raw_VEC (maximumPredictionOrder);
	my covariancesw = zero_VEC (maximumPredictionOrder);
	my covarmatrixw = zero_MAT (maximumPredictionOrder, maximumPredictionOrder);
	//my covarmatrixw = matrixview(my covarmatrixstaticAllocation.get(), 1, my predictionOrder, 1, my predictionOrder);
	my svd = SVD_create (maximumPredictionOrder, maximumPredictionOrder);
}

void huber_struct_resize (struct huber_struct *me, integer newPredictionOrder) {
	Melder_assert (newPredictionOrder <= my maximumPredictionOrder);
	if (newPredictionOrder == my predictionOrder)
		return;
	my coefficients.resize (newPredictionOrder);
	my covariancesw.resize (newPredictionOrder);
	my svd -> numberOfColumns = my svd -> numberOfRows = newPredictionOrder;
	// TODO also resize the u, v and d matrices
	// temporary solution do svd on the complete matrix with zeros added.
}

static void huber_struct_getWeights (struct huber_struct *me, constVEC const& error) {
	Melder_assert (error.size == my numberOfSamples);
	const double kstdev = my k_stdev * my scale;

	for (integer isamp = 1 ; isamp <= my numberOfSamples; isamp ++) {
		const double absDiff = fabs (error [isamp] - my location);
		my weights [isamp] = absDiff < kstdev ? 1.0 : kstdev / absDiff;
	}
}

static void huber_struct_getWeightedCovars (struct huber_struct *me, constVEC const& s) {
	Melder_assert (s.size == my numberOfSamples);
	MATVU covar = MATVU (my covarmatrixw.part (1, my predictionOrder, 1, my predictionOrder));
	for (integer i = 1; i <= my predictionOrder; i ++) {
		for (integer j = i; j <= my predictionOrder; j ++) {
			longdouble cv1 = 0.0;
			for (integer k = my predictionOrder + 1; k <= my numberOfSamples; k ++)
				cv1 += s [k - j] * s [k - i] *  my weights [k];
			covar [i] [j] = covar [j] [i] = (double) cv1;
		}
		longdouble cv2 = 0.0;
		for (integer k = my predictionOrder + 1; k <= my numberOfSamples; k ++)
			cv2 += s [k - i] * s [k] *  my weights [k];
		my covariancesw [i] = - cv2;
	}
}

static void huber_struct_solvelpc (struct huber_struct *me) {
	// we cannot resize the svd-matrices therefore add zero's and svd the full matrix
	if (my predictionOrder < my maximumPredictionOrder) {
		my covarmatrixw. part (my predictionOrder + 1, my maximumPredictionOrder, 1, my maximumPredictionOrder) <<= 0.0;
		my covarmatrixw. part (1, my predictionOrder, my predictionOrder + 1, my maximumPredictionOrder) <<= 0.0;
		my coefficients.resize (my maximumPredictionOrder);
	}
	my svd -> u.all()  <<=  my covarmatrixw.all();
	SVD_setTolerance (my svd.get(), my tol_svd);
	SVD_compute (my svd.get());
	SVD_solve_preallocated (my svd.get(), my covariancesw.get(), my coefficients.get());
	my coefficients.resize (my predictionOrder); // maintain invariant
}

void huber_struct_minimize (struct huber_struct *me, constVEC const& sound, constVEC const& lpcFrom, VEC const& lpcTo) {
	Melder_assert (lpcFrom.size == lpcTo.size);
	Melder_assert (lpcFrom.size <= my predictionOrder);
	Melder_assert (sound.size == my numberOfSamples);

	my iter = 0;
	my scale = 1e308;
	bool farFromScale = true;
	do {
		const double previousScale = my scale;
		my error.all()  <<=  sound;
		VECfilterInverse_inplace (my error.get(), lpcTo, my workSpace); // lpcTo has alreay a copy of lpcFrom
		NUMstatistics_huber (my error.get(), & my location, my wantlocation, & my scale, my wantscale, my k_stdev, my tol, my huber_iterations, my workSpace);

		huber_struct_getWeights (me, my error.get());
		huber_struct_getWeightedCovars (me, sound);
		/*
			Solve C a = [-] c
		*/
		try {
			huber_struct_solvelpc (me);
		} catch (MelderError) {
			lpcTo  <<=  lpcFrom; // No change could be made
			throw MelderError();
		}
		lpcTo  <<=  my coefficients.all();
		farFromScale = ( fabs (my scale - previousScale) > std::max (my tol * fabs (my scale), NUMeps) );
	} while (++ my iter < my itermax && farFromScale);
}

autoLPC LPC_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth, double preEmphasisFrequency, double k_stdev,
	integer itermax, double tol, bool wantlocation) {
	struct huber_struct struct_huber;
	try {
		const double samplingFrequency = 1.0 / my dx, tol_svd = 0.000001;
		const double windowDuration = 2 * analysisWidth; /* Gaussian window */
		const integer predictionOrder = thy maxnCoefficients;
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"Time domains should be equal.");
		Melder_require (my dx == thy samplingPeriod,
			U"Sampling intervals should be equal.");
		Melder_require (Melder_roundDown (windowDuration / my dx) > predictionOrder,
			U"Analysis window too short.");
		double t1;
		integer numberOfFrames;
		Sampled_shortTermAnalysis (me, windowDuration, thy dx, & numberOfFrames, & t1);
		Melder_require (numberOfFrames == thy nx && t1 == thy x1,
			U"Incorrect retrieved analysis width.");

		autoSound sound = Data_copy (me);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoLPC him = Data_copy (thee);
		double location = 0.0;
		huber_struct_init (& struct_huber, window -> nx, predictionOrder, location, wantlocation);
		struct_huber.k_stdev = k_stdev;
		struct_huber.tol = tol;
		struct_huber.tol_svd = tol_svd;
		struct_huber.itermax = itermax;

		autoMelderProgress progess (U"LPC analysis");

		Sound_preEmphasis (sound.get(), preEmphasisFrequency);
		integer frameErrorCount = 0, iter = 0;
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const LPC_Frame lpc = & thy d_frames [iframe];
			const LPC_Frame lpcto = & his d_frames [iframe];
			const double t = Sampled_indexToX (thee, iframe);

			Sound_into_Sound (sound.get(), sframe.get(), t - windowDuration / 2);
			Vector_subtractMean (sframe.get());
			Sounds_multiply (sframe.get(), window.get());
			//huber_struct_resize (& struct_huber, lpc -> nCoefficients);
			try {
				huber_struct_minimize (& struct_huber, sframe -> z.row(1), lpc -> a.get(), lpcto -> a.get());
			} catch (MelderError) {
				frameErrorCount ++;
			}

			iter += struct_huber.iter;

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames,
					U"LPC analysis of frame ", iframe, U" out of ", numberOfFrames, U".");
		}

		if (frameErrorCount > 0)
			Melder_warning (U"Results of ", frameErrorCount, U" frame(s) out of ", numberOfFrames, 
				U" could not be optimised.");
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no robust LPC created.");
	}
}

autoFormant Sound_to_Formant_robust (Sound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double halfdt_window, double preEmphasisFrequency, double safetyMargin, double k, integer itermax, double tol, bool wantlocation) {
	const double dt = dt_in > 0.0 ? dt_in : halfdt_window / 4.0;
	const double nyquist = 0.5 / my dx;
	const integer predictionOrder = Melder_ifloor (2 * numberOfFormants);
	try {
		autoSound sound;
		if (maximumFrequency <= 0.0 || fabs (maximumFrequency / nyquist - 1.0) < 1.0e-12)
			sound = Data_copy (me);   // will be modified
		else
			sound = Sound_resample (me, maximumFrequency * 2.0, 50);

		autoLPC lpc = Sound_to_LPC_autocorrelation (sound.get(), predictionOrder, halfdt_window, dt, preEmphasisFrequency);
		autoLPC lpcRobust = LPC_Sound_to_LPC_robust (lpc.get(), sound.get(), halfdt_window, preEmphasisFrequency, k, itermax, tol, wantlocation);
		autoFormant thee = LPC_to_Formant (lpcRobust.get(), safetyMargin);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}

/* End of file Sound_and_LPC_robust.cpp */
