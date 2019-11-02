#ifndef _CategoriesEditor_h_
#define _CategoriesEditor_h_
/* CategoriesEditor.h
 *
 * Copyright (C) 1993-2018 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Command.h"
#include "Editor.h"
#include "Categories.h"

Thing_define (CategoriesEditor, Editor) {
	autoCommandHistory history;
	integer position;
	GuiList list;
	GuiText text;
	GuiButton undo, redo;
	GuiLabel outOfView;
	GuiButton remove, insert, insertAtEnd, replace, moveUp, moveDown;

	void v_destroy () noexcept
		override;
	void v_createChildren ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_dataChanged ()
		override;
};

autoCategoriesEditor CategoriesEditor_create (conststring32 title, Categories data);

#endif /* _CategoriesEditor_h_ */
