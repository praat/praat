/* FormantPath.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "FormantPath.h"
#include "LPC_and_Formant.h"
#include "Sound_to_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound.h"
#include "Sound_and_LPC_robust.h"

#include "oo_DESTROY.h"
#include "FormantPath_def.h"
#include "oo_COPY.h"
#include "FormantPath_def.h"
#include "oo_EQUAL.h"
#include "FormantPath_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantPath_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantPath_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantPath_def.h"
#include "oo_READ_TEXT.h"
#include "FormantPath_def.h"
#include "oo_READ_BINARY.h"
#include "FormantPath_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantPath_def.h"

void structFormantPath :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of Formant objects: ", formants . size);
	for (integer ic = 1; ic <= ceilings.size; ic ++)
		MelderInfo_writeLine (U"Ceiling ", ic, U": ", ceilings [ic], U" Hz");
}

double structFormantPath :: v_getValueAtSample (integer iframe, integer which, int units) {
	const Formant formant = reinterpret_cast<Formant> (our formants.at [our path [iframe]]);
	return formant -> v_getValueAtSample (iframe, which, units);
}

conststring32 structFormantPath :: v_getUnitText (integer level, int unit, uint32 flags) {
	return U"Frequency (Hz)";
	
};

Thing_implement (FormantPath, Function, 0);

autoFormantPath FormantPath_create (double xmin, double xmax, integer nx, double dx, double x1, integer numberOfCeilings) {
	autoFormantPath me = Thing_new (FormantPath);
	Sampled_init (me.get (), xmin, xmax, nx, dx, x1);
	my ceilings = newVECzero (numberOfCeilings);
	my path = newINTVECzero (nx);
	return me;
}

autoFormant FormantPath_extractFormant (FormantPath me) {
	Formant formant = my formants. at [1];
	autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, formant -> maxnFormants);
	for (integer iframe = 1; iframe <= my path.size; iframe ++) {
		Formant source = reinterpret_cast <Formant> (my formants. at [my path [iframe]]);
		Formant_Frame targetFrame = & thy frames [iframe];
		Formant_Frame sourceFrame = & source -> frames [iframe];
		sourceFrame -> copy (targetFrame);
	}
	return thee;
}

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants,
	double formantCeiling, double windowLength, double preemphasisFrequency, double ceilingStepFraction, 
	integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol,
	integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel) {
	try {
		Melder_require (ceilingStepFraction > 0.0 && ceilingStepFraction < 1.0,
			U"The ceiling step fraction should be a number between 0.0 and 1.0");
		const double nyquistFrequency = 0.5 / my dx;
		const integer numberOfCeilings = 2 * numberOfStepsToACeiling + 1;
		const double minimumCeiling = formantCeiling * pow (1.0 - ceilingStepFraction, numberOfStepsToACeiling);
		Melder_require (minimumCeiling > 0.0,
			U"Your minimum ceiling is ", minimumCeiling, U" Hz, but it should be positive.\n"
			"We computed it as your middle ceiling (", formantCeiling, U" Hz) times (1.0 - ", ceilingStepFraction, 
			U")^", numberOfStepsToACeiling, U" Hz. Decrease the ceiling step or the number of steps or both.");
		const double maximumCeiling = formantCeiling * pow (1.0 +  ceilingStepFraction, numberOfStepsToACeiling);		
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should be smaller than ", nyquistFrequency, U" Hz. "
			"Decrease the 'ceiling step' or the 'number of steps' or both.");
		integer fake_nx = 1; double fake_x1 = 0.005, fake_dx = 0.001; // we know them after the analyses
		autoFormantPath thee = FormantPath_create (my xmin, my xmax, fake_nx, fake_dx, fake_x1, numberOfCeilings);
		autoSound sources [1 + numberOfCeilings];
		const double formantSafetyMargin = 50.0;
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		for (integer ic  = 1; ic <= numberOfCeilings; ic ++) {
			autoLPC lpc;
			double factor = 1.0;
			if (ic <= numberOfStepsToACeiling)
				factor = pow (1.0 - ceilingStepFraction, numberOfStepsToACeiling + 1 - ic);
			else if (ic > numberOfStepsToACeiling + 1)
				factor = pow (1.0 + ceilingStepFraction, ic - numberOfStepsToACeiling - 1);
			thy ceilings [ic] = formantCeiling * factor;
			autoSound resampled = Sound_resample (me, 2.0 * thy ceilings [ic], 50);
			if (lpcType == kLPC_Analysis::BURG)
				lpc = Sound_to_LPC_burg (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::AUTOCORRELATION)
				lpc = Sound_to_LPC_autocorrelation (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::COVARIANCE)
				lpc = Sound_to_LPC_covariance (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::MARPLE)
				lpc = Sound_to_LPC_marple (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency, marple_tol1, marple_tol2);
			else if (lpcType == kLPC_Analysis::ROBUST) {
				autoLPC lpc_in = Sound_to_LPC_autocorrelation (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
				lpc = LPC_Sound_to_LPC_robust (lpc_in.get(), resampled.get(), windowLength, preemphasisFrequency, huber_numberOfStdDev, huber_maximumNumberOfIterations, huber_tol, true);
			}
			autoFormant formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy formants . addItem_move (formant.move());
			if (sourcesMultiChannel) {
				autoSound source = LPC_Sound_filterInverse (lpc.get(), resampled.get ());
				sources [ic] = Sound_resample (source.get(), 2.0 * formantCeiling, 50).move();
			}
		}
		/*
			Maintain invariants
		*/
		Melder_assert (thy formants . size == numberOfCeilings);
		Formant formant = thy formants . at [numberOfStepsToACeiling + 1];
		thy nx = formant -> nx;
		thy dx = formant -> dx;
		thy x1 = formant -> x1;
		thy path = newINTVECraw (thy nx);
		for (integer i = 1; i <= thy path.size; i++)
			thy path [i] = numberOfStepsToACeiling + 1;
		if (sourcesMultiChannel) {
			Sound mid = sources [numberOfStepsToACeiling + 1].get();
			autoSound multiChannel = Sound_create (numberOfCeilings, mid -> xmin, mid -> xmax, mid -> nx, mid -> dx, mid -> x1);
			for (integer ic = 1; ic <= numberOfCeilings; ic ++) {
				Sound him = sources [ic] . get();
				const integer numberOfSamples = std::min (mid -> nx, his nx);
				multiChannel -> z.row (ic).part (1, numberOfSamples) <<= his z.row (1).part (1, numberOfSamples);
			}
			*sourcesMultiChannel = multiChannel.move();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantPath not created.");
	}
}

/* End of file FormantPath.cpp */
