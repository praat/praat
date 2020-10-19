#ifndef _DurationTier_h_
#define _DurationTier_h_
/* DurationTier.h
 *
 * Copyright (C) 1992-2005,2007,2010-2012,2015,2016,2018,2020 Paul Boersma
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

#include "RealTier.h"
#include "Graphics.h"

/********** class DurationTier **********/

Thing_define (DurationTier, RealTier) {
	void v_info ()
		override;
	int v_domainQuantity ()
		override { return MelderQuantity_TIME_SECONDS; }
};

autoDurationTier DurationTier_create (double tmin, double tmax);

void DurationTier_draw (DurationTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, conststring32 method, bool garnish);

autoDurationTier PointProcess_upto_DurationTier (PointProcess me);

/* End of file DurationTier.h */
#endif
