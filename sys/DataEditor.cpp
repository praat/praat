/* DataEditor.cpp
 *
 * Copyright (C) 1995-2022 Paul Boersma
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

#define NAME_X  30
#define TEXT_X  250
#define BUTTON_X  250
#define LIST_Y  (2 * Gui_TOP_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT)
#define EDITOR_WIDTH  820
#define EDITOR_HEIGHT  (Machine_getMenuBarBottom () + LIST_Y + kDataSubEditor_MAXNUM_ROWS * ROW_HEIGHT + 29)
#define ROW_HEIGHT  31

#define SCROLL_BAR_WIDTH  Machine_getScrollBarWidth ()

#include "DataEditor.h"
#include "EditorM.h"
#include "Collection.h"
#include "machine.h"

static Data_Description Class_getDescription (ClassInfo table) {
	return ((Daata) _Thing_dummyObject (table)) -> v_description ();
}

static void VectorEditor_create (DataEditor root, conststring32 title, void *address,
	Data_Description description, integer minimum, integer maximum);

static void MatrixEditor_create (DataEditor root, conststring32 title, void *address,
	Data_Description description, integer min1, integer max1, integer min2, integer max2);

static void StructEditor_create (DataEditor root, conststring32 title, void *address, Data_Description description);

static void ClassEditor_create (DataEditor root, conststring32 title, void *address, Data_Description description);

static inline conststring32 strip_d (conststring32 s) {
	return s && s [0] == U'd' && s [1] == U'_' ? & s [2] : & s [0];
}

/********** DataSubEditor **********/

Thing_implement (DataSubEditor, Editor, 0);

void structDataSubEditor :: v9_destroy () noexcept {
	//for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++)
	//	Melder_free (d_fieldData [i]. history);
	if (our root)
		for (integer i = our root -> children.size; i > 0; i --)
			if (our root -> children.at [i] == this)
				our root -> children. subtractItem_ref (i);
	DataSubEditor_Parent :: v9_destroy ();
}

static void update (DataSubEditor me) {

	/* Hide all the existing widgets. */

	for (integer i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++) {
		my d_fieldData [i]. address = nullptr;
		my d_fieldData [i]. description = nullptr;
		GuiThing_hide (my d_fieldData [i]. label);
		GuiThing_hide (my d_fieldData [i]. button);
		GuiThing_hide (my d_fieldData [i]. text);
	}

	my d_irow = 0;
	my v_showMembers ();
}

static Data_Description DataSubEditor_findNumberUse (DataSubEditor me, conststring32 number) {
	Data_Description structDescription, result;
	char32 string [100];
	if (my classInfo == classMatrixEditor)
		return nullptr;   // no structs inside
	if (my classInfo == classVectorEditor) {
		if (my d_description -> type != structwa)
			return nullptr;   // no structs inside
		structDescription = * (Data_Description *) my d_description -> tagType;
	} else { /* StructEditor or ClassEditor or DataEditor. */
		structDescription = my d_description;
	}
	Melder_sprint (string,100, number);
	if ((result = Data_Description_findNumberUse (structDescription, string)) != nullptr)
		return result;
	Melder_sprint (string,100, number, U" - 1");
	if ((result = Data_Description_findNumberUse (structDescription, string)) != nullptr)
		return result;
	return nullptr;
}

static void gui_button_cb_change (DataSubEditor me, GuiButtonEvent /* event */) {
	int irow = 1;
	for (; irow <= kDataSubEditor_MAXNUM_ROWS; irow ++) {
		#if motif
			const bool visible = XtIsManaged (my d_fieldData [irow]. text -> d_widget);
		#elif gtk
			gboolean visible;
			g_object_get (G_OBJECT (my d_fieldData [irow]. text -> d_widget), "visible", & visible, nullptr);
		#elif defined (macintosh)
			const bool visible = ! [(GuiCocoaTextField *) my d_fieldData [irow]. text -> d_widget   isHidden];
		#else
			const bool visible = false;
		#endif
		if (visible) {
			int type = my d_fieldData [irow]. description -> type;
			if (type > maxsingletypewa)
				continue;
			autostring32 text = GuiText_getString (my d_fieldData [irow]. text);
			switch (type) {
				case bytewa: {
					signed char oldValue = * (signed char *) my d_fieldData [irow]. address, newValue = (signed char) Melder_atoi (text.get());
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse)
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						else
							* (signed char *) my d_fieldData [irow]. address = newValue;
					}
				} break;
				case int16wa: {
					int16 oldValue = * (int16 *) my d_fieldData [irow]. address;
					int64 newValue = Melder_atoi (text.get());
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse)
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						else if (newValue < INT16_MIN || newValue > INT16_MAX)
							Melder_flushError (U"Field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" can have no values less than ", INT16_MIN, U" or greater than ", INT16_MAX, U".");
						else
							* (int16 *) my d_fieldData [irow]. address = (int16) newValue;   // guarded conversion
					}
				} break;
				case intwa: {
					int oldValue = * (int *) my d_fieldData [irow]. address, newValue = (int) Melder_atoi (text.get());
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse)
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						else
							* (int *) my d_fieldData [irow]. address = newValue;
					}
				} break;
				case integerwa: {
					integer oldValue = * (integer *) my d_fieldData [irow]. address, newValue = Melder_atoi (text.get());
					if (newValue != oldValue) {
						Data_Description numberUse = DataSubEditor_findNumberUse (me, my d_fieldData [irow]. description -> name);
						if (numberUse)
							Melder_flushError (U"Changing field \"", strip_d (my d_fieldData [irow]. description -> name),
								U"\" would damage the array \"", strip_d (numberUse -> name), U"\".");
						else
							* (integer *) my d_fieldData [irow]. address = newValue;
					}
				} break;
				case ubytewa: {
					* (unsigned char *) my d_fieldData [irow]. address = (uint8) Melder_atoi (text.get());
				} break;
				case uintwa: {
					* (unsigned int *) my d_fieldData [irow]. address = (uint32) Melder_atoi (text.get());
				} break;
				case uintegerwa: {
					* (uinteger *) my d_fieldData [irow]. address = (uinteger) Melder_atoi (text.get());
				} break;
				case floatwa: {
					* (double *) my d_fieldData [irow]. address = Melder_atof (text.get());
				} break;
				case doublewa: {
					* (double *) my d_fieldData [irow]. address = Melder_atof (text.get());
				} break;
				case complexwa: { dcomplex *x = (dcomplex *) my d_fieldData [irow]. address;
					double re, im;
					sscanf (Melder_peek32to8 (text.get()), "%lf + %lf i", & re, & im);
					x -> real (re);
					x -> imag (im);
				} break;
				case enumwa: {
					if (Melder_length (text.get()) < 3)
						goto error;
					text [Melder_length (text.get()) - 1] = U'\0';   // remove trailing ">"
					int value = ((int (*) (conststring32)) (my d_fieldData [irow]. description -> tagType)) (text.get() + 1);   // skip leading "<"
					if (value < 0)
						goto error;
					* (signed char *) my d_fieldData [irow]. address = (signed char) value;
				} break;
				case lenumwa: {
					if (Melder_length (text.get()) < 3)
						goto error;
					text [Melder_length (text.get()) - 1] = U'\0';   // remove trailing ">"
					int value = ((int (*) (conststring32)) (my d_fieldData [irow]. description -> tagType)) (text.get() + 1);   // skip leading "<"
					if (value < 0)
						goto error;
					* (signed short *) my d_fieldData [irow]. address = (signed short) value;
				} break;
				case booleanwa: {
					bool value;
					if (str32nequ (text.get(), U"<true>", 6))
						value = true;
					else if (str32nequ (text.get(), U"<false>", 7))
						value = false;
					else
						goto error;
					* (bool *) my d_fieldData [irow]. address = value;
				} break;
				case questionwa: {
					bool value;
					if (str32nequ (text.get(), U"<yes>", 5))
						value = true;
					else if (str32nequ (text.get(), U"<no>", 4))
						value = false;
					else
						goto error;
					* (bool *) my d_fieldData [irow]. address = value;
				} break;
				case stringwa:
				case lstringwa: {
					char32 *old = * (char32 **) my d_fieldData [irow]. address;
					Melder_free (old);
					* (char32 **) my d_fieldData [irow]. address = Melder_dup_f (text.get()).transfer();
				} break;
				default: break;
			}
		}
	}
	/*
		Several collaborators have to be notified of this change:
		1. The owner (creator) of our root DataEditor: so that she can notify other editors, if any.
		2. All our sibling DataSubEditors.
	*/
	Melder_assert (my root);
	Editor_broadcastDataChanged (my root);
	Melder_assert (my root);
	update (me);
	Melder_assert (my root);
	for (int isub = 1; isub <= my root -> children.size; isub ++) {
		DataSubEditor subeditor = my root -> children.at [isub];
		if (subeditor != me)
			update (subeditor);
	}
	return;
error:
	Melder_flushError (U"Edit field \"", strip_d (my d_fieldData [irow]. description -> name), U"\" or click \"Cancel\".");
}

static void gui_button_cb_cancel (DataSubEditor me, GuiButtonEvent /* event */) {
	update (me);
}

static void gui_cb_scroll (DataSubEditor me, GuiScrollBarEvent event) {
	my d_topField = GuiScrollBar_getValue (event -> scrollBar) + 1;
	update (me);
}

static void gui_button_cb_open (DataSubEditor me, GuiButtonEvent event) {
	integer ifield = 0;
	static MelderString name;
	MelderString_empty (& name);

	/* Identify the pressed button; it must be one of those created in the list. */

	for (integer i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++)
		if (my d_fieldData [i]. button == event -> button) {
			ifield = i;
			break;
		}
	Melder_assert (ifield != 0);

	/* Launch the appropriate subeditor. */

	DataSubEditor_FieldData fieldData = & my d_fieldData [ifield];
	if (! fieldData -> description) {
		Melder_casual (U"Not yet implemented.");
		return;   // not yet implemented
	}

	if (fieldData -> description -> rank == 1 || fieldData -> description -> rank == 3 || fieldData -> description -> rank < 0) {
		MelderString_append (& name, fieldData -> history.get(), U". ", strip_d (fieldData -> description -> name),
				U" [", fieldData -> minimum, U"..", fieldData -> maximum, U"]");
		VectorEditor_create (my root, name.string, fieldData -> address,
				fieldData -> description, fieldData -> minimum, fieldData -> maximum);
	} else if (fieldData -> description -> rank == 2) {
		MelderString_append (& name, fieldData -> history.get(), U". ", strip_d (fieldData -> description -> name),
				U" [", fieldData -> minimum, U"..", fieldData -> maximum, U"]");
		MelderString_append (& name, U" [", fieldData -> min2, U"..", fieldData -> max2, U"]");
		MatrixEditor_create (my root, name.string, fieldData -> address, fieldData -> description,
				fieldData -> minimum, fieldData -> maximum, fieldData -> min2, fieldData -> max2);
	} else if (fieldData -> description -> type == structwa) {
		MelderString_append (& name, fieldData -> history.get(), U". ", strip_d (fieldData -> description -> name));
		StructEditor_create (my root, name.string, fieldData -> address,
				* (Data_Description *) fieldData -> description -> tagType);
	} else if (fieldData -> description -> type == objectwa ||
	           fieldData -> description -> type == collectionofwa ||
			   fieldData -> description -> type == collectionwa) {
		MelderString_append (& name, fieldData -> history.get(), U". ", strip_d (fieldData -> description -> name));
		ClassEditor_create (my root, name.string, fieldData -> address,
				Class_getDescription ((ClassInfo) fieldData -> description -> tagType));
	} else /*if (fieldData -> description -> type == inheritwa)*/ {
		ClassEditor_create (my root, fieldData -> history.get(), fieldData -> address,
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
	int x = Gui_LEFT_DIALOG_SPACING, y = Gui_TOP_DIALOG_SPACING + Machine_getMenuBarBottom (), buttonWidth = 120;

	GuiButton_createShown (our windowForm, x, x + buttonWidth, y, y + Gui_PUSHBUTTON_HEIGHT,
			U"Change", gui_button_cb_change, this, 0);
	x += buttonWidth + Gui_HORIZONTAL_DIALOG_SPACING;
	GuiButton_createShown (our windowForm, x, x + buttonWidth, y, y + Gui_PUSHBUTTON_HEIGHT,
			U"Cancel", gui_button_cb_cancel, this, 0);

	y = Machine_getMenuBarBottom () + LIST_Y;
	d_scrollBar = GuiScrollBar_createShown (our windowForm,
		- SCROLL_BAR_WIDTH, 0, y, 0,
		0, d_numberOfFields, 0, d_numberOfFields < kDataSubEditor_MAXNUM_ROWS ? d_numberOfFields : kDataSubEditor_MAXNUM_ROWS, 1, kDataSubEditor_MAXNUM_ROWS - 1,
		gui_cb_scroll, this, 0
	);

	y += 10;
	for (int i = 1; i <= kDataSubEditor_MAXNUM_ROWS; i ++) {
		d_fieldData [i]. label = GuiLabel_create (our windowForm, 0, 200, y, y + Gui_TEXTFIELD_HEIGHT, U"label", 0);   // no fixed x value: sometimes indent
		d_fieldData [i]. button = GuiButton_create (our windowForm, BUTTON_X, BUTTON_X + buttonWidth, y, y + Gui_TEXTFIELD_HEIGHT,
				U"Open", gui_button_cb_open, this, 0);
		d_fieldData [i]. text = GuiText_create (our windowForm, TEXT_X, -30, y, y + Gui_TEXTFIELD_HEIGHT, 0);
		d_fieldData [i]. y = y;
		y += ROW_HEIGHT;
	}
}

static void menu_cb_help (DataSubEditor, EDITOR_ARGS) { Melder_help (U"Inspect"); }

void structDataSubEditor :: v_createMenuItems_help (EditorMenu menu) {
	DataSubEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"DataEditor help", '?', menu_cb_help);
}

static void DataSubEditor_init (DataSubEditor me, DataEditor root, conststring32 title, void *address, Data_Description description) {
	my root = root;
	if (me != root)
		root -> children.addItem_ref (me);
	my d_address = address;
	my d_description = description;
	my d_topField = 1;
	my d_numberOfFields = my v_countFields ();
	Editor_init (me, 0, 0, EDITOR_WIDTH, EDITOR_HEIGHT, title, nullptr);
	update (me);
}

/********** StructEditor **********/

Thing_implement (StructEditor, DataSubEditor, 0);

integer structStructEditor :: v_countFields () {
	return Data_Description_countMembers (d_description);
}

static conststring32 singleTypeToText (void *address, int type, void *tagType, MelderString *buffer) {
	switch (type) {
		case bytewa:     MelderString_append (buffer, Melder_integer  (* (signed char *)    address)); break;
		case int16wa:    MelderString_append (buffer, Melder_integer  (* (int16 *)          address)); break;
		case intwa:      MelderString_append (buffer, Melder_integer  (* (int *)            address)); break;
		case integerwa:  MelderString_append (buffer, Melder_integer  (* (integer *)        address)); break;
		case ubytewa:    MelderString_append (buffer, Melder_integer  (* (unsigned char *)  address)); break;
		case uintwa:     MelderString_append (buffer, Melder_integer  (* (unsigned int *)   address)); break;
		case uintegerwa: MelderString_append (buffer, Melder_integer  (* (uinteger *)       address)); break;
		case floatwa:    MelderString_append (buffer, Melder_single   (* (double *)         address)); break;
		case doublewa:   MelderString_append (buffer, Melder_double   (* (double *)         address)); break;
		case complexwa:  MelderString_append (buffer, Melder_dcomplex (* (dcomplex *)       address)); break;
		case enumwa:     MelderString_append (buffer, U"<", ((conststring32 (*) (int)) tagType) (* (signed char *)  address), U">"); break;
		case lenumwa:    MelderString_append (buffer, U"<", ((conststring32 (*) (int)) tagType) (* (signed short *) address), U">"); break;
		case booleanwa:  MelderString_append (buffer, * (bool *) address ? U"<true>" : U"<false>"); break;
		case questionwa: MelderString_append (buffer, * (bool *) address ? U"<yes>"  : U"<no>"   ); break;
		case stringwa:
		case lstringwa: {
			char32 *string = * (char32 **) address;
			if (! string) {
				MelderString_empty (buffer);   // convert null string to empty string
				return buffer -> string;
			}
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
	int type = memberDescription -> type, rank = memberDescription -> rank;
	bool isSingleType = ( type <= maxsingletypewa && rank == 0 );
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
			XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, 60, nullptr);   // TODO: change to GuiObject_size
		#endif
		autoMelderString buffer;
		conststring32 text = singleTypeToText (memberAddress, type, memberDescription -> tagType, & buffer);
		GuiText_setString (fieldData -> text, text);
		GuiThing_show (fieldData -> text);
		fieldData -> address = memberAddress;
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
	} else if (rank == 1) {
		void *arrayAddress = * (void **) memberAddress;
		integer minimum, maximum;
		if (! arrayAddress)
			return;   // no button for empty fields
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min1, & minimum);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & maximum);
		if (maximum < minimum)
			return;   // no button if no elements
		fieldData -> address = arrayAddress;   // indirect
		fieldData -> description = memberDescription;
		fieldData -> minimum = minimum;   // normally 1
		fieldData -> maximum = maximum;
		fieldData -> rank = 1;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank < 0) {
		/*
			This represents an in-line array.
		*/
		integer maximum;   /* But: capacity = - rank */
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & maximum);
		if (-- maximum < 0)
			return;   // subtract one for zero-based array; no button if no elements
		fieldData -> address = memberAddress;   /* Direct. */
		fieldData -> description = memberDescription;
		fieldData -> minimum = 0;   // in-line arrays start with index 0
		fieldData -> maximum = maximum;   // probably between -1 and capacity - 1
		fieldData -> rank = rank;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank == 3) {
		/*
			This represents an in-line set.
		*/
		fieldData -> address = memberAddress;   // direct
		fieldData -> description = memberDescription;
		fieldData -> minimum = str32equ (((conststring32 (*) (int)) memberDescription -> min1) (0), U"_") ? 1 : 0;
		fieldData -> maximum = ((int (*) (conststring32)) memberDescription -> max1) (U"\n");
		fieldData -> rank = rank;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (rank == 2) {
		constMAT mat = * (constMAT *) memberAddress;
		//Melder_casual (U"Showing matrix member with ", mat.nrow, U" rows and ", mat.ncol, U" columns.");
		if (NUMisEmpty (mat))
			return;   // no button for empty fields
		integer min1, max1, min2, max2;
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min1, & min1);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max1, & max1);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> min2, & min2);
		Data_Description_evaluateInteger (structAddress, structDescription,
			memberDescription -> max2, & max2);
		if (max1 < min1 || max2 < min2)
			return;   // no button if no elements
		fieldData -> address = memberAddress;   // direct
		fieldData -> description = memberDescription;
		fieldData -> minimum = min1;   // normally 1
		fieldData -> maximum = max1;
		fieldData -> min2 = min2;
		fieldData -> max2 = max2;
		fieldData -> rank = 2;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (type == structwa) {   // in-line struct
		fieldData -> address = memberAddress;   // direct
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (type == objectwa || type == collectionwa) {
		fieldData -> address = * (Daata *) memberAddress;   // indirect  // FIXME: not guaranteed for auto objects
		if (! fieldData -> address)
			return;   // no button if no object
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	} else if (type == collectionofwa) {
		fieldData -> address = (Daata) memberAddress;   // direct  // FIXME: not guaranteed for auto objects
		//Melder_casual (U"Daata ", Melder_pointer (fieldData -> address));
		//Melder_casual (U"Class ", ((Daata) fieldData -> address) -> classInfo -> className);
		if (! fieldData -> address)
			return;   // no button if no object
		fieldData -> description = memberDescription;
		fieldData -> rank = 0;
		fieldData -> history = Melder_dup_f (history);
		GuiThing_show (fieldData -> button);
	}
}

static void showStructMembers (DataSubEditor me, void *structAddress, Data_Description structDescription, int fromMember, char32 *history) {
	integer i = 1;
	Data_Description memberDescription = structDescription;
	for (; i < fromMember && memberDescription -> name != nullptr; i ++, memberDescription ++)
		(void) 0;
	for (; memberDescription -> name != nullptr; memberDescription ++) {
		if (++ my d_irow > kDataSubEditor_MAXNUM_ROWS)
			return;
		showStructMember (structAddress, structDescription, memberDescription, & my d_fieldData [my d_irow], history);
	}
}

void structStructEditor :: v_showMembers () {
	showStructMembers (this, our d_address, our d_description, our d_topField, our name.get());
}

static void StructEditor_init (StructEditor me, DataEditor root, conststring32 title, void *address, Data_Description description) {
	DataSubEditor_init (me, root, title, address, description);
}

static void StructEditor_create (DataEditor root, conststring32 title, void *address, Data_Description description) {
	try {
		autoStructEditor me = Thing_new (StructEditor);
		StructEditor_init (me.get(), root, title, address, description);
		return me.releaseToUser();
	} catch (MelderError) {
		Melder_throw (U"Struct inspector window not created.");
	}
}

/********** VectorEditor **********/

Thing_implement (VectorEditor, DataSubEditor, 0);

integer structVectorEditor :: v_countFields () {
	integer numberOfElements = d_maximum - d_minimum + 1;
	if (d_description -> type == structwa)
		return numberOfElements * (Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1);
	else
		return numberOfElements;
}

void structVectorEditor :: v_showMembers () {
	int type = our d_description -> type;
	bool isSingleType = ( type <= maxsingletypewa );
	integer elementSize = ( type == structwa ?
		Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1 : 1 );
	integer firstElement = d_minimum + (d_topField - 1) / elementSize;

	for (integer ielement = firstElement; ielement <= d_maximum; ielement ++) {
		int skip = ielement == firstElement ? (our d_topField - 1) % elementSize : 0;

		if (++ our d_irow > kDataSubEditor_MAXNUM_ROWS) return;
		DataSubEditor_FieldData fieldData = & our d_fieldData [d_irow];

		if (isSingleType) {
			/*
				TODO: assume the address refers to a constVEC;
				this is not yet true of Collections, and
				indirect has to be changed to direct in showStructMember()
			*/
			unsigned char *elementAddress = (unsigned char *) our d_address + (ielement - 1) * our d_description -> size;

			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label,
				Melder_cat (strip_d (our d_description -> name), U" [",
					( our d_description -> rank == 3 ? ((conststring32 (*) (int)) our d_description -> min1) (ielement) : Melder_integer (ielement) ),
					U"]"));
			GuiThing_show (fieldData -> label);

			autoMelderString buffer;
			conststring32 text = singleTypeToText (elementAddress, type, our d_description -> tagType, & buffer);
			#if motif
				XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, 60, nullptr);   // TODO: change to GuiObject_size
			#endif
			GuiText_setString (fieldData -> text, text);
			GuiThing_show (fieldData -> text);
			fieldData -> address = elementAddress;
			fieldData -> description = d_description;
		} else if (type == structwa) {
			/*
				TODO: assume the address refers to a constVEC;
				this is not yet true of Collections, and
				indirect has to be changed to direct in showStructMember()
			*/
			unsigned char *elementAddress = (unsigned char *) our d_address + (ielement - 1) * our d_description -> size;

			static MelderString history;
			MelderString_copy (& history, our name.get());

			/* Replace things like [1..100] by things like [19]. */

			if (history.string [history.length - 1] == ']') {
				char32 *openingBracket = str32rchr (history.string, U'[');
				Melder_assert (openingBracket != nullptr);
				* openingBracket = '\0';
				history.length = openingBracket - history.string;
			}
			MelderString_append (& history, U"[", ielement, U"]");

			if (skip) {
				our d_irow --;
			} else {
				GuiControl_move (fieldData -> label, 0, fieldData -> y);
				GuiLabel_setText (fieldData -> label,
					Melder_cat (strip_d (d_description -> name), U" [", ielement, U"]: ---------------------------"));
				GuiThing_show (fieldData -> label);
			}
			showStructMembers (this, elementAddress, * (Data_Description *) d_description -> tagType, skip, history.string);
		} else if (type == objectwa) {
			/*
				TODO: assume the address refers to the items of a Collection (?)
			*/
			unsigned char *elementAddress = (unsigned char *) our d_address + ielement * our d_description -> size;

			static MelderString history;
			MelderString_copy (& history, our name.get());
			if (history.string [history.length - 1] == U']') {
				char32 *openingBracket = str32rchr (history.string, U'[');
				Melder_assert (openingBracket != nullptr);
				* openingBracket = U'\0';
				history.length = openingBracket - history.string;
			}
			MelderString_append (& history, U"[", ielement, U"]");

			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label, Melder_cat (strip_d (our d_description -> name), U" [", ielement, U"]"));
			GuiThing_show (fieldData -> label);

			Daata object = * (Daata *) elementAddress;
			if (! object)
				return;   // no button if no object
			if (! Class_getDescription (object -> classInfo))
				return;   // no button if no description for this class
			fieldData -> address = object;
			fieldData -> description = Class_getDescription (object -> classInfo);
			fieldData -> rank = 0;
			fieldData -> history = Melder_dup_f (history.string);
			GuiThing_show (fieldData -> button);
		}
	}
}

static void VectorEditor_create (DataEditor root, conststring32 title, void *address,
	Data_Description description, integer minimum, integer maximum)
{
	try {
		autoVectorEditor me = Thing_new (VectorEditor);
		my d_minimum = minimum;
		my d_maximum = maximum;
		DataSubEditor_init (me.get(), root, title, address, description);
		return me.releaseToUser();
	} catch (MelderError) {
		Melder_throw (U"Vector inspector window not created.");
	}
}

/********** MatrixEditor **********/

Thing_implement (MatrixEditor, DataSubEditor, 0);

integer structMatrixEditor :: v_countFields () {
	integer numberOfElements = (d_maximum - d_minimum + 1) * (d_max2 - d_min2 + 1);
	if (d_description -> type == structwa)
		return numberOfElements * (Data_Description_countMembers (* (Data_Description *) d_description -> tagType) + 1);
	else
		return numberOfElements;
}

void structMatrixEditor :: v_showMembers () {
	int type = our d_description -> type;
	bool isSingleType = ( type <= maxsingletypewa );
	Melder_assert (isSingleType);   // allow no struct matrices
	integer rowSize = our d_max2 - our d_min2 + 1;
	constMAT mat = * (constMAT *) d_address;   // HACK: this could be a MAT or an INTMAT
	Melder_assert (rowSize == mat.ncol);   // HACK: this should work correctly even for an INTMAT
	integer firstRow = d_minimum + (d_topField - 1) / rowSize;
	integer firstColumn = d_min2 + (d_topField - 1 - (firstRow - d_minimum) * rowSize);

	for (integer irow = firstRow; irow <= d_maximum; irow ++)
	for (integer icolumn = irow == firstRow ? firstColumn : d_min2; icolumn <= d_max2; icolumn ++) {
		unsigned char *elementAddress = (unsigned char *) mat.cells + ((irow - 1) * rowSize + (icolumn - 1)) * d_description -> size;
			// not & mat [irow] [icol], because that HACK would not work for an INTMAT

		if (++ d_irow > kDataSubEditor_MAXNUM_ROWS)
			return;
		DataSubEditor_FieldData fieldData = & d_fieldData [d_irow];
		
		if (isSingleType) {
			GuiControl_move (fieldData -> label, 0, fieldData -> y);
			GuiLabel_setText (fieldData -> label, Melder_cat (strip_d (d_description -> name), U" [", irow, U"] [", icolumn, U"]"));
			GuiThing_show (fieldData -> label);

			autoMelderString buffer;
			conststring32 text = singleTypeToText (elementAddress, type, d_description -> tagType, & buffer);
			#if motif
				XtVaSetValues (fieldData -> text -> d_widget, XmNcolumns, 60, nullptr);   // TODO: change to GuiObject_size
			#endif
			GuiText_setString (fieldData -> text, text);
			GuiThing_show (fieldData -> text);
			fieldData -> address = elementAddress;
			fieldData -> description = d_description;
		}
	}
}

static void MatrixEditor_create (DataEditor root, conststring32 title, void *address,
	Data_Description description, integer min1, integer max1, integer min2, integer max2)
{
	try {
		autoMatrixEditor me = Thing_new (MatrixEditor);
		my d_minimum = min1;
		my d_maximum = max1;
		my d_min2 = min2;
		my d_max2 = max2;
		DataSubEditor_init (me.get(), root, title, address, description);
		return me.releaseToUser();
	} catch (MelderError) {
		Melder_throw (U"Matrix inspector window not created.");
	}
}

/********** ClassEditor **********/

Thing_implement (ClassEditor, StructEditor, 0);

static void ClassEditor_showMembers_recursive (ClassEditor me, ClassInfo klas) {
	ClassInfo parentClass = klas -> semanticParent;
	Data_Description description = Class_getDescription (klas);
	int classFieldsTraversed = 0;
	while (Class_getDescription (parentClass) == description)
		parentClass = parentClass -> semanticParent;
	if (parentClass != classDaata) {
		ClassEditor_showMembers_recursive (me, parentClass);
		classFieldsTraversed = Data_Description_countMembers (Class_getDescription (parentClass));
		//Melder_casual (U"ClassEditor_showMembers_recursive: classFieldsTraversed = ", classFieldsTraversed);
	}
	showStructMembers (me, my d_address, description, my d_irow ? 1 : my d_topField - classFieldsTraversed, my name.get());
}

void structClassEditor :: v_showMembers () {
	ClassEditor_showMembers_recursive (this, ((Daata) d_address) -> classInfo);
}

static void ClassEditor_init (ClassEditor me, DataEditor root, conststring32 title, void *address, Data_Description description) {
	if (! description)
		Melder_throw (U"Class ", Thing_className ((Thing) address), U" cannot be inspected.");
	StructEditor_init (me, root, title, address, description);
}

static void ClassEditor_create (DataEditor root, conststring32 title, void *address, Data_Description description) {
	try {
		autoClassEditor me = Thing_new (ClassEditor);
		ClassEditor_init (me.get(), root, title, address, description);
		return me.releaseToUser();
	} catch (MelderError) {
		Melder_throw (U"Class inspector window not created.");
	}
}

/********** DataEditor **********/

Thing_implement (DataEditor, ClassEditor, 0);

static void DataEditor_destroyAllChildren (DataEditor me) {
	/*
		To destroy all children, we travel them from youngest to oldest,
		because the array of children will change from under us:
	*/
	for (int i = my children.size; i >= 1; i --) {
		/*
			An optimization coming!
			
			Instead of
				DataSubEditor child = my children [i];
				forget (child);
			we isolate the child from the parent before destroying the child,
			so that the child won't try to remove the reference
			that the parent has to her.
			So first we make the parent forget the moribund child,
			which prevents a dangling pointer:
		*/
		DataSubEditor child = my children. subtractItem_ref (i);
		/*
			That was fast, because subtracting the last item involves no shifting
			of the remaining items.

			Second, we make the child forget the parent,
			so that the child won't try to remove the reference
			that the parent had to her (but no longer has):
		*/
		child -> root = nullptr;
		/*
			The child is now fully isolated, so we are ready to destroy her:
		*/
		forget (child);
		/*
			This procedure was an optimization because if we just destroyed each child,
			each child would remove itself from the array by (1) searching for itself
			and (2) shifting the remaining children, both of which have a complexity
			that is linear in the number of children. So we would end up with quadratic complexity,
			whereas the procedure that we use above has linear complexity.
			
			This linear complexity makes this procedure good enough for `v9_destroy()`
			(where obtaining linear complexity would have been easy anyway),
			and nice enough for `v1_dataChanged()`.
			
			Something to note is that this procedure doesn't care whether the autoCollection
			`children` owns its items or not.
		*/
	}
}

void structDataEditor :: v9_destroy () noexcept {
	DataEditor_destroyAllChildren (this);
	DataEditor_Parent :: v9_destroy ();
}

void structDataEditor :: v1_dataChanged (Editor sender) {
	if (this == sender)
		return;
	/*
		Someone *else* changed our data.
		We know that the top-level data is still accessible,
		so we update the top-level window to show the change:
	*/
	update (this);
	/*
		Changing the data may have changed any part of the *structure* of the data,
		so we do not know if the data visible in any of the subeditors is still valid.
		We follow the most straightforward solution, which is to simply close all the child windows,
		which guarantees the removal of all dangling visual representations:
	*/
	DataEditor_destroyAllChildren (this);
}

autoDataEditor DataEditor_create (conststring32 title, Daata data) {
	try {
		ClassInfo klas = data -> classInfo;
		if (Class_getDescription (klas) == nullptr)
			Melder_throw (U"Class ", klas -> className, U" cannot be inspected.");
		autoDataEditor me = Thing_new (DataEditor);
		ClassEditor_init (me.get(), me.get(), title, data, Class_getDescription (klas));
		return me;
	} catch (MelderError) {
		Melder_throw (U"Inspector window not created.");
	}
}

/* End of file DataEditor.cpp */
