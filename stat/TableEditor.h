#ifndef _TableEditor_h_
#define _TableEditor_h_
/* TableEditor.h
 *
 * Copyright (C) 2006-2011,2012 Paul Boersma
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

Thing_define (TableEditor, Editor) {
	// new data:
	public:
		long topRow, leftColumn, selectedRow, selectedColumn;
		GuiText text;
		GuiDrawingArea drawingArea;
		GuiScrollBar horizontalScrollBar, verticalScrollBar;
		double columnLeft [kTableEditor_MAXNUM_VISIBLE_COLUMNS], columnRight [kTableEditor_MAXNUM_VISIBLE_COLUMNS];
		Graphics graphics;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_createChildren ();
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_dataChanged ();
	// new methods:
		virtual void v_draw ();
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
};

TableEditor TableEditor_create (const wchar_t *title, Table table);

/* End of file TableEditor.h */
#endif
