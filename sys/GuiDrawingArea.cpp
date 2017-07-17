/* GuiDrawingArea.cpp
 *
 * Copyright (C) 1993-2012,2013,2015,2016,2017 Paul Boersma,
 *               2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

#include "GuiP.h"
#if gtk
	#include "gdk/gdkkeysyms.h"
	#include <locale.h>
#endif

Thing_implement (GuiDrawingArea, GuiControl, 0);

#if motif
	#define iam_drawingarea \
		Melder_assert (widget -> widgetClass == xmDrawingAreaWidgetClass); \
		GuiDrawingArea me = (GuiDrawingArea) widget -> userData
#else
	#define iam_drawingarea \
		GuiDrawingArea me = (GuiDrawingArea) _GuiObject_getUserData (widget)
#endif

#if gtk
	static void _GuiGtkDrawingArea_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiDrawingArea);
		forget (me);
	}
	static gboolean _GuiGtkDrawingArea_exposeCallback (GuiObject widget, GdkEventExpose *expose, gpointer void_me) {
		trace (U"begin");
		iam (GuiDrawingArea);
		Melder_assert (me);
		// TODO: that helps against the damaged regions outside the rect where the
		// Graphics drawing is done, but where does that margin come from in the
		// first place?? Additionally this causes even more flickering
		//gdk_window_clear_area ((GTK_WIDGET (widget)) -> window, expose->area.x, expose->area.y, expose->area.width, expose->area.height);
		if (my d_exposeCallback) {
			struct structGuiDrawingArea_ExposeEvent event { me, 0 };
			event. x = expose -> area. x;
			event. y = expose -> area. y;
			event. width = expose -> area. width;
			event. height = expose -> area. height;
			try {
				//GdkRectangle rect = { event. x, event. y, event. width, event. height };
				//gdk_window_begin_paint_rect ((GTK_WIDGET (widget)) -> window, & rect);
				trace (U"send the expose callback");
				trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
				my d_exposeCallback (my d_exposeBoss, & event);
				trace (U"the expose callback finished");
				trace (U"locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
				//gdk_window_end_paint ((GTK_WIDGET (widget)) -> window);
				//gdk_window_flush ((GTK_WIDGET (widget)) -> window);
				//gdk_flush ();
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
			trace (U"the expose callback handled drawing");
			return true;
		}
		trace (U"GTK will handle redrawing");
		return false;
	}
	static gboolean _GuiGtkDrawingArea_clickCallback (GuiObject widget, GdkEvent *e, gpointer void_me) {
		iam (GuiDrawingArea);
		if (e -> type != GDK_BUTTON_PRESS) return false;
		if (my d_clickCallback) {
			struct structGuiDrawingArea_ClickEvent event { me, 0 };
			event. button = ((GdkEventButton *) e) -> button;
			event. x = ((GdkEventButton *) e) -> x;
			event. y = ((GdkEventButton *) e) -> y;
			event. shiftKeyPressed = (((GdkEventButton *) e) -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (((GdkEventButton *) e) -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (((GdkEventButton *) e) -> state & GDK_MOD1_MASK) != 0;
			try {
				my d_clickCallback (my d_clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse click not completely handled.");
			}
			return true;
		}
		return false;
	}
	static gboolean _GuiGtkDrawingArea_keyCallback (GuiObject widget, GdkEvent *gevent, gpointer void_me) {
		iam (GuiDrawingArea);
		trace (U"begin");
		if (my d_keyCallback && gevent -> type == GDK_KEY_PRESS) {
			struct structGuiDrawingArea_KeyEvent event { me, 0 };
			GdkEventKey *gkeyEvent = (GdkEventKey *) gevent;
			event. key = gkeyEvent -> keyval;
			/*
			 * Translate with the help of /usr/include/gtk-2.0/gdk/gdkkeysyms.h
			 */
			if (event. key == GDK_KEY_Escape) event. key = 27;
			if (event. key == GDK_KEY_Left)   event. key = 0x2190;
			if (event. key == GDK_KEY_Up)     event. key = 0x2191;
			if (event. key == GDK_KEY_Right)  event. key = 0x2192;
			if (event. key == GDK_KEY_Down)   event. key = 0x2193;
			event. shiftKeyPressed = (gkeyEvent -> state & GDK_SHIFT_MASK) != 0;
			event. commandKeyPressed = (gkeyEvent -> state & GDK_CONTROL_MASK) != 0;
			event. optionKeyPressed = (gkeyEvent -> state & GDK_MOD1_MASK) != 0;
			event. extraControlKeyPressed = false;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
			/*
			 * FIXME: here we should empty the type-ahead buffer
			 */
			return true;
		}
		return false;   // if the drawing area has no keyCallback, the system will send the key press to a text field.
	}
	static gboolean _GuiGtkDrawingArea_resizeCallback (GuiObject widget, GtkAllocation *allocation, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my d_resizeCallback) {
			struct structGuiDrawingArea_ResizeEvent event { me, 0 };
			trace (U"drawingArea resized to ", allocation -> width, U" x ", allocation -> height, U".");
			event. width = allocation -> width;
			event. height = allocation -> height;
			//g_debug("%d %d", allocation->width, allocation->height);
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
			return true;
		}
		return false;
	}
#elif motif
	void _GuiWinDrawingArea_destroy (GuiObject widget) {
		iam_drawingarea;
		DestroyWindow (widget -> window);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinDrawingArea_update (GuiObject widget) {
		iam_drawingarea;
		PAINTSTRUCT paintStruct;
		BeginPaint (widget -> window, & paintStruct);
		if (my d_exposeCallback) {
			struct structGuiDrawingArea_ExposeEvent event { me };
			try {
				my d_exposeCallback (my d_exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
		}
		EndPaint (widget -> window, & paintStruct);
	}
	void _GuiWinDrawingArea_handleClick (GuiObject widget, int x, int y) {
		iam_drawingarea;
		if (my d_clickCallback) {
			struct structGuiDrawingArea_ClickEvent event { me, 0 };
			event. x = x;
			event. y = y;
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;
			event. optionKeyPressed = GetKeyState (VK_MENU) < 0;
			event. commandKeyPressed = GetKeyState (VK_CONTROL) < 0;
			try {
				my d_clickCallback (my d_clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse click not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_handleKey (GuiObject widget, TCHAR kar) {   // TODO: event?
		iam_drawingarea;
		if (my d_keyCallback) {
			struct structGuiDrawingArea_KeyEvent event { me, 0 };
			event. key = kar;
			if (event. key == VK_RETURN) event. key = 10;
			if (event. key == VK_LEFT)  event. key = 0x2190;
			if (event. key == VK_RIGHT) event. key = 0x2192;
			if (event. key == VK_UP)    event. key = 0x2191;
			if (event. key == VK_DOWN)  event. key = 0x2193;
			event. shiftKeyPressed = GetKeyState (VK_SHIFT) < 0;   // TODO: event -> key?
			event. optionKeyPressed = GetKeyState (VK_MENU) < 0;
			event. commandKeyPressed = GetKeyState (VK_CONTROL) < 0;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
		}
	}
	void _GuiWinDrawingArea_shellResize (GuiObject widget) {
		iam_drawingarea;
		if (my d_resizeCallback) {
			struct structGuiDrawingArea_ResizeEvent event { me };
			event. width = widget -> width;
			event. height = widget -> height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
		}
	}
#elif cocoa
	@interface GuiCocoaDrawingArea ()
	@property (nonatomic, assign) BOOL inited;
	@property (nonatomic, retain) NSTrackingArea *trackingArea;
	@end
	@implementation GuiCocoaDrawingArea {
		GuiDrawingArea d_userData;
	}
	- (id) initWithFrame: (NSRect) frame {
		self = [super initWithFrame: frame];
		if (self) {
			_trackingArea = [[[NSTrackingArea alloc]
				initWithRect: [self visibleRect]
				options: NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingInVisibleRect | NSTrackingActiveAlways
				owner: self
				userInfo: nil]
				autorelease];
			[self   addTrackingArea: _trackingArea];
		}
		return self;
	}
	- (void) dealloc {   // override
		GuiDrawingArea me = d_userData;
		forget (me);
		[self removeTrackingArea: _trackingArea];
		trace (U"deleting a drawing area");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		d_userData = static_cast <GuiDrawingArea> (userData);
	}
	- (void) resizeCallback: (NSRect) rect {
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (me && my d_resizeCallback) {
			struct structGuiDrawingArea_ResizeEvent event { me, 0 };
			event. width = rect. size. width;
			event. height = rect. size. height;
			try {
				my d_resizeCallback (my d_resizeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Window resizing not completely handled.");
			}
		}
	}
	- (void) drawRect: (NSRect) dirtyRect {
		trace (U"dirtyRect: ", dirtyRect.origin.x, U", ", dirtyRect.origin.y, U", ", dirtyRect.size.width, U", ", dirtyRect.size.height);
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (! _inited) {
			// Last chance to do this. Is there a better place?
			[self   resizeCallback: self. frame];
			_inited = YES;
		}
		if (my d_exposeCallback) {
			struct structGuiDrawingArea_ExposeEvent event { me };
			try {
				my d_exposeCallback (my d_exposeBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Redrawing not completed");
			}
		}
	}
	- (void) setFrame: (NSRect) rect {
		[self   resizeCallback: rect];
		[super   setFrame: rect];
	}
	- (BOOL) acceptsFirstResponder {
		/*
		 * This overridden method tells the event chain whether the drawing area can accept key events.
		 * It is important that the Demo window and the RunnerMFC window accept key events.
		 * A side effect of accepting key events is that the drawing area obtains the key focus when the user clicks in the drawing area.
		 * It is important, however, that the drawing area of the TextGrid window cannot take away the key focus
		 * from the text field at the top; therefore, that drawing area should not accept key events.
		 * The implementation below is based on the fact that, naturally, the Demo window and the RunnerMFC window
		 * have a key callback, and the drawing area of the TextGrid window has not
		 * (a side effect of this implementation is that the drawing area of the Manual window does not take away
		 * the key focus from the Search field, a situation that cannot hurt).
		 */
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		return !! my d_keyCallback;
	}
	- (void) mouseEntered: (NSEvent *) nsEvent {
		(void) nsEvent;
		[[NSCursor crosshairCursor] push];
	}
	- (void) mouseExited: (NSEvent *) nsEvent{
		(void) nsEvent;
		[[NSCursor currentCursor] pop];
	}
	- (void) mouseDown: (NSEvent *) nsEvent {
	 //   [self becomeFirstResponder];
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (my d_clickCallback) {
			struct structGuiDrawingArea_ClickEvent event { me, 0 };
			NSPoint local_point = [self   convertPoint: [nsEvent locationInWindow]   fromView: nil];
			event. x = local_point. x;
			//event. y = [self frame]. size. height - local_point. y;
			event. y = local_point. y;
			NSUInteger modifiers = [nsEvent modifierFlags];
			event. shiftKeyPressed = modifiers & NSShiftKeyMask;
			event. optionKeyPressed = modifiers & NSAlternateKeyMask;
			event. commandKeyPressed = modifiers & NSCommandKeyMask;
			try {
				my d_clickCallback (my d_clickBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Mouse click not completely handled.");
			}
		}
	}
	- (void) scrollWheel: (NSEvent *) nsEvent {
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (my d_horizontalScrollBar || my d_verticalScrollBar) {
			if (my d_horizontalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget;
				[cocoaScrollBar scrollBy: [nsEvent scrollingDeltaX]];
			}
			if (my d_verticalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget;
				[cocoaScrollBar scrollBy: [nsEvent scrollingDeltaY]];
			}
		} else {
			[super scrollWheel: nsEvent];
		}
	}
	- (void) magnifyWithEvent: (NSEvent *) nsEvent {
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (my d_horizontalScrollBar || my d_verticalScrollBar) {
			if (my d_horizontalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_horizontalScrollBar -> d_widget;
				[cocoaScrollBar magnifyBy: [nsEvent magnification]];
			}
			if (my d_verticalScrollBar) {
				GuiCocoaScrollBar *cocoaScrollBar = (GuiCocoaScrollBar *) my d_verticalScrollBar -> d_widget;
				[cocoaScrollBar magnifyBy: [nsEvent magnification]];
			}
		} else {
			[super magnifyWithEvent: nsEvent];
		}
	}
	- (BOOL) isFlipped {
		return YES;
	}
	- (void) keyDown: (NSEvent *) nsEvent {
		GuiDrawingArea me = (GuiDrawingArea) d_userData;
		if (my d_keyCallback) {
			struct structGuiDrawingArea_KeyEvent event { me, 0 };
			event. key = [[nsEvent charactersIgnoringModifiers]   characterAtIndex: 0];
			if (event. key == NSLeftArrowFunctionKey)  event. key = 0x2190;
			if (event. key == NSRightArrowFunctionKey) event. key = 0x2192;
			if (event. key == NSUpArrowFunctionKey)    event. key = 0x2191;
			if (event. key == NSDownArrowFunctionKey)  event. key = 0x2193;
			trace (U"key ", event. key);
			NSUInteger modifiers = [nsEvent modifierFlags];
			event. shiftKeyPressed = modifiers & NSShiftKeyMask;
			event. optionKeyPressed = modifiers & NSAlternateKeyMask;
			event. commandKeyPressed = modifiers & NSCommandKeyMask;
			try {
				my d_keyCallback (my d_keyBoss, & event);
			} catch (MelderError) {
				Melder_flushError (U"Key press not completely handled.");
			}
		}
	}
	@end
#endif

#if gtk
	static gboolean _guiGtkDrawingArea_swipeCallback (GuiObject w, GdkEventScroll *event, gpointer void_me) {
		iam (GuiDrawingArea);
		if (my d_horizontalScrollBar) {
			double hv = gtk_range_get_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget));
			GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (my d_horizontalScrollBar -> d_widget));
			gdouble hi;
			g_object_get (adjustment, "step_increment", & hi, nullptr);
			switch (event -> direction) {
				case GDK_SCROLL_LEFT:
					gtk_range_set_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget), hv - hi);
					break;
				case GDK_SCROLL_RIGHT:
					gtk_range_set_value (GTK_RANGE (my d_horizontalScrollBar -> d_widget), hv + hi);
					break;
			}
		}
		if (my d_verticalScrollBar) {
			double vv = gtk_range_get_value (GTK_RANGE (my d_verticalScrollBar -> d_widget));
			GtkAdjustment *adjustment = gtk_range_get_adjustment (GTK_RANGE (my d_verticalScrollBar -> d_widget));
			gdouble vi;
			g_object_get (adjustment, "step_increment", & vi, nullptr);
			switch (event -> direction) {
				case GDK_SCROLL_UP:
					gtk_range_set_value (GTK_RANGE (my d_verticalScrollBar -> d_widget), vv - vi);
					break;
				case GDK_SCROLL_DOWN:
					gtk_range_set_value (GTK_RANGE (my d_verticalScrollBar -> d_widget), vv + vi);
					break;
			}
		}
		return true;
	}
#endif

GuiDrawingArea GuiDrawingArea_create (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_ClickCallback clickCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback, Thing boss,
	uint32 /* flags */)
{
	autoGuiDrawingArea me = Thing_new (GuiDrawingArea);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my d_clickCallback = clickCallback;
	my d_clickBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "expose-event",         G_CALLBACK (_GuiGtkDrawingArea_exposeCallback),  me.get());
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_GuiGtkDrawingArea_destroyCallback), me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		if (parent) {
			Melder_assert (parent -> d_widget);
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_GuiGtkDrawingArea_keyCallback), me.get());
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me.get());

		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		gtk_widget_set_double_buffered (GTK_WIDGET (my d_widget), false);
    #elif motif
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, U"drawingArea");
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindowEx (0, Melder_peek32toW (_GuiWin_getDrawingAreaClassName ()), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, nullptr, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif cocoa
		GuiCocoaDrawingArea *drawingArea = [[GuiCocoaDrawingArea alloc] init];
		my d_widget = (GuiObject) drawingArea;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[drawingArea   setUserData: me.get()];
		if (keyCallback) {
			[[drawingArea window]   makeFirstResponder: drawingArea];   // needed in DemoWindow
		}
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiDrawingArea GuiDrawingArea_createShown (GuiForm parent, int left, int right, int top, int bottom,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_ClickCallback clickCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback, Thing boss,
	uint32 flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, left, right, top, bottom, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	GuiThing_show (me);
	return me;
}

GuiDrawingArea GuiDrawingArea_create (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_ClickCallback clickCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback, Thing boss,
	uint32 /* flags */)
{
	autoGuiDrawingArea me = Thing_new (GuiDrawingArea);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_exposeCallback = exposeCallback;
	my d_exposeBoss = boss;
	my d_clickCallback = clickCallback;
	my d_clickBoss = boss;
	my d_keyCallback = keyCallback;
	my d_keyBoss = boss;
	my d_resizeCallback = resizeCallback;
	my d_resizeBoss = boss;
	#if gtk
		my d_widget = gtk_drawing_area_new ();
		GdkEventMask mask = (GdkEventMask) (GDK_EXPOSURE_MASK   // receive exposure events
			| GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK   // receive click events
			| GDK_BUTTON_MOTION_MASK                            // receive motion notifies when a button is pressed
			| GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK
			| GDK_POINTER_MOTION_HINT_MASK);                    // receive fewer motion notify events (the cb might take time)
		gtk_widget_set_events (GTK_WIDGET (my d_widget), mask);
		g_signal_connect (G_OBJECT (my d_widget), "expose-event",         G_CALLBACK (_GuiGtkDrawingArea_exposeCallback),  me.get());
		g_signal_connect (G_OBJECT (my d_widget), "destroy",              G_CALLBACK (_GuiGtkDrawingArea_destroyCallback), me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-press-event",   G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		g_signal_connect (G_OBJECT (my d_widget), "button-release-event", G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		g_signal_connect (G_OBJECT (my d_widget), "motion-notify-event",  G_CALLBACK (_GuiGtkDrawingArea_clickCallback),   me.get());
		if (parent) {
			g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (parent -> d_widget))), "key-press-event",
				G_CALLBACK (_GuiGtkDrawingArea_keyCallback), me.get());
		}
		g_signal_connect (G_OBJECT (my d_widget), "size-allocate", G_CALLBACK (_GuiGtkDrawingArea_resizeCallback), me.get());
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
		gtk_widget_set_double_buffered (GTK_WIDGET (my d_widget), false);
    #elif motif
		my d_widget = _Gui_initializeWidget (xmDrawingAreaWidgetClass, parent -> d_widget, U"drawingArea");
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindowEx (0, Melder_peek32toW (_GuiWin_getDrawingAreaClassName ()), L"drawingArea",
			WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS,
			0, 0, my d_widget -> width, my d_widget -> height, my d_widget -> parent -> window, nullptr, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
	#elif cocoa
		GuiCocoaDrawingArea *drawingArea = [[GuiCocoaDrawingArea alloc] init];
		my d_widget = (GuiObject) drawingArea;
		my v_positionInScrolledWindow (my d_widget, width, height, parent);
		[drawingArea setUserData: me.get()];
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiDrawingArea GuiDrawingArea_createShown (GuiScrolledWindow parent, int width, int height,
	GuiDrawingArea_ExposeCallback exposeCallback,
	GuiDrawingArea_ClickCallback clickCallback,
	GuiDrawingArea_KeyCallback keyCallback,
	GuiDrawingArea_ResizeCallback resizeCallback, Thing boss,
	uint32 flags)
{
	GuiDrawingArea me = GuiDrawingArea_create (parent, width, height, exposeCallback, clickCallback, keyCallback, resizeCallback, boss, flags);
	GuiThing_show (me);
	return me;
}

void GuiDrawingArea_setSwipable (GuiDrawingArea me, GuiScrollBar horizontalScrollBar, GuiScrollBar verticalScrollBar) {
	my d_horizontalScrollBar = horizontalScrollBar;
	my d_verticalScrollBar = verticalScrollBar;
	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "scroll-event", G_CALLBACK (_guiGtkDrawingArea_swipeCallback), me);
	#endif
}

void GuiDrawingArea_setExposeCallback (GuiDrawingArea me, GuiDrawingArea_ExposeCallback callback, Thing boss) {
	my d_exposeCallback = callback;
	my d_exposeBoss = boss;
}

void GuiDrawingArea_setClickCallback (GuiDrawingArea me, GuiDrawingArea_ClickCallback callback, Thing boss) {
	my d_clickCallback = callback;
	my d_clickBoss = boss;
}

void GuiDrawingArea_setResizeCallback (GuiDrawingArea me, GuiDrawingArea_ResizeCallback callback, Thing boss) {
	my d_resizeCallback = callback;
	my d_resizeBoss = boss;
}

/* End of file GuiDrawingArea.cpp */
