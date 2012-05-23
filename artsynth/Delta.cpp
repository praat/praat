/* Delta.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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

#include "Delta.h"

Thing_implement (Delta, Thing, 0);

void structDelta :: v_destroy () {
	NUMvector_free (this -> tube, 1);
	Delta_Parent :: v_destroy ();
}

void structDelta :: init (int a_numberOfTubes) {
	Melder_assert (a_numberOfTubes >= 1);
	this -> numberOfTubes = a_numberOfTubes;
	this -> tube = NUMvector <struct structDelta_Tube> (1, a_numberOfTubes);
	for (int itube = 1; itube <= a_numberOfTubes; itube ++) {
		Delta_Tube t = this -> tube + itube;
		t -> parallel = 1;
	}
}

Delta Delta_create (int a_numberOfTubes) {
	autoDelta me = Thing_new (Delta);
	my init (a_numberOfTubes);
	return me.transfer();
}

/* End of file Delta.cpp */
