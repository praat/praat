/* Discriminant_PatternList_Categories.cpp
 *
 * Copyright (C) 2004-2019 David Weenink
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
 djmw 20040422 Initial version
*/

#include "Discriminant_PatternList_Categories.h"
#include "TableOfReal_and_Discriminant.h"
#include "Matrix_Categories.h"

autoDiscriminant PatternList_Categories_to_Discriminant (PatternList me, Categories thee) {
	try {
		autoTableOfReal t = Matrix_Categories_to_TableOfReal (me, thee);
		autoDiscriminant him = TableOfReal_to_Discriminant (t.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Discriminant not created from PatternList & Categories.");
	}
}

autoCategories Discriminant_PatternList_to_Categories (Discriminant me, PatternList thee, int poolCovarianceMatrices, int useAprioriProbabilities) {
	try {
		autoTableOfReal t = Matrix_to_TableOfReal (thee);
		autoClassificationTable ct = Discriminant_TableOfReal_to_ClassificationTable (me, t.get(), poolCovarianceMatrices, useAprioriProbabilities);
		autoCategories him =  ClassificationTable_to_Categories_maximumProbability (ct.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Categories not created from PatternList & Discriminant.");
	}
}

/* End of file Discriminant_PatternList_Categories.cpp */
