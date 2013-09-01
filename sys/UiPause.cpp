/* UiPause.cpp
 *
 * Copyright (C) 2009-2011,2013 Paul Boersma
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
 * pb 2009/01/20 created
 * pb 2010/07/13 GTK
 * pb 2010/07/26 removed UiFile_hide
 * pb 2011/02/01 cancelContinueButton
 * pb 2011/05/15 C++
 * pb 2011/07/12 C++
 */

#include "UiPause.h"
#include "praatP.h"

static UiForm thePauseForm = NULL;
static Any thePauseFormRadio = NULL;
static int thePauseForm_clicked = 0;
static int theCancelContinueButton = 0;
static int theEventLoopDepth = 0;

static void thePauseFormOkCallback (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *closure) {
	(void) sendingForm;
	(void) sendingString;
	(void) interpreter;
	(void) invokingButtonTitle;
	(void) modified;
	/* BUG: should also restore praatP. editor. */
	/*
	 * Get the data from the pause form.
	 */
	thePauseForm_clicked = UiForm_getClickedContinueButton (thePauseForm);
	if (thePauseForm_clicked != theCancelContinueButton)
		UiForm_Interpreter_addVariables (thePauseForm, (Interpreter) closure);   // 'closure', not 'interpreter' or 'theInterpreter'!
}
static void thePauseFormCancelCallback (Any dia, void *closure) {
	(void) dia;
	(void) closure;
	if (theCancelContinueButton != 0) {
		thePauseForm_clicked = theCancelContinueButton;
	} else {
		thePauseForm_clicked = -1;   // STOP button
	}
}
void UiPause_begin (GuiWindow topShell, const wchar_t *title, Interpreter interpreter) {
	if (theEventLoopDepth > 0)
		Melder_throw ("Praat cannot have more than one pause form at a time.");
	forget (thePauseForm);   // in case an earlier build-up of another pause window was interrupted
	thePauseForm = UiForm_create (topShell, Melder_wcscat (L"Pause: ", title),
		thePauseFormOkCallback, interpreter,   // pass interpreter as closure!
		NULL, NULL);
}
void UiPause_real (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"real\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addReal (thePauseForm, label, defaultValue);
}
void UiPause_positive (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"positive\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addPositive (thePauseForm, label, defaultValue);
}
void UiPause_integer (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"integer\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addInteger (thePauseForm, label, defaultValue);
}
void UiPause_natural (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"natural\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addNatural (thePauseForm, label, defaultValue);
}
void UiPause_word (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"word\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addWord (thePauseForm, label, defaultValue);
}
void UiPause_sentence (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"sentence\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addSentence (thePauseForm, label, defaultValue);
}
void UiPause_text (const wchar_t *label, const wchar_t *defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"text\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addText (thePauseForm, label, defaultValue);
}
void UiPause_boolean (const wchar_t *label, int defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"boolean\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addBoolean (thePauseForm, label, defaultValue);
}
void UiPause_choice (const wchar_t *label, int defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"choice\" has to be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addRadio (thePauseForm, label, defaultValue);
}
void UiPause_optionMenu (const wchar_t *label, int defaultValue) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"optionMenu\" has to be between a \"beginPause\" and an \"endPause\".");
	thePauseFormRadio = UiForm_addOptionMenu (thePauseForm, label, defaultValue);
}
void UiPause_option (const wchar_t *label) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"option\" has to be between a \"beginPause\" and an \"endPause\".");
	if (thePauseFormRadio == NULL) {
		forget (thePauseForm);
		Melder_throw ("Found the function \"option\" without a preceding \"choice\" or \"optionMenu\".");
	}
	UiOptionMenu_addButton (thePauseFormRadio, label);
}
void UiPause_comment (const wchar_t *label) {
	if (thePauseForm == NULL)
		Melder_throw ("The function \"comment\" has to be between a \"beginPause\" and an \"endPause\".");
	UiForm_addLabel (thePauseForm, L"", label);
}
int UiPause_end (int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const wchar_t *continueText1, const wchar_t *continueText2, const wchar_t *continueText3,
	const wchar_t *continueText4, const wchar_t *continueText5, const wchar_t *continueText6,
	const wchar_t *continueText7, const wchar_t *continueText8, const wchar_t *continueText9,
	const wchar_t *continueText10, Interpreter interpreter)
{
	if (thePauseForm == NULL)
		Melder_throw ("Found the function \"endPause\" without a preceding \"beginPause\".");
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
	#ifndef CONSOLE_APPLICATION
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
				#elif motif
					do {
						XEvent event;
						GuiNextEvent (& event);
						XtDispatchEvent (& event);
					} while (! thePauseForm_clicked);
				#endif
			} catch (MelderError) {
				Melder_flushError ("An error made it to the outer level in a pause window; should not occur! Please write to paul.boersma@uva.nl");
			}
			theEventLoopDepth --;
		}
		if (wasBackgrounding) praat_background ();
		/* BUG: should also restore praatP. editor. */
		thePauseForm = NULL;   // undangle
		thePauseFormRadio = NULL;   // undangle
		if (thePauseForm_clicked == -1) {
			Interpreter_stop (interpreter);
			Melder_throw ("You interrupted the script.");
			//Melder_flushError (NULL);
			//Melder_clearError ();
		} else {
			//Melder_casual ("Clicked %d", thePauseForm_clicked);
		}
	#endif
	return thePauseForm_clicked;
}

/* End of file UiPause.cpp */

