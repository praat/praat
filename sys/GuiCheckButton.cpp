/* GuiCheckButton.cpp
 *
 * Copyright (C) 1993-2012,2013,2014,2015,2016,2017 Paul Boersma,
 *               2007-2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

Thing_implement (GuiCheckButton, GuiControl, 0);

#if motif
	#define iam_checkbutton \
		Melder_assert (widget -> widgetClass == xmToggleButtonWidgetClass); \
		GuiCheckButton me = (GuiCheckButton) widget -> userData
#else
	#define iam_checkbutton \
		GuiCheckButton me = (GuiCheckButton) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkCheckButton_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiCheckButton);
		forget (me);
	}
	static void _GuiGtkCheckButton_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiCheckButton);
		if (my d_valueChangedCallback && ! my d_blockValueChangedCallbacks) {
			struct structGuiCheckButtonEvent event { me };
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
#elif motif
	void _GuiWinCheckButton_destroy (GuiObject widget) {
		iam_checkbutton;
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinCheckButton_handleClick (GuiObject widget) {
		iam_checkbutton;
		if (my d_valueChangedCallback) {
			struct structGuiCheckButtonEvent event { me };
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
#elif cocoa
	@implementation GuiCocoaCheckButton {
		GuiCheckButton d_userData;
	}
	- (void) dealloc {   // override
		GuiCheckButton me = d_userData;
		forget (me);
		trace (U"deleting a check button");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiCheckButton));
		d_userData = static_cast <GuiCheckButton> (userData);
	}
	- (void) _guiCocoaButton_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiCheckButton me = d_userData;
		if (my d_valueChangedCallback) {
			Melder_assert (! my d_blockValueChangedCallbacks);
			struct structGuiCheckButtonEvent event { me };
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
	@end
#endif

GuiCheckButton GuiCheckButton_create (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiCheckButton_ValueChangedCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	autoGuiCheckButton me = Thing_new (GuiCheckButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	#if gtk
		my d_widget = gtk_check_button_new_with_label (Melder_peek32to8 (buttonText));
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), (flags & GuiCheckButton_SET) != 0);
		if (flags & GuiCheckButton_INSENSITIVE) {
			GuiThing_setSensitive (me.get(), false);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkCheckButton_destroyCallback), me.get());
		g_signal_connect (GTK_TOGGLE_BUTTON (my d_widget), "toggled", G_CALLBACK (_GuiGtkCheckButton_valueChangedCallback), me.get());
	#elif motif
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> isRadioButton = false;
		my d_widget -> window = CreateWindow (L"button", Melder_peek32toW (_GuiWin_expandAmpersands (buttonText)),
			WS_CHILD | BS_AUTOCHECKBOX | WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), false);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiCheckButton_SET) {
			Button_SetCheck (my d_widget -> window, BST_CHECKED);
		}
		if (flags & GuiCheckButton_INSENSITIVE) {
			GuiThing_setSensitive (me.get(), false);
		}
	#elif cocoa
		GuiCocoaCheckButton *checkButton = [[GuiCocoaCheckButton alloc] init];
		my d_widget = (GuiObject) checkButton;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[checkButton setUserData: me.get()];
		[checkButton setButtonType: NSSwitchButton];
		[checkButton setTitle: (NSString *) Melder_peek32toCfstring (buttonText)];
		[checkButton setTarget: checkButton];
		[checkButton setAction: @selector (_guiCocoaButton_activateCallback:)];
		if (flags & GuiCheckButton_SET) {
			[checkButton setState: NSOnState];
		}
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiCheckButton GuiCheckButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiCheckButton_ValueChangedCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	GuiCheckButton me = GuiCheckButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	GuiThing_show (me);
	return me;
}

bool GuiCheckButton_getValue (GuiCheckButton me) {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (my d_widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif motif
		value = (Button_GetState (my d_widget -> window) & 0x0003) == BST_CHECKED;
	#elif cocoa
        GuiCocoaCheckButton *checkButton = (GuiCocoaCheckButton *) my d_widget;
        value = [checkButton state] == NSOnState;
	#endif
	return value;
}

void GuiCheckButton_setValue (GuiCheckButton me, bool value) {
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), value);
	#elif motif
		Button_SetCheck (my d_widget -> window, value ? BST_CHECKED : BST_UNCHECKED);
	#elif cocoa
		GuiCocoaCheckButton *checkButton = (GuiCocoaCheckButton *) my d_widget;
		[checkButton setState: value ? NSOnState: NSOffState];
	#endif
}

/* End of file GuiCheckButton.cpp */
