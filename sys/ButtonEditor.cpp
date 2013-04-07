/* ButtonEditor.cpp
 *
 * Copyright (C) 1996-2011,2013 Paul Boersma
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

static void drawMenuCommand (ButtonEditor me, praat_Command cmd, long i) {
	static MelderString text = { 0 };
	int isAdded = cmd -> uniqueID != 0 || cmd -> script != NULL;
	int isHidden = cmd -> hidden;
	int isToggled = cmd -> toggled;
	const wchar_t *clickText = isHidden ? (isToggled ? (isAdded ? L"REMOVED" : L"HIDDEN") : L"hidden") :
		(isToggled ? L"SHOWN" :  (isAdded ? (cmd -> uniqueID ? L"ADDED" : L"START-UP") : L"shown"));
	MelderString_empty (& text);
	if (cmd -> unhidable) {
		MelderString_append (& text, L"#unhidable ");
	} else {
		MelderString_append (& text, L"@@m", Melder_integer (i), L"|", clickText, L"@ ");
	}
	MelderString_append (& text, cmd -> window, L": ");
	if (cmd -> menu) {
		MelderString_append (& text, cmd -> menu, L": ");
	}
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, L"@@p", Melder_integer (i), L"|", cmd -> title, L"@");
		} else {
			MelderString_append (& text, cmd -> title);
		}
	} else {
		MelderString_append (& text, L"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, L", %%%%after \"", cmd -> after, L"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, L", script \"", Melder_peekExpandBackslashes (cmd -> script), L"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void drawAction (ButtonEditor me, praat_Command cmd, long i) {
	static MelderString text = { 0 };
	int isAdded = cmd -> uniqueID != 0 || cmd -> script != NULL;
	int isHidden = cmd -> hidden, isToggled = cmd -> toggled;
	const wchar_t *clickText = isHidden ? (isToggled ? (isAdded ? L"REMOVED" : L"HIDDEN") : L"hidden") :
		(isToggled ? L"SHOWN" :  (isAdded ? (cmd -> uniqueID ? L"ADDED" : L"START-UP") : L"shown"));
	int n1 = cmd -> n1;
	MelderString_empty (& text);
	if (cmd -> class4) {
		MelderString_append (& text, L"#unhidable ");
	} else {
		MelderString_append (& text, L"@@a", Melder_integer (i), L"|", clickText, L"@ ");
	}
	MelderString_append (& text, cmd -> class1 -> className);
	if (n1) {
		MelderString_append (& text, L" (", Melder_integer (n1), L")");
	}
	if (cmd -> class2) {
		int n2 = cmd -> n2;
		MelderString_append (& text, L" & ", cmd -> class2 -> className);
		if (n2) {
			MelderString_append (& text, L" (", Melder_integer (n2), L")");
		}
	}
	if (cmd -> class3) {
		int n3 = cmd -> n3;
		MelderString_append (& text, L" & ", cmd -> class3 -> className);
		if (n3) {
			MelderString_append (& text, L" (", Melder_integer (n3), L")");
		}
	}
	if (cmd -> class4) {
		int n4 = cmd -> n4;
		MelderString_append (& text, L" & ", cmd -> class4 -> className);
		if (n4) {
			MelderString_append (& text, L" (", Melder_integer (n4), L")");
		}
	}
	MelderString_append (& text, L": ");
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append (& text, L"@@e", Melder_integer (i), L"|", cmd -> title, L"@");
		} else {
			MelderString_append (& text, cmd -> title);
		}
	} else {
		MelderString_append (& text, L"---------");
	}
	if (cmd -> after) {
		MelderString_append (& text, L", %%%%after \"", cmd -> after, L"\"%%");
	}
	if (cmd -> script) {
		MelderString_append (& text, L", script \"", Melder_peekExpandBackslashes (cmd -> script), L"\"");
	}
	HyperPage_any (me, text.string, my p_font, my p_fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

void structButtonEditor :: v_draw () {
	Graphics_clearWs (g);
	switch (show) {
		case 1:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (wcsequ (cmd -> window, L"Objects"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 2:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (wcsequ (cmd -> window, L"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 3:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (! wcsequ (cmd -> window, L"Objects") && ! wcsequ (cmd -> window, L"Picture"))
					drawMenuCommand (this, praat_getMenuCommand (i), i);
			}
			break;
		case 4:
			for (long i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				const wchar_t *klas = cmd -> class1 -> className;
				if (wcscmp (klas, L"N") < 0)
					drawAction (this, praat_getAction (i), i);
			}
			break;
		case 5:
			for (long i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				const wchar_t *klas = cmd -> class1 -> className;
				if (wcscmp (klas, L"N") >= 0)
					drawAction (this, praat_getAction (i), i);
			}
			break;
	}
}

int structButtonEditor :: v_goToPage (const wchar_t *title) {
	if (! title || ! title [0]) return 0;
	if (wcsequ (title, L"Buttons")) return 1;
	switch (title [0]) {
		case 'a': {   /* Toggle visibility of action.*/
			long i = wcstol (& title [1], NULL, 10);
			praat_Command action = praat_getAction (i);
			if (! action) return 0;
			if (action -> hidden)
				praat_showAction (action -> class1, action -> class2, action -> class3, action -> title);
			else
				praat_hideAction (action -> class1, action -> class2, action -> class3, action -> title);
		} break;
		case 'm': {   /* Toggle visibility of menu command. */
			long i = wcstol (& title [1], NULL, 10);
			praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand) return 0;
			if (menuCommand -> hidden)
				praat_showMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
			else
				praat_hideMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
		} break;
		case 'e': {   /* Execute action. */
			long i = wcstol (& title [1], NULL, 10);
			praat_Command action = praat_getAction (i);
			if (! action || ! action -> callback) return 0;
			if (action -> title) {
				UiHistory_write (L"\ndo (\"");
				UiHistory_write_expandQuotes (action -> title);
				UiHistory_write (L"\")");
			}
			if (action -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (NULL, 0, NULL, action -> script, NULL, NULL, false, NULL);
				} catch (MelderError) {
					Melder_flushError ("Command not executed.");
				}
			} else {
				try {
					action -> callback (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
				} catch (MelderError) {
					Melder_flushError ("Command not executed.");
				}
			}
			praat_updateSelection ();
		} break;
		case 'p': {   /* Perform menu command. */
			long i = wcstol (& title [1], NULL, 10);
			praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand || ! menuCommand -> callback) return 0;
			if (menuCommand -> title) {
				UiHistory_write (L"\ndo (\"");
				UiHistory_write_expandQuotes (menuCommand -> title);
				UiHistory_write (L"\")");
			}
			if (menuCommand -> script) {
				try {
					DO_RunTheScriptFromAnyAddedMenuCommand (NULL, 0, NULL, menuCommand -> script, NULL, NULL, false, NULL);
				} catch (MelderError) {
					Melder_flushError ("Command not executed.");
				}
			} else {
				try {
					menuCommand -> callback (NULL, 0, NULL, NULL, NULL, NULL, false, NULL);
				} catch (MelderError) {
					Melder_flushError ("Command not executed.");
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
	( show == 1 ? my button1 : show == 2 ? my button2 : show == 3 ? my button3 : show == 4 ? my button4 : my button5 ) -> f_set ();
	HyperPage_goToPage (me, L"Buttons");
}

static void gui_radiobutton_cb_objects (I, GuiRadioButtonEvent event) { (void) event; which ((ButtonEditor) void_me, 1); }
static void gui_radiobutton_cb_picture (I, GuiRadioButtonEvent event) { (void) event; which ((ButtonEditor) void_me, 2); }
static void gui_radiobutton_cb_editors (I, GuiRadioButtonEvent event) { (void) event; which ((ButtonEditor) void_me, 3); }
static void gui_radiobutton_cb_actionsAM (I, GuiRadioButtonEvent event) { (void) event; which ((ButtonEditor) void_me, 4); }
static void gui_radiobutton_cb_actionsNZ (I, GuiRadioButtonEvent event) { (void) event; which ((ButtonEditor) void_me, 5); }

void structButtonEditor :: v_createChildren () {
	ButtonEditor_Parent :: v_createChildren ();
	int x = 3, y = Machine_getMenuBarHeight () + 4;
	GuiRadioGroup_begin ();
	button1 = GuiRadioButton_createShown (d_windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Objects", gui_radiobutton_cb_objects, this, GuiRadioButton_SET);
	x += BUTTON_WIDTH + 5;
	button2 = GuiRadioButton_createShown (d_windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Picture", gui_radiobutton_cb_picture, this, 0);
	x += BUTTON_WIDTH + 5;
	button3 = GuiRadioButton_createShown (d_windowForm, x, x + BUTTON_WIDTH, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Editors", gui_radiobutton_cb_editors, this, 0);
	x += BUTTON_WIDTH + 5;
	button4 = GuiRadioButton_createShown (d_windowForm, x, x + BUTTON_WIDTH + 30, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Actions A-M", gui_radiobutton_cb_actionsAM, this, 0);
	x += BUTTON_WIDTH + 35;
	button5 = GuiRadioButton_createShown (d_windowForm, x, x + BUTTON_WIDTH + 30, y, y + Gui_RADIOBUTTON_HEIGHT,
		L"Actions N-Z", gui_radiobutton_cb_actionsNZ, this, 0);
	GuiRadioGroup_end ();
}

static void menu_cb_ButtonEditorHelp (EDITOR_ARGS) { EDITOR_IAM (ButtonEditor); Melder_help (L"ButtonEditor"); }

void structButtonEditor :: v_createHelpMenuItems (EditorMenu menu) {
	ButtonEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"ButtonEditor help", '?', menu_cb_ButtonEditorHelp);
}

ButtonEditor ButtonEditor_create () {
	try {
		autoButtonEditor me = Thing_new (ButtonEditor);
		HyperPage_init (me.peek(), L"Buttons", NULL);
		which (me.peek(), 1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Buttons window not created.");
	}
}

/* End of file ButtonEditor.cpp */
