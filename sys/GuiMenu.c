/* GuiMenu.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/06/09 wchar_t
 * pb 2007/12/13 Gui
 */

#include "Gui.h"

Widget GuiMenuBar_addMenu (Widget bar, const wchar_t *title, long flags) {
	Widget menuTitle, menu;
	menuTitle = XmCreateCascadeButton (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
	if (wcsequ (title, L"Help"))   /* BUG: Mac reacts to this title... */
		XtVaSetValues (bar, XmNmenuHelpWidget, menuTitle, NULL);   /* ...instead of to this resource. */
	menu = XmCreatePulldownMenu (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
	if (flags & GuiMenu_INSENSITIVE)
		XtSetSensitive (menu, False);
	XtVaSetValues (menuTitle, XmNsubMenuId, menu, NULL);
	XtManageChild (menuTitle);
	return menu;
}

Widget GuiMenuBar_addMenu2 (Widget bar, const wchar_t *title, long flags, Widget *menuTitle) {
	Widget menu;
	*menuTitle = XmCreateCascadeButton (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
	if (wcsequ (title, L"Help"))
		XtVaSetValues (bar, XmNmenuHelpWidget, *menuTitle, NULL);
	menu = XmCreatePulldownMenu (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
	if (flags & GuiMenu_INSENSITIVE)
		XtSetSensitive (menu, False);
	XtVaSetValues (*menuTitle, XmNsubMenuId, menu, NULL);
	XtManageChild (*menuTitle);
	return menu;
}

Widget GuiMenu_addItem (Widget menu, const wchar_t *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure)
{
	Boolean toggle = flags & (GuiMenu_CHECKABLE | GuiMenu_CHECKED) ? True : False;
	Widget button;
	int accelerator = flags & 127;
	Melder_assert (title != NULL);
	button = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (title),
		toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu, NULL);
	Melder_assert (button != NULL);
	if (flags & GuiMenu_INSENSITIVE)
		XtSetSensitive (button, False);
	if (flags & GuiMenu_CHECKED)
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
			flags |= GuiMenu_COMMAND;
		/*
		 * Build the Motif accelerator string and the text that will appear in the menu item.
		 */
		acceleratorString [0] = '\0';
		acceleratorText [0] = '\0';
		if (flags & GuiMenu_COMMAND) {
			strcat (acceleratorString, "Ctrl ");
			strcat (acceleratorText, "Ctrl-");
		}
		if (flags & GuiMenu_SHIFT) {
			strcat (acceleratorString, "Shift ");
			strcat (acceleratorText, "Shift-");
		}
		if (flags & GuiMenu_OPTION) {
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

Widget GuiMenu_addSeparator (Widget menu) {
	return XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu, NULL);
}

/* End of file GuiMenu.c */
