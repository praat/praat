#ifndef _IntensityTierEditor_h_
#define _IntensityTierEditor_h_
/* IntensityTierEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015-2018,2020 Paul Boersma
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

#include "RealTierEditor.h"
#include "IntensityTier.h"
#include "Sound.h"

Thing_define (IntensityTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" dB"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 100.0; }
};

inline autoIntensityTierArea IntensityTierArea_create (FunctionEditor editor, double ymin_fraction, double ymax_fraction) {
	autoIntensityTierArea me = Thing_new (IntensityTierArea);
	FunctionArea_init (me.get(), editor, ymin_fraction, ymax_fraction);
	return me;
}

Thing_define (IntensityTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double tmin, double tmax)
		override;
	conststring32 v_quantityText ()
		override { return U"Intensity (dB)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set intensity range..."; }
	conststring32 v_defaultYminText ()
		override { return U"50.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"100.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum intensity (dB)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum intensity (dB)"; }
};

autoIntensityTierEditor IntensityTierEditor_create (conststring32 title,
	IntensityTier intensity, Sound sound, bool ownSound);
/*
	'sound' may be null.
*/

/* End of file IntensityTierEditor.h */
#endif
