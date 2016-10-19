/* praat_Matrix.cpp
 *
 * Copyright (C) 1992-2012,2013,2014,2015,2016 Paul Boersma
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

#include "praat_Matrix.h"

#undef iam
#define iam iam_LOOP

int praat_Matrix_formula (UiForm dia, Interpreter interpreter) {
	int IOBJECT;
	LOOP {
		iam (Matrix);
		try {
			Matrix_formula (me, GET_STRING (U"formula"), interpreter, nullptr);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
	return 1;
}

/* End of file praat_Matrix.cpp */
