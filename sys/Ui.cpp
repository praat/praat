/* Ui.cpp
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#include <wctype.h>
#include <ctype.h>
#include "longchar.h"
#include "machine.h"
#include "GuiP.h"
#include "Collection.h"
#include "UiP.h"
#include "Editor.h"
#include "Graphics.h"   // colours

/***** class UiField: the things that have values in an UiForm dialog *****/

Thing_implement (UiField, Thing, 0);

void structUiField :: v_destroy () noexcept {
	Melder_free (our formLabel);
	Melder_free (our stringValue);
	Melder_free (our stringDefaultValue);
	if (our owned) {
		our numericVectorVariable -> reset();
		our numericMatrixVariable -> reset();
	}
	our UiField_Parent :: v_destroy ();
}

static UiField UiField_create (_kUiField_type type, const char32 *name) {
	autoUiField me = Thing_new (UiField);
	char32 shortName [1+100], *p;
	my type = type;
	my formLabel = Melder_dup (name);
	str32ncpy (shortName, name, 100);
	shortName [100] = U'\0';
	/*
		Strip parentheses and colon off parameter name.
	*/
	if (!! (p = (char32 *) str32chr (shortName, U'('))) {
		*p = U'\0';
		if (p - shortName > 0 && p [-1] == U' ') p [-1] = U'\0';
	}
	p = shortName;
	if (*p != U'\0' && p [str32len (p) - 1] == U':') p [str32len (p) - 1] = U'\0';
	Thing_setName (me.get(), shortName);
	return me.releaseToAmbiguousOwner();
}

/***** class UiOption: radio buttons and menu options *****/

Thing_implement (UiOption, Thing, 0);

static autoUiOption UiOption_create (const char32 *label) {
	autoUiOption me = Thing_new (UiOption);
	Thing_setName (me.get(), label);
	return me;
}

UiOption UiRadio_addButton (UiField me, const char32 *label) {
	if (! me) return nullptr;
	Melder_assert (my type == _kUiField_type::RADIO_ || my type == _kUiField_type::OPTIONMENU_);
	autoUiOption thee = UiOption_create (label);
	return my options. addItem_move (thee.move());
}

UiOption UiOptionMenu_addButton (UiField me, const char32 *label) {
	if (! me) return nullptr;
	Melder_assert (my type == _kUiField_type::RADIO_ || my type == _kUiField_type::OPTIONMENU_);
	autoUiOption thee = UiOption_create (label);
	return my options. addItem_move (thee.move());
}

/***** Things to do with UiField objects. *****/

static void UiField_setDefault (UiField me) {
	switch (my type) {
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::COLOUR_:
		case _kUiField_type::CHANNEL_:
		case _kUiField_type::TEXT_:
		case _kUiField_type::NUMVEC_:
		case _kUiField_type::NUMMAT_:
		{
			GuiText_setString (my text, my stringDefaultValue);
		}
		break;
		case _kUiField_type::BOOLEAN_:
		{
			GuiCheckButton_setValue (my checkButton, my integerDefaultValue);
		}
		break;
		case _kUiField_type::RADIO_:
		{
			for (int i = 1; i <= my options.size; i ++) {
				if (i == my integerDefaultValue) {
					UiOption b = my options.at [i];
					GuiRadioButton_set (b -> radioButton);
				}
			}
		}
		break;
		case _kUiField_type::OPTIONMENU_:
		{
			GuiOptionMenu_setValue (my optionMenu, my integerDefaultValue);
		}
		break;
		case _kUiField_type::LIST_:
		{
			GuiList_selectItem (my list, my integerDefaultValue);
		}
	}
}

static int colourToValue (UiField me, char32 *string) {
	char32 *p = string;
	while (*p == U' ' || *p == U'\t') p ++;
	*p = (char32) tolower ((int) *p);
	char32 first = *p;
	if (first == U'{') {
		my colourValue. red = Melder_atof (++ p);
		p = str32chr (p, U',');
		if (! p) return 0;
		my colourValue. green = Melder_atof (++ p);
		p = str32chr (p, U',');
		if (! p) return 0;
		my colourValue. blue = Melder_atof (++ p);
	} else {
		*p = (char32) tolower ((int) *p);
		if (str32equ (p, U"black")) my colourValue = Graphics_BLACK;
		else if (str32equ (p, U"white")) my colourValue = Graphics_WHITE;
		else if (str32equ (p, U"red")) my colourValue = Graphics_RED;
		else if (str32equ (p, U"green")) my colourValue = Graphics_GREEN;
		else if (str32equ (p, U"blue")) my colourValue = Graphics_BLUE;
		else if (str32equ (p, U"yellow")) my colourValue = Graphics_YELLOW;
		else if (str32equ (p, U"cyan")) my colourValue = Graphics_CYAN;
		else if (str32equ (p, U"magenta")) my colourValue = Graphics_MAGENTA;
		else if (str32equ (p, U"maroon")) my colourValue = Graphics_MAROON;
		else if (str32equ (p, U"lime")) my colourValue = Graphics_LIME;
		else if (str32equ (p, U"navy")) my colourValue = Graphics_NAVY;
		else if (str32equ (p, U"teal")) my colourValue = Graphics_TEAL;
		else if (str32equ (p, U"purple")) my colourValue = Graphics_PURPLE;
		else if (str32equ (p, U"olive")) my colourValue = Graphics_OLIVE;
		else if (str32equ (p, U"pink")) my colourValue = Graphics_PINK;
		else if (str32equ (p, U"silver")) my colourValue = Graphics_SILVER;
		else if (str32equ (p, U"grey")) my colourValue = Graphics_GREY;
		else { *p = first; return 0; }
		*p = first;
	}
	return 1;
}

#define EVALUATE_WIDGET_REPRESENTATIONS  0
static void UiField_widgetToValue (UiField me) {
	switch (my type)
	{
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		{
			autostring32 dirty = GuiText_getString (my text);   // the text as typed by the user
			Interpreter_numericExpression (nullptr, dirty.peek(), & my realValue);
			#if EVALUATE_WIDGET_REPRESENTATIONS
				/*
					Put a clean version of the new value in the form.
					If the value is equal to the default value, make sure that any default comments are included.
				*/
				if (my realValue == Melder_atof (my stringDefaultValue)) {
					GuiText_setString (my text, my stringDefaultValue);
				} else {
					char32 clean [40];
					str32cpy (clean, Melder_double (my realValue));
					/*
						If the default value is overtly real (rather than integer), the shown value must be overtly real as well.
					*/
					if ((str32chr (my stringDefaultValue, U'.') || str32chr (my stringDefaultValue, U'e')) &&
						! (str32chr (clean, U'.') || str32chr (clean, U'e')))
					{
						str32cpy (clean + str32len (clean), U".0");
					}
					GuiText_setString (my text, clean);
				}
			#endif
			if (isundef (my realValue) && my type != _kUiField_type::REAL_OR_UNDEFINED_)
				Melder_throw (U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" has the value \"undefined\".");
			if (my type == _kUiField_type::POSITIVE_ && my realValue <= 0.0)
				Melder_throw (U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" should be greater than 0.0.");
			if (my realVariable) *my realVariable = my realValue;
		}
		break;
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_:
		{
			autostring32 dirty = GuiText_getString (my text);
			if (my type == _kUiField_type::CHANNEL_ && (str32equ (dirty.peek(), U"Left") || str32equ (dirty.peek(), U"Mono"))) {
				my integerValue = 1;
			} else if (my type == _kUiField_type::CHANNEL_ && (str32equ (dirty.peek(), U"Right") || str32equ (dirty.peek(), U"Stereo"))) {
				my integerValue = 2;
			} else {
				double realValue;
				Interpreter_numericExpression (nullptr, dirty.peek(), & realValue);
				my integerValue = Melder_iround (realValue);
			}
			#if EVALUATE_WIDGET_REPRESENTATIONS
				if (my integerValue == Melder_atoi (my stringDefaultValue)) {
					GuiText_setString (my text, my stringDefaultValue);
				} else {
					GuiText_setString (my text, Melder_integer (my integerValue));
				}
			#endif
			if (my type == _kUiField_type::NATURAL_ && my integerValue < 1) {
				Melder_throw (U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" should be a positive whole number.");
			}
			if (my type == _kUiField_type::CHANNEL_ && my integerValue < 0) {
				Melder_throw (U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" should be a positive whole number or zero.");
			}
			if (my integerVariable) *my integerVariable = my integerValue;
		}
		break;
		case _kUiField_type::WORD_:
		{
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
			#if EVALUATE_WIDGET_REPRESENTATIONS
				char32 *p = my stringValue;
				while (*p != '\0') { if (*p == U' ' || *p == U'\t') *p = U'\0'; p ++; }
				GuiText_setString (my text, my stringValue);
			#else
				if (str32chr (my stringValue, U' ') || str32chr (my stringValue, U'\t')) {
					Melder_throw (U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" should be a single word and cannot contain a space.");
				}
			#endif
			if (my stringVariable) *my stringVariable = my stringValue;
		}
		break;
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		{
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
			if (my stringVariable) *my stringVariable = my stringValue;
		}
		break;
		case _kUiField_type::NUMVEC_:
		{
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
			if (my numericVectorVariable) {
				if (my owned) my numericVectorVariable -> reset();
				Interpreter_numericVectorExpression (nullptr, my stringValue, my numericVectorVariable, & my owned);
			}
		}
		break;
		case _kUiField_type::NUMMAT_:
		{
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
			if (my numericMatrixVariable) {
				if (my owned) my numericMatrixVariable -> reset();
				Interpreter_numericMatrixExpression (nullptr, my stringValue, my numericMatrixVariable, & my owned);
			}
		}
		break;
		case _kUiField_type::BOOLEAN_:
		{
			my integerValue = GuiCheckButton_getValue (my checkButton);
			if (my boolVariable) *my boolVariable = my integerValue;
		}
		break;
		case _kUiField_type::RADIO_:
		{
			my integerValue = 0;
			for (int i = 1; i <= my options.size; i ++) {
				UiOption b = my options.at [i];
				if (GuiRadioButton_getValue (b -> radioButton))
					my integerValue = i;
			}
			if (my integerValue == 0)
				Melder_throw (U"No option chosen for " U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U".");
			if (my intVariable) *my intVariable = my integerValue - my subtract;
			if (my stringVariable) *my stringVariable = my options.at [my integerValue] -> name;
		}
		break;
		case _kUiField_type::OPTIONMENU_:
		{
			my integerValue = GuiOptionMenu_getValue (my optionMenu);
			if (my integerValue == 0)
				Melder_throw (U"No option chosen for " U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U".");
			if (my intVariable) *my intVariable = my integerValue - my subtract;
			if (my stringVariable) *my stringVariable = my options.at [my integerValue] -> name;
		}
		break;
		case _kUiField_type::LIST_:
		{
			integer numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);   // BUG memory
			if (! selected) {
				Melder_warning (U"No items selected.");
				my integerValue = 1;
			} else {
				if (numberOfSelected > 1) Melder_warning (U"More than one item selected.");
				my integerValue = selected [1];
				NUMvector_free <integer> (selected, 1);
			}
			if (my integerVariable) *my integerVariable = my integerValue;
			if (my stringVariable) *my stringVariable = (char32 *) my strings [my integerValue];
		}
		break;
		case _kUiField_type::COLOUR_:
		{
			autostring32 string = GuiText_getString (my text);
			if (colourToValue (me, string.peek())) {
				// do nothing
			} else {
				Interpreter_numericExpression (nullptr, string.peek(), & my colourValue. red);
				my colourValue. green = my colourValue. blue = my colourValue. red;
			}
			if (my colourVariable) *my colourVariable = my colourValue;
		}
	}
}

/***** History mechanism. *****/

static MelderString theHistory { };
void UiHistory_write (const char32 *string) { MelderString_append (& theHistory, string); }
void UiHistory_write_expandQuotes (const char32 *string) {
	if (! string) return;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (*p == U'\"') MelderString_append (& theHistory, U"\"\""); else MelderString_appendCharacter (& theHistory, *p);
	}
}
void UiHistory_write_colonize (const char32 *string) {
	if (! string) return;
	for (const char32 *p = & string [0]; *p != U'\0'; p ++) {
		if (*p == U'.' && p [1] == U'.' && p [2] == U'.') {
			MelderString_append (& theHistory, U":");
			p += 2;
		} else {
			MelderString_appendCharacter (& theHistory, *p);
		}
	}
}
char32 *UiHistory_get () { return theHistory.string; }
void UiHistory_clear () { MelderString_empty (& theHistory); }

/***** class UiForm: dialog windows *****/

Thing_implement (UiForm, Thing, 0);

bool (*theAllowExecutionHookHint) (void *closure) = nullptr;
void *theAllowExecutionClosureHint = nullptr;

void Ui_setAllowExecutionHook (bool (*allowExecutionHook) (void *closure), void *allowExecutionClosure) {
	theAllowExecutionHookHint = allowExecutionHook;
	theAllowExecutionClosureHint = allowExecutionClosure;
}

void structUiForm :: v_destroy () noexcept {
	for (int ifield = 1; ifield <= our numberOfFields; ifield ++)
		forget (our field [ifield]);
	if (our d_dialogForm) {
		trace (U"invoking button title ", our invokingButtonTitle);
		GuiObject_destroy (our d_dialogForm -> d_widget);   // BUG: make sure this destroys the shell
	}
	Melder_free (our invokingButtonTitle);
	Melder_free (our helpTitle);
	our UiForm_Parent :: v_destroy ();
}

static void gui_button_cb_revert (UiForm me, GuiButtonEvent /* event */) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
}

static void gui_dialog_cb_close (UiForm me) {
	if (my cancelCallback) my cancelCallback (me, my buttonClosure);
	GuiThing_hide (my d_dialogForm);
	if (my destroyWhenUnmanaged) forget (me);
}
static void gui_button_cb_cancel (UiForm me, GuiButtonEvent /* event */) {
	if (my cancelCallback) my cancelCallback (me, my buttonClosure);
	GuiThing_hide (my d_dialogForm);
	if (my destroyWhenUnmanaged) forget (me);
}

static void UiForm_okOrApply (UiForm me, GuiButton button, int hide) {
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_flushError (U"Cannot execute command window " U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U".");
		return;
	}
	try {
		for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
			UiField_widgetToValue (my field [ifield]);
	} catch (MelderError) {
		Melder_flushError (U"Please correct command window " U_LEFT_DOUBLE_QUOTE, my name, U_RIGHT_DOUBLE_QUOTE U" or cancel.");
		return;
	}
	if (my okButton) GuiThing_setSensitive (my okButton, false);
	for (int i = 1; i <= my numberOfContinueButtons; i ++)
		if (my continueButtons [i])
			GuiThing_setSensitive (my continueButtons [i], false);
	if (my applyButton)  GuiThing_setSensitive (my applyButton,  false);
	if (my cancelButton) GuiThing_setSensitive (my cancelButton, false);
	if (my revertButton) GuiThing_setSensitive (my revertButton, false);
	if (my helpButton)   GuiThing_setSensitive (my helpButton,   false);
	#if defined (_WIN32)
		GdiFlush ();
	#endif
	if (my isPauseForm) {
		for (int i = 1; i <= my numberOfContinueButtons; i ++) {
			if (button == my continueButtons [i]) {
				my clickedContinueButton = i;
			}
		}
	}
	/*
		Keep the gate for error handling.
	*/
	try {
		my okCallback (me, 0, nullptr, nullptr, nullptr, nullptr, false, my buttonClosure);
		/*
			Write everything to history. Before destruction!
		*/
		if (! my isPauseForm) {
			UiHistory_write (U"\n");
			UiHistory_write_colonize (my invokingButtonTitle);
			int size = my numberOfFields;
			while (size >= 1 && my field [size] -> type == _kUiField_type::LABEL_)
				size --;   // ignore trailing fields without a value
			int next = 0;
			for (int ifield = 1; ifield <= size; ifield ++) {
				UiField field = my field [ifield];
				switch (field -> type)
				{
					case _kUiField_type::REAL_:
					case _kUiField_type::REAL_OR_UNDEFINED_:
					case _kUiField_type::POSITIVE_:
					{
						UiHistory_write (next -- ? U", " : U" ");
						UiHistory_write (Melder_double (field -> realValue));
					}
					break;
					case _kUiField_type::INTEGER_:
					case _kUiField_type::NATURAL_:
					case _kUiField_type::CHANNEL_:
					{
						UiHistory_write (next -- ? U", " : U" ");
						UiHistory_write (Melder_integer (field -> integerValue));
					}
					break;
					case _kUiField_type::WORD_:
					case _kUiField_type::SENTENCE_:
					case _kUiField_type::TEXT_:
					{
						UiHistory_write (next -- ? U", \"" : U" \"");
						UiHistory_write_expandQuotes (field -> stringValue);
						UiHistory_write (U"\"");
					}
					break;
					case _kUiField_type::BOOLEAN_:
					{
						UiHistory_write (field -> integerValue ? (next -- ? U", \"yes\"" : U" \"yes\"") : (next -- ? U", \"no\"" : U" \"no\""));
					}
					break;
					case _kUiField_type::RADIO_:
					case _kUiField_type::OPTIONMENU_:
					{
						UiOption b = field -> options.at [field -> integerValue];
						UiHistory_write (next -- ? U", \"" : U" \"");
						UiHistory_write_expandQuotes (b -> name);
						UiHistory_write (U"\"");
					}
					break;
					case _kUiField_type::LIST_:
					{
						UiHistory_write (next -- ? U", \"" : U" \"");
						UiHistory_write_expandQuotes (field -> strings [field -> integerValue]);
						UiHistory_write (U"\"");
					}
					break;
					case _kUiField_type::COLOUR_:
					{
						UiHistory_write (next -- ? U", \"" : U" \"");
						UiHistory_write (Graphics_Colour_name (field -> colourValue));
						UiHistory_write (U"\"");
					}
				}
			}
		}
		if (hide) {
			GuiThing_hide (my d_dialogForm);
			if (my destroyWhenUnmanaged) {
				forget (me);
				return;
			}
		}
	} catch (MelderError) {
		/*
			If a solution has already been suggested, or the "error" was actually a conscious user action, do not add anything more.
		*/
		if (! str32str (Melder_getError (), U"Please ") && ! str32str (Melder_getError (), U"You could ") &&
			! str32str (Melder_getError (), U"You interrupted ") && ! str32str (Melder_getError (), U"Interrupted!"))
		{
			/*
				Otherwise, show a generic message.
			*/
			if (str32str (Melder_getError (), U"Selection changed!")) {
				Melder_appendError (U"Please change the selection in the object list, or click Cancel in the command window " U_LEFT_DOUBLE_QUOTE,
					my name, U_RIGHT_DOUBLE_QUOTE U".");
			} else {
				Melder_appendError (U"Please change something in the command window " U_LEFT_DOUBLE_QUOTE,
					my name, U_RIGHT_DOUBLE_QUOTE U", or click Cancel in that window.");
			}
		}
		Melder_flushError ();
	}
	if (my okButton) GuiThing_setSensitive (my okButton, true);
	for (int i = 1; i <= my numberOfContinueButtons; i ++)
		if (my continueButtons [i])
			GuiThing_setSensitive (my continueButtons [i], true);
	if (my applyButton)  GuiThing_setSensitive (my applyButton,  true);
	if (my cancelButton) GuiThing_setSensitive (my cancelButton, true);
	if (my revertButton) GuiThing_setSensitive (my revertButton, true);
	if (my helpButton)   GuiThing_setSensitive (my helpButton,   true);
}

static void gui_button_cb_ok (UiForm me, GuiButtonEvent event) {
	UiForm_okOrApply (me, event -> button, true);
}

static void gui_button_cb_apply (UiForm me, GuiButtonEvent event) {
	UiForm_okOrApply (me, event -> button, false);
}

static void gui_button_cb_help (UiForm me, GuiButtonEvent /* event */) {
	Melder_help (my helpTitle);
}

UiForm UiForm_create (GuiWindow parent, const char32 *title,
	UiCallback okCallback, void *buttonClosure,
	const char32 *invokingButtonTitle, const char32 *helpTitle)
{
	autoUiForm me = Thing_new (UiForm);
	my d_dialogParent = parent;
	Thing_setName (me.get(), title);
	my okCallback = okCallback;
	my buttonClosure = buttonClosure;
	my invokingButtonTitle = Melder_dup (invokingButtonTitle);
	my helpTitle = Melder_dup (helpTitle);
	return me.releaseToAmbiguousOwner();
}

void UiForm_setPauseForm (UiForm me,
	int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const char32 *continue1, const char32 *continue2, const char32 *continue3,
	const char32 *continue4, const char32 *continue5, const char32 *continue6,
	const char32 *continue7, const char32 *continue8, const char32 *continue9,
	const char32 *continue10,
	void (*cancelCallback) (UiForm dia, void *closure))
{
	my isPauseForm = true;
	my numberOfContinueButtons = numberOfContinueButtons;
	my defaultContinueButton = defaultContinueButton;
	my cancelContinueButton = cancelContinueButton;
	my continueTexts [1] = continue1;
	my continueTexts [2] = continue2;
	my continueTexts [3] = continue3;
	my continueTexts [4] = continue4;
	my continueTexts [5] = continue5;
	my continueTexts [6] = continue6;
	my continueTexts [7] = continue7;
	my continueTexts [8] = continue8;
	my continueTexts [9] = continue9;
	my continueTexts [10] = continue10;
	my cancelCallback = cancelCallback;
}

static void commonOkCallback (UiForm /* dia */, integer /* narg */, Stackel /* args */, const char32 * /* sendingString */,
	Interpreter interpreter, const char32 * /* invokingButtonTitle */, bool /* modified */, void *closure)
{
	EditorCommand cmd = (EditorCommand) closure;
	cmd -> commandCallback (cmd -> d_editor, cmd, cmd -> d_uiform.get(), 0, nullptr, nullptr, interpreter);
}

autoUiForm UiForm_createE (EditorCommand cmd, const char32 *title, const char32 *invokingButtonTitle, const char32 *helpTitle) {
	Editor editor = cmd -> d_editor;
	autoUiForm dia (UiForm_create (editor -> windowForm, title, commonOkCallback, cmd, invokingButtonTitle, helpTitle));
	dia -> command = cmd;
	return dia;
}

static UiField UiForm_addField (UiForm me, _kUiField_type type, const char32 *label) {
	if (my numberOfFields == MAXIMUM_NUMBER_OF_FIELDS)
		Melder_throw (U"Cannot have more than ", MAXIMUM_NUMBER_OF_FIELDS, U"fields in a form.");
	return my field [++ my numberOfFields] = UiField_create (type, label);
}

UiField UiForm_addReal (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::REAL_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy realVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addRealOrUndefined (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::REAL_OR_UNDEFINED_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy realVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addPositive (UiForm me, double *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::POSITIVE_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy realVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addInteger (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::INTEGER_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy integerVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addNatural (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::NATURAL_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy integerVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addWord (UiForm me, char32 **variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::WORD_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy stringVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addSentence (UiForm me, char32 **variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::SENTENCE_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy stringVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addLabel (UiForm me, char32 **variable, const char32 *label) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::LABEL_, U""));   // this field gets no name; so that the user can give it any title
	thy stringVariable = variable;
	thy stringValue = Melder_dup (label);
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addBoolean (UiForm me, bool *variable, const char32 *variableName, const char32 *label, bool defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::BOOLEAN_, label));
	thy integerDefaultValue = defaultValue;
	thy boolVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addText (UiForm me, char32 **variable, const char32 *variableName, const char32 *name, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::TEXT_, name));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy stringVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addNumvec (UiForm me, numvec *variable, const char32 *variableName, const char32 *name, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::NUMVEC_, name));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy numericVectorVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addNummat (UiForm me, nummat *variable, const char32 *variableName, const char32 *name, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::NUMMAT_, name));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy numericMatrixVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addRadio (UiForm me, int *intVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, int defaultValue, int base) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::RADIO_, label));
	thy integerDefaultValue = defaultValue;
	thy intVariable = intVariable;
	thy stringVariable = stringVariable;
	thy variableName = variableName;
	thy subtract = ( base == 1 ? 0 : 1 );
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addOptionMenu (UiForm me, int *intVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, int defaultValue, int base) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::OPTIONMENU_, label));
	thy integerDefaultValue = defaultValue;
	thy intVariable = intVariable;
	thy stringVariable = stringVariable;
	thy variableName = variableName;
	thy subtract = ( base == 1 ? 0 : 1 );
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addList (UiForm me, integer *integerVariable, char32 **stringVariable, const char32 *variableName, const char32 *label, integer numberOfStrings, const char32 **strings, integer defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::LIST_, label));
	thy numberOfStrings = numberOfStrings;
	thy strings = strings;
	thy integerDefaultValue = defaultValue;
	thy integerVariable = integerVariable;
	thy stringVariable = stringVariable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addColour (UiForm me, Graphics_Colour *colourVariable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::COLOUR_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy colourVariable = colourVariable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

UiField UiForm_addChannel (UiForm me, integer *variable, const char32 *variableName, const char32 *label, const char32 *defaultValue) {
	autoUiField thee (UiForm_addField (me, _kUiField_type::CHANNEL_, label));
	thy stringDefaultValue = Melder_dup (defaultValue);
	thy integerVariable = variable;
	thy variableName = variableName;
	return thee.releaseToAmbiguousOwner();
}

#define DIALOG_X  150
#define DIALOG_Y  70
#define HELP_BUTTON_WIDTH  60
#define STANDARDS_BUTTON_WIDTH  100
#define REVERT_BUTTON_WIDTH  60
#define STOP_BUTTON_WIDTH  50
#define HELP_BUTTON_X  20
#define LIST_HEIGHT  192

static MelderString theFinishBuffer { };

static void appendColon () {
	integer length = theFinishBuffer.length;
	if (length < 1) return;
	char32 lastCharacter = theFinishBuffer.string [length - 1];
	if (lastCharacter == U':' || lastCharacter == U'?' || lastCharacter == U'.') return;
	MelderString_appendCharacter (& theFinishBuffer, U':');
}

void UiForm_finish (UiForm me) {
	if (! my d_dialogParent && ! my isPauseForm) return;

	int size = my numberOfFields;
	int dialogHeight = 0, x = Gui_LEFT_DIALOG_SPACING, y;
	int textFieldHeight = Gui_TEXTFIELD_HEIGHT;
	int dialogWidth = 520, dialogCentre = dialogWidth / 2, fieldX = dialogCentre + Gui_LABEL_SPACING / 2;
	int labelWidth = fieldX - Gui_LABEL_SPACING - x, fieldWidth = labelWidth, halfFieldWidth = fieldWidth / 2 - 6;

	GuiForm form;

	/*
		Compute height. Cannot leave this to the default geometry management system.
	*/
	for (integer ifield = 1; ifield <= my numberOfFields; ifield ++ ) {
		UiField thee = my field [ifield], previous = my field [ifield - 1];
		dialogHeight +=
			ifield == 1 ? Gui_TOP_DIALOG_SPACING :
			thy type == _kUiField_type::RADIO_ || previous -> type == _kUiField_type::RADIO_ ? Gui_VERTICAL_DIALOG_SPACING_DIFFERENT :
			thy type >= _kUiField_type::LABELLED_TEXT_MIN_ && thy type <= _kUiField_type::LABELLED_TEXT_MAX_ && str32nequ (thy name, U"right ", 6) &&
			previous -> type >= _kUiField_type::LABELLED_TEXT_MIN_ && previous -> type <= _kUiField_type::LABELLED_TEXT_MAX_ &&
			str32nequ (previous -> name, U"left ", 5) ? - textFieldHeight : Gui_VERTICAL_DIALOG_SPACING_SAME;
		thy y = dialogHeight;
		dialogHeight +=
			thy type == _kUiField_type::BOOLEAN_ ? Gui_CHECKBUTTON_HEIGHT :
			thy type == _kUiField_type::RADIO_ ? thy options.size * Gui_RADIOBUTTON_HEIGHT +
				(thy options.size - 1) * Gui_RADIOBUTTON_SPACING :
			thy type == _kUiField_type::OPTIONMENU_ ? Gui_OPTIONMENU_HEIGHT :
			thy type == _kUiField_type::LIST_ ? LIST_HEIGHT :
			thy type == _kUiField_type::LABEL_ && thy stringValue [0] != U'\0' && thy stringValue [str32len (thy stringValue) - 1] != U'.' &&
				ifield != my numberOfFields ? textFieldHeight
				#ifdef _WIN32
					- 6 :
				#else
					- 10 :
				#endif
			textFieldHeight;
	}
	dialogHeight += 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT;
	my d_dialogForm = GuiDialog_create (my d_dialogParent, DIALOG_X, DIALOG_Y, dialogWidth, dialogHeight, my name, gui_dialog_cb_close, me, 0);

	form = my d_dialogForm;

	for (integer ifield = 1; ifield <= size; ifield ++) {
		UiField field = my field [ifield];
		y = field -> y;
		switch (field -> type)
		{
			case _kUiField_type::REAL_:
			case _kUiField_type::REAL_OR_UNDEFINED_:
			case _kUiField_type::POSITIVE_:
			case _kUiField_type::INTEGER_:
			case _kUiField_type::NATURAL_:
			case _kUiField_type::WORD_:
			case _kUiField_type::SENTENCE_:
			case _kUiField_type::COLOUR_:
			case _kUiField_type::CHANNEL_:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 3;
				#endif
				if (str32nequ (field -> name, U"left ", 5)) {
					MelderString_copy (& theFinishBuffer, field -> formLabel + 5);
					appendColon ();
					field -> label = GuiLabel_createShown (form, 0, x + labelWidth, ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					field -> text = GuiText_createShown (form, fieldX, fieldX + halfFieldWidth, y, y + Gui_TEXTFIELD_HEIGHT, 0);
				} else if (str32nequ (field -> name, U"right ", 6)) {
					field -> text = GuiText_createShown (form, fieldX + halfFieldWidth + 12, fieldX + fieldWidth,
						y, y + Gui_TEXTFIELD_HEIGHT, 0);
				} else {
					MelderString_copy (& theFinishBuffer, field -> formLabel);
					appendColon ();
					field -> label = GuiLabel_createShown (form, 0, x + labelWidth,
						ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					field -> text = GuiText_createShown (form, fieldX, fieldX + fieldWidth, // or once the dialog is a Form: - Gui_RIGHT_DIALOG_SPACING,
						y, y + Gui_TEXTFIELD_HEIGHT, 0);
				}
			}
			break;
			case _kUiField_type::TEXT_:
			case _kUiField_type::NUMVEC_:
			case _kUiField_type::NUMMAT_:
			{
				field -> text = GuiText_createShown (form, x, x + dialogWidth - Gui_LEFT_DIALOG_SPACING - Gui_RIGHT_DIALOG_SPACING,
					y, y + Gui_TEXTFIELD_HEIGHT, 0);
			}
			break;
			case _kUiField_type::LABEL_:
			{
				MelderString_copy (& theFinishBuffer, field -> stringValue);
				field -> label = GuiLabel_createShown (form,
					x, dialogWidth /* allow to extend into the margin */, y + 5, y + 5 + textFieldHeight,
					theFinishBuffer.string, 0);
			}
			break;
			case _kUiField_type::RADIO_:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 1;
				#endif
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				field -> label = GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_RADIOBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);
				GuiRadioGroup_begin ();
				for (integer ibutton = 1; ibutton <= field -> options.size; ibutton ++) {
					UiOption button = field -> options.at [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					button -> radioButton = GuiRadioButton_createShown (form,
						fieldX, dialogWidth /* allow to extend into the margin */,
						y + (ibutton - 1) * (Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING),
						y + (ibutton - 1) * (Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING) + Gui_RADIOBUTTON_HEIGHT,
						theFinishBuffer.string, nullptr, nullptr, 0);
				}
				GuiRadioGroup_end ();
			}
			break;
			case _kUiField_type::OPTIONMENU_:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 2;
				#endif
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				field -> label = GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_OPTIONMENU_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);
				field -> optionMenu = GuiOptionMenu_createShown (form, fieldX, fieldX + fieldWidth, y, y + Gui_OPTIONMENU_HEIGHT, 0);
				for (integer ibutton = 1; ibutton <= field -> options.size; ibutton ++) {
					UiOption button = field -> options.at [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					GuiOptionMenu_addOption (field -> optionMenu, theFinishBuffer.string);
				}
			}
			break;
			case _kUiField_type::BOOLEAN_:
			{
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				/*field -> label = GuiLabel_createShown (form, x, x + labelWidth, y, y + Gui_CHECKBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT); */
				field -> checkButton = GuiCheckButton_createShown (form,
					fieldX, dialogWidth /* allow to extend into the margin */, y, y + Gui_CHECKBUTTON_HEIGHT,
					theFinishBuffer.string, nullptr, nullptr, 0);
			}
			break;
			case _kUiField_type::LIST_:
			{
				int listWidth = my numberOfFields == 1 ? dialogWidth - fieldX : fieldWidth;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				field -> label = GuiLabel_createShown (form, x, x + labelWidth, y + 1, y + 21,
					theFinishBuffer.string, GuiLabel_RIGHT);
				field -> list = GuiList_create (form, fieldX, fieldX + listWidth, y, y + LIST_HEIGHT, false, theFinishBuffer.string);
				for (integer i = 1; i <= field -> numberOfStrings; i ++) {
					GuiList_insertItem (field -> list, field -> strings [i], 0);
				}
				GuiThing_show (field -> list);
			}
			break;
		}
	}
	for (integer ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
	/*separator = XmCreateSeparatorGadget (column, "separator", nullptr, 0);*/
	y = dialogHeight - Gui_BOTTOM_DIALOG_SPACING - Gui_PUSHBUTTON_HEIGHT;
	if (my helpTitle) {
		my helpButton = GuiButton_createShown (form, HELP_BUTTON_X, HELP_BUTTON_X + HELP_BUTTON_WIDTH, y, y + Gui_PUSHBUTTON_HEIGHT,
			U"Help", gui_button_cb_help, me, 0);
	}
	bool commentsOnly = true;
	for (integer ifield = 1; ifield <= my numberOfFields; ifield ++) {
		if (my field [ifield] -> type != _kUiField_type::LABEL_) {
			commentsOnly = false;
			break;
		}
	}
	if (! commentsOnly) {
		if (my isPauseForm) {
			my revertButton = GuiButton_createShown (form,
				HELP_BUTTON_X, HELP_BUTTON_X + REVERT_BUTTON_WIDTH,
				y, y + Gui_PUSHBUTTON_HEIGHT, U"Revert", gui_button_cb_revert, me, 0);
		} else {
			my revertButton = GuiButton_createShown (form,
				HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING,
				HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING + STANDARDS_BUTTON_WIDTH,
				y, y + Gui_PUSHBUTTON_HEIGHT, U"Standards", gui_button_cb_revert, me, 0);
		}
	}
	if (my isPauseForm) {
		x = HELP_BUTTON_X + REVERT_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING;
		if (my cancelContinueButton == 0) {
			my cancelButton = GuiButton_createShown (form, x, x + STOP_BUTTON_WIDTH, y, y + Gui_PUSHBUTTON_HEIGHT,
				U"Stop", gui_button_cb_cancel, me, GuiButton_CANCEL);
			x += STOP_BUTTON_WIDTH + 7;
		} else {
			x += 30;
		}
		int room = dialogWidth - Gui_RIGHT_DIALOG_SPACING - x;
		int roomPerContinueButton = room / my numberOfContinueButtons;
		int horizontalSpacing = my numberOfContinueButtons > 7 ? Gui_HORIZONTAL_DIALOG_SPACING - 2 * (my numberOfContinueButtons - 7) : Gui_HORIZONTAL_DIALOG_SPACING;
		int continueButtonWidth = roomPerContinueButton - horizontalSpacing;
		for (int i = 1; i <= my numberOfContinueButtons; i ++) {
			x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - roomPerContinueButton * (my numberOfContinueButtons - i + 1) + horizontalSpacing;
			my continueButtons [i] = GuiButton_createShown (form, x, x + continueButtonWidth, y, y + Gui_PUSHBUTTON_HEIGHT,
				my continueTexts [i], gui_button_cb_ok, me, i == my defaultContinueButton ? GuiButton_DEFAULT : 0);
		}
	} else {
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - 2 * Gui_HORIZONTAL_DIALOG_SPACING
			 - Gui_APPLY_BUTTON_WIDTH - Gui_CANCEL_BUTTON_WIDTH;
		my cancelButton = GuiButton_createShown (form, x, x + Gui_CANCEL_BUTTON_WIDTH, y, y + Gui_PUSHBUTTON_HEIGHT,
			U"Cancel", gui_button_cb_cancel, me, GuiButton_CANCEL);
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - Gui_HORIZONTAL_DIALOG_SPACING - Gui_APPLY_BUTTON_WIDTH;
		if (my numberOfFields > 1 || my field [1] -> type != _kUiField_type::LABEL_) {
			my applyButton = GuiButton_createShown (form, x, x + Gui_APPLY_BUTTON_WIDTH, y, y + Gui_PUSHBUTTON_HEIGHT,
				U"Apply", gui_button_cb_apply, me, 0);
		}
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH;
		my okButton = GuiButton_createShown (form, x, x + Gui_OK_BUTTON_WIDTH, y, y + Gui_PUSHBUTTON_HEIGHT,
			my isPauseForm ? U"Continue" : U"OK", gui_button_cb_ok, me, GuiButton_DEFAULT);
	}
	/*GuiObject_show (separator);*/
}

void UiForm_destroyWhenUnmanaged (UiForm me) {
	my destroyWhenUnmanaged = true;
}

void UiForm_do (UiForm me, bool modified) {
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	Melder_assert (my d_dialogForm);
	GuiThing_show (my d_dialogForm);
	if (modified)
		UiForm_okOrApply (me, nullptr, true);
}

static void UiField_api_header_C (UiField me, UiField next, bool isLastNonLabelField) {
	if (my type == _kUiField_type::LABEL_) {
		bool weAreFollowedByAWideField =
			next && (next -> type == _kUiField_type::TEXT_ || next -> type == _kUiField_type::NUMVEC_ || next -> type == _kUiField_type::NUMMAT_);
		bool weLabelTheFollowingField =
			weAreFollowedByAWideField &&
			Melder_stringMatchesCriterion (my stringValue, kMelder_string::ENDS_WITH, U":");
		bool weAreAComment = ! weLabelTheFollowingField;
		if (weAreAComment) {
			MelderInfo_writeLine (U"\t/* ", my stringValue, U" */");
		}
		return;
	}

	/*
		Write the type of the field.
	*/
	bool isText = false, isBoolean = false, isEnum = false, isPositive = false;
	switch (my type)
	{
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		{
			MelderInfo_write (U"\tdouble ");
			isPositive = ( my type == _kUiField_type::POSITIVE_);
		}
		break;
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_:
		{
			MelderInfo_write (U"\tint64_t ");
			isPositive = ( my type == _kUiField_type::NATURAL_);
		}
		break;
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		case _kUiField_type::COLOUR_:
		case _kUiField_type::LIST_:
		{
			MelderInfo_write (U"\tconst char *");
			isText = true;
		}
		break;
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		{
			MelderInfo_write (U"\tconst char *");
			isText = true;
			isEnum = true;
		}
		break;
		case _kUiField_type::BOOLEAN_:
		{
			MelderInfo_write (U"\tint32_t ");
			isBoolean = true;
		}
		break;
		default:
		{
		}
	}

	/*
		Write the title of the field.
	*/
	char32 cName [100], *q = & cName [0];
	Melder_assert (my formLabel);
	const char32 *p = & my formLabel [0];
	*q ++ = tolower32 (*p ++);
	bool up = false;
	for (; *p != U'\0'; p ++) {
		if (*p == U'(') {
			break;
		} else if (*p == U'\'') {
			continue;
		} else if (*p == U' ' || *p == U'-') {
			if (p [1] == U'(') { p ++; break; }
			up = true;
		} else if (*p == U'*') {
			*q ++ = U'S';
			*q ++ = U't';
			*q ++ = U'a';
			*q ++ = U'r';
		} else if (up) {
			*q ++ = toupper32 (*p);
			up = false;
		} else {
			*q ++ = *p;
		}
	}
	*q = U'\0';
	if (! my variableName)
		Melder_warning (U"Missing variable name for field label: ", my formLabel);
	MelderInfo_write (my variableName ? my variableName : cName);
	if (! isLastNonLabelField) MelderInfo_write (U",");

	/*
		Get the units.
	*/
	char32 units [100];
	q = & units [0];
	if (*p == U'(') {
		for (p ++; *p != U'\0'; p ++) {
			if (*p == U')') {
				break;
			} else {
				*q ++ = *p;
			}
		}
	}
	*q = U'\0';
	bool unitsAreAvailable = ( units [0] != U'\0' );
	bool unitsContainRange = str32str (units, U"-");

	/*
		Get the example.
	*/
	const char32 *example = my stringDefaultValue;
	bool exampleIsAvailable = ( example && example [0] != U'\0' );

	if (exampleIsAvailable) {
		/*
			Split up the default string.
		*/
		char32 defaultValue [100], defaultComment [100];
		str32cpy (defaultValue, my stringDefaultValue);
		str32cpy (defaultComment, U"");
		if (unitsAreAvailable) {
			char32 *parenthesis = str32chr (defaultValue, U'(');
			if (parenthesis && parenthesis - defaultValue > 1) {
				parenthesis [-1] = U'\0';
				str32cpy (defaultComment, parenthesis);
			}
		}

		MelderInfo_write (U"   // ");
		if (isPositive) {
			MelderInfo_write (U"positive, ");
		}
		if (unitsContainRange) {
			MelderInfo_write (units, U", ");
		}
		MelderInfo_write (U"e.g. ");
		if (isText) MelderInfo_write (U"\"");
		MelderInfo_write (defaultValue);
		if (isText) MelderInfo_write (U"\"");
		if (unitsAreAvailable && ! unitsContainRange) {
			MelderInfo_write (U" ", units);
		}
		if (defaultComment [0]) {
			MelderInfo_write (U" ", defaultComment);
		}
	} else if (isBoolean) {
		MelderInfo_write (U"   // boolean, e.g. ");
		MelderInfo_write (my integerDefaultValue, my integerDefaultValue ? U" (true)" : U" (false)");
	} else if (isEnum) {
		MelderInfo_write (U"   // e.g. \"");
		MelderInfo_write (my options.at [my integerDefaultValue] -> name);
		MelderInfo_write (U"\"; other choice", ( my options.size > 2 ? U"s" : U"" ), U":");
		bool firstWritten = false;
		for (int i = 1; i <= my options.size; i ++) {
			if (i == my integerDefaultValue) continue;
			if (firstWritten) MelderInfo_write (U",");
			MelderInfo_write (U" \"", my options.at [i] -> name, U"\"");
			firstWritten = true;
		}
	}
	MelderInfo_writeLine (U"");
}

void UiForm_info (UiForm me, integer narg) {
	if (narg == -1) {
		/*
			The C interface.
		*/
		int lastNonLabelFieldNumber = 0;
		for (int ifield = my numberOfFields; ifield > 0; ifield --) {
			if (my field [ifield] -> type != _kUiField_type::LABEL_) {
				lastNonLabelFieldNumber = ifield;
				break;
			}
		}
		for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
			UiField_api_header_C (my field [ifield], ifield == my numberOfFields ? nullptr : my field [ifield + 1], ifield == lastNonLabelFieldNumber);
		}
	}
}

static void UiField_argToValue (UiField me, Stackel arg, Interpreter /* interpreter */) {
	switch (my type)
	{
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		{
			if (arg -> which != Stackel_NUMBER)
				Melder_throw (U"Argument \"", my name, U"\" should be a number, not ", Stackel_whichText (arg), U".");
			my realValue = arg -> number;
			if (isundef (my realValue) && my type != _kUiField_type::REAL_OR_UNDEFINED_)
				Melder_throw (U"Argument \"", my name, U"\" has the value \"undefined\".");
			if (my type == _kUiField_type::POSITIVE_ && my realValue <= 0.0)
				Melder_throw (U"Argument \"", my name, U"\" must be greater than 0.");
			if (my realVariable) *my realVariable = my realValue;
		}
		break;
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_:
		{
			if (arg -> which == Stackel_STRING) {
				if (my type == _kUiField_type::CHANNEL_) {
					if (str32equ (arg -> string, U"All") || str32equ (arg -> string, U"Average")) {
						my integerValue = 0;
					} else if (str32equ (arg -> string, U"Left") || str32equ (arg -> string, U"Mono")) {
						my integerValue = 1;
					} else if (str32equ (arg -> string, U"Right") || str32equ (arg -> string, U"Stereo")) {
						my integerValue = 2;
					} else {
						Melder_throw (U"Channel argument \"", my name,
							U"\" can only be a number or one of the strings \"All\", \"Average\", \"Left\", \"Right\", \"Mono\" or \"Stereo\".");
					}
				} else {
					Melder_throw (U"Argument \"", my name, U"\" should be a number, not ", Stackel_whichText (arg), U".");
				}
			} else if (arg -> which == Stackel_NUMBER) {
				my integerValue = Melder_iround (arg -> number);
				if (my type == _kUiField_type::NATURAL_ && my integerValue < 1)
					Melder_throw (U"Argument \"", my name, U"\" should be a positive whole number.");
			} else {
				Melder_throw (U"Argument \"", my name, U"\" should be a number, not ", Stackel_whichText (arg), U".");
			}
			if (my integerVariable) *my integerVariable = my integerValue;
		}
		break;
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		{
			if (arg -> which != Stackel_STRING)
				Melder_throw (U"Argument \"", my name, U"\" should be a string, not ", Stackel_whichText (arg), U".");
			Melder_free (my stringValue);
			my stringValue = Melder_dup (arg -> string);
			if (my stringVariable) *my stringVariable = my stringValue;
		}
		break;
		case _kUiField_type::NUMVEC_:
		{
			if (arg -> which != Stackel_NUMERIC_VECTOR)
				Melder_throw (U"Argument \"", my name, U"\" should be a numeric vector, not ", Stackel_whichText (arg), U".");
			if (my numericVectorVariable) {
				if (my owned) my numericVectorVariable -> reset();
				*my numericVectorVariable = arg -> numericVector;
				my owned = arg -> owned;
			}
		}
		break;
		case _kUiField_type::NUMMAT_:
		{
			if (arg -> which != Stackel_NUMERIC_MATRIX)
				Melder_throw (U"Argument \"", my name, U"\" should be a numeric matrix, not ", Stackel_whichText (arg), U".");
			if (my numericMatrixVariable) {
				if (my owned) my numericMatrixVariable -> reset();
				*my numericMatrixVariable = arg -> numericMatrix;
				my owned = arg -> owned;
			}
		}
		break;
		case _kUiField_type::BOOLEAN_:
		{
			if (arg -> which == Stackel_STRING) {
				if (str32equ (arg -> string, U"no") || str32equ (arg -> string, U"off")) {
					my integerValue = 0;
				} else if (str32equ (arg -> string, U"yes") || str32equ (arg -> string, U"on")) {
					my integerValue = 1;
				} else {
					Melder_throw (U"Boolean argument \"", my name,
						U"\" can only be a number or one of the strings \"yes\" or \"no\".");
				}
			} else if (arg -> which == Stackel_NUMBER) {
				my integerValue = arg -> number == 0.0 ? 0.0 : 1.0;
			} else {
				Melder_throw (U"Boolean argument \"", my name, U"\" should be a number (0 or 1), not ", Stackel_whichText (arg), U".");
			}
			if (my boolVariable) *my boolVariable = my integerValue;
		}
		break;
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		{
			if (arg -> which != Stackel_STRING)
				Melder_throw (U"Option argument \"", my name, U"\" should be a string, not ", Stackel_whichText (arg), U".");
			my integerValue = 0;
			for (int i = 1; i <= my options.size; i ++) {
				UiOption b = my options.at [i];
				if (str32equ (arg -> string, b -> name))
					my integerValue = i;
			}
			if (my integerValue == 0) {
				/*
					Retry with different case.
				*/
				for (int i = 1; i <= my options.size; i ++) {
					UiOption b = my options.at [i];
					if (Melder_equ_firstCharacterCaseInsensitive (arg -> string, b -> name))
						my integerValue = i;
				}
			}
			if (my integerValue == 0) {
				if (my intVariable)
					Melder_throw (U"Option argument \"", my name, U"\" cannot have the value \"", arg -> string, U"\".");
				if (my stringVariable) {
					*my stringVariable = arg -> string;
					return;
				}
			}
			if (my intVariable) *my intVariable = my integerValue - my subtract;
			if (my stringVariable) *my stringVariable = my options.at [my integerValue] -> name;
		}
		break;
		case _kUiField_type::LIST_:
		{
			if (arg -> which != Stackel_STRING)
				Melder_throw (U"List argument \"", my name, U"\" should be a string, not ", Stackel_whichText(arg), U".");
			integer i = 1;
			for (; i <= my numberOfStrings; i ++)
				if (str32equ (arg -> string, my strings [i])) break;
			if (i > my numberOfStrings)
				Melder_throw (U"List argument \"", my name, U"\" cannot have the value \"", arg -> string, U"\".");
			my integerValue = i;
			if (my integerVariable) *my integerVariable = my integerValue;
			if (my stringVariable) *my stringVariable = (char32 *) my strings [my integerValue];
		}
		break;
		case _kUiField_type::COLOUR_:
		{
			if (arg -> which == Stackel_NUMBER) {
				if (arg -> number < 0.0 || arg -> number > 1.0)
					Melder_throw (U"Grey colour argument \"", my name, U"\" has to lie between 0.0 and 1.0.");
				my colourValue. red = my colourValue. green = my colourValue. blue = arg -> number;
			} else if (arg -> which == Stackel_STRING) {
				autostring32 string2 = Melder_dup (arg -> string);
				if (! colourToValue (me, string2.peek())) {
					Melder_throw (U"Cannot compute a colour from \"", string2.peek(), U"\".");
				}
			}
			if (my colourVariable) *my colourVariable = my colourValue;
		}
		break;
		default:
		{
			Melder_throw (U"Unknown field type ", (int) my type, U".");
		}
	}
}

void UiForm_call (UiForm me, integer narg, Stackel args, Interpreter interpreter) {
	integer size = my numberOfFields, iarg = 0;
	//while (size >= 1 && my field [size] -> type == _kUiField_type::LABEL_)
	//	size --;   // ignore trailing fields without a value
	for (integer i = 1; i <= size; i ++) {
		if (my field [i] -> type == _kUiField_type::LABEL_)
			continue;   // ignore non-trailing fields without a value
		iarg ++;
		if (iarg > narg)
			Melder_throw (U"Command requires more than the given ", narg, U" arguments: no value for argument \"", my field [i] -> name, U"\".");
		UiField_argToValue (my field [i], & args [iarg], interpreter);
	}
	if (iarg < narg)
		Melder_throw (U"Command requires only ", iarg, U" arguments, not the ", narg, U" given.");
	my okCallback (me, 0, nullptr, nullptr, interpreter, nullptr, false, my buttonClosure);
}

/*
	DEPRECATED_2014 (i.e. remove in 2036)
*/
static void UiField_stringToValue (UiField me, const char32 *string, Interpreter interpreter) {
	/*
		This belongs to the deprecated dots-based syntax described below at `UiForm_parseString`.
		This is included for backward compatibility (until 2036),
		but does not support newer expression types such as numeric vectors and matrices.
	*/
	switch (my type)
	{
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		{
			if (str32spn (string, U" \t") == str32len (string))
				Melder_throw (U"Argument “", my name, U"” empty.");
			Interpreter_numericExpression (interpreter, string, & my realValue);
			if (isundef (my realValue) && my type != _kUiField_type::REAL_OR_UNDEFINED_)
				Melder_throw (U"\"", my name, U"\" has the value \"undefined\".");
			if (my type == _kUiField_type::POSITIVE_ && my realValue <= 0.0)
				Melder_throw (U"\"", my name, U"\" must be greater than 0.");
			if (my realVariable) *my realVariable = my realValue;
		}
		break;
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_: {
			if (str32spn (string, U" \t") == str32len (string))
				Melder_throw (U"Argument “", my name, U"” empty.");
			if (my type == _kUiField_type::CHANNEL_ && (str32equ (string, U"All") || str32equ (string, U"Average"))) {
				my integerValue = 0;
			} else if (my type == _kUiField_type::CHANNEL_ && (str32equ (string, U"Left") || str32equ (string, U"Mono"))) {
				my integerValue = 1;
			} else if (my type == _kUiField_type::CHANNEL_ && (str32equ (string, U"Right") || str32equ (string, U"Stereo"))) {
				my integerValue = 2;
			} else {
				double realValue;
				Interpreter_numericExpression (interpreter, string, & realValue);
				my integerValue = Melder_iround (realValue);
			}
			if (my type == _kUiField_type::NATURAL_ && my integerValue < 1)
				Melder_throw (U"\"", my name, U"\" should be a positive whole number.");
			if (my integerVariable) *my integerVariable = my integerValue;
		}
		break;
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		{
			Melder_free (my stringValue);
			my stringValue = Melder_dup_f (string);
			if (my stringVariable) *my stringVariable = my stringValue;
		}
		break;
		case _kUiField_type::BOOLEAN_:
		{
			if (! string [0])
				Melder_throw (U"Empty argument for toggle button.");
			my integerValue = string [0] == U'1' || string [0] == U'y' || string [0] == U'Y' ||
				string [0] == U't' || string [0] == U'T';
			if (my boolVariable) *my boolVariable = my integerValue;
		}
		break;
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		{
			my integerValue = 0;
			for (int i = 1; i <= my options.size; i ++) {
				UiOption b = my options.at [i];
				if (str32equ (string, b -> name))
					my integerValue = i;
			}
			if (my integerValue == 0) {
				/*
					Retry with different case.
				*/
				for (int i = 1; i <= my options.size; i ++) {
					UiOption b = my options.at [i];
					if (Melder_equ_firstCharacterCaseInsensitive (string, b -> name))
						my integerValue = i;
				}
			}
			if (my integerValue == 0) {
				Melder_throw (U"Field \"", my name, U"\" must not have the value \"", string, U"\".");
			}
			if (my intVariable) *my intVariable = my integerValue - my subtract;
			if (my stringVariable) *my stringVariable = my options.at [my integerValue] -> name;
		}
		break;
		case _kUiField_type::LIST_:
		{
			integer i = 1;
			for (; i <= my numberOfStrings; i ++)
				if (str32equ (string, my strings [i])) break;
			if (i > my numberOfStrings)
				Melder_throw (U"Field \"", my name, U"\" must not have the value \"", string, U"\".");
			my integerValue = i;
			if (my integerVariable) *my integerVariable = my integerValue;
			if (my stringVariable) *my stringVariable = (char32 *) my strings [my integerValue];
		}
		break;
		case _kUiField_type::COLOUR_:
		{
			autostring32 string2 = Melder_dup_f (string);
			if (colourToValue (me, string2.peek())) {
				/* OK */
			} else {
				try {
					Interpreter_numericExpression (interpreter, string2.peek(), & my colourValue. red);
					my colourValue. green = my colourValue. blue = my colourValue. red;
				} catch (MelderError) {
					Melder_clearError ();
					Melder_throw (U"Cannot compute a colour from \"", string2.peek(), U"\".");
				}
			}
			if (my colourVariable) *my colourVariable = my colourValue;
		}
		break;
		default:
		{
			Melder_throw (U"Unknown field type ", (int) my type, U".");
		}
	}
}

/*
	DEPRECATED_2014 (i.e. remove in 2036)
*/
void UiForm_parseString (UiForm me, const char32 *arguments, Interpreter interpreter) {
	/*
		This implements the dots-based scripting style
			Create Sound from formula... sineWithNoise 1 0 1 44100 0.5 * sin (2*pi*377*x)
		This was deprecated with the advent of the colon-based scripting style
			Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, "0.5 * sin (2*pi*377*x)"
		or
			Create Sound from formula: "sineWithNoise", 1, 0, 1, 44100, ~ 0.5 * sin (2*pi*377*x)
		in 2014, i.e. 22 years after Praat started.
		If we want to conservatively support old scripts, we will have
		to continue to support the dots-based scripting style until 2036.
	*/
	int size = my numberOfFields;
	while (size >= 1 && my field [size] -> type == _kUiField_type::LABEL_)
		size --;   // ignore trailing fields without a value
	for (int i = 1; i < size; i ++) {
		static char32 stringValue [3000];
		int ichar = 0;
		if (my field [i] -> type == _kUiField_type::LABEL_)
			continue;   // ignore non-trailing fields without a value
		/*
			Skip spaces until next argument.
		*/
		while (*arguments == U' ' || *arguments == U'\t') arguments ++;
		/*
			The argument is everything up to the next space, or, if that starts with a double quote,
			everything between this quote and the matching double quote;
			in this case, the argument can represent a double quote by a sequence of two double quotes.
			Example: the string
				"I said ""hello"""
			will be passed to the dialog as a single argument containing the text
				I said "hello"
		*/
		if (*arguments == U'\"') {
			arguments ++;   // do not include leading double quote
			for (;;) {
				if (*arguments == U'\0')
					Melder_throw (U"Missing matching quote.");
				if (*arguments == U'\"' && * ++ arguments != U'\"') break;   // remember second quote
				stringValue [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != U' ' && *arguments != U'\t' && *arguments != U'\0')
				stringValue [ichar ++] = *arguments ++;
		}
		stringValue [ichar] = U'\0';   // trailing null character
		try {
			UiField_stringToValue (my field [i], stringValue, interpreter);
		} catch (MelderError) {
			Melder_throw (U"Don't understand contents of field \"", my field [i] -> name, U"\".");
		}
	}
	/*
		The last item is handled separately, because it consists of the rest of the line.
		Leading spaces are skipped, but trailing spaces are included.
	*/
	if (size > 0) {
		while (*arguments == U' ' || *arguments == U'\t') arguments ++;
		try {
			UiField_stringToValue (my field [size], arguments, interpreter);
		} catch (MelderError) {
			Melder_throw (U"Don't understand contents of field \"", my field [size] -> name, U"\".");
		}
	}
	my okCallback (me, 0, nullptr, nullptr, interpreter, nullptr, false, my buttonClosure);
}

void UiForm_parseStringE (EditorCommand cmd, integer narg, Stackel args, const char32 *arguments, Interpreter interpreter) {
	if (args)
		UiForm_call (cmd -> d_uiform.get(), narg, args, interpreter);
	else
		UiForm_parseString (cmd -> d_uiform.get(), arguments, interpreter);
}

static void fatalField (UiForm dia) {
	Melder_fatal (U"Wrong field in command window \"", dia -> name, U"\".");
}

void UiForm_setReal (UiForm me, double *p_variable, double value) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> realVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::REAL_:
				case _kUiField_type::REAL_OR_UNDEFINED_:
				case _kUiField_type::POSITIVE_:
				{
					if (value == Melder_atof (field -> stringDefaultValue)) {
						GuiText_setString (field -> text, field -> stringDefaultValue);
					} else {
						char32 s [40];
						str32cpy (s, Melder_double (value));
						/*
							If the default is overtly real, the shown value should be as well.
						*/
						if ((str32chr (field -> stringDefaultValue, U'.') || str32chr (field -> stringDefaultValue, U'e')) &&
							! (str32chr (s, U'.') || str32chr (s, U'e')))
						{
							str32cpy (s + str32len (s), U".0");
						}
						GuiText_setString (field -> text, s);
					}
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Real field not found in command window \"", my name, U"\".");
}

void UiForm_setRealAsString (UiForm me, double *p_variable, const char32 *stringValue) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> realVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::REAL_:
				case _kUiField_type::REAL_OR_UNDEFINED_:
				case _kUiField_type::POSITIVE_:
				{
					GuiText_setString (field -> text, stringValue);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Real field not found in command window \"", my name, U"\".");
}

void UiForm_setInteger (UiForm me, integer *p_variable, integer value) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> integerVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::INTEGER_:
				case _kUiField_type::NATURAL_:
				case _kUiField_type::CHANNEL_:
				{
					if (value == Melder_atoi (field -> stringDefaultValue)) {
						GuiText_setString (field -> text, field -> stringDefaultValue);
					} else {
						GuiText_setString (field -> text, Melder_integer (value));
					}
				}
				break;
				case _kUiField_type::LIST_:
				{
					if (value < 1 || value > field -> numberOfStrings) value = 1;   // guard against incorrect prefs file
					GuiList_selectItem (field -> list, value);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Integer field not found in command window \"", my name, U"\".");
}

void UiForm_setIntegerAsString (UiForm me, integer *p_variable, const char32 *stringValue /* cattable */) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> integerVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::INTEGER_:
				case _kUiField_type::NATURAL_:
				case _kUiField_type::CHANNEL_:
				{
					GuiText_setString (field -> text, stringValue);
				}
				break;
				case _kUiField_type::LIST_:
				{
					integer i = 1;
					for (; i <= field -> numberOfStrings; i ++)
						if (str32equ (stringValue, field -> strings [i])) break;
					if (i > field -> numberOfStrings) i = 1;   // guard against incorrect prefs file
					GuiList_selectItem (field -> list, i);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Integer field not found in command window \"", my name, U"\".");
}

void UiForm_setBoolean (UiForm me, bool *p_variable, bool value) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> boolVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::BOOLEAN_:
				{
					GuiCheckButton_setValue (field -> checkButton, value);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Boolean field not found in command window \"", my name, U"\".");
}

void UiForm_setOption (UiForm me, int *p_variable, int value) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> intVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::RADIO_:
				{
					if (value < 1 || value > field -> options.size) value = 1;   // guard against incorrect prefs file
					UiOption option = field -> options.at [value];
					GuiRadioButton_set (option -> radioButton);
				}
				break;
				case _kUiField_type::OPTIONMENU_:
				{
					if (value < 1 || value > field -> options.size) value = 1;   // guard against incorrect prefs file
					GuiOptionMenu_setValue (field -> optionMenu, value);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Option field not found in command window \"", my name, U"\".");
}

void UiForm_setOptionAsString (UiForm me, int *p_variable, const char32 *stringValue) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> intVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::RADIO_:
				{
					for (int i = 1; i <= field -> options.size; i ++) {
						UiOption b = field -> options.at [i];
						if (str32equ (stringValue, b -> name)) {
							GuiRadioButton_set (b -> radioButton);
						}
					}
					/* If not found: do nothing (guard against incorrect prefs file). */
				}
				break;
				case _kUiField_type::OPTIONMENU_:
				{
					int optionValue = 0;
					for (int i = 1; i <= field -> options.size; i ++) {
						UiOption b = field -> options.at [i];
						if (str32equ (stringValue, b -> name)) {
							optionValue = i;
							break;
						}
					}
					GuiOptionMenu_setValue (field -> optionMenu, optionValue);
					/* If not found: do nothing (guard against incorrect prefs file). */
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Option field not found in command window \"", my name, U"\".");
}

void UiForm_setString (UiForm me, char32 **p_variable, const char32 *value /* cattable */) {
	if (! value) value = U"";   // accept null strings
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> stringVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::WORD_:
				case _kUiField_type::SENTENCE_:
				case _kUiField_type::COLOUR_:
				case _kUiField_type::TEXT_:
				{
					GuiText_setString (field -> text, value);
				}
				break;
				case _kUiField_type::LABEL_:
				{
					GuiLabel_setText (field -> label, value);
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Text field not found in command window \"", my name, U"\".");
}

void UiForm_setColourAsGreyValue (UiForm me, Graphics_Colour *p_variable, double greyValue) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		if (field -> colourVariable == p_variable) {
			switch (field -> type)
			{
				case _kUiField_type::COLOUR_:
				{
					GuiText_setString (field -> text, Melder_double (greyValue));
				}
				break;
				default:
				{
					fatalField (me);
				}
			}
			return;
		}
	}
	Melder_fatal (U"Colour field not found in command window \"", my name, U"\".");
}

static UiField findField (UiForm me, const char32 *fieldName) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
		if (str32equ (fieldName, my field [ifield] -> name)) return my field [ifield];
	return nullptr;
}

static UiField findField_check (UiForm me, const char32 *fieldName) {
	UiField result = findField (me, fieldName);
	if (! result) {
		Melder_throw (U"Cannot find field \"", fieldName, U"\" in form.\n"
			U"The script may have changed while the form was open.\n"
			U"Please click Cancel in the form and try again.");
	}
	return result;
}

double UiForm_getReal_check (UiForm me, const char32 *fieldName) {
	UiField field = findField_check (me, fieldName);
	switch (field -> type)
	{
		case _kUiField_type::REAL_:
		case _kUiField_type::REAL_OR_UNDEFINED_:
		case _kUiField_type::POSITIVE_:
		{
			return field -> realValue;
		}
		break;
		default:
		{
			Melder_throw (U"Cannot find a real value in field \"", fieldName, U"\" in the form.\n"
				U"The script may have changed while the form was open.\n"
				U"Please click Cancel in the form and try again.");
		}
	}
	return 0.0;
}

integer UiForm_getInteger (UiForm me, const char32 *fieldName) {
	UiField field = findField (me, fieldName);
	if (! field) Melder_fatal (U"(UiForm_getInteger:) No field \"", fieldName, U"\" in command window \"", my name, U"\".");
	switch (field -> type)
	{
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_:
		case _kUiField_type::BOOLEAN_:
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		case _kUiField_type::LIST_:
		{
			return field -> integerValue;
		}
		break;
		default:
		{
			fatalField (me);
		}
	}
	return 0;
}

integer UiForm_getInteger_check (UiForm me, const char32 *fieldName) {
	UiField field = findField_check (me, fieldName);
	switch (field -> type)
	{
		case _kUiField_type::INTEGER_:
		case _kUiField_type::NATURAL_:
		case _kUiField_type::CHANNEL_:
		case _kUiField_type::BOOLEAN_:
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		case _kUiField_type::LIST_:
		{
			return field -> integerValue;
		}
		break;
		default:
		{
			Melder_throw (U"Cannot find an integer value in field \"", fieldName, U"\" in the form.\n"
				U"The script may have changed while the form was open.\n"
				U"Please click Cancel in the form and try again.");
		}
	}
	return 0;
}

char32 * UiForm_getString (UiForm me, const char32 *fieldName) {
	UiField field = findField (me, fieldName);
	if (! field) Melder_fatal (U"(UiForm_getString:) No field \"", fieldName, U"\" in command window \"", my name, U"\".");
	switch (field -> type)
	{
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		{
			return field -> stringValue;
		}
		break;
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		{
			UiOption b = field -> options.at [field -> integerValue];
			return b -> name;
		}
		break;
		case _kUiField_type::LIST_:
		{
			return (char32 *) field -> strings [field -> integerValue];
		}
		break;
		default:
		{
			fatalField (me);
		}
	}
	return nullptr;
}

char32 * UiForm_getString_check (UiForm me, const char32 *fieldName) {
	UiField field = findField_check (me, fieldName);
	switch (field -> type)
	{
		case _kUiField_type::WORD_:
		case _kUiField_type::SENTENCE_:
		case _kUiField_type::TEXT_:
		{
			return field -> stringValue;
		}
		break;
		case _kUiField_type::RADIO_:
		case _kUiField_type::OPTIONMENU_:
		{
			UiOption b = field -> options.at [field -> integerValue];
			return b -> name;
		}
		break;
		case _kUiField_type::LIST_:
		{
			return (char32 *) field -> strings [field -> integerValue];
		}
		break;
		default:
		{
			Melder_throw (U"Cannot find a string in field \"", fieldName, U"\" in the form.\n"
				U"The script may have changed while the form was open.\n"
				U"Please click Cancel in the form and try again.");
		}
	}
	return nullptr;
}

Graphics_Colour UiForm_getColour_check (UiForm me, const char32 *fieldName) {
	UiField field = findField_check (me, fieldName);
	switch (field -> type)
	{
		case _kUiField_type::COLOUR_: {
			return field -> colourValue;
		}
		break;
		default:
		{
			Melder_throw (U"Cannot find a real value in field \"", fieldName, U"\" in the form.\n"
				U"The script may have changed while the form was open.\n"
				U"Please click Cancel in the form and try again.");
		}
	}
	return Graphics_BLACK;
}

void UiForm_Interpreter_addVariables (UiForm me, Interpreter interpreter) {
	static MelderString lowerCaseFieldName { };
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		MelderString_copy (& lowerCaseFieldName, field -> name);
		/*
			Change e.g. "Number of people" to "number_of_people".
		*/
		lowerCaseFieldName.string [0] = tolower32 (lowerCaseFieldName.string [0]);   // BUG for non-BMP characters
		for (char32 *p = & lowerCaseFieldName.string [0]; *p != U'\0'; p ++) {
			if (*p == U' ') *p = U'_';
		}
		switch (field -> type)
		{
			case _kUiField_type::INTEGER_:
			case _kUiField_type::NATURAL_:
			case _kUiField_type::CHANNEL_:
			case _kUiField_type::BOOLEAN_:
			{
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				var -> numericValue = field -> integerValue;
			}
			break;
			case _kUiField_type::REAL_:
			case _kUiField_type::REAL_OR_UNDEFINED_:
			case _kUiField_type::POSITIVE_:
			{
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				var -> numericValue = field -> realValue;
			}
			break;
			case _kUiField_type::RADIO_:
			case _kUiField_type::OPTIONMENU_:
			{
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				var -> numericValue = field -> integerValue;
				MelderString_appendCharacter (& lowerCaseFieldName, U'$');
				var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				Melder_free (var -> stringValue);
				UiOption b = field -> options.at [field -> integerValue];
				var -> stringValue = Melder_dup (b -> name);
			}
			break;
			case _kUiField_type::LIST_:
			{
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				var -> numericValue = field -> integerValue;
				MelderString_appendCharacter (& lowerCaseFieldName, U'$');
				var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				Melder_free (var -> stringValue);
				var -> stringValue = Melder_dup (field -> strings [field -> integerValue]);
			}
			break;
			case _kUiField_type::WORD_:
			case _kUiField_type::SENTENCE_:
			case _kUiField_type::TEXT_:
			{
				MelderString_appendCharacter (& lowerCaseFieldName, U'$');
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string);
				Melder_free (var -> stringValue);
				var -> stringValue = Melder_dup (field -> stringValue);
			}
			break;
			case _kUiField_type::COLOUR_:
			{
				// to be implemented
			}
			break;
			default:
			{
			}
		}
	}
}

int UiForm_getClickedContinueButton (UiForm me) {
	return my clickedContinueButton;
}

/* End of file Ui.cpp */
