#ifndef _DataEditor_h_
#define _DataEditor_h_
/* DataEditor.h
 *
 * Copyright (C) 1995-2011,2012,2015 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Editor.h"

Thing_declare (DataSubEditor);
Thing_declare (VectorEditor);
Thing_declare (MatrixEditor);
Thing_declare (StructEditor);
Thing_declare (ClassEditor);
Thing_declare (DataEditor);

typedef struct structDataSubEditor_FieldData {
	GuiLabel label;
	GuiButton button;
	GuiText text;
	void *address;
	Data_Description description;
	integer minimum, maximum, min2, max2;
	autostring32 history;   // the full prefix of the members
	int rank;   // should the button open a StructEditor (0) or VectorEditor (1) or MatrixEditor (2) ?
	int y;
} *DataSubEditor_FieldData;

#define kDataSubEditor_MAXNUM_ROWS  12

Thing_define (DataSubEditor, Editor) {
	DataEditor root;
	void *d_address;
	Data_Description d_description;
	GuiScrollBar d_scrollBar;
	int d_irow, d_topField, d_numberOfFields;
	struct structDataSubEditor_FieldData d_fieldData [1 + kDataSubEditor_MAXNUM_ROWS];

	void v_destroy () noexcept
		override;
	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;

	virtual integer v_countFields () { return 0; }
	virtual void v_showMembers () { }
};

Thing_define (VectorEditor, DataSubEditor) {
	integer d_minimum, d_maximum;

	integer v_countFields ()
		override;
	void v_showMembers ()
		override;
};

Thing_define (MatrixEditor, DataSubEditor) {
	integer d_minimum, d_maximum, d_min2, d_max2;

	integer v_countFields ()
		override;
	void v_showMembers ()
		override;
};

Thing_define (StructEditor, DataSubEditor) {
	integer v_countFields ()
		override;
	void v_showMembers ()
		override;
};

Thing_define (ClassEditor, StructEditor) {
	void v_showMembers ()
		override;
};

Thing_define (DataEditor, ClassEditor) {
	CollectionOf <structDataSubEditor> children;

	void v_destroy () noexcept
		override;
	void v_dataChanged ()
		override;
};

autoDataEditor DataEditor_create (conststring32 title, Daata data);

/* End of file DataEditor.h */
#endif
