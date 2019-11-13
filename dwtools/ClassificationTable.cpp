/* ClassificationTable.cpp
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
 djmw 1998
 djmw 20020315 GPL header
 djmw 20040422 Added ClassificationTable_to_Categories_maximumProbability
 djmw 20040623 Added ClassificationTable_to_Strings_maximumProbability
 djmw 20040824 Added Strings_extensions.h header
 djmw 20101122 ClassificationTable_to_Correlation_columns
 djmw 20110304 Thing_new
*/

#include "ClassificationTable.h"
#include "Distributions_and_Strings.h"
#include "Strings_extensions.h"
#include "NUM2.h"

Thing_implement (ClassificationTable, TableOfReal, 0);

autoClassificationTable ClassificationTable_create (integer numberOfRows, integer numberOfClasses) {
	try {
		autoClassificationTable me = Thing_new (ClassificationTable);
		TableOfReal_init (me.get(), numberOfRows, numberOfClasses);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ClassificationTable not created.");
	}
}

autoConfusion ClassificationTable_to_Confusion (ClassificationTable me, bool onlyClassLabels) {
	try {
		autoStrings responses = TableOfReal_extractColumnLabelsAsStrings (me);
		autoStrings s2 = TableOfReal_extractRowLabelsAsStrings (me);
		autoDistributions d2 = Strings_to_Distributions (s2.get());
		autoStrings stimuli = TableOfReal_extractRowLabelsAsStrings (d2.get());
		autoConfusion thee = Confusion_createFromStringses (( onlyClassLabels ? responses.get() : stimuli.get() ), responses.get());
		Confusion_ClassificationTable_increase (thee.get(), me);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": confusions cannot be calculated.");
	}
}

void Confusion_ClassificationTable_increase (Confusion me, ClassificationTable thee) {
	Melder_require (my numberOfColumns == thy numberOfColumns, 
		U"The number of columns should be equal.");
	for (integer irow = 1; irow <= thy numberOfRows; irow ++) {
		const integer index = TableOfReal_getColumnIndexAtMaximumInRow (thee, irow);
		Confusion_increase (me, thy rowLabels [irow].get(), my columnLabels [index].get());
	}
}

autoStrings ClassificationTable_to_Strings_maximumProbability (ClassificationTable me) {
	try {
		autoStrings thee = Strings_createFixedLength (my numberOfRows);
		Melder_assert (my numberOfColumns > 0);
		for (integer i = 1; i <= my numberOfRows; i ++) {
			const integer col = NUMmaxPos (my data.row (i));
			if (my columnLabels [col])
				Strings_replace (thee.get(), i, my columnLabels [col].get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": strings cannot be created.");
	}
}

autoCategories ClassificationTable_to_Categories_maximumProbability (ClassificationTable me) {
	try {
		autoCategories thee = Categories_create ();
		Melder_assert (my numberOfColumns > 0);
		for (integer i = 1; i <= my numberOfRows; i ++) {
			const integer col = NUMmaxPos (my data.row (i));
			OrderedOfString_append (thee.get(), my columnLabels [col].get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Categories created.");
	}
}

autoCorrelation ClassificationTable_to_Correlation_columns (ClassificationTable me) {
	try {
		autoCorrelation thee = Correlation_create (my numberOfColumns);
		for (integer icol = 1; icol <= thy numberOfColumns; icol ++) {
			const conststring32 label = my columnLabels [icol].get();
			TableOfReal_setRowLabel (thee.get(), icol, label);
			TableOfReal_setColumnLabel (thee.get(), icol, label);
		}
		for (integer irow = 1; irow <= thy numberOfColumns; irow ++) {
			thy data [irow] [irow] = 1.0;
			for (integer icol = irow + 1; icol <= thy numberOfColumns; icol ++) {
				const double n12 = NUMinner (my data.column (irow), my data.column (icol));
				const double n11 = NUMinner (my data.column (irow), my data.column (irow));
				const double n22 = NUMinner (my data.column (icol), my data.column (icol));
				// probabilities might be very low!
				if (n12 > 0.0 && n22 > 0.0) {
					thy data [irow] [icol] = thy data [icol] [irow] = n12 / sqrt (n11 * n22);
				}
			}
		}
		thy numberOfObservations = my numberOfRows;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no correlation created.");
	}
}

/* End of file ClassificationTable.cpp */
