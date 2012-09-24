/* StringsEditor.cpp
 *
 * Copyright (C) 2007-2011 Paul Boersma
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

static void menu_cb_help (EDITOR_ARGS) {
	EDITOR_IAM (StringsEditor);
	(void) me;
	Melder_help (L"StringsEditor");
}

void structStringsEditor :: v_createHelpMenuItems (EditorMenu menu) {
	StringsEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"StringsEditor help", '?', menu_cb_help);
}

static void updateList (StringsEditor me) {
	Strings strings = (Strings) my data;
	my list -> f_deleteAllItems ();
	for (long i = 1; i <= strings -> numberOfStrings; i ++)
		my list -> f_insertItem (strings -> strings [i], 0);
}

static void gui_button_cb_insert (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	/*
	 * Find the first selected item.
	 */
	long numberOfSelected, *selected = my list -> f_getSelectedPositions (& numberOfSelected);
	long position = selected == NULL ? strings -> numberOfStrings + 1 : selected [1];
	NUMvector_free (selected, 1);
	wchar_t *text = my text -> f_getString ();
	/*
	 * Change the data.
	 */
	Strings_insert (strings, position, text);
	/*
	 * Change the list.
	 */
	my list -> f_insertItem (text, position);
	my list -> f_deselectAllItems ();
	my list -> f_selectItem (position);
	/*
	 * Clean up.
	 */
	Melder_free (text);
	my broadcastDataChanged ();
}

static void gui_button_cb_append (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	wchar_t *text = my text -> f_getString ();
	/*
	 * Change the data.
	 */
	Strings_insert (strings, 0, text);
	/*
	 * Change the list.
	 */
	my list -> f_insertItem (text, 0);
	my list -> f_deselectAllItems ();
	my list -> f_selectItem (strings -> numberOfStrings);
	/*
	 * Clean up.
	 */
	Melder_free (text);
	my broadcastDataChanged ();
}

static void gui_button_cb_remove (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	long numberOfSelected, *selected = my list -> f_getSelectedPositions (& numberOfSelected);
	for (long iselected = numberOfSelected; iselected >= 1; iselected --) {
		Strings_remove ((Strings) my data, selected [iselected]);
	}
	NUMvector_free (selected, 1);
	updateList (me);
	my broadcastDataChanged ();
}

static void gui_button_cb_replace (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	long numberOfSelected, *selected = my list -> f_getSelectedPositions (& numberOfSelected);
	wchar_t *text = my text -> f_getString ();
	for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
		Strings_replace (strings, selected [iselected], text);
		my list -> f_replaceItem (text, selected [iselected]);
	}
	Melder_free (text);
	my broadcastDataChanged ();
}

static void gui_list_cb_doubleClick (GuiObject widget, void *void_me, long item) {
	(void) widget;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	if (item <= strings -> numberOfStrings)
		my text -> f_setString (strings -> strings [item]);
}

void structStringsEditor :: v_createChildren () {
	list = GuiList_create (d_windowForm, 1, 0, Machine_getMenuBarHeight (), -70, true, NULL);
	//GuiList_setDoubleClickCallback (list, gui_list_cb_doubleClick, this);
	list -> f_show ();

	text = GuiText_createShown (d_windowForm, 0, 0, -40 - Gui_TEXTFIELD_HEIGHT, -40, 0);
	GuiButton_createShown (d_windowForm, 10, 100, -10 - Gui_PUSHBUTTON_HEIGHT, -10, L"Insert", gui_button_cb_insert, this, GuiButton_DEFAULT);
	GuiButton_createShown (d_windowForm, 110, 200, -10 - Gui_PUSHBUTTON_HEIGHT, -10, L"Append", gui_button_cb_append, this, 0);
	GuiButton_createShown (d_windowForm, 210, 300, -10 - Gui_PUSHBUTTON_HEIGHT, -10, L"Replace", gui_button_cb_replace, this, 0);
	GuiButton_createShown (d_windowForm, 310, 400, -10 - Gui_PUSHBUTTON_HEIGHT, -10, L"Remove", gui_button_cb_remove, this, 0);	
}

void structStringsEditor :: v_dataChanged () {
	updateList (this);
}

StringsEditor StringsEditor_create (const wchar_t *title, Strings data) {
	try {
		autoStringsEditor me = Thing_new (StringsEditor);
		Editor_init (me.peek(), 20, 40, 600, 600, title, data);
		updateList (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings window not created.");
	}
}

/* End of file StringsEditor.cpp */
