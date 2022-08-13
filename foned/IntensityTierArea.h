#ifndef _IntensityTierArea_h_
#define _IntensityTierArea_h_
/* IntensityTierArea.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015-2018,2020-2022 Paul Boersma
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
#include "IntensityTier.h"

Thing_define (IntensityTierArea, RealTierArea) {
	conststring32 v_menuTitle ()
		override { return U"IntensityTier"; }
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	conststring32 v_quantityText ()
		override { return U"Intensity (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set intensity range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum intensity (dB)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum intensity (dB)"; }

	#include "IntensityTierArea_prefs.h"
};

DEFINE_FunctionArea_create (IntensityTierArea, IntensityTier)

/* End of file IntensityTierArea.h */
#endif
