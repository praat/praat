/* FormantList_and_TextGrid.cpp
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

#include "FormantList_and_TextGrid.h"
#include "melder_search.h"

autoFormant FormantList_and_TextGrid_to_Formant (FormantList me, TextGrid thee, integer logTierNumber) {
	try {
		Melder_require (my xmin == thy xmin && fabs (my xmax - thy xmax) < 0.004, // Ad hoc: based on TIMIT
			U"The time domain of the FormantList and the TextGrid should be equal.");
		TextGrid_checkSpecifiedTierIsIntervalTier(thee, logTierNumber);
		IntervalTier logTier = reinterpret_cast<IntervalTier> (thy tiers -> at [logTierNumber]);
		Melder_require (Melder_stringMatchesCriterion (logTier -> name.get(), kMelder_string::ENDS_WITH, U"-log", true),
			U"The tier name should end with \"-log\".");
		autoFormant him = Data_copy (my formants .at [my defaultFormantObject]);
		autoMelderString formantId;
		for (integer iinterval = 1; iinterval <= logTier -> intervals.size; iinterval ++) {
			const TextInterval interval = logTier -> intervals.at [iinterval];
			integer formantIndex = my defaultFormantObject;
			if (interval -> name && interval -> name [0]) {
				MelderString_copy (& formantId, interval -> text.get());
				char32 *found = str32chr (formantId.string, U';');
				formantIndex = 0;
				if (found) {
					*found = U'\0';
					formantIndex = FormantList_identifyFormantIndexByCriterion (me, kMelder_string::EQUAL_TO, formantId.string, true);
				}
			}
			Melder_require (formantIndex > 0,
				U"The formant identification \", formantId.string, U\" in interval ", interval, U" cannot be found in the FormantList. Probably the tier in the TextGrid is not related to this FormantList.");
			if (formantIndex != my defaultFormantObject)
				Formant_and_FormantList_replaceFrames (him.get(), me, interval -> xmin, interval->xmax, formantIndex);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Formant not created.");
	}
}

/*
	A log tier starts with '<id>;'
*/
integer TextGrid_and_FormantList_findLogTier (TextGrid me, FormantList thee) {
	STRVEC validLabels = thy formantIdentifier.get();
	autoINTVEC minimumValidLabelLengths = newINTVECraw (validLabels.size);
	for (integer ilabel = 1; ilabel <= validLabels.size; ilabel ++)
		minimumValidLabelLengths [ilabel] = str32len (validLabels [ilabel]) + 1; // +1 for the ';'
	const integer minimumLabelLength = NUMmin (minimumValidLabelLengths.get());
	
	for (integer itier = 1; itier <= my tiers->size; itier ++) {
		const Function tier = my tiers->at [itier];
		if (tier -> classInfo != classIntervalTier)
			continue;
		bool tierIsLogTier = true;
		const IntervalTier intervalTier = reinterpret_cast<IntervalTier> (tier);
		for (integer interval = 1; interval <= intervalTier -> intervals.size; interval ++) {
			const TextInterval textInterval = intervalTier -> intervals.at [interval];
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
				tierIsLogTier = false;
				break;
			}
		}
		if (tierIsLogTier)
				return itier;
	}
	return 0;
}

/* End of file FormantList_and_TextGrid.cpp */
