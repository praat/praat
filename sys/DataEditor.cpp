/* DataEditor.cpp
 *
 * Copyright (C) 1995-2012,2015 Paul Boersma
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

#define NAME_X  30
#define TEXT_X  250
#define BUTTON_X  250
#define LIST_Y  (2 * Gui_TOP_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT)
#define EDITOR_WIDTH  820
#define EDITOR_HEIGHT  (LIST_Y + kDataSubEditor_MAXNUM_ROWS * ROW_HEIGHT + 29 + Machine_getMenuBarHeight ())
#define ROW_HEIGHT  31

#define SCROLL_BAR_WIDTH  Machine_getScrollBarWidth ()

#include "DataEditor.h"
#include "EditorM.h"
#include "Collection.h"
#include "machine.h"

static Data_Description Class_getDescription (ClassInfo table) {
	return ((Data) _Thing_dummyObject (table)) -> v_description ();
}

/*static const char * typeStrings [] = { "none",
	"byte", "int", "long", "ubyte", "ushort", "uint", "ulong", "bool",
	"float", "double", "fcomplex", "dcomplex",
	"enum", "lenum", "boolean", "question", "stringw", "lstringw",
	"struct", "widget", "object", "collection" };*/
static int stringLengths [] = { 0,
	4, 6, 11, 3, 5, 10, 1,
	15, 27, 35, 59,
	33, 33, 8, 6, 60, 60 };

static VectorEditor VectorEditor_create (DataEditor root, const char32 *title, void *address,
	Data_Description description, long minimum, long maximum);

static MatrixEditor MatrixEditor_create (DataEditor root, const char32 *title, void *address,
	Data_Description description, long min1, long max1, long min2, long max2);

static StructEditor StructEditor_create (DataEditor root, const char32 *title, void *address, Data_Description description);

static ClassEditor ClassEditor_create (DataEditor root, const char32 *title, void *address, Data_Description description);

static inline const char32 * strip_d (const char32 *s) {
	return s && s [0] == U'd' && s [1] == U'_' ? & s [2] : & s [0];
}

/********** DataSubEditor **********/

Thing_implement (DataSubEditor, Editor, 0);

void structDataSubEditor :: v_destroy () {
	for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++)
		Melder_free (d_fieldData [i]. history);
	if (d_root && d_root -> d_children)
		for (int i = d_root -> d_children -> size; i > 0; i --)
			if (d_root -> d_children -> item [i] == this)
				Collection_subtractItem (d_root -> d_children, i);
	DataSubEditor_Parent :: v_destroy ();
}

static void update (DataSubEditor me) {

	/* Hide all the existing widgets. */

	for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++) {
		my d_fieldData [i]. address = NULL;
		my d_fieldData [i]. description = NULL;
		GuiThing_hide (my d_fieldData [i]. label);
		GuiThing_hide (my d_fieldData [i]. button);
		GuiThing_hide (my d_fieldData [i]. text);
	}

	my d_irow = 0;
	my v_showMembers ();
}

static Data_Description DataSubEditor_findNumberUse (DataSubEditor me, const char32 *number) {
	Data_Description structDescription, result;
	char32 string [100];
	if (my classInfo == classMatrixEditor) return NULL;   // no structs inside
	if (my classInfo == classVectorEditor) {
		if (my d_description -> type != structwa) return NULL;   // no structs inside
		structDescription = * (Data_Description *) my d_description -> tagType;
	} else { /* StructEditor or ClassEditor or DataEditor. */
		structDescription = my d_description;
	}
	Melder_sprint (string,100, number);
	if ((result = Data_Description_findNumberUse (structDescription, string)) != NULL) return result;
	Melder_sprint (string,100, number, U" - 1");
	if ((result = Data_Description_findNumberUse (structDescription, string)) != NULL) return result;
	return NULL;
}

static void gui_button_cb_change (I, GuiButtonEvent event) {
	(void) event;
	iam (DataSubEditor);
	int irow = 1;
	for (; irow <= kDataSubEditor_MAXNUM_ROWS; irow ++) {
		#if motif
			bool visible = XtIsManaged (my d_fieldData [irow]. text -> d_widget);
		#elif gtk
			gboolean visible;
			g_object_get (G_OBJECT (my d_fieldData [irow]. text), "visible", & visible, NULL);
		#elif defined (macintosh) && ! useCarbon
			bool visible = ! [(GuiCocoaTextField *) my d_fieldData [irow]. text -> d_widget   isHidden];
		#else
			bool visible = false;
		#endif
		if (visible) {
			int type = my d_fieldData [irow]. description -> type;
			char32 *text;
			if (type > maxsingletypewa) continue;
			text = GuiText_getString (my d_fieldData [irow]. text);
			switch (type) {
				case bytewa: {
					signed char oldValue = * (signed char *) my d_fieldData [irow]. address, newValue = (signed char) Melder_atoi (text);
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse) {
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						} else {
							* (signed char *) my d_fieldData [irow]. address = newValue;
						}
					}
				} break;
				case intwa: {
					int oldValue = * (int *) my d_fieldData [irow]. address, newValue = Melder_atoi (text);
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse) {
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						} else {
							* (int *) my d_fieldData [irow]. address = newValue;
						}
					}
				} break;
				case longwa: {
					long oldValue = * (long *) my d_fieldData [irow]. address, newValue = Melder_atoi (text);
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse) {
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						} else {
							* (long *) my d_fieldData [irow]. address = newValue;
						}
					}
				} break;
				case ubytewa: { * (unsigned char *) my d_fieldData [irow]. address = (uint8) Melder_atoi (text); } break;
				case uintwa: { * (unsigned int *) my d_fieldData [irow]. address = Melder_atoi (text); } break;
				case ulongwa: { * (unsigned long *) my d_fieldData [irow]. address = (unsigned long) Melder_atoi (text); } break;
				case boolwa: { * (bool *) my d_fieldData [irow]. address = Melder_atoi (text); } break;
				case floatwa: { * (double *) my d_fieldData [irow]. address = Melder_atof (text); } break;
				case doublewa: { * (double *) my d_fieldData [irow]. address = Melder_atof (text); } break;
				case fcomplexwa: { fcomplex *x = (fcomplex *) my d_fieldData [irow]. address;
					sscanf (Melder_peek32to8 (text), "%f + %f i", & x -> re, & x -> im); } break;
				case dcomplexwa: { dcomplex *x = (dcomplex *) my d_fieldData [irow]. address;
					sscanf (Melder_peek32to8 (text), "%lf + %lf i", & x -> re, & x -> im); } break;
				case enumwa: {
					if (str32len (text) < 3) goto error;
					text [str32len (text) - 1] = '\0';   // remove trailing ">"
					int value = ((int (*) (const char32 *)) (my d_fieldData [irow]. description -> tagType)) (text + 1);   // skip leading "<"
					if (value < 0) goto error;
					* (signed char *) my d_fieldData [irow]. address = (signed char) value;
				} break;
				case lenumwa: {
					if (str32len (text) < 3) goto error;
					text [str32len (text) - 1] = '\0';   // remove trailing ">"
					int value = ((int (*) (const char32 *)) (my d_fieldData [irow]. description -> tagType)) (text + 1);   // skip leading "<"
					if (value < 0) goto error;
					* (signed short *) my d_fieldData [irow]. address = (signed short) value;
				} break;
				case booleanwa: {
					int value = str32nequ (text, U"<true>", 6) ? 1 : str32nequ (text, U"<false>", 7) ? 0 : -1;
					if (value < 0) goto error;
					* (signed char *) my d_fieldData [irow]. address = (signed char) value;
				} break;
				case questionwa: {
					int value = str32nequ (text, U"<yes>", 5) ? 1 : str32nequ (text, U"<no>", 4) ? 0 : -1;
					if (value < 0) goto error;
					* (signed char *) my d_fieldData [irow]. address = (signed char) value;
				} break;
				case stringwa:
				case lstringwa: {
					char32 *old = * (char32 **) my d_fieldData [irow]. address;
					Melder_free (old);
					* (char32 **) my d_fieldData [irow]. address = Melder_dup_f (text);
				} break;
				default: break;
			}
			Melder_free (text);
		}
	}
	/* Several collaborators have to be notified of this change:
	 * 1. The owner (creator) of our root DataEditor: so that she can notify other editors, if any.
	 * 2. All our sibling DataSubEditors.
	 */
	Editor_broadcastDataChanged (my d_root);
	update (me);
	for (int isub = 1; isub <= my d_root -> d_children -> size; isub ++) {
		DataSubEditor subeditor = (DataSubEditor) my d_root -> d_children -> item [isub];
		if (subeditor != me) update (subeditor);
	}
	return;
error:
	Melder_flushError (U"Edit field \"", strip_d (my d_fieldData [irow]. description -> name), U"\" or click \"Cancel\".");
}

static void gui_button_cb_cancel (I, GuiButtonEvent event) {
	(void) event;
	iam (DataSubEditor);
	update (me);
}

static void gui_cb_scroll (I, GuiScrollBarEvent event) {
	iam (DataSubEditor);
	my d_topField = GuiScrollBar_getValue (event -> scrollBar) + 1;
	update (me);
}

static void gui_button_cb_open (I, GuiButtonEvent event) {
	iam (DataSubEditor);
	int ifield = 0;
	static MelderString name { 0 };
	MelderString_empty (& name);

	/* Identify the pressed button; it must be one of those created in the list. */

	for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++)
		if (my d_fieldData [i]. button == event -> button) { ifield = i; break; }
	Melder_assert (ifield != 0);

	/* Launch the appropriate subeditor. */

	DataSubEditor_FieldData fieldData = & my d_fieldData [ifield];
	if (! fieldData -> description) {
		Melder_casual (U"Not yet implemented.");
		return;   /* Not yet implemented. */
	}

	if (fieldData -> description -> rank == 1 || fieldData -> description -> rank == 3 || fieldData -> description -> rank < 0) {
		MelderString_append (& name, fieldData -> history, U". ", strip_d (fieldData -> description -> name),
			U" [", fieldData -> minimum, U"..", fieldData -> maximum, U"]");
		VectorEditor_create (my d_root, name.string, fieldData -> address,
			fieldData -> description, fieldData -> minimum, fieldData -> maximum);
	} else if (fieldData -> description -> rank == 2) {
		MelderString_append (& name, fieldData -> history, U". ", strip_d (fieldData -> description -> name),
			U" [", fieldData -> minimum, U"..", fieldData -> maximum, U"]");
		MelderString_append (& name, U" [", fieldData -> min2, U"..", fieldData -> max2, U"]");
		MatrixEditor_create (my d_root, name.string, fieldData -> address, fieldData -> description,
			fieldData -> minimum, fieldData -> maximum, fieldData -> min2, fieldData -> max2);
	} else if (fieldData -> description -> type == structwa) {
		MelderString_append (& name, fieldData -> history, U". ", strip_d (fieldData -> description -> name));
		StructEditor_create (my d_root, name.string, fieldData -> address,
			* (Data_Description *) fieldData -> description -> tagType);
	} else if (fieldData -> description -> type == objectwa || fieldData -> description -> type == collectionwa) {
		MelderString_append (& name, fieldData -> history, U". ", strip_d (fieldData -> description -> name));
		ClassEditor_create (my d_root, name.string, fieldData -> address,
			Class_getDescription ((ClassInfo) fieldData -> description -> tagType));
	} else /*if (fieldData -> description -> type == inheritwa)*/ {
		ClassEditor_create (my d_root, fieldData -> history, fieldData -> address,
			fieldData -> description);
/*	} else {
		Melder_casual (
			U"Strange editor \"", strip_d (fieldData -> description -> name),
			U"\" required (type ", fieldData -> description -> type,
			U", rank ", fieldData -> description -> rank,
			U")."
		);*/
	}
}

void structDataSubEditor :: v_createChildren () {
	int x = Gui_LEFT_DIALOG_SPACING, y = Gui_TOP_DIALOG_SPACING + Machine_getMenuBarHeight (), buttonWidth = 120;

	GuiButton_createShown (d_windowForm, x, x + buttonWidth, y, y + Gui_PUSHBUTTON_HEIGHT,
		U"Change", gui_button_cb_change, this, 0);
	x += buttonWidth + Gui_HORIZONTAL_DIALOG_SPACING;
	GuiButton_createShown (d_windowForm, x, x + buttonWidth, y, y + Gui_PUSHBUTTON_HEIGHT,
		U"Cancel", gui_button_cb_cancel, this, 0);

	y = LIST_Y + Machine_getMenuBarHeight ();
	d_scrollBar = GuiScrollBar_createShown (d_windowForm,
		- SCROLL_BAR_WIDTH, 0, y, 0,
		0, d_numberOfFields, 0, d_numberOfFields < kDataSubEditor_MAXNUM_ROWS ? d_numberOfFields : kDataSubEditor_MAXNUM_ROWS, 1, kDataSubEditor_MAXNUM_ROWS - 1,
		gui_cb_scroll, this, 0);

	y += 10;
	for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++) {
		d_fieldData [i]. label = GuiLabel_create (d_windowForm, 0, 200, y, y + Gui_TEXTFIELD_HEIGHT, U"label", 0);   // no fixed x value: sometimes indent
		d_fieldData [i]. button = GuiButton_create (d_windowForm, BUTTON_X, BUTTON_X + buttonWidth, y, y + Gui_TEXTFIELD_HEIGHT,
			U"Open", gui_button_cb_open, this, 0);
		d_fieldData [i]. text = GuiText_create (d_windowForm, TEXT_X, -30, y, y + Gui_TEXTFIELD_HEIGHT, 0);
		d_fieldData [i]. y = y;
		y += ROW_HEIGHT;
	}
}

static void menu_cb_help (EDITOR_ARGS) { EDITOR_IAM (DataSubEditor); Melder_help (U"Inspect"); }

void structDataSubEditor :: v_createHelpMenuItems (EditorMenu menu) {
	DataSubEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"DataEditor help", '?', menu_cb_help);
}

static void DataSubEditor_init (DataSubEditor me, DataEditor root, const char32 *title, void *address, Data_Description description) {
	my d_root = root;
	if (me != root) {
		Collection_addItem (root -> d_children, me);
	}
	my d_address = address;
	my d_description = description;
	my d_topField = 1;
	my d_numberOfFields = my v_countFields ();
	Editor_init (me, 0, 0, EDITOR_WIDTH, EDITOR_HEIGHT, title, NULL);
	update (me);
}

/********** StructEditor **********/

Thing_implement (StructEditor, DataSubEditor, 0);

long structStructEditor :: v_countFields () {
	return Data_Description_countMembers (d_description);
}

static const char32 * singleTypeToText (void *address, int type, void *tagType, MelderString *buffer) {
	switch (type) {
		case bytewa:   MelderString_append (buffer, Melder_integer (* (signed char *)    address)); break;
		case intwa:    MelderString_append (buffer, Melder_integer (* (int *)            address)); break;
		case longwa:   MelderString_append (buffer, Melder_integer (* (long *)           address)); break;
		case ubytewa:  MelderString_append (buffer, Melder_integer (* (unsigned char *)  address)); break;
		case uintwa:   MelderString_append (buffer, Melder_integer (* (unsigned int *)   address)); break;
		case ulongwa:  MelderString_append (buffer, Melder_integer (* (unsigned long *)  address)); break;
		case boolwa:   MelderString_append (buffer, Melder_integer (* (bool *)           address)); break;
		case floatwa:  MelderString_append (buffer, Melder_single  (* (double *)         address)); break;
		case doublewa: MelderString_append (buffer, Melder_double  (* (double *)         address)); break;
		case fcomplexwa: { fcomplex value = * (fcomplex *) address;
			MelderString_append (buffer, Melder_single (value. re), U" + ", Melder_single (value. im), U" i"); } break;
		case dcomplexwa: { dcomplex value = * (dcomplex *) address;
			MelderString_append (buffer, Melder_double (value. re), U" + ", Melder_double (value. im), U" i"); } break;
		case enumwa:  MelderString_append (buffer, U"<", ((const char32 * (*) (int)) tagType) (* (signed char *)  address), U">"); break;
		case lenumwa: MelderString_append (buffer, U"<", ((const char32 * (*) (int)) tagType) (* (signed short *) address), U">"); break;
		case booleanwa:  MelderString_append (buffer, * (signed char *) address ? U"<true>" : U"<false>"); break;
		case questionwa: MelderString_append (buffer, * (signed char *) address ? U"<yes>"  : U"<no>"   ); break;
		case stringwa:
		case lstringwa: {
			char32 *string = * (char32 **) address;
			if (string == NULL) { MelderString_empty (buffer); return buffer -> string; }   // convert NULL string to empty string
			return string;   // may be much longer than the usual size of 'buffer'
		} break;
		default: return U"(unknown)";
	}
	return buffer -> string;   // Mind the special return for strings above.
}

static void showStructMember (
	void *structAddress,   /* The address of (the first member of) the struct. */
	Data_Description structDescription,   /* The description of (the first member of) the struct. */
	Data_Description memberDescription,   /* The description of the current member. */
	DataSubEditor_FieldData fieldData,   /* The widgets in which to show the info about the current member. */
	char32 *history)
{
	int type = memberDescription -> type, rank = memberDescription -> rank, isSingleType = type <= maxsingletypewa && rank == 0;
	unsigned char *memberAddress = (unsigned char *) structAddress + memberDescription -> offset;
	if (type == inheritwa) {
		GuiLabel_setText (fieldData -> label,
			Melder_cat (U"Class part \"", memberDescription -> name, U"\":"));
	} else {
		GuiLabel_setText (fieldData -> label,
			Melder_cat (U"   ", strip_d (memberDescription -> name),
				( rank == 0 ? U"" : rank == 1 || rank == 3 || rank < 0 ? U" [ ]" : U" [ ] [ ]" )));
	}
	//GuiControl_move (fieldData -> label, type == inheritwa ? 0 : NAME_X, fieldData -> y);
	GuiThing_show (fieldData -> label);

	/* Show the value (for a single type) or a button (for a composite type). */
	if (isSingleType) {
		#if motif
			XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, stringLengths [type], NULL);   // TODO: change to GuiObject_size
		#endif
		autoMelderString buffer;
		const char32 *text = singleTypeToText (memberAddress, type, memberDescription -> tagType, & buffer);
		GuiText_setString (fieldData -> text, text);
		GuiThing_show (fieldData -> text);
		fieldData -> address = memberAddress;
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
	} else if (rank == 1) {
		void *arrayAddress = * (void **) memberAddress;
		long minimum, maximum;
		if (arrayAddress == NULL) return;   // no button for empty fields
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min1, & minimum);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & maximum);
		if (maximum < minimum) return;   // no button if no elements
		fieldData -> address = arrayAddress;   // indirect
		fieldData -> description = memberDescription;
		fieldData -> minimum = minimum;   // normally 1
		fieldData -> maximum = maximum;
		fieldData -> rank = 1;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank < 0) {
		/*
		 * This represents an in-line array.
		 */
		long maximum;   /* But: capacity = - rank */
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & maximum);
		if (-- maximum < 0) return;   /* Subtract one for zero-based array; no button if no elements. */
		fieldData -> address = memberAddress;   /* Direct. */
		fieldData -> description = memberDescription;
		fieldData -> minimum = 0;   /* In-line arrays start with index 0. */
		fieldData -> maximum = maximum;   /* Probably between -1 and capacity - 1. */
		fieldData -> rank = rank;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank == 3) {
		/*
		 * This represents an in-line set.
		 */
		fieldData -> address = memberAddress;   /* Direct. */
		fieldData -> description = memberDescription;
		fieldData -> minimum = str32equ (((const char32 * (*) (int)) memberDescription -> min1) (0), U"_") ? 1 : 0;
		fieldData -> maximum = ((int (*) (const char32 *)) memberDescription -> max1) (U"\n");
		fieldData -> rank = rank;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank == 2) {
		void *arrayAddress = * (void **) memberAddress;
		long min1, max1, min2, max2;
		if (arrayAddress == NULL) return;   // no button for empty fields
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min1,  & min1);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & max1);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min2,  & min2);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max2, & max2);
		if (max1 < min1 || max2 < min2) return;   // no button if no elements
		fieldData -> address = arrayAddress;   // indirect
		fieldData -> description = memberDescription;
		fieldData -> minimum = min1;   // normally 1
		fieldData -> maximum = max1;
		fieldData -> min2 = min2;
		fieldData -> max2 = max2;
		fieldData -> rank = 2;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (type == structwa) {   // in-line struct
		fieldData -> address = memberAddress;   // direct
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (type == objectwa || type == collectionwa) {
		fieldData -> address = * (Data *) memberAddress;   /* Indirect. */
		if (! fieldData -> address) return;   /* No button if no object. */
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
		Melder_free (fieldData -> history); fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	}
}

static void showStructMembers (DataSubEditor me, void *structAddress, Data_Description structDescription, int fromMember, char32 *history) {
	int i = 1;
	Data_Description memberDescription = structDescription;
	for (; i < fromMember && memberDescription -> name != NULL; i ++, memberDescription ++)
		(void) 0;
	for (; memberDescription -> name != NULL; memberDescription ++) {
		if (++ my d_irow > kDataSubEditor_MAXNUM_ROWS) return;
		showStructMember (structAddress, structDescription, memberDescription, & my d_fieldData [my d_irow], history);
	}
}

void structStructEditor :: v_showMembers () {
	showStructMembers (this, d_address, d_description, d_topField, name);
}

static void StructEditor_init (StructEditor me, DataEditor root, const char32 *title, void *address, Data_Description description) {
	DataSubEditor_init (me, root, title, address, description);
}

static StructEditor StructEditor_create (DataEditor root, const char32 *title, void *address, Data_Description description) {
	try {
		autoStructEditor me = Thing_new (StructEditor);
		StructEditor_init (me.peek(), root, title, address, description);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Struct inspector window not created.");
	}
}

/********** VectorEditor **********/

Thing_implement (VectorEditor, DataSubEditor, 0);

long structVectorEditor :: v_countFields () {
	long numberOfElements = d_maximum - d_minimum + 1;
	if (d_description -> type == structwa)
		return numberOfElements * (Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1);
	else
		return numberOfElements;
}

void structVectorEditor :: v_showMembers () {
	int type = d_description -> type, isSingleType = type <= maxsingletypewa;
	int elementSize = type == structwa ?
		Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1 : 1;
	long firstElement = d_minimum + (d_topField - 1) / elementSize;

	for (long ielement = firstElement; ielement <= d_maximum; ielement ++) {
		unsigned char *elementAddress = (unsigned char *) d_address + ielement * d_description -> size;
		int skip = ielement == firstElement ? (d_topField - 1) % elementSize : 0;

		if (++ d_irow > kDataSubEditor_MAXNUM_ROWS) return;
		DataSubEditor_FieldData fieldData = & d_fieldData [d_irow];

		if (isSingleType) {
			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label,
				Melder_cat (strip_d (d_description -> name), U" [",
					( d_description -> rank == 3 ? ((const char32 * (*) (int)) d_description -> min1) (ielement) : Melder_integer (ielement) ),
					U"]"));
			GuiThing_show (fieldData -> label);

			autoMelderString buffer;
			const char32 *text = singleTypeToText (elementAddress, type, d_description -> tagType, & buffer);
			#if motif
				XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, stringLengths [type], NULL);   // TODO: change to GuiObject_size
			#endif
			GuiText_setString (fieldData -> text, text);
			GuiThing_show (fieldData -> text);
			fieldData -> address = elementAddress;
			fieldData -> description = d_description;
		} else if (type == structwa) {
			static MelderString history { 0 };
			MelderString_copy (& history, name);

			/* Replace things like [1..100] by things like [19]. */

			if (history.string [history.length - 1] == ']') {
				char32 *openingBracket = str32rchr (history.string, U'[');
				Melder_assert (openingBracket != NULL);
				* openingBracket = '\0';
				history.length = openingBracket - history.string;
			}
			MelderString_append (& history, U"[", ielement, U"]");

			if (skip) {
				d_irow --;
			} else {
				GuiControl_move (fieldData -> label, 0, fieldData -> y);
				GuiLabel_setText (fieldData -> label,
					Melder_cat (strip_d (d_description -> name), U" [", ielement, U"]: ---------------------------"));
				GuiThing_show (fieldData -> label);
			}
			showStructMembers (this, elementAddress, * (Data_Description *) d_description -> tagType, skip, history.string);
		} else if (type == objectwa) {
			static MelderString history { 0 };
			MelderString_copy (& history, name);
			if (history.string [history.length - 1] == U']') {
				char32 *openingBracket = str32rchr (history.string, U'[');
				Melder_assert (openingBracket != NULL);
				* openingBracket = U'\0';
				history.length = openingBracket - history.string;
			}
			MelderString_append (& history, U"[", ielement, U"]");

			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label, Melder_cat (strip_d (d_description -> name), U" [", ielement, U"]"));
			GuiThing_show (fieldData -> label);

			Data object = * (Data *) elementAddress;
			if (object == NULL) return;   // no button if no object
			if (! Class_getDescription (object -> classInfo)) return;   // no button if no description for this class
			fieldData -> address = object;
			fieldData -> description = Class_getDescription (object -> classInfo);
			fieldData -> rank = 0;
			if (fieldData -> history) Melder_free (fieldData -> history);
			fieldData -> history = Melder_dup_f (history.string);
			GuiThing_show (fieldData -> button);
		}
	}
}

static VectorEditor VectorEditor_create (DataEditor root, const char32 *title, void *address,
	Data_Description description, long minimum, long maximum)
{
	try {
		autoVectorEditor me = Thing_new (VectorEditor);
		my d_minimum = minimum;
		my d_maximum = maximum;
		DataSubEditor_init (me.peek(), root, title, address, description);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Vector inspector window not created.");
	}
}

/********** MatrixEditor **********/

Thing_implement (MatrixEditor, DataSubEditor, 0);

long structMatrixEditor :: v_countFields () {
	long numberOfElements = (d_maximum - d_minimum + 1) * (d_max2 - d_min2 + 1);
	if (d_description -> type == structwa)
		return numberOfElements * (Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1);
	else
		return numberOfElements;
}

void structMatrixEditor :: v_showMembers () {
	int type = d_description -> type, isSingleType = type <= maxsingletypewa;
	int elementSize = type == structwa ?
		Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1 : 1;
	int rowSize = elementSize * (d_max2 - d_min2 + 1);
	long firstRow = d_minimum + (d_topField - 1) / rowSize;
	long firstColumn = d_min2 + (d_topField - 1 - (firstRow - d_minimum) * rowSize) / elementSize;

	for (long irow = firstRow; irow <= d_maximum; irow ++)
	for (long icolumn = irow == firstRow ? firstColumn : d_min2; icolumn <= d_max2; icolumn ++) {
		unsigned char *elementAddress = * ((unsigned char **) d_address + irow) + icolumn * d_description -> size;

		if (++ d_irow > kDataSubEditor_MAXNUM_ROWS) return;
		DataSubEditor_FieldData fieldData = & d_fieldData [d_irow];
		
		if (isSingleType) {
			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label, Melder_cat (strip_d (d_description -> name), U" [", irow, U"] [", icolumn, U"]"));
			GuiThing_show (fieldData -> label);

			autoMelderString buffer;
			const char32 *text = singleTypeToText (elementAddress, type, d_description -> tagType, & buffer);
			#if motif
				XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, stringLengths [type], NULL);   // TODO: change to GuiObject_size
			#endif
			GuiText_setString (fieldData -> text, text);
			GuiThing_show (fieldData -> text);
			fieldData -> address = elementAddress;
			fieldData -> description = d_description;
		}
	}
}

static MatrixEditor MatrixEditor_create (DataEditor root, const char32 *title, void *address,
	Data_Description description, long min1, long max1, long min2, long max2)
{
	try {
		autoMatrixEditor me = Thing_new (MatrixEditor);
		my d_minimum = min1;
		my d_maximum = max1;
		my d_min2 = min2;
		my d_max2 = max2;
		DataSubEditor_init (me.peek(), root, title, address, description);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Matrix inspector window not created.");
	}
}

/********** ClassEditor **********/

Thing_implement (ClassEditor, StructEditor, 0);

static void ClassEditor_showMembers_recursive (ClassEditor me, ClassInfo klas) {
	ClassInfo parentClass = klas -> parent;
	Data_Description description = Class_getDescription (klas);
	int classFieldsTraversed = 0;
	while (Class_getDescription (parentClass) == description)
		parentClass = parentClass -> parent;
	if (parentClass != classData) {
		ClassEditor_showMembers_recursive (me, parentClass);
		classFieldsTraversed = Data_Description_countMembers (Class_getDescription (parentClass));
		//Melder_casual (U"ClassEditor_showMembers_recursive: classFieldsTraversed = ", classFieldsTraversed);
	}
	showStructMembers (me, my d_address, description, my d_irow ? 1 : my d_topField - classFieldsTraversed, my name);
}

void structClassEditor :: v_showMembers () {
	ClassEditor_showMembers_recursive (this, ((Data) d_address) -> classInfo);
}

static void ClassEditor_init (ClassEditor me, DataEditor root, const char32 *title, void *address, Data_Description description) {
	if (description == NULL)
		Melder_throw (U"Class ", Thing_className ((Thing) address), U" cannot be inspected.");
	StructEditor_init (me, root, title, address, description);
}

static ClassEditor ClassEditor_create (DataEditor root, const char32 *title, void *address, Data_Description description) {
	try {
		autoClassEditor me = Thing_new (ClassEditor);
		ClassEditor_init (me.peek(), root, title, address, description);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Class inspector window not created.");
	}
}

/********** DataEditor **********/

Thing_implement (DataEditor, ClassEditor, 0);

void structDataEditor :: v_destroy () {

	/* Tell my children not to notify me when they die. */

	for (int i = 1; i <= d_children -> size; i ++) {
		DataSubEditor child = (DataSubEditor) d_children -> item [i];
		child -> d_root = NULL;
	}

	forget (d_children);
	DataEditor_Parent :: v_destroy ();
}

void structDataEditor :: v_dataChanged () {
	/*
	 * Someone else changed our data.
	 * We know that the top-level data is still accessible.
	 */
	update (this);
	/*
	 * But all structure may have changed,
	 * so that we do not know if any of the subeditors contain valid data.
	 */
	for (int i = d_children -> size; i >= 1; i --) {
		DataSubEditor subeditor = (DataSubEditor) d_children -> item [i];
		Collection_subtractItem (d_children, i);
		forget (subeditor);
	}
}

DataEditor DataEditor_create (const char32 *title, Data data) {
	try {
		ClassInfo klas = data -> classInfo;
		if (Class_getDescription (klas) == NULL)
			Melder_throw (U"Class ", klas -> className, U" cannot be inspected.");
		autoDataEditor me = Thing_new (DataEditor);
		my d_children = Collection_create (classDataSubEditor, 10);
		ClassEditor_init (me.peek(), me.peek(), title, data, Class_getDescription (klas));
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (U"Inspector window not created.");
	}
}

/* End of file DataEditor.cpp */
