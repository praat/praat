/* GuiMenuItem.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"
#if gtk
	#include <gdk/gdkkeysyms.h>
#endif

#define _motif_SHIFT_MASK  1
#define _motif_COMMAND_MASK  2
#define _motif_OPTION_MASK  4

Thing_implement (GuiMenuItem, GuiThing, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if gtk
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) _GuiObject_getUserData (widget)
#elif cocoa
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) [(GuiCocoaMenuItem *) widget userData];
#elif motif
	#define iam_menuitem  GuiMenuItem me = (GuiMenuItem) widget -> userData
#endif

#if motif
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
			MelderString_append (& title, _GuiWin_expandAmpersands (my name), L"\t",
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
		SetMenuItemTextWithCFString (my nat.entry.handle, my nat.entry.item, (CFStringRef) Melder_peekWcsToCfstring (my name));
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

#if gtk
	static void _guiGtkMenuItem_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiMenuItem);
		trace ("destroying GuiMenuItem %p", me);
		forget (me);
	}
	static void _guiGtkMenuItem_activateCallback (GuiObject widget, gpointer void_me) {
		iam (GuiMenuItem);
		if (my d_callbackBlocked) return;
		if (G_OBJECT_TYPE (widget) == GTK_TYPE_RADIO_MENU_ITEM && ! gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (widget))) return;
		struct structGuiMenuItemEvent event = { me, 0 };
		if (my d_commandCallback) {
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your choice of menu item \"", GTK_WIDGET (widget) -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
#elif cocoa
	@implementation GuiCocoaMenuItem {
		GuiMenuItem d_userData;
	}
	- (void) dealloc {   // override
		GuiMenuItem me = d_userData;
		forget (me);
		trace ("deleting a menu item");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiMenuItem));
		d_userData = static_cast <GuiMenuItem> (userData);
	}
	- (void) _guiCocoaMenuItem_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiMenuItem me = d_userData;
		if (my d_commandCallback != NULL) {
			struct structGuiMenuItemEvent event = { me, 0 };
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your choice of menu item \"", "xx", "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
	@end
#elif motif
	static void _guiMotifMenuItem_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiMenuItem);
		forget (me);
	}
	static void _guiMotifMenuItem_activateCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		iam (GuiMenuItem);
		if (my d_commandCallback != NULL) {
			struct structGuiMenuItemEvent event = { me, 0 };
			try {
				my d_commandCallback (my d_boss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your choice of menu item \"", widget -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
#endif

GuiMenuItem GuiMenu_addItem (GuiMenu menu, const wchar_t *title, long flags,
	void (*commandCallback) (void *boss, GuiMenuItemEvent event), void *boss)
{
	GuiMenuItem me = Thing_new (GuiMenuItem);
	my d_shell = menu -> d_shell;
	my d_parent = menu;
	my d_menu = menu;

	trace ("creating item \"%ls\" in menu %p", title, menu);
	bool toggle = flags & (GuiMenu_CHECKBUTTON | GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT | GuiMenu_TOGGLE_ON) ? true : false;
	int accelerator = flags & 127;
	Melder_assert (title != NULL);
	#if gtk
		static GSList *group = NULL;
		if (toggle) {
			if (flags & (GuiMenu_RADIO_FIRST)) group = NULL;
			if (flags & (GuiMenu_RADIO_FIRST | GuiMenu_RADIO_NEXT)) {
				my d_widget = gtk_radio_menu_item_new_with_label (group, Melder_peekWcsToUtf8 (title));
				group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (my d_widget));
				trace ("created a radio menu item with title \"%ls\", group %p", title, group);
			} else {
				my d_widget = gtk_check_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
			}
		} else {
			my d_widget = gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		}
		Melder_assert (menu -> d_widget);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu -> d_widget), GTK_WIDGET (my d_widget));
		_GuiObject_setUserData (my d_widget, me);
	#elif cocoa
		NSString *string = (NSString *) Melder_peekWcsToCfstring (title);
		GuiCocoaMenuItem *menuItem = [[GuiCocoaMenuItem alloc]
			initWithTitle:string
			action: NULL
			keyEquivalent: @""];
		//Melder_assert ([string retainCount] == 2 || [string retainCount] == -1);   // the menu item retains the string (assertion can fail on 10.6)
		trace ("string retain count = %d", (int) [string retainCount]);
		my d_widget = menuItem;
		trace ("installing item in GuiMenu %p (NSMenu %p); retain count = %d", menu, menu -> d_cocoaMenu, [menuItem retainCount]);
		[menu -> d_cocoaMenu  addItem: (NSMenuItem *) my d_widget];   // the menu will retain the item...
		trace ("installed item in GuiMenu %p (NSMenu %p); retain count = %d", menu, menu -> d_cocoaMenu, [menuItem retainCount]);
		trace ("release the item");
		[menuItem release];   // ... so we can release the item already
		trace ("set user data");
		[menuItem setUserData: me];
	#elif motif
		my d_widget = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (title),
			toggle ? xmToggleButtonGadgetClass : xmPushButtonGadgetClass, menu -> d_widget, NULL);
		_GuiObject_setUserData (my d_widget, me);
	#endif
	Melder_assert (my d_widget != NULL);

	trace ("set sensitivity");
	if (flags & GuiMenu_INSENSITIVE)
		my f_setSensitive (false);

	trace ("understand toggle menu items");
	if (flags & GuiMenu_TOGGLE_ON)
		#if gtk
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (my d_widget), TRUE);
		#elif cocoa
			[menuItem setState: NSOnState];
		#elif motif
			XmToggleButtonGadgetSetState (my d_widget, True, False);
		#endif

	if (accelerator) {
		trace ("adding accelerator %d", accelerator);
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

			GdkModifierType modifiers = (GdkModifierType) 0;
			if (flags & GuiMenu_COMMAND) modifiers = (GdkModifierType) (modifiers | GDK_CONTROL_MASK);
			if (flags & GuiMenu_SHIFT)   modifiers = (GdkModifierType) (modifiers | GDK_SHIFT_MASK);
			if (flags & GuiMenu_OPTION)  modifiers = (GdkModifierType) (modifiers | GDK_MOD1_MASK);

			guint key;
			if (accelerator < 32) {
				key = acceleratorKeys [accelerator];
			} else {
				// gdk key symbols in the ASCII range are equal to ASCII
				key = accelerator;
			}

			GtkAccelGroup *ag = gtk_menu_get_accel_group (GTK_MENU (menu -> d_widget));

			if (key != 0)
				gtk_widget_add_accelerator (GTK_WIDGET (my d_widget), toggle ? "toggled" : "activate",
					ag, key, modifiers, GTK_ACCEL_VISIBLE);

		#elif cocoa
			accelerator = tolower (accelerator);   // otherwise, a Shift key is introduced in the mask
			NSUInteger mask = 0;
			if (flags & GuiMenu_COMMAND) mask |= NSCommandKeyMask;
			if (flags & GuiMenu_SHIFT)   mask |= NSShiftKeyMask;
			if (flags & GuiMenu_OPTION)  mask |= NSAlternateKeyMask;
			[menuItem setKeyEquivalentModifierMask: mask];
			if (accelerator > 0 && accelerator < 32) {
				static unichar acceleratorKeys [] = { 0,
					NSLeftArrowFunctionKey, NSRightArrowFunctionKey, NSUpArrowFunctionKey, NSDownArrowFunctionKey, NSPauseFunctionKey, NSDeleteFunctionKey, NSInsertFunctionKey, NSBackspaceCharacter,
					NSTabCharacter, NSNewlineCharacter, NSHomeFunctionKey, NSEndFunctionKey, NSCarriageReturnCharacter, NSPageUpFunctionKey, NSPageDownFunctionKey, 27,
					NSF1FunctionKey, NSF2FunctionKey, NSF3FunctionKey, NSF4FunctionKey, NSF5FunctionKey, NSF6FunctionKey,
					NSF7FunctionKey, NSF8FunctionKey, NSF9FunctionKey, NSF10FunctionKey, NSF11FunctionKey, NSF12FunctionKey,
					0, 0, 0 };
				[menuItem   setKeyEquivalent: [NSString   stringWithCharacters: & acceleratorKeys [accelerator]   length: 1]];
			} else {
				[menuItem setKeyEquivalent: [NSString stringWithFormat: @"%c", accelerator]];
			}
		#elif motif
			int modifiers = 0;
			if (flags & GuiMenu_COMMAND) modifiers |= _motif_COMMAND_MASK;
			if (flags & GuiMenu_SHIFT)   modifiers |= _motif_SHIFT_MASK;
			if (flags & GuiMenu_OPTION)  modifiers |= _motif_OPTION_MASK;
			if (accelerator > 0 && accelerator < 32) {
				if (my d_widget -> shell) {
					my d_widget -> shell -> motiff.shell.lowAccelerators [modifiers] |= 1 << accelerator;
				} else {
					theGuiTopLowAccelerators [modifiers] |= 1 << accelerator;
				}
			} else if (accelerator == '?' || accelerator == '{' || accelerator == '}' || accelerator == '\"' ||
				accelerator == '<' || accelerator == '>' || accelerator == '|' || accelerator == '_' || accelerator == '+' || accelerator == '~')
			{
				modifiers |= _motif_SHIFT_MASK;
			}
			my d_widget -> motiff.pushButton.acceleratorChar = accelerator;
			my d_widget -> motiff.pushButton.acceleratorModifiers = modifiers;
			NativeMenuItem_setText (my d_widget);
		#endif
		trace ("added accelerator %d", accelerator);
	}
	#if mac && useCarbon
		if (flags & GuiMenu_ATTRACTIVE) {
			trace ("attractive!");
			SetItemStyle (my d_widget -> nat.entry.handle, my d_widget -> nat.entry.item, bold);
		}
	#endif

	trace ("install the command callback");
	my d_commandCallback = commandCallback;
	my d_boss = boss;
	#if gtk
		if (commandCallback != NULL) {
			if (flags == GuiMenu_TAB) {
				GtkWidget *shell = gtk_widget_get_toplevel (gtk_menu_get_attach_widget (GTK_MENU (menu -> d_widget)));
				trace ("tab set in GTK window %p", shell);
				g_object_set_data (G_OBJECT (shell), "tabCallback", (gpointer) _guiGtkMenuItem_activateCallback);
				g_object_set_data (G_OBJECT (shell), "tabClosure", (gpointer) me);
			} else if (flags == (GuiMenu_TAB | GuiMenu_SHIFT)) {
				GtkWidget *shell = gtk_widget_get_toplevel (gtk_menu_get_attach_widget (GTK_MENU (menu -> d_widget)));
				trace ("shift-tab set in GTK window %p", shell);
				g_object_set_data (G_OBJECT (shell), "shiftTabCallback", (gpointer) _guiGtkMenuItem_activateCallback);
				g_object_set_data (G_OBJECT (shell), "shiftTabClosure", (gpointer) me);
			} else {
				g_signal_connect (G_OBJECT (my d_widget),
					toggle ? "toggled" : "activate",
					G_CALLBACK (_guiGtkMenuItem_activateCallback), (gpointer) me);
			}
		} else {
			gtk_widget_set_sensitive (GTK_WIDGET (my d_widget), FALSE);
		}
		gtk_widget_show (GTK_WIDGET (my d_widget));
	#elif cocoa
		[(NSMenuItem *) my d_widget setTarget: (id) my d_widget];
		[(NSMenuItem *) my d_widget setAction: @selector (_guiCocoaMenuItem_activateCallback:)];
	#elif motif
		XtAddCallback (my d_widget,
			toggle ? XmNvalueChangedCallback : XmNactivateCallback,
			_guiMotifMenuItem_activateCallback, (XtPointer) me);
	#endif

	trace ("make sure that I will be destroyed when my widget is destroyed");
	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenuItem_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenuItem_destroyCallback, me);
	#endif

	return me;
}

GuiMenuItem GuiMenu_addSeparator (GuiMenu menu) {
	GuiMenuItem me = Thing_new (GuiMenuItem);
	my d_shell = menu -> d_shell;
	my d_parent = menu;
	my d_menu = menu;
	#if gtk
		my d_widget = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu -> d_widget), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (my d_widget));
	#elif cocoa
		my d_widget = (GuiObject) [GuiCocoaMenuItem separatorItem];
		trace ("install separator in menu %p", menu);
		trace ("installing separator in GuiMenu %p (NSMenu %p); retain count = %d", menu, menu -> d_cocoaMenu, [((NSMenuItem *) my d_widget) retainCount]);
		[menu -> d_cocoaMenu  addItem: (NSMenuItem *) my d_widget];   // the menu will retain the item...
		trace ("installed separator in GuiMenu %p (NSMenu %p); retain count = %d", menu, menu -> d_cocoaMenu, [((NSMenuItem *) my d_widget) retainCount]);
		trace ("release the item");
		//[(NSMenuItem *) my d_widget release];   // ... so we can release the item already
		trace ("set user data");
		[(GuiCocoaMenuItem *) my d_widget setUserData: me];
	#elif motif
		my d_widget = XtVaCreateManagedWidget ("menuSeparator", xmSeparatorGadgetClass, menu -> d_widget, NULL);
	#endif

	trace ("make sure that I will be destroyed when my widget is destroyed");
	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenuItem_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenuItem_destroyCallback, me);
	#endif
	return me;
}

void structGuiMenuItem :: f_check (bool check) {
	Melder_assert (d_widget != NULL);
	#if gtk
		d_callbackBlocked = true;
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (d_widget), check);
		d_callbackBlocked = false;
	#elif cocoa
		GuiCocoaMenuItem *item = (GuiCocoaMenuItem*)d_widget;
		[item   setState: check];
	#elif motif
		XmToggleButtonGadgetSetState (d_widget, check, False);
	#endif
}

/* End of file GuiMenuItem.cpp */
