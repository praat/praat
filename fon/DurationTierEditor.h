#ifndef _DurationTierEditor_h_
#define _DurationTierEditor_h_
/* DurationTierEditor.h
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

#include "RealTierEditor.h"
#include "DurationTierArea.h"
#include "Sound.h"

Thing_define (DurationTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double fromTime, double toTime)
		override;
	conststring32 v_quantityText ()
		override { return U"Relative duration"; }
	conststring32 v_setRangeTitle ()
		override { return U"Set duration range..."; }
	conststring32 v_defaultYminText ()
		override { return U"0.25"; }
	conststring32 v_defaultYmaxText ()
		override { return U"3.0"; }
	conststring32 v_yminText ()
		override { return U"Minimum duration"; }
	conststring32 v_ymaxText ()
		override { return U"Maximum duration"; }
};

autoDurationTierEditor DurationTierEditor_create (conststring32 title,
	DurationTier duration, Sound sound, bool ownSound);
/*
	'sound' may be null.
*/

/* End of file DurationTierEditor.h */
#endif
