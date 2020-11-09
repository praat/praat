/* ButtonEditor.cpp
 *
 * Copyright (C) 1996-2020 Paul Boersma
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
	static MelderString text;
	bool isAdded = cmd -> uniqueID != 0 || !! cmd -> script;
	bool isHidden = cmd -> hidden;
	bool isToggled = cmd -> toggled;
	conststring32 clickText = isHidden ? (isToggled ? (isAdded ? U"REMOVED" : U"HIDDEN") : U"hidden") :
		(isToggled ? U"SHOWN" :  (isAdded ? (cmd -> uniqueID ? U"ADDED" : U"START-UP") : U"shown"));
	MelderString_empty (& text);
	if (cmd -> unhidable) {
		MelderString_append (& text, U"#unhidable ");
	} else {
		MelderString_append (& text, U"@@m", i, U"|", clickText, U"@ ");
	}
	MelderString_append (& text, cmd -> window.get(), U": ");
	if (cmd -> menu) {
		MelderString_append (& text, cmd -> menu.get(), U": ");
	}
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, U"@@p", i, U"|", cmd -> title.get(), U"@");
		} else {
			MelderString_append (& text, cmd -> title.get());
		}
	} else {
		MelderString_append (& text, U"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, U", %%%%after \"", cmd -> after.get(), U"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, U", script \"", Melder_peekExpandBackslashes (cmd -> script.get()), U"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void drawAction (ButtonEditor me, Praat_Command cmd, integer i) {
	static MelderString text;
	bool isAdded = cmd -> uniqueID != 0 || !! cmd -> script;
	bool isHidden = cmd -> hidden, isToggled = cmd -> toggled;
	conststring32 clickText = isHidden ? (isToggled ? (isAdded ? U"REMOVED" : U"HIDDEN") : U"hidden") :
		(isToggled ? U"SHOWN" :  (isAdded ? (cmd -> uniqueID ? U"ADDED" : U"START-UP") : U"shown"));
	integer n1 = cmd -> n1;
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
		integer n2 = cmd -> n2;
		MelderString_append (& text, U" & ", cmd -> class2 -> className);
		if (n2) {
			MelderString_append (& text, U" (", n2, U")");
		}
	}
	if (cmd -> class3) {
		integer n3 = cmd -> n3;
		MelderString_append (& text, U" & ", cmd -> class3 -> className);
		if (n3) {
			MelderString_append (& text, U" (", n3, U")");
		}
	}
	if (cmd -> class4) {
		integer n4 = cmd -> n4;
		MelderString_append (& text, U" & ", cmd -> class4 -> className);
		if (n4) {
			MelderString_append (& text, U" (", n4, U")");
		}
	}
	MelderString_append (& text, U": ");
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, U"@@e", i, U"|", cmd -> title.get(), U"@");
		} else {
			MelderString_append (& text, cmd -> title.get());
		}
	} else {
		MelderString_append (& text, U"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, U", %%%%after \"", cmd -> after.get(), U"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, U", script \"", Melder_peekExpandBackslashes (cmd -> script.get()), U"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

void structButtonEditor :: v_draw () {
	switch (show) {
		case 1:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (str32equ (cmd -> window.get(), U"Objects"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 2:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (str32equ (cmd -> window.get(), U"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 3:
			for (integer i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				Praat_Command cmd = praat_getMenuCommand (i);
				if (! str32equ (cmd -> window.get(), U"Objects") && ! str32equ (cmd -> window.get(), U"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 4:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'A' && klas [0] <= U'D')
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 5:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'E' && klas [0] <= U'H')
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 6:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'I' && klas [0] <= U'L')
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 7:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'M' && klas [0] <= U'O')
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 8:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'P' && klas [0] <= U'S')
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 9:
			for (integer i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				Praat_Command cmd = praat_getAction (i);
				conststring32 klas = cmd -> class1 -> className;
				if (klas [0] >= U'T' && klas [0] <= U'Z')
					drawAction (this, praat_getAction (i), i);
			}
			break;
	}
}

int structButtonEditor :: v_goToPage (conststring32 title) {
	if (! title || ! title [0])
		return 0;
	if (str32equ (title, U"Buttons"))
		return 1;
	switch (title [0]) {
		case 'a': {   // toggle visibility of action
			integer i = Melder_atoi (& title [1]);
			Praat_Command action = praat_getAction (i);
			if (! action) return 0;
			if (action -> hidden)
				praat_showAction (action -> class1, action -> class2, action -> class3, action -> title.get());
			else
				praat_hideAction (action -> class1, action -> class2, action -> class3, action -> title.get());
		} break;
		case 'm': {   // toggle visibility of menu command
			integer i = Melder_atoi (& title [1]);
			Praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand)
				return 0;
			if (menuCommand -> hidden)
				praat_showMenuCommand (menuCommand -> window.get(), menuCommand -> menu.get(), menuCommand -> title.get());
			else
				praat_hideMenuCommand (menuCommand -> window.get(), menuCommand -> menu.get(), menuCommand -> title.get());
		} break;
		case 'e': {   // execute action
			integer i = Melder_atoi (& title [1]);
			Praat_Command action = praat_getAction (i);
			if (! action || ! action -> callback)
				return 0;
			if (action -> title) {
				UiHistory_write (U"\n");
				UiHistory_write_colonize (action -> title.get());
			}
			if (action -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, action -> script.get(), nullptr, nullptr, false, nullptr);
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
			if (! menuCommand || ! menuCommand -> callback)
				return 0;
			if (menuCommand -> title) {
				UiHistory_write (U"\n");
				UiHistory_write_colonize (menuCommand -> title.get());
			}
			if (menuCommand -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (nullptr, 0, nullptr, menuCommand -> script.get(), nullptr, nullptr, false, nullptr);
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
	GuiRadioButton_set (show == 1 ? my button1 : show == 2 ? my button2 : show == 3 ? my button3 :
		show == 4 ? my buttonAD : show == 5 ? my buttonEH : show == 6 ? my buttonIL :
		show == 7 ? my buttonMO : show == 8 ? my buttonPS : my buttonTZ
	);
	HyperPage_goToPage (me, U"Buttons");
}

static void gui_radiobutton_cb_objects (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 1); }
static void gui_radiobutton_cb_picture (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 2); }
static void gui_radiobutton_cb_editors (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 3); }
static void gui_radiobutton_cb_actionsAD (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 4); }
static void gui_radiobutton_cb_actionsEH (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 5); }
static void gui_radiobutton_cb_actionsIL (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 6); }
static void gui_radiobutton_cb_actionsMO (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 7); }
static void gui_radiobutton_cb_actionsPS (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 8); }
static void gui_radiobutton_cb_actionsTZ (ButtonEditor me, GuiRadioButtonEvent /* event */) { which (me, 9); }

void structButtonEditor :: v_createChildren () {
	ButtonEditor_Parent :: v_createChildren ();
	int x = 3, y = Machine_getMenuBarHeight () + 4;
	GuiRadioGroup_begin ();
	constexpr int LETTER_BUTTON_WIDTH = BUTTON_WIDTH * 2 / 3;
	button1 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Objects", gui_radiobutton_cb_objects, this, GuiRadioButton_SET);
	x += BUTTON_WIDTH + 5;
	button2 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Picture", gui_radiobutton_cb_picture, this, 0);
	x += BUTTON_WIDTH + 5;
	button3 = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Editors", gui_radiobutton_cb_editors, this, 0);
	x += BUTTON_WIDTH + 5;
	buttonAD = GuiRadioButton_createShown (our windowForm, x, x + BUTTON_WIDTH + 30, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"Actions: A–D", gui_radiobutton_cb_actionsAD, this, 0);
	x += BUTTON_WIDTH + 35;
	buttonEH = GuiRadioButton_createShown (our windowForm, x, x + LETTER_BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"E–H", gui_radiobutton_cb_actionsEH, this, 0);
	x += LETTER_BUTTON_WIDTH + 5;
	buttonIL = GuiRadioButton_createShown (our windowForm, x, x + LETTER_BUTTON_WIDTH - 5, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"I–L", gui_radiobutton_cb_actionsIL, this, 0);
	x += LETTER_BUTTON_WIDTH - 5 + 5;
	buttonMO = GuiRadioButton_createShown (our windowForm, x, x + LETTER_BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"M–O", gui_radiobutton_cb_actionsMO, this, 0);
	x += LETTER_BUTTON_WIDTH + 5;
	buttonPS = GuiRadioButton_createShown (our windowForm, x, x + LETTER_BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"P–S", gui_radiobutton_cb_actionsPS, this, 0);
	x += LETTER_BUTTON_WIDTH + 5;
	buttonTZ = GuiRadioButton_createShown (our windowForm, x, x + LETTER_BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		U"T–Z", gui_radiobutton_cb_actionsTZ, this, 0);
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
