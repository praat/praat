/* StringsEditor.cpp
 *
 * Copyright (C) 2007-2011,2015 Paul Boersma
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

#include "StringsEditor.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (StringsEditor, Editor, 0);

void structStringsEditor :: v_destroy () {
	StringsEditor_Parent :: v_destroy ();
}

static void menu_cb_help (StringsEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"StringsEditor");
}

void structStringsEditor :: v_createHelpMenuItems (EditorMenu menu) {
	StringsEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"StringsEditor help", U'?', menu_cb_help);
}

static void updateList (StringsEditor me) {
	Strings strings = (Strings) my data;
	GuiList_deleteAllItems (my list);
	for (long i = 1; i <= strings -> numberOfStrings; i ++)
		GuiList_insertItem (my list, strings -> strings [i], 0);
}

static void gui_button_cb_insert (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	/*
	 * Find the first selected item.
	 */
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	long position = selected ? selected [1] : strings -> numberOfStrings + 1;
	NUMvector_free (selected, 1);
	char32 *text = GuiText_getString (my text);
	/*
	 * Change the data.
	 */
	Strings_insert (strings, position, text);
	/*
	 * Change the list.
	 */
	GuiList_insertItem (my list, text, position);
	GuiList_deselectAllItems (my list);
	GuiList_selectItem (my list, position);
	/*
	 * Clean up.
	 */
	Melder_free (text);
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_append (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	char32 *text = GuiText_getString (my text);
	/*
	 * Change the data.
	 */
	Strings_insert (strings, 0, text);
	/*
	 * Change the list.
	 */
	GuiList_insertItem (my list, text, 0);
	GuiList_deselectAllItems (my list);
	GuiList_selectItem (my list, strings -> numberOfStrings);
	/*
	 * Clean up.
	 */
	Melder_free (text);
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_remove (StringsEditor me, GuiButtonEvent /* event */) {
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	for (long iselected = numberOfSelected; iselected >= 1; iselected --) {
		Strings_remove ((Strings) my data, selected [iselected]);
	}
	NUMvector_free (selected, 1);
	updateList (me);
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_replace (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	char32 *text = GuiText_getString (my text);
	for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
		Strings_replace (strings, selected [iselected], text);
		GuiList_replaceItem (my list, text, selected [iselected]);
	}
	Melder_free (text);
	Editor_broadcastDataChanged (me);
}

static void gui_list_cb_doubleClick (StringsEditor me, GuiList_DoubleClickEvent /* event */) {
	Strings strings = (Strings) my data;
	//if (event -> position <= strings -> numberOfStrings)   // FIXME
	//	GuiText_setString (my text, strings -> strings [event -> position]);
}

void structStringsEditor :: v_createChildren () {
	list = GuiList_create (d_windowForm, 1, 0, Machine_getMenuBarHeight (), -70, true, nullptr);
	GuiList_setDoubleClickCallback (list, gui_list_cb_doubleClick, this);
	GuiThing_show (list);

	text = GuiText_createShown (d_windowForm, 0, 0, -40 - Gui_TEXTFIELD_HEIGHT, -40, 0);
	GuiButton_createShown (d_windowForm, 10, 100, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Insert", gui_button_cb_insert, this, GuiButton_DEFAULT);
	GuiButton_createShown (d_windowForm, 110, 200, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Append", gui_button_cb_append, this, 0);
	GuiButton_createShown (d_windowForm, 210, 300, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Replace", gui_button_cb_replace, this, 0);
	GuiButton_createShown (d_windowForm, 310, 400, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Remove", gui_button_cb_remove, this, 0);
}

void structStringsEditor :: v_dataChanged () {
	updateList (this);
}

autoStringsEditor StringsEditor_create (const char32 *title, Strings data) {
	try {
		autoStringsEditor me = Thing_new (StringsEditor);
		Editor_init (me.peek(), 20, 40, 600, 600, title, data);
		updateList (me.peek());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings window not created.");
	}
}

/* End of file StringsEditor.cpp */
