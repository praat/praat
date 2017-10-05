/* ButtonEditor.cpp
 *
 * Copyright (C) 1996-2011,2013,2014,2015,2017 Paul Boersma
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

#include "ButtonEditor.h"
#include "praatP.h"
#include "praat_script.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (ButtonEditor, HyperPage, 0);

#if defined (_WIN32)
	#define BUTTON_WIDTH  72
#elif defined (macintosh)
	#define BUTTON_WIDTH  72
#else
	#define BUTTON_WIDTH  96
#endif

static void drawMenuCommand (ButtonEditor me, Praat_Command cmd, integer i) {
	static MelderString text { };
	bool isAdded = cmd -> uniqueID != 0 || cmd -> script != nullptr;
	bool isHidden = cmd -> hidden;
	bool isToggled = cmd -> toggled;
	const char32 *clickText = isHidden ? (isToggled ? (isAdded ? U"REMOVED" : U"HIDDEN") : U"hidden") :
		(isToggled ? U"SHOWN" :  (isAdded ? (cmd -> uniqueID ? U"ADDED" : U"START-UP") : U"shown"));
	MelderString_empty (& text);
	if (cmd -> unhidable) {
		MelderString_append (& text, U"#unhidable ");
	} else {
		MelderString_append (& text, U"@@m", i, U"|", clickText, U"@ ");
	}
	MelderString_append (& text, cmd -> window, U": ");
	if (cmd -> menu) {
		MelderString_append (& text, cmd -> menu, U": ");
	}
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, U"@@p", i, U"|", cmd -> title, U"@");
		} else {
			MelderString_append (& text, cmd -> title);
		}
	} else {
		MelderString_append (& text, U"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, U", %%%%after \"", cmd -> after, U"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, U", script \"", Melder_peekExpandBackslashes (cmd -> script), U"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void drawAction (ButtonEditor me, Praat_Command cmd, integer i) {
	static MelderString text { };
	bool isAdded = cmd -> uniqueID != 0 || cmd -> script != nullptr;
	bool isHidden = cmd -> hidden, isToggled = cmd -> toggled;
	const char32 *clickText = isHidden ? (isToggled ? (isAdded ? U"REMOVED" : U"HIDDEN") : U"hidden") :
		(isToggled ? U"SHOWN" :  (isAdded ? (cmd -> uniqueID ? U"ADDED" : U"START-UP") : U"shown"));
	int n1 = cmd -> n1;
	MelderString_empty (& text);
	if (cmd -> class4) {
		MelderString_append (& text, U"#unhidable ");
	} else {
		MelderString_append (& text, U"@@a", i, U"|", clickText, U"@ ");
	}
	MelderString_append (& text, cmd -> class1 -> className);
	if (n1) {
		MelderString_append (& text, U" (", n1, U")");
	}
	if (cmd -> class2) {
		int n2 = cmd -> n2;
		MelderString_append (& text, U" & ", cmd -> class2 -> className);
		if (n2) {
			MelderString_append (& text, U" (", n2, U")");
		}
	}
	if (cmd -> class3) {
		int n3 = cmd -> n3;
		MelderString_append (& text, U" & ", cmd -> class3 -> className);
		if (n3) {
			MelderString_append (& text, U" (", n3, U")");
		}
	}
	if (cmd -> class4) {
		int n4 = cmd -> n4;
		MelderString_append (& text, U" & ", cmd -> class4 -> className);
		if (n4) {
			MelderString_append (& text, U" (", n4, U")");
		}
	}
	MelderString_append (& text, U": ");
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, U"@@e", i, U"|", cmd -> title, U"@");
		} else {
			MelderString_append (& text, cmd -> title);
		}
	} else {
		MelderString_append (& text, U"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, U", %%%%after \"", cmd -> after, U"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, U", script \"", Melder_peekExpandBackslashes (cmd -> script), U"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

void structButtonEditor :: v_draw () {
	Graphics_clearWs (our graphics.get());
	switch (show) {
		case 1:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (str32equ (cmd -> window, U"Objects"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 2:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (str32equ (cmd -> window, U"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 3:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (! str32equ (cmd -> window, U"Objects") && ! str32equ (cmd -> window, U"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 4:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				const char32 *klas = cmd -> class1 -> className;
				if (str32cmp (klas, U"N") < 0)
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 5:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				const char32 *klas = cmd -> class1 -> className;
				if (str32cmp (klas, U"N") >= 0)
					drawAction (this, praat_getAction (i), i);
			}
			break;
	}
}

int structButtonEditor :: v_goToPage (const char32 *title) {
	if (! title || ! title [0]) return 0;
	if (str32equ (title, U"Buttons")) return 1;
	switch (title [0]) {
		case 'a': {   // toggle visibility of action
			integer i = Melder_atoi (& title [1]);
			Praat_Command action = praat_getAction (i);
			if (! action) return 0;
			if (action -> hidden)
				praat_showAction (action -> class1, action -> class2, action -> class3, action -> title);
			else
				praat_hideAction (action -> class1, action -> class2, action -> class3, action -> title);
		} break;
		case 'm': {   // toggle visibility of menu command
			integer i = Melder_atoi (& title [1]);
			Praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand) return 0;
			if (menuCommand -> hidden)
				praat_showMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
			else
				praat_hideMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
		} break;
		case 'e': {   // execute action
			integer i = Melder_atoi (& title [1]);
			Praat_Command action = praat_getAction (i);
			if (! action || ! action -> callback) return 0;
			if (action -> title) {
				UiHistory_write (U"\n");
				UiHistory_write_colonize (action -> title);
			}
			if (action -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, action -> script, nullptr, nullptr, false, nullptr);
				} catch (MelderError) {
					Melder_flushError (U"Command not executed.");
				}
			} else {
				try {
					action -> callback (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr);
				} catch (MelderError) {
					Melder_flushError (U"Command not executed.");
				}
			}
			praat_updateSelection ();
		} break;
		case 'p': {   // perform menu command
			integer i = Melder_atoi (& title [1]);
			Praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand || ! menuCommand -> callback) return 0;
			if (menuCommand -> title) {
				UiHistory_write (U"\n");
				UiHistory_write_colonize (menuCommand -> title);
			}
			if (menuCommand -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, menuCommand -> script, nullptr, nullptr, false, nullptr);
				} catch (MelderError) {
					Melder_flushError (U"Command not executed.");
				}
			} else {
				try {
					menuCommand -> callback (nullptr, 0, nullptr, nullptr, nullptr, nullptr, false, nullptr);
				} catch (MelderError) {
					Melder_flushError (U"Command not executed.");
				}
			}
			praat_updateSelection ();
		} break;
		default: break;
	}
	return 0;
}

static void which (ButtonEditor me, int show) {
	my show = show;
	GuiRadioButton_set (show == 1 ? my button1 : show == 2 ? my button2 : show == 3 ? my button3 : show == 4 ? my button4 : my button5);
	HyperPage_goToPage (me, U"Buttons");
}

static void gui_radiobutton_cb_objects (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 1); }
static void gui_radiobutton_cb_picture (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 2); }
static void gui_radiobutton_cb_editors (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 3); }
static void gui_radiobutton_cb_actionsAM (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 4); }
static void gui_radiobutton_cb_actionsNZ (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 5); }

void structButtonEditor :: v_createChildren () {
	ButtonEditor_Parent :: v_createChildren ();
	int x = 3, y = Machine_getMenuBarHeight () + 4;
	GuiRadioGroup_begin ();
	button1 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Objects", gui_radiobutton_cb_objects, this, GuiRadioButton_SET);
	x += BUTTON_WIDTH + 5;
	button2 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Picture", gui_radiobutton_cb_picture, this, 0);
	x += BUTTON_WIDTH + 5;
	button3 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Editors", gui_radiobutton_cb_editors, this, 0);
	x += BUTTON_WIDTH + 5;
	button4 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH + 30, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Actions A-M", gui_radiobutton_cb_actionsAM, this, 0);
	x += BUTTON_WIDTH + 35;
	button5 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH + 30, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Actions N-Z", gui_radiobutton_cb_actionsNZ, this, 0);
	GuiRadioGroup_end ();
}

static void menu_cb_ButtonEditorHelp (ButtonEditor /* me */, EDITOR_ARGS_DIRECT) { Melder_help (U"ButtonEditor"); }

void structButtonEditor :: v_createHelpMenuItems (EditorMenu menu) {
	ButtonEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"ButtonEditor help", '?', menu_cb_ButtonEditorHelp);
}

autoButtonEditor ButtonEditor_create () {
	try {
		autoButtonEditor me = Thing_new (ButtonEditor);
		HyperPage_init (me.get(), U"Buttons", nullptr);
		which (me.get(), 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Buttons window not created.");
	}
}

/* End of file ButtonEditor.cpp */
