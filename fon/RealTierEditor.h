#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/06/10
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _RealTier_h_
	#include "RealTier.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#define RealTierEditor_members FunctionEditor_members \
	int ownSound; \
	double ymin, ymax, ycursor;
#define RealTierEditor_methods FunctionEditor_methods \
	int zeroIsMinimum; \
	const wchar_t *quantityText, *quantityKey, *leftTickFormat, *rightTickFormat; \
	double defaultYmin, defaultYmax; \
	const wchar_t *setRangeTitle, *defaultYminText, *defaultYmaxText; \
	const wchar_t *yminText, *ymaxText, *yminKey, *ymaxKey;
class_create (RealTierEditor, FunctionEditor);

void RealTierEditor_updateScaling (I);
/*
	Computes the ymin and ymax values on the basis of the data.
	Call after every change in the data.
*/

int RealTierEditor_init (I, Widget parent, const wchar_t *title, RealTier data, Sound sound, int ownSound);
/*
	'Sound' may be NULL;
	if 'ownSound' is TRUE, the editor will contain a deep copy of the Sound,
	which the editor will destroy when the editor is destroyed.
*/

/* End of file RealTierEditor.h */
#endif
