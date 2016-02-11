/* Discriminant_Pattern_Categories.cpp
 *
 * Copyright (C) 2004-2011, 2015 David Weenink
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
 djmw 20040422 Initial version
*/

#include "Discriminant_Pattern_Categories.h"
#include "TableOfReal.h"
#include "Matrix_Categories.h"

autoDiscriminant Pattern_and_Categories_to_Discriminant (Pattern me, Categories thee) {
	try {
		autoTableOfReal t = Matrix_and_Categories_to_TableOfReal (me, thee);
		autoDiscriminant him = TableOfReal_to_Discriminant (t.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Discriminant not created from Pattern & Categories.");
	}
}

autoCategories Discriminant_and_Pattern_to_Categories (Discriminant me, Pattern thee, int poolCovarianceMatrices, int useAprioriProbabilities) {
	try {
		autoTableOfReal t = Matrix_to_TableOfReal (thee);
		autoClassificationTable ct = Discriminant_and_TableOfReal_to_ClassificationTable (me, t.get(), poolCovarianceMatrices, useAprioriProbabilities);
		autoCategories him =  ClassificationTable_to_Categories_maximumProbability (ct.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Categories not created from Pattern & Discriminant.");
	}
}

/* End of file Discriminant_Pattern_Categories.cpp */
