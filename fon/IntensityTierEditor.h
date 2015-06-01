#ifndef _IntensityTierEditor_h_
#define _IntensityTierEditor_h_
/* IntensityTierEditor.h
 *
 * Copyright (C) 1992-2011,2012,2015 Paul Boersma
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

#include "RealTierEditor.h"
#include "IntensityTier.h"
#include "Sound.h"

Thing_define (IntensityTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double tmin, double tmax)
		override;
	const wchar_t * v_quantityText ()
		override { return L"Intensity (dB)"; }
	const wchar_t * v_quantityKey ()
		override { return L"Intensity"; }
	const wchar_t * v_rightTickUnits ()
		override { return L" dB"; }
	double v_defaultYmin ()
		override { return 50.0; }
	double v_defaultYmax ()
		override { return 100.0; }
	const wchar_t * v_setRangeTitle ()
		override { return L"Set intensity range..."; }
	const wchar_t * v_defaultYminText ()
		override { return L"50.0"; }
	const wchar_t * v_defaultYmaxText ()
		override { return L"100.0"; }
	const wchar_t * v_yminText ()
		override { return L"Minimum intensity (dB)"; }
	const wchar_t * v_ymaxText ()
		override { return L"Maximum intensity (dB)"; }
	const wchar_t * v_yminKey ()
		override { return L"Minimum intensity"; }
	const wchar_t * v_ymaxKey ()
		override { return L"Maximum intensity"; }
};

IntensityTierEditor IntensityTierEditor_create (const wchar_t *title,
	IntensityTier intensity, Sound sound, bool ownSound);
/*
	'sound' may be NULL.
*/

/* End of file IntensityTierEditor.h */
#endif
