#ifndef _IntensityTierArea_h_
#define _IntensityTierArea_h_
/* IntensityTierArea.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015-2018,2020,2021 Paul Boersma
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
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultMinimumValue ()
		override { return our pref_dataFreeMinimum(); }
	double v_defaultMaximumValue ()
		override { return our pref_dataFreeMaximum(); }

	#include "IntensityTierArea_prefs.h"
};

inline autoIntensityTierArea IntensityTierArea_create (FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	autoIntensityTierArea me = Thing_new (IntensityTierArea);
	Melder_casual(U"IntensityTierArea_create ", my ymin, U" ", my ymax);
	RealTierArea_init (me.get(), editor, ymin_fraction, ymax_fraction);
	Melder_casual(U"IntensityTierArea_create ", my ymin, U" ", my ymax);
	return me;
}

/* End of file IntensityTierArea.h */
#endif
