/* ButtonEditor.c
 *
 * Copyright (C) 1996-2009 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2004/05/10 corrected redrawing
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/12/14 Gui
 * pb 2008/03/20 split off Help menu
 * pb 2008/03/21 new Editor API
 * pb 2009/01/17 arguments to UiForm callbacks
 */

#include "ButtonEditor.h"
#include "praatP.h"
#include "praat_script.h"
#include "EditorM.h"
#include "machine.h"

#if defined (_WIN32)
	#define BUTTON_WIDTH  72
#elif defined (macintosh)
	#define BUTTON_WIDTH  72
#else
	#define BUTTON_WIDTH  96
#endif

#define ButtonEditor__members(Klas) HyperPage__members(Klas) \
	int show; \
	Widget button1, button2, button3, button4, button5;
#define ButtonEditor__methods(Klas) HyperPage__methods(Klas)
Thing_declare2 (ButtonEditor, HyperPage);

static void drawMenuCommand (ButtonEditor me, praat_Command cmd, long i) {
	static MelderString text = { 0 };
	int isAdded = cmd -> uniqueID != 0 || cmd -> script != NULL;
	int isHidden = cmd -> hidden;
	int isToggled = cmd -> toggled;
	const wchar_t *clickText = isHidden ? (isToggled ? (isAdded ? L"REMOVED" : L"HIDDEN") : L"hidden") :
		(isToggled ? L"SHOWN" :  (isAdded ? (cmd -> uniqueID ? L"ADDED" : L"START-UP") : L"shown"));
	MelderString_empty (& text);
	if (cmd -> unhidable) {
		MelderString_append1 (& text, L"#unhidable ");
	} else {
		MelderString_append5 (& text, L"@@m", Melder_integer (i), L"|", clickText, L"@ ");
	}
	MelderString_append2 (& text, cmd -> window, L": ");
	if (cmd -> menu) {
		MelderString_append2 (& text, cmd -> menu, L": ");
	}
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append5 (& text, L"@@p", Melder_integer (i), L"|", cmd -> title, L"@");
		} else {
			MelderString_append1 (& text, cmd -> title);
		}
	} else {
		MelderString_append1 (& text, L"---------");
	}
	if (cmd -> after) {
		MelderString_append3 (& text, L", %%%%after \"", cmd -> after, L"\"%%");
	}
	if (cmd -> script) {
		MelderString_append3 (& text, L", script \"", Melder_peekExpandBackslashes (cmd -> script), L"\"");
	}
	HyperPage_any (me, text.string, my font, my fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
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
		MelderString_append1 (& text, L"#unhidable ");
	} else {
		MelderString_append5 (& text, L"@@a", Melder_integer (i), L"|", clickText, L"@ ");
	}
	MelderString_append (& text, ((Data_Table) cmd -> class1) -> _className);
	if (n1) {
		MelderString_append3 (& text, L" (", Melder_integer (n1), L")");
	}
	if (cmd -> class2) {
		int n2 = cmd -> n2;
		MelderString_append2 (& text, L" & ", ((Data_Table) cmd -> class2) -> _className);
		if (n2) {
			MelderString_append3 (& text, L" (", Melder_integer (n2), L")");
		}
	}
	if (cmd -> class3) {
		int n3 = cmd -> n3;
		MelderString_append2 (& text, L" & ", ((Data_Table) cmd -> class3) -> _className);
		if (n3) {
			MelderString_append3 (& text, L" (", Melder_integer (n3), L")");
		}
	}
	if (cmd -> class4) {
		int n4 = cmd -> n4;
		MelderString_append2 (& text, L" & ", ((Data_Table) cmd -> class4) -> _className);
		if (n4) {
			MelderString_append3 (& text, L" (", Melder_integer (n4), L")");
		}
	}
	MelderString_append (& text, L": ");
	if (cmd -> title) {
		if (cmd -> executable) {
			MelderString_append5 (& text, L"@@e", Melder_integer (i), L"|", cmd -> title, L"@");
		} else {
			MelderString_append1 (& text, cmd -> title);
		}
	} else {
		MelderString_append1 (& text, L"---------");
	}
	if (cmd -> after) {
		MelderString_append3 (& text, L", %%%%after \"", cmd -> after, L"\"%%");
	}
	if (cmd -> script) {
		MelderString_append3 (& text, L", script \"", Melder_peekExpandBackslashes (cmd -> script), L"\"");
	}
	HyperPage_any (me, text.string, my font, my fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void draw (ButtonEditor me) {
	Graphics_clearWs (my g);
	switch (my show) {
		case 1:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (wcsequ (cmd -> window, L"Objects"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 2:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (wcsequ (cmd -> window, L"Picture"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 3:
			for (long i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (! wcsequ (cmd -> window, L"Objects") && ! wcsequ (cmd -> window, L"Picture"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 4:
			for (long i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				wchar_t *klas = ((Data_Table) cmd -> class1) -> _className;
				if (wcscmp (klas, L"N") < 0)
					drawAction (me, praat_getAction (i), i);
			}
			break;
		case 5:
			for (long i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				wchar_t *klas = ((Data_Table) cmd -> class1) -> _className;
				if (wcscmp (klas, L"N") >= 0)
					drawAction (me, praat_getAction (i), i);
			}
			break;
	}
}

static int goToPage (ButtonEditor me, const wchar_t *title) {
	(void) me;
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
				UiHistory_write (L"\n");
				UiHistory_write (action -> title);
			}
			if (action -> script) {
				if (! DO_RunTheScriptFromAnyAddedMenuCommand (NULL, action -> script, NULL, NULL, false, NULL)) Melder_flushError ("Command not executed.");
			} else {
				if (! action -> callback (NULL, NULL, NULL, NULL, false, NULL)) Melder_flushError ("Command not executed.");
			}
			praat_updateSelection ();
		} break;
		case 'p': {   /* Perform menu command. */
			long i = wcstol (& title [1], NULL, 10);
			praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand || ! menuCommand -> callback) return 0;
			if (menuCommand -> title) {
				UiHistory_write (L"\n");
				UiHistory_write (menuCommand -> title);
			}
			if (menuCommand -> script) {
				if (! DO_RunTheScriptFromAnyAddedMenuCommand (NULL, menuCommand -> script, NULL, NULL, false, NULL)) Melder_flushError ("Command not executed.");
			} else {
				if (! menuCommand -> callback (NULL, NULL, NULL, NULL, false, NULL)) Melder_flushError ("Command not executed.");
			}
			praat_updateSelection ();
		} break;
		default: break;
	}
	return 0;
}

static void which (ButtonEditor me, int show) {
	my show = show;
	#if motif
	GuiRadioButton_setValue (my button1, show == 1);
	GuiRadioButton_setValue (my button2, show == 2);
	GuiRadioButton_setValue (my button3, show == 3);
	GuiRadioButton_setValue (my button4, show == 4);
	GuiRadioButton_setValue (my button5, show == 5);
	#endif
	HyperPage_goToPage (me, L"Buttons");
}

static void gui_radiobutton_cb_objects (I, GuiRadioButtonEvent event) { (void) event; which (void_me, 1); }
static void gui_radiobutton_cb_picture (I, GuiRadioButtonEvent event) { (void) event; which (void_me, 2); }
static void gui_radiobutton_cb_editors (I, GuiRadioButtonEvent event) { (void) event; which (void_me, 3); }
static void gui_radiobutton_cb_actionsAM (I, GuiRadioButtonEvent event) { (void) event; which (void_me, 4); }
static void gui_radiobutton_cb_actionsNZ (I, GuiRadioButtonEvent event) { (void) event; which (void_me, 5); }

static void createChildren (ButtonEditor me) {
	#if gtk
		void *group = NULL;
	#endif
	int x = 3, y = Machine_getMenuBarHeight () + 4;
	inherited (ButtonEditor) createChildren (ButtonEditor_as_parent (me));
	my button1 = GuiRadioButton_createShown (my holder, x, x + BUTTON_WIDTH, y, Gui_AUTOMATIC,
		L"Objects", gui_radiobutton_cb_objects, me, GuiRadioButton_SET);
	x += BUTTON_WIDTH + 5;
	my button2 = GuiRadioButton_createShown (my holder, x, x + BUTTON_WIDTH, y, Gui_AUTOMATIC,
		L"Picture", gui_radiobutton_cb_picture, me, 0);
	x += BUTTON_WIDTH + 5;
	my button3 = GuiRadioButton_createShown (my holder, x, x + BUTTON_WIDTH, y, Gui_AUTOMATIC,
		L"Editors", gui_radiobutton_cb_editors, me, 0);
	x += BUTTON_WIDTH + 5;
	my button4 = GuiRadioButton_createShown (my holder, x, x + BUTTON_WIDTH + 30, y, Gui_AUTOMATIC,
		L"Actions A-M", gui_radiobutton_cb_actionsAM, me, 0);
	x += BUTTON_WIDTH + 35;
	my button5 = GuiRadioButton_createShown (my holder, x, x + BUTTON_WIDTH + 30, y, Gui_AUTOMATIC,
		L"Actions N-Z", gui_radiobutton_cb_actionsNZ, me, 0);
	
	#if gtk
		group = GuiRadioButton_getGroup(my button1);
		GuiRadioButton_setGroup(my button2, group);
		group = GuiRadioButton_getGroup(my button2);
		GuiRadioButton_setGroup(my button3, group);
		group = GuiRadioButton_getGroup(my button3);
		GuiRadioButton_setGroup(my button4, group);
		group = GuiRadioButton_getGroup(my button4);
		GuiRadioButton_setGroup(my button5, group);
	#endif
}

static int menu_cb_ButtonEditorHelp (EDITOR_ARGS) { EDITOR_IAM (ButtonEditor); Melder_help (L"ButtonEditor"); return 1; }

static void createHelpMenuItems (ButtonEditor me, EditorMenu menu) {
	inherited (ButtonEditor) createHelpMenuItems (ButtonEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"ButtonEditor help", '?', menu_cb_ButtonEditorHelp);
}

class_methods (ButtonEditor, HyperPage) {
	us -> scriptable = false;
	class_method (createChildren)
	class_method (createHelpMenuItems)
	class_method (draw)
	class_method (goToPage)
	class_methods_end
}

ButtonEditor ButtonEditor_create (Widget parent) {
	ButtonEditor me = new (ButtonEditor); cherror
	HyperPage_init (ButtonEditor_as_parent (me), parent, L"Buttons", NULL); cherror
	which (me, 1);
end:
	iferror forget (me);
	return me;
}

/* End of file ButtonEditor.c */
