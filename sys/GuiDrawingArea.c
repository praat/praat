/* GuiDrawingArea.c
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
 * pb 2007/12/28 extracted from Motif
 */

#include "GuiP.h"
#define my  me ->
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_drawingarea \
		Melder_assert (widget -> widgetClass == xmDrawingAreaWidgetClass); \
		GuiDrawingArea me = widget -> userData
#else
	#define iam_drawingarea \
		GuiDrawingArea me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiDrawingArea {
	Widget widget;
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event);
	void *exposeBoss;
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event);
	void *clickBoss;
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event);
	void *keyBoss;
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event);
	void *resizeBoss;
} *GuiDrawingArea;

#if gtk
	static void _GuiGtkDrawingArea_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiDrawingArea);
		Melder_free (me);
	}
	static void _GuiGtkDrawingArea_activateCallback (Widget widget, gpointer void_me) {
		iam (GuiDrawingArea);
		struct structGuiDrawingAreaEvent event = { widget, 0 };
		if (my activateCallback != NULL) {
			my activateCallback (my activateBoss, & event);
		}
	}
#elif win || mac
	void _GuiWinMacDrawingArea_destroy (Widget widget) {
		iam_drawingarea;
		_GuiNativeControl_destroy (widget);
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	#if win
		void _GuiWinDrawingArea_update (Widget widget) {
			iam_drawingarea;
			PAINTSTRUCT paintStruct;
			BeginPaint (widget -> window, & paintStruct);
			if (my exposeCallback) {
				struct structGuiDrawingAreaExposeEvent event = { widget };
				my exposeCallback (my exposeBoss, & event);
			}
			EndPaint (widget -> window, & paintStruct);
		}
		void _GuiWinDrawingArea_handleClick (Widget widget, int x, int y) {
			iam_drawingarea;
			if (my clickCallback) {
				struct structGuiDrawingAreaClickEvent event = { widget, 0 };
				event. x = x;
				event. y = y;
				event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
				my clickCallback (my clickBoss, & event);
			}
		}
		void _GuiWinDrawingArea_handleKey (Widget widget, TCHAR kar) {   // TODO: event?
			iam_drawingarea;
			if (my keyCallback) {
				struct structGuiDrawingAreaKeyEvent event = { widget, 0 };
				event. key = kar;
				event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;   // TODO: event -> key?
				my keyCallback (my keyBoss, & event);
			}
		}
		void _GuiWinDrawingArea_shellResize (Widget widget) {
			iam_drawingarea;
			if (my resizeCallback) {
				struct structGuiDrawingAreaResizeEvent event = { widget };
				event. width = widget -> width;
				event. height = widget -> height;
				my resizeCallback (my resizeBoss, & event);
			}
		}
	#elif mac
		void _GuiMacDrawingArea_update (Widget widget) {
			iam_drawingarea;
			if (my exposeCallback) {
				struct structGuiDrawingAreaExposeEvent event = { widget };
				_GuiMac_clipOnParent (widget);
				my exposeCallback (my exposeBoss, & event);
				GuiMac_clipOff ();
			}
		}
		void _GuiMacDrawingArea_handleClick (Widget widget, EventRecord *macEvent) {
			iam_drawingarea;
			if (my clickCallback) {
				struct structGuiDrawingAreaClickEvent event = { widget, 0 };
				event. x = macEvent -> where. h;
				event. y = macEvent -> where. v;
				event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
				event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
				event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
				event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
				my clickCallback (my clickBoss, & event);
			}
		}
		bool _GuiMacDrawingArea_tryToHandleKey (Widget widget, EventRecord *macEvent) {
			iam_drawingarea;
			if (my keyCallback) {
				struct structGuiDrawingAreaKeyEvent event = { widget, 0 };
				event. key = macEvent -> message & charCodeMask;
				event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
				event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
				event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
				event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
				my keyCallback (my keyBoss, & event);
				return true;
			}
			return false;
		}
		void _GuiMacDrawingArea_shellResize (Widget widget) {
			iam_drawingarea;
			if (my resizeCallback) {
				struct structGuiDrawingAreaResizeEvent event = { widget, 0 };
				event. width = widget -> width;
				event. height = widget -> height;
				my resizeCallback (my resizeBoss, & event);
			}
		}
	#endif
#else
	static void _GuiMotifDrawingArea_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiDrawingArea);
		Melder_free (me);
	}
	static void _GuiMotifDrawingArea_exposeCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiDrawingArea);
		XEvent *xevent = ((XmDrawingAreaCallbackStruct *) call) -> event;
		if (xevent -> xexpose. count) return;   // BUG: always, or only in ArtwordEditor, FunctionEditor, HyperPage, RunnerMFC, TableEditor...?
		if (my exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { widget, 0 };
			event. x = xevent -> xexpose. x;
			event. y = xevent -> xexpose. y;
			event. width = xevent -> xexpose. width;
			event. height = xevent -> xexpose. height;
			my exposeCallback (my exposeBoss, & event);
		}
	}
	static void _GuiMotifDrawingArea_inputCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiDrawingArea);
		XEvent *xevent = ((XmDrawingAreaCallbackStruct *) call) -> event;
		if (xevent -> type == ButtonPress) {
			if (my clickCallback) {
				struct structGuiDrawingAreaClickEvent event = { widget, 0 };
				event. x = xevent -> xbutton.x;
				event. y = xevent -> xbutton.y;
				event. shiftKeyPressed = (xevent -> xbutton.state & ShiftMask) != 0;
				event. commandKeyPressed = (xevent -> xbutton.state & ControlMask) != 0;
				event. optionKeyPressed = (xevent -> xbutton.state & Mod1Mask) != 0;
				event. extraControlKeyPressed = (xevent -> xbutton.state & Mod2Mask) != 0;
				unsigned char map [4];
				XGetPointerMapping (XtDisplay (widget), map, 3);
				bool leftHanded = ( map [0] == 3 );
				event. button =
					xevent -> xbutton.button == Button1 ? 1 :
					xevent -> xbutton.button == Button2 ? ( leftHanded ? 3 : 2 ) :
					( leftHanded ? 2 : 3 );
				my clickCallback (my clickBoss, & event);
			}
		} else if (xevent -> type == KeyPress) {
			if (my keyCallback) {
				struct structGuiDrawingAreaKeyEvent event = { widget, 0 };
				char key;
				XLookupString (& xevent -> xkey, & key, 1, NULL, NULL);
				event. key = key;
				event. shiftKeyPressed = (xevent -> xkey.state & ShiftMask) != 0;
				event. commandKeyPressed = (xevent -> xkey.state & ControlMask) != 0;
				event. optionKeyPressed = (xevent -> xkey.state & Mod1Mask) != 0;
				event. extraControlKeyPressed = (xevent -> xkey.state & Mod2Mask) != 0;
				my keyCallback (my keyBoss, & event);
			}
		}
	}
	static void _GuiMotifDrawingArea_resizeCallback (Widget widget, XtPointer void_me, XtPointer call) {
		iam (GuiDrawingArea);
		(void) call;
		if (my resizeCallback != NULL) {
			struct structGuiDrawingAreaResizeEvent event = { widget, 0 };
			Dimension width, height;
			XtVaGetValues (widget, XmNwidth, & width, XmNheight, & height, NULL);
			event. width = width;
			event. height = height;
			my resizeCallback (my resizeBoss, & event);
		}
	}
#endif

Widget GuiDrawingArea_create (Widget parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = Melder_calloc (struct structGuiDrawingArea, 1);
	my exposeCallback = exposeCallback;
	my exposeBoss = boss;
	my clickCallback = clickCallback;
	my clickBoss = boss;
	my keyCallback = keyCallback;
	my keyBoss = boss;
	my resizeCallback = resizeCallback;
	my resizeBoss = boss;
	#if gtk
		my widget = ...
	#elif win
		my widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent, L"drawingArea");
		_GuiObject_setUserData (my widget, me);
		my widget -> window = CreateWindowEx (0, _GuiWin_getDrawingAreaClassName (), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			my widget -> x, my widget -> y, my widget -> width, my widget -> height, my widget -> parent -> window, NULL, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		_GuiObject_position (my widget, left, right, top, bottom);
	#elif mac
		my widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent, L"drawingArea");
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
	#elif motif
		my widget = XtVaCreateWidget ("drawingArea", xmDrawingAreaWidgetClass, parent, NULL);
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		if (flags & GuiDrawingArea_BORDER) XtVaSetValues (my widget, XmNborderWidth, 1, NULL);   // TODO: marginWidth & marginHeight
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifDrawingArea_destroyCallback, me);
		XtAddCallback (my widget, XmNexposeCallback, _GuiMotifDrawingArea_exposeCallback, me);
		XtAddCallback (my widget, XmNinputCallback, _GuiMotifDrawingArea_inputCallback, me);
		XtAddCallback (my widget, XmNresizeCallback, _GuiMotifDrawingArea_resizeCallback, me);
	#endif
	return my widget;
}

Widget GuiDrawingArea_createShown (Widget parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	Widget me = GuiDrawingArea_create (parent, left, right, top, bottom, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	GuiObject_show (me);
	return me;
}

void GuiDrawingArea_setExposeCallback (Widget widget, void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss) {
	GuiDrawingArea me = _GuiObject_getUserData (widget);
	if (me != NULL) {
		my exposeCallback = callback;
		my exposeBoss = boss;
	}
}

void GuiDrawingArea_setClickCallback (Widget widget, void (*callback) (void *boss, GuiDrawingAreaClickEvent event), void *boss) {
	GuiDrawingArea me = _GuiObject_getUserData (widget);
	if (me != NULL) {
		my clickCallback = callback;
		my clickBoss = boss;
	}
}

/* End of file GuiDrawingArea.c */
