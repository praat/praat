/* Sound_to_Formant_mt.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include <thread>
#include "Sound_to_Formant_mt.h"
#include "SoundFrameIntoFormantFrame.h"
#include "Sound_extensions.h"
/*
	Precondition:
		Sound already has the 'right' sampling frequency and has been preemphasized
*/

static autoFormant createFormant_common (constSound me, double dt, integer numberOfPoles, double effectiveAnalysisWidth,
	double safetyMargin)
{
	integer numberOfFrames;
	double t1;
	const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
	Sampled_shortTermAnalysis (me, physicalAnalysisWidth, dt, & numberOfFrames, & t1);
	const integer numberOfFormants = numberOfFormantsFromNumberOfCoefficients (numberOfPoles, safetyMargin);
	autoFormant formant = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
	return formant;
}

autoFormant Sound_to_Formant_burg_mt (constSound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin)
{
	try {
		const double dt = dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0;
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preEmphasisFrequency);
		const integer numberOfPoles = numberOfPolesFromNumberOfFormants (numberOfFormants);
		autoFormant formant = createFormant_common (sound.get(), dt, numberOfPoles, effectiveAnalysisWidth, safetyMargin);
		autoLPC lpc = LPC_create (my xmin, my xmax, formant -> nx, formant -> dx, formant -> x1, numberOfPoles, sound -> dx);
		autoSoundFrameIntoLPCFrameBurg first = SoundFrameIntoLPCFrameBurg_create (me, lpc.get(), effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		autoLPCFrameIntoFormantFrame second = LPCFrameIntoFormantFrame_create (lpc.get(), formant.get(), safetyMargin);
		autoSoundFrameIntoFormantFrame soundIntoFormant = SoundFrameIntoFormantFrame_create (first.releaseToAmbiguousOwner(), second.releaseToAmbiguousOwner());
		autoSoundIntoFormantStatus status = SoundIntoFormantStatus_create (formant -> nx);
		autoSampledIntoSampled sis = SampledIntoSampled_create (me, formant.get(), soundIntoFormant.releaseToAmbiguousOwner(),
			status.releaseToAmbiguousOwner());
		SampledIntoSampled_analyseThreaded (sis.get());
		return formant;
	} catch (MelderError) {
		Melder_throw (U"Could not create Formant (burg).");
	}
}

autoFormant Sound_and_LPC_to_Formant (constSound me, constLPC lpc, double effectiveAnalysisWidth, double preEmphasisFrequency, 
	double safetyMargin, double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	try {
		const double maximumFrequency = 1.0 / lpc -> samplingPeriod;
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preEmphasisFrequency);
		autoFormant formant = createFormant_common (sound.get(), lpc -> dx, lpc -> maxnCoefficients, effectiveAnalysisWidth, safetyMargin);
		autoLPC outputlpc = Data_copy (lpc);
		autoLPCAndSoundFramesIntoLPCFrameRobust lpcAndSoundIntoLPC = LPCAndSoundFramesIntoLPCFrameRobust_create (lpc, me, outputlpc.get(),
			effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2, k_stdev, itermax, tol, location, wantlocation);
		autoLPCFrameIntoFormantFrame lpcFrameIntoFormant = LPCFrameIntoFormantFrame_create (outputlpc.get(), formant.get(), safetyMargin);
		autoSoundFrameIntoFormantFrame sif = SoundFrameIntoFormantFrame_create (lpcAndSoundIntoLPC.get(), lpcFrameIntoFormant.get());
		autoSoundAndLPCIntoFormantRobustStatus status = SoundAndLPCIntoFormantRobustStatus_create (formant -> nx);
		autoSampledIntoSampled sis = SampledIntoSampled_create (me, formant.get(), sif.releaseToAmbiguousOwner(),
			status.releaseToAmbiguousOwner());
		SampledIntoSampled_analyseThreaded (sis.get());
		return formant;
	} catch (MelderError) {
		Melder_throw (U"Could not create Formant from Sound and LPC.");
	}
}

autoFormant Sound_to_Formant_robust_mt (constSound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin, double k_stdev, integer itermax, double tol,
	double location, bool wantlocation)
{
	try {
		const double dt = dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0;
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preEmphasisFrequency);
		const integer numberOfPoles = numberOfPolesFromNumberOfFormants (numberOfFormants);
		autoFormant formant = createFormant_common (sound.get(), dt, numberOfPoles, effectiveAnalysisWidth, safetyMargin);
		autoLPC lpc = LPC_create (my xmin, my xmax, formant -> nx, formant -> dx, formant -> x1, numberOfPoles, my dx);
		autoLPC outputLPC = Data_copy (lpc.get());
		const kSound_windowShape windowShape = kSound_windowShape::GAUSSIAN_2;
		autoSoundFrameIntoLPCFrameAuto soundIntoLPC1 = SoundFrameIntoLPCFrameAuto_create (me, lpc.get(), effectiveAnalysisWidth, windowShape);
		autoLPCAndSoundFramesIntoLPCFrameRobust lpcAndSoundIntoLPC = LPCAndSoundFramesIntoLPCFrameRobust_create (lpc.get(), me, outputLPC.get(),
			effectiveAnalysisWidth, windowShape, k_stdev, itermax, tol, location, wantlocation);
		autoSoundFrameIntoLPCFrame soundIntoLPC2 = SoundFrameIntoLPCFrameRobust_create (soundIntoLPC1.get(), lpcAndSoundIntoLPC.get());
		autoLPCFrameIntoFormantFrame lpcFrameIntoFormant = LPCFrameIntoFormantFrame_create (outputLPC.get(), formant.get(), safetyMargin);
		autoSoundFrameIntoFormantFrame sif = SoundFrameIntoFormantFrame_create (soundIntoLPC2.get(), lpcFrameIntoFormant.get());
		autoSoundIntoFormantRobustStatus status = SoundIntoFormantRobustStatus_create (formant -> nx);
		autoSampledIntoSampled sis = SampledIntoSampled_create (me, formant.get(), sif.releaseToAmbiguousOwner(),
			status.releaseToAmbiguousOwner());
		SampledIntoSampled_analyseThreaded (sis.get());
		return formant;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}
/*
void Sound_into_Formant_robust_mt (constSound me, mutableFormant thee, double effectiveAnalysisWidth, integer numberOfPoles, double safetyMargin,
	double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	autoSoundFrameIntoFormantFrameRobust ws = SoundFrameIntoFormantFrameRobust_create (me, thee,
		effectiveAnalysisWidth, kSound_windowShape :: GAUSSIAN_2, k_stdev, itermax, tol, location, wantlocation, numberOfPoles, safetyMargin);
	
	
	SampledToSampled_analyseThreaded (ws.get());
}

autoFormant Sound_to_Formant_robust_mt (constSound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preEmphasisFrequency, double safetyMargin, double k_stdev, integer itermax, double tol,
	double location, bool wantlocation)
{
	const double dt = dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0;
	try {
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preEmphasisFrequency);
		integer numberOfFrames;
		double t1;
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		Sampled_shortTermAnalysis (sound.get(), physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		const integer numberOfPoles = numberOfPolesFromNumberOfFormants (numberOfFormants);
		const integer numberOfFormants = numberOfFormantsFromNumberOfCoefficients (numberOfPoles, safetyMargin);
		autoFormant formant = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		Sound_into_Formant_robust_mt (sound.get(), formant.get(), effectiveAnalysisWidth, numberOfPoles, safetyMargin, k_stdev,
			itermax, tol, location, wantlocation);
		return formant;
	} catch (MelderError) {
		Melder_throw (me, U": no robust Formant created.");
	}
}
*/
/* End of file Sound_to_Formant_mt.cpp */
