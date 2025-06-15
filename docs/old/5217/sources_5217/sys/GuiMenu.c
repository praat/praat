/* GuiMenu.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2010/11/28 removed explicit Motif
 */

#include "GuiP.h"

#define _motif_SHIFT_MASK  1
#define _motif_COMMAND_MASK  2
#define _motif_OPTION_MASK  4

GuiObject GuiMenuBar_addMenu (GuiObject bar, const wchar_t *title, long flags) {
	GuiObject menu = NULL, menuTitle;
	menu = GuiMenuBar_addMenu2 (bar, title, flags, & menuTitle);
	return menu;
}

GuiObject GuiMenuBar_addMenu2 (GuiObject bar, const wchar_t *title, long flags, GuiObject *menuTitle) {
	GuiObject menu;
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
	#elif win || mac
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
static void set_position (GtkMenu *menu, gint *px, gint *py, gpointer data)
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

GuiObject GuiMenuBar_addMenu3 (GuiObject parent, const wchar_t *title, long flags, GuiObject *button) {
	GuiObject menu;
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

void GuiMenuItem_check (GuiObject menuItem, bool check) {
	Melder_assert (menuItem != NULL);
	#if gtk
		gulong handlerId = (gulong) g_object_get_data (G_OBJECT (menuItem), "handlerId");
		void (*commandCallback) (GuiObject, XtPointer, XtPointer) = g_object_get_data (G_OBJECT (menuItem), "commandCallback");
		void *commandClosure = g_object_get_data (G_OBJECT (menuItem), "commandClosure");
		//Melder_casual ("GuiMenuItem_check %ld %ld %ld", handlerId, commandCallback, commandClosure);
		g_signal_handler_disconnect (G_OBJECT (menuItem), handlerId);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuItem), check);
		handlerId = g_signal_connect (G_OBJECT (menuItem), "toggled", G_CALLBACK (commandCallback), (gpointer) commandClosure);
		g_object_set_data (G_OBJECT (menuItem), "handlerId", (gpointer) handlerId);
	#elif win || mac
		XmToggleButtonGadgetSetState (menuItem, check, False);
	#endif
}

#if win || mac
static void NativeMenuItem_setText (GuiObject me) {
	int acc = my motiff.pushButton.acceleratorChar, modifiers = my motiff.pushButton.acceleratorModifiers;
	#if win
		static MelderString title = { 0 };
		if (acc == 0) {
			MelderString_copy (& title, _GuiWin_expandAmpersands (my name));
		} else {
			static const wchar_t *keyStrings [256] = {
				0, L"<-", L"->", L"Up", L"Down", L"PAUSE", L"Del", L"Ins", L"Backspace", L"Tab", L"LineFeed", L"Home", L"End", L"Enter", L"PageUp", L"PageDown",
				L"Esc", L"F1", L"F2", L"F3", L"F4", L"F5", L"F6", L"F7", L"F8", L"F9", L"F10", L"F11", L"F12", 0, 0, 0,
				L"Space", L"!", L"\"", L"#", L"$", L"%", L"&", L"\'", L"(", L")", L"*", L"+", L",", L"-", L".", L"/",
				L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L":", L";", L"<", L"=", L">", L"?",
				L"@", L"A", L"B", L"C", L"D", L"E", L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O",
				L"P", L"Q", L"R", L"S", L"T", L"U", L"V", L"W", L"X", L"Y", L"Z", L"[", L"\\", L"]", L"^", L"_",
				L"`", L"a", L"b", L"c", L"d", L"e", L"f", L"g", L"h", L"i", L"j", L"k", L"l", L"m", L"n", L"o",
				L"p", L"q", L"r", L"s", L"t", L"u", L"v", L"w", L"x", L"y", L"z", L"{", L"|", L"}", L"~", L"Del",
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"[", L"]", L",", L"?", L".", L"\\",
				L";", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, L"-", L"`", L"=", L"\'", 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
			const wchar_t *keyString = keyStrings [acc] ? keyStrings [acc] : L"???";
			MelderString_empty (& title);
			MelderString_append6 (&title, _GuiWin_expandAmpersands (my name), L"\t",
				modifiers & _motif_COMMAND_MASK ? L"Ctrl-" : NULL,
				modifiers & _motif_OPTION_MASK ? L"Alt-" : NULL,
				modifiers & _motif_SHIFT_MASK ? L"Shift-" : NULL, keyString);
		}
		ModifyMenu (my nat.entry.handle, my nat.entry.id, MF_BYCOMMAND | MF_STRING, my nat.entry.id, title.string);
	#elif mac
		static int theGlyphs [1+31] = { 0,
			kMenuLeftArrowDashedGlyph, kMenuRightArrowDashedGlyph, kMenuUpArrowDashedGlyph, kMenuDownwardArrowDashedGlyph, 0,
			kMenuDeleteRightGlyph, 0, kMenuDeleteLeftGlyph, kMenuTabRightGlyph, 0,
			0, 0, kMenuReturnGlyph, kMenuPageUpGlyph, kMenuPageDownGlyph,
			kMenuEscapeGlyph, kMenuF1Glyph, kMenuF2Glyph, kMenuF3Glyph, kMenuF4Glyph,
			kMenuF5Glyph, kMenuF6Glyph, kMenuF7Glyph, kMenuF8Glyph, kMenuF9Glyph,
			kMenuF10Glyph, kMenuF11Glyph, kMenuF12Glyph, 0, 0,
			0 };
		SetMenuItemTextWithCFString (my nat.entry.handle, my nat.entry.item, Melder_peekWcsToCfstring (my name));
		if (acc > 32) {
			SetItemCmd (my nat.entry.handle, my nat.entry.item, acc);
		} else {
			Melder_assert (acc > 0 && acc < 32);
			SetItemCmd (my nat.entry.handle, my nat.entry.item, ' ');   /* Funny that this should be needed. */
			SetMenuItemKeyGlyph (my nat.entry.handle, my nat.entry.item, theGlyphs [acc]);
		}
		SetMenuItemModifiers (my nat.entry.handle, my nat.entry.item,
			( modifiers & _motif_OPTION_MASK ? kMenuOptionModifier : 0 ) +
			( modifiers & _motif_SHIFT_MASK ? kMenuShiftModifier : 0 ) +
			( modifiers & _motif_COMMAND_MASK ? 0 : kMenuNoCommandModifier ));
	#endif
}
#endif

GuiObject GuiMenu_addItem (GuiObject menu, const wchar_t *title, long flags,
	void (*commandCallback) (GuiObject, XtPointer, XtPointer), const void *closure)
{
	Boolean toggle = flags & (GuiMenu_CHECKBUTTON | GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON) ? True : False;
	GuiObject button;
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
	#elif win || mac
		button = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (title),
			toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu, NULL);
	#endif
	Melder_assert (button != NULL);
	if (flags & GuiMenu_INSENSITIVE)
		GuiObject_setSensitive (button, false);
	if (flags & GuiMenu_TOGGLE_ON)
		#if gtk
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (button), TRUE);
		#elif win || mac
			XmToggleButtonGadgetSetState (button, True, False);
		#endif
	if (accelerator) {
		/*
		 * For printable characters, the Command key is assumed.
		 */
		if (accelerator >= 32)
			flags |= GuiMenu_COMMAND;
		
		#if gtk
			static const guint acceleratorKeys [] = { 0,
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
				key = acceleratorKeys [accelerator];
			} else {
				// gdk key symbols in the ASCII range are equal to ASCII
				key = accelerator;
			}

			GtkAccelGroup *ag = gtk_menu_get_accel_group (GTK_MENU (menu));

			if (key != 0)
				gtk_widget_add_accelerator (button, toggle ? "toggled" : "activate",
					ag, key, modifiers, GTK_ACCEL_VISIBLE);

		#elif win || mac
			int modifiers = 0;
			if (flags & GuiMenu_COMMAND) modifiers |= _motif_COMMAND_MASK;
			if (flags & GuiMenu_SHIFT) modifiers |= _motif_SHIFT_MASK;
			if (flags & GuiMenu_OPTION) modifiers |= _motif_OPTION_MASK;
			if (accelerator > 0 && accelerator < 32) {
				button -> shell -> motiff.shell.lowAccelerators [modifiers] |= 1 << accelerator;
			} else if (accelerator == '?' || accelerator == '{' || accelerator == '}' || accelerator == '\"' ||
				accelerator == '<' || accelerator == '>' || accelerator == '|' || accelerator == '_' || accelerator == '+' || accelerator == '~')
			{
				modifiers |= _motif_SHIFT_MASK;
			}
			button -> motiff.pushButton.acceleratorChar = accelerator;
			button -> motiff.pushButton.acceleratorModifiers = modifiers;
			NativeMenuItem_setText (button);
		#endif
	}
	#if mac
		if (flags & GuiMenu_ATTRACTIVE) {
			//Melder_casual ("attractive!");
			SetItemStyle (button -> nat.entry.handle, button -> nat.entry.item, bold);
		}
	#endif
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
	#elif win || mac
		XtAddCallback (button,
			toggle ? XmNvalueChangedCallback : XmNactivateCallback,
			commandCallback, (XtPointer) closure);
	#endif

	return button;
}

GuiObject GuiMenu_addSeparator (GuiObject menu) {
	#if gtk
		GuiObject separator = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), separator);
		gtk_widget_show (separator);
		return separator;
	#elif win || mac
		return XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu, NULL);
	#endif
}

/* End of file GuiMenu.c */
