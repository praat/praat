#ifndef _AmplitudeTierEditor_h_
#define _AmplitudeTierEditor_h_
/* AmplitudeTierEditor.h
 *
 * Copyright (C) 2003-2005,2007,2009-2012,2014-2018,2020 Paul Boersma
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
#include "AmplitudeTier.h"
#include "Sound.h"

Thing_define (AmplitudeTierArea, RealTierArea) {
	conststring32 v_rightTickUnits ()
		override { return U" Pa"; }
	double v_defaultYmin ()
		override { return -1.0; }
	double v_defaultYmax ()
		override { return +1.0; }
};

Thing_define (AmplitudeTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double fromTime, double toTime)
		override;
	conststring32 v_quantityText ()
		override { return U"Sound pressure (Pa)"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set amplitude range..."; }
	conststring32 v_defaultYminText ()
		override { return U"-1.0"; }
	conststring32 v_defaultYmaxText ()
		override { return U"+1.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum amplitude (Pa)"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum amplitude (Pa)"; }
};

autoAmplitudeTierEditor AmplitudeTierEditor_create (conststring32 title,
	AmplitudeTier amplitude,
	Sound sound,   // may be null
	bool ownSound);

/* End of file AmplitudeTierEditor.h */
#endif
