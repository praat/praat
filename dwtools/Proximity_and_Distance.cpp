/* Proximity_and_Distance.cpp
 *
 * Copyright (C) 2018-2019 David Weenink
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

#include "Proximity_and_Distance.h"
#include "TableOfReal_extensions.h"

autoDistance Dissimilarity_to_Distance (Dissimilarity me, kMDS_AnalysisScale scale) {
	try {
		double additiveConstant = 0.0;

		autoDistance thee = Distance_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		if (scale == kMDS_AnalysisScale::ORDINAL) {
			additiveConstant = Dissimilarity_getAdditiveConstant (me);
			Melder_require (isdefined (additiveConstant),
				U"The additive constant could not be determined. Something is wrong with your Dissimilarity.");
		}
		for (integer i = 1; i <= my numberOfRows - 1; i ++) {
			for (integer j = i + 1; j <= my numberOfColumns; j ++) {
				const double d = 0.5 * (my data [i] [j] + my data [j] [i]) + additiveConstant;
				thy data [i] [j] = thy data [j] [i] = d;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Distance created.");
	}
}

autoDissimilarity Distance_to_Dissimilarity (Distance me) {
	try {
		autoDissimilarity thee = Dissimilarity_create (my numberOfRows);
		TableOfReal_copyLabels (me, thee.get(), 1, 1);
		Melder_assert (thy data.ncol == my numberOfColumns);
		thy data.all() <<= my data.all();
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Dissimilarity not created from Distance.");
	}
}


autoDistanceList DissimilarityList_to_DistanceList (DissimilarityList me, kMDS_AnalysisScale scale) {
	try {
		autoDistanceList thee = DistanceList_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoDistance him = Dissimilarity_to_Distance (my at [i], scale);
			conststring32 name = Thing_getName (my at [i]);
			Thing_setName (him.get(), name ? name : U"untitled");
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no DistanceList created.");
	}
}
