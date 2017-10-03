/* UiPause.cpp
 *
 * Copyright (C) 2009-2011,2013,2015,2017 Paul Boersma
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

static UiForm thePauseForm = nullptr;
static UiField thePauseFormRadio = nullptr;
static int thePauseForm_clicked = 0;
static int theCancelContinueButton = 0;
static int theEventLoopDepth = 0;

static void thePauseFormOkCallback (UiForm /* sendingForm */, int /* narg */, Stackel /* args */,
	const char32 * /* sendingString */, Interpreter /* interpreter */,
	const char32 * /* invokingButtonTitle */, bool /* modified */, void *closure) {
	/* BUG: should also restore praatP. editor. */
	/*
	 * Get the data from the pause form.
	 */
	thePauseForm_clicked = UiForm_getClickedContinueButton (thePauseForm);
	if (thePauseForm_clicked != theCancelContinueButton)
		UiForm_Interpreter_addVariables (thePauseForm, (Interpreter) closure);   // 'closure', not 'interpreter' or 'theInterpreter'!
}
static void thePauseFormCancelCallback (UiForm /* dia */, void * /* closure */) {
	if (theCancelContinueButton != 0) {
		thePauseForm_clicked = theCancelContinueButton;
	} else {
		thePauseForm_clicked = -1;   // STOP button
	}
}
void UiPause_begin (GuiWindow topShell, const char32 *title, Interpreter interpreter) {
	if (theEventLoopDepth > 0)
		Melder_throw (U"Praat cannot have more than one pause form at a time.");
	forget (thePauseForm);   // in case an earlier build-up of another pause window was interrupted
	thePauseForm = UiForm_create (topShell, Melder_cat (U"Pause: ", title),
		thePauseFormOkCallback, interpreter,   // pass interpreter as closure!
		nullptr, nullptr);
}
void UiPause_real (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"real\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addReal (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_positive (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"positive\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addPositive (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_integer (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"integer\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addInteger (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_natural (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"natural\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addNatural (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_word (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"word\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addWord (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_sentence (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"sentence\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addSentence (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_text (const char32 *label, const char32 *defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"text\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addText (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_boolean (const char32 *label, int defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"boolean\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addBoolean (thePauseForm, nullptr, nullptr, label, defaultValue);
}
void UiPause_choice (const char32 *label, int defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"choice\" has to be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addRadio (thePauseForm, nullptr, nullptr, nullptr, label, defaultValue, 1);
}
void UiPause_optionMenu (const char32 *label, int defaultValue) {
	if (! thePauseForm)
		Melder_throw (U"The function \"optionMenu\" has to be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addOptionMenu (thePauseForm, nullptr, nullptr, nullptr, label, defaultValue, 1);
}
void UiPause_option (const char32 *label) {
	if (! thePauseForm)
		Melder_throw (U"The function \"option\" has to be between a \"beginPause\" and an \"endPause\".");
	if (! thePauseFormRadio) {
		forget (thePauseForm);
		Melder_throw (U"Found the function \"option\" without a preceding \"choice\" or \"optionMenu\".");
	}
	UiOptionMenu_addButton (thePauseFormRadio, label);
}
void UiPause_comment (const char32 *label) {
	if (! thePauseForm)
		Melder_throw (U"The function \"comment\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addLabel (thePauseForm, nullptr, label);
}
int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const char32 *continueText1, const char32 *continueText2, const char32 *continueText3,
	const char32 *continueText4, const char32 *continueText5, const char32 *continueText6,
	const char32 *continueText7, const char32 *continueText8, const char32 *continueText9,
	const char32 *continueText10, Interpreter interpreter)
{
	if (! thePauseForm)
		Melder_throw (U"Found the function \"endPause\" without a preceding \"beginPause\".");
	UiForm_setPauseForm (thePauseForm, numberOfContinueButtons, defaultContinueButton, cancelContinueButton,
		continueText1, continueText2, continueText3, continueText4, continueText5,
		continueText6, continueText7, continueText8, continueText9, continueText10,
		thePauseFormCancelCallback);
	theCancelContinueButton = cancelContinueButton;
	UiForm_finish (thePauseForm);
	bool wasBackgrounding = Melder_backgrounding;
	//if (theCurrentPraatApplication -> batch) goto end;
	if (wasBackgrounding) praat_foreground ();
	/*
	 * Put the pause form on the screen.
	 */
	UiForm_destroyWhenUnmanaged (thePauseForm);
	UiForm_do (thePauseForm, false);
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
	if (wasBackgrounding) praat_background ();
	/* BUG: should also restore praatP. editor. */
	thePauseForm = nullptr;   // undangle
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

