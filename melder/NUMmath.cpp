/* NUMmath.cpp
 *
 * Copyright (C) 1992-2008,2011,2012,2014,2015,2017,2018,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

void NUMshift (double *x, double xfrom, double xto) {
	if (*x == xfrom)
		*x = xto;
	else
		*x += xto - xfrom;
}

void NUMscale (double *x, double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	if (*x == xminfrom)
		*x = xminto;
	else if (*x == xmaxfrom)
		*x = xmaxto;
	else
		*x = xminto + (xmaxto - xminto) * ((*x - xminfrom) / (xmaxfrom - xminfrom));
}

/* End of file NUMmath.cpp */
