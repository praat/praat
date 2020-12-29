/* MDSVec.cpp
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

#include "NUM2.h"
#include "MDSVec.h"

#include "oo_DESTROY.h"
#include "MDSVec_def.h"
#include "oo_COPY.h"
#include "MDSVec_def.h"
#include "oo_EQUAL.h"
#include "MDSVec_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "MDSVec_def.h"
#include "oo_WRITE_TEXT.h"
#include "MDSVec_def.h"
#include "oo_WRITE_BINARY.h"
#include "MDSVec_def.h"
#include "oo_READ_TEXT.h"
#include "MDSVec_def.h"
#include "oo_READ_BINARY.h"
#include "MDSVec_def.h"
#include "oo_DESCRIPTION.h"
#include "MDSVec_def.h"

Thing_implement (MDSVec, Daata, 0);

void structMDSVec :: v_info () noexcept {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of points: ", numberOfPoints);
	MelderInfo_writeLine (U"Number of proximities: ", numberOfProximities);
}

autoMDSVec MDSVec_create (integer numberOfPoints) {
	try {
		autoMDSVec me = Thing_new (MDSVec);
		my numberOfPoints = numberOfPoints;
		my numberOfProximities = numberOfPoints * (numberOfPoints - 1) / 2;
		my proximity = zero_VEC (my numberOfProximities);
		my rowIndex = zero_INTVEC (my numberOfProximities);
		my columnIndex = zero_INTVEC (my numberOfProximities);
		return me;
	} catch (MelderError) {
		Melder_throw (U"MDSVec not created.");
	}
}

autoMDSVec Dissimilarity_to_MDSVec (Dissimilarity me) {
	try {
		autoMDSVec thee = MDSVec_create (my numberOfRows);

		integer n = 0;
		for (integer irow = 1; irow <= my numberOfRows - 1; irow ++) {
			for (integer icol = irow + 1; icol <= my numberOfColumns; icol ++) {
				const double f = 0.5 * (my data [irow] [icol] + my data [icol] [irow]);
				if (f > 0.0) {
					n ++;
					thy proximity [n] = f;
					thy rowIndex [n] = irow;
					thy columnIndex [n] = icol;
				}
			}
		}
		thy numberOfProximities = n;
		VECsort3_inplace (thy proximity.get(), thy rowIndex.get(), thy columnIndex.get(), false);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MDSVec created.");
	}
}

/*********************** MDSVECS *******************************************/

Thing_implement (MDSVecList, Ordered, 0);

autoMDSVecList DissimilarityList_to_MDSVecList (DissimilarityList me) {
	try {
		autoMDSVecList thee = MDSVecList_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoMDSVec him = Dissimilarity_to_MDSVec (my at [i]);
			Thing_setName (him.get(), Thing_getName (my at [i]));
			thy addItem_move (him.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MDSVecs created.");
	}
}

/* End of file MDSVec.cpp */

