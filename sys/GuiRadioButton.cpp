/* GuiRadioButton.cpp
 *
 * Copyright (C) 1993-2011,2012,2013,2015,2016,2017 Paul Boersma
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
		trace (U"enter");
		if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget))) {
			trace (U"on");
			if (my d_valueChangedCallback && ! my d_blockValueChangedCallbacks) {
				struct structGuiRadioButtonEvent event { me };
				event. position = _GuiRadioButton_getPosition (me);
				my d_valueChangedCallback (my d_valueChangedBoss, & event);
			}
		}
	}
#elif motif
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
		for (GuiRadioButton sibling = my d_previous; sibling != nullptr; sibling = sibling -> d_previous) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		for (GuiRadioButton sibling = my d_next; sibling != nullptr; sibling = sibling -> d_next) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		if (my d_valueChangedCallback) {
			struct structGuiRadioButtonEvent event { me };
			event. position = _GuiRadioButton_getPosition (me);
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
#elif cocoa
	@implementation GuiCocoaRadioButton {
		GuiRadioButton d_userData;
	}
	- (void) dealloc {   // override
		GuiRadioButton me = d_userData;
		forget (me);
		trace (U"deleting a radio button");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiRadioButton));
		d_userData = static_cast <GuiRadioButton> (userData);
	}
	- (void) _guiCocoaRadioButton_activateCallback: (id) widget {
		trace (U"enter");
		Melder_assert (self == widget);   // sender (widget) and receiver (self) happen to be the same object
		GuiRadioButton me = d_userData;
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = my d_previous; sibling != nullptr; sibling = sibling -> d_previous) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		for (GuiRadioButton sibling = my d_next; sibling != nullptr; sibling = sibling -> d_next) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		if (my d_valueChangedCallback) {
			Melder_assert (! my d_blockValueChangedCallbacks);
			struct structGuiRadioButtonEvent event { me };
			event. position = _GuiRadioButton_getPosition (me);
			my d_valueChangedCallback (my d_valueChangedBoss, & event);
		}
	}
	@end
#endif

static GuiRadioButton latestRadioButton = nullptr;

void GuiRadioGroup_begin () {
	latestRadioButton = nullptr;
}

void GuiRadioGroup_end () {
	latestRadioButton = nullptr;
}

GuiRadioButton GuiRadioButton_create (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiRadioButtonCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	trace (U"begin: text %", buttonText);
	autoGuiRadioButton me = Thing_new (GuiRadioButton);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_valueChangedCallback = valueChangedCallback;
	my d_valueChangedBoss = valueChangedBoss;
	my d_previous = latestRadioButton;
	my d_next = nullptr;
	#if gtk
		my d_widget = gtk_radio_button_new_with_label_from_widget (latestRadioButton ? GTK_RADIO_BUTTON (latestRadioButton -> d_widget) : nullptr, Melder_peek32to8 (buttonText));
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiRadioButton_SET) {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), true);
		} else {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), false);
		}
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkRadioButton_destroyCallback), me.get());
		g_signal_connect (GTK_TOGGLE_BUTTON (my d_widget), "toggled", G_CALLBACK (_GuiGtkRadioButton_handleToggle), me.get());
	#elif motif
		my d_widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent -> d_widget, buttonText);
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> isRadioButton = true;
		my d_widget -> window = CreateWindow (L"button", Melder_peek32toW (_GuiWin_expandAmpersands (buttonText)),
			WS_CHILD
			| ( my d_widget -> parent -> radioBehavior ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON )
			| WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), false);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		if (flags & GuiRadioButton_SET) {
			Button_SetCheck (my d_widget -> window, BST_CHECKED);
		}
	#elif cocoa
		my d_cocoaRadioButton = [[GuiCocoaRadioButton alloc] init];
		my d_widget = my d_cocoaRadioButton;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[my d_cocoaRadioButton   setUserData: me.get()];
		[my d_cocoaRadioButton   setButtonType: NSRadioButton];
		if (true) {
			/*
				HACK:
				The button's state will be under our control,
				instead of automatically changing under the user's clicks on the neighbours.
				Make the button therefore behave as a switch button but look as a radio button.
			*/
			NSImage *image = [my d_cocoaRadioButton image], *alternateImage = [my d_cocoaRadioButton alternateImage];
			[my d_cocoaRadioButton   setButtonType: NSSwitchButton];
			[my d_cocoaRadioButton   setImage: image];
			[my d_cocoaRadioButton   setAlternateImage: alternateImage];
		}
		[my d_cocoaRadioButton setTitle: (NSString *) Melder_peek32toCfstring (buttonText)];
		if (flags & GuiCheckButton_SET) {
			[my d_cocoaRadioButton setState: NSOnState];
		}
		[my d_cocoaRadioButton setTarget: my d_cocoaRadioButton];
		[my d_cocoaRadioButton setAction: @selector (_guiCocoaRadioButton_activateCallback:)];
	#elif cocoa
		/*
			An alternate way to implement the single-button hack.
		*/
		NSRect matrixRect = NSMakeRect (20.0, 20.0, 125.0, 125.0);
		my d_cocoaRadioButton = [[GuiCocoaRadioButton alloc] initWithFrame:matrixRect];
		[my d_cocoaRadioButton   setUserData: me.get()];
		[my d_cocoaRadioButton setButtonType: NSRadioButton];
		[my d_cocoaRadioButton setTitle: (NSString *) Melder_peek32toCfstring (buttonText)];
    	NSMatrix *radioMatrix = [[NSMatrix alloc] initWithFrame: matrixRect   mode: NSRadioModeMatrix
			prototype: (NSCell *) [my d_cocoaRadioButton cell]   numberOfRows: 1   numberOfColumns: 1];
		my d_widget = (GuiObject) radioMatrix; //my d_cocoaRadioButton;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[radioMatrix   addSubview: my d_cocoaRadioButton];
		[my d_cocoaRadioButton   setUserData: me.get()];
		[my d_cocoaRadioButton setButtonType: NSRadioButton];
		[my d_cocoaRadioButton setTitle: (NSString *) Melder_peek32toCfstring (buttonText)];
		if (flags & GuiCheckButton_SET) {
			[my d_cocoaRadioButton setState: NSOnState];
		}
		[my d_cocoaRadioButton setTarget: my d_cocoaRadioButton];
		[my d_cocoaRadioButton setAction: @selector (_guiCocoaRadioButton_activateCallback:)];
	#endif
	if (flags & GuiRadioButton_INSENSITIVE) {
		GuiThing_setSensitive (me.get(), false);
	}
	if (my d_previous) {
		Melder_assert (my d_previous);
		Melder_assert (my d_previous -> classInfo == classGuiRadioButton);
		my d_previous -> d_next = me.get();
	}
	latestRadioButton = me.get();
	trace (U"end");
	return me.releaseToAmbiguousOwner();
}

GuiRadioButton GuiRadioButton_createShown (GuiForm parent, int left, int right, int top, int bottom,
	conststring32 buttonText, GuiRadioButtonCallback valueChangedCallback, Thing valueChangedBoss, uint32 flags)
{
	GuiRadioButton me = GuiRadioButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	GuiThing_show (me);
	return me;
}

bool GuiRadioButton_getValue (GuiRadioButton me) {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (my d_widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif motif
		value = (Button_GetState (my d_widget -> window) & 0x0003) == BST_CHECKED;
	#elif cocoa
        value = [my d_cocoaRadioButton state] == NSOnState;
	#endif
	return value;
}

void GuiRadioButton_set (GuiRadioButton me) {
	trace (U"enter");
	GuiControlBlockValueChangedCallbacks block (me);   // the value should be set without calling the valueChanged callback (crucial on GTK)
	#if gtk
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my d_widget), true);
	#elif motif
		Button_SetCheck (my d_widget -> window, BST_CHECKED);
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = my d_previous; sibling != nullptr; sibling = sibling -> d_previous) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
		for (GuiRadioButton sibling = my d_next; sibling != nullptr; sibling = sibling -> d_next) {
			Button_SetCheck (sibling -> d_widget -> window, BST_UNCHECKED);
		}
	#elif cocoa
		[my d_cocoaRadioButton   setState: NSOnState];
		/*
		 * Deselect the sister buttons.
		 */
		for (GuiRadioButton sibling = my d_previous; sibling != nullptr; sibling = sibling -> d_previous) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
		for (GuiRadioButton sibling = my d_next; sibling != nullptr; sibling = sibling -> d_next) {
			[sibling -> d_cocoaRadioButton   setState: NSOffState];
		}
	#endif
	trace (U"exit");
}

/* End of file GuiRadioButton.cpp */
