/* AffineTransform.cpp
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
 djmw 2001
 djmw 20020315 GPL header
 djmw 20041027 Added AffineTransform_extractMatrix
 djmw 20050726 Added AffineTransform_extractTranslationVector
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
 */

#include "AffineTransform.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "AffineTransform_def.h"
#include "oo_COPY.h"
#include "AffineTransform_def.h"
#include "oo_EQUAL.h"
#include "AffineTransform_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "AffineTransform_def.h"
#include "oo_WRITE_TEXT.h"
#include "AffineTransform_def.h"
#include "oo_WRITE_BINARY.h"
#include "AffineTransform_def.h"
#include "oo_READ_TEXT.h"
#include "AffineTransform_def.h"
#include "oo_READ_BINARY.h"
#include "AffineTransform_def.h"
#include "oo_DESCRIPTION.h"
#include "AffineTransform_def.h"

void structAffineTransform :: v_transform (MATVU const& out, constMATVU const& in) {
	Melder_assert (in.nrow == out.nrow);
	Melder_assert (in.ncol == out.ncol);
	mul_MAT_out (out, in, our r.get());
	out  +=  our t.all();
}

autoAffineTransform structAffineTransform :: v_invert () {
	autoAffineTransform thee = Data_copy (this);
	constexpr double tolerance = 0.000001;

	MATpseudoInverse (thy r.get(), our r.get(), tolerance);
	mul_VEC_out (thy t.get(), thy r.get(), our t.get());
	thy t.get()  *=  -1.0;
	return thee;
}

Thing_implement (AffineTransform, Daata, 0);

void AffineTransform_init (AffineTransform me, integer dimension) {
	Melder_require (dimension > 0, U"Dimensionality should be greater than zero.");
	
	my dimension = dimension;
	my r = zero_MAT (dimension, dimension);
	my t = zero_VEC (dimension);
}

autoAffineTransform AffineTransform_create (integer dimension) {
	try {
		autoAffineTransform me = Thing_new (AffineTransform);
		AffineTransform_init (me.get(), dimension);
		return me;
	} catch (MelderError) {
		Melder_throw (U"AffineTransform not created.");
	}
}

autoAffineTransform AffineTransform_invert (AffineTransform me) {
	return my v_invert ();
}

autoTableOfReal AffineTransform_extractMatrix (AffineTransform me) {
	try {
		autoTableOfReal thee = TableOfReal_create (my dimension, my dimension);
		thy data.all() <<= my r.all();
		for (integer i = 1; i <= my dimension; i ++) {
			char32 label [40];
			Melder_sprint (label,40, i);
			TableOfReal_setRowLabel (thee.get(), i, label);
			TableOfReal_setColumnLabel (thee.get(), i, label);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": transformation matrix not extracted.");
	}
}

autoTableOfReal AffineTransform_extractTranslationVector (AffineTransform me) {
	try {
		autoTableOfReal thee = TableOfReal_create (1, my dimension);
		thy data.row (1) <<= my t.get();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": translation vector not extracted.");
	}
}

/* End of file AffineTransform.cpp */
