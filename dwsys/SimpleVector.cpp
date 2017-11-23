/* SimpleVector.cpp
 *
 * Copyright (C) 1994-2012, 2015-2017 David Weenink
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
 djmw 20020812 GPL header
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
*/

#include "SimpleVector.h"

#include "oo_DESTROY.h"
#include "SimpleVector_def.h"
#include "oo_COPY.h"
#include "SimpleVector_def.h"
#include "oo_EQUAL.h"
#include "SimpleVector_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SimpleVector_def.h"
#include "oo_WRITE_TEXT.h"
#include "SimpleVector_def.h"
#include "oo_WRITE_BINARY.h"
#include "SimpleVector_def.h"
#include "oo_READ_TEXT.h"
#include "SimpleVector_def.h"
#include "oo_READ_BINARY.h"
#include "SimpleVector_def.h"
#include "oo_DESCRIPTION.h"
#include "SimpleVector_def.h"

Thing_implement (DoubleVector, Daata, 0);

void DoubleVector_init (DoubleVector me, integer min, integer max) {
	my min = min; my max = max;
	my v = NUMvector<double> (min, max);
}

autoDoubleVector DoubleVector_create (integer min, integer max) {
	try {
		autoDoubleVector me = Thing_new (DoubleVector);
		DoubleVector_init (me.get(), min, max);
		return me;
	} catch (MelderError) {
		Melder_throw (U"DoubleVector not created.");
	}
}

Thing_implement (ComplexVector, Daata, 0);

void ComplexVector_init (ComplexVector me, integer min, integer max) {
	my min = min; my max = max;
	my v = NUMvector<dcomplex> (min, max);
}

autoComplexVector ComplexVector_create (integer min, integer max) {
	try {
		autoComplexVector me = Thing_new (ComplexVector);
		ComplexVector_init (me.get(), min, max);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ComplexVector not created.");
	}
}

/* End of file SimpleVector.cpp */
