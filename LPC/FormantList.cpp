/* FormantList.cpp
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

#include "FormantList.h"
#include "LPC_and_Formant.h"
#include "Sound_to_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound.h"
#include "Sound_and_LPC_robust.h"

#include "oo_DESTROY.h"
#include "FormantList_def.h"
#include "oo_COPY.h"
#include "FormantList_def.h"
#include "oo_EQUAL.h"
#include "FormantList_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantList_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantList_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantList_def.h"
#include "oo_READ_TEXT.h"
#include "FormantList_def.h"
#include "oo_READ_BINARY.h"
#include "FormantList_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantList_def.h"

void structFormantList :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of Formant objects: ", formants . size);
	MelderInfo_writeLine (U"  Identifiers:");
	for (integer iformant = 1; iformant <= formants . size; iformant ++)
		MelderInfo_writeLine (U"  ", iformant, U": ", formantIdentifier [iformant].get(),
			( iformant == defaultFormantObject ? U" (default)" : U"" ));
}

Thing_implement (FormantList, Function, 0);

autoFormantList FormantList_create (double fromTime, double toTime, integer numberOfFormantObjects) {
	autoFormantList me = Thing_new (FormantList);
	Function_init (me.get(), fromTime, toTime);
	my formantIdentifier = autoSTRVEC (numberOfFormantObjects);
	my numberOfFormantObjects = numberOfFormantObjects;
	return me;
}

integer FormantList_identifyFormantIndexByCriterion (FormantList me, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	for (integer istr = 1; istr <= my formantIdentifier.size; istr ++)
		if (Melder_stringMatchesCriterion (my formantIdentifier [istr].get(), which, criterion, caseSensitive))
			return istr;
	return 0;
}

Formant FormantList_identifyFormantByCriterion (FormantList me, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	const integer index = FormantList_identifyFormantIndexByCriterion (me, which, criterion, caseSensitive);
	return ( index > 0 ? my formants.at [index] : nullptr );
}

static void Formant_replaceFrames (Formant target, Formant source, double fromTime, double toTime) {
	integer ifmin, ifmax, ifmin2, ifmax2;
	const integer numberOfFrames = Sampled_getWindowSamples (target, fromTime, toTime, & ifmin, & ifmax);
	const integer numberOfFrames2 = Sampled_getWindowSamples (source, fromTime, toTime, & ifmin2, & ifmax2);
	Melder_require (numberOfFrames == numberOfFrames2 && numberOfFrames > 0,
		U"The number of frames for the selected intervals should be equal.");
	for (integer iframe = ifmin ; iframe <= ifmax; iframe ++) {
		Formant_Frame targetFrame = & target -> frames [iframe];
		Formant_Frame sourceFrame = & source -> frames [iframe];
		sourceFrame -> copy (targetFrame);
	}
}

void Formant_and_FormantList_replaceFrames (Formant target, FormantList sourceList, double fromTime, double toTime, integer sourceIndex) {
	Melder_assert (sourceIndex > 0 && sourceIndex <= sourceList -> numberOfFormantObjects);
	Formant source = sourceList -> formants.at [sourceIndex];	
	Formant_replaceFrames (target, source, fromTime, toTime);
}

autoFormantList Sound_to_FormantList_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumFrequency, double maximumNumberOfFormants, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel) {
	try {
		
		const double nyquistFrequency = 0.5 / my dx;
		const integer numberOfCeilings = 2 * numberOfStepsToACeiling + 1;
		double minimumCeiling = maximumFrequency - numberOfStepsToACeiling * ceilingStep;
		Melder_require (minimumCeiling > 0,
			U"The minim ceiling should be positive. Decrease the 'ceiling step' or the 'number of steps' or both.");
		double maximumCeiling = maximumFrequency + numberOfStepsToACeiling * ceilingStep;		
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should be smaller than ", nyquistFrequency, U" Hz. "
			"Decrease the 'ceiling step' or the 'number of steps' or both.");		
		autoFormantList thee = FormantList_create (my xmin, my xmax, numberOfCeilings);
		thy defaultFormantObject = numberOfStepsToACeiling + 1;
		autoSound sources [1 + numberOfCeilings];
		const double formantSafetyMargin = 50.0;
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		for (integer ic  = 1; ic <= numberOfCeilings; ic ++) {
			autoLPC lpc;
			const double ceiling = minimumCeiling + (ic -1) * ceilingStep;
			autoSound resampled = Sound_resample (me, 2.0 * ceiling, 50);
			if (lpcType == kLPC_Analysis::BURG)
				lpc = Sound_to_LPC_burg (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::AUTOCORRELATION)
				lpc = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::COVARIANCE)
				lpc = Sound_to_LPC_covar (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::MARPLE)
				lpc = Sound_to_LPC_marple (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency, marple_tol1, marple_tol2);
			else if (lpcType == kLPC_Analysis::ROBUST) {
				autoLPC lpc_in = Sound_to_LPC_auto (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
				lpc = LPC_Sound_to_LPC_robust (lpc_in.get(), resampled.get(), windowLength, preemphasisFrequency, huber_numberOfStdDev, huber_maximumNumberOfIterations, huber_tol, true);
			}
			autoFormant formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy formantIdentifier [ic] =  Melder_dup (Melder_double (ceiling));
			thy formants . addItem_move (formant.move());
			if (sourcesMultiChannel) {
				autoSound source = LPC_Sound_filterInverse (lpc.get(), resampled.get ());
				sources [ic] = Sound_resample (source.get(), 2.0 * maximumFrequency, 50).move();
			}
		}
		Melder_assert (thy formants.size == thy numberOfFormantObjects); // maintain invariant
		if (sourcesMultiChannel) {
			Sound mid = sources [numberOfStepsToACeiling].get();
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
		Melder_throw (me, U": FormantList not created.");
	}
}

/* End of file FormantList.cpp */
