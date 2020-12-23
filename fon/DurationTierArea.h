#ifndef _DurationTierArea_h_
#define _DurationTierArea_h_
/* DurationTierArea.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2014-2018,2020 Paul Boersma
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
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U""; }
	double v_defaultYmin ()
		override { return 0.25; }
	double v_defaultYmax ()
		override { return 3.0; }

	#include "DurationTierArea_prefs.h"
};

inline autoDurationTierArea DurationTierArea_create (FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	autoDurationTierArea me = Thing_new (DurationTierArea);
	FunctionArea_init (me.get(), editor, ymin_fraction, ymax_fraction);
	return me;
}

/* End of file DurationTierArea.h */
#endif
