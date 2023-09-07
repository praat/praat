/* NoulliGrid.cpp
 *
 * Copyright (C) 2018,2020,2021,2023 Paul Boersma
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
#include "Graphics.h"

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
				const double duration = inpoint -> xmax - inpoint -> xmin;
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

void NoulliGrid_paintInside (NoulliGrid me, Graphics graphics, double tmin, double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setColour (graphics, Melder_WHITE);
	Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (graphics, 0.0, 1.0, 0.0, 1.0);
	if (my numberOfCategories == 2) {
		Graphics_setWindow (graphics, tmin, tmax, 0.0, 1.0);
		Graphics_setLineWidth (graphics, 3.0);
		for (integer itier = 1; itier <= my tiers.size; itier ++) {
			NoulliTier tier = my tiers.at [itier];
			for (integer ipoint = 1; ipoint < tier -> points.size; ipoint ++) {
				NoulliPoint point = tier -> points.at [ipoint], nextPoint = tier -> points.at [ipoint + 1];
				const double time = 0.5 * (point -> xmin + point -> xmax);
				const double nextTime = 0.5 * (nextPoint -> xmin + nextPoint -> xmax);
				if (time > tmin && nextTime < tmax) {
					const double prob = point -> probabilities [1], nextProb = nextPoint -> probabilities [1];
					Graphics_setColour (graphics, Melder_cyclingBackgroundColour (itier));
					Graphics_line (graphics, time, prob, nextTime, nextProb);
				}
			}
			Graphics_setColour (graphics, Melder_BLACK);
		}
	} else {
		Graphics_setWindow (graphics, tmin, tmax, 0.0, my tiers.size);
		for (integer itier = 1; itier <= my tiers.size; itier ++) {
			NoulliTier tier = my tiers.at [itier];
			const double ymin = my tiers.size - itier, ymax = ymin + 1.0;
			for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
				NoulliPoint point = tier -> points.at [ipoint];
				if (point -> xmax > tmin && point -> xmin < tmax) {
					const double xmin = Melder_clippedLeft (tmin, point -> xmin);
					const double xmax = Melder_clippedRight (point -> xmax, tmax);
					double prob1 = 1.0;
					for (integer icategory = 1; icategory <= point -> numberOfCategories; icategory ++) {
						const double prob2 = prob1;
						prob1 -= point -> probabilities [icategory];
						Graphics_setColour (graphics, Melder_cyclingBackgroundColour (icategory));
						Graphics_fillRectangle (graphics, xmin, xmax,
								ymin + prob1 * (ymax - ymin), ymin + prob2 * (ymax - ymin));
					}
				}
			}
			Graphics_setColour (graphics, Melder_BLACK);
			if (itier > 1) {
				Graphics_setLineWidth (graphics, 1.0);
				Graphics_line (graphics, tmin, ymax, tmax, ymax);
			}
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	Graphics_setColour (graphics, Melder_BLACK);
}

void NoulliGrid_paint (NoulliGrid me, Graphics g, double tmin, double tmax, bool garnish) {
	Graphics_setInner (g);
	NoulliGrid_paintInside (me, g, tmin, tmax);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

/* End of file NoulliGrid.cpp */

