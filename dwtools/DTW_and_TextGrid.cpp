/* DTW_and_TextGrid.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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
 djmw 20060906
 djmw 20070306: Reverse x and y. Reference should always be vertical!
 djmw 20110304 Thing_new
*/

#include "DTW_and_TextGrid.h"

// begin old prototypes for compatibility reasons with the past
autoTextGrid DTW_TextGrid_to_TextGrid_old (DTW me, TextGrid thee);
autoIntervalTier DTW_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee);
autoTextTier DTW_TextTier_to_TextTier_old (DTW me, TextTier thee);
// end old

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
autoTextTier DTW_TextTier_to_TextTier (DTW me, TextTier thee, double precision) {
	try {
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			autoTextTier him = Data_copy (thee);
			his xmin = my xmin;
			his xmax = my xmax;
			for (integer i = 1; i <= his points.size; i ++) {
				const TextPoint textpoint = his points.at [i];
				const double time = DTW_getXTimeFromYTime (me, textpoint -> number);
				textpoint -> number = time;
			}
			return him;
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) { // map from X to Y
			autoTextTier him = Data_copy (thee);
			his xmin = my ymin;
			his xmax = my ymax;
			for (integer i = 1; i <= his points.size; i ++) {
				const TextPoint textpoint = his points.at [i];
				const double time = DTW_getYTimeFromXTime (me, textpoint -> number);
				textpoint -> number = time;
			}
			return him;
		} else {
			Melder_throw (U"The domain of the TextTier and one of the domains of the DTW should be equal.");
		}
	} catch (MelderError) {
		Melder_throw (U"TextTier not created from DTW & TextTier.");
	}
}

autoIntervalTier DTW_IntervalTier_to_IntervalTier (DTW me, IntervalTier thee, double precision) {
	try {
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			autoIntervalTier him = Data_copy (thee);
			his xmin = my xmin;
			his xmax = my xmax;
			TextInterval textinterval = his intervals.at [1];
			textinterval -> xmin = his xmin;
			double xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
			for (integer i = 2; i <= his intervals.size; i ++) {
				textinterval = his intervals.at [i];
				textinterval -> xmin = xmax;
				xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
				textinterval -> xmax = xmax;
			}
			textinterval = his intervals.at [his intervals.size];
			textinterval -> xmax = his xmax;
			Melder_assert (textinterval -> xmin < textinterval -> xmax);
			return him;
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) { // map from X to Y
			autoIntervalTier him = Data_copy (thee);
			his xmin = my ymin;
			his xmax = my ymax;
			TextInterval textinterval = his intervals.at [1];
			textinterval -> xmin = his xmin;
			double xmax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
			for (integer i = 2; i <= his intervals.size; i ++) {
				textinterval = his intervals.at [i];
				textinterval -> xmin = xmax;
				xmax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
				textinterval -> xmax = xmax;
			}
			textinterval = his intervals.at [his intervals.size];
			textinterval -> xmax = his xmax;
			Melder_assert (textinterval -> xmin < textinterval -> xmax);
			return him;
		} else {
			Melder_throw (U"The domain of the IntervalTier and one of the domains of the DTW should be equal.");
		}
	} catch (MelderError) {
		Melder_throw (U"IntervalTier not created from DTW & IntervalTier.");
	}
}

autoTextGrid DTW_TextGrid_to_TextGrid (DTW me, TextGrid thee, double precision) {
	try {
		double tmin, tmax;
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) {
			tmin = my xmin;
			tmax = my xmax;
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) {
			tmin = my ymin;
			tmax = my ymax;
		} else {
			Melder_throw (U"The domain of the TextGrid should be equal to one of the domains of the DTW.");
		}

		autoTextGrid him = TextGrid_createWithoutTiers (tmin, tmax);

		for (integer i = 1; i <= thy tiers->size; i ++) {
			const Function anyTier = thy tiers->at [i];

			if (anyTier -> classInfo == classIntervalTier) {
				autoIntervalTier tier = DTW_IntervalTier_to_IntervalTier (me, (IntervalTier) anyTier, precision);
				TextGrid_addTier_copy (him.get(), tier.get());
			} else if (anyTier -> classInfo == classTextTier) {
				autoTextTier tier = DTW_TextTier_to_TextTier (me, (TextTier) anyTier, precision);
				TextGrid_addTier_copy (him.get(), tier.get());
			} else {
				Melder_throw (U"Unknown tier.");
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created from DTW & TextGrid.");
	}
}

autoTable DTW_IntervalTier_to_Table (DTW me, IntervalTier thee, double precision) {
	try {
		const integer numberOfIntervals = thy intervals.size;
		autoTable him = Table_createWithColumnNames (numberOfIntervals, U"tmin tmax label dist");
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			integer pathIndex = 1;
			for (integer i = 1; i <= numberOfIntervals; i ++) {
				const TextInterval textinterval = thy intervals.at [i];
				const double xmin = DTW_getXTimeFromYTime (me, textinterval -> xmin);
				const double xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
				integer ixmin, ixmax;
				const integer numberOfFrames = Matrix_getWindowSamplesX (me, xmin, xmax, & ixmin, & ixmax);
				double sumOfDistances = 0.0;
				while (pathIndex < my pathLength && my path [pathIndex]. x < ixmax) {
					sumOfDistances += my z [my path [pathIndex]. y] [my path [pathIndex]. x];
					pathIndex ++;
				}
				Table_setNumericValue (him.get(), i, 1, textinterval -> xmin);
				Table_setNumericValue (him.get(), i, 2, textinterval -> xmax);
				Table_setStringValue  (him.get(), i, 3, textinterval -> text.get());
				Table_setNumericValue (him.get(), i, 4, sumOfDistances / numberOfFrames);
			}
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) {  // map from X to Y
			integer pathIndex = 1;
			for (integer i = 1; i <= numberOfIntervals; i ++) {
				const TextInterval textinterval = thy intervals.at [i];
				const double ymin = DTW_getYTimeFromXTime (me, textinterval -> xmin);
				const double ymax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
				integer iymin, iymax;
				const integer numberOfFrames = Matrix_getWindowSamplesY (me, ymin, ymax, & iymin, & iymax);
				double sumOfDistances = 0;
				while (pathIndex < my pathLength && my path [pathIndex]. y < iymax) {
					sumOfDistances += my z [my path [pathIndex]. y] [my path [pathIndex]. x];
					pathIndex ++;
				}
				Table_setNumericValue (him.get(), i, 1, textinterval -> xmin);
				Table_setNumericValue (him.get(), i, 2, textinterval -> xmax);
				Table_setStringValue  (him.get(), i, 3, textinterval -> text.get());
				Table_setNumericValue (him.get(), i, 4, sumOfDistances / numberOfFrames);
			}
		} else {
			Melder_throw (U"The domain of the IntervalTier and one of the domains of the DTW should be equal.");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Table with distances created.");
	}
}

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
autoTextTier DTW_TextTier_to_TextTier_old (DTW me, TextTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The domain of the TextTier and the DTW should be equal.");
		autoTextTier him =  Data_copy (thee);
		his xmin = my ymin;
		his xmax = my ymax;
		for (integer i = 1; i <= his points.size; i ++) {
			const TextPoint textpoint = his points.at [i];
			const double time = DTW_getYTimeFromXTime (me, textpoint -> number);
			textpoint -> number = time;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TextTier not created.");
	}
}

autoIntervalTier DTW_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The domain of the IntervalTier and the DTW should be equal.");
		
		autoIntervalTier him = Data_copy (thee);

		his xmin = my ymin;
		his xmax = my ymax;

		for (integer i = 1; i <= his intervals.size; i ++) {
			const TextInterval textinterval = his intervals.at [i];
			const double xmin = DTW_getYTimeFromXTime (me, textinterval -> xmin);
			textinterval -> xmin = xmin;
			const double xmax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"IntervalTier not created.");
	}
}

autoTextGrid DTW_TextGrid_to_TextGrid_old (DTW me, TextGrid thee) {
	try {
		autoTextGrid him = Thing_new (TextGrid);
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The domain of the TextGrid and the y-domain of the DTW should be equal.");

		his xmin = my ymin;
		his xmax = my ymax;
		his tiers = FunctionList_create ();

		for (integer i = 1; i <= thy tiers->size; i ++) {
			const Daata anyTier = thy tiers->at [i];
			if (anyTier -> classInfo == classIntervalTier) {
				autoIntervalTier tier = DTW_IntervalTier_to_IntervalTier_old (me, (IntervalTier) anyTier);
				TextGrid_addTier_copy (him.get(), tier.get());
			} else if (anyTier -> classInfo == classTextTier) {
				autoTextTier tier = DTW_TextTier_to_TextTier_old (me, (TextTier) anyTier);
				TextGrid_addTier_copy (him.get(), tier.get());
			} else {
				Melder_throw (U"Unknown tier.");
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"TextGrid not created.");
	}
}

/* End of file DTW_and_TextGrid.cpp */
