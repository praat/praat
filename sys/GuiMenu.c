/* GuiMenu.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * sdk 2008/02/08 GTK
 */

#include "Gui.h"

Widget GuiMenuBar_addMenu (Widget bar, const wchar_t *title, long flags) {
	Widget menu = NULL, menuTitle;
	menu = GuiMenuBar_addMenu2 (bar, title, flags, &menuTitle);
	return menu;
}

Widget GuiMenuBar_addMenu2 (Widget bar, const wchar_t *title, long flags, Widget *menuTitle) {
	Widget menu;
	#if gtk
		*menuTitle = gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		menu = gtk_menu_new ();
		if (flags & GuiMenu_INSENSITIVE)
			gtk_widget_set_sensitive (menu, FALSE);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (*menuTitle), menu);
		gtk_menu_shell_append (GTK_MENU_SHELL (bar), *menuTitle);
		gtk_widget_show (menu);
		gtk_widget_show (*menuTitle);
	#elif motif
		*menuTitle = XmCreateCascadeButton (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
		if (wcsequ (title, L"Help"))
			XtVaSetValues (bar, XmNmenuHelpWidget, *menuTitle, NULL);
		menu = XmCreatePulldownMenu (bar, Melder_peekWcsToUtf8 (title), NULL, 0);
		if (flags & GuiMenu_INSENSITIVE)
			XtSetSensitive (menu, False);
		XtVaSetValues (*menuTitle, XmNsubMenuId, menu, NULL);
		XtManageChild (*menuTitle);
	#endif
	return menu;
}

Widget GuiMenu_addItem (Widget menu, const wchar_t *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure)
{
	Boolean toggle = flags & (GuiMenu_CHECKABLE | GuiMenu_CHECKED) ? True : False;
	Widget button;
	int accelerator = flags & 127;
	Melder_assert (title != NULL);
	#if gtk
		if (toggle) {
			button = gtk_check_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		} else {
			button = gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		}
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), button);
	#elif motif
		button = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (title),
			toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu, NULL);
	#endif
	Melder_assert (button != NULL);
	if (flags & GuiMenu_INSENSITIVE)
		#if gtk
			gtk_widget_set_sensitive (button, FALSE);
		#elif motif
			XtSetSensitive (button, False);
		#endif
	if (flags & GuiMenu_CHECKED)
		#if gtk
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (button), TRUE);
		#elif motif
			XmToggleButtonGadgetSetState (button, True, False);
		#endif
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
		#if gtk
// TODO: Even uitzoeken hoe dit 'accel_path' werkt
//			gtk_menu_item_set_accel_path (GTK_MENU_ITEM (button), ...);
		#elif motif
			if (acceleratorString [0])
				XtVaSetValues (button, XmNaccelerator, acceleratorString, NULL);
			XtVaSetValues (button, motif_argXmString (XmNacceleratorText, acceleratorText), NULL);
		#endif
	}
	#if gtk
		g_signal_connect_swapped (G_OBJECT (button), toggle ? "toggled" : "activate", G_CALLBACK (commandCallback), (gpointer) closure);
		gtk_widget_show (button);
	#elif motif
		XtAddCallback (button,
			toggle ? XmNvalueChangedCallback : XmNactivateCallback,
			commandCallback, (XtPointer) closure);
	#endif

	return button;
}

Widget GuiMenu_addSeparator (Widget menu) {
	#if gtk
		Widget separator = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
		gtk_widget_show (separator);
		return separator;
	#elif motif
		return XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu, NULL);
	#endif
}

/* End of file GuiMenu.c */
