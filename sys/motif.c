/* motif.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2002/03/11 Mach
 * pb 2004/10/21 on Unix, Ctrl becomes the command key
 */

#include "Gui.h"
#include "longchar.h"
#include "melder.h"
#include <math.h>

Widget motif_addShell (Widget widget, long flags) {
	(void) flags;
	#if defined (macintosh) || defined (_WIN32)
		(void) widget;
		return XmCreateShell (NULL, "Praatwulg", NULL, 0);
	#else
		return XtAppCreateShell (NULL, "Praatwulg", applicationShellWidgetClass,
			XtDisplay (widget), NULL, 0);
		/*return XtVaCreateWidget ("picture", topLevelShellWidgetClass, widget, NULL);*/
	#endif
}

Widget motif_addMenuBar (Widget form) {
	Widget menuBar = XmCreateMenuBar (form, "menuBar", NULL, 0);
	XtVaSetValues (menuBar, XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
	return menuBar;
}

Widget motif_addMenu (Widget bar, const char *title, long flags) {
	Widget menuTitle, menu;
	Longchar_nativize (title, Melder_buffer1, TRUE);
	menuTitle = XmCreateCascadeButton (bar, Melder_buffer1, NULL, 0);
	if (strequ (title, "Help"))   /* BUG: Mac reacts to this title... */
		XtVaSetValues (bar, XmNmenuHelpWidget, menuTitle, NULL);   /* ...instead of to this resource. */
	menu = XmCreatePulldownMenu (bar, Melder_buffer1, NULL, 0);
	if (flags & motif_INSENSITIVE)
		XtSetSensitive (menu, False);
	XtVaSetValues (menuTitle, XmNsubMenuId, menu, NULL);
	XtManageChild (menuTitle);
	return menu;
}

Widget motif_addMenu2 (Widget bar, const char *title, long flags, Widget *menuTitle) {
	Widget menu;
	Longchar_nativize (title, Melder_buffer1, TRUE);
	*menuTitle = XmCreateCascadeButton (bar, Melder_buffer1, NULL, 0);
	if (strequ (title, "Help"))
		XtVaSetValues (bar, XmNmenuHelpWidget, *menuTitle, NULL);
	menu = XmCreatePulldownMenu (bar, Melder_buffer1, NULL, 0);
	if (flags & motif_INSENSITIVE)
		XtSetSensitive (menu, False);
	XtVaSetValues (*menuTitle, XmNsubMenuId, menu, NULL);
	XtManageChild (*menuTitle);
	return menu;
}

Widget motif_addItem (Widget menu, const char *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure)
{
	Boolean toggle = flags & (motif_CHECKABLE | motif_CHECKED) ? True : False;
	Widget button;
	int accelerator = flags & 127;
	Longchar_nativize (title, Melder_buffer1, TRUE);
	button = XtVaCreateManagedWidget (Melder_buffer1,
		toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu, NULL);
	if (flags & motif_INSENSITIVE)
		XtSetSensitive (button, False);
	if (flags & motif_CHECKED)
		XmToggleButtonGadgetSetState (button, True, False);
	if (accelerator) {
		static char *acceleratorStrings [] = { "",
			"Left", "Right", "Up", "Down", "Pause", "Delete", "Insert", "BackSpace",
			"Tab", "Return", "Home", "End", "Return", "Page_Up", "Page_Down", "Escape",
			"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
			"", "", "" };
		static char *acceleratorTexts [] = { "",
			" <-", " ->", "UP", "DOWN", "Pause", "Delete", "Insert", "Backspace",
			"Tab", "Enter", "Home", "End", "Enter", "PageUp", "PageDown", "Esc",
			"F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
			"", "", "" };
		char acceleratorString [100], acceleratorText [100];
		/*
		 * For printable characters, the Command key is assumed.
		 */
		if (accelerator >= 32)
			flags |= motif_COMMAND;
		/*
		 * Build the Motif accelerator string and the text that will appear in the menu item.
		 */
		acceleratorString [0] = '\0';
		acceleratorText [0] = '\0';
		if (flags & motif_COMMAND) {
			strcat (acceleratorString, "Ctrl ");
			strcat (acceleratorText, "Ctrl-");
		}
		if (flags & motif_SHIFT) {
			strcat (acceleratorString, "Shift ");
			strcat (acceleratorText, "Shift-");
		}
		if (flags & motif_OPTION) {
			strcat (acceleratorString, "Mod1 ");
			strcat (acceleratorText, "Alt-");
		}
		/*
		 * Delete the final space, if it exists.
		 */
		if (acceleratorString [0])
			acceleratorString [strlen (acceleratorString) - 1] = '\0';

		strcat (acceleratorString, "<Key>");
		if (accelerator < 32) {
			strcat (acceleratorString, acceleratorStrings [accelerator]);
			strcat (acceleratorText, acceleratorTexts [accelerator]);
		} else if (accelerator == '?') {
			strcat (acceleratorString, "question");
			strcat (acceleratorText, "?");
		} else if (accelerator == '[') {
			strcat (acceleratorString, "bracketleft");
			strcat (acceleratorText, "[");
		} else if (accelerator == ']') {
			strcat (acceleratorString, "bracketright");
			strcat (acceleratorText, "]");
		} else {
			static char single [2] = " ";
			single [0] = accelerator;
			strcat (acceleratorString, single);
			strcat (acceleratorText, single);
		}
		if (acceleratorString [0])
			XtVaSetValues (button, XmNaccelerator, acceleratorString, NULL);
		XtVaSetValues (button, motif_argXmString (XmNacceleratorText, acceleratorText), NULL);
	}
	XtAddCallback (button,
		toggle ? XmNvalueChangedCallback : XmNactivateCallback,
		commandCallback, (XtPointer) closure);
	return button;
}

Widget motif_addSeparator (Widget menu) {
	return XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu, NULL);
}

int motif_getResolution (Widget widget) {
	#if defined (macintosh)
		(void) widget;
		return 72;
	#elif defined (_WIN32)
		(void) widget;
		return 72;
	#else
		Display *display = XtDisplay (widget);
		return floor (25.4 * (double) DisplayWidth (display, DefaultScreen (display)) /
			DisplayWidthMM (display, DefaultScreen (display)) + 0.5);
	#endif
}

/* End of file motif.c */
