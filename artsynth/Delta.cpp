/* Delta.cpp
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

#include "Delta.h"

static void destroy (I) {
	iam (Delta);
	if (my tube) {
		my tube ++;
		free (my tube);
		my tube = NULL;
	}
	inherited (Delta) destroy (me);
}

class_methods (Delta, Thing) {
	class_method (destroy)
	class_methods_end
}

Delta Delta_create (int numberOfTubes) {
	autoDelta me = Thing_new (Delta);
	Melder_assert (numberOfTubes >= 1);
	my numberOfTubes = numberOfTubes;
	my tube = (struct structDelta_Tube *) calloc (numberOfTubes, sizeof (struct structDelta_Tube));
	my tube --;
	for (int itube = 1; itube <= numberOfTubes; itube ++) {
		Delta_Tube t = my tube + itube;
		t -> parallel = 1;
	}
	return me.transfer();
}

/* End of file Delta.cpp */
