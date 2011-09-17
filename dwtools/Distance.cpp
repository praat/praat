/* Distance.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20020813 GPL header
 djmw 20110304 Thing_new
*/

#include "Distance.h"

Thing_implement (Distance, Proximity, 0);

Distance Distance_create (long numberOfPoints) {
	try {
		autoDistance me = Thing_new (Distance);
		Proximity_init (me.peek(), numberOfPoints);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Distance not created.");
	}
}

void Distance_drawDendogram (Distance me, Graphics g, int method) {
	(void) me;
	(void) g;
	(void) method;
}

/* End of file Distance.cpp */
