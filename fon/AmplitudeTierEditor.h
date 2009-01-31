#ifndef _AmplitudeTierEditor_h_
#define _AmplitudeTierEditor_h_
/* AmplitudeTierEditor.h
 *
 * Copyright (C) 2003-2007 Paul Boersma
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

#ifndef _RealTierEditor_h_
	#include "RealTierEditor.h"
#endif
#ifndef _AmplitudeTier_h_
	#include "AmplitudeTier.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#define AmplitudeTierEditor__parents(Klas) RealTierEditor__parents(Klas) Thing_inherit (Klas, RealTierEditor)
Thing_declare1 (AmplitudeTierEditor);

#define AmplitudeTierEditor__members(Klas) RealTierEditor__members(Klas)
#define AmplitudeTierEditor__methods(Klas) RealTierEditor__methods(Klas)
Thing_declare2 (AmplitudeTierEditor, RealTierEditor);

AmplitudeTierEditor AmplitudeTierEditor_create (Widget parent, const wchar_t *title,
	AmplitudeTier amplitude, Sound sound, int ownSound);
/*
	'sound' may be NULL.
*/

/* End of file AmplitudeTierEditor.h */
#endif
