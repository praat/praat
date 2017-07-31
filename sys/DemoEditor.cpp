/* DemoEditor.cpp
 *
 * Copyright (C) 2009-2011,2013,2015,2016,2017 Paul Boersma
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

#include "DemoEditor.h"
#include "machine.h"
#include "praatP.h"
#include "UnicodeData.h"

Thing_implement (DemoEditor, Editor, 0);

static DemoEditor theReferenceToTheOnlyDemoEditor;

/***** DemoEditor methods *****/

void structDemoEditor :: v_destroy () noexcept {
	Melder_free (praatPicture);
	theReferenceToTheOnlyDemoEditor = nullptr;
	DemoEditor_Parent :: v_destroy ();
}

void structDemoEditor :: v_info () {
	DemoEditor_Parent :: v_info ();
	MelderInfo_writeLine (U"Colour: ", Graphics_Colour_name (((PraatPicture) praatPicture) -> colour));
	MelderInfo_writeLine (U"Font: ", kGraphics_font_getText (((PraatPicture) praatPicture) -> font));
	MelderInfo_writeLine (U"Font size: ", ((PraatPicture) praatPicture) -> fontSize);
}

void structDemoEditor :: v_goAway () {
	if (waitingForInput) {
		userWantsToClose = true;
	} else {
		DemoEditor_Parent :: v_goAway ();
	}
}

void structDemoEditor :: v_createMenus () {
	DemoEditor_Parent :: v_createMenus ();
}

static void gui_drawingarea_cb_expose (DemoEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics) return;   // could be the case in the very beginning
	/*
	 * Erase the background. Don't record this erasure!
	 */
	Graphics_stopRecording (my graphics.get());   // the only place in Praat (the Picture window has a separate Graphics for erasing)?
	Graphics_setColour (my graphics.get(), Graphics_WHITE);
	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Graphics_BLACK);
	Graphics_startRecording (my graphics.get());
	Graphics_play (my graphics.get(), my graphics.get());
}

static void gui_drawingarea_cb_click (DemoEditor me, GuiDrawingArea_ClickEvent event) {
	if (! my graphics) return;   // could be the case in the very beginning
	my clicked = true;
	my keyPressed = false;
	my x = event -> x;
	my y = event -> y;
	my key = UNICODE_BULLET;
	my shiftKeyPressed = event -> shiftKeyPressed;
	my commandKeyPressed = event -> commandKeyPressed;
	my optionKeyPressed = event -> optionKeyPressed;
	my extraControlKeyPressed = event -> extraControlKeyPressed;
}

static void gui_drawingarea_cb_key (DemoEditor me, GuiDrawingArea_KeyEvent event) {
	if (! my graphics) return;   // could be the case in the very beginning
	my clicked = false;
	my keyPressed = true;
	my x = 0;
	my y = 0;
	my key = event -> key;
	trace (my key);
	my shiftKeyPressed = event -> shiftKeyPressed;
	my commandKeyPressed = event -> commandKeyPressed;
	my optionKeyPressed = event -> optionKeyPressed;
	my extraControlKeyPressed = event -> extraControlKeyPressed;
}

static void gui_drawingarea_cb_resize (DemoEditor me, GuiDrawingArea_ResizeEvent event) {
	if (! my graphics) return;   // could be the case in the very beginning
	trace (event -> width, U" ", event -> height);
	Graphics_setWsViewport (my graphics.get(), 0.0, event -> width, 0.0, event -> height);
	Graphics_setWsWindow (my graphics.get(), 0.0, 100.0, 0.0, 100.0);
	//Graphics_setViewport (my graphics.get(), 0.0, 100.0, 0.0, 100.0);
	Graphics_updateWs (my graphics.get());
}

void structDemoEditor :: v_createChildren () {
	drawingArea = GuiDrawingArea_createShown (our windowForm, 0, 0, 0, 0,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, this, 0);
}

void DemoEditor_init (DemoEditor me) {
	Editor_init (me, 0, 0, 1344, 756, U"", nullptr);   // 70 percent of the standard 1920x1080 screen
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setColour (my graphics.get(), Graphics_WHITE);
	Graphics_setWindow (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (my graphics.get(), 0.0, 1.0, 0.0, 1.0);
	Graphics_setColour (my graphics.get(), Graphics_BLACK);
	Graphics_startRecording (my graphics.get());
	Graphics_setWsViewport (my graphics.get(), 0.0, GuiControl_getWidth  (my drawingArea),
	                                           0.0, GuiControl_getHeight (my drawingArea));
	Graphics_setWsWindow (my graphics.get(), 0.0, 100.0, 0.0, 100.0);
	Graphics_setViewport (my graphics.get(), 0.0, 100.0, 0.0, 100.0);
	Graphics_updateWs (my graphics.get());
}

autoDemoEditor DemoEditor_create () {
	try {
		autoDemoEditor me = Thing_new (DemoEditor);
		DemoEditor_init (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Demo window not created.");
	}
}

void Demo_open () {
	if (Melder_batch) {
		/*
		 * Batch scripts have to be able to run demos.
		 */
		//Melder_batch = false;
	}
	if (! theReferenceToTheOnlyDemoEditor) {
		autoDemoEditor editor = DemoEditor_create ();
		Melder_assert (editor);
		//GuiObject_show (editor -> windowForm);
		editor -> praatPicture = Melder_calloc_f (structPraatPicture, 1);
		theCurrentPraatPicture = (PraatPicture) editor -> praatPicture;
		theCurrentPraatPicture -> graphics = editor -> graphics.get();
		theCurrentPraatPicture -> font = kGraphics_font_HELVETICA;
		theCurrentPraatPicture -> fontSize = 10;
		theCurrentPraatPicture -> lineType = Graphics_DRAWN;
		theCurrentPraatPicture -> colour = Graphics_BLACK;
		theCurrentPraatPicture -> lineWidth = 1.0;
		theCurrentPraatPicture -> arrowSize = 1.0;
		theCurrentPraatPicture -> speckleSize = 1.0;
		theCurrentPraatPicture -> x1NDC = 0.0;
		theCurrentPraatPicture -> x2NDC = 100.0;
		theCurrentPraatPicture -> y1NDC = 0.0;
		theCurrentPraatPicture -> y2NDC = 100.0;
		theReferenceToTheOnlyDemoEditor = editor.get();
		editor.releaseToUser();
	}
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput)
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	theCurrentPraatPicture = (PraatPicture) theReferenceToTheOnlyDemoEditor -> praatPicture;
}

void Demo_close () {
	theCurrentPraatPicture = & theForegroundPraatPicture;
}

int Demo_windowTitle (const char32 *title) {
	autoDemoOpen demo;
	Thing_setName (theReferenceToTheOnlyDemoEditor, title);
	return 1;
}

int Demo_show () {
	if (! theReferenceToTheOnlyDemoEditor) return 0;
	autoDemoOpen demo;
	GuiThing_show (theReferenceToTheOnlyDemoEditor -> windowForm);
	GuiShell_drain (theReferenceToTheOnlyDemoEditor -> windowForm);
	return 1;
}

#if cocoa
	@interface DemoWindowTimer: NSObject
	- (void) timerCallback: (NSTimer *) timer;
	@end
	@implementation DemoWindowTimer
	- (void) timerCallback: (NSTimer *) timer {
		(void) timer;
		printf ("eureka\n");
	}
	@end
	DemoWindowTimer *theDemoWindowTimer;
#endif

void Demo_timer (double duration) {
	#if cocoa
		if (! theDemoWindowTimer)
			theDemoWindowTimer = [[DemoWindowTimer alloc] init];
		[NSTimer scheduledTimerWithTimeInterval: duration
			target: theDemoWindowTimer
			selector: @selector (timerCallback)
			userInfo: nil
			repeats: false];
	#endif
}

void Demo_waitForInput (Interpreter interpreter) {
	if (! theReferenceToTheOnlyDemoEditor) return;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	//GuiObject_show (theReferenceToTheOnlyDemoEditor -> windowForm);
	theReferenceToTheOnlyDemoEditor -> clicked = false;
	theReferenceToTheOnlyDemoEditor -> keyPressed = false;
	theReferenceToTheOnlyDemoEditor -> waitingForInput = true;
	{// scope
		autoMelderSaveDefaultDir saveDir;
		bool wasBackgrounding = Melder_backgrounding;
		if (wasBackgrounding) praat_foreground ();
		try {
			#if gtk
				do {
					gtk_main_iteration ();
				} while (! theReferenceToTheOnlyDemoEditor -> clicked &&
				         ! theReferenceToTheOnlyDemoEditor -> keyPressed &&
						 ! theReferenceToTheOnlyDemoEditor -> userWantsToClose);
			#elif cocoa
				do {
					NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
					[theReferenceToTheOnlyDemoEditor -> windowForm -> d_cocoaShell   flushWindow];
					Graphics_updateWs (theReferenceToTheOnlyDemoEditor -> graphics.get());   // make sure that even texts will be drawn
					NSEvent *nsEvent = [NSApp
						nextEventMatchingMask: NSAnyEventMask
						untilDate: [NSDate distantFuture]   // wait
						inMode: NSDefaultRunLoopMode
						dequeue: YES
					];
					Melder_assert (nsEvent);
					[NSApp  sendEvent: nsEvent];
					[NSApp  updateWindows];   // called automatically?
					[pool release];
				} while (! theReferenceToTheOnlyDemoEditor -> clicked &&
				         ! theReferenceToTheOnlyDemoEditor -> keyPressed &&
						 ! theReferenceToTheOnlyDemoEditor -> userWantsToClose);
			#elif defined (_WIN32)
				do {
					XEvent event;
					GuiNextEvent (& event);
					XtDispatchEvent (& event);
				} while (! theReferenceToTheOnlyDemoEditor -> clicked &&
				         ! theReferenceToTheOnlyDemoEditor -> keyPressed &&
						 ! theReferenceToTheOnlyDemoEditor -> userWantsToClose);
			#endif
		} catch (MelderError) {
			Melder_flushError (U"An error made it to the outer level in the Demo window; should not occur! Please write to paul.boersma@uva.nl");
		}
		if (wasBackgrounding) praat_background ();
	}
	theReferenceToTheOnlyDemoEditor -> waitingForInput = false;
	if (theReferenceToTheOnlyDemoEditor -> userWantsToClose) {
		Interpreter_stop (interpreter);
		forget (theReferenceToTheOnlyDemoEditor);
		Melder_throw (U"You interrupted the script.");
	}
}

void Demo_peekInput (Interpreter interpreter) {
	if (! theReferenceToTheOnlyDemoEditor) return;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	//GuiObject_show (theReferenceToTheOnlyDemoEditor -> windowForm);
	theReferenceToTheOnlyDemoEditor -> clicked = false;
	theReferenceToTheOnlyDemoEditor -> keyPressed = false;
	theReferenceToTheOnlyDemoEditor -> x = 0;
	theReferenceToTheOnlyDemoEditor -> y = 0;
	theReferenceToTheOnlyDemoEditor -> key = U'\0';
	theReferenceToTheOnlyDemoEditor -> shiftKeyPressed = false;
	theReferenceToTheOnlyDemoEditor -> commandKeyPressed = false;
	theReferenceToTheOnlyDemoEditor -> optionKeyPressed = false;
	theReferenceToTheOnlyDemoEditor -> extraControlKeyPressed = false;
	theReferenceToTheOnlyDemoEditor -> waitingForInput = true;
	{// scope
		autoMelderSaveDefaultDir saveDir;
		//bool wasBackgrounding = Melder_backgrounding;
		//if (wasBackgrounding) praat_foreground ();
		try {
			#if gtk
				while (gtk_events_pending ()) {
					gtk_main_iteration ();
				}
			#elif cocoa
				NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
				[theReferenceToTheOnlyDemoEditor -> windowForm -> d_cocoaShell   flushWindow];
				Graphics_updateWs (theReferenceToTheOnlyDemoEditor -> graphics.get());   // make sure that even texts will be drawn
				while (NSEvent *nsEvent = [NSApp
					nextEventMatchingMask: NSAnyEventMask
					untilDate: [NSDate distantPast]   // don't wait
					inMode: NSDefaultRunLoopMode
					dequeue: YES])
				{
					[NSApp  sendEvent: nsEvent];
				}
				[NSApp  updateWindows];   // called automatically?
				[pool release];
			#elif defined (_WIN32)
				XEvent event;
				while (PeekMessage (& event, 0, 0, 0, PM_REMOVE)) {
					XtDispatchEvent (& event);
				}
			#endif
		} catch (MelderError) {
			Melder_flushError (U"An error made it to the outer level in the Demo window; should not occur! Please write to paul.boersma@uva.nl");
		}
		//if (wasBackgrounding) praat_background ();
	}
	theReferenceToTheOnlyDemoEditor -> waitingForInput = false;
	if (theReferenceToTheOnlyDemoEditor -> userWantsToClose) {
		Interpreter_stop (interpreter);
		forget (theReferenceToTheOnlyDemoEditor);
		Melder_throw (U"You interrupted the script.");
	}
}

bool Demo_clicked () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> clicked;
}

double Demo_x () {
	if (! theReferenceToTheOnlyDemoEditor) return undefined;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	trace (U"NDC before: ", theReferenceToTheOnlyDemoEditor -> graphics -> d_x1NDC, U" ", theReferenceToTheOnlyDemoEditor -> graphics -> d_x2NDC);
	Graphics_setInner (theReferenceToTheOnlyDemoEditor -> graphics.get());
	trace (U"NDC after: ", theReferenceToTheOnlyDemoEditor -> graphics -> d_x1NDC, U" ", theReferenceToTheOnlyDemoEditor -> graphics -> d_x2NDC);
	double xWC, yWC;
	trace (U"DC: x ", theReferenceToTheOnlyDemoEditor -> x, U", y ", theReferenceToTheOnlyDemoEditor -> y);
	Graphics_DCtoWC (theReferenceToTheOnlyDemoEditor -> graphics.get(), theReferenceToTheOnlyDemoEditor -> x, theReferenceToTheOnlyDemoEditor -> y, & xWC, & yWC);
	trace (U"WC: x ", xWC, U", y ", yWC);
	Graphics_unsetInner (theReferenceToTheOnlyDemoEditor -> graphics.get());
	return xWC;
}

double Demo_y () {
	if (! theReferenceToTheOnlyDemoEditor) return undefined;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	Graphics_setInner (theReferenceToTheOnlyDemoEditor -> graphics.get());
	double xWC, yWC;
	Graphics_DCtoWC (theReferenceToTheOnlyDemoEditor -> graphics.get(), theReferenceToTheOnlyDemoEditor -> x, theReferenceToTheOnlyDemoEditor -> y, & xWC, & yWC);
	Graphics_unsetInner (theReferenceToTheOnlyDemoEditor -> graphics.get());
	return yWC;
}

bool Demo_keyPressed () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> keyPressed;
}

char32 Demo_key () {
	if (! theReferenceToTheOnlyDemoEditor) return U'\0';
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> key;
}

bool Demo_shiftKeyPressed () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> shiftKeyPressed;
}

bool Demo_commandKeyPressed () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> commandKeyPressed;
}

bool Demo_optionKeyPressed () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> optionKeyPressed;
}

bool Demo_extraControlKeyPressed () {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return theReferenceToTheOnlyDemoEditor -> extraControlKeyPressed;
}

bool Demo_input (const char32 *keys) {
	if (! theReferenceToTheOnlyDemoEditor) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	return str32chr (keys, theReferenceToTheOnlyDemoEditor -> key) != nullptr;
}

bool Demo_clickedIn (double left, double right, double bottom, double top) {
	if (! theReferenceToTheOnlyDemoEditor || ! theReferenceToTheOnlyDemoEditor -> clicked) return false;
	if (theReferenceToTheOnlyDemoEditor -> waitingForInput) {
		Melder_throw (U"You cannot work with the Demo window while it is waiting for input. "
			U"Please click or type into the Demo window or close it.");
	}
	if (! theReferenceToTheOnlyDemoEditor -> clicked) return false;
	double xWC = Demo_x (), yWC = Demo_y ();
	return xWC >= left && xWC < right && yWC >= bottom && yWC < top;
}

/* End of file DemoEditor.cpp */
