/* GuiCheckButton.cpp
 *
 * Copyright (C) 1993-2012,2013 Paul Boersma, 2007-2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

Thing_implement (GuiCheckButton, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
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
		if (my d_valueChangedCallback != NULL && ! my d_blockValueChangedCallbacks) {
			struct structGuiCheckButtonEvent event = { me };
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
		trace ("deleting a check button");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiCheckButton));
		d_userData = static_cast <GuiCheckButton> (userData);
	}
	- (void) _guiCocoaButton_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiCheckButton me = d_userData;
		if (my d_valueChangedCallback != NULL) {
			Melder_assert (! my d_blockValueChangedCallbacks);
			struct structGuiCheckButtonEvent event = { me };
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
@end

#elif win
	void _GuiWinCheckButton_destroy (GuiObject widget) {
		iam_checkbutton;
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinCheckButton_handleClick (GuiObject widget) {
		iam_checkbutton;
		if (my d_valueChangedCallback != NULL) {
			struct structGuiCheckButtonEvent event = { me };
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
#elif mac
	void _GuiMacCheckButton_destroy (GuiObject widget) {
		iam_checkbutton;
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacCheckButton_handleClick (GuiObject widget, EventRecord *macEvent) {
		iam_checkbutton;
		_GuiMac_clipOnParent (widget);
		bool clicked = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
		GuiMac_clipOff ();
		if (clicked) {
			if (my d_valueChangedCallback != NULL) {
				struct structGuiCheckButtonEvent event = { me };
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			}
		}
	}
#endif

GuiCheckButton GuiCheckButton_create (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiCheckButton me = Thing_new (GuiCheckButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	#if gtk
		my d_widget = gtk_check_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), (flags & GuiCheckButton_SET) != 0);
		if (flags & GuiCheckButton_INSENSITIVE) {
			my f_setSensitive (false);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkCheckButton_destroyCallback), me);
		g_signal_connect (GTK_TOGGLE_BUTTON (my d_widget), "toggled", G_CALLBACK (_GuiGtkCheckButton_valueChangedCallback), me);
	#elif cocoa
		GuiCocoaCheckButton *checkButton = [[GuiCocoaCheckButton alloc] init];
		my d_widget = (GuiObject) checkButton;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[checkButton setUserData: me];
		[checkButton setButtonType: NSSwitchButton];
		[checkButton setTitle: (NSString *) Melder_peekWcsToCfstring (buttonText)];
		[checkButton setTarget: checkButton];
		[checkButton setAction: @selector (_guiCocoaButton_activateCallback:)];
		if (flags & GuiCheckButton_SET) {
			[checkButton setState: NSOnState];
		}
	#elif win
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> isRadioButton = false;
		my d_widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (buttonText),
			WS_CHILD | BS_AUTOCHECKBOX | WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiCheckButton_SET) {
			Button_SetCheck (my d_widget -> window, BST_CHECKED);
		}
		if (flags & GuiCheckButton_INSENSITIVE) {
			my f_setSensitive (false);
		}
	#elif mac
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> isRadioButton = false;
		CreateCheckBoxControl (my d_widget -> macWindow, & my d_widget -> rect, NULL,
			(flags & GuiCheckButton_SET) != 0, true, & my d_widget -> nat.control.handle);
		Melder_assert (my d_widget -> nat.control.handle != NULL);
		SetControlReference (my d_widget -> nat.control.handle, (long) my d_widget);
		my d_widget -> isControl = true;
		_GuiNativeControl_setFont (my d_widget, 0, 13);
		_GuiNativeControl_setTitle (my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiCheckButton_INSENSITIVE) {
			my f_setSensitive (false);
		}
	#endif
	return me;
}

GuiCheckButton GuiCheckButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiCheckButton me = GuiCheckButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	my f_show ();
	return me;
}

bool structGuiCheckButton :: f_getValue () {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d_widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif cocoa
        GuiCocoaCheckButton *checkButton = (GuiCocoaCheckButton*)d_widget;
        value = [checkButton state] == NSOnState;
	#elif win
		value = (Button_GetState (d_widget -> window) & 0x0003) == BST_CHECKED;
	#elif mac
		value = GetControlValue (d_widget -> nat.control.handle);
	#endif
	return value;
}

void structGuiCheckButton :: f_setValue (bool value) {
	/*
	 * The value should be set without calling the valueChanged callback.
	 */
	#if gtk
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d_widget), value);
	#elif cocoa
		GuiCocoaCheckButton *checkButton = (GuiCocoaCheckButton *) d_widget;
		[checkButton setState: value ? NSOnState: NSOffState];
	#elif win
		Button_SetCheck (d_widget -> window, value ? BST_CHECKED : BST_UNCHECKED);
	#elif mac
		SetControlValue (d_widget -> nat.control.handle, value);
	#endif
}

/* End of file GuiCheckButton.cpp */
