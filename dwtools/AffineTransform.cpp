/* AffineTransform.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
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

void structAffineTransform :: v_transform (double **in, integer nrows, double **out) {
	for (integer i = 1; i <= nrows; i ++) {
		for (integer j = 1; j <= n; j ++) {
			double tmp = 0;
			for (integer k = 1; k <= n; k ++) {
				tmp += in [i] [k] * r [k] [j];
			}
			out [i] [j] = tmp + t [j];
		}
	}
}

autoAffineTransform structAffineTransform :: v_invert () {
	autoAffineTransform thee = Data_copy (this);
	double tolerance = 0.000001;

	NUMpseudoInverse (r, n, n, thy r, tolerance);
	for (integer i = 1; i <= n; i ++) {
		thy t [i] = 0.0;
		for (integer j = 1; j <= thy n; j ++) {
			thy t [i] -= thy r [i] [j] * t [j];
		}
	}
	return thee;
}

Thing_implement (AffineTransform, Daata, 0);

void AffineTransform_init (AffineTransform me, integer n) {
	Melder_require (n > 0, U"Dimensionality should be greater than zero.");
	
	my n = n;
	my r = NUMmatrix<double> (1, n, 1, n);
	my t = NUMvector<double> (1, n);
}

autoAffineTransform AffineTransform_create (integer n) {
	try {
		autoAffineTransform me = Thing_new (AffineTransform);
		AffineTransform_init (me.get(), n);
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
		autoTableOfReal thee = TableOfReal_create (my n, my n);
		NUMmatrix_copyElements (my r, thy data, 1, my n, 1, my n);
		for (integer i = 1; i <= my n; i ++) {
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
		autoTableOfReal thee = TableOfReal_create (1, my n);
		for (integer i = 1; i <= my n; i ++) {
			thy data [1] [i] = my t [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": translation vector not extracted.");
	}
}

/* End of file AffineTransform.cpp */
