/* ButtonEditor.c
 *
 * Copyright (C) 1996-2004 Paul Boersma
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

#define ButtonEditor_members HyperPage_members \
	int show; \
	Widget button1, button2, button3, button4, button5;
#define ButtonEditor_methods HyperPage_methods
class_create_opaque (ButtonEditor, HyperPage)

static void drawMenuCommand (ButtonEditor me, praat_Command cmd, long i) {
	char text [1000];
	int isAdded = cmd -> uniqueID != 0 || cmd -> script != NULL;
	int isHidden = cmd -> hidden;
	int isToggled = cmd -> toggled;
	const char *clickText = isHidden ? (isToggled ? (isAdded ? "REMOVED" : "HIDDEN") : "hidden") :
		(isToggled ? "SHOWN" :  (isAdded ? (cmd -> uniqueID ? "ADDED" : "START-UP") : "shown"));
	text [0] = '\0';
	if (cmd -> unhidable) sprintf (text + strlen (text), "#unhidable ");
	else sprintf (text + strlen (text), "@@m%ld|%s@ ", i, clickText);
	sprintf (text + strlen (text), "%s: ", cmd -> window);
	if (cmd -> menu) sprintf (text + strlen (text), "%s: ", cmd -> menu);
	if (cmd -> title)
		if (cmd -> executable)
			sprintf (text + strlen (text), "@@p%ld|%s@", i, cmd -> title);
		else
			sprintf (text + strlen (text), "%s", cmd -> title);
	else
		sprintf (text + strlen (text), "---------");
	if (cmd -> after)
		sprintf (text + strlen (text), ", %%%%after \"%s\"%%", cmd -> after);
	if (cmd -> script)
		sprintf (text + strlen (text), ", script \"%s\"", Melder_asciiMessage (cmd -> script));
	HyperPage_any (me, text, my font, my fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void drawAction (ButtonEditor me, praat_Command cmd, long i) {
	char text [1000];
	int isAdded = cmd -> uniqueID != 0 || cmd -> script != NULL;
	int isHidden = cmd -> hidden, isToggled = cmd -> toggled;
	const char *clickText = isHidden ? (isToggled ? (isAdded ? "REMOVED" : "HIDDEN") : "hidden") :
		(isToggled ? "SHOWN" :  (isAdded ? (cmd -> uniqueID ? "ADDED" : "START-UP") : "shown"));
	int n1 = cmd -> n1;
	text [0] = '\0';
	if (cmd -> class4) sprintf (text + strlen (text), "#unhidable ");
	else sprintf (text + strlen (text), "@@a%ld|%s@", i, clickText);
	sprintf (text + strlen (text), " %s", ((Data_Table) cmd -> class1) -> _className);
	if (n1) sprintf (text + strlen (text), " (%d)", n1);
	if (cmd -> class2) {
		int n2 = cmd -> n2;
		sprintf (text + strlen (text), " & %s", ((Data_Table) cmd -> class2) -> _className);
		if (n2) sprintf (text + strlen (text), " (%d)", n2);
	}
	if (cmd -> class3) {
		int n3 = cmd -> n3;
		sprintf (text + strlen (text), " & %s", ((Data_Table) cmd -> class3) -> _className);
		if (n3) sprintf (text + strlen (text), " (%d)", n3);
	}
	if (cmd -> class4) {
		int n4 = cmd -> n4;
		sprintf (text + strlen (text), " & %s", ((Data_Table) cmd -> class4) -> _className);
		if (n4) sprintf (text + strlen (text), " (%d)", n4);
	}
	if (cmd -> title)
		if (cmd -> executable)
			sprintf (text + strlen (text), ": @@e%ld|%s@", i, cmd -> title);
		else
			sprintf (text + strlen (text), ": %s", cmd -> title);
	else
		sprintf (text + strlen (text), ": ---------");
	if (cmd -> after)
		sprintf (text + strlen (text), ", %%%%after \"%s\"%%", cmd -> after);
	if (cmd -> script)
		sprintf (text + strlen (text), ", script \"%s\"", Melder_asciiMessage (cmd -> script));
	HyperPage_any (me, text, my font, my fontSize, cmd -> callback ? 0 : Graphics_ITALIC, 0.0,
		cmd -> depth * 0.3, 0.4, 0.0, 0.0, 0);
}

static void draw (I) {
	iam (ButtonEditor);
	long i, n;
	Graphics_clearWs (my g);
	switch (my show) {
		case 1:
			for (i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (strequ (cmd -> window, "Objects"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 2:
			for (i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (strequ (cmd -> window, "Picture"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 3:
			for (i = 1, n = praat_getNumberOfMenuCommands (); i <= n; i ++) {
				praat_Command cmd = praat_getMenuCommand (i);
				if (! strequ (cmd -> window, "Objects") && ! strequ (cmd -> window, "Picture"))
					drawMenuCommand (me, praat_getMenuCommand (i), i);
			}
			break;
		case 4:
			for (i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				char *klas = ((Data_Table) cmd -> class1) -> _className;
				if (strcmp (klas, "N") < 0)
					drawAction (me, praat_getAction (i), i);
			}
			break;
		case 5:
			for (i = 1, n = praat_getNumberOfActions (); i <= n; i ++) {
				praat_Command cmd = praat_getAction (i);
				char *klas = ((Data_Table) cmd -> class1) -> _className;
				if (strcmp (klas, "N") >= 0)
					drawAction (me, praat_getAction (i), i);
			}
			break;
	}
}

static int goToPage (Any editor, const char *title) {
	(void) editor;
	if (! title || ! title [0]) return 0;
	if (strequ (title, "Buttons")) return 1;
	switch (title [0]) {
		case 'a': {   /* Toggle visibility of action.*/
			long i = atol (& title [1]);
			praat_Command action = praat_getAction (i);
			if (! action) return 0;
			if (action -> hidden)
				praat_showAction (action -> class1, action -> class2, action -> class3, action -> title);
			else
				praat_hideAction (action -> class1, action -> class2, action -> class3, action -> title);
		} break;
		case 'm': {   /* Toggle visibility of menu command. */
			long i = atol (& title [1]);
			praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand) return 0;
			if (menuCommand -> hidden)
				praat_showMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
			else
				praat_hideMenuCommand (menuCommand -> window, menuCommand -> menu, menuCommand -> title);
		} break;
		case 'e': {   /* Execute action. */
			long i = atol (& title [1]);
			praat_Command action = praat_getAction (i);
			if (! action || ! action -> callback) return 0;
			if (action -> title) UiHistory_write ("\n%s", action -> title);
			if (action -> script) {
				if (! DO_RunTheScriptFromAnyAddedMenuCommand ((Any) action -> script, NULL)) Melder_flushError ("Command not executed.");
			} else {
				if (! action -> callback (NULL, FALSE)) Melder_flushError ("Command not executed.");
			}
			praat_updateSelection ();
		} break;
		case 'p': {   /* Perform menu command. */
			long i = atol (& title [1]);
			praat_Command menuCommand = praat_getMenuCommand (i);
			if (! menuCommand || ! menuCommand -> callback) return 0;
			if (menuCommand -> title) UiHistory_write ("\n%s", menuCommand -> title);
			if (menuCommand -> script) {
				if (! DO_RunTheScriptFromAnyAddedMenuCommand ((Any) menuCommand -> script, NULL)) Melder_flushError ("Command not executed.");
			} else {
				if (! menuCommand -> callback (NULL, FALSE)) Melder_flushError ("Command not executed.");
			}
			praat_updateSelection ();
		} break;
		default: break;
	}
	return 0;
}

static void which (ButtonEditor me, int show) {
	my show = show;
	XmToggleButtonSetState (my button1, show == 1,  False);
	XmToggleButtonSetState (my button2, show == 2,  False);
	XmToggleButtonSetState (my button3, show == 3,  False);
	XmToggleButtonSetState (my button4, show == 4,  False);
	XmToggleButtonSetState (my button5, show == 5,  False);
	HyperPage_goToPage (me, "Buttons");
}

MOTIF_CALLBACK (cb_objects) which (void_me, 1); MOTIF_CALLBACK_END
MOTIF_CALLBACK (cb_picture) which (void_me, 2); MOTIF_CALLBACK_END
MOTIF_CALLBACK (cb_editors) which (void_me, 3); MOTIF_CALLBACK_END
MOTIF_CALLBACK (cb_actionsAM) which (void_me, 4); MOTIF_CALLBACK_END
MOTIF_CALLBACK (cb_actionsNZ) which (void_me, 5); MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (ButtonEditor);
	int height = Machine_getTextHeight (), width = BUTTON_WIDTH, x = 3, y = Machine_getMenuBarHeight () + 4;
	inherited (ButtonEditor) createChildren (me);
	my button1 = XtVaCreateManagedWidget ("Objects", xmToggleButtonWidgetClass, my dialog,
		XmNx, x, XmNy, y, XmNheight, height, XmNwidth, width, XmNindicatorType, XmONE_OF_MANY, NULL);
	XtAddCallback (my button1, XmNvalueChangedCallback, cb_objects, (XtPointer) me);
	my button2 = XtVaCreateManagedWidget ("Picture", xmToggleButtonWidgetClass, my dialog,
		XmNx, x += width + 5, XmNy, y, XmNheight, height, XmNwidth, width, XmNindicatorType, XmONE_OF_MANY, NULL);
	XtAddCallback (my button2, XmNvalueChangedCallback, cb_picture, (XtPointer) me);
	my button3 = XtVaCreateManagedWidget ("Editors", xmToggleButtonWidgetClass, my dialog,
		XmNx, x += width + 5, XmNy, y, XmNheight, height, XmNwidth, width, XmNindicatorType, XmONE_OF_MANY, NULL);
	XtAddCallback (my button3, XmNvalueChangedCallback, cb_editors, (XtPointer) me);
	my button4 = XtVaCreateManagedWidget ("Actions A-M", xmToggleButtonWidgetClass, my dialog,
		XmNx, x += width + 5, XmNy, y, XmNheight, height, XmNwidth, width + 30, XmNindicatorType, XmONE_OF_MANY, NULL);
	XtAddCallback (my button4, XmNvalueChangedCallback, cb_actionsAM, (XtPointer) me);
	my button5 = XtVaCreateManagedWidget ("Actions N-Z", xmToggleButtonWidgetClass, my dialog,
		XmNx, x += width + 30, XmNy, y, XmNheight, height, XmNwidth, width + 30, XmNindicatorType, XmONE_OF_MANY, NULL);
	XtAddCallback (my button5, XmNvalueChangedCallback, cb_actionsNZ, (XtPointer) me);
}

DIRECT (ButtonEditor, cb_ButtonEditorHelp) Melder_help ("ButtonEditor"); END

static void createMenus (I) {
	iam (ButtonEditor);
	inherited (ButtonEditor) createMenus (me);
	Editor_addCommand (me, "Help", "ButtonEditor help", '?', cb_ButtonEditorHelp);
}

class_methods (ButtonEditor, HyperPage)
	us -> scriptable = FALSE;
	class_method (createChildren)
	class_method (createMenus)
	class_method (draw)
	class_method (goToPage)
class_methods_end

ButtonEditor ButtonEditor_create (Widget parent) {
	ButtonEditor me = new (ButtonEditor);
	if (! me) return NULL;
	if (! HyperPage_init (me, parent, "Buttons", NULL))
		{ forget (me); return NULL; }
	which (me, 1);
	return me;
}

/* End of file ButtonEditor.c */
