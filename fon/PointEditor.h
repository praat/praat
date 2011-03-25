#ifndef _PointEditor_h_
#define _PointEditor_h_
/* PointEditor.h
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

/*
 * pb 2011/03/23
 */

#include "TimeSoundEditor.h"
#include "PointProcess.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define PointEditor__parents(Klas) TimeSoundEditor__parents(Klas) Thing_inherit (Klas, TimeSoundEditor)
Thing_declare1 (PointEditor);

#define PointEditor__members(Klas) TimeSoundEditor__members(Klas) \
	Sound monoSound; \
	GuiObject addPointAtDialog;
#define PointEditor__methods(Klas) TimeSoundEditor__methods(Klas)
Thing_declare2 (PointEditor, TimeSoundEditor);

PointEditor PointEditor_create (GuiObject parent, const wchar_t *title,
	PointProcess point, Sound sound);
/*
	'sound' may be NULL.
*/

#ifdef __cplusplus
	}
#endif

/* End of file PointEditor.h */
#endif
