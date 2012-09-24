#ifndef _AmplitudeTierEditor_h_
#define _AmplitudeTierEditor_h_
/* AmplitudeTierEditor.h
 *
 * Copyright (C) 2003-2011,2012 Paul Boersma
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
	// overridden methods:
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_play (double tmin, double tmax);
		virtual const wchar_t *
			v_quantityText ()
				{ return L"Sound pressure (Pa)"; }
		virtual const wchar_t * v_quantityKey () { return L"Sound pressure"; }
		virtual const wchar_t * v_rightTickUnits () { return L" Pa"; }
		virtual double v_defaultYmin () { return -1.0; }
		virtual double v_defaultYmax () { return +1.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set amplitude range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"-1.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"+1.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum amplitude (Pa)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum amplitude (Pa)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum amplitude"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum amplitude"; }
};

AmplitudeTierEditor AmplitudeTierEditor_create (const wchar_t *title,
	AmplitudeTier amplitude,
	Sound sound,   // may be NULL
	bool ownSound);

/* End of file AmplitudeTierEditor.h */
#endif
