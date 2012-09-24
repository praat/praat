#ifndef _PitchTierEditor_h_
#define _PitchTierEditor_h_
/* PitchTierEditor.h
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
#include "PitchTier.h"
#include "Sound.h"

Thing_define (PitchTierEditor, RealTierEditor) {
	// overridden methods:
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_play (double tmin, double tmax);
		virtual double v_minimumLegalValue () { return 0.0; }
		virtual const wchar_t * v_quantityText () { return L"Frequency (Hz)"; }
		virtual const wchar_t * v_quantityKey () { return L"Frequency"; }
		virtual const wchar_t * v_rightTickUnits () { return L" Hz"; }
		virtual double v_defaultYmin () { return 50.0; }
		virtual double v_defaultYmax () { return 600.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set frequency range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"50.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"600.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum frequency (Hz)"; }
		virtual const wchar_t * v_ymaxText () { return L"Maximum frequency (Hz)"; }
		virtual const wchar_t * v_yminKey () { return L"Minimum frequency"; }
		virtual const wchar_t * v_ymaxKey () { return L"Maximum frequency"; }
};

PitchTierEditor PitchTierEditor_create (const wchar_t *title,
	PitchTier pitch,
	Sound sound,   // may be NULL
	bool ownSound);

/* End of file PitchTierEditor.h */
#endif
