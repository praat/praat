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
 * sdk 2008/03/24 GTK
 */

#include "GuiP.h"

Widget GuiMenuBar_addMenu (Widget bar, const wchar_t *title, long flags) {
	Widget menu = NULL, menuTitle;
	menu = GuiMenuBar_addMenu2 (bar, title, flags, & menuTitle);
	return menu;
}

Widget GuiMenuBar_addMenu2 (Widget bar, const wchar_t *title, long flags, Widget *menuTitle) {
	Widget menu;
	#if gtk
		*menuTitle = gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		menu = gtk_menu_new ();
		GtkAccelGroup *ag = GTK_IS_MENU_BAR (bar) ? g_object_get_data (G_OBJECT (bar), "accel-group") : gtk_menu_get_accel_group (GTK_MENU (bar));
		gtk_menu_set_accel_group (GTK_MENU (menu), ag);
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

#if gtk
void set_position (GtkMenu *menu, gint *px, gint *py, gpointer data)
{
	gint w, h;
	GtkWidget *button = (GtkWidget *) g_object_get_data (G_OBJECT (menu), "button");

	if (GTK_WIDGET (menu) -> requisition. width < button->allocation.width)
		gtk_widget_set_size_request(GTK_WIDGET(menu), button->allocation.width, -1);

	gdk_window_get_origin (button->window, px, py);
	*px += button->allocation.x;
	*py += button->allocation.y + button->allocation.height; /* Dit is vreemd */

}

static gint button_press (GtkWidget *widget, GdkEvent *event)
{
	gint w, h;
	GtkWidget *button = (GtkWidget *) g_object_get_data (G_OBJECT (widget), "button");

/*	gdk_window_get_size (button->window, &w, &h);
	gtk_widget_set_usize (widget, w, 0);*/
	
	if (event->type == GDK_BUTTON_PRESS) {
		GdkEventButton *bevent = (GdkEventButton *) event;
		gtk_menu_popup (GTK_MENU (widget), NULL, NULL, (GtkMenuPositionFunc) set_position, NULL, bevent->button, bevent->time);
		return TRUE;
	}
	return FALSE;
}

Widget GuiMenuBar_addMenu3 (Widget parent, const wchar_t *title, long flags, Widget *button) {
	Widget menu;
	menu = gtk_menu_new ();
	*button = gtk_button_new_with_label (Melder_peekWcsToUtf8 (title));
	g_signal_connect_object (G_OBJECT (*button), "event",
		GTK_SIGNAL_FUNC (button_press), G_OBJECT (menu), G_CONNECT_SWAPPED);
	g_object_set_data (G_OBJECT (menu), "button", *button);
	if (flags & GuiMenu_INSENSITIVE)
		gtk_widget_set_sensitive (menu, FALSE);
	gtk_menu_attach_to_widget (GTK_MENU (menu), *button, NULL);
	/* TODO: Free button? */
	gtk_container_add (GTK_CONTAINER (parent), *button);
	gtk_widget_show (menu);
	gtk_widget_show (*button);
	return menu;
}
#endif

#if gtk
	#include <gdk/gdkkeysyms.h>
	static GSList *group = NULL;
#endif

void GuiMenuItem_check (Widget menuItem, bool check) {
	#if gtk
		gulong handlerId = (gulong) g_object_get_data (G_OBJECT (menuItem), "handlerId");
		void (*commandCallback) (Widget, XtPointer, XtPointer) = g_object_get_data (G_OBJECT (menuItem), "commandCallback");
		void *commandClosure = g_object_get_data (G_OBJECT (menuItem), "commandClosure");
		//Melder_casual ("GuiMenuItem_check %ld %ld %ld", handlerId, commandCallback, commandClosure);
		g_signal_handler_disconnect (G_OBJECT (menuItem), handlerId);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuItem), check);
		handlerId = g_signal_connect (G_OBJECT (menuItem), "toggled", G_CALLBACK (commandCallback), (gpointer) commandClosure);
		g_object_set_data (G_OBJECT (menuItem), "handlerId", (gpointer) handlerId);
	#elif motif
		XmToggleButtonGadgetSetState (menuItem, check, False);
	#endif
}

Widget GuiMenu_addItem (Widget menu, const wchar_t *title, long flags,
	void (*commandCallback) (Widget, XtPointer, XtPointer), const void *closure)
{
	Boolean toggle = flags & (GuiMenu_CHECKBUTTON | GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON) ? True : False;
	Widget button;
	int accelerator = flags & 127;
	Melder_assert (title != NULL);
	#if gtk
		if (toggle) {
			if (flags & (GuiMenu_RADIO_FIRST)) group = NULL;
			if (flags & (GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT)) {
				button = gtk_radio_menu_item_new_with_label (group, Melder_peekWcsToUtf8 (title));
				group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (button));
				//Melder_casual ("Created a radio menu item with title %ls, group %ld", title, group);
			} else {
				button = gtk_check_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
			}
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
	if (flags & GuiMenu_TOGGLE_ON)
		#if gtk
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (button), TRUE);
		#elif motif
			XmToggleButtonGadgetSetState (button, True, False);
		#endif
	if (accelerator) {
		/*
		 * For printable characters, the Command key is assumed.
		 */
		if (accelerator >= 32)
			flags |= GuiMenu_COMMAND;
		
		#if gtk
			static const guint acceleratorKeys[] = { 0,
				GDK_Left, GDK_Right, GDK_Up, GDK_Down, GDK_Pause, GDK_Delete, GDK_Insert, GDK_BackSpace,
				GDK_Tab, GDK_Return, GDK_Home, GDK_End, GDK_Return, GDK_Page_Up, GDK_Page_Down, GDK_Escape,
				GDK_F1, GDK_F2, GDK_F3, GDK_F4, GDK_F5, GDK_F6, GDK_F7, GDK_F8, GDK_F9, GDK_F10, GDK_F11, GDK_F12,
				0, 0, 0 };

			GdkModifierType modifiers = 0;
			if (flags & GuiMenu_COMMAND) modifiers |= GDK_CONTROL_MASK;
			if (flags & GuiMenu_SHIFT)   modifiers |= GDK_SHIFT_MASK;
			if (flags & GuiMenu_OPTION)  modifiers |= GDK_MOD1_MASK;

			guint key;
			if (accelerator < 32) {
				key = acceleratorKeys[accelerator];
			} else {
				// gdk key symbols in the ASCII range are equal to ASCII
				key = accelerator;
			}

			GtkAccelGroup *ag = gtk_menu_get_accel_group(GTK_MENU(menu));

			if (key != 0)
				gtk_widget_add_accelerator (button, toggle ? "toggled" : "activate",
					ag, accelerator, modifiers, GTK_ACCEL_VISIBLE);

		#elif motif
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
			if (acceleratorString [0]) {
				acceleratorString [strlen (acceleratorString) - 1] = '\0';
			}
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
			if (acceleratorString [0]) {
				XtVaSetValues (button, XmNaccelerator, acceleratorString, NULL);
			}
			XtVaSetValues (button, motif_argXmString (XmNacceleratorText, acceleratorText), NULL);
		#endif
	}
	#if gtk
		if (commandCallback != NULL) {
			gulong handlerId = g_signal_connect (G_OBJECT (button),
				toggle ? "toggled" : "activate",
				G_CALLBACK (commandCallback), (gpointer) closure);
			g_object_set_data (G_OBJECT (button), "handlerId", (gpointer) handlerId);
			g_object_set_data (G_OBJECT (button), "commandCallback", (gpointer) commandCallback);
			g_object_set_data (G_OBJECT (button), "commandClosure", (gpointer) closure);
		} else {
			gtk_widget_set_sensitive (button, FALSE);
		}
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
