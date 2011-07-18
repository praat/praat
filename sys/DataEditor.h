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

Thing_declare1cpp (DataSubEditor);
Thing_declare1cpp (VectorEditor);
Thing_declare1cpp (MatrixEditor);
Thing_declare1cpp (StructEditor);
Thing_declare1cpp (ClassEditor);
Thing_declare1cpp (DataEditor);

typedef struct structDataSubEditor_FieldData {
	GuiObject label, button, text;
	void *address;
	Data_Description description;
	long minimum, maximum, min2, max2;
	wchar *history;   // the full prefix of the members
	int rank;   // should the button open a StructEditor (0) or VectorEditor (1) or MatrixEditor (2) ?
} *DataSubEditor_FieldData;

#define kDataSubEditor_MAXNUM_ROWS  12

struct structDataSubEditor : public structEditor {
	// new data:
		DataEditor root;
		void *address;
		Data_Description description;
		GuiObject scrollBar;
		int irow, topField, numberOfFields;
		struct structDataSubEditor_FieldData fieldData [1 + kDataSubEditor_MAXNUM_ROWS];
	// overridden methods:
		void v_destroy ();
		bool v_scriptable () { return false; }
		void v_createChildren ();
		void v_createHelpMenuItems (EditorMenu menu);
};
#define DataSubEditor__methods(Klas) \
	long (*countFields) (Klas me); \
	void (*showMembers) (Klas me);
Thing_declare2cpp (DataSubEditor, Editor);

struct structVectorEditor : public structDataSubEditor {
	long minimum, maximum;
};
#define VectorEditor__methods(Klas) DataSubEditor__methods(Klas)
Thing_declare2cpp (VectorEditor, DataSubEditor);

struct structMatrixEditor : public structDataSubEditor {
	long minimum, maximum, min2, max2;
};
#define MatrixEditor__methods(Klas) DataSubEditor__methods(Klas)
Thing_declare2cpp (MatrixEditor, DataSubEditor);

struct structStructEditor : public structDataSubEditor {
};
#define StructEditor__methods(Klas) DataSubEditor__methods(Klas)
Thing_declare2cpp (StructEditor, DataSubEditor);

struct structClassEditor : public structStructEditor {
};
#define ClassEditor__methods(Klas) StructEditor__methods(Klas)
Thing_declare2cpp (ClassEditor, StructEditor);

struct structDataEditor : public structClassEditor {
	// new data:
		Collection children;
	// overridden methods:
		void v_destroy ();
		void v_dataChanged ();
};
#define DataEditor__methods(Klas) ClassEditor__methods(Klas)
Thing_declare2cpp (DataEditor, ClassEditor);

DataEditor DataEditor_create (GuiObject parent, const wchar *title, Any data);

/* End of file DataEditor.h */
#endif
