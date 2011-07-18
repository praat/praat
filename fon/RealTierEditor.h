#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

Thing_declare1cpp (RealTierEditor);
struct structRealTierEditor : structTimeSoundEditor {
	// new data:
		double ymin, ymax, ycursor;
	// overridden methods:
		void v_createMenus ();
		void v_dataChanged ();
};
#define RealTierEditor__methods(Klas) TimeSoundEditor__methods(Klas) \
	double minimumLegalValue, maximumLegalValue; \
	const wchar *quantityText, *quantityKey, *rightTickUnits; \
	double defaultYmin, defaultYmax; \
	const wchar *setRangeTitle, *defaultYminText, *defaultYmaxText; \
	const wchar *yminText, *ymaxText, *yminKey, *ymaxKey;
Thing_declare2cpp (RealTierEditor, TimeSoundEditor);

void RealTierEditor_updateScaling (RealTierEditor me);
/*
	Computes the ymin and ymax values on the basis of the data.
	Call after every change in the data.
*/

void RealTierEditor_init (RealTierEditor me, GuiObject parent, const wchar *title, RealTier data, Sound sound, bool ownSound);
/*
	'Sound' may be NULL;
	if 'ownSound' is TRUE, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
