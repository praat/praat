#ifndef _Ui_h_
#define _Ui_h_
/* Ui.h
 *
 * Copyright (C) 1992-2011,2012,2013,2015,2017 Paul Boersma
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

#include "Graphics.h"
#include "Gui.h"
#include "Interpreter.h"
Thing_declare (EditorCommand);

/* Forms for getting arguments from the user. */

/* Example of usage:
{
	static UiForm dia = nullptr;
	if (! dia) {
		UiField radio;
		dia = UiForm_create
		  (topShell,   // the parent GuiWindow of the dialog window
			U"Create a new person",   // the window title
			DO_Person_create,   // the function to call when the user clicks OK
			nullptr,   // the last argument to the OK routine (also for the other buttons); could be a ScriptEditor, or an EditorCommand, or an Interpreter, or nullptr
			U"Create person...",   // the invoking button title
			U"Create person...");   // the help string; may be nullptr
		static integer age;
		UiForm_addNatural (dia, & age, U"age", U"Age (years)", U"18");
		static double length;
		UiForm_addPositive (dia, & length, U"length", U"Length (metres)", U"1.68 (= average)");
		static bool beard;
		UiForm_addBoolean (dia, & beard, U"beard", U"Beard", false);
		static int sex;
		radio = UiForm_addRadio (dia, & sex, U"sex", U"Sex", 1);
			UiRadio_addButton (radio, U"Female");
			UiRadio_addButton (radio, U"Male");
		UiForm_addWord (dia, colour, U"colour", U"Colour", U"black");
		UiForm_addLabel (dia, U"features", U"Some less conspicuous features:");
		static integer numberOfBirthMarks;
		UiForm_addNatural (dia, & numberOfBirthMarks, U"numberOfBirthMarks", U"Number of birth marks", U"28");
		static char *favouriteGreeting;
		UiForm_addSentence (dia, & favouriteGreeting, U"favouriteGreeting", U"Favourite greeting", U"Good morning");
		UiForm_finish (dia);
	}
	UiForm_setReal (dia, & length, myLength);
	UiForm_setInteger (dia, & numberOfBirthMarks, 30);
	UiForm_do (dia, false);   // show dialog box
}
	Real, Positive, Integer, Natural, Channel, Word, and Sentence show a label and an editable text field.
	Radio shows a label and has Button children stacked below it.
	OptionMenu shows a label and has Button children in a menu.
	Label only shows its value.
	Text, Numvec and Nummat show an editable text field over the whole width of the form.
	Boolean shows a labeled toggle button which is on (true) or off (false).
	Button does the same inside a radio box or option menu.
	List shows a scrollable list.
	Colour shows a label and an editable text field for a grey value between 0.0 and 1.0, a colour name, or {r,g,b}.
	Channel shows a label and an editable text field for a natural number or one of the texts "Left", "Right", "Mono" or "Stereo".
	As shown in the example, Real, Positive, Integer, and Natural may contain extra text;
	this text is considered a comment.
	When you click "Standards", the standard values (including comments)
	are restored to all items in the form.
*/

Thing_define (UiOption, Thing) {
	GuiRadioButton radioButton;
	GuiObject menuItem;
};

Thing_define (UiField, Thing) {
	int type;
	const char32 *formLabel;
	double realValue;
	integer integerValue, integerDefaultValue;
	char32 *stringValue; const char32 *stringDefaultValue;
	autonumvec numericVectorValue;
	autonummat numericMatrixValue;
	Graphics_Colour colourValue;
	char *stringValueA;
	OrderedOf<structUiOption> options;
	integer numberOfStrings;
	const char32 **strings;
	GuiLabel label;
	GuiText text;
	GuiCheckButton checkButton;
	GuiRadioButton radioButton;
	GuiList list;
	GuiOptionMenu optionMenu;
	int y;

	const char32 *variableName;
	double *realVariable;
	integer *integerVariable;
	int *intVariable;
	bool *boolVariable;
	char32 **stringVariable;
	Graphics_Colour *colourVariable;

	numvec *numericVectorVariable;
	nummat *numericMatrixVariable;
	bool owned;

	int subtract;

	void v_destroy () noexcept
		override;
};

#define UiCallback_ARGS \
	UiForm sendingForm, int narg, Stackel args, const char32 *sendingString, Interpreter interpreter, const char32 *invokingButtonTitle, bool modified, void *closure
typedef void (*UiCallback) (UiCallback_ARGS);

#define MAXIMUM_NUMBER_OF_FIELDS  50
#define MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS  10

Thing_define (UiForm, Thing) {
	EditorCommand command;
	GuiWindow d_dialogParent;
	const char32 *invokingButtonTitle, *helpTitle;
	UiCallback okCallback;
	void *buttonClosure;

	/*
	 * In case the validity of the form depends on the selected objects.
	 */
	bool (*allowExecutionHook) (void *closure);
	void *allowExecutionClosure;

	/*
	 * In case the form is built by specifying buttons (rather than a system-built-in file dialog).
	 */
	GuiDialog d_dialogForm;
	void (*applyCallback) (UiForm dia, void *closure);
	void (*cancelCallback) (UiForm dia, void *closure);
	int numberOfContinueButtons, defaultContinueButton, cancelContinueButton, clickedContinueButton;
	const char32 *continueTexts [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];
	int numberOfFields;
	UiField field [1 + MAXIMUM_NUMBER_OF_FIELDS];
	GuiButton okButton, cancelButton, revertButton, helpButton, applyButton, continueButtons [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];
	bool destroyWhenUnmanaged, isPauseForm;

	/*
	 * In case the form contains a file.
	 */
	structMelderFile file;
	int shiftKeyPressed;
	bool allowMultipleFiles;   // for input

	void v_destroy () noexcept
		override;
};

/* The following functions work on the screen and from batch. */
UiForm UiForm_create (GuiWindow parent, const char32 *title,
	UiCallback okCallback, void *buttonClosure,
	const char32 *invokingButtonTitle, const char32 *helpTitle);
UiField UiForm_addReal (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addRealOrUndefined (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addPositive (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addInteger (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addNatural (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addWord (UiForm me, char32 **variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addSentence (UiForm me, char32 **variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addLabel (UiForm me, char32 **variable, const char32 *label);
UiField UiForm_addBoolean (UiForm me, bool *variable, const char32 *variableName, const char32 *label, int defaultValue);
UiField UiForm_addText (UiForm me, char32 **variable, const char32 *variableName, const char32 *name, const char32 *defaultValue);
UiField UiForm_addNumvec (UiForm me, numvec *variable, const char32 *variableName, const char32 *name, const char32 *defaultValue);
UiField UiForm_addNummat (UiForm me, nummat *variable, const char32 *variableName, const char32 *name, const char32 *defaultValue);
UiField UiForm_addRadio (UiForm me, int *intVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, int defaultValue, int base);
UiOption UiRadio_addButton (UiField me, const char32 *label);
UiField UiForm_addOptionMenu (UiForm me, int *intVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, int defaultValue, int base);
UiOption UiOptionMenu_addButton (UiField me, const char32 *label);
UiField UiForm_addList (UiForm me, integer *integerVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, integer numberOfStrings, const char32 **strings, integer defaultValue);
UiField UiForm_addColour (UiForm me, Graphics_Colour *colourVariable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
UiField UiForm_addChannel (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue);
void UiForm_finish (UiForm me);

void UiForm_destroyWhenUnmanaged (UiForm me);
void UiForm_setPauseForm (UiForm me,
	int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const char32 *continue1, const char32 *continue2, const char32 *continue3,
	const char32 *continue4, const char32 *continue5, const char32 *continue6,
	const char32 *continue7, const char32 *continue8, const char32 *continue9,
	const char32 *continue10,
	void (*cancelCallback) (UiForm dia, void *closure));

/* The following nine functions set values in widgets. */
/* Do not call from batch. */
/* 'fieldName' is name from UiForm_addXXXXXX (), */
/* without anything from and including the first " (" or ":". */

/* Real, RealOrUndefined, and Positive fields: */
	void UiForm_setReal (UiForm me, double *p_variable, double value);
	void UiForm_setRealAsString (UiForm me, double *p_variable, const char32 *stringValue /* cattable */);
/* Integer, Natural, Channel, and List fields: */
	void UiForm_setInteger (UiForm me, integer *p_variable, integer value);
	void UiForm_setIntegerAsString (UiForm me, integer *p_variable, const char32 *stringValue /* cattable */);
/* Word, Sentence, Text, and Label fields: */
	void UiForm_setString (UiForm me, char32 **p_variable, const char32 *text /* cattable */);
/* Boolean fields: */
	void UiForm_setBoolean (UiForm me, bool *p_variable, bool value);
/* Radio and OptionMenu fields: */
	void UiForm_setOption (UiForm me, int *p_variable, int value);
	void UiForm_setOptionAsString (UiForm me, int *p_variable, const char32 *stringValue /* cattable */);
/* Colour fields: */
	void UiForm_setColourAsGreyValue (UiForm me, Graphics_Colour *p_variable, double greyValue);

void UiForm_do (UiForm me, bool modified);
/*
	Function:
		put the form on the screen.
	Behaviour:
		If the user clicks "OK",
		the form will call the `okCallback` that was registered with UiForm_create ().
		   If the `okCallback` then returns 1, the form will disappear from the screen;
		if it returns 0, the form will stay on the screen; this can be used
		for enabling the user to repair mistakes in the form.

		If the user clicks "Apply",
		the form will call the `okCallback` that was registered with UiForm_create (),
		and the form disappears from the screen.

		If the user clicks "Cancel", the form disappears from the screen.

		If the user clicks "Help", the form calls `help` with the `helpTitle`
		and stays on the screen.

		When the form disappears from the screen, the values in the fields
		will remain until the next invocation of UiForm_do () for the same form.
	Arguments:
		the above behaviour describes the action when `modified` is false.
		If `modified` is true, the user does not have to click OK.
		The form will still appear on the screen,
		but the `okCallback` will be called immediately.
*/
void UiForm_info (UiForm me, int narg);

/*
	The `okCallback` can use the following seven functions to ask arguments.
	The field names are the `label` or `name` arguments to UiForm_addXXXXXX (),
	without anything from parentheses or from a colon.
	These functions work from the GUI as well as from a script.
*/
integer UiForm_getInteger (UiForm me, const char32 *fieldName);   // Integer, Natural, Boolean, Radio, List
char32 * UiForm_getString (UiForm me, const char32 *fieldName);   // Word, Sentence, Text, Numvec, Nummat, Radio, List
MelderFile UiForm_getFile (UiForm me, const char32 *fieldName);   // FileIn, FileOut

double UiForm_getReal_check (UiForm me, const char32 *fieldName);
integer UiForm_getInteger_check (UiForm me, const char32 *fieldName);
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

void UiInfile_do (UiForm me);

void UiOutfile_do (UiForm me, const char32 *defaultName);

MelderFile UiFile_getFile (UiForm me);

void UiFile_hide ();
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
char32 *UiHistory_get ();
void UiHistory_clear ();

void Ui_setAllowExecutionHook (bool (*allowExecutionHook) (void *closure), void *allowExecutionClosure);

void UiForm_Interpreter_addVariables (UiForm me, Interpreter interpreter);
int UiForm_getClickedContinueButton (UiForm me);

/* End of file Ui.h */
#endif
