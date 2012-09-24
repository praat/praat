#ifndef _VocalTract_h_
#define _VocalTract_h_
/* VocalTract.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Vector.h"
#include "Graphics.h"

Thing_define (VocalTract, Vector) {
	// overridden methods:
	protected:
		virtual void v_info ();
		virtual int v_domainQuantity () { return MelderQuantity_DISTANCE_FROM_GLOTTIS_METRES; }
};

/*
	Attributes:
		xmin				// 0.
		xmax > xmin		// Total length (metres).
		nx >= 1			// Number of sections.
		dx > 0.0			// Section length (metres).
		x1				// Centre of first section (metres).
		ymin, ymax, ny, dy, y1 = 1
		z [1] [1..nx]		// The area, in square metres.
*/

VocalTract VocalTract_create (long nx, double dx);
/*
	Function:
		create a VocalTract, or NULL if out of memory.
	Preconditions:
		nx >= 1;
		dx > 0.0;
	Postconditions:
		my xmin == 0;				my ymin == 1;
		my xmax == nx * dx;		my ymax == 1;
		my nx == nx;				my ny == 1;
		my dx == dx;				my dy == 1;
		my x1 == 0.5 * dx;			my y1 == 1;
		my z [1] [1..nx] == 1e-4; // straight tube, area 1 cm2.
 */

VocalTract VocalTract_createFromPhone (const wchar_t *phone);
/* 'phone' is one of the following: a e i o u y1 y2 y3 jery p t k x pa ta ka pi ti ki pu tu ku */

void VocalTract_draw (VocalTract me, Graphics g);   /* Draw a VocalTract into a Graphics. */

Matrix VocalTract_to_Matrix (VocalTract me);
/*
	Create a Matrix from a VocalTract,
	with deep copy of all of its Matrix attributes, except class information and methods.
	Return NULL if out of memory.  
*/

VocalTract Matrix_to_VocalTract (Matrix me);
/*
	Function:
		create a VocalTract from a Matrix.
		Return NULL if out of memory.
	Postconditions:
		thy xmin == my xmin;
		thy xmax == my xmax;
		thy nx == my nx;
		thy dx == my dx;
		thy x1 == my x1;
		thy ymin ymax ny dy y1 == 1;
		thy z [1] [...] == my z [1] [...];
*/

/* End of file VocalTract.h */
#endif
