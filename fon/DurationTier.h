#ifndef _DurationTier_h_
#define _DurationTier_h_
/* DurationTier.h
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

#include "RealTier.h"
#include "Graphics.h"

/********** class DurationTier **********/

Thing_define (DurationTier, RealTier) {
	// overridden methods:
	protected:
		virtual void v_info ();
		virtual int v_domainQuantity () { return MelderQuantity_TIME_SECONDS; }
};

DurationTier DurationTier_create (double tmin, double tmax);

void DurationTier_draw (DurationTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, const wchar_t *method, int garnish);

DurationTier PointProcess_upto_DurationTier (PointProcess me);

/* End of file DurationTier.h */
#endif
