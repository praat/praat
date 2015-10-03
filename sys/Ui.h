#ifndef _Ui_h_
#define _Ui_h_
/* Ui.h
 *
 * Copyright (C) 1992-2011,2012,2013,2015 Paul Boersma
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

#include "Graphics.h"
#include "Gui.h"
#include "Interpreter.h"
Thing_declare (EditorCommand);

/* Forms for getting arguments from the user. */

/* Example of usage:
{
	static UiForm dia = NULL;
	if (dia == NULL) {
		Any radio;
		dia = UiForm_create
		  (topShell,   // The parent GuiWindow of the dialog window.
			U"Create a new person",   // The window title.
			DO_Person_create,   // The routine to call when the user clicks OK.
			NULL,   // The last argument to the OK routine (also for the other buttons). Could be a ScriptEditor, or an EditorCommand, or an Interpreter, or NULL.
			U"Create person...",   // The invoking button title.
			U"Create person...");   // The help string; may be NULL.
		UiForm_addNatural (dia, U"Age (years)", U"18");
		UiForm_addPositive (dia, U"Length (metres)", U"1.68 (average)");
		UiForm_addBoolean (dia, U"Beard", false);
		radio = UiForm_addRadio (dia, U"Sex", 1);
			UiRadio_addButton (radio, U"Female");
			UiRadio_addButton (radio, U"Male");
		UiForm_addWord (dia, U"Colour", U"black");
		UiForm_addLabel (dia, U"features", U"Some less conspicuous features:");
		UiForm_addNatural (dia, U"Number of birth marks", U"28");
		UiForm_addSentence (dia, U"Favourite greeting", U"Good morning");
		UiForm_finish (dia);
	}
	UiForm_setReal (dia, U"Length", myLength);
	UiForm_setInteger (dia, U"Number of birth marks", 30);
	UiForm_do (dia, 0);   // Show dialog box.
}
	Real, Positive, Integer, Natural, Word, and Sentence
		show a label (name) and an editable text field (value).
	Radio shows a label (name) and has Button children.
	OptionMenu shows a label (name) and has Button children in a menu.
	Label only shows its value.
	Text only shows an editable text field (value).
	Boolean shows a labeled toggle button which is on (1) or off (0).
	Button does the same inside a radio box or option menu.
	List shows a scrollable list.
	Colour shows a label (name) and an editable text field for a grey value between 0 and 1, a colour name, ar {r,g,b}.
	Channel shows a label (name) and an editable text field for a natural number or the text Left or Right.
	As shown in the example, Real, Positive, Integer, Natural, and Word may contain extra text;
	this text is considered as comments and is erased as soon as you click OK.
	When you click "Standards", the standard values (including comments)
	are restored to all items in the form.
*/

Thing_define (UiField, Thing) {
	int type;
	const char32 *formLabel;
	double realValue, realDefaultValue;
	long integerValue, integerDefaultValue;
	char32 *stringValue; const char32 *stringDefaultValue;
	Graphics_Colour colourValue;
	char *stringValueA;
	Ordered options;
	long numberOfStrings;
	const char32 **strings;
	GuiLabel label;
	GuiText text;
	GuiCheckButton checkButton;
	GuiRadioButton radioButton;
	GuiList list;
	GuiOptionMenu optionMenu;
	int y;

	void v_destroy ()
		override;
};

typedef void (*UiCallback) (UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *closure);

#define MAXIMUM_NUMBER_OF_FIELDS  50
#define MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS  10

Thing_define (UiForm, Thing) {
	EditorCommand command;
	GuiWindow d_dialogParent;
	GuiDialog d_dialogForm;
	UiCallback okCallback;
	void (*applyCallback) (Any dia, void *closure);
	void (*cancelCallback) (Any dia, void *closure);
	void *buttonClosure;
	const char32 *invokingButtonTitle, *helpTitle;
	int numberOfContinueButtons, defaultContinueButton, cancelContinueButton, clickedContinueButton;
	const char32 *continueTexts [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];
	int numberOfFields;
	UiField field [1 + MAXIMUM_NUMBER_OF_FIELDS];
	GuiButton okButton, cancelButton, revertButton, helpButton, applyButton, continueButtons [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];
	bool destroyWhenUnmanaged, isPauseForm;
	bool (*allowExecutionHook) (void *closure);
	void *allowExecutionClosure;

	void v_destroy ()
		override;
};

/* The following routines work on the screen and from batch. */
UiForm UiForm_create (GuiWindow parent, const char32 *title,
	UiCallback okCallback, void *buttonClosure,
	const char32 *invokingButtonTitle, const char32 *helpTitle);
Any UiForm_addReal (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addRealOrUndefined (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addPositive (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addInteger (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addNatural (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addWord (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addSentence (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addLabel (UiForm me, const char32 *name, const char32 *label);
Any UiForm_addBoolean (UiForm me, const char32 *label, int defaultValue);
Any UiForm_addText (UiForm me, const char32 *name, const char32 *defaultValue);
Any UiForm_addRadio (UiForm me, const char32 *label, int defaultValue);
Any UiRadio_addButton (I, const char32 *label);
Any UiForm_addOptionMenu (UiForm me, const char32 *label, int defaultValue);
Any UiOptionMenu_addButton (I, const char32 *label);
Any UiForm_addList (UiForm me, const char32 *label, long numberOfStrings, const char32 **strings, long defaultValue);
Any UiForm_addColour (UiForm me, const char32 *label, const char32 *defaultValue);
Any UiForm_addChannel (UiForm me, const char32 *label, const char32 *defaultValue);
void UiForm_finish (UiForm me);

void UiForm_destroyWhenUnmanaged (UiForm me);
void UiForm_setPauseForm (UiForm me,
	int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const char32 *continue1, const char32 *continue2, const char32 *continue3,
	const char32 *continue4, const char32 *continue5, const char32 *continue6,
	const char32 *continue7, const char32 *continue8, const char32 *continue9,
	const char32 *continue10,
	void (*cancelCallback) (Any dia, void *closure));

/* The following three routines set values in widgets. */
/* Do not call from batch. */
/* 'fieldName' is name from UiForm_addXXXXXX (), */
/* without anything from and including the first " (" or ":". */
void UiForm_setString (UiForm me, const char32 *fieldName, const char32 *text /* cattable */);
	/* Real, Positive, Integer, Natural, Word, Sentence, Label, Text, Radio, List. */
void UiForm_setReal (UiForm me, const char32 *fieldName, double value);
	/* Real, Positive. */
void UiForm_setInteger (UiForm me, const char32 *fieldName, long value);
	/* Integer, Natural, Boolean, Radio, List. */

void UiForm_do (UiForm me, bool modified);
/*
	Function:
		put the form on the screen.
	Behaviour:
		If the user clicks "OK",
		the form will call the okCallback that was registered with UiForm_create ().
		   If the okCallback then returns 1, the form will disappear from the screen;
		if it returns 0, the form will stay on the screen; this can be used
		for enabling the user to repair mistakes in the form.

		If the user clicks "Apply",
		the form will call the okCallback that was registered with UiForm_create (),
		and the form disappears from the screen.

		If the user clicks "Cancel", the form disappears from the screen.

		If the user clicks "Help", the form calls "help" with the "helpTitle"
		and stays on the screen.

		When the form disappears from the screen, the values in the fields
		will remain until the next invocation of UiForm_do () for the same form.
	Arguments:
		the above behaviour describes the action when 'modified' is 0.
		If 'modified' is set, the user does not have to click OK.
		The form will still appear on the screen,
		but the okCallback will be called immediately.
*/

/* The 'okCallback' can use the following four routines to ask arguments. */
/* The field names are the 'label' or 'name' arguments to UiForm_addXXXXXX (), */
/* without anything from parentheses or from a colon. */
/* These routines work from the screen and from batch. */
double UiForm_getReal (UiForm me, const char32 *fieldName);	// Real, Positive
long UiForm_getInteger (UiForm me, const char32 *fieldName);	// Integer, Natural, Boolean, Radio, List
char32 * UiForm_getString (UiForm me, const char32 *fieldName);	// Word, Sentence, Text, Radio, List
Graphics_Colour UiForm_getColour (UiForm me, const char32 *fieldName);   // Colour
MelderFile UiForm_getFile (UiForm me, const char32 *fieldName); // FileIn, FileOut

double UiForm_getReal_check (UiForm me, const char32 *fieldName);
long UiForm_getInteger_check (UiForm me, const char32 *fieldName);
char32 * UiForm_getString_check (UiForm me, const char32 *fieldName);
Graphics_Colour UiForm_getColour_check (UiForm me, const char32 *fieldName);

void UiForm_call (UiForm me, int narg, Stackel args, Interpreter interpreter);
void UiForm_parseString (UiForm me, const char32 *arguments, Interpreter interpreter);

UiForm UiInfile_create (GuiWindow parent, const char32 *title,
  UiCallback okCallback, void *okClosure,
  const char32 *invokingButtonTitle, const char32 *helpTitle, bool allowMultipleFiles);

UiForm UiOutfile_create (GuiWindow parent, const char32 *title,
  UiCallback okCallback, void *okClosure,
  const char32 *invokingButtonTitle, const char32 *helpTitle);

void UiInfile_do (I);

void UiOutfile_do (I, const char32 *defaultName);

MelderFile UiFile_getFile (I);

void UiFile_hide (void);
/*
	Hides the visible UiFile that was opened most recently.
	Normally, file selectors stay open until their okCallback has completed.
	However, the okCallback may initiate an event loop allowing the user
	to interact with the application, for instance in Melder_pause ().
	In order that the user does not have to hide the modal file selector
	manually (by clicking the Cancel button), the application can call
	UiFile_hide () before Melder_pause ().
*/

void UiHistory_write (const char32 *string);
void UiHistory_write_expandQuotes (const char32 *string);
void UiHistory_write_colonize (const char32 *string);
char32 *UiHistory_get (void);
void UiHistory_clear (void);

void Ui_setAllowExecutionHook (bool (*allowExecutionHook) (void *closure), void *allowExecutionClosure);

void UiForm_widgetsToValues (UiForm me);
void UiForm_Interpreter_addVariables (UiForm me, Interpreter interpreter);
int UiForm_getClickedContinueButton (UiForm me);

/* End of file Ui.h */
#endif
