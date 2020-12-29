/* NoulliGrid.cpp
 *
 * Copyright (C) 2018,2020 Paul Boersma
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

#include "NoulliGrid.h"

#include "oo_DESTROY.h"
#include "NoulliGrid_def.h"
#include "oo_COPY.h"
#include "NoulliGrid_def.h"
#include "oo_EQUAL.h"
#include "NoulliGrid_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "NoulliGrid_def.h"
#include "oo_WRITE_TEXT.h"
#include "NoulliGrid_def.h"
#include "oo_READ_TEXT.h"
#include "NoulliGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "NoulliGrid_def.h"
#include "oo_READ_BINARY.h"
#include "NoulliGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "NoulliGrid_def.h"

Thing_implement (NoulliPoint, Function, 0);

Thing_implement (NoulliTier, Function, 0);

Thing_implement (NoulliGrid, Function, 0);

integer NoulliPoint_getWinningCategory (NoulliPoint me) {
	integer winningCategory = 0;
	double maximumProbability = 0.0;
	for (integer icat = 1; icat <= my numberOfCategories; icat ++) {
		if (my probabilities [icat] > maximumProbability) {   // NaN-safe comparison
			maximumProbability = my probabilities [icat];
			winningCategory = icat;
		}
	}
	return winningCategory;
}

autoNoulliPoint NoulliGrid_average (NoulliGrid me, integer tierNumber, double tmin, double tmax) {
	try {
		Melder_require (tierNumber >= 1, U"Tier number should be positive");
		Melder_require (tierNumber <= my tiers.size,
			U"Tier number (", tierNumber, U") should not be higher than the number of tiers (", my tiers.size, U").");
		NoulliTier tier = my tiers.at [tierNumber];
		autoNoulliPoint you = Thing_new (NoulliPoint);
		your numberOfCategories = my numberOfCategories;
		your probabilities = zero_VEC (my numberOfCategories);
		double numberOfSeconds = 0.0;
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			NoulliPoint inpoint = tier -> points.at [ipoint];
			if (inpoint -> xmax > tmin && inpoint -> xmin < tmax) {
				double duration = inpoint -> xmax - inpoint -> xmin;
				for (integer icat = 1; icat <= my numberOfCategories; icat ++)
					your probabilities [icat] += inpoint -> probabilities [icat] * duration;
				numberOfSeconds += duration;
			}
		}
		if (numberOfSeconds == 0.0) {
			for (integer icat = 1; icat <= my numberOfCategories; icat ++)
				your probabilities [icat] = undefined;
		} else {
			for (integer icat = 1; icat <= my numberOfCategories; icat ++)
				your probabilities [icat] /= numberOfSeconds;
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": average not computed.");
	}
}

autoVEC NoulliGrid_getAverageProbabilities (NoulliGrid me, integer tierNumber, double tmin, double tmax) {
	autoNoulliPoint point = NoulliGrid_average (me, tierNumber, tmin, tmax);
	return copy_VEC (point -> probabilities.get());
}

/* End of file NoulliGrid.cpp */

