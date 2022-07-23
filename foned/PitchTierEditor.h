#ifndef _PitchTierEditor_h_
#define _PitchTierEditor_h_
/* PitchTierEditor.h
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

#include "RealTierEditor.h"
#include "PitchTierArea.h"
#include "Sound.h"

Thing_define (PitchTierEditor, RealTierEditor) {
	PitchTierArea pitchTierArea() { return static_cast <PitchTierArea> (our realTierArea.get()); }

	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double tmin, double tmax)
		override;
	conststring32 v_quantityText ()
		override { return U"Frequency (Hz)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	conststring32 v_minimumLabelText ()
		override { return U"Minimum frequency (Hz)"; }
	conststring32 v_maximumLabelText ()
		override { return U"Maximum frequency (Hz)"; }
};

autoPitchTierEditor PitchTierEditor_create (conststring32 title,
	PitchTier pitch,
	Sound sound   // will be copied; may be null
);

/* End of file PitchTierEditor.h */
#endif
