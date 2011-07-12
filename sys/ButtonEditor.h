#ifndef _ButtonEditor_h_
#define _ButtonEditor_h_
/* ButtonEditor.h
 *
 * Copyright (C) 1996-2011 Paul Boersma
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
 * pb 2011/03/02
 */

#include "HyperPage.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (ButtonEditor);

ButtonEditor ButtonEditor_create (GuiObject parent);

#ifdef __cplusplus
	}

	struct structButtonEditor : public structHyperPage {
		int show;
		GuiObject button1, button2, button3, button4, button5;
	};
	#define ButtonEditor__methods(Klas) HyperPage__methods(Klas)
	Thing_declare2cpp (ButtonEditor, HyperPage);

#endif // __cplusplus

/* End of file ButtonEditor.h */
#endif
