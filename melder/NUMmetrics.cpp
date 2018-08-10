/* NUMmetrics.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018 Paul Boersma
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

#include "melder.h"

int NUMrotationsPointInPolygon (double x0, double y0, integer n, double x [], double y []) {
	integer nup = 0, i;
	int upold = y [n] > y0, upnew;
	for (i = 1; i <= n; i ++) if ((upnew = y [i] > y0) != upold) {
		integer j = i == 1 ? n : i - 1;
		if (x0 < x [i] + (x [j] - x [i]) * (y0 - y [i]) / (y [j] - y [i])) {
			if (upnew) nup ++; else nup --;
		}
		upold = upnew;
	}
	return nup;
}

/* End of file NUM.cpp */
