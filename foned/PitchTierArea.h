#ifndef _PitchTierArea_h_
#define _PitchTierArea_h_
/* PitchTierArea.h
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
#include "PitchTier.h"

Thing_define (PitchTierArea, RealTierArea) {
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }

	#include "PitchTierArea_prefs.h"
};

inline autoPitchTierArea PitchTierArea_create (FunctionEditor editor) {
	autoPitchTierArea me = Thing_new (PitchTierArea);
	RealTierArea_init (me.get(), editor);
	return me;
}

/* End of file PitchTierArea.h */
#endif
