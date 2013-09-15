/* GuiMenu.cpp
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

Thing_implement (GuiMenu, GuiThing, 0);

void structGuiMenu :: v_destroy () {
	forget (d_cascadeButton);
	forget (d_menuItem);
	GuiMenu_Parent :: v_destroy ();   // if (d_widget) { _GuiObject_setUserData (d_widget, NULL); GuiObject_destroy (d_widget); }
}

#if gtk
	static void _guiGtkMenu_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) void_me;
		GuiMenu me = (GuiMenu) _GuiObject_getUserData (widget);
		trace ("destroying GuiMenu %p", me);
		if (! me) return;   // we could be destroying me
		my d_widget = NULL;   // undangle
		if (my d_cascadeButton) my d_cascadeButton -> d_widget = NULL;   // undangle
		if (my d_menuItem) my d_menuItem -> d_widget = NULL;   // undangle
		forget (me);
	}
	static void _guiGtkMenuCascadeButton_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) void_me;
		GuiMenu me = (GuiMenu) _GuiObject_getUserData (widget);
		if (! me) return;
		trace ("destroying GuiButton %p", my d_cascadeButton);
		gtk_widget_destroy (GTK_WIDGET (my d_widget));
	}
#elif cocoa
	static void (*theOpenDocumentCallback) (MelderFile file);
	void Gui_setOpenDocumentCallback (void (*openDocumentCallback) (MelderFile file)) {
		theOpenDocumentCallback = openDocumentCallback;
	}
	static NSMenu *theMenuBar;
	static int theNumberOfMenuBarItems = 0;
	static NSMenuItem *theMenuBarItems [30];
	@interface GuiCocoaApplicationDelegate : NSObject { }
	@end
	@implementation GuiCocoaApplicationDelegate
	- (void) applicationWillFinishLaunching: (NSNotification *) note
	{
		(void) note;
		for (int imenu = 1; imenu <= theNumberOfMenuBarItems; imenu ++) {
			[[NSApp mainMenu] addItem: theMenuBarItems [imenu]];   // the menu will retain the item...
			[theMenuBarItems [imenu] release];   // ... so we can release the item
		}
	}
	- (void) application: (NSApplication *) sender openFiles: (NSArray *) fileNames
	{
		for (int i = 1; i <= [fileNames count]; i ++) {
			NSString *cocoaFileName = [fileNames objectAtIndex: i - 1];
			structMelderFile file = { 0 };
			Melder_8bitFileRepresentationToWcs_inline ([cocoaFileName UTF8String], file. path);
			if (theOpenDocumentCallback)
				theOpenDocumentCallback (& file);
		}
	}
	@end
	static id theGuiCocoaApplicationDelegate;
#elif motif
	static void _guiMotifMenu_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) void_me;
		(void) call;
		GuiMenu me = (GuiMenu) _GuiObject_getUserData (widget);
		trace ("destroying GuiMenu %p", me);
		if (! me) return;   // we could be destroying me
		my d_widget = NULL;   // undangle
		if (my d_cascadeButton) my d_cascadeButton -> d_widget = NULL;   // undangle
		if (my d_menuItem) my d_menuItem -> d_widget = NULL;   // undangle
		forget (me);
	}
#endif

void structGuiMenu :: v_hide () {
	#if gtk
		gtk_widget_hide (GTK_WIDGET (d_gtkMenuTitle));
	#elif cocoa
	#elif motif
		XtUnmanageChild (d_xmMenuTitle);
	#endif
}

void structGuiMenu :: v_setSensitive (bool sensitive) {
	#if gtk
		gtk_widget_set_sensitive (GTK_WIDGET (d_gtkMenuTitle), sensitive);
	#elif cocoa
	#elif motif
		XtSetSensitive (d_xmMenuTitle, sensitive);
	#endif
}

void structGuiMenu :: v_show () {
	trace ("begin");
	#if gtk
		gtk_widget_show (GTK_WIDGET (d_gtkMenuTitle));
	#elif cocoa
	#elif motif
		XtManageChild (d_xmMenuTitle);
	#endif
	trace ("end");
}

void structGuiMenu :: f_empty () {
	#if gtk
		trace ("begin");
		Melder_assert (d_widget);
		/*
		 * Destroy my widget, but prevent forgetting me.
		 */
		_GuiObject_setUserData (d_widget, NULL);
		gtk_widget_destroy (GTK_WIDGET (d_widget));

		d_widget = gtk_menu_new ();
		trace ("shell %p", d_shell);
		Melder_assert (d_shell);
		trace ("shell class name %ls", Thing_className (d_shell));
		Melder_assert (d_shell -> classInfo == classGuiWindow);
		Melder_assert (((GuiWindow) d_shell) -> d_gtkMenuBar);
		GtkAccelGroup *ag = (GtkAccelGroup *) g_object_get_data (G_OBJECT (((GuiWindow) d_shell) -> d_gtkMenuBar), "accel-group");
		gtk_menu_set_accel_group (GTK_MENU (d_widget), ag);
		Melder_assert (ag);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (d_gtkMenuTitle), GTK_WIDGET (d_widget));
		gtk_widget_show (GTK_WIDGET (d_widget));
		_GuiObject_setUserData (d_widget, this);
	#elif cocoa
	#elif motif
	#endif
}

#if cocoa
	@implementation GuiCocoaMenuButton {
		GuiMenu d_userData;
	}
	- (void) dealloc {   // override
		GuiMenu me = d_userData;
		forget (me);
		trace ("deleting a menu button");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiMenu));
		d_userData = static_cast <GuiMenu> (userData);
	}
	@end
	@implementation GuiCocoaMenu {
		GuiMenu d_userData;
	}
	- (void) dealloc {   // override
		GuiMenu me = d_userData;
		forget (me);
		trace ("deleting a menu");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiMenu));
		d_userData = static_cast <GuiMenu> (userData);
	}
	@end
#endif

GuiMenu GuiMenu_createInWindow (GuiWindow window, const wchar_t *title, long flags) {
	GuiMenu me = Thing_new (GuiMenu);
	my d_shell = window;
	my d_parent = window;
	#if gtk
		trace ("create and show the menu title");
		my d_gtkMenuTitle = (GtkMenuItem *) gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		gtk_menu_shell_append (GTK_MENU_SHELL (window -> d_gtkMenuBar), GTK_WIDGET (my d_gtkMenuTitle));
		if (flags & GuiMenu_INSENSITIVE)
			gtk_widget_set_sensitive (GTK_WIDGET (my d_gtkMenuTitle), FALSE);
		gtk_widget_show (GTK_WIDGET (my d_gtkMenuTitle));
		trace ("create the menu");
		my d_widget = gtk_menu_new ();
		GtkAccelGroup *ag = (GtkAccelGroup *) g_object_get_data (G_OBJECT (window -> d_gtkMenuBar), "accel-group");
		gtk_menu_set_accel_group (GTK_MENU (my d_widget), ag);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (my d_gtkMenuTitle), GTK_WIDGET (my d_widget));
		_GuiObject_setUserData (my d_widget, me);
	#elif cocoa
		if (! theGuiCocoaApplicationDelegate) {
			int numberOfMenus = [[[NSApp mainMenu] itemArray] count];
			trace ("Number of menus: %d.", numberOfMenus);
			theGuiCocoaApplicationDelegate = [[GuiCocoaApplicationDelegate alloc] init];
			[NSApp   setDelegate: theGuiCocoaApplicationDelegate];
			theMenuBar = [[NSMenu alloc] init];
			[NSApp   setMainMenu: theMenuBar];
		}
		my d_cocoaMenu = [[GuiCocoaMenu alloc]
			initWithTitle: (NSString *) Melder_peekWcsToCfstring (title)];
		my d_widget = my d_cocoaMenu;
		[my d_cocoaMenu   setAutoenablesItems: NO];
		if (window == NULL) {
			/*
			 * Install the menu in the main OS X menu bar along the top of the screen.
			 * This is done by creating a menu item for the main menu bar,
			 * and during applicationWillFinishLaunching installing that item.
			 */
            NSString *itemTitle = (NSString *) Melder_peekWcsToCfstring (title);
			my d_cocoaMenuItem = [[GuiCocoaMenuItem alloc]
				initWithTitle: itemTitle  action: NULL   keyEquivalent: @""];

			[my d_cocoaMenuItem   setSubmenu: my d_cocoaMenu];   // the item will retain the menu...
			[my d_cocoaMenu release];   // ... so we can release the menu already (before even returning it!)
			theMenuBarItems [++ theNumberOfMenuBarItems] = my d_cocoaMenuItem;
		} else if ([(NSView *) window -> d_widget   isKindOfClass: [NSView class]]) {
			/*
			 * Install the menu at the top of a window.
			 * Menu title positioning information is maintained in that GuiWindow.
			 */
			NSRect parentRect = [(NSView *) window -> d_widget   frame];   // this is the window's top form
			int parentWidth = parentRect.size.width, parentHeight = parentRect.size.height;
			if (window -> d_menuBarWidth == 0)
				window -> d_menuBarWidth = -1;
			int width = 18 + 7 * wcslen (title), height = 35 /*25*/;
			int x = window -> d_menuBarWidth, y = parentHeight + 1 - height;
            NSUInteger resizingMask = NSViewMinYMargin;
			if (Melder_wcsequ (title, L"Help")) {
				x = parentWidth + 1 - width;
                resizingMask |= NSViewMinXMargin;
			} else {
				window -> d_menuBarWidth += width - 1;
			}
			NSRect rect = { { x, y }, { width, height } };
			my d_cocoaMenuButton = [[GuiCocoaMenuButton alloc]
				initWithFrame: rect   pullsDown: YES];
			[my d_cocoaMenuButton   setAutoenablesItems: NO];
			[my d_cocoaMenuButton   setBezelStyle: NSShadowlessSquareBezelStyle];
			[my d_cocoaMenuButton   setImagePosition: NSImageAbove];   // this centers the text
			//[nsPopupButton setBordered: NO];
            [my d_cocoaMenuButton   setAutoresizingMask: resizingMask]; // stick to top

			[[my d_cocoaMenuButton cell]   setArrowPosition: NSPopUpNoArrow /*NSPopUpArrowAtBottom*/];
			[[my d_cocoaMenuButton cell]   setPreferredEdge: NSMaxYEdge];
			/*
			 * Apparently, Cocoa swallows title setting only if there is already a menu with a dummy item.
			 */
			GuiCocoaMenuItem *item = [[GuiCocoaMenuItem alloc] initWithTitle: @"-you should never get to see this-" action: NULL keyEquivalent: @""];
			[my d_cocoaMenu   addItem: item];   // the menu will retain the item...
			[item release];   // ... so we can release the item already
			/*
			 * Install the menu button in the form.
			 */
			[(NSView *) window -> d_widget   addSubview: my d_cocoaMenuButton];   // parent will retain the button...

			[my d_cocoaMenuButton   release];   // ... so we can release the button already
			/*
			 * Attach the menu to the button.
			 */
			[my d_cocoaMenuButton   setMenu: my d_cocoaMenu];   // the button will retain the menu...
			[my d_cocoaMenu   release];   // ... so we can release the menu already (before even returning it!)
			[my d_cocoaMenuButton   setTitle: (NSString *) Melder_peekWcsToCfstring (title)];

		}
	#elif motif
		if (window == NULL) {
			my d_xmMenuTitle = XmCreateCascadeButton (theGuiTopMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
			if (wcsequ (title, L"Help"))
				XtVaSetValues (theGuiTopMenuBar, XmNmenuHelpWidget, my d_xmMenuTitle, NULL);
			my d_widget = XmCreatePulldownMenu (theGuiTopMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
			if (flags & GuiMenu_INSENSITIVE)
				XtSetSensitive (my d_xmMenuTitle, False);
			XtVaSetValues (my d_xmMenuTitle, XmNsubMenuId, my d_widget, NULL);
			XtManageChild (my d_xmMenuTitle);
		} else {
			my d_xmMenuTitle = XmCreateCascadeButton (window -> d_xmMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
			if (wcsequ (title, L"Help"))
				XtVaSetValues (window -> d_xmMenuBar, XmNmenuHelpWidget, my d_xmMenuTitle, NULL);
			my d_widget = XmCreatePulldownMenu (window -> d_xmMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
			if (flags & GuiMenu_INSENSITIVE)
				XtSetSensitive (my d_xmMenuTitle, False);
			XtVaSetValues (my d_xmMenuTitle, XmNsubMenuId, my d_widget, NULL);
			XtManageChild (my d_xmMenuTitle);
		}
		_GuiObject_setUserData (my d_widget, me);
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenu_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenu_destroyCallback, me);
	#endif
	return me;
}

GuiMenu GuiMenu_createInMenu (GuiMenu supermenu, const wchar_t *title, long flags) {
	GuiMenu me = Thing_new (GuiMenu);
	my d_shell = supermenu -> d_shell;
	my d_parent = supermenu;
	my d_menuItem = Thing_new (GuiMenuItem);
	my d_menuItem -> d_shell = my d_shell;
	my d_menuItem -> d_parent = supermenu;
	my d_menuItem -> d_menu = me;
	#if gtk
		my d_menuItem -> d_widget = gtk_menu_item_new_with_label (Melder_peekWcsToUtf8 (title));
		my d_widget = gtk_menu_new ();
		GtkAccelGroup *ag = (GtkAccelGroup *) gtk_menu_get_accel_group (GTK_MENU (supermenu -> d_widget));
		gtk_menu_set_accel_group (GTK_MENU (my d_widget), ag);
		if (flags & GuiMenu_INSENSITIVE)
			gtk_widget_set_sensitive (GTK_WIDGET (my d_widget), FALSE);
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (my d_menuItem -> d_widget), GTK_WIDGET (my d_widget));
		gtk_menu_shell_append (GTK_MENU_SHELL (supermenu -> d_widget), GTK_WIDGET (my d_menuItem -> d_widget));
		gtk_widget_show (GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (my d_menuItem -> d_widget));
		_GuiObject_setUserData (my d_widget, me);
	#elif cocoa
		trace ("creating menu item %ls", title);
		NSMenuItem *item = [[NSMenuItem alloc]
			initWithTitle: (NSString *) Melder_peekWcsToCfstring (title)
			action: NULL
			keyEquivalent: @""];
		trace ("adding the item to its supermenu %p", supermenu);
		[supermenu -> d_cocoaMenu  addItem: item];   // the menu will retain the item...
		trace ("release the item");
		[item release];   // ... so we can release the item already
		trace ("creating menu %ls", title);
		my d_cocoaMenu = [[GuiCocoaMenu alloc]
			initWithTitle: (NSString *) Melder_peekWcsToCfstring (title)];
		[my d_cocoaMenu setAutoenablesItems: NO];
		trace ("adding the new menu %p to its supermenu %p", me, supermenu);
		[supermenu -> d_cocoaMenu   setSubmenu: my d_cocoaMenu   forItem: item];   // the supermenu will retain the menu...
		Melder_assert ([my d_cocoaMenu retainCount] == 2);
		[my d_cocoaMenu release];   // ... so we can release the menu already, even before returning it
		my d_widget = my d_cocoaMenu;
	#elif motif
		my d_menuItem -> d_widget = XmCreateCascadeButton (supermenu -> d_widget, Melder_peekWcsToUtf8 (title), NULL, 0);
		my d_widget = XmCreatePulldownMenu (supermenu -> d_widget, Melder_peekWcsToUtf8 (title), NULL, 0);
		if (flags & GuiMenu_INSENSITIVE)
			XtSetSensitive (my d_menuItem -> d_widget, False);
		XtVaSetValues (my d_menuItem -> d_widget, XmNsubMenuId, my d_widget, NULL);
		XtManageChild (my d_menuItem -> d_widget);
		_GuiObject_setUserData (my d_widget, me);
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenu_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenu_destroyCallback, me);
	#endif
	return me;
}

#if gtk
static void set_position (GtkMenu *menu, gint *px, gint *py, gpointer data)
{
	gint w, h;
	GtkWidget *button = (GtkWidget *) g_object_get_data (G_OBJECT (menu), "button");

	if (GTK_WIDGET (menu) -> requisition. width < button->allocation.width)
		gtk_widget_set_size_request (GTK_WIDGET (menu), button->allocation.width, -1);

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
#endif

GuiMenu GuiMenu_createInForm (GuiForm form, int left, int right, int top, int bottom, const wchar_t *title, long flags) {
	GuiMenu me = Thing_new (GuiMenu);
	my d_shell = form -> d_shell;
	my d_parent = form;
	my d_cascadeButton = Thing_new (GuiButton);
	my d_cascadeButton -> d_shell = my d_shell;
	my d_cascadeButton -> d_parent = form;
	my d_cascadeButton -> d_menu = me;
	#if gtk
		my d_cascadeButton -> d_widget = gtk_button_new_with_label (Melder_peekWcsToUtf8 (title));
		my d_cascadeButton -> v_positionInForm (my d_cascadeButton -> d_widget, left, right, top, bottom, form);
		gtk_widget_show (GTK_WIDGET (my d_cascadeButton -> d_widget));

		my d_widget = gtk_menu_new ();
		if (flags & GuiMenu_INSENSITIVE)
			gtk_widget_set_sensitive (GTK_WIDGET (my d_widget), FALSE);

		g_signal_connect_object (G_OBJECT (my d_cascadeButton -> d_widget), "event",
			GTK_SIGNAL_FUNC (button_press), G_OBJECT (my d_widget), G_CONNECT_SWAPPED);
		g_object_set_data (G_OBJECT (my d_widget), "button", my d_cascadeButton -> d_widget);
		gtk_menu_attach_to_widget (GTK_MENU (my d_widget), GTK_WIDGET (my d_cascadeButton -> d_widget), NULL);
		gtk_button_set_alignment (GTK_BUTTON (my d_cascadeButton -> d_widget), 0.0f, 0.5f);
		_GuiObject_setUserData (my d_widget, me);
		_GuiObject_setUserData (my d_cascadeButton -> d_widget, me);
	#elif cocoa
		my d_cascadeButton -> d_widget = my d_cocoaMenuButton = [[GuiCocoaMenuButton alloc] init];
		my d_cascadeButton -> v_positionInForm (my d_cocoaMenuButton, left, right, top, bottom, form);
		[my d_cocoaMenuButton   setUserData: me];
		[my d_cocoaMenuButton   setPullsDown: YES];
		[my d_cocoaMenuButton   setAutoenablesItems: NO];
		[my d_cocoaMenuButton   setBezelStyle: NSShadowlessSquareBezelStyle];
		[my d_cocoaMenuButton   setImagePosition: NSImageAbove];   // this centers the text
		[[my d_cocoaMenuButton cell]   setArrowPosition: NSPopUpNoArrow /*NSPopUpArrowAtBottom*/];

        NSString *menuTitle = (NSString*)Melder_peekWcsToCfstring (title);
        my d_widget = my d_cocoaMenu = [[GuiCocoaMenu alloc] initWithTitle:menuTitle];
		[my d_cocoaMenu   setAutoenablesItems: NO];
		/*
		 * Apparently, Cocoa swallows title setting only if there is already a menu with a dummy item.
		 */
		NSMenuItem *item = [[NSMenuItem alloc] initWithTitle: @"-you should never get to see this-" action: NULL keyEquivalent: @""];
		[my d_cocoaMenu   addItem: item];   // the menu will retain the item...
		[item release];   // ... so we can release the item already
		/*
		 * Attach the menu to the button.
		 */
		[my d_cocoaMenuButton   setMenu: my d_cocoaMenu];   // the button will retain the menu...
		[my d_cocoaMenu   release];   // ... so we can release the menu already (before even returning it!)
		[my d_cocoaMenuButton   setTitle: (NSString *) Melder_peekWcsToCfstring (title)];
	#elif motif
		my d_xmMenuBar = XmCreateMenuBar (form -> d_widget, "dynamicSubmenuBar", 0, 0);
		form -> v_positionInForm (my d_xmMenuBar, left, right, top, bottom, form);
		my d_cascadeButton -> d_widget = XmCreateCascadeButton (my d_xmMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
		form -> v_positionInForm (my d_cascadeButton -> d_widget, 0, right - left - 4, 0, bottom - top, form);
		my d_widget = XmCreatePulldownMenu (my d_xmMenuBar, Melder_peekWcsToUtf8 (title), NULL, 0);
		if (flags & GuiMenu_INSENSITIVE)
			XtSetSensitive (my d_cascadeButton -> d_widget, False);
		XtVaSetValues (my d_cascadeButton -> d_widget, XmNsubMenuId, my d_widget, NULL);
		XtManageChild (my d_cascadeButton -> d_widget);
		XtManageChild (my d_xmMenuBar);
		_GuiObject_setUserData (my d_widget, me);
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkMenu_destroyCallback), me);
		g_signal_connect (G_OBJECT (my d_cascadeButton -> d_widget), "destroy", G_CALLBACK (_guiGtkMenuCascadeButton_destroyCallback), me);
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifMenu_destroyCallback, me);
	#endif
	return me;
};

/* End of file GuiMenu.cpp */
