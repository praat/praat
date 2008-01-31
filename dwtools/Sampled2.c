/* Sampled2.c
 *
 * Copyright (C) 1993-2008 David Weenink
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
 djmw 19970408
 djmw 20020813 GPL header
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
*/

#include "Sampled2.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Sampled2_def.h"
#include "oo_COPY.h"
#include "Sampled2_def.h"
#include "oo_EQUAL.h"
#include "Sampled2_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Sampled2_def.h"
#include "oo_WRITE_TEXT.h"
#include "Sampled2_def.h"
#include "oo_WRITE_BINARY.h"
#include "Sampled2_def.h"
#include "oo_READ_BINARY.h"
#include "Sampled2_def.h"
#include "oo_DESCRIPTION.h"
#include "Sampled2_def.h"

static int readText (I, MelderReadString *text) {
	iam (Sampled2);
	my xmin = texgetr8 (text);
	my xmax = texgetr8 (text);
	my nx = texgeti4 (text);
	my dx = texgetr8 (text);
	my x1 = texgetr8 (text);
	my ymin = texgetr8 (text);
	my ymax = texgetr8 (text);
	my ny = texgeti4 (text);
	my dy = texgetr8 (text);
	my y1 = texgetr8 (text);
	if (my xmin > my xmax || my ymin > my ymax)
		return Melder_error1 (L"(Sampled2::readText:) xmin should <= xmax and ymin <= ymax.");
	if (my nx < 1 || my ny < 1)
		return Melder_error1 (L"(Sampled2::readText:) nx should >= 1 and ny >= 1.");
	if (my dx <= 0 || my dy <= 0)
		return Melder_error1 (L"(Sampled2::readText:) dx should > 0 and dy > 0.");
	return 1;
}

class_methods (Sampled2, Data)
	class_method_local (Sampled2, destroy)
	class_method_local (Sampled2, description)
	class_method_local (Sampled2, copy)
	class_method_local (Sampled2, equal)
	class_method_local (Sampled2, canWriteAsEncoding)
	class_method_local (Sampled2, writeText)
	class_method (readText)
	class_method_local (Sampled2, writeBinary)
	class_method_local (Sampled2, readBinary)
class_methods_end

int Sampled2_init
	(I, double xmin, double xmax, long nx, double dx, double x1,
		 double ymin, double ymax, long ny, double dy, double y1)
{
	iam (Sampled2);
	my xmin = xmin; my xmax = xmax; my nx = nx; my dx = dx; my x1 = x1;
	my ymin = ymin; my ymax = ymax; my ny = ny; my dy = dy; my y1 = y1;
	return 1;
}

double Sampled2_columnToX (I, double column) { iam (Sampled2); return my x1 + (column - 1) * my dx; }

double Sampled2_rowToY (I, double row) { iam (Sampled2); return my y1 + (row - 1) * my dy; }

double Sampled2_xToColumn (I, double x) { iam (Sampled2); return (x - my x1) / my dx + 1; }

long Sampled2_xToLowColumn (I, double x) { iam (Sampled2); return (long) floor (Sampled2_xToColumn (me, x)); }

long Sampled2_xToHighColumn (I, double x) { iam (Sampled2); return (long) ceil (Sampled2_xToColumn (me, x)); }

long Sampled2_xToNearestColumn (I, double x) { iam (Sampled2); return (long) floor (Sampled2_xToColumn (me, x) + 0.5); }

double Sampled2_yToRow (I, double y) { iam (Sampled2); return (y - my y1) / my dy + 1; }

long Sampled2_yToLowRow (I, double y) { iam (Sampled2); return (long) floor (Sampled2_yToRow (me, y)); }

long Sampled2_yToHighRow (I, double y) { iam (Sampled2); return (long) ceil (Sampled2_yToRow (me, y)); }

long Sampled2_yToNearestRow (I, double y) { iam (Sampled2); return (long) floor (Sampled2_yToRow (me, y) + 0.5); }

long Sampled2_getWindowSamplesX (I, double xmin, double xmax, long *ixmin, long *ixmax) {
	iam (Sampled2);
	*ixmin = 1 + (long) ceil  ((xmin - my x1) / my dx);
	*ixmax = 1 + (long) floor ((xmax - my x1) / my dx);
	if (*ixmin < 1) *ixmin = 1;
	if (*ixmax > my nx) *ixmax = my nx;
	if (*ixmin > *ixmax) return 0;
	return *ixmax - *ixmin + 1;
}

long Sampled2_getWindowSamplesY (I, double ymin, double ymax, long *iymin, long *iymax) {
	iam (Sampled2);
	*iymin = 1 + (long) ceil  ((ymin - my y1) / my dy);
	*iymax = 1 + (long) floor ((ymax - my y1) / my dy);
	if (*iymin < 1) *iymin = 1;
	if (*iymax > my ny) *iymax = my ny;
	if (*iymin > *iymax) return 0;
	return *iymax - *iymin + 1;
}

long Sampled2_getWindowExtrema_d (I, double **z, long ixmin, long ixmax, long iymin, long iymax,
	double *minimum, double *maximum)
{
	iam (Sampled2);
	long iy, ix;
	if (ixmin == 0) ixmin = 1;
	if (ixmax == 0) ixmax = my nx;
	if (iymin == 0) iymin = 1;
	if (iymax == 0) iymax = my ny;
	if (ixmin > ixmax || iymin > iymax) return 0;
	*minimum = *maximum = z[iymin][ixmin];
	for (iy = iymin; iy <= iymax; iy ++)
		for (ix = ixmin; ix <= ixmax; ix ++)
		{
			if (z[iy][ix] < *minimum) *minimum = z[iy][ix];
			if (z[iy][ix] > *maximum) *maximum = z[iy][ix];
		}
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

long Sampled2_getWindowExtrema_f (I, float **z, long ixmin, long ixmax, long iymin, long iymax,
	double *minimum, double *maximum)
{
	iam (Sampled2);
	long iy, ix;
	if (ixmin == 0) ixmin = 1;
	if (ixmax == 0) ixmax = my nx;
	if (iymin == 0) iymin = 1;
	if (iymax == 0) iymax = my ny;
	if (ixmin > ixmax || iymin > iymax) return 0;
	*minimum = *maximum = z[iymin] [ixmin];
	for (iy = iymin; iy <= iymax; iy ++)
		for (ix = ixmin; ix <= ixmax; ix ++)
		{
			if (z[iy][ix] < *minimum) *minimum = z[iy][ix];
			if (z[iy][ix] > *maximum) *maximum = z[iy][ix];
		}
	return (ixmax - ixmin + 1) * (iymax - iymin + 1);
}

/* End of file Sampled2.c */
