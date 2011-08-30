#ifndef _DataEditor_h_
#define _DataEditor_h
/* DataEditor.h
 *
 * Copyright (C) 1995-2011 Paul Boersma
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

Thing_declare (DataSubEditor);
Thing_declare (VectorEditor);
Thing_declare (MatrixEditor);
Thing_declare (StructEditor);
Thing_declare (ClassEditor);
Thing_declare (DataEditor);

typedef struct structDataSubEditor_FieldData {
	GuiObject label, button, text;
	void *address;
	Data_Description description;
	long minimum, maximum, min2, max2;
	wchar *history;   // the full prefix of the members
	int rank;   // should the button open a StructEditor (0) or VectorEditor (1) or MatrixEditor (2) ?
} *DataSubEditor_FieldData;

#define kDataSubEditor_MAXNUM_ROWS  12

Thing_define (DataSubEditor, Editor) {
	// new data:
	public:
		DataEditor root;
		void *address;
		Data_Description description;
		GuiObject scrollBar;
		int irow, topField, numberOfFields;
		struct structDataSubEditor_FieldData fieldData [1 + kDataSubEditor_MAXNUM_ROWS];
	// overridden methods:
		virtual void v_destroy ();
		virtual bool v_scriptable () { return false; }
		virtual void v_createChildren ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
	// new methods:
		virtual long v_countFields () { return 0; }
		virtual void v_showMembers () { }
};

Thing_define (VectorEditor, DataSubEditor) {
	// new data:
	public:
		long minimum, maximum;
	// overridden methods:
		virtual long v_countFields ();
		virtual void v_showMembers ();
};

Thing_define (MatrixEditor, DataSubEditor) {
	// new data:
	public:
		long minimum, maximum, min2, max2;
	// overridden methods:
		virtual long v_countFields ();
		virtual void v_showMembers ();
};

Thing_define (StructEditor, DataSubEditor) {
	// overridden methods:
		virtual long v_countFields ();
		virtual void v_showMembers ();
};

Thing_define (ClassEditor, StructEditor) {
	// overridden methods:
		virtual void v_showMembers ();
};

Thing_define (DataEditor, ClassEditor) {
	// new data:
	public:
		Collection children;
	// overridden methods:
		void v_destroy ();
		void v_dataChanged ();
};

DataEditor DataEditor_create (GuiObject parent, const wchar *title, Any data);

/* End of file DataEditor.h */
#endif
