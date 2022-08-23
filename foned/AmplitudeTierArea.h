#ifndef _AmplitudeTierArea_h_
#define _AmplitudeTierArea_h_
/* AmplitudeTierArea.h
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
#include "AmplitudeTier.h"

Thing_define (AmplitudeTierArea, RealTierArea) {
	conststring32 v_menuTitle ()
		override { return U"AmplitudeTier"; }
	conststring32 v_rightTickUnits ()
		override { return U" Pa"; }
	conststring32 v_quantityText ()
		override { return U"Sound pressure (Pa)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum amplitude (Pa)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum amplitude (Pa)"; }

	#include "AmplitudeTierArea_prefs.h"
};

DEFINE_FunctionArea_create (AmplitudeTierArea, AmplitudeTier)

/* End of file AmplitudeTierArea.h */
#endif
