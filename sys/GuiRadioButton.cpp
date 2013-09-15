/* GuiRadioButton.cpp
 *
 * Copyright (C) 1993-2011,2012,2013 Paul Boersma
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
 * pb 2007/12/26 extracted from Motif
 * sdk 2007/12/27 gtk
 * pb 2010/05/15 prevented procreation of valueChanged events in GuiRadioButton_setValue
 * pb 2010/06/14 HandleControlClick
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 */

#include "GuiP.h"

Thing_implement (GuiRadioButton, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if motif
	#define iam_radiobutton \
		Melder_assert (widget -> widgetClass == xmToggleButtonWidgetClass); \
		GuiRadioButton me = (GuiRadioButton) widget -> userData
#else
	#define iam_radiobutton \
		GuiRadioButton me = (GuiRadioButton) _GuiObject_getUserData (widget)
#endif

static int _GuiRadioButton_getPosition (GuiRadioButton me) {
	int position = 1;
	while (my d_previous) {
		position ++;
		me = my d_previous;
	}
	return position;
}

#if gtk
	static void _GuiGtkRadioButton_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiRadioButton);
		forget (me);
	}
	static void _GuiGtkRadioButton_handleToggle (GuiObject widget, gpointer void_me) {
		iam (GuiRadioButton);
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
			if (my d_valueChangedCallback != NULL && ! my d_blockValueChangedCallbacks) {
				struct structGuiRadioButtonEvent event = { me };
				event. position = _GuiRadioButton_getPosition (me);
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			}
		}
	}
#elif cocoa
	@implementation GuiCocoaRadioButton {
		GuiRadioButton d_userData;
	}
	- (void) dealloc {   // override
		GuiRadioButton me = d_userData;
		forget (me);
		trace ("deleting a radio button");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiRadioButton));
		d_userData = static_cast <GuiRadioButton> (userData);
	}
	- (void) _guiCocoaRadioButton_activateCallback: (id) widget {
		trace ("enter");
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiRadioButton me = d_userData;
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = my d_previous; sibling != NULL; sibling = sibling -> d_previous) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		for (GuiRadioButton sibling = my d_next; sibling != NULL; sibling = sibling -> d_next) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		if (my d_valueChangedCallback != NULL) {
			Melder_assert (! my d_blockValueChangedCallbacks);
			struct structGuiRadioButtonEvent event = { me };
			event. position = _GuiRadioButton_getPosition (me);
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
	@end
#elif win
	void _GuiWinRadioButton_destroy (GuiObject widget) {
		iam_radiobutton;
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinRadioButton_handleClick (GuiObject widget) {
		iam_radiobutton;
		Button_SetCheck (widget -> window, BST_CHECKED);
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = my d_previous; sibling != NULL; sibling = sibling -> d_previous) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		for (GuiRadioButton sibling = my d_next; sibling != NULL; sibling = sibling -> d_next) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		if (my d_valueChangedCallback != NULL) {
			struct structGuiRadioButtonEvent event = { me };
			event. position = _GuiRadioButton_getPosition (me);
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
#elif mac
	void _GuiMacRadioButton_destroy (GuiObject widget) {
		iam_radiobutton;
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacRadioButton_handleClick (GuiObject widget, EventRecord *macEvent) {
		iam_radiobutton;
		_GuiMac_clipOnParent (widget);
		bool clicked = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
		GuiMac_clipOff ();
		if (clicked) {
			/*
			 * Deselect the sister buttons.
			 */
			for (GuiRadioButton sibling = my d_previous; sibling != NULL; sibling = sibling -> d_previous) {
				SetControlValue (sibling -> d_widget -> nat.control.handle, 0);
			}
			for (GuiRadioButton sibling = my d_next; sibling != NULL; sibling = sibling -> d_next) {
				SetControlValue (sibling -> d_widget -> nat.control.handle, 0);
			}
			if (my d_valueChangedCallback != NULL) {
				struct structGuiRadioButtonEvent event = { me };
				event. position = _GuiRadioButton_getPosition (me);
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			}
		}
	}
#endif

static GuiRadioButton latestRadioButton = NULL;

void GuiRadioGroup_begin () {
	latestRadioButton = NULL;
}

void GuiRadioGroup_end () {
	latestRadioButton = NULL;
}

GuiRadioButton GuiRadioButton_create (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	trace ("begin: text %ls", buttonText);
	GuiRadioButton me = Thing_new (GuiRadioButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	my d_previous = latestRadioButton;
	my d_next = NULL;
	#if gtk
		my d_widget = gtk_radio_button_new_with_label_from_widget (latestRadioButton ? GTK_RADIO_BUTTON (latestRadioButton -> d_widget) : NULL, Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiRadioButton_SET) {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), TRUE);
		} else {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), FALSE);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkRadioButton_destroyCallback), me);
		g_signal_connect (GTK_TOGGLE_BUTTON (my d_widget), "toggled", G_CALLBACK (_GuiGtkRadioButton_handleToggle), me);
	#elif cocoa
		my d_cocoaRadioButton = [[GuiCocoaRadioButton alloc] init];
		my d_widget = my d_cocoaRadioButton;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[my d_cocoaRadioButton   setUserData: me];
		[my d_cocoaRadioButton setButtonType: NSRadioButton];
		[my d_cocoaRadioButton setTitle: (NSString *) Melder_peekWcsToCfstring (buttonText)];
		if (flags & GuiCheckButton_SET) {
			[my d_cocoaRadioButton setState: NSOnState];
		}
		[my d_cocoaRadioButton setTarget: my d_cocoaRadioButton];
		[my d_cocoaRadioButton setAction: @selector (_guiCocoaRadioButton_activateCallback:)];
	#elif win
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> isRadioButton = true;
		my d_widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (buttonText),
			WS_CHILD
			| ( my d_widget -> parent -> radioBehavior ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON )
			| WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiRadioButton_SET) {
			Button_SetCheck (my d_widget -> window, BST_CHECKED);
		}
	#elif mac
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> isRadioButton = true;
		CreateRadioButtonControl (my d_widget -> macWindow, & my d_widget -> rect, NULL,
			(flags & GuiRadioButton_SET) != 0, true, & my d_widget -> nat.control.handle);
		Melder_assert (my d_widget -> nat.control.handle != NULL);
		SetControlReference (my d_widget -> nat.control.handle, (long) my d_widget);
		my d_widget -> isControl = true;
		_GuiNativeControl_setFont (my d_widget, 0, 13);
		_GuiNativeControl_setTitle (my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#endif
	if (flags & GuiRadioButton_INSENSITIVE) {
		my f_setSensitive (false);
	}
	if (my d_previous) {
		Melder_assert (my d_previous != NULL);
		Melder_assert (my d_previous -> classInfo == classGuiRadioButton);
		my d_previous -> d_next = me;
	}
	latestRadioButton = me;
	trace ("end");
	return me;
}

GuiRadioButton GuiRadioButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiRadioButton me = GuiRadioButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	my f_show ();
	return me;
}

bool structGuiRadioButton :: f_getValue () {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (d_widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif cocoa
        value = [d_cocoaRadioButton state] == NSOnState;
	#elif win
		value = (Button_GetState (d_widget -> window) & 0x0003) == BST_CHECKED;
	#elif mac
		value = GetControlValue (d_widget -> nat.control.handle);
	#endif
	return value;
}

void structGuiRadioButton :: f_set () {
	GuiControlBlockValueChangedCallbacks block (this);   // the value should be set without calling the valueChanged callback (crucial on GTK)
	#if gtk
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (d_widget), TRUE);
	#elif cocoa
		[d_cocoaRadioButton   setState: NSOnState];
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = d_previous; sibling != NULL; sibling = sibling -> d_previous) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		for (GuiRadioButton sibling = d_next; sibling != NULL; sibling = sibling -> d_next) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
	#elif win
		Button_SetCheck (d_widget -> window, BST_CHECKED);
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = d_previous; sibling != NULL; sibling = sibling -> d_previous) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		for (GuiRadioButton sibling = d_next; sibling != NULL; sibling = sibling -> d_next) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
	#elif mac
		SetControlValue (d_widget -> nat.control.handle, true);
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = d_previous; sibling != NULL; sibling = sibling -> d_previous) {
			SetControlValue (sibling -> d_widget -> nat.control.handle, false);
		}
		for (GuiRadioButton sibling = d_next; sibling != NULL; sibling = sibling -> d_next) {
			SetControlValue (sibling -> d_widget -> nat.control.handle, false);
		}
	#endif
}

/* End of file GuiRadioButton.cpp */
