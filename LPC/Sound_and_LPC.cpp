/* Sound_and_LPC.cpp
 *
 * Copyright (C) 1994-2024 David Weenink
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
 djmw 20020625 GPL header
*/

#include "SoundToLPCWorkspace.h"

#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include "Spectrum.h"
#include "NUM2.h"


static void Sound_and_LPC_require_equalDomainsAndSamplingPeriods (constSound me, constLPC thee) {
	Melder_require (my xmin == thy xmin && thy xmax == my xmax,
			U"The domains of the Sound and the LPC should be equal.");
	Melder_require (my dx == thy samplingPeriod,
			U"The sampling periods of the Sound and the LPC should be equal.");
}

autoLPC LPC_createEmptyFromAnalysisSpecifications (constSound me, int predictionOrder, double physicalAnalysisWidth, double dt) {
	try {
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);		
		integer numberOfFrames;
		double t1;
		Sampled_shortTermAnalysis (me, physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		autoLPC thee = LPC_create (my xmin, my xmax, numberOfFrames, dt, t1, predictionOrder, my dx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": LPC not created from specification.");
	}
}

void Sound_into_LPC_autocorrelation (constSound me, mutableLPC thee, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCAutocorrelationWorkspace ws = SoundToLPCAutocorrelationWorkspace_create (
		me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2
	);
	SampledToSampledWorkspace_analyseThreaded (ws.get());
}

autoLPC Sound_to_LPC_autocorrelation (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (emphasized.get(), predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_autocorrelation (emphasized.get(), thee.get(), effectiveAnalysisWidth);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (auto) created.");
	}
}

void Sound_into_LPC_covariance (constSound me, mutableLPC thee, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCCovarianceWorkspace ws = SoundToLPCCovarianceWorkspace_create (
		me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SampledToSampledWorkspace_analyseThreaded (ws.get());
}

autoLPC Sound_to_LPC_covariance (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (emphasized.get(), predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_covariance (emphasized.get(), thee.get(), effectiveAnalysisWidth);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (covar) created.");
	}
}

void Sound_into_LPC_burg (constSound me, mutableLPC thee, double effectiveAnalysisWidth) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCBurgWorkspace ws = SoundToLPCBurgWorkspace_create (
		me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	SampledToSampledWorkspace_analyseThreaded (ws.get());
}

autoLPC Sound_to_LPC_burg (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (emphasized.get(), predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_burg (emphasized.get(), thee.get(), effectiveAnalysisWidth);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (burg) created.");
	}
}

void Sound_into_LPC_marple (constSound me, mutableLPC thee, double effectiveAnalysisWidth, double tol1, double tol2) {
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCMarpleWorkspace ws = SoundToLPCMarpleWorkspace_create (
		me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, tol1, tol2);
	SampledToSampledWorkspace_analyseThreaded (ws.get());
}

autoLPC Sound_to_LPC_marple (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency,
	double tol1, double tol2)
{
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, predictionOrder);
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (emphasized.get(), predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_marple (emphasized.get(), thee.get(), effectiveAnalysisWidth, tol1, tol2);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (marple) created.");
	}
}


void LPC_and_Sound_into_LPC_robust (constLPC original, constSound me, mutableLPC output, double effectiveAnalysisWidth, double k_stdev,
	integer itermax, double tol, bool wantlocation)
{
	try {
		Melder_assert (original -> xmin == output -> xmin && original -> xmax == output -> xmax);
		Melder_assert (original -> nx == output -> nx && original -> maxnCoefficients == output -> maxnCoefficients);
		Melder_assert (original -> samplingPeriod == output -> samplingPeriod && original -> x1 == output -> x1); 
		Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, original);	
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		double location = 0.0;
		checkLPCAnalysisParameters_e (my dx, my nx, physicalAnalysisWidth, output -> maxnCoefficients);
		
		autoSoundAndLPCToLPCRobustWorkspace ws = SoundAndLPCToLPCRobustWorkspace_create (me, original, output,
			effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, k_stdev, itermax, tol, location, wantlocation);		
		SampledToSampledWorkspace_analyseThreaded (ws.get());
	} catch (MelderError) {
		Melder_throw (me, U": no robust LPC calculated.");
	}	
}

autoLPC LPC_and_Sound_to_LPC_robust (constLPC thee, constSound me, double effectiveAnalysisWidth, 
	double preEmphasisFrequency, double k_stdev, integer itermax, double tol, bool wantlocation)
{
	try {
		Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);	
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC result = LPC_create (thy xmin, thy xmax, thy nx, thy dx, thy x1, thy maxnCoefficients, thy samplingPeriod);
		LPC_and_Sound_into_LPC_robust (thee, emphasized.get(), result.get(), effectiveAnalysisWidth, k_stdev, itermax, tol, wantlocation);
		return result;
	} catch (MelderError) {
		Melder_throw (me, U": no robust LPC created.");
	}	
}

void Sound_into_LPC_robust (constSound me, mutableLPC thee, double effectiveAnalysisWidth, 
	double k_stdev, integer itermax, double tol, bool wantlocation)
{
	Sound_and_LPC_require_equalDomainsAndSamplingPeriods (me, thee);
	autoSoundToLPCRobustWorkspace ws = SoundToLPCRobustWorkspace_create (me, thee, effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, k_stdev, itermax, tol, 0.0, wantlocation);
	
	SampledToSampledWorkspace_analyseThreaded (ws.get());
}

autoLPC Sound_to_LPC_robust (constSound me, int predictionOrder, double effectiveAnalysisWidth, double dt, double preEmphasisFrequency,
	double k_stdev,	integer itermax, double tol, bool wantlocation) {
	try {
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		autoSound emphasized = Sound_resampleAndOrPreemphasize (me, 0.0, 0, preEmphasisFrequency);
		autoLPC thee = LPC_createEmptyFromAnalysisSpecifications (emphasized.get(), predictionOrder, physicalAnalysisWidth, dt);
		Sound_into_LPC_robust (emphasized.get(), thee.get(), effectiveAnalysisWidth, k_stdev, itermax, tol, wantlocation);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no LPC (robust) created.");
	}
}

/*********************** analysis ******************************/

void checkLPCAnalysisParameters_e (double sound_dx, integer sound_nx, double physicalAnalysisWidth, integer predictionOrder)
{
	volatile const double physicalDuration = sound_dx * sound_nx;
	Melder_require (physicalAnalysisWidth <= physicalDuration,
		U"Your sound is too short: it should be at least as long as two window lengths. "
		"Either your sound is too short or your window is too long.");
	// we round the minimum duration to be able to use asserterror in testing scripts.
	conststring32 minimumDurationRounded = Melder_fixed (predictionOrder * sound_dx , 5);
	const integer approximateNumberOfSamplesPerWindow = Melder_roundDown (physicalAnalysisWidth / sound_dx);
	Melder_require (approximateNumberOfSamplesPerWindow > predictionOrder,
		U"Analysis window duration too short. For a prediction order of ", predictionOrder,
		U", the analysis window duration should be greater than ", minimumDurationRounded,
		U" s. Please increase the analysis window duration or lower the prediction order.");
}


/*********************** (inverse) filtering ******************************/

static void LPC_Frame_Sound_filter (constLPC_Frame me, mutableSound thee, integer channel) {
	const VEC y = thy z.row (channel);
	for (integer i = 1; i <= thy nx; i ++) {
		const integer m = ( i > my nCoefficients ? my nCoefficients : i - 1 );   // ppgb: what is m?
		for (integer j = 1; j <= m; j ++)
			y [i] -= my a [j] * y [i - j];
	}
}

autoSound LPC_Sound_filterInverse (constLPC me, constSound thee) {
	try {
		Melder_require (my samplingPeriod == thy dx,
			U"The sampling frequencies should be equal.");
		Melder_require (my xmin == thy xmin && thy xmax == my xmax,
			U"The domains of LPC and Sound should be equal.");
		
		autoSound him = Data_copy (thee);
		VEC source = his z.row (1);
		VEC sound = thy z.row (1);
		for (integer isamp = 1; isamp <= his nx; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				source [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSoundDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSoundDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				source [isamp] += frame -> a [icoef] * sound [isamp - icoef];
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/*
	Gain used as a constant amplitude multiplier within a frame of duration my dx.
	future alternative: convolve gain with a smoother.
*/
autoSound LPC_Sound_filter (constLPC me, constSound thee, bool useGain) {
	try {
		const double xmin = std::max (my xmin, thy xmin);
		const double xmax = std::min (my xmax, thy xmax);
		Melder_require (xmin < xmax,
			U"Domains of Sound [", thy xmin, U",", thy xmax, U"] and LPC [",
			my xmin, U",", my xmax, U"] should overlap."
		);
		/*
			Resample the sound if the sampling frequencies do not match.
		*/
		autoSound source;
		if (my samplingPeriod != thy dx) {
			source = Sound_resample (thee, 1.0 / my samplingPeriod, 50);
			thee = source.get();   // reference copy; remove at end
		}

		autoSound him = Data_copy (thee);

		const integer ifirst = std::max (1_integer, Sampled_xToHighIndex (thee, xmin));
		const integer ilast = std::min (Sampled_xToLowIndex (thee, xmax), thy nx);
		for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
			const double sampleTime = Sampled_indexToX (him.get(), isamp);
			const integer frameNumber = Sampled_xToNearestIndex (me, sampleTime);
			if (frameNumber < 1 || frameNumber > my nx) {
				his z [1] [isamp] = 0.0;
				continue;
			}
			const LPC_Frame frame = & my d_frames [frameNumber];
			const integer maximumFilterDepth = frame -> nCoefficients;
			const integer maximumSourceDepth = isamp - 1;
			const integer usableDepth = std::min (maximumFilterDepth, maximumSourceDepth);
			for (integer icoef = 1; icoef <= usableDepth; icoef ++)
				his z [1] [isamp] -= frame -> a [icoef] * his z [1] [isamp - icoef];
		}
		/*
			Make samples before first frame and after last frame zero.
		*/
		for (integer isamp = 1; isamp < ifirst; isamp ++)
			his z [1] [isamp] = 0.0;
		for (integer isamp = ilast + 1; isamp <= his nx; isamp ++)
			his z [1] [isamp] = 0.0;

		if (useGain) {
			for (integer isamp = ifirst; isamp <= ilast; isamp ++) {
				const double sampleTime = Sampled_indexToX (him.get(), isamp);
				const double realFrameNumber = Sampled_xToIndex (me, sampleTime);
				const integer leftFrameNumber = Melder_ifloor (realFrameNumber);
				const integer rightFrameNumber = leftFrameNumber + 1;
				const double phase = realFrameNumber - leftFrameNumber;
				if (rightFrameNumber < 1 || leftFrameNumber > my nx)
					his z [1] [isamp] = 0.0;
				else if (rightFrameNumber == 1)
					his z [1] [isamp] *= sqrt (my d_frames [1]. gain) * phase;
				else if (leftFrameNumber == my nx)
					his z [1] [isamp] *= sqrt (my d_frames [my nx]. gain) * (1.0 - phase);
				else 
					his z [1] [isamp] *=
							phase * sqrt (my d_frames [rightFrameNumber]. gain) +
							(1.0 - phase) * sqrt (my d_frames [leftFrameNumber]. gain);
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time) {
	integer frameIndex = Sampled_xToNearestIndex (me, time);
	Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
	if (channel > thy ny)
		channel = 1;
	Melder_require (frameIndex > 0 && frameIndex <= my nx,
		U"Frame should be in the range [1, ", my nx, U"].");

	if (channel > 0)
		LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, channel);
	else
		for (integer ichan = 1; ichan <= thy ny; ichan ++)
			LPC_Frame_Sound_filter (& my d_frames [frameIndex], thee, ichan);
}

autoSound LPC_Sound_filterWithFilterAtTime (constLPC me, constSound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not filtered.");
	}
}

void LPC_Sound_filterInverseWithFilterAtTime_inplace (constLPC me, mutableSound thee, integer channel, double time) {
	try {
		integer frameIndex = Sampled_xToNearestIndex (me, time);
		Melder_clip (1_integer, & frameIndex, my nx);   // constant extrapolation
		if (channel > thy ny)
			channel = 1;
		LPC_Frame lpc = & my d_frames [frameIndex];
		autoVEC workspace = raw_VEC (lpc -> nCoefficients);
		if (channel > 0)
			VECfilterInverse_inplace (thy z.row (channel), lpc -> a.get(), workspace.get());
		else
			for (integer ichan = 1; ichan <= thy ny; ichan ++)
				VECfilterInverse_inplace (thy z.row (ichan), lpc -> a.get(), workspace.get());
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

autoSound LPC_Sound_filterInverseWithFilterAtTime (constLPC me, constSound thee, integer channel, double time) {
	try {
		autoSound him = Data_copy (thee);
		LPC_Sound_filterInverseWithFilterAtTime_inplace (me, him.get(), channel, time);
		return him;
	} catch (MelderError) {
		Melder_throw (thee, U": not inverse filtered.");
	}
}

/* End of file Sound_and_LPC.cpp */
