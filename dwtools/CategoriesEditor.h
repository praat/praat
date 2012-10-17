#ifndef _CategoriesEditor_h_
#define _CategoriesEditor_h_
/* CategoriesEditor.h
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Command.h"
#include "Editor.h"
#include "Categories.h"

Thing_define (CategoriesEditor, Editor) {
	// new data:
	public:
		CommandHistory history;
		int position;
		GuiList list;
		GuiText text;
		GuiButton undo, redo;
		GuiLabel outOfView;
		GuiButton remove, insert, insertAtEnd, replace, moveUp, moveDown;
	// overridden methods:
		void v_destroy ();
		void v_createChildren ();
		void v_createHelpMenuItems (EditorMenu menu);
		void v_dataChanged ();
};

CategoriesEditor CategoriesEditor_create (const wchar_t *title, Categories data);

#endif /* _CategoriesEditor_h_ */