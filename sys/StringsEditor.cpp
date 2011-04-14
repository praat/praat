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

/*
 * pb 2007/12/19 created
 * pb 2008/02/06 const
 * pb 2008/03/20 split off Help menu
 * pb 2011/04/06 C++
 */

#include "StringsEditor.h"
#include "EditorM.h"
#include "machine.h"

static void destroy (I) {
	iam (StringsEditor);
	inherited (StringsEditor) destroy (me);
}

static int menu_cb_help (EDITOR_ARGS) {
	EDITOR_IAM (StringsEditor);
	(void) me;
	Melder_help (L"StringsEditor");
	return 1;
}

static void createHelpMenuItems (StringsEditor me, EditorMenu menu) {
	inherited (StringsEditor) createHelpMenuItems (StringsEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"StringsEditor help", '?', menu_cb_help);
}

static void updateList (StringsEditor me) {
	Strings strings = (Strings) my data;
	GuiList_deleteAllItems (my list);
	for (long i = 1; i <= strings -> numberOfStrings; i ++)
		GuiList_insertItem (my list, strings -> strings [i], 0);
}

static void gui_button_cb_insert (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	/*
	 * Find the first selected item.
	 */
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	long position = selected == NULL ? strings -> numberOfStrings + 1 : selected [1];
	NUMlvector_free (selected, 1);
	wchar_t *text = GuiText_getString (my text);
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
	Editor_broadcastChange (StringsEditor_as_Editor (me));
}

static void gui_button_cb_append (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	wchar_t *text = GuiText_getString (my text);
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
	Editor_broadcastChange (StringsEditor_as_Editor (me));
}

static void gui_button_cb_remove (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	for (long iselected = numberOfSelected; iselected >= 1; iselected --) {
		Strings_remove ((Strings) my data, selected [iselected]);
	}
	NUMlvector_free (selected, 1);
	updateList (me);
	Editor_broadcastChange (StringsEditor_as_Editor (me));
}

static void gui_button_cb_replace (I, GuiButtonEvent event) {
	(void) event;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
	wchar_t *text = GuiText_getString (my text);
	for (long iselected = 1; iselected <= numberOfSelected; iselected ++) {
		Strings_replace (strings, selected [iselected], text);
		GuiList_replaceItem (my list, text, selected [iselected]);
	}
	Melder_free (text);
	Editor_broadcastChange (StringsEditor_as_Editor (me));
}

static void gui_list_cb_doubleClick (GuiObject widget, void *void_me, long item) {
	(void) widget;
	iam (StringsEditor);
	Strings strings = (Strings) my data;
	if (item <= strings -> numberOfStrings)
		GuiText_setString (my text, strings -> strings [item]);
}

static void createChildren (StringsEditor me) {
	my list = GuiList_create (my dialog, 1, 0, Machine_getMenuBarHeight (), -70, true, NULL);
	//GuiList_setDoubleClickCallback (my list, gui_list_cb_doubleClick, me);
	GuiObject_show (my list);

	my text = GuiText_createShown (my dialog, 0, 0, Gui_AUTOMATIC, -40, 0);
	GuiButton_createShown (my dialog, 10, 100, Gui_AUTOMATIC, -10, L"Insert", gui_button_cb_insert, me, GuiButton_DEFAULT);
	GuiButton_createShown (my dialog, 110, 200, Gui_AUTOMATIC, -10, L"Append", gui_button_cb_append, me, 0);
	GuiButton_createShown (my dialog, 210, 300, Gui_AUTOMATIC, -10, L"Replace", gui_button_cb_replace, me, 0);
	GuiButton_createShown (my dialog, 310, 400, Gui_AUTOMATIC, -10, L"Remove", gui_button_cb_remove, me, 0);	
}

static void dataChanged (StringsEditor me) {
	updateList (me);
}

class_methods (StringsEditor, Editor) {
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createHelpMenuItems)
	class_methods_end
}

StringsEditor StringsEditor_create (GuiObject parent, const wchar_t *title, Any data) {
	StringsEditor me = Thing_new (StringsEditor); cherror
	Editor_init (StringsEditor_as_parent (me), parent, 20, 40, 600, 600, title, data); cherror
	updateList (me);
end:
	iferror forget (me);
	return me;
}

/* End of file StringsEditor.cpp */
