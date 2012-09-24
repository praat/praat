#ifndef _DurationTierEditor_h_
#define _DurationTierEditor_h_
/* DurationTierEditor.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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
#include "DurationTier.h"
#include "Sound.h"

Thing_define (DurationTierEditor, RealTierEditor) {
	// overridden methods:
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_play (double tmin, double tmax);
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual const wchar_t * v_quantityText () { return L"Relative duration"; }
		virtual const wchar_t * v_quantityKey () { return L"Relative duration"; }
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.25; }
		virtual double v_defaultYmax () { return 3.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set duration range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.25"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"3.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum duration"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum duration"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum duration"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum duration"; }
};

DurationTierEditor DurationTierEditor_create (const wchar_t *title,
	DurationTier duration, Sound sound, bool ownSound);
/*
	'sound' may be NULL.
*/

/* End of file DurationTierEditor.h */
#endif
