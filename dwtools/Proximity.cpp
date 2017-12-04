/* Proximity.cpp
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
 djmw 20020813 GPL header
 djmw 20040309 Removed assertion 'numberOfPoints> 0' in Proximity_init
*/

#include "Proximity.h"
#include "TableOfReal_extensions.h"

Thing_implement (Proximity, TableOfReal, 0);

void Proximity_init (Proximity me, integer numberOfPoints) {
	TableOfReal_init (me, numberOfPoints, numberOfPoints);
	TableOfReal_setSequentialRowLabels (me, 0, 0, nullptr, 1, 1);
	TableOfReal_setSequentialColumnLabels (me, 0, 0, nullptr, 1, 1);
}

/* End of file Proximity.cpp */
