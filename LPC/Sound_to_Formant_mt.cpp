/* Sound_to_Formant_mt.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "Sound_to_Formant_mt.h"
#include "SoundToFormantWorkspace.h"
#include "Sound_extensions.h"
/*
	Precondition:
		Sound already has the 'right' sampling frequency and has been preemphasized
*/

static integer getNumberOfFormants (double numberOfFormants) {
	return (Melder_iround (2.0 * numberOfFormants) + 1) / 2;
}

void Sound_into_Formant_burg_mt (constSound me, Formant thee, double effectiveAnalysisWidth, integer numberOfPoles, double safetyMargin) {
	try {
		autoSoundToFormantBurgWorkspace ws = SoundToFormantBurgWorkspace_create (me, thee, effectiveAnalysisWidth,
			kSound_windowShape :: GAUSSIAN_2, numberOfPoles, safetyMargin);
		SampledToSampledWorkspace_analyseThreaded (ws.get());
	} catch (MelderError) {
		Melder_throw (me, U": Formant could not be calculated.");
	}
}

autoFormant Sound_to_Formant_burg_mt (constSound me, double dt_in, double numberOfFormants, double maximumFrequency,
	double effectiveAnalysisWidth, double preemphasisFrequency, double safetyMargin)
{
	try {
		const double dt = dt_in > 0.0 ? dt_in : effectiveAnalysisWidth / 4.0;
		autoSound sound = Sound_resampleAndOrPreemphasize (me, maximumFrequency, 50, preemphasisFrequency);
		integer numberOfFrames;
		double t1;
		const double physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, kSound_windowShape::GAUSSIAN_2);
		Sampled_shortTermAnalysis (me, physicalAnalysisWidth, dt, & numberOfFrames, & t1);
		const integer numberOfPoles = numberOfPolesFromNumberOfFormants (numberOfFormants);
		const integer numberOfFormants = numberOfFormantsFromNumberOfCoefficients (numberOfPoles, safetyMargin);
		autoFormant formant = Formant_create (my xmin, my xmax, numberOfFrames, dt, t1, numberOfFormants);
		Sound_into_Formant_burg_mt (me, formant.get(), effectiveAnalysisWidth, numberOfPoles, safetyMargin);
		return formant;
	} catch (MelderError) {
		Melder_throw (me, U"Could not create Formant.");
	}
}

void Sound_into_Formant_robust_mt (constSound me, mutableFormant thee, double effectiveAnalysisWidth, integer numberOfPoles, double safetyMargin,
	double k_stdev, integer itermax, double tol, double location, bool wantlocation)
{
	autoSoundToFormantRobustWorkspace ws = SoundToFormantRobustWorkspace_create (me, thee,
		effectiveAnalysisWidth, kSound_windowShape :: GAUSSIAN_2, k_stdev, itermax, tol, location, wantlocation, numberOfPoles, safetyMargin);
	SampledToSampledWorkspace_analyseThreaded (ws.get());
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



/* End of file Sound_to_Formant_mt.cpp */
