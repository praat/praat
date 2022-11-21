/* EEGWindow.cpp
 *
 * Copyright (C) 2011-2018,2021,2022 Paul Boersma
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

#include "EEGWindow.h"
#include "EditorM.h"

Thing_implement (EEGWindow, FunctionEditor, 0);

#include "Prefs_define.h"
#include "EEGWindow_prefs.h"
#include "Prefs_install.h"
#include "EEGWindow_prefs.h"
#include "Prefs_copyToInstance.h"
#include "EEGWindow_prefs.h"

static void menu_cb_EEGWindowHelp (EEGWindow, EDITOR_ARGS) { Melder_help (U"EEG window"); }

void structEEGWindow :: v_createMenuItems_help (EditorMenu menu) {
	structFunctionEditor :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"EEGWindow help", '?', menu_cb_EEGWindowHelp);
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedEEG_preserveTimes (EEGWindow me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		if (my endSelection <= my startSelection)
			Melder_throw (U"No selection.");
		autoEEG result = EEG_extractPart (my eeg(), my startSelection, my endSelection, true);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

static void CONVERT_DATA_TO_ONE__ExtractSelectedEEG_timeFromZero (EEGWindow me, EDITOR_ARGS) {
	CONVERT_DATA_TO_ONE
		if (my endSelection <= my startSelection)
			Melder_throw (U"No selection.");
		autoEEG result = EEG_extractPart (my eeg(), my startSelection, my endSelection, false);
	CONVERT_DATA_TO_ONE_END (U"untitled")
}

void structEEGWindow :: v_createMenus () {
	EEGWindow_Parent :: v_createMenus ();
	EditorMenu eegMenu = Editor_addMenu (this, U"EEG", 0);
	our extractSelectedEEGPreserveTimesButton = EditorMenu_addCommand (eegMenu, U"Extract selected EEG (preserve times)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedEEG_preserveTimes);
	our extractSelectedEEGTimeFromZeroButton = EditorMenu_addCommand (eegMenu, U"Extract selected EEG (time from zero)", 0,
			CONVERT_DATA_TO_ONE__ExtractSelectedEEG_timeFromZero);
}

void structEEGWindow :: v_updateMenuItems () {
	EEGWindow_Parent :: v_updateMenuItems ();
	GuiThing_setSensitive (our extractSelectedEEGPreserveTimesButton, our endSelection > our startSelection);
	GuiThing_setSensitive (our extractSelectedEEGTimeFromZeroButton,  our endSelection > our startSelection);
}

autoEEGWindow EEGWindow_create (conststring32 title, EEG eeg) {
	try {
		autoEEGWindow me = Thing_new (EEGWindow);
		my eegArea() = EEGArea_create (false, nullptr, me.get());
		my eegArea() -> borrowedEEG = eeg;
		my eegAnalysisArea() = EEGAnalysisArea_create (false, nullptr, me.get());
		my textGridArea() = TextGridArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, eeg);
		// no spelling checker
		// no callback socket
		return me;
	} catch (MelderError) {
		Melder_throw (U"EEG window not created.");
	}
}

/* End of file EEGWindow.cpp */
