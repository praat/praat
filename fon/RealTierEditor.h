#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
 *
 * Copyright (C) 1992-2009 Paul Boersma
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

/*
 * pb 2009/01/23
 */

#ifndef _TimeSoundEditor_h_
	#include "TimeSoundEditor.h"
#endif
#ifndef _RealTier_h_
	#include "RealTier.h"
#endif

#define RealTierEditor__parents(Klas) TimeSoundEditor__parents(Klas) Thing_inherit (Klas, TimeSoundEditor)
Thing_declare1 (RealTierEditor);

#define RealTierEditor__members(Klas) TimeSoundEditor__members(Klas) \
	double ymin, ymax, ycursor;
#define RealTierEditor__methods(Klas) TimeSoundEditor__methods(Klas) \
	double minimumLegalValue, maximumLegalValue; \
	const wchar_t *quantityText, *quantityKey, *rightTickUnits; \
	double defaultYmin, defaultYmax; \
	const wchar_t *setRangeTitle, *defaultYminText, *defaultYmaxText; \
	const wchar_t *yminText, *ymaxText, *yminKey, *ymaxKey;
Thing_declare2 (RealTierEditor, TimeSoundEditor);

void RealTierEditor_updateScaling (RealTierEditor me);
/*
	Computes the ymin and ymax values on the basis of the data.
	Call after every change in the data.
*/

int RealTierEditor_init (RealTierEditor me, Widget parent, const wchar_t *title, RealTier data, Sound sound, int ownSound);
/*
	'Sound' may be NULL;
	if 'ownSound' is TRUE, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
