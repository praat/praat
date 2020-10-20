/* UiPause.cpp
 *
 * Copyright (C) 2009-2020 Paul Boersma
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

#include "UiPause.h"
#include "praatP.h"

static autoUiForm thePauseForm;
static UiField thePauseFormRadio = nullptr;
static int thePauseForm_clicked = 0;
static int theCancelContinueButton = 0;
static int theEventLoopDepth = 0;

static void thePauseFormOkCallback (UiForm /* sendingForm */, integer /* narg */, Stackel /* args */,
	conststring32 /* sendingString */, Interpreter /* interpreter */,
	conststring32 /* invokingButtonTitle */, bool /* modified */, void *closure) {
	/* BUG: should also restore praatP. editor. */
	/*
	 * Get the data from the pause form.
	 */
	thePauseForm_clicked = UiForm_getClickedContinueButton (thePauseForm.get());
	if (thePauseForm_clicked != theCancelContinueButton)
		UiForm_Interpreter_addVariables (thePauseForm.get(), (Interpreter) closure);   // 'closure', not 'interpreter' or 'theInterpreter'!
}
static void thePauseFormCancelCallback (UiForm /* dia */, void * /* closure */) {
	if (theCancelContinueButton != 0) {
		thePauseForm_clicked = theCancelContinueButton;
	} else {
		thePauseForm_clicked = -1;   // STOP button
	}
}
void UiPause_begin (GuiWindow topShell, conststring32 title, Interpreter interpreter) {
	if (theEventLoopDepth > 0)
		Melder_throw (U"Praat cannot have more than one pause form at a time.");
	thePauseForm = UiForm_create (topShell, Melder_cat (U"Pause: ", title),
		thePauseFormOkCallback, interpreter,   // pass interpreter as closure!
		nullptr, nullptr);
}
void UiPause_real (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"real\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addReal (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_positive (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"positive\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addPositive (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_integer (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"integer\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addInteger (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_natural (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"natural\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addNatural (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_word (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"word\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addWord (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_sentence (conststring32 label, conststring32 defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"sentence\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addSentence (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_text (conststring32 label, conststring32 defaultValue, integer numberOfLines) {
	if (! thePauseForm)
		Melder_throw (U"The function \"text\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addText (thePauseForm.get(), nullptr, nullptr, label, defaultValue, numberOfLines);
}
void UiPause_boolean (conststring32 label, bool defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"boolean\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addBoolean (thePauseForm.get(), nullptr, nullptr, label, defaultValue);
}
void UiPause_choice (conststring32 label, int defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"choice\" should be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addRadio (thePauseForm.get(), nullptr, nullptr, nullptr, label, defaultValue, 1);
}
void UiPause_optionMenu (conststring32 label, int defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"optionMenu\" should be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addOptionMenu (thePauseForm.get(), nullptr, nullptr, nullptr, label, defaultValue, 1);
}
void UiPause_option (conststring32 label) {
	if (! thePauseForm)
		Melder_throw (U"The function \"option\" should be between a \"beginPause\" and an \"endPause\".");
	if (! thePauseFormRadio) {
		thePauseForm. reset();
		Melder_throw (U"Found the function \"option\" without a preceding \"choice\" or \"optionMenu\".");
	}
	UiOptionMenu_addButton (thePauseFormRadio, label);
}
void UiPause_comment (conststring32 label) {
	if (! thePauseForm)
		Melder_throw (U"The function \"comment\" should be between a \"beginPause\" and an \"endPause\".");
	UiForm_addLabel (thePauseForm.get(), nullptr, label);
}
int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	conststring32 continueText1, conststring32 continueText2, conststring32 continueText3,
	conststring32 continueText4, conststring32 continueText5, conststring32 continueText6,
	conststring32 continueText7, conststring32 continueText8, conststring32 continueText9,
	conststring32 continueText10, Interpreter interpreter)
{
	if (! thePauseForm)
		Melder_throw (U"Found the function \"endPause\" without a preceding \"beginPause\".");
	UiForm_setPauseForm (thePauseForm.get(), numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
		continueText1, continueText2, continueText3, continueText4, continueText5,
		continueText6, continueText7, continueText8, continueText9, continueText10,
		thePauseFormCancelCallback);
	theCancelContinueButton = cancelContinueButton;
	UiForm_finish (thePauseForm.get());
	bool wasBackgrounding = Melder_backgrounding;
	//if (theCurrentPraatApplication -> batch) goto end;
	if (wasBackgrounding)
		praat_foreground ();
	/*
	 * Put the pause form on the screen.
	 */
	UiForm_destroyWhenUnmanaged (thePauseForm.get());
	UiForm_do (thePauseForm.get(), false);
	/*
	 * Wait for the user to click Stop or Continue.
	 */
	{// scope
		autoMelderSaveDefaultDir saveDir;
		thePauseForm_clicked = 0;
		Melder_assert (theEventLoopDepth == 0);
		theEventLoopDepth ++;
		try {
			#if gtk
				do {
					gtk_main_iteration ();
				} while (! thePauseForm_clicked);
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
				} while (! thePauseForm_clicked);
			#elif motif
				do {
					XEvent event;
					GuiNextEvent (& event);
					XtDispatchEvent (& event);
				} while (! thePauseForm_clicked);
			#endif
		} catch (MelderError) {
			Melder_flushError (U"An error made it to the outer level in a pause window; should not occur! Please write to paul.boersma@uva.nl");
		}
		theEventLoopDepth --;
	}
	if (wasBackgrounding)
		praat_background ();
	/* BUG: should also restore praatP. editor. */
	thePauseForm. releaseToUser();   // undangle
	thePauseFormRadio = nullptr;   // undangle
	if (thePauseForm_clicked == -1) {
		Interpreter_stop (interpreter);
		Melder_throw (U"You interrupted the script.");
		//Melder_flushError ();
		//Melder_clearError ();
	} else {
		//Melder_casual (U"Clicked ", thePauseForm_clicked);
	}
	return thePauseForm_clicked;
}

/* End of file UiPause.cpp */

