/* GuiCheckButton.c
 *
 * Copyright (C) 1993-2007 Paul Boersma
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
 */

#include "GuiP.h"
#define my  me ->
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_checkbutton \
		Melder_assert (widget -> widgetClass == xmToggleButtonWidgetClass); \
		GuiCheckButton me = widget -> userData
#else
	#define iam_checkbutton \
		GuiCheckButton me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiCheckButton {
	Widget widget;
	void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event);
	void *valueChangedBoss;
} *GuiCheckButton;

#if gtk
	static void _GuiGtkCheckButton_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiCheckButton);
		Melder_free (me);
	}
	static void _GuiGtkCheckButton_valueChangedCallback (Widget widget, gpointer void_me) {
		iam (GuiCheckButton);
		struct structGuiCheckButtonEvent event = { widget };
		if (my valueChangedCallback != NULL) {
			my valueChangedCallback (my valueChangedBoss, & event);
		}
	}
#elif win || mac
	void _GuiWinMacCheckButton_destroy (Widget widget) {
		iam_checkbutton;
		_GuiNativeControl_destroy (widget);
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	#if win
		void _GuiWinCheckButton_handleClick (Widget widget) {
			iam_checkbutton;
			if (my valueChangedCallback != NULL) {
				struct structGuiCheckButtonEvent event = { widget };
				my valueChangedCallback (my valueChangedBoss, & event);
			}
		}
	#elif mac
		void _GuiMacCheckButton_handleClick (Widget widget, EventRecord *macEvent) {
			iam_checkbutton;
			_GuiMac_clipOnParent (widget);
			bool clicked = TrackControl (widget -> nat.control.handle, macEvent -> where, NULL);
			GuiMac_clipOff ();
			if (clicked) {
				if (my valueChangedCallback != NULL) {
					struct structGuiCheckButtonEvent event = { widget };
					my valueChangedCallback (my valueChangedBoss, & event);
				}
			}
		}
	#endif
#else
	static void _GuiMotifCheckButton_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiCheckButton);
		(void) widget;
		(void) call;
		Melder_free (me);
	}
	static void _GuiMotifCheckButton_valueChangedCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiCheckButton);
		(void) call;
		struct structGuiCheckButtonEvent event = { widget };
		if (my valueChangedCallback != NULL) {
			my valueChangedCallback (my valueChangedBoss, & event);
		}
	}
#endif

Widget GuiCheckButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	GuiCheckButton me = Melder_calloc (struct structGuiCheckButton, 1);
	my valueChangedCallback = valueChangedCallback;
	my valueChangedBoss = valueChangedBoss;
	#if gtk
		my widget = gtk_check_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		gtk_box_pack_start (GTK_BOX (parent), my widget, TRUE, FALSE, 0);
		g_signal_connect (G_OBJECT (my widget), "destroy",
			G_CALLBACK (_GuiGtkCheckButton_destroyCallback), me);
		g_signal_connect (GTK_TOGGLE_BUTTON (my widget), "toggled",   // gtk_check_button inherits from gtk_toggle_button
			G_CALLBACK (_GuiGtkCheckButton_valueChangedCallback), me);
	#elif win
		my widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		my widget -> isRadioButton = false;
		my widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (buttonText),
			WS_CHILD | BS_AUTOCHECKBOX | WS_CLIPSIBLINGS,
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		SetWindowFont (my widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiCheckButton_SET) {
			Button_SetCheck (my widget -> window, BST_CHECKED);
		}
		if (flags & GuiCheckButton_INSENSITIVE) {
			GuiObject_setSensitive (my widget, false);
		}
	#elif mac
		my widget = _Gui_initializeWidget (xmToggleButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		my widget -> isRadioButton = false;
		CreateCheckBoxControl (my widget -> macWindow, & my widget -> rect, NULL,
			(flags & GuiCheckButton_SET) != 0, true, & my widget -> nat.control.handle);
		Melder_assert (my widget -> nat.control.handle != NULL);
		SetControlReference (my widget -> nat.control.handle, (long) my widget);
		my widget -> isControl = true;
		_GuiNativeControl_setFont (my widget, 12);
		_GuiNativeControl_setTitle (my widget);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiCheckButton_INSENSITIVE) {
			GuiObject_setSensitive (my widget, false);
		}
	#elif motif
		my widget = XtVaCreateWidget (Melder_peekWcsToUtf8 (buttonText), xmToggleButtonWidgetClass, parent,
			XmNindicatorType, XmN_OF_MANY,   // a check box, not a radio button
			XmNset, (flags & GuiCheckButton_SET) != 0,
			XmNsensitive, (flags & GuiCheckButton_INSENSITIVE) == 0,
			NULL);
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifCheckButton_destroyCallback, me);
		XtAddCallback (my widget, XmNvalueChangedCallback, _GuiMotifCheckButton_valueChangedCallback, me);
	#endif
	return my widget;
}

Widget GuiCheckButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*valueChangedCallback) (void *boss, GuiCheckButtonEvent event), void *valueChangedBoss, unsigned long flags)
{
	Widget me = GuiCheckButton_create (parent, left, right, top, bottom, buttonText, valueChangedCallback, valueChangedBoss, flags);
	GuiObject_show (me);
	return me;
}

bool GuiCheckButton_getValue (Widget widget) {
	bool value = false;
	#if gtk
		value = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget));   // gtk_check_button inherits from gtk_toggle_button
	#elif win
		value = (Button_GetState (widget -> window) & 0x0003) == BST_CHECKED;
	#elif mac
		value = GetControlValue (widget -> nat.control.handle);
	#elif motif
		value = XmToggleButtonGetState (widget);
	#endif
	return value;
}

void GuiCheckButton_setValue (Widget widget, bool value) {
	#if gtk
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), value);   // gtk_check_button inherits from gtk_toggle_button
	#elif win
		Button_SetCheck (widget -> window, value ? BST_CHECKED : BST_UNCHECKED);
	#elif mac
		SetControlValue (widget -> nat.control.handle, value);
	#elif motif
		XmToggleButtonSetState (widget, value, False);
	#endif
}

/* End of file GuiCheckButton.c */
