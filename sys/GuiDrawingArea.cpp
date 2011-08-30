/* GuiDrawingArea.cpp
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
 * pb 2007/12/28 extracted from Motif
 * sdk 2008/03/24 GTK
 * sdk 2008/07/01 GTK resize callback
 * fb 2010/02/23 GTK
 * pb 2010/07/13 GTK key event
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 * pb 2011/06/05 all callbacks protected against exceptions
 */

#include "GuiP.h"
#if gtk
	#include "gdk/gdkkeysyms.h"
#endif
#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_drawingarea \
		Melder_assert (widget -> widgetClass == xmDrawingAreaWidgetClass); \
		GuiDrawingArea me = (GuiDrawingArea) widget -> userData
#else
	#define iam_drawingarea \
		GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget)
#endif

typedef struct structGuiDrawingArea {
	GuiObject widget;
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
	static void _GuiGtkDrawingArea_destroyCallback (GuiObject widget, gpointer void_me) {
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
	static gboolean _GuiGtkDrawingArea_exposeCallback (GuiObject widget, GdkEventExpose *expose, gpointer void_me) {
		iam (GuiDrawingArea);
		// TODO: that helps agains the damaged regions outside the rect where the
		// Graphics drawing is done, but where does that margin come from in the
		// first place?? Additionally this causes even more flickering
		//gdk_window_clear_area(widget->window, expose->area.x, expose->area.y, expose->area.width, expose->area.height);
		if (my exposeCallback) {
			struct structGuiDrawingAreaExposeEvent event = { widget, 0 };
			event. x = expose -> area. x;
			event. y = expose -> area. y;
			event. width = expose -> area. width;
			event. height = expose -> area. height;
			try {
				my exposeCallback (my exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Redrawing not completed");
			}
			return TRUE;		// let the expose callback handle redrawing
		}
		return FALSE;			// let GTK+ handle redrawing
	}
	static gboolean _GuiGtkDrawingArea_clickCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my clickCallback) {
			struct structGuiDrawingAreaClickEvent event = { widget, 0 };
			//Melder_casual("event type %ld", (long)e->type);
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
					event. button =
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
			if (e -> type == GDK_BUTTON_PRESS || 1) {
				try {
					my clickCallback (my clickBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Mouse click not completely handled.");
				}
				return TRUE;
			}
		}
		return FALSE;
	}
	static gboolean _GuiGtkDrawingArea_keyCallback (GuiObject widget, GdkEvent *gevent, gpointer void_me) {
		iam (GuiDrawingArea);
		//Melder_casual ("_GuiGtkDrawingArea_keyCallback");
		if (my keyCallback && gevent -> type == GDK_KEY_PRESS) {
			struct structGuiDrawingAreaKeyEvent event = { widget, 0 };
			GdkEventKey *gkeyEvent = (GdkEventKey *) gevent;
			event. key = gkeyEvent -> keyval;
			/*
			 * Translate with the help of /usr/include/gtk-2.0/gdk/gdkkeysyms.h
			 */
			if (event. key == GDK_Escape) event. key = 27;
			if (event. key == GDK_Left) event. key = 0x2190;
			if (event. key == GDK_Up) event. key = 0x2191;
			if (event. key == GDK_Right) event. key = 0x2192;
			if (event. key == GDK_Down) event. key = 0x2193;
			event. shiftKeyPressed = (gkeyEvent -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (gkeyEvent -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (gkeyEvent -> state & GDK_MOD1_MASK) != 0;
			event. extraControlKeyPressed = false;
			try {
				my keyCallback (my keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Key press not completely handled.");
			}
			/*
			 * FIXME: here we should empty the type-ahead buffer
			 */
			return TRUE;
		}
		return FALSE;   // if the drawing area has no keyCallback, the system will send the key press to a text field.
	}
	static gboolean _GuiGtkDrawingArea_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my resizeCallback) {
			struct structGuiDrawingAreaResizeEvent event = { widget, 0 };
			event. width = allocation -> width;
			event. height = allocation -> height;
			//g_debug("%d %d", allocation->width, allocation->height);
			try {
				my resizeCallback (my clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError ("Window resizing not completely handled.");
			}
			return TRUE;
		}
		return FALSE;
	}

#elif win || mac
	void _GuiWinMacDrawingArea_destroy (GuiObject widget) {
		iam_drawingarea;
		#if win
			DestroyWindow (widget -> window);
		#endif
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	#if win
		void _GuiWinDrawingArea_update (GuiObject widget) {
			iam_drawingarea;
			PAINTSTRUCT paintStruct;
			BeginPaint (widget -> window, & paintStruct);
			if (my exposeCallback) {
				struct structGuiDrawingAreaExposeEvent event = { widget };
				try {
					my exposeCallback (my exposeBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Redrawing not completed");
				}
			}
			EndPaint (widget -> window, & paintStruct);
		}
		void _GuiWinDrawingArea_handleClick (GuiObject widget, int x, int y) {
			iam_drawingarea;
			if (my clickCallback) {
				struct structGuiDrawingAreaClickEvent event = { widget, 0 };
				event. x = x;
				event. y = y;
				event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
				try {
					my clickCallback (my clickBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Mouse click not completely handled.");
				}
			}
		}
		void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar) {   // TODO: event?
			iam_drawingarea;
			if (my keyCallback) {
				struct structGuiDrawingAreaKeyEvent event = { widget, 0 };
				event. key = kar;
				if (event. key == VK_LEFT) event. key = 0x2190;
				if (event. key == VK_RIGHT) event. key = 0x2192;
				if (event. key == VK_UP) event. key = 0x2191;
				if (event. key == VK_DOWN) event. key = 0x2193;
				event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;   // TODO: event -> key?
				try {
					my keyCallback (my keyBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Key press not completely handled.");
				}
			}
		}
		void _GuiWinDrawingArea_shellResize (GuiObject widget) {
			iam_drawingarea;
			if (my resizeCallback) {
				struct structGuiDrawingAreaResizeEvent event = { widget };
				event. width = widget -> width;
				event. height = widget -> height;
				try {
					my resizeCallback (my resizeBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Window resizing not completely handled.");
				}
			}
		}
	#elif mac
		void _GuiMacDrawingArea_update (GuiObject widget) {
			iam_drawingarea;
			if (my exposeCallback) {
				struct structGuiDrawingAreaExposeEvent event = { widget };
				_GuiMac_clipOnParent (widget);
				try {
					my exposeCallback (my exposeBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Redrawing not completed");
				}
				GuiMac_clipOff ();
			}
		}
		void _GuiMacDrawingArea_handleClick (GuiObject widget, EventRecord *macEvent) {
			iam_drawingarea;
			if (my clickCallback) {
				struct structGuiDrawingAreaClickEvent event = { widget, 0 };
				event. x = macEvent -> where. h;
				event. y = macEvent -> where. v;
				event. shiftKeyPressed = (macEvent -> modifiers & shiftKey) != 0;
				event. commandKeyPressed = (macEvent -> modifiers & cmdKey) != 0;
				event. optionKeyPressed = (macEvent -> modifiers & optionKey) != 0;
				event. extraControlKeyPressed = (macEvent -> modifiers & controlKey) != 0;
				try {
					my clickCallback (my clickBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Mouse click not completely handled.");
				}
			}
		}
		bool _GuiMacDrawingArea_tryToHandleKey (GuiObject widget, EventRecord *macEvent) {
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
				try {
					my keyCallback (my keyBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Key press not completely handled.");
				}
				return true;
			}
			return false;
		}
		void _GuiMacDrawingArea_shellResize (GuiObject widget) {
			iam_drawingarea;
			if (my resizeCallback) {
				struct structGuiDrawingAreaResizeEvent event = { widget, 0 };
				event. width = widget -> width;
				event. height = widget -> height;
				try {
					my resizeCallback (my resizeBoss, & event);
				} catch (MelderError) {
					Melder_flushError ("Window resizing not completely handled.");
				}
			}
		}
	#endif
#endif

GuiObject GuiDrawingArea_create (GuiObject parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiDrawingArea me = Melder_calloc_f (struct structGuiDrawingArea, 1);
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
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK   // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);   // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my widget), mask);
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
		if (parent != NULL) {
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent))), "key-press-event",
				G_CALLBACK (_GuiGtkDrawingArea_keyCallback), me);
		}
		g_signal_connect (G_OBJECT (my widget), "size-allocate",
			G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me);
		
//		g_signal_connect (GTK_WIDGET (my widget), "activate",
//			G_CALLBACK (_GuiGtkDrawingArea_activateCallback), me);

		/*
		// Positioning is done by the container this widget gets child of
		// For a container allowing positioning by pixel coordinates, see GtkFixed,
		// but it is not as flexible as a typical layout (*box, table, etc.)
// 		_GuiObject_position (my widget, left, right, top, bottom);
		*/

		_GuiObject_setUserData (my widget, me);
		GuiObject_size (my widget, right - left, bottom - top);
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
	#endif
	return my widget;
}

GuiObject GuiDrawingArea_createShown (GuiObject parent, int left, int right, int top, int bottom,
	void (*exposeCallback) (void *boss, GuiDrawingAreaExposeEvent event),
	void (*clickCallback) (void *boss, GuiDrawingAreaClickEvent event),
	void (*keyCallback) (void *boss, GuiDrawingAreaKeyEvent event),
	void (*resizeCallback) (void *boss, GuiDrawingAreaResizeEvent event), void *boss,
	unsigned long flags)
{
	GuiObject me = GuiDrawingArea_create (parent, left, right, top, bottom, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	#if gtk
		gtk_container_add (GTK_CONTAINER (parent), GTK_WIDGET (me));
	#endif
	GuiObject_show (me);
	return me;
}

void GuiDrawingArea_setExposeCallback (GuiObject widget, void (*callback) (void *boss, GuiDrawingAreaExposeEvent event), void *boss) {
	GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget);
	if (me != NULL) {
		my exposeCallback = callback;
		my exposeBoss = boss;
	}
}

void GuiDrawingArea_setClickCallback (GuiObject widget, void (*callback) (void *boss, GuiDrawingAreaClickEvent event), void *boss) {
	GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget);
	if (me != NULL) {
		my clickCallback = callback;
		my clickBoss = boss;
	}
}

/* End of file GuiDrawingArea.cpp */
