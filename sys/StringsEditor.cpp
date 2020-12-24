/* StringsEditor.cpp
 *
 * Copyright (C) 2007-2012,2015-2020 Paul Boersma
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

#include "StringsEditor.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (StringsEditor, Editor, 0);

void structStringsEditor :: v_destroy () noexcept {
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
	for (integer i = 1; i <= strings -> numberOfStrings; i ++)
		GuiList_insertItem (my list, strings -> strings [i].get(), 0);
}

static void gui_button_cb_insert (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	/*
		Find the first selected item.
	*/
	autoINTVEC selected = GuiList_getSelectedPositions (my list);
	integer position = ( selected.size >= 1 ? selected [1] : strings -> numberOfStrings + 1 );
	autostring32 text = GuiText_getString (my text);
	/*
		Change the data.
	*/
	Strings_insert (strings, position, text.get());
	/*
		Change the list.
	*/
	GuiList_insertItem (my list, text.get(), position);
	GuiList_deselectAllItems (my list);
	GuiList_selectItem (my list, position);
	/*
		Clean up.
	*/
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_append (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	autostring32 text = GuiText_getString (my text);
	/*
		Change the data.
	*/
	Strings_insert (strings, 0, text.get());
	/*
		Change the list.
	*/
	GuiList_insertItem (my list, text.get(), 0);
	GuiList_deselectAllItems (my list);
	GuiList_selectItem (my list, strings -> numberOfStrings);
	/*
		Clean up.
	*/
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_remove (StringsEditor me, GuiButtonEvent /* event */) {
	autoINTVEC selected = GuiList_getSelectedPositions (my list);
	for (integer iselected = selected.size; iselected >= 1; iselected --)
		Strings_remove ((Strings) my data, selected [iselected]);
	updateList (me);
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_replace (StringsEditor me, GuiButtonEvent /* event */) {
	Strings strings = (Strings) my data;
	autoINTVEC selected = GuiList_getSelectedPositions (my list);
	autostring32 text = GuiText_getString (my text);
	for (integer iselected = 1; iselected <= selected.size; iselected ++) {
		Strings_replace (strings, selected [iselected], text.get());
		GuiList_replaceItem (my list, text.get(), selected [iselected]);
	}
	Editor_broadcastDataChanged (me);
}

static void gui_list_cb_doubleClick (StringsEditor me, GuiList_DoubleClickEvent /* event */) {
	Strings strings = (Strings) my data;
	//if (event -> position <= strings -> numberOfStrings)   // FIXME
	//	GuiText_setString (my text, strings -> strings [event -> position]);
}

void structStringsEditor :: v_createChildren () {
	list = GuiList_create (our windowForm, 1, 0, Machine_getMenuBarHeight (), -70, true, nullptr);
	GuiList_setDoubleClickCallback (list, gui_list_cb_doubleClick, this);
	GuiThing_show (list);

	text = GuiText_createShown (our windowForm, 0, 0, -40 - Gui_TEXTFIELD_HEIGHT, -40, 0);
	GuiButton_createShown (our windowForm, 10, 100, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Insert", gui_button_cb_insert, this, GuiButton_DEFAULT);
	GuiButton_createShown (our windowForm, 110, 200, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Append", gui_button_cb_append, this, 0);
	GuiButton_createShown (our windowForm, 210, 300, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Replace", gui_button_cb_replace, this, 0);
	GuiButton_createShown (our windowForm, 310, 400, -10 - Gui_PUSHBUTTON_HEIGHT, -10, U"Remove", gui_button_cb_remove, this, 0);
}

void structStringsEditor :: v_dataChanged () {
	updateList (this);
}

autoStringsEditor StringsEditor_create (conststring32 title, Strings data) {
	try {
		autoStringsEditor me = Thing_new (StringsEditor);
		Editor_init (me.get(), 20, 40, 600, 600, title, data);
		updateList (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings window not created.");
	}
}

/* End of file StringsEditor.cpp */
