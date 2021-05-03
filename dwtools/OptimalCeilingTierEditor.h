#ifndef _OptimalCeilingTierEditor_h_
#define _OptimalCeilingTierEditor_h_
/* OptimalCeilingTierEditor.h
 *
 * Copyright (C) 2015 David Weenink, 2017,2020 Paul Boersma
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
#include "OptimalCeilingTier.h"
#include "Sound.h"

Thing_define (OptimalCeilingTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	double v_defaultYmin ()
		override { return 4000.0; }
	double v_defaultYmax ()
		override { return 6000.0; }
};

Thing_define (OptimalCeilingTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime)
		override;
	conststring32 v_quantityText ()
		override { return U"Frequency (Hz)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set frequency range..."; }
	conststring32 v_defaultYminText ()
		override { return U"4000.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"6000.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum frequency (Hz)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum frequency (Hz)"; }
};

autoOptimalCeilingTierEditor OptimalCeilingTierEditor_create (conststring32 title,
	OptimalCeilingTier amplitude,
	Sound sound,   // may be null
	bool ownSound);

/* End of file OptimalCeilingTierEditor.h */
#endif
