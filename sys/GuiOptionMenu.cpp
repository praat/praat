/* GuiOptionMenu.cpp
 *
 * Copyright (C) 1993-2018 Paul Boersma, 2007 Stefan de Konink, 2013 Tom Naughton
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "GuiP.h"

Thing_implement (GuiOptionMenu, GuiControl, 0);

#if gtk
	static void _guiGtkOptionMenu_destroyCallback (GuiObject /* widget */, gpointer void_me) {
		iam (GuiOptionMenu);
		forget (me);
	}
#elif motif
	static void _guiMotifOptionMenu_destroyCallback (GuiObject /* widget */, XtPointer void_me, XtPointer /* call */) {
		iam (GuiOptionMenu);
		forget (me);
	}
#elif cocoa
	@implementation GuiCocoaOptionMenu {
		GuiOptionMenu d_userData;
	}
	- (void) dealloc {   // override
		GuiOptionMenu me = d_userData;
        [self removeAllItems];
        [self setMenu:nil];
        
		forget (me);
		trace (U"deleting an option menu");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		d_userData = (GuiOptionMenu) userData;
	}
	@end
#endif

void structGuiOptionMenu :: v_show () {
	#if gtk
		GuiOptionMenu_Parent :: v_show ();
	#elif motif
		XtManageChild (d_xmMenuBar);
    #elif cocoa
		//NSLog(@"cocoa structGuiOptionMenu :: v_show"); // ?
	#endif
}

void GuiOptionMenu_init (GuiOptionMenu me, GuiForm parent, int left, int right, int top, int bottom, uint32 flags)
{
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_combo_box_text_new ();
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), right - left, bottom - top + 8);
		gtk_fixed_put (GTK_FIXED (parent -> d_widget), GTK_WIDGET (my d_widget), left, top - 6);
		gtk_widget_set_focus_on_click (GTK_WIDGET (my d_widget), false);
		gtk_widget_set_can_default (GTK_WIDGET (my d_widget), false);
	#elif motif
		my d_xmMenuBar = XmCreateMenuBar (parent -> d_widget, "UiOptionMenu", nullptr, 0);
		XtVaSetValues (my d_xmMenuBar, XmNx, left - 4, XmNy, top - 4,
			XmNwidth, right - left + 8, XmNheight, bottom - top + 8, nullptr);
		my d_xmCascadeButton = XmCreateCascadeButton (my d_xmMenuBar, "choice", nullptr, 0);
		my d_widget = XmCreatePulldownMenu (my d_xmMenuBar, "choice", nullptr, 0);
		if (flags & GuiMenu_INSENSITIVE)
			XtSetSensitive (my d_widget, False);
		XtVaSetValues (my d_xmCascadeButton, XmNsubMenuId, my d_widget, nullptr);
		XtManageChild (my d_xmCascadeButton);
		XtVaSetValues (my d_xmMenuBar, XmNwidth, right - left + 8, nullptr);   // BUG: twice?
		XtVaSetValues (my d_xmCascadeButton, XmNx, 4, XmNy, 4, XmNwidth, right - left, XmNheight, bottom - top, nullptr);
	#elif cocoa
        GuiCocoaOptionMenu *optionMenu = [[GuiCocoaOptionMenu alloc] init];
        my d_widget = (GuiObject) optionMenu;
		my v_positionInForm (my d_widget, left, right, top - 1, bottom + 1, parent);
        [optionMenu   setUserData: me];
//        [optionMenu setBezelStyle: NSRoundedBezelStyle];
//        [optionMenu setBordered: NO];
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkOptionMenu_destroyCallback), me);
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifOptionMenu_destroyCallback, me);
	#elif cocoa
	#endif
}

GuiOptionMenu GuiOptionMenu_create (GuiForm parent, int left, int right, int top, int bottom, uint32 flags) {
	autoGuiOptionMenu me = Thing_new (GuiOptionMenu);
	GuiOptionMenu_init (me.get(), parent, left, right, top, bottom, flags);
	return me.releaseToAmbiguousOwner();
}

GuiOptionMenu GuiOptionMenu_createShown (GuiForm parent, int left, int right, int top, int bottom, uint32 flags) {
	GuiOptionMenu me = GuiOptionMenu_create (parent, left, right, top, bottom, flags);
	GuiThing_show (me);
	return me;
}

#if motif
	static void cb_optionChanged (GuiObject w, XtPointer void_me, XtPointer call) {
		iam (GuiOptionMenu);
		(void) call;
		for (int i = 1; i <= my d_options.size; i ++) {
			GuiMenuItem item = my d_options.at [i];
			if (item -> d_widget == w) {
				XtVaSetValues (my d_xmCascadeButton, XmNlabelString, Melder_peek32to8 (item -> d_widget -> name.get()), nullptr);
				XmToggleButtonSetState (item -> d_widget, true, false);
				if (Melder_debug == 11) {
					Melder_warning (i, U" \"", item -> d_widget -> name.get(), U"\"");
				}
			} else {
				XmToggleButtonSetState (item -> d_widget, false, false);
			}
		}
	}
#endif

void GuiOptionMenu_addOption (GuiOptionMenu me, conststring32 text) {
	#if gtk
		gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (my d_widget), Melder_peek32to8 (text));
	#elif motif
		autoGuiMenuItem menuItem = Thing_new (GuiMenuItem);
		menuItem -> d_widget = XtVaCreateManagedWidget (Melder_peek32to8 (text), xmToggleButtonWidgetClass, my d_widget, nullptr);
		XtAddCallback (menuItem -> d_widget, XmNvalueChangedCallback, cb_optionChanged, (XtPointer) me);
		my d_options. addItem_move (menuItem.move());
    #elif cocoa
        GuiCocoaOptionMenu *menu = (GuiCocoaOptionMenu *) my d_widget;
        [menu addItemWithTitle: [NSString stringWithUTF8String: Melder_peek32to8 (text)]];
	#endif
}

int GuiOptionMenu_getValue (GuiOptionMenu me) {
	my d_value = 0;
	#if gtk
		// TODO: Graag even een check :)
		my d_value = gtk_combo_box_get_active (GTK_COMBO_BOX (my d_widget)) + 1;
	#elif motif
		for (int i = 1; i <= my d_options.size; i ++) {
			GuiMenuItem menuItem = my d_options.at [i];
			if (XmToggleButtonGetState (menuItem -> d_widget))
				my d_value = i;
		}
    #elif cocoa
		GuiCocoaOptionMenu *menu = (GuiCocoaOptionMenu *) my d_widget;
		my d_value = [menu indexOfSelectedItem] + 1;
	#endif
	return my d_value;
}

void GuiOptionMenu_setValue (GuiOptionMenu me, int value) {
	#if gtk
		gtk_combo_box_set_active (GTK_COMBO_BOX (my d_widget), value - 1);
	#elif motif
		for (int i = 1; i <= my d_options.size; i ++) {
			GuiMenuItem menuItem = my d_options.at [i];
			XmToggleButtonSetState (menuItem -> d_widget, i == value, False);
			if (i == value) {
				XtVaSetValues (my d_xmCascadeButton, XmNlabelString, Melder_peek32to8 (menuItem -> d_widget -> name.get()), nullptr);
			}
		}
	#elif cocoa
        GuiCocoaOptionMenu *menu = (GuiCocoaOptionMenu *) my d_widget;
        [menu   selectItemAtIndex: value - 1];
	#endif
	my d_value = value;
}

/* End of file GuiOptionMenu.cpp */
