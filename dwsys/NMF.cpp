/* NMF.cpp
 *
 * Copyright (C) 2019 David Weenink
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

#include "NMF.h"

void structNMF :: v_info () {
	MelderInfo_writeLine (U"Number of rows: ", numberOfRows);
	MelderInfo_writeLine (U"Number of columns: ", numberOfColumns);
	MelderInfo_writeLine (U"Dimension of approximation", dimensionOfApproximation);
}

Thing_implement (NMF, Daata, 0);


autoNMF NMF_create (integer numberOfRows, integer numberOfColumns, integer dimensionOfApproximation) {
	try {
		autoNMF me = Thing_new (NMF);
		my numberOfRows = numberOfRows;
		my numberOfColumns = numberOfColumns;
		my dimensionOfApproximation = dimensionOfApproximation;
		my w = newMATzero (numberOfRows,  dimensionOfApproximation);
		my h = newMATzero (dimensionOfApproximation, numberOfColumns);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NMF not created.");
	}
}

autoNMF NMF_createFromGeneralMatrix (constMAT m, integer dimensionOfApproximation) {
	
}


#endif /* _NMF_h_ */
