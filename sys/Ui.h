#ifndef _Ui_h_
#define _Ui_h_
/* Ui.h
 *
 * Copyright (C) 1992-2005,2007-2020 Paul Boersma
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
	static autoUiForm dia;
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
		UiForm_addNatural (dia.get(), & age, U"age", U"Age (years)", U"18");
		static double length;
		UiForm_addPositive (dia.get(), & length, U"length", U"Length (metres)", U"1.68 (= average)");
		static bool beard;
		UiForm_addBoolean (dia.get(), & beard, U"beard", U"Beard", false);
		static int sex;
		radio = UiForm_addRadio (dia.get(), & sex, U"sex", U"Sex", 1);
			UiRadio_addButton (radio, U"Female");
			UiRadio_addButton (radio, U"Male");
		UiForm_addWord (dia.get(), colour, U"colour", U"Colour", U"black");
		UiForm_addLabel (dia.get(), U"features", U"Some less conspicuous features:");
		static integer numberOfBirthMarks;
		UiForm_addNatural (dia.get(), & numberOfBirthMarks, U"numberOfBirthMarks", U"Number of birth marks", U"28");
		static char *favouriteGreeting;
		UiForm_addSentence (dia.get(), & favouriteGreeting, U"favouriteGreeting", U"Favourite greeting", U"Good morning");
		UiForm_finish (dia.get());
	}
	UiForm_setReal (dia.get(), & length, myLength);
	UiForm_setInteger (dia.get(), & numberOfBirthMarks, 30);
	UiForm_do (dia.get(), false);   // show dialog box
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

enum class _kUiField_type {
	REAL_ = 1,
	REAL_OR_UNDEFINED_ = 2,
	POSITIVE_ = 3,
	INTEGER_ = 4,
	NATURAL_ = 5,
	WORD_ = 6,
	SENTENCE_ = 7,
	COLOUR_ = 8,
	CHANNEL_ = 9,
	LABEL_ = 10,
	TEXT_ = 11,
	NUMVEC_ = 12,
	NUMMAT_ = 13,
	BOOLEAN_ = 14,
	RADIO_ = 15,
	OPTIONMENU_ = 16,
	LIST_ = 17,
	LABELLED_TEXT_MIN_ = 1,
	LABELLED_TEXT_MAX_ = 9
};

Thing_define (UiField, Thing) {
	_kUiField_type type;
	autostring32 formLabel;
	double realValue;
	integer integerValue, integerDefaultValue;
	autostring32 stringValue, stringDefaultValue;
	autoVEC numericVectorValue;
	autoMAT numericMatrixValue;
	MelderColour colourValue;
	OrderedOf<structUiOption> options;
	constSTRVEC strings;
	GuiLabel label;
	GuiText text;
	GuiCheckButton checkButton;
	GuiRadioButton radioButton;
	GuiList list;
	GuiOptionMenu optionMenu;
	int y;

	conststring32 variableName;   // a reference to a name known at compile time, for use by the Praat library
	double *realVariable;
	integer *integerVariable;
	int *intVariable;
	bool *boolVariable;
	conststring32 *stringVariable;
	MelderColour *colourVariable;
	constVEC *numericVectorVariable;
	constMAT *numericMatrixVariable;

	int subtract;
	integer numberOfLines;

	void v_destroy () noexcept
		override;
};

#define UiCallback_ARGS \
	UiForm _sendingForm, integer _narg, Stackel _args, conststring32 _sendingString, Interpreter interpreter, conststring32 _invokingButtonTitle, bool _isModified, void *_closure
typedef void (*UiCallback) (UiCallback_ARGS);

#define MAXIMUM_NUMBER_OF_FIELDS  50
#define MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS  10

Thing_define (UiForm, Thing) {
	EditorCommand command;
	GuiWindow d_dialogParent;
	autostring32 invokingButtonTitle, helpTitle;
	UiCallback okCallback;
	void *buttonClosure;

	/*
		In case the validity of the form depends on the selected objects.
	*/
	bool (*allowExecutionHook) (void *closure);
	void *allowExecutionClosure;

	/*
		In case the form is built by specifying buttons (rather than a system-built-in file dialog).
	*/
	GuiDialog d_dialogForm;
	void (*applyCallback) (UiForm dia, void *closure);
	void (*cancelCallback) (UiForm dia, void *closure);
	int numberOfContinueButtons, defaultContinueButton, cancelContinueButton, clickedContinueButton;
	conststring32 continueTexts [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];   // references to strings owned by a script
	int numberOfFields;
	autoUiField field [1 + MAXIMUM_NUMBER_OF_FIELDS];
	GuiButton okButton, cancelButton, revertButton, helpButton, applyButton, continueButtons [1 + MAXIMUM_NUMBER_OF_CONTINUE_BUTTONS];
	bool destroyWhenUnmanaged, isPauseForm;

	/*
		In case the form contains a file.
	*/
	structMelderFile file;
	int shiftKeyPressed;
	bool allowMultipleFiles;   // for input

	void v_destroy () noexcept
		override;
};

/* The following functions work on the screen and from batch. */
autoUiForm UiForm_create (GuiWindow parent, conststring32 title,
	UiCallback okCallback, void *buttonClosure,
	conststring32 invokingButtonTitle, conststring32 helpTitle);
UiField UiForm_addReal (UiForm me, double *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addRealOrUndefined (UiForm me, double *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addPositive (UiForm me, double *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addInteger (UiForm me, integer *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addNatural (UiForm me, integer *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addWord (UiForm me, conststring32 *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addSentence (UiForm me, conststring32 *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addLabel (UiForm me, conststring32 *variable, conststring32 label);
UiField UiForm_addBoolean (UiForm me, bool *variable, conststring32 variableName, conststring32 label, bool defaultValue);
UiField UiForm_addText (UiForm me, conststring32 *variable, conststring32 variableName, conststring32 name, conststring32 defaultValue, integer numberOfLines = 1);
UiField UiForm_addNumvec (UiForm me, constVEC *variable, conststring32 variableName, conststring32 name, conststring32 defaultValue);
UiField UiForm_addNummat (UiForm me, constMAT *variable, conststring32 variableName, conststring32 name, conststring32 defaultValue);
UiField UiForm_addRadio (UiForm me, int *intVariable, conststring32 *stringVariable, conststring32 variableName, conststring32 label, int defaultValue, int base);
UiOption UiRadio_addButton (UiField me, conststring32 label);
UiField UiForm_addOptionMenu (UiForm me, int *intVariable, conststring32 *stringVariable, conststring32 variableName, conststring32 label, int defaultValue, int base);
UiOption UiOptionMenu_addButton (UiField me, conststring32 label);
UiField UiForm_addList (UiForm me, integer *integerVariable, conststring32 *stringVariable, conststring32 variableName, conststring32 label, constSTRVEC strings, integer defaultValue);
UiField UiForm_addColour (UiForm me, MelderColour *colourVariable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
UiField UiForm_addChannel (UiForm me, integer *variable, conststring32 variableName, conststring32 label, conststring32 defaultValue);
void UiForm_finish (UiForm me);

void UiForm_destroyWhenUnmanaged (UiForm me);
void UiForm_setPauseForm (UiForm me,
	int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	conststring32 continue1, conststring32 continue2, conststring32 continue3,
	conststring32 continue4, conststring32 continue5, conststring32 continue6,
	conststring32 continue7, conststring32 continue8, conststring32 continue9,
	conststring32 continue10,
	void (*cancelCallback) (UiForm dia, void *closure));

/* The following nine functions set values in widgets. */
/* Do not call from batch. */
/* 'fieldName' is name from UiForm_addXXXXXX (), */
/* without anything from and including the first " (" or ":". */

/* Real, RealOrUndefined, and Positive fields: */
	void UiForm_setReal (UiForm me, double *p_variable, double value);
	void UiForm_setRealAsString (UiForm me, double *p_variable, conststring32 stringValue /* cattable */);
/* Integer, Natural, Channel, and List fields: */
	void UiForm_setInteger (UiForm me, integer *p_variable, integer value);
	void UiForm_setIntegerAsString (UiForm me, integer *p_variable, conststring32 stringValue /* cattable */);
/* Word, Sentence, Text, and Label fields: */
	void UiForm_setString (UiForm me, conststring32 *p_variable, conststring32 text /* cattable */);
/* Boolean fields: */
	void UiForm_setBoolean (UiForm me, bool *p_variable, bool value);
/* Radio and OptionMenu fields: */
	void UiForm_setOption (UiForm me, int *p_variable, int value);
	void UiForm_setOptionAsString (UiForm me, int *p_variable, conststring32 stringValue /* cattable */);
/* Colour fields: */
	void UiForm_setColourAsGreyValue (UiForm me, MelderColour *p_variable, double greyValue);

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
void UiForm_info (UiForm me, integer narg);

/*
	The `okCallback` can use the following seven functions to ask arguments.
	The field names are the `label` or `name` arguments to UiForm_addXXXXXX (),
	without anything from parentheses or from a colon.
	These functions work from the GUI as well as from a script.
*/
integer UiForm_getInteger (UiForm me, conststring32 fieldName);   // Integer, Natural, Boolean, Radio, List
char32 * UiForm_getString (UiForm me, conststring32 fieldName);   // Word, Sentence, Text, Numvec, Nummat, Radio, List
MelderFile UiForm_getFile (UiForm me, conststring32 fieldName);   // FileIn, FileOut

double UiForm_getReal_check (UiForm me, conststring32 fieldName);
integer UiForm_getInteger_check (UiForm me, conststring32 fieldName);
char32 * UiForm_getString_check (UiForm me, conststring32 fieldName);
MelderColour UiForm_getColour_check (UiForm me, conststring32 fieldName);

void UiForm_call (UiForm me, integer narg, Stackel args, Interpreter interpreter);
void UiForm_parseString (UiForm me, conststring32 arguments, Interpreter interpreter);

autoUiForm UiInfile_create (GuiWindow parent, conststring32 title,
  UiCallback okCallback, void *okClosure,
  conststring32 invokingButtonTitle, conststring32 helpTitle, bool allowMultipleFiles);

autoUiForm UiOutfile_create (GuiWindow parent, conststring32 title,
  UiCallback okCallback, void *okClosure,
  conststring32 invokingButtonTitle, conststring32 helpTitle);

void UiInfile_do (UiForm me);

void UiOutfile_do (UiForm me, conststring32 defaultName);

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

void UiHistory_write (conststring32 string);
void UiHistory_write_expandQuotes (conststring32 string);
void UiHistory_write_colonize (conststring32 string);
char32 *UiHistory_get ();
void UiHistory_clear ();

void Ui_setAllowExecutionHook (bool (*allowExecutionHook) (void *closure), void *allowExecutionClosure);

void UiForm_Interpreter_addVariables (UiForm me, Interpreter interpreter);
int UiForm_getClickedContinueButton (UiForm me);

/* End of file Ui.h */
#endif
