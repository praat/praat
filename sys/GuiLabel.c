/* GuiLabel.c
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
 * pb & sdk 2007/12/28 gtk
 */

#include "GuiP.h"
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_label \
		Melder_assert (widget -> widgetClass == xmLabelWidgetClass); \
		GuiLabel me = widget -> userData
#else
	#define iam_label \
		GuiLabel me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiLabel {
	Widget widget;
} *GuiLabel;

#if gtk
	static void _GuiGtkLabel_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiLabel);
		Melder_free (me);
	}
#elif win || mac
	void _GuiWinMacLabel_destroy (Widget widget) {
		iam_label;
		_GuiNativeControl_destroy (widget);
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
#else
	static void _GuiMotifButton_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiLabel);
		Melder_free (me);
	}
#endif

Widget GuiLabel_create (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags)
{
	GuiLabel me = Melder_calloc (struct structGuiLabel, 1);
	#if gtk
		my widget = gtk_label_new (Melder_peekWcsToUtf8 (labelText));
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		gtk_box_pack_start (GTK_BOX (parent), my widget, TRUE, FALSE, 0);
		g_signal_connect (G_OBJECT (my widget), "destroy",
				  G_CALLBACK (_GuiGtkLabel_destroyCallback), me);
		gtk_label_set_justify(flags);
	#elif win
		my widget = _Gui_initializeWidget (xmLabelWidgetClass, parent, labelText);
		_GuiObject_setUserData (my widget, me);
		my widget -> window = CreateWindow (L"static", _GuiWin_expandAmpersands (my widget -> name),
			WS_CHILD
			| ( flags & GuiLabel_RIGHT ? SS_RIGHT : flags & GuiLabel_CENTRE ? SS_CENTER : SS_LEFT )
			| SS_CENTERIMAGE,
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		SetWindowFont (my widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		_GuiObject_position (my widget, left, right, top, bottom);
	#elif mac
		my widget = _Gui_initializeWidget (xmLabelWidgetClass, parent, labelText);
		_GuiObject_setUserData (my widget, me);
		ControlFontStyleRec macFontStyleRecord = { 0 };   // BUG: _GuiNativeControl_setFont will reset alignment (should do inheritance)
		macFontStyleRecord. flags = kControlUseFontMask | kControlUseSizeMask | kControlUseJustMask;
		macFontStyleRecord. font = systemFont;
		macFontStyleRecord. size = 12;
		macFontStyleRecord. just = ( flags & GuiLabel_RIGHT ? teFlushRight : flags & GuiLabel_CENTRE ? teCenter : teFlushLeft );
		CreateStaticTextControl (my widget -> macWindow, & my widget -> rect, NULL, & macFontStyleRecord, & my widget -> nat.control.handle);
		Melder_assert (my widget -> nat.control.handle != NULL);
		SetControlReference (my widget -> nat.control.handle, (long) my widget);
		my widget -> isControl = true;
		_GuiNativeControl_setTitle (my widget);
		_GuiObject_position (my widget, left, right, top, bottom);
	#elif motif
		my widget = XtVaCreateWidget (Melder_peekWcsToUtf8 (labelText), xmLabelWidgetClass, parent,
			XmNalignment, ( flags & GuiLabel_RIGHT ? XmALIGNMENT_END : flags & GuiLabel_CENTRE ? XmALIGNMENT_CENTER : XmALIGNMENT_BEGINNING ), NULL);
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifButton_destroyCallback, me);
	#endif
	return my widget;
}

Widget GuiLabel_createShown (Widget parent, int left, int right, int top, int bottom,
	const wchar_t *labelText, unsigned long flags)
{
	Widget me = GuiLabel_create (parent, left, right, top, bottom, labelText, flags);
	GuiObject_show (me);
	return me;
}

void GuiLabel_setString (Widget widget, const wchar_t *text) {
	#if gtk
		gtk_label_set_text (widget, Melder_peekWcsToUtf8 (text));
	#elif win || mac
		Melder_free (widget -> name);
		widget -> name = Melder_wcsdup (text);
		_GuiNativeControl_setTitle (widget);
	#elif motif
		char *text_utf8 = Melder_peekWcsToUtf8 (text);
		XtVaSetValues (widget, XtVaTypedArg, XmNlabelString, XmRString, text_utf8, strlen (text_utf8), NULL);
	#endif
}

/* End of file GuiLabel.c */
