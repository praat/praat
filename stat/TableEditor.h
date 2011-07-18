#ifndef _TableEditor_h_
#define _TableEditor_h_
/* TableEditor.h
 *
 * Copyright (C) 2006-2011 Paul Boersma
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

#include "Editor.h"
#include "Table.h"

#define kTableEditor_MAXNUM_VISIBLE_COLUMNS  100

Thing_declare1cpp (TableEditor);
struct structTableEditor : public structEditor {
	// new data:
		long topRow, leftColumn, selectedRow, selectedColumn;
		GuiObject text, drawingArea, horizontalScrollBar, verticalScrollBar;
		double columnLeft [kTableEditor_MAXNUM_VISIBLE_COLUMNS], columnRight [kTableEditor_MAXNUM_VISIBLE_COLUMNS];
		Graphics graphics;
	// overridden methods:
		void v_destroy ();
		void v_createChildren ();
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
		void v_dataChanged ();
};
#define TableEditor__methods(Klas) \
	void (*draw) (Klas me); \
	int (*click) (Klas me, double xWC, double yWC, int shiftKeyPressed);
Thing_declare2cpp (TableEditor, Editor);

TableEditor TableEditor_create (GuiObject parent, const wchar_t *title, Table table);

/* End of file TableEditor.h */
#endif
