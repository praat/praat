#ifndef _OptimalCeilingTierArea_h_
#define _OptimalCeilingTierArea_h_
/* OptimalCeilingTierArea.h
 *
 * Copyright (C) 2015 David Weenink, 2017,2020-2022 Paul Boersma
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
#include "OptimalCeilingTier.h"

Thing_define (OptimalCeilingTierArea, RealTierArea) {
	conststring32 v_menuTitle ()
		override { return U"OptimalCeilingTier"; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	conststring32 v_quantityText ()
		override { return U"Frequency (Hz)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum frequency (Hz)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum frequency (Hz)"; }

	#include "OptimalCeilingTierArea_prefs.h"
};

DEFINE_FunctionArea_create (OptimalCeilingTierArea, OptimalCeilingTier)

/* End of file OptimalCeilingTierArea.h */
#endif
