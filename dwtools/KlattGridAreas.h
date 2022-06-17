#ifndef _KlattGridAreas_h_
#define _KlattGridAreas_h_
/* KlattGridAreas.h
 *
 * Copyright (C) 2009-2011 David Weenink, 2017,2021,2022 Paul Boersma
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

#include "RealTierArea.h"
#include "KlattGrid.h"

Thing_define (KlattGrid_IntensityTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	#include "KlattGrid_IntensityTierArea_prefs.h"
};

/* End of file KlattGridAreas.h */
#endif
