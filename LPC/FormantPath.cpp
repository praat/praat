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
	MelderInfo_writeLine (U"  Identifiers:");
	for (integer iformant = 1; iformant <= formants . size; iformant ++)
		MelderInfo_writeLine (U"  ", iformant, U": ", formantIdentifiers [iformant].get(),
			( iformant == defaultFormant ? U" (default)" : U"" ));
	if (intervalTierNavigator) {
		our intervalTierNavigator -> v_info ();
	}
}

Thing_implement (FormantPath, Function, 0);

autoFormantPath FormantPath_create (double fromTime, double toTime, integer numberOfFormantObjects) {
	autoFormantPath me = Thing_new (FormantPath);
	Function_init (me.get(), fromTime, toTime);
	my formantIdentifiers = autoSTRVEC (numberOfFormantObjects);
	my numberOfFormants = numberOfFormantObjects;
	my path = TextGrid_create (fromTime, toTime, U"path/formant", U"");
	my intervalTierNavigator = IntervalTierNavigator_createFromTextGrid (my path.get(), 1);
	my pathTierNumber = 1;
	return me;
}

integer FormantPath_identifyPathTier (FormantPath me, TextGrid thee) {
	STRVEC validLabels = my formantIdentifiers.get();
	autoINTVEC minimumValidLabelLengths = newINTVECraw (validLabels.size);
	for (integer ilabel = 1; ilabel <= validLabels.size; ilabel ++)
		minimumValidLabelLengths [ilabel] = str32len (validLabels [ilabel]) + 1; // +1 for the extra ';' at end
	const integer minimumLabelLength = NUMmin (minimumValidLabelLengths.get());
	
	for (integer itier = 1; itier <= thy tiers -> size; itier ++) {
		const Function anyTier = thy tiers -> at [itier];
		if (anyTier -> classInfo != classIntervalTier)
			continue;
		const IntervalTier maybePathTier = reinterpret_cast<IntervalTier> (anyTier);
		if (Melder_stringMatchesCriterion (maybePathTier -> name.get(), kMelder_string::DOES_NOT_END_WITH, U"/formant", false))
			continue;
		/*
			We have a potential pathTier, check if all labeled intervals are consistent with our Formant id's.
		*/
		bool tierIsPathTier = true;
		for (integer interval = 1; interval <= maybePathTier -> intervals.size; interval ++) {
			const TextInterval textInterval = maybePathTier -> intervals.at [interval];
			conststring32 label = textInterval -> text.get();
			bool labelMatches = true; // empty interval is ok.
			if (label && label [0]) {
				integer validIndex = 0;
				const integer labelLength = str32len (label);
				if (labelLength >= minimumLabelLength) {
					for (integer index = 1; index <= validLabels.size; index ++) {
						const integer validLabelLength = minimumValidLabelLengths [index];
						if (labelLength >= validLabelLength && Melder_stringMatchesCriterion
							(label, kMelder_string::STARTS_WITH, validLabels [index], false) &&
							label [validLabelLength - 1] == U';') {
							validIndex = index;
							break;
						}
					}
				}
				labelMatches = validIndex != 0;
			}
			if (! labelMatches) {
				tierIsPathTier = false;
				break;
			}
		}
		if (tierIsPathTier)
			return itier;
	}
	return 0;
}

integer FormantPath_identifyFormantIndexByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	for (integer istr = 1; istr <= my formantIdentifiers.size; istr ++)
		if (Melder_stringMatchesCriterion (my formantIdentifiers [istr].get(), which, criterion, caseSensitive))
			return istr;
	return 0;
}

Formant FormantPath_identifyFormantByCriterion (FormantPath me, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	const integer index = FormantPath_identifyFormantIndexByCriterion (me, which, criterion, caseSensitive);
	return ( index > 0 ? my formants.at [index] : nullptr );
}

static void Formant_replaceFrames (Formant target, double fromTime, double toTime, Formant source) {
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

void FormantPath_replaceFrames (FormantPath me, double fromTime, double toTime, integer formantIndex) {
	Melder_assert (formantIndex > 0 && formantIndex <= my numberOfFormants);
	Formant_replaceFrames (my formant.get(), fromTime, toTime, my formants . at [formantIndex]);

}

void FormantPath_setNavigationLabels (FormantPath me, Strings navigationLabels, integer navigationTierNumber, kMelder_string criterion) {
	try {
		autoIntervalTierNavigator thee = IntervalTierNavigator_createFromTextGrid (my path.get(), navigationTierNumber);
		IntervalTierNavigator_setNavigationLabels (thee.get(), navigationLabels, criterion);
		integer numberOfMatches = IntervalTierNavigator_getNumberOfMatches (thee.get());
		Melder_require (numberOfMatches > 0,
			U"Not a single navigation label matches with the labels in the selected tier.");
		my intervalTierNavigator = thee.move();
		my navigationTierNumber = navigationTierNumber;
	} catch (MelderError) {
		Melder_throw (me, U": could not set the  navigation labels.");
	}
}

void FormantPath_setLeftContextNavigationLabels (FormantPath me, Strings navigationLabels, kMelder_string criterion) {
	Melder_require (my intervalTierNavigator && my intervalTierNavigator -> navigationLabels,
		U"There is no navigation posible. First use the \"Set navigation labels\" to make it happen.");
	IntervalTierNavigator_setLeftContextNavigationLabels (my intervalTierNavigator.get(), navigationLabels, criterion);
}

void FormantPath_setRightContextNavigationLabels (FormantPath me, Strings navigationLabels, kMelder_string criterion) {
	Melder_require (my intervalTierNavigator && my intervalTierNavigator -> navigationLabels,
			U"There is no navigation posible. First use the \"Set navigation labels\" to make it happen.");
	IntervalTierNavigator_setRightContextNavigationLabels (my intervalTierNavigator.get(), navigationLabels, criterion);
}

void FormantPath_setNavigationContextUse (FormantPath me,  kContextUse contextUse) {
	Melder_require (my intervalTierNavigator && my intervalTierNavigator -> navigationLabels,
		U"There is no navigation posible. First use the \"Set navigation labels\" to make it happen.");
	IntervalTierNavigator_setNavigationContextUse (my intervalTierNavigator.get(), contextUse);
}

integer FormantPath_getFormantIndexFromLabel (FormantPath me, conststring32 label) {
	/*
		Find part before ';'
	*/
	autoMelderString formantId;
	integer formantIndex = 0;
	MelderString_copy (& formantId, label);
	char32 *found = str32chr (formantId.string, U';');
	if (found)
		*found = U'\0';
	formantIndex = FormantPath_identifyFormantIndexByCriterion (me, kMelder_string::EQUAL_TO, formantId.string, true);
	return formantIndex;
}

void FormantPath_reconstructFormant (FormantPath me) {
	autoFormant thee = Data_copy (my formants .at [my defaultFormant]);
	IntervalTier pathTier = reinterpret_cast<IntervalTier> (my path -> tiers -> at [my pathTierNumber]);
	for (integer interval = 1; interval <= pathTier -> intervals.size; interval ++) {
		TextInterval textInterval = pathTier -> intervals . at [interval];
		conststring32 label = textInterval -> text.get();
		if (! label || ! label [0])
			continue;
		const integer formantIndex = FormantPath_getFormantIndexFromLabel (me, label);
		Melder_require (formantIndex > 0,
			U"Interval ", interval, U" has invalid data: \"", label, U"\".");
		Formant_replaceFrames (thee.get(), textInterval -> xmin, textInterval -> xmax, my formants.at [formantIndex]);
	}
	my formant = thee.move();
}

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double maximumFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel) {
	try {
		const double nyquistFrequency = 0.5 / my dx;
		const integer numberOfCeilings = 2 * numberOfStepsToACeiling + 1;
		double minimumCeiling = maximumFrequency - numberOfStepsToACeiling * ceilingStep;
		Melder_require (minimumCeiling > 0.0,
			U"The minimum ceiling should be positive. Decrease the 'ceiling step' or the 'number of steps' or both.");
		double maximumCeiling = maximumFrequency + numberOfStepsToACeiling * ceilingStep;		
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should be smaller than ", nyquistFrequency, U" Hz. "
			"Decrease the 'ceiling step' or the 'number of steps' or both.");		
		autoFormantPath thee = FormantPath_create (my xmin, my xmax, numberOfCeilings);
		thy sound = Data_copy (me);
		thy defaultFormant = numberOfStepsToACeiling + 1;
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
			thy formantIdentifiers [ic] =  Melder_dup (Melder_double (ceiling));
			thy formants . addItem_move (formant.move());
			if (sourcesMultiChannel) {
				autoSound source = LPC_Sound_filterInverse (lpc.get(), resampled.get ());
				sources [ic] = Sound_resample (source.get(), 2.0 * maximumFrequency, 50).move();
			}
		}
		Melder_assert (thy formants.size == thy numberOfFormants); // maintain invariant
		thy formant = Data_copy (thy formants. at [thy defaultFormant]);
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
		Melder_throw (me, U": FormantPath not created.");
	}
}

void FormantPath_mergeTextGrid (FormantPath me, TextGrid thee, integer navigationTierNumber) {
	/*
		Is there already a log/formant tier in the grid.
	*/
	autoTextGrid thycopy = Data_copy (thee);
	integer pathTierNumber = FormantPath_identifyPathTier (me, thycopy.get());
	if (pathTierNumber == 0) {
		TextGrid_addTier_copy (thycopy.get(), my path -> tiers -> at [my pathTierNumber]);
		pathTierNumber = thycopy -> tiers -> size;
	}
	my path = thycopy.move();
	my pathTierNumber = pathTierNumber;
	if (navigationTierNumber > 0)
		my intervalTierNavigator = IntervalTierNavigator_createFromTextGrid (thee, navigationTierNumber);
}

autoFormantPath Sound_and_TextGrid_to_FormantPath_any (Sound me, TextGrid thee, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants, double maximumFormantFrequency, double windowLength, double preemphasisFrequency, double ceilingStep, integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol, integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel) {
	/*
		The domain of the TextGid can differ a little from the domain of the SOund,
		e.g. TextGrids derived form Timit label files.
	*/
	double margin = 0.004;
	Melder_require (fabs (my xmin - thy xmin) < margin && fabs (my xmax - thy xmax) < margin,
		U"The domains of the Sound and the TextGrid should be equal within 0.003 s.");
	
	autoFormantPath him = Sound_to_FormantPath_any (me, lpcType, timeStep, maximumNumberOfFormants, maximumFormantFrequency, windowLength, preemphasisFrequency, ceilingStep, numberOfStepsToACeiling, marple_tol1, marple_tol2, huber_numberOfStdDev, huber_tol, huber_maximumNumberOfIterations, sourcesMultiChannel);
	/*
		Merge the TextGrid into path
	*/
	FormantPath_mergeTextGrid (him.get(), thee, 0); // no navigation.
	return him;
}

/* End of file FormantPath.cpp */
