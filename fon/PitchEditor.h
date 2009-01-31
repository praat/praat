#ifndef _PitchEditor_h_
#define _PitchEditor_h_
/* PitchEditor.h
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
#ifndef _Pitch_h_
	#include "Pitch.h"
#endif

#define PitchEditor__parents(Klas) FunctionEditor__parents(Klas) Thing_inherit (Klas, FunctionEditor)
Thing_declare1 (PitchEditor);

#define PitchEditor__members(Klas) FunctionEditor__members(Klas)
#define PitchEditor__methods(Klas) FunctionEditor__methods(Klas)
Thing_declare2 (PitchEditor, FunctionEditor);

PitchEditor PitchEditor_create (Widget parent, const wchar_t *title, Pitch pitch);

/* End of file PitchEditor.h */
#endif
