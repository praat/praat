/* Function.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/07/10 NUMbessel_i0_f
 * pb 2005/06/16 units
 * pb 2006/12/08 info
 * pb 2007/03/17 domain texts
 * pb 2007/08/12 wchar_t
 * pb 2007/10/01 can write as encoding
 */

#include "Function.h"

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

/*
 * Methods for Function.
 */

static void info (I) {
	iam (Function);
	classData -> info (me);
	MelderInfo_writeLine1 (L"Domain:");
	MelderInfo_writeLine2 (L"   xmin: ", Melder_double (my xmin));
	MelderInfo_writeLine2 (L"   xmax: ", Melder_double (my xmax));
}

static double getXmin (I) {
	iam (Function);
	return my xmin;
}

static double getXmax (I) {
	iam (Function);
	return my xmax;
}

/*
 * Methods for Function_Table.
 */

static int getMinimumUnit (I, long ilevel) {
	(void) void_me;
	(void) ilevel;
	return 0;
}

static int getMaximumUnit (I, long ilevel) {
	(void) void_me;
	(void) ilevel;
	return 0;
}

static const wchar_t * getUnitText (I, long ilevel, int unit, unsigned long flags) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	(void) flags;
	return L"";
}

static int isUnitLogarithmic (I, long ilevel, int unit) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	return FALSE;
}

static double convertStandardToSpecialUnit (I, double value, long ilevel, int unit) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	return value;
}

static double convertSpecialToStandardUnit (I, double value, long ilevel, int unit) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	return value;
}

class_methods (Function, Data) {
	class_method_local (Function, copy)
	class_method_local (Function, equal)
	class_method_local (Function, canWriteAsEncoding)
	class_method_local (Function, writeText)
	class_method_local (Function, readText)
	class_method_local (Function, writeBinary)
	class_method_local (Function, readBinary)
	class_method_local (Function, description)
	class_method (info)
	class_method (getXmin)
	class_method (getXmax)
	us -> domainQuantity = 0;
	class_method (getMinimumUnit)
	class_method (getMaximumUnit)
	class_method (getUnitText)
	class_method (isUnitLogarithmic)
	class_method (convertStandardToSpecialUnit)
	class_method (convertSpecialToStandardUnit)
	class_methods_end
}

int Function_init (I, double xmin, double xmax) {
	iam (Function);
	my xmin = xmin;
	my xmax = xmax;
	return 1;
}

int ClassFunction_getMinimumUnit (I, long ilevel) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return my getMinimumUnit (me, ilevel);
}

int ClassFunction_getMaximumUnit (I, long ilevel) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return my getMaximumUnit (me, ilevel);
}

int ClassFunction_getDomainQuantity (I) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return my domainQuantity;
}

const wchar_t * ClassFunction_getUnitText (I, long ilevel, int unit, unsigned long flags) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	Melder_assert (unit >= my getMinimumUnit (me, ilevel) && unit <= my getMaximumUnit (me, ilevel));
	return my getUnitText (me, ilevel, unit, flags);
}

int ClassFunction_isUnitLogarithmic (I, long ilevel, int unit) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	Melder_assert (unit >= my getMinimumUnit (me, ilevel) && unit <= my getMaximumUnit (me, ilevel));
	return my isUnitLogarithmic (me, ilevel, unit);
}

double ClassFunction_convertStandardToSpecialUnit (I, double value, long ilevel, int unit) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return NUMdefined (value) ? my convertStandardToSpecialUnit (me, value, ilevel, unit) : NUMundefined;
}

double ClassFunction_convertSpecialToStandardUnit (I, double value, long ilevel, int unit) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return NUMdefined (value) ? my convertSpecialToStandardUnit (me, value, ilevel, unit) : NUMundefined;
}

double ClassFunction_convertToNonlogarithmic (I, double value, long ilevel, int unit) {
	iam (Function_Table);
	if (! my destroy) my _initialize (me);
	return NUMdefined (value) && my isUnitLogarithmic (me, ilevel, unit) ? pow (10.0, value) : value;
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
			if (! one_by_bessi_0_12) one_by_bessi_0_12 = 1.0 / NUMbessel_i0_f (12);
			return NUMbessel_i0_f (12 * sqrt (1 - (1.0 / 0.77 / 0.77) * tim * tim)) * one_by_bessi_0_12;
		case Function_KAISER20:
			if (tim <= -1 || tim >= 1) return 0.0;
			if (! one_by_bessi_0_20) one_by_bessi_0_20 = 1.0 / NUMbessel_i0_f (20.24);
			return NUMbessel_i0_f (20.24 * sqrt (1 - tim * tim)) * one_by_bessi_0_20;
		case Function_GAUSSIAN:
			return exp ((- NUMpi * NUMpi) * tim * tim);
		default:
			return 0.0;
	}
}

void Function_unidirectionalAutowindow (I, double *xmin, double *xmax) {
	iam (Function);
	if (*xmin >= *xmax) {
		*xmin = my xmin;
		*xmax = my xmax;
	}
}

void Function_bidirectionalAutowindow (I, double *x1, double *x2) {
	iam (Function);
	if (*x1 == *x2) {
		*x1 = my xmin;
		*x2 = my xmax;
	}
}

int Function_intersectRangeWithDomain (I, double *x1, double *x2) {
	iam (Function);
	if (*x1 == *x2) return 0;
	if (*x1 < *x2) {
		if (*x1 < my xmin) *x1 = my xmin;   /* Intersect requested range with logical domain. */
		if (*x2 > my xmax) *x2 = my xmax;
		if (*x2 <= *x1) return 0;   /* Requested range and logical domain do not intersect. */
	} else {
		if (*x2 < my xmin) *x2 = my xmin;   /* Intersect requested range with logical domain. */
		if (*x1 > my xmax) *x1 = my xmax;
		if (*x1 <= *x2) return 0;   /* Requested range and logical domain do not intersect. */
	}
	return 1;
}

/* End of file Function.c */
