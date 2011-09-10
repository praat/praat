/* GuiRadioButton.cpp
 *
 * Copyright (C) 1993-2011 Paul Boersma
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
#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_radiobutton \
		Melder_assert (widget -> widgetClass == xmToggleButtonWidgetClass); \
		GuiRadioButton me = (GuiRadioButton) widget -> userData
#else
	#define iam_radiobutton \
		GuiRadioButton me = (GuiRadioButton) _GuiObject_getUserData (widget)
#endif

typedef struct structGuiRadioButton {
	GuiObject widget;
	void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event);
	void *valueChangedBoss;
	#if gtk
		gulong valueChangedHandlerId;
	#endif
} *GuiRadioButton;

#if gtk
	static void _GuiGtkRadioButton_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiRadioButton);
		Melder_free (me);
	}
	static void _GuiGtkRadioButton_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiRadioButton);
		struct structGuiRadioButtonEvent event = { widget };
		if (my valueChangedCallback != NULL) {
			my valueChangedCallback (my valueChangedBoss, & event);
		}
	}
#elif win || mac
	void _GuiWinMacRadioButton_destroy (GuiObject widget) {
		iam_radiobutton;
		_GuiNativeControl_destroy (widget);
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	#if win
		void _GuiWinRadioButton_handleClick (GuiObject widget) {
			iam_radiobutton;
			if (my valueChangedCallback != NULL) {
				struct structGuiRadioButtonEvent event = { widget };
				my valueChangedCallback (my valueChangedBoss, & event);
			}
		}
	#elif mac
		void _GuiMacRadioButton_handleClick (GuiObject widget, EventRecord *macEvent) {
			iam_radiobutton;
			_GuiMac_clipOnParent (widget);
			bool clicked = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
			GuiMac_clipOff ();
			if (clicked) {
				if (widget -> parent -> radioBehavior) {
					/*
					 * Deselect the sister buttons.
					 */
					for (GuiObject sibling = widget -> parent -> firstChild; sibling != NULL; sibling = sibling -> nextSibling) {
						if (sibling -> widgetClass == xmToggleButtonWidgetClass && sibling != widget)
							SetControlValue (sibling -> nat.control.handle, 0);
					}
				}
				if (my valueChangedCallback != NULL) {
					struct structGuiRadioButtonEvent event = { widget };
					my valueChangedCallback (my valueChangedBoss, & event);
				}
			}
		}
	#endif
#endif

GuiObject GuiRadioButton_create (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiRadioButton me = Melder_calloc_f (struct structGuiRadioButton, 1);
	my valueChangedCallback = valueChangedCallback;
	my valueChangedBoss = valueChangedBoss;
	#if gtk
		my widget = gtk_radio_button_new_with_label (NULL, Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my widget, me);
//		_GuiObject_position (my widget, left, right, top, bottom);
		if (GTK_IS_BOX (parent)) {
			gtk_container_add (GTK_CONTAINER (parent), GTK_WIDGET (my widget));
		}
		if (flags & GuiRadioButton_SET) {
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my widget), TRUE);
		}
		if (flags & GuiCheckButton_INSENSITIVE) {
			GuiObject_setSensitive (my widget, FALSE);
		}
		g_signal_connect (G_OBJECT (my widget), "destroy", G_CALLBACK (_GuiGtkRadioButton_destroyCallback), me);
		my valueChangedHandlerId = g_signal_connect (GTK_TOGGLE_BUTTON (my widget), "toggled", G_CALLBACK (_GuiGtkRadioButton_valueChangedCallback), me);
	#elif win
		my widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		my widget -> isRadioButton = true;
		my widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (buttonText),
			WS_CHILD
			| ( my widget -> parent -> radioBehavior ? BS_AUTORADIOBUTTON : BS_RADIOBUTTON )
			| WS_CLIPSIBLINGS,
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLongPtr (my widget -> window, GWLP_USERDATA, (LONG_PTR) my widget);
		SetWindowFont (my widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiRadioButton_SET) {
			Button_SetCheck (my widget -> window, BST_CHECKED);
		}
		if (flags & GuiRadioButton_INSENSITIVE) {
			GuiObject_setSensitive (my widget, false);
		}
	#elif mac
		my widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		my widget -> isRadioButton = true;
		CreateRadioButtonControl (my widget -> macWindow, & my widget -> rect, NULL,
			(flags & GuiRadioButton_SET) != 0, true, & my widget -> nat.control.handle);
		Melder_assert (my widget -> nat.control.handle != NULL);
		SetControlReference (my widget -> nat.control.handle, (long) my widget);
		my widget -> isControl = true;
		_GuiNativeControl_setFont (my widget, 0, 13);
		_GuiNativeControl_setTitle (my widget);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiRadioButton_INSENSITIVE) {
			GuiObject_setSensitive (my widget, false);
		}
	#endif
	return my widget;
}

GuiObject GuiRadioButton_createShown (GuiObject parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiRadioButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiObject me = GuiRadioButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	GuiObject_show (me);
	return me;
}

bool GuiRadioButton_getValue (GuiObject widget) {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif win
		value = (Button_GetState (widget -> window) & 0x0003) == BST_CHECKED;
	#elif mac
		value = GetControlValue (widget -> nat.control.handle);
	#endif
	return value;
}

void GuiRadioButton_setValue (GuiObject widget, bool value) {
	/*
	 * The value should be set without calling the valueChanged callback.
	 */
	#if gtk
		iam_radiobutton;
		g_signal_handler_disconnect (GTK_TOGGLE_BUTTON (my widget), my valueChangedHandlerId);
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (my widget), value);
		my valueChangedHandlerId = g_signal_connect (GTK_TOGGLE_BUTTON (my widget), "toggled", G_CALLBACK (_GuiGtkRadioButton_valueChangedCallback), me);
	#elif win
		Button_SetCheck (widget -> window, value ? BST_CHECKED : BST_UNCHECKED);
	#elif mac
		SetControlValue (widget -> nat.control.handle, value);
	#endif
}

#if gtk
void * GuiRadioButton_getGroup (GuiObject widget) {
	return (void *) gtk_radio_button_get_group (GTK_RADIO_BUTTON (widget));
}

void GuiRadioButton_setGroup (GuiObject widget, void *group) {
	gtk_radio_button_set_group (GTK_RADIO_BUTTON (widget), (GSList *) group);
}
#endif

/* End of file GuiRadioButton.cpp */
