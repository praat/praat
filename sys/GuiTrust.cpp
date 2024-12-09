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

static GuiButton optionButtons [1+6];
static integer theClickedButtonId;
static int theEventLoopDepth = 0;

static void gui_dialog_cb_close (Thing) {
	theClickedButtonId = -1;   // cancel
}
static void gui_dialog_cb_default (Thing) {
	theClickedButtonId = -1;   // cancel
}
static void gui_dialog_cb_cancel (Thing me, GuiButtonEvent) {
	theClickedButtonId = -1;   // cancel
}
static void gui_dialog_cb_ok (Thing, GuiButtonEvent event) {
	GuiButton clickedButton = event -> button;
	for (int i = 1; i <= 6; i ++)
		if (clickedButton == optionButtons [i]) {
			theClickedButtonId = i;
			return;
		}
	theClickedButtonId = -1;
}

integer GuiTrust_get (GuiWindow optionalParent, Editor optionalTrustWindowOwningEditor,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5,
	Interpreter interpreter
) {
	if (theEventLoopDepth > 0)
		Melder_throw (Melder_upperCaseAppName(), U" cannot have more than one trust form at a time.");
	Melder_assert (interpreter);
	constexpr int DIALOG_WIDTH = 700;
	constexpr int BUTTON_HEIGHT = 60;
	/*
		Compute the height of the trust form.
	*/
	constexpr int SHORT_LABEL_HEIGHT = 25;
	constexpr int TALL_LABEL_HEIGHT = 40;
	int dialogHeight = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = ( str32chr (message, U'\n') || message [0] == U'“' ? 2 : 1 );
			const int labelHeight = (numberOfLines == 1 ? SHORT_LABEL_HEIGHT : TALL_LABEL_HEIGHT);
			dialogHeight += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	dialogHeight += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;   // the CANCEL button
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
			U"Pausing the script for security and safety: Do you trust this script?", gui_dialog_cb_close, nullptr, GuiDialog_Modality::BLOCKING);
	GuiDialog_setDefaultCallback (me, gui_dialog_cb_default, nullptr);
	/*
		Add the labels.
	*/
	int y = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = ( str32chr (message, U'\n') || message [0] == U'“' ? 2 : 1 );
			const int labelHeight = (numberOfLines == 1 ? SHORT_LABEL_HEIGHT : TALL_LABEL_HEIGHT);
			uint32 labelFlags = GuiLabel_CENTRE | GuiLabel_MULTILINE;
			if (message [0] == U'“')
				labelFlags |= GuiLabel_BOLD;
			GuiLabel label = GuiLabel_createShown (me, x, x + buttonWidth, y, y + labelHeight, message, labelFlags);
			y += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	y += Gui_VERTICAL_DIALOG_SPACING_DIFFERENT;
	optionButtons [0] = GuiButton_createShown (me, x, x + buttonWidth, y, y + BUTTON_HEIGHT,
		U"CANCEL\n(because I don’t completely trust the authors’ skills and/or intentions)",
		gui_dialog_cb_cancel, nullptr, GuiButton_DEFAULT | GuiButton_MULTILINE
	);
	y += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
	for (int i = 1; i <= 5; i ++) {
		conststring32 option = ( i == 1 ? option1 : i == 2 ? option2 : i == 3 ? option3 : i == 4 ? option4 : option5 );
		if (option) {
			optionButtons [i] = GuiButton_createShown (me, x, x + buttonWidth, y, y + BUTTON_HEIGHT,
					option, gui_dialog_cb_ok, nullptr, GuiButton_MULTILINE);
			y += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
		} else {
			optionButtons [i] = nullptr;
		}
	}
	const bool wasBackgrounding = Melder_backgrounding;
	//if (theCurrentPraatApplication -> batch) goto end;
	if (wasBackgrounding)
		praat_foreground ();
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
			#if gtk
				do {
					gtk_main_iteration ();
				} while (! theClickedButtonId);
			#elif cocoa
				do {
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
				} while (! theClickedButtonId);
			#elif motif
				do {
					XEvent event;
					GuiNextEvent (& event);
					XtDispatchEvent (& event);
				} while (! theClickedButtonId);
			#endif
		} catch (MelderError) {
			Melder_flushError (U"An error made it to the outer level in a trust window; should not occur! Please write to paul.boersma@uva.nl");
		}
		theEventLoopDepth --;
	}
	if (wasBackgrounding)
		praat_background ();
	GuiThing_hide (me);
	GuiObject_destroy (my d_widget);
	if (theClickedButtonId == -1) {
		Interpreter_stop (interpreter);
		Melder_throw (U"You interrupted the script.");
	}
	return theClickedButtonId;
}

/* End of file GuiTrust.cpp */
