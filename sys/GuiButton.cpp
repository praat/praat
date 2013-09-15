/* GuiButton.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2007-2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

Thing_implement (GuiButton, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if gtk
	#define iam_button  GuiButton me = (GuiButton) _GuiObject_getUserData (widget)
#elif cocoa
	#define iam_button  GuiButton me = (GuiButton) [(GuiCocoaButton *) widget userData];
#elif motif
	#define iam_button  GuiButton me = (GuiButton) widget -> userData
#endif

#if gtk
	static void _GuiGtkButton_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiButton);
		trace ("destroying GuiButton %p", me);
		forget (me);
	}
	static void _GuiGtkButton_activateCallback (GuiObject widget, gpointer void_me) {
		iam (GuiButton);
		struct structGuiButtonEvent event = { me, 0 };
		if (my d_activateCallback != NULL) {
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", GTK_WIDGET (widget) -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
#elif cocoa
	@implementation GuiCocoaButton {
		GuiButton d_userData;
	}
	- (void) dealloc {   // override
		GuiButton me = d_userData;
		forget (me);
		trace ("deleting a button");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiButton));
		d_userData = static_cast <GuiButton> (userData);
	}
	- (void) _guiCocoaButton_activateCallback: (id) widget {
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiButton me = d_userData;
		if (my d_activateCallback != NULL) {
			struct structGuiButtonEvent event = { me, 0 };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", "xx", "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
	@end
#elif win
	void _GuiWinButton_destroy (GuiObject widget) {
		iam_button;
		if (widget == widget -> shell -> defaultButton)
			widget -> shell -> defaultButton = NULL;   // remove dangling reference
		if (widget == widget -> shell -> cancelButton)
			widget -> shell -> cancelButton = NULL;   // remove dangling reference
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinButton_handleClick (GuiObject widget) {
		iam_button;
		if (my d_activateCallback != NULL) {
			struct structGuiButtonEvent event = { me, 0 };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", widget -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
	bool _GuiWinButton_tryToHandleShortcutKey (GuiObject widget) {
		iam_button;
		if (my d_activateCallback != NULL) {
			struct structGuiButtonEvent event = { me, 0 };
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", widget -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
			return true;
		}
		return false;
	}
#elif mac
	void _GuiMacButton_destroy (GuiObject widget) {
		iam_button;
		if (widget == widget -> shell -> defaultButton)
			widget -> shell -> defaultButton = NULL;   // remove dangling reference
		if (widget == widget -> shell -> cancelButton)
			widget -> shell -> cancelButton = NULL;   // remove dangling reference
		_GuiNativeControl_destroy (widget);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacButton_handleClick (GuiObject widget, EventRecord *macEvent) {
		iam_button;
		_GuiMac_clipOnParent (widget);
		bool pushed = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
		GuiMac_clipOff ();
		if (pushed && my d_activateCallback != NULL) {
			struct structGuiButtonEvent event = { me, 0 };
			//enum { cmdKey = 256, shiftKey = 512, optionKey = 2048, controlKey = 4096 };
			Melder_assert (macEvent -> what == mouseDown);
			event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
			event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
			event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
			event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", widget -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
		}
	}
	bool _GuiMacButton_tryToHandleShortcutKey (GuiObject widget, EventRecord *macEvent) {
		iam_button;
		if (my d_activateCallback != NULL) {
			struct structGuiButtonEvent event = { me, 0 };
			// ignore modifier keys for Enter
			try {
				my d_activateCallback (my d_activateBoss, & event);
			} catch (MelderError) {
				Melder_error_ ("Your click on button \"", widget -> name, "\" was not completely handled.");
				Melder_flushError (NULL);
			}
			return true;
		}
		return false;
	}
#endif

GuiButton GuiButton_create (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*activateCallback) (void *boss, GuiButtonEvent event), void *activateBoss, unsigned long flags)
{
	GuiButton me = Thing_new (GuiButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_activateCallback = activateCallback;
	my d_activateBoss = activateBoss;
	#if gtk
		my d_widget = gtk_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my d_widget, me);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiButton_DEFAULT || flags & GuiButton_ATTRACTIVE) {
			GTK_WIDGET_SET_FLAGS (my d_widget, GTK_CAN_DEFAULT);
			GtkWidget *shell = gtk_widget_get_toplevel (GTK_WIDGET (my d_widget));
			Melder_assert (shell != NULL);
			gtk_window_set_default (GTK_WINDOW (shell), GTK_WIDGET (my d_widget));
		} else if (1) {
			gtk_button_set_focus_on_click (GTK_BUTTON (my d_widget), false);
			GTK_WIDGET_UNSET_FLAGS (my d_widget, GTK_CAN_DEFAULT);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkButton_destroyCallback), me);
		g_signal_connect (GTK_BUTTON (my d_widget), "clicked", G_CALLBACK (_GuiGtkButton_activateCallback), me);
//		if (flags & GuiButton_CANCEL) {
//			parent -> shell -> cancelButton = parent -> cancelButton = my widget;
//		}
	#elif cocoa
		GuiCocoaButton *button = [[GuiCocoaButton alloc] init];
		my d_widget = (GuiObject) button;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[button setUserData: me];
		[button setButtonType: NSMomentaryPushInButton];
		[button setBezelStyle: NSRoundedBezelStyle];
		[button setImagePosition: NSNoImage];
		[button setBordered: YES];
		static NSFont *theButtonFont;
		if (! theButtonFont) {
			theButtonFont = [NSFont systemFontOfSize: 13.0];
		}
		[button setFont: theButtonFont];
		[button setTitle: (NSString *) Melder_peekWcsToCfstring (buttonText)];
		[button setTarget: (id) my d_widget];
		[button setAction: @selector (_guiCocoaButton_activateCallback:)];
		//[button setAutoresizingMask: NSViewNotSizable];
		if (flags & GuiButton_DEFAULT) {
			[button setKeyEquivalent: @"\r"];
		}
		if (flags & GuiButton_ATTRACTIVE) {
			//[button setKeyEquivalent: @"\r"];   // slow!
			[button highlight: YES];   // lasts only till it's clicked!
			//[button setBezelStyle: NSThickerSquareBezelStyle];
			//[button setFont: [NSFont boldSystemFontOfSize: 14.0]];
		}
	#elif win
		my d_widget = _Gui_initializeWidget (xmPushButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (my d_widget -> name),
			WS_CHILD
			| ( flags & (GuiButton_DEFAULT | GuiButton_ATTRACTIVE) ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON )
			| WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiButton_DEFAULT || flags & GuiButton_ATTRACTIVE) {
			parent -> d_widget -> shell -> defaultButton = parent -> d_widget -> defaultButton = my d_widget;
		}
		if (flags & GuiButton_CANCEL) {
			parent -> d_widget -> shell -> cancelButton = parent -> d_widget -> cancelButton = my d_widget;
		}
	#elif mac
		my d_widget = _Gui_initializeWidget (xmPushButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me);
		CreatePushButtonControl (my d_widget -> macWindow, & my d_widget -> rect, NULL, & my d_widget -> nat.control.handle);
		Melder_assert (my d_widget -> nat.control.handle != NULL);
		SetControlReference (my d_widget -> nat.control.handle, (long) my d_widget);
		my d_widget -> isControl = true;
		_GuiNativeControl_setFont (my d_widget, flags & GuiButton_ATTRACTIVE ? /*1*/0 : 0, 13);
		_GuiNativeControl_setTitle (my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiButton_DEFAULT || flags & GuiButton_ATTRACTIVE) {
			parent -> d_widget -> shell -> defaultButton = parent -> d_widget -> defaultButton = my d_widget;
			Boolean set = true;
			SetControlData (my d_widget -> nat.control.handle, kControlEntireControl, kControlPushButtonDefaultTag, sizeof (Boolean), & set);
		}
		if (flags & GuiButton_CANCEL) {
			parent -> d_widget -> shell -> cancelButton = parent -> d_widget -> cancelButton = my d_widget;
		}
	#endif
	if (flags & GuiButton_INSENSITIVE) {
		my f_setSensitive (false);
	}
	return me;
}

GuiButton GuiButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *clickedBoss, unsigned long flags)
{
	GuiButton me = GuiButton_create (parent, left, right, top, bottom, buttonText, clickedCallback, clickedBoss, flags);
	my f_show ();
	return me;
}

void structGuiButton :: f_setString (const wchar_t *text) {
	#if gtk
		gtk_button_set_label (GTK_BUTTON (d_widget), Melder_peekWcsToUtf8 (text));
	#elif cocoa
		[(NSButton *) d_widget setTitle: (NSString *) Melder_peekWcsToCfstring (text)];
	#elif motif
		Melder_free (d_widget -> name);
		d_widget -> name = Melder_wcsdup_f (text);
		_GuiNativeControl_setTitle (d_widget);
	#endif
}

/* End of file GuiButton.cpp */
