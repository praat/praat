/* Function.cpp
 *
 * Copyright (C) 1992-2012,2015,2017 Paul Boersma
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

#include "Function.h"

#include "oo_DESTROY.h"
#include "Function_def.h"
#include "oo_COPY.h"
#include "Function_def.h"
#include "oo_EQUAL.h"
#include "Function_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Function_def.h"
#include "oo_WRITE_TEXT.h"
#include "Function_def.h"
#include "oo_READ_TEXT.h"
#include "Function_def.h"
#include "oo_WRITE_BINARY.h"
#include "Function_def.h"
#include "oo_READ_BINARY.h"
#include "Function_def.h"
#include "oo_DESCRIPTION.h"
#include "Function_def.h"

Thing_implement (Function, Daata, 0);

void structFunction :: v_info () {
	Function_Parent :: v_info ();
	MelderInfo_writeLine (U"Domain:");
	MelderInfo_writeLine (U"   xmin: ", xmin);
	MelderInfo_writeLine (U"   xmax: ", xmax);
}

void structFunction :: v_shiftX (double xfrom, double xto) {
	NUMshift (& xmin, xfrom, xto);
	NUMshift (& xmax, xfrom, xto);
}

void structFunction :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	NUMscale (& xmin, xminfrom, xmaxfrom, xminto, xmaxto);
	NUMscale (& xmax, xminfrom, xmaxfrom, xminto, xmaxto);
}

void Function_init (Function me, double xmin_, double xmax_) {
	my xmin = xmin_;
	my xmax = xmax_;
}

int Function_getMinimumUnit (Function me, integer level) {
	return my v_getMinimumUnit (level);
}

int Function_getMaximumUnit (Function me, integer level) {
	return my v_getMaximumUnit (level);
}

int Function_getDomainQuantity (Function me) {
	return my v_domainQuantity ();
}

conststring32 Function_getUnitText (Function me, integer level, int unit, uint32 flags) {
	Melder_assert (unit >= my v_getMinimumUnit (level) && unit <= my v_getMaximumUnit (level));
	return my v_getUnitText (level, unit, flags);
}

bool Function_isUnitLogarithmic (Function me, integer level, int unit) {
	Melder_assert (unit >= my v_getMinimumUnit (level) && unit <= my v_getMaximumUnit (level));
	return my v_isUnitLogarithmic (level, unit);
}

double Function_convertStandardToSpecialUnit (Function me, double value, integer level, int unit) {
	return isdefined (value) ? my v_convertStandardToSpecialUnit (value, level, unit) : undefined;
}

double Function_convertSpecialToStandardUnit (Function me, double value, integer level, int unit) {
	return isdefined (value) ? my v_convertSpecialToStandardUnit (value, level, unit) : undefined;
}

double Function_convertToNonlogarithmic (Function me, double value, integer level, int unit) {
	return isdefined (value) && my v_isUnitLogarithmic (level, unit) ? pow (10.0, value) : value;
}

void Function_shiftXBy (Function me, double shift) {
	my v_shiftX (0.0, shift);
}

void Function_shiftXTo (Function me, double xfrom, double xto) {
	my v_shiftX (xfrom, xto);
}

void Function_scaleXBy (Function me, double factor) {
	my v_scaleX (0.0, 1.0, 0.0, factor);
}

void Function_scaleXTo (Function me, double xminto, double xmaxto) {
	my v_scaleX (my xmin, my xmax, xminto, xmaxto);
}

double Function_window (double tim, int windowType) {
	static double one_by_bessi_0_12, one_by_bessi_0_20;
	switch (windowType) {
		case Function_RECTANGULAR:
			if (tim < -0.5 || tim > 0.5) return 0.0;
			return 1;
		case Function_TRIANGULAR:
			if (tim < -0.5 || tim > 0.5) return 0.0;
			return 1 - tim - tim;
		case Function_PARABOLIC:
			if (tim < -0.5 || tim > 0.5) return 0.0;
			return 1 - 4 * tim * tim;
		case Function_HANNING:
			if (tim < -0.5 || tim > 0.5) return 0.0;
			return 0.5 + 0.5 * cos (2 * NUMpi * tim);
		case Function_HAMMING:
			if (tim < -0.5 || tim > 0.5) return 0.0;
			return 0.54 + 0.46 * cos (2 * NUMpi * tim);
		case Function_POTTER:
			if (tim < -0.77 || tim > 0.77) return 0.0;
			return 0.54 + 0.46 * cos (2 * NUMpi * tim);
		case Function_KAISER12:
			if (tim < -0.77 || tim > 0.77) return 0.0;
			if (one_by_bessi_0_12 == 0.0) one_by_bessi_0_12 = 1.0 / NUMbessel_i0_f (12);
			return NUMbessel_i0_f (12 * sqrt (1 - (1.0 / 0.77 / 0.77) * tim * tim)) * one_by_bessi_0_12;
		case Function_KAISER20:
			if (tim <= -1 || tim >= 1) return 0.0;
			if (one_by_bessi_0_20 == 0.0) one_by_bessi_0_20 = 1.0 / NUMbessel_i0_f (20.24);
			return NUMbessel_i0_f (20.24 * sqrt (1 - tim * tim)) * one_by_bessi_0_20;
		case Function_GAUSSIAN:
			return exp ((- NUMpi * NUMpi) * tim * tim);
		default:
			return 0.0;
	}
}

void Function_unidirectionalAutowindow (Function me, double *xmin, double *xmax) {
	if (*xmin >= *xmax) {
		*xmin = my xmin;
		*xmax = my xmax;
	}
}

void Function_bidirectionalAutowindow (Function me, double *x1, double *x2) {
	if (*x1 == *x2) {
		*x1 = my xmin;
		*x2 = my xmax;
	}
}

bool Function_intersectRangeWithDomain (Function me, double *x1, double *x2) {
	if (*x1 == *x2) return false;
	if (*x1 < *x2) {
		if (*x1 < my xmin) *x1 = my xmin;   // intersect requested range with logical domain
		if (*x2 > my xmax) *x2 = my xmax;
		if (*x2 <= *x1) return false;   // requested range and logical domain do not intersect
	} else {
		if (*x2 < my xmin) *x2 = my xmin;   // intersect requested range with logical domain
		if (*x1 > my xmax) *x1 = my xmax;
		if (*x1 <= *x2) return false;   // requested range and logical domain do not intersect
	}
	return true;
}

/* End of file Function.cpp */
