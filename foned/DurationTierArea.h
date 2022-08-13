#ifndef _DurationTierArea_h_
#define _DurationTierArea_h_
/* DurationTierArea.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2014-2018,2020-2022 Paul Boersma
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
#include "DurationTier.h"

Thing_define (DurationTierArea, RealTierArea) {
	DurationTier durationTier() { return static_cast <DurationTier> (our function()); }

	conststring32 v_menuTitle ()
		override { return U"DurationTier"; }
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	conststring32 v_quantityText ()
		override { return U"Relative duration"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set duration range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum duration"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum duration"; }

	#include "DurationTierArea_prefs.h"
	void v9_repairPreferences () override;
};

DEFINE_FunctionArea_create (DurationTierArea, DurationTier)

/* End of file DurationTierArea.h */
#endif
