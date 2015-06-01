#ifndef _AmplitudeTierEditor_h_
#define _AmplitudeTierEditor_h_
/* AmplitudeTierEditor.h
 *
 * Copyright (C) 2003-2011,2012,2014,2015 Paul Boersma
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
#include "AmplitudeTier.h"
#include "Sound.h"

Thing_define (AmplitudeTierEditor, RealTierEditor) {
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_play (double fromTime, double toTime)
		override;
	const wchar_t * v_quantityText ()
		override { return L"Sound pressure (Pa)"; }
	const wchar_t * v_quantityKey ()
		override { return L"Sound pressure"; }
	const wchar_t * v_rightTickUnits ()
		override { return L" Pa"; }
	double v_defaultYmin ()
		override { return -1.0; }
	double v_defaultYmax ()
		override { return +1.0; }
	const wchar_t * v_setRangeTitle ()
		override { return L"Set amplitude range..."; }
	const wchar_t * v_defaultYminText ()
		override { return L"-1.0"; }
	const wchar_t * v_defaultYmaxText ()
		override { return L"+1.0"; }
	const wchar_t * v_yminText ()
		override { return L"Minimum amplitude (Pa)"; }
	const wchar_t * v_ymaxText ()
		override { return L"Maximum amplitude (Pa)"; }
	const wchar_t * v_yminKey ()
		override { return L"Minimum amplitude"; }
	const wchar_t * v_ymaxKey ()
		override { return L"Maximum amplitude"; }
};

AmplitudeTierEditor AmplitudeTierEditor_create (const wchar_t *title,
	AmplitudeTier amplitude,
	Sound sound,   // may be NULL
	bool ownSound);

/* End of file AmplitudeTierEditor.h */
#endif
