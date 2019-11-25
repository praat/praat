/* Delta.cpp
 *
 * Copyright (C) 1992-2005,2011,2012,2014-2017,2019 Paul Boersma
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

#include "Delta.h"

Thing_implement (Delta, Thing, 0);

void Delta_init (Delta me, integer numberOfTubes) {
	Melder_assert (numberOfTubes >= 1);
	my numberOfTubes = numberOfTubes;
	my tubes = newvectorzero <structDelta_Tube> (numberOfTubes);
	for (int itube = 1; itube <= numberOfTubes; itube ++) {
		Delta_Tube t = & my tubes [itube];
		t -> parallel = 1;
	}
}

autoDelta Delta_create (integer numberOfTubes) {
	autoDelta me = Thing_new (Delta);
	Delta_init (me.get(), numberOfTubes);
	return me;
}

/* End of file Delta.cpp */
