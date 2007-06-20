#ifndef _ArtwordEditor_h_
#define _ArtwordEditor_h
/* ArtwordEditor.h
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

#ifndef _Editor_h_
	#include "Editor.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _Artword_h_
	#include "Artword.h"
#endif

#define ArtwordEditor_members Editor_members \
	Graphics graphics; \
	int feature; \
	Widget list, drawingArea, radio, time, value; \
	Widget button [1 + enumlength (Art_MUSCLE)];
#define ArtwordEditor_methods Editor_methods
class_create (ArtwordEditor, Editor);

ArtwordEditor ArtwordEditor_create (Widget parent, const wchar_t *title, Artword data);

/* End of file ArtwordEditor.h */
#endif

