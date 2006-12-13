/* VocalTract.c
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/08/08 reduced compiler warnings
 * pb 2006/12/10 MelderInfo
 */

#include "VocalTract.h"

static void info (I) {
	iam (VocalTract);
	classData -> info (me);
	MelderInfo_writeLine3 ("Vocal tract length: ", Melder_single (my xmax), " metres");
	MelderInfo_writeLine2 ("Number of sections: ", Melder_integer (my nx));
	MelderInfo_writeLine3 ("Section length: ", Melder_single (my dx), " metres");
}

class_methods (VocalTract, Vector)
	class_method (info)
class_methods_end

VocalTract VocalTract_create (long nx, double dx) {
	VocalTract me = new (VocalTract);
	if (! me || ! Matrix_init (me, 0, nx * dx, nx, dx, 0.5 * dx, 1, 1, 1, 1, 1)) return NULL;
	return me;
}

#define MinimumWidth 0.0001
static struct { char *phone; int numberOfSections; float area [40]; }
	data [] = {
{ "a", 34, { 1.7, 1.2, 1.6, 3.39, 2.1, 1.4, 1, 0.8, 0.8, 0.8, 1, 1.4,
	2.1, 2.9, 3.09, 2.1, 2.5, 4, 5.3, 6.16, 7, 7.6, 8.15, 8.5, 8.6,
	8.4, 8, 7.5, 6.9, 6, 5.1, 5, 5.5, 7.9 } },
{ "e", 33, { 2.3, 1.95, 1.73, 1.7, 5.3, 6.3, 6.8, 7.55, 8.2, 9.1, 9.7,
	10.1, 10.2, 10, 8, 7.2, 7.5, 6.4, 5.4, 4.9, 4.35, 3.9, 3.5, 3.1,
	2.7, 2.4, 2.2, 2.5, 3.4, 5, 6.7, 8.5, 10 } },
{ "i", 35, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 5, 8 } },
{ "o", 37, { 2.6, 2.05, 1.56, 1.3, 5.2, 4.54, 3.49, 2.6, 2.1, 1.8, 1.6, 1.4,
	1.29, 1.19, 1.22, 2.6, 2.9, 2.2, 2.6, 3.6, 4.55, 5.55, 6.4, 7.15,
	8, 8.9, 9.6, 10.5, 11.8, 14.6, 14.5, 12.9, 10.4, 5, 3.4, 3.4, 4 } },
{ "u", 40, { 2.5, 2.5, 2.5, 2.4, 5, 8.1, 8.9, 8.9, 8.4, 7.5, 5.8, 3.9, 2.3, 1.6, 1.2,
	1.05, 1.1, 1.4, 2.2, 2, 1.3, 2, 2.2, 2.3, 2.8, 3.7, 5, 6.2, 7.9,
	10.9, 12.9, 13.15, 13, 12.5, 9.9, 3.9, 1.8, 0.32, 0.4, 0.6 } },
{ "y1", 37, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 1.8, 0.32,
	0.4, 0.6 } },
{ "y2", 38, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 4, 1.8, 0.32,
	0.4, 0.6 } },
{ "y3", 39, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 4, 4, 1.8, 0.32,
	0.4, 0.6 } },
{ "jery", 38, { 3.2, 3.2, 3.2, 3.2, 10.5, 10.5, 13, 13, 10.5, 10.5, 10.5, 10.5, 10.5,
	8, 6.5, 6.5, 5, 4, 2, 2.6, 1.6, 1.3, 1, 1, 1.3, 1.6, 2, 2, 2.6, 3.2, 5,
	8, 8, 8, 6.5,
	2, 6.5, 6.5 } },
{ "p", 39, { 3.5, 3.2, 2.9, 2.6, 6.9, 6.65, 5.8, 4.9, 4, 3.14, 2.5, 1.84,
	1.25, 0.83, 0.6, 0.53,
	0.5, 0.6, 0.85, 1, 1.6, 2.05, 2.55, 3.08, 3.67, 4.15, 4.8, 5.5, 6.3,
	7.4, 12, 12.98, 12.9, 11.4, 6.6, 2, 0.45, 0.1, MinimumWidth } },
{ "t", 36, { 2.8, 2.7, 2.4, 2.6, 6.45, 6.01, 5.31, 4.85, 4.55, 4.32, 4.18, 4.1,
	4.04, 3.97, 3.85, 3.7, 3.4, 3.05, 2.91, 3.1, 3.55, 3.9, 4.1, 4, 3.8,
	3.3, 2.55, 1.8, 1, 0.45, 0.1, MinimumWidth, 0.8, 2.5, 6, 9 } },
{ "k", 38, { 2.4, 2.7, 3, 3.3, 7, 9.38, 9.25, 8.62, 7.8, 6.7, 5.4, 4, 2.8, 1.9, 1.35,
	0.9, 0.55, 0.3, 0.19, 0.07, MinimumWidth, 0.12, 0.17, 0.3, 0.5, 0.9,
	1.4, 2.2, 3.3, 5, 9, 11.25, 10.9, 7.3, 4.3, 3.5, 3.7, 6 } },
{ "x", 40, { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 } },
{ "pa", 39, { 1.7, 1.2, 1.6, 3.39, 2.1, 1.4, 1, 0.8, 0.8, 0.8, 1, 1.4,
	2.1, 2.9, 3.09, 2.1, 2.5, 4, 5.3, 6.16, 7, 7.6, 8.15, 8.5, 8.6,
	8.4, 8, 7.5, 6.9, 6, 5.1, 5, 5.5, 7.9, 6.6, 2, 0.45, 0.1,
	MinimumWidth } },
{ "ta", 34, { 1.7, 1.2, 1.6, 3.39, 2.1, 1.4, 1, 0.8, 0.8, 0.8, 1, 1.4,
	2.1, 2.9, 3.09, 2.1, 2.5, 4, 5.3, 6.16, 7, 7.6, 8.15, 8.5, 8.6,
	8.5, 6, 2, 0.45, MinimumWidth, 0.8, 2.5, 5.5, 7.9 } },
{ "ka", 34, { 1.7, 1.2, 1.6, 3.39, 2.1, 1.4, 1, 0.8, 0.8, 0.8, 1, 1.4,
	2.1, 2.9, 3.09, 2.1, 0.3, MinimumWidth,
	0.3, 2, 5, 7.6, 8.15, 8.5, 8.6,
	8.4, 8, 7.5, 6.9, 6, 5.1, 5, 5.5, 7.9 } },
{ "pi", 39, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 5,
	6.6, 2, 0.45, 0.1, MinimumWidth } },
{ "ti", 35, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 7.8, 7, 4.5, 2.8, 1.9, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.5, 0.5, 0.3, 0.1, MinimumWidth,
	0.8, 2.5, 8 } },
{ "ki", 35, { 3, 2.9, 2.75, 2.58, 2.7, 6.35, 7.8, 8.9, 9.6, 10.15, 10.55,
	10.9, 11.15, 11.3, 11.2, 10.8, 8, 6, 2, 0.3, MinimumWidth, 0.3, 1.3,
	0.9, 0.65, 0.55, 0.5, 0.55, 0.7, 0.95, 1.3, 2, 3, 5, 8 } },
{ "pu", 40, { 2.5, 2.5, 2.5, 2.4, 5, 8.1, 8.9, 8.9, 8.4, 7.5, 5.8, 3.9, 2.3, 1.6, 1.2,
	1.05, 1.1, 1.4, 2.2, 2, 1.3, 2, 2.2, 2.3, 2.8, 3.7, 5, 6.2, 7.9,
	10.9, 12.9, 13.15, 13, 12.5, 9.9, 6.6, 2, 0.45, 0.1, MinimumWidth } },
{ "tu", 40, { 2.5, 2.5, 2.5, 2.4, 5, 8.1, 8.9, 8.9, 8.4, 7.5, 5.8, 3.9, 2.3, 1.6, 1.2,
	1.05, 1.1, 1.4, 2.2, 2, 1.3, 2, 2.2, 2.3, 2.8, 3.7, 5, 6.2, 7.9,
	10.9, 9, 3, 0.4, MinimumWidth, 0.8, 2.5, 1.8, 0.32, 0.4, 0.6 } },
{ "ku", 40, { 2.5, 2.5, 2.5, 2.4, 5, 8.1, 8.9, 8.9, 8.4, 7.5, 5.8, 3.9, 2.3, 1.6, 1.2,
	1.05, 1.1, 1.4, 1.2, 0.3, MinimumWidth, 0.3, 2.2, 2.3, 2.8, 3.7, 5, 6.2, 7.9,
	10.9, 12.9, 13.15, 13, 12.5, 9.9, 3.9, 1.8, 0.32, 0.4, 0.6 } },
{ NULL, 0, { 0 } } };

VocalTract VocalTract_createFromPhone (char *phone) {
	VocalTract me;
	int i = 0, isection;
	while (data [i].phone && strcmp (data [i].phone, phone)) i ++;
	if (! data [i].phone)
		return Melder_errorp ("VocalTract_createFromFant60: unknown phone %.100s", phone);
	me = VocalTract_create (data [i]. numberOfSections, 0.005);
	for (isection = 1; isection <= my nx; isection ++)
		my z [1] [isection] = data [i]. area [isection - 1] * 0.0001;
	return me;
}

void VocalTract_draw (VocalTract me, Graphics g) {
	Matrix_drawRows (me, g, 0, 0, 0, 0, 0, 0);
}

Matrix VocalTract_to_Matrix (VocalTract me) {
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
							my ymin, my ymax, my ny, my dy, my y1);
	NUMfvector_copyElements (my z [1], thy z [1], 1, my nx);
	return thee;
}

VocalTract Matrix_to_VocalTract (Matrix me) {
	VocalTract thee = VocalTract_create (my nx, my dx);
	if (! thee) return NULL;
	NUMfvector_copyElements (my z [1], thy z [1], 1, my nx);
	return thee;
}

/* End of file VocalTract.c */
