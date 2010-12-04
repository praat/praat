#ifndef _DataEditor_h_
#define _DataEditor_h
/* DataEditor.h
 *
 * Copyright (C) 1995-2007 Paul Boersma
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
 * pb 2007/06/09
 */

#ifndef _Editor_h_
	#include "Editor.h"
#endif

#define DataSubEditor__parents(Klas) Editor__parents(Klas) Thing_inherit (Klas, Editor)
Thing_declare1 (DataSubEditor);

#define VectorEditor__parents(Klas) DataSubEditor__parents(Klas) Thing_inherit (Klas, DataSubEditor)
Thing_declare1 (VectorEditor);

#define MatrixEditor__parents(Klas) DataSubEditor__parents(Klas) Thing_inherit (Klas, DataSubEditor)
Thing_declare1 (MatrixEditor);

#define StructEditor__parents(Klas) DataSubEditor__parents(Klas) Thing_inherit (Klas, DataSubEditor)
Thing_declare1 (StructEditor);

#define ClassEditor__parents(Klas) StructEditor__parents(Klas) Thing_inherit (Klas, StructEditor)
Thing_declare1 (ClassEditor);

#define DataEditor__parents(Klas) ClassEditor__parents(Klas) Thing_inherit (Klas, ClassEditor)
Thing_declare1 (DataEditor);

DataEditor DataEditor_create (GuiObject parent, const wchar_t *title, Any data);

/* End of file DataEditor.h */
#endif
