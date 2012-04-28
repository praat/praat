/* DTW_and_TextGrid.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20060906
 djmw 20070306: Reverse x and y. Reference should always be vertical!
 djmw 20110304 Thing_new
*/

#include "DTW_and_TextGrid.h"

// begin old prototypes for compatibility reasons with the past
TextGrid DTW_and_TextGrid_to_TextGrid_old (DTW me, TextGrid thee);
IntervalTier DTW_and_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee);
TextTier DTW_and_TextTier_to_TextTier_old (DTW me, TextTier thee);
// end old

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
TextTier DTW_and_TextTier_to_TextTier (DTW me, TextTier thee, double precision) {
	try {
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			autoTextTier him = Data_copy (thee);
			his xmin = my xmin;
			his xmax = my xmax;
			for (long i = 1; i <= his points -> size; i++) {
				TextPoint textpoint = (TextPoint) his points -> item[i];
				double time = DTW_getXTimeFromYTime (me, textpoint -> number);
				textpoint -> number = time;
			}
			return him.transfer();
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) { // map from X to Y
			autoTextTier him = Data_copy (thee);
			his xmin = my ymin;
			his xmax = my ymax;
			for (long i = 1; i <= his points -> size; i++) {
				TextPoint textpoint = (TextPoint) his points -> item[i];
				double time = DTW_getYTimeFromXTime (me, textpoint -> number);
				textpoint -> number = time;
			}
			return him.transfer();
		} else {
			Melder_throw ("The domain of the TextTier and one of the domains of the DTW must be equal.");
		}
	} catch (MelderError) {
		Melder_throw ("TextTier not created from DTW & TextTier.");
	}
}

IntervalTier DTW_and_IntervalTier_to_IntervalTier (DTW me, IntervalTier thee, double precision) {
	try {
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			autoIntervalTier him = Data_copy (thee);
			his xmin = my xmin;
			his xmax = my xmax;
			for (long i = 1; i <= his intervals -> size; i++) {
				TextInterval textinterval = (TextInterval) his intervals -> item[i];
				double xmin = DTW_getXTimeFromYTime (me, textinterval -> xmin);
				textinterval -> xmin = xmin;
				double xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
				textinterval -> xmax = xmax;
			}
			return him.transfer();
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) { // map from X to Y
			autoIntervalTier him = Data_copy (thee);
			his xmin = my ymin;
			his xmax = my ymax;
			for (long i = 1; i <= his intervals -> size; i++) {
				TextInterval textinterval = (TextInterval) his intervals -> item[i];
				double xmin = DTW_getYTimeFromXTime (me, textinterval -> xmin);
				textinterval -> xmin = xmin;
				double xmax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
				textinterval -> xmax = xmax;
			}
			return him.transfer();
		} else {
			Melder_throw ("The domain of the IntervalTier and one of the domains of the DTW must be equal.");
		}
	} catch (MelderError) {
		Melder_throw ("IntervalTier not created from DTW & IntervalTier.");
	}
}

TextGrid DTW_and_TextGrid_to_TextGrid (DTW me, TextGrid thee, double precision) {
	try {
		double tmin, tmax;
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) {
			tmin = my xmin;
			tmax = my xmax;
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) {
			tmin = my ymin;
			tmax = my ymax;
		} else {
			Melder_throw ("The domain of the TextGrid must be equal to one of the domains of the DTW.");
		}

		autoTextGrid him = TextGrid_createWithoutTiers (tmin, tmax);

		for (long i = 1; i <= thy tiers -> size; i++) {
			Function anyTier = (Function) thy tiers -> item[i];

			if (anyTier -> classInfo == classIntervalTier) {
				autoIntervalTier tier = DTW_and_IntervalTier_to_IntervalTier (me, (IntervalTier) anyTier, precision);
				TextGrid_addTier (him.peek(), tier.peek());
			} else if (anyTier -> classInfo == classTextTier) {
				autoTextTier tier = DTW_and_TextTier_to_TextTier (me, (TextTier) anyTier, precision);
				TextGrid_addTier (him.peek(), tier.peek());
			} else {
				Melder_throw ("Unknown tier.");
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not created from DTW & TextGrid.");
	}
}

Table DTW_and_IntervalTier_to_Table (DTW me, IntervalTier thee, double precision) {
	try {
		long numberOfIntervals = thy intervals -> size;
		autoTable him = Table_createWithColumnNames (numberOfIntervals, L"tmin tmax label dist");
		if (fabs (my ymin - thy xmin) <= precision && fabs (my ymax - thy xmax) <= precision) { // map from Y to X
			long pathIndex = 1;
			for (long i = 1; i <= numberOfIntervals; i++) {
				TextInterval textinterval = (TextInterval) thy intervals -> item[i];
				double xmin = DTW_getXTimeFromYTime (me, textinterval -> xmin);
				double xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
				long ixmin, ixmax;
				long numberOfFrames = Matrix_getWindowSamplesX (me, xmin, xmax, &ixmin, &ixmax);
				double sumOfDistances = 0;
				while (pathIndex < my pathLength && my path[pathIndex].x < ixmax) {
					sumOfDistances += my z[my path[pathIndex].y][my path[pathIndex].x];
					pathIndex++;
				}
				Table_setNumericValue (him.peek(), i, 1, textinterval -> xmin);
				Table_setNumericValue (him.peek(), i, 2, textinterval -> xmax);
				Table_setStringValue  (him.peek(), i, 3, textinterval -> text);
				Table_setNumericValue (him.peek(), i, 4, sumOfDistances / numberOfFrames);
			}
		} else if (fabs (my xmin - thy xmin) <= precision && fabs (my xmax - thy xmax) <= precision) {  // map from X to Y
			long pathIndex = 1;
			for (long i = 1; i <= numberOfIntervals; i++) {
				TextInterval textinterval = (TextInterval) thy intervals -> item[i];
				double ymin = DTW_getYTimeFromXTime (me, textinterval -> xmin);
				double ymax = DTW_getYTimeFromXTime (me, textinterval -> xmax);
				long iymin, iymax;
				long numberOfFrames = Matrix_getWindowSamplesY (me, ymin, ymax, &iymin, &iymax);
				double sumOfDistances = 0;
				while (pathIndex < my pathLength && my path[pathIndex].y < iymax) {
					sumOfDistances += my z[my path[pathIndex].y][my path[pathIndex].x];
					pathIndex++;
				}
				Table_setNumericValue (him.peek(), i, 1, textinterval -> xmin);
				Table_setNumericValue (him.peek(), i, 2, textinterval -> xmax);
				Table_setStringValue  (him.peek(), i, 3, textinterval -> text);
				Table_setNumericValue (him.peek(), i, 4, sumOfDistances / numberOfFrames);
			}
		} else {
			Melder_throw ("The domain of the IntervalTier and one of the domains of the DTW must be equal.");
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Table with distances created.");
	}


}

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
TextTier DTW_and_TextTier_to_TextTier_old (DTW me, TextTier thee) {
	try {
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("The domain of the TextTier and the DTW must be equal.");
		}
		autoTextTier him =  Data_copy (thee);
		his xmin = my ymin;
		his xmax = my ymax;

		for (long i = 1; i <= his points -> size; i++) {
			TextPoint textpoint = (TextPoint) his points -> item[i];
			double time = DTW_getPathY (me, textpoint -> number);
			textpoint -> number = time;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TextTier not created.");
	}
}

IntervalTier DTW_and_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee) {
	try {
		if ( (my xmin != thy xmin) || my xmax != thy xmax) Melder_throw
			("The domain of the IntervalTier and the DTW must be equal.");

		autoIntervalTier him = Data_copy (thee);

		his xmin = my ymin;
		his xmax = my ymax;

		for (long i = 1; i <= his intervals -> size; i++) {
			TextInterval textinterval = (TextInterval) his intervals -> item[i];
			double xmin = DTW_getPathY (me, textinterval -> xmin);
			textinterval -> xmin = xmin;
			double xmax = DTW_getPathY (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("IntervalTier not created.");
	}
}

TextGrid DTW_and_TextGrid_to_TextGrid_old (DTW me, TextGrid thee) {
	try {
		autoTextGrid him = Thing_new (TextGrid);
		if (my xmin != thy xmin || my xmax != thy xmax) {
			Melder_throw ("The domain of the TextGrid and the y-domain of the DTW must be equal.");
		}

		his xmin = my ymin;
		his xmax = my ymax;
		his tiers = Ordered_create ();

		for (long i = 1; i <= thy tiers -> size; i++) {
			Data anyTier = (Data) thy tiers -> item[i];

			if (anyTier -> classInfo == classIntervalTier) {

				autoIntervalTier tier = DTW_and_IntervalTier_to_IntervalTier_old (me, (IntervalTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek());
			} else if (anyTier -> classInfo == classTextTier) {
				autoTextTier tier = DTW_and_TextTier_to_TextTier_old (me, (TextTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek());
			} else {
				Melder_throw (L"Unknown tier.");
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("TextGrid not created.");
	}
}

/* End of file DTW_and_TextGrid.cpp */
