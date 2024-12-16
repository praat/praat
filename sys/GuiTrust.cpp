/* GuiTrust.cpp
 *
 * Copyright (C) 2024 Paul Boersma
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

#include "GuiTrust.h"
#include "praatP.h"
#include "GuiP.h"

static GuiButton optionButtons [1+6];
static integer theClickedButtonId = 1;   //default is CANCEL
static int theEventLoopDepth = 0;

static void gui_dialog_cb_close (Thing) {
	theClickedButtonId = 1;   // cancel
	#if cocoa
		[NSApp stopModal];
	#endif
}
static void gui_dialog_cb_default (Thing) {
	theClickedButtonId = 1;   // cancel
	#if cocoa
		[NSApp stopModal];
	#endif
}
static void gui_dialog_cb_ok (Thing, GuiButtonEvent event) {
	GuiButton clickedButton = event -> button;
	for (int i = 1; i <= 6; i ++)
		if (clickedButton == optionButtons [i]) {
			theClickedButtonId = i;
			#if cocoa
				[NSApp stopModal];
			#endif
			return;
		}
	Melder_assert (false);   // should never occur
}

GuiDialog GuiTrust_createDialog (GuiWindow optionalParent,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5
) {
	constexpr int DIALOG_WIDTH = 700;
	constexpr int BUTTON_HEIGHT = 60;
	/*
		Compute the height of the trust form.
	*/
	constexpr int SHORT_LABEL_HEIGHT = 25;
	constexpr int STEP_LABEL_HEIGHT = 15;
	int dialogHeight = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = 1 + !! str32chr (message, U'\n') + !! str32chr (message, U'“');
			const int labelHeight = SHORT_LABEL_HEIGHT + (numberOfLines - 1) * STEP_LABEL_HEIGHT;
			dialogHeight += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	dialogHeight += Gui_VERTICAL_DIALOG_SPACING_DIFFERENT;
	for (int i = 1; i <= 5; i ++) {
		conststring32 option = ( i == 1 ? option1 : i == 2 ? option2 : i == 3 ? option3 : i == 4 ? option4 : option5 );
		if (option)
			dialogHeight += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
	}
	dialogHeight += 60;   // the Help button
	/*
		Create the dialog.
	*/
	int x = Gui_LEFT_DIALOG_SPACING, buttonWidth = DIALOG_WIDTH - x - Gui_RIGHT_DIALOG_SPACING;
	GuiDialog me = GuiDialog_create (optionalParent, 150, 70, DIALOG_WIDTH, dialogHeight,
			U"Praat Trust window: checking for security and safety", gui_dialog_cb_close, nullptr, GuiDialog_Modality::BLOCKING);
	GuiDialog_setDefaultCallback (me, gui_dialog_cb_default, nullptr);
	/*
		Add the labels.
	*/
	int y = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = 1 + !! str32chr (message, U'\n') + !! str32chr (message, U'“');
			const int labelHeight = SHORT_LABEL_HEIGHT + (numberOfLines - 1) * STEP_LABEL_HEIGHT;
			uint32 labelFlags = GuiLabel_CENTRE | GuiLabel_MULTILINE;
			if (str32chr (message, U'“'))
				labelFlags |= GuiLabel_BOLD;
			GuiLabel_createShown (me, x, x + buttonWidth, y, y + labelHeight, message, labelFlags);
			y += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	/*
		Add the buttons.
	*/
	y += Gui_VERTICAL_DIALOG_SPACING_DIFFERENT;
	for (int i = 1; i <= 5; i ++) {
		conststring32 option = ( i == 1 ? option1 : i == 2 ? option2 : i == 3 ? option3 : i == 4 ? option4 : option5 );
		if (option) {
			optionButtons [i] = GuiButton_createShown (me, x, x + buttonWidth, y, y + BUTTON_HEIGHT,
					option, gui_dialog_cb_ok, nullptr, GuiButton_MULTILINE + ( i == 1 ? GuiButton_DEFAULT : 0 ));
			y += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
		} else {
			optionButtons [i] = nullptr;
		}
	}
	return me;
}

static void Gui_waitAndHandleOneEvent_any () {
	#if gtk
		gtk_main_iteration ();
	#elif cocoa
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		//[theDemoEditor -> windowForm -> d_cocoaWindow   flushWindow];
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
	#elif motif
		XEvent event;
		GuiNextEvent (& event);
		XtDispatchEvent (& event);
	#endif
}

integer GuiTrust_get_XXX (GuiWindow optionalParent, Editor optionalTrustWindowOwningEditor,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5,
	Interpreter interpreter
) {
	if (theEventLoopDepth > 0)
		Melder_throw (Melder_upperCaseAppName(), U" cannot have more than one trust form at a time.");
	Melder_assert (interpreter);
	GuiDialog me = GuiTrust_createDialog (optionalParent,
			message1, message2, message3, message4, message5, option1, option2, option3, option4, option5);
	const bool wasBackgrounding = Melder_backgrounding;
	//if (theCurrentPraatApplication -> batch) goto end;
	if (wasBackgrounding)
		praat_foreground ();   // because we will allow any event, including object selection and dynamic-menu clicks
	/*
		Put the trust form on the screen.
	*/
	GuiThing_show (me);
	/*
		Wait for the user to click a button.
	*/
	{// scope
		autoMelderSaveCurrentFolder saveFolder;
		theClickedButtonId = 0;
		Melder_assert (theEventLoopDepth == 0);
		theEventLoopDepth ++;
		try {
			do {
				Gui_waitAndHandleOneEvent_any ();
			} while (! theClickedButtonId);
		} catch (MelderError) {
			Melder_flushError (U"An error made it to the outer level in a trust window; should not occur! Please write to paul.boersma@uva.nl");
		}
		theEventLoopDepth --;
	}
	if (wasBackgrounding)
		praat_background ();
	GuiThing_hide (me);
	GuiObject_destroy (my d_widget);
	if (theClickedButtonId == 1) {
		Interpreter_stop (interpreter);
		Melder_throw (U"You interrupted the script.");
	}
	return theClickedButtonId;
}

integer GuiTrust_get (GuiWindow optionalParent, Editor optionalTrustWindowOwningEditor,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5,
	Interpreter interpreter
) {
	Melder_assert (interpreter);
	GuiDialog me = GuiTrust_createDialog (optionalParent,
			message1, message2, message3, message4, message5, option1, option2, option3, option4, option5);
	GuiThing_show (me);
	GuiDialog_run (me);
	GuiThing_hide (me);
	GuiObject_destroy (my d_widget);
	if (theClickedButtonId == 1) {
		Interpreter_stop (interpreter);
		Melder_throw (U"You interrupted the script.");
	}
	return theClickedButtonId;
}

integer GuiDialog_run (GuiDialog me) {
	#if gtk
	#elif motif
		//TRACE
		trace (1);
		theClickedButtonId = my clickedButtonId = 0;
		UpdateWindow (my d_widget -> window);   // the only way to actually show the contents of the dialog
		MSG event;
		while (! theClickedButtonId) {
			GetMessage (& event, nullptr, 0, 0);
			if (event. hwnd) {
				GuiObject object = (GuiObject) GetWindowLongPtr (event. hwnd, GWLP_USERDATA);
				if (IsDialogMessage (my d_widget -> window, & event)) {
					trace (U"dialog message ", event. message);
				} else if (event. message == WM_LBUTTONDOWN || event. message == WM_LBUTTONUP || event. message == WM_MOUSEMOVE ||
					event. message == WM_KEYDOWN || event. message == WM_KEYUP || event. message == WM_CHAR ||
					event. message == WM_NCLBUTTONDOWN || event. message == WM_NCLBUTTONUP || event. message == WM_NCMOUSEMOVE
				) {
					if (object && object -> shell == my d_widget -> shell) {
						TranslateMessage (& event);
						DispatchMessage (& event);
						if (0)for (int i = 1; i <= 5; i ++) {
							//if (object == optionButtons [i] -> d_widget) {
							//trace (U"hit ", i);
							//TranslateMessage (& message);
							//DispatchMessage (& message);
						//}
						//if (my clickedButtonId != 0)
							//return my clickedButtonId;
							theClickedButtonId = i;
							return i;
						}
					} else {
						trace (U"click somewhere else");
						//TranslateMessage (& event);   // don't handle clicks in other windows
						//DispatchMessage (& event);
					}
				} else {
					trace (U"not a click or key event? ", event. message);
					//TranslateMessage (& event);
					//DispatchMessage (& event);
				}
			} else {
				trace (U"not a window event ", event. message);
				TranslateMessage (& event);
				DispatchMessage (& event);
			}
		}
		//return my clickedButtonId;   // TODO: no tags (yet) in buttons on Windows
		return theClickedButtonId;
	#elif cocoa
		[[NSApplication sharedApplication] runModalForWindow: my d_cocoaShell];
		//return my clickedButtonId;
		return theClickedButtonId;
	#endif
	return 0;   // cancel
}

/* End of file GuiTrust.cpp */
