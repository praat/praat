/* GuiButton.c
 *
 * Copyright (C) 1993-2010 Paul Boersma
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
 * pb & sdk 2007/12/25 gtk
 * fb 2010/02/23 GTK
 * pb 2010/06/14 HandleControlClick
 */

#include "GuiP.h"
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_button \
		Melder_assert (widget -> widgetClass == xmPushButtonWidgetClass); \
		GuiButton me = widget -> userData
#else
	#define iam_button \
		GuiButton me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiButton {
	Widget widget;
	void (*activateCallback) (void *boss, GuiButtonEvent event);
	void *activateBoss;
} *GuiButton;

#if gtk
	static void _GuiGtkButton_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiButton);
		Melder_free (me);
	}
	static void _GuiGtkButton_activateCallback (Widget widget, gpointer void_me) {
		iam (GuiButton);
		struct structGuiButtonEvent event = { widget, 0 };
		if (my activateCallback != NULL) {
			my activateCallback (my activateBoss, & event);
		}
	}
#elif win || mac
	void _GuiWinMacButton_destroy (Widget widget) {
		iam_button;
		if (widget == widget -> shell -> defaultButton)
			widget -> shell -> defaultButton = NULL;   // remove dangling reference
		if (widget == widget -> shell -> cancelButton)
			widget -> shell -> cancelButton = NULL;   // remove dangling reference
		_GuiNativeControl_destroy (widget);
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	#if win
		void _GuiWinButton_handleClick (Widget widget) {
			iam_button;
			if (my activateCallback != NULL) {
				struct structGuiButtonEvent event = { widget, 0 };
				my activateCallback (my activateBoss, & event);
			}
		}
		bool _GuiWinButton_tryToHandleShortcutKey (Widget widget) {
			iam_button;
			if (my activateCallback != NULL) {
				struct structGuiButtonEvent event = { widget, 0 };
				my activateCallback (my activateBoss, & event);
				return true;
			}
			return false;
		}
	#elif mac
		void _GuiMacButton_handleClick (Widget widget, EventRecord *macEvent) {
			iam_button;
			_GuiMac_clipOnParent (widget);
			bool pushed = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
			GuiMac_clipOff ();
			if (pushed && my activateCallback != NULL) {
				struct structGuiButtonEvent event = { widget, 0 };
				//enum { cmdKey = 256, shiftKey = 512, optionKey = 2048, controlKey = 4096 };
				Melder_assert (macEvent -> what == mouseDown);
				event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
				event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
				event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
				event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
				my activateCallback (my activateBoss, & event);
			}
		}
		bool _GuiMacButton_tryToHandleShortcutKey (Widget widget, EventRecord *macEvent) {
			iam_button;
			if (my activateCallback != NULL) {
				struct structGuiButtonEvent event = { widget, 0 };
				// ignore modifier keys for Enter
				my activateCallback (my activateBoss, & event);
				return true;
			}
			return false;
		}
	#endif
#else
	static void _GuiMotifButton_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiButton);
		Melder_free (me);
	}
	static void _GuiMotifButton_activateCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiButton);
		struct structGuiButtonEvent event = { widget, 0 };
		XButtonPressedEvent *xevent = (XButtonPressedEvent *) ((XmDrawingAreaCallbackStruct *) call) -> event;
		if (xevent -> type == ButtonPress) {
			event. shiftKeyPressed = ( xevent -> state & ShiftMask ) != 0;
			event. commandKeyPressed = ( xevent -> state & ControlMask ) != 0;
			event. optionKeyPressed = ( xevent -> state & Mod1Mask ) != 0;
			event. extraControlKeyPressed = false;
		} else {
			(void) 0;   // ignore modifier keys for Enter
		}
		if (my activateCallback != NULL) {
			my activateCallback (my activateBoss, & event);
		}
	}
#endif

Widget GuiButton_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*activateCallback) (void *boss, GuiButtonEvent event), void *activateBoss, unsigned long flags)
{
	GuiButton me = Melder_calloc (struct structGuiButton, 1);
	my activateCallback = activateCallback;
	my activateBoss = activateBoss;
	#if gtk
		my widget = gtk_button_new_with_label (Melder_peekWcsToUtf8 (buttonText));
		_GuiObject_setUserData (my widget, me);
//		_GuiObject_position (my widget, left, right, top, bottom);

		/* TODO: dit moet eigenlijk netter, problemen zijn er al met focus van
		 * dialogbox */
		// TODO: use gtk_box_pack_start(GTK_BOX(parent), my widget, FALSE, FALSE, ?)
		if (parent)
			gtk_container_add (GTK_CONTAINER (parent), my widget);
		
		g_signal_connect (G_OBJECT (my widget), "destroy",
				  G_CALLBACK (_GuiGtkButton_destroyCallback), me);
		g_signal_connect (GTK_BUTTON (my widget), "clicked",
				  G_CALLBACK (_GuiGtkButton_activateCallback), me);
		if (flags & GuiButton_DEFAULT) {
			// TODO: Werkt nog niet
			GTK_WIDGET_SET_FLAGS (my widget, GTK_CAN_DEFAULT);
			GTK_WIDGET_SET_FLAGS (my widget, GTK_HAS_DEFAULT);
			gtk_widget_activate (my widget);
			gtk_widget_grab_default (my widget);
			gtk_widget_grab_focus (my widget);
		}
//		if (flags & GuiButton_CANCEL) {
//			parent -> shell -> cancelButton = parent -> cancelButton = my widget;
//		}
	#elif win
		my widget = _Gui_initializeWidget (xmPushButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		my widget -> window = CreateWindow (L"button", _GuiWin_expandAmpersands (my widget -> name),
			WS_CHILD
			| ( flags & GuiButton_DEFAULT ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON )
			| WS_CLIPSIBLINGS,
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		SetWindowFont (my widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiButton_DEFAULT) {
			parent -> shell -> defaultButton = parent -> defaultButton = my widget;
		}
		if (flags & GuiButton_CANCEL) {
			parent -> shell -> cancelButton = parent -> cancelButton = my widget;
		}
	#elif mac
		my widget = _Gui_initializeWidget (xmPushButtonWidgetClass, parent, buttonText);
		_GuiObject_setUserData (my widget, me);
		CreatePushButtonControl (my widget -> macWindow, & my widget -> rect, NULL, & my widget -> nat.control.handle);
		Melder_assert (my widget -> nat.control.handle != NULL);
		SetControlReference (my widget -> nat.control.handle, (long) my widget);
		my widget -> isControl = true;
		_GuiNativeControl_setFont (my widget, 13);
		_GuiNativeControl_setTitle (my widget);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiButton_DEFAULT) {
			parent -> shell -> defaultButton = parent -> defaultButton = my widget;
			Boolean set = true;
			SetControlData (my widget -> nat.control.handle, kControlEntireControl, kControlPushButtonDefaultTag, sizeof (Boolean), & set);
		}
		if (flags & GuiButton_CANCEL) {
			parent -> shell -> cancelButton = parent -> cancelButton = my widget;
		}
	#elif motif
		my widget = XtVaCreateWidget (Melder_peekWcsToUtf8 (buttonText), xmPushButtonWidgetClass, parent, XmNalignment, XmALIGNMENT_CENTER, NULL);
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifButton_destroyCallback, me);
		XtAddCallback (my widget, XmNactivateCallback, _GuiMotifButton_activateCallback, me);
		if (flags & GuiButton_DEFAULT) {
			XtVaSetValues (parent, XmNdefaultButton, my widget, NULL);
		}
		if (flags & GuiButton_CANCEL) {
			XtVaSetValues (parent, XmNcancelButton, my widget, NULL);
		}
	#endif
	if (flags & GuiButton_INSENSITIVE) {
		GuiObject_setSensitive (my widget, false);
	}

	return my widget;
}

Widget GuiButton_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *buttonText, void (*clickedCallback) (void *boss, GuiButtonEvent event), void *clickedBoss, unsigned long flags)
{
	Widget me = GuiButton_create (parent, left, right, top, bottom, buttonText, clickedCallback, clickedBoss, flags);
	GuiObject_show (me);
	return me;
}

void GuiButton_setString (Widget widget, const wchar_t *text) {
	#if gtk
		gtk_button_set_label (GTK_BUTTON (widget), Melder_peekWcsToUtf8 (text));
	#elif win || mac
		Melder_free (widget -> name);
		widget -> name = Melder_wcsdup (text);
		_GuiNativeControl_setTitle (widget);
	#elif motif
		char *text_utf8 = Melder_peekWcsToUtf8 (text);
		XtVaSetValues (widget, XtVaTypedArg, XmNlabelString, XmRString, text_utf8, strlen (text_utf8), NULL);
	#endif
}

/* End of file GuiButton.c */
