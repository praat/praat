#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
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

#include "TimeSoundEditor.h"
#include "RealTier.h"

Thing_define (RealTierEditor, TimeSoundEditor) {
	// new data:
	public:
		double ymin, ymax, ycursor;
	// overridden methods:
		virtual void v_createMenus ();
		virtual void v_dataChanged ();
		virtual void v_draw ();
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_play (double tmin, double tmax);
		virtual void v_createMenuItems_view (EditorMenu menu);
	// new methods:
		virtual double v_minimumLegalValue () { return NUMundefined; }
		virtual double v_maximumLegalValue () { return NUMundefined; }
		virtual const wchar_t * v_quantityText () { return L"Y"; }   // normally includes units
		virtual const wchar_t * v_quantityKey () { return L"Y"; }   // without units
		virtual const wchar_t * v_rightTickUnits () { return L""; }
		virtual double v_defaultYmin () { return 0.0; }
		virtual double v_defaultYmax () { return 1.0; }
		virtual const wchar_t * v_setRangeTitle () { return L"Set range..."; }
		virtual const wchar_t * v_defaultYminText () { return L"0.0"; }
		virtual const wchar_t * v_defaultYmaxText () { return L"1.0"; }
		virtual const wchar_t * v_yminText () { return L"Minimum"; }   // normally includes units
		virtual const wchar_t * v_ymaxText () { return L"Maximum"; }   // normally includes units
		virtual const wchar_t * v_yminKey () { return L"Minimum"; }   // without units
		virtual const wchar_t * v_ymaxKey () { return L"Maximum"; }   // without units
};

void RealTierEditor_updateScaling (RealTierEditor me);
/*
	Computes the ymin and ymax values on the basis of the data.
	Call after every change in the data.
*/

void RealTierEditor_init (RealTierEditor me, const wchar_t *title, RealTier data, Sound sound, bool ownSound);
/*
	'Sound' may be NULL;
	if 'ownSound' is TRUE, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
