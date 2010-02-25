/* GuiDrawingArea.c
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
 * pb 2007/12/28 extracted from Motif
 * sdk 2008/03/24 GTK
 * sdk 2008/07/01 GTK resize callback
 * fb 2010/02/23 GTK
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
//	static void _GuiGtkDrawingArea_activateCallback (GtkAction *action, gpointer void_me) {
		//iam (GuiDrawingArea);
		// TODO: compliled niet
		/*
		struct structGuiDrawingAreaEvent event = { widget, 0 };
		if (my activateCallback != NULL) {
			my activateCallback (my activateBoss, & event);
		}*/
//	}
	static gboolean  _GuiGtkDrawingArea_exposeCallback (Widget widget, GdkEventExpose *expose, gpointer void_me) {
		iam (GuiDrawingArea);
		// TODO: that helps agains the damaged regions outside the rect where the
		// Graphics drawing is done, but where does that margin come from in the
		// first place?? Additionally this causes even more flickering
//	gdk_window_clear_area(widget->window, expose->area.x, expose->area.y, expose->area.width, expose->area.height);
		if (my exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { widget, 0 };
			event. x = expose -> area. x;
			event. y = expose -> area. y;
			event. width = expose -> area. width;
			event. height = expose -> area. height;
			my exposeCallback (my exposeBoss, & event);
			return TRUE;		// let the expose callback handle redrawing
		}
		return FALSE;			// let GTK+ handle redrawing
	}
	// "button-press-event" and "button-release-event" send a GdkEventButton,
	// but "motion-notify-event" sends a GdkEventMotion, so we cannot generally
	// assume to receive a GdkEventButton (in contrast, the motion-event does
	// not contain a field "button", the pressed mouse-button is part of the
	// "state" bitfield)
	static gboolean _GuiGtkDrawingArea_clickCallback (Widget widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my clickCallback) {
			struct structGuiDrawingAreaClickEvent event = { widget, 0 };
			switch (e -> type) {
				case GDK_BUTTON_PRESS:
					event. type = BUTTON_PRESS;
					event. button = ((GdkEventButton *) e) -> button;
					break;
				case GDK_BUTTON_RELEASE:
					event. type = BUTTON_RELEASE;
					event. button = ((GdkEventButton *) e) -> button;
					break;
				case GDK_MOTION_NOTIFY:
					event. type = MOTION_NOTIFY;
					event.button =
						((GdkEventMotion *) e) -> state & GDK_BUTTON1_MASK ? 1 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON2_MASK ? 2 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON3_MASK ? 3 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON4_MASK ? 4 :
						((GdkEventMotion *) e) -> state & GDK_BUTTON5_MASK ? 5 : 0;
					break;
				default:
					// Do NOTHING
					return FALSE;
			}
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			
			if (event.button != 0) {
				my clickCallback (my clickBoss, & event);
				return TRUE;
			}
		}
		return FALSE;
	}
	static gboolean  _GuiGtkDrawingArea_resizeCallback(Widget widget, GtkAllocation *allocation, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my resizeCallback) {
			struct structGuiDrawingAreaResizeEvent event = { widget, 0 };
			event. width = allocation -> width;
			event. height = allocation -> height;
			//g_debug("%d %d", allocation->width, allocation->height);
			my resizeCallback (my clickBoss, & event);
			return TRUE;
		}
		return FALSE;
	}

#elif win || mac
	void _GuiWinMacDrawingArea_destroy (Widget widget) {
		iam_drawingarea;
		#if win
			DestroyWindow (widget -> window);
		#endif
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
				if (event. key == VK_LEFT) event. key = 0x2190;
				if (event. key == VK_RIGHT) event. key = 0x2192;
				if (event. key == VK_UP) event. key = 0x2191;
				if (event. key == VK_DOWN) event. key = 0x2193;
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
				if (event. key == 28) event. key = 0x2190;
				if (event. key == 29) event. key = 0x2192;
				if (event. key == 30) event. key = 0x2191;
				if (event. key == 31) event. key = 0x2193;
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
				event. extraControlKeyPressed = false;
				unsigned char map [4];
				XGetPointerMapping (XtDisplay (widget), map, 3);
				bool leftHanded = ( map [0] == 3 );
				if (Melder_debug == 24) {
					Melder_casual ("GuiDrawingArea::_motif_inputCallback: button %d left-handed %d state %ld",
						xevent -> xbutton.button, leftHanded, xevent -> xbutton.state);
				}
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
				event. extraControlKeyPressed = false;
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
		my widget = gtk_drawing_area_new ();
		// TODO: maybe make this a composite widget and use connect_after for expose?
		// see http://library.gnome.org/devel/gdk/stable/gdk-Windows.html#COMPOSITED-WINDOWS
		g_signal_connect (G_OBJECT(my widget), "expose-event",
			G_CALLBACK (_GuiGtkDrawingArea_exposeCallback), me);
		g_signal_connect (G_OBJECT (my widget), "destroy",
			G_CALLBACK (_GuiGtkDrawingArea_destroyCallback), me);
		g_signal_connect (G_OBJECT (my widget), "button-press-event",
			G_CALLBACK (_GuiGtkDrawingArea_clickCallback), me);
		g_signal_connect (G_OBJECT (my widget), "button-release-event",
			G_CALLBACK (_GuiGtkDrawingArea_clickCallback), me);
		g_signal_connect (G_OBJECT (my widget), "motion-notify-event",
			G_CALLBACK (_GuiGtkDrawingArea_clickCallback), me);
		g_signal_connect (G_OBJECT (my widget), "size-allocate",
			G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me);
		
		GdkEventMask mask = GDK_EXPOSURE_MASK; // receive exposure events
		mask |= GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK; // receive click events
		mask |= GDK_BUTTON_MOTION_MASK;        // receive motion notifies when a button is pressed
		mask |= GDK_POINTER_MOTION_HINT_MASK;  // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (my widget, mask);
//		g_signal_connect (GTK_WIDGET (my widget), "activate",
//			G_CALLBACK (_GuiGtkDrawingArea_activateCallback), me);

		/*
		// Positioning is done by the container this widget gets child of
		// For a container allowing positioning by pixel coordinates, see GtkFixed,
		// but it is not as flexible as a typical layout (*box, table, etc.)
// 		_GuiObject_position (my widget, left, right, top, bottom);
		*/

		_GuiObject_setUserData (my widget, me);
		GuiObject_size(my widget, right - left, bottom - top);
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
	#if gtk
		gtk_container_add (GTK_CONTAINER (parent), GTK_WIDGET (me));
	#endif
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
