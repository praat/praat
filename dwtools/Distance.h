#ifndef _Distance_h_
#define _Distance_h_
/* Distance.h
 *
 * Copyright (C) 1993-2019 David Weenink
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

#include "Configuration.h"
#include "Proximity.h"
#include "Graphics.h"

Thing_define (Distance, Proximity) {
};

autoDistance Distance_create (integer numberOfPoints);

autoDistance Configuration_to_Distance (Configuration me);

void Distance_drawDendogram (Distance me, Graphics g, int method);

double Distance_getMaximumDistance (Distance me);

Collection_define (DistanceList, OrderedOf, Distance) {
	ProximityList asProximityList () {
		return reinterpret_cast<ProximityList> (this);
	}
	TableOfRealList asTableOfRealList () {
		return reinterpret_cast<TableOfRealList> (this);
	}
};

#endif /* _Distance_h_ */
