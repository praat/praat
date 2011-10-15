/* Ui.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include <wctype.h>
#include <ctype.h>
#include "longchar.h"
#include "machine.h"
#include "Gui.h"
#include "Collection.h"
#include "UiP.h"
#include "Editor.h"
#include "Graphics.h"   /* colours. */

/***** class UiField: the things that have values in an UiForm dialog *****/

/* Values for 'type'. */
#define UI_REAL  1
#define UI_REAL_OR_UNDEFINED  2
#define UI_POSITIVE  3
#define UI_INTEGER  4
#define UI_NATURAL  5
#define UI_WORD  6
#define UI_SENTENCE  7
#define UI_COLOUR  8
#define UI_CHANNEL  9
	#define UI_LABELLEDTEXT_MIN  UI_REAL
	#define UI_LABELLEDTEXT_MAX  UI_CHANNEL
#define UI_LABEL  10
#define UI_TEXT  11
#define UI_BOOLEAN  12
#define UI_RADIO  13
#define UI_OPTIONMENU  14
#define UI_LIST  15

Thing_implement (UiField, Thing, 0);

void structUiField :: v_destroy () {
	Melder_free (formLabel);
	Melder_free (stringValue);
	Melder_free (stringValueA);
	Melder_free (stringDefaultValue);
	forget (options);
	UiField_Parent :: v_destroy ();
}

static UiField UiField_create (int type, const wchar_t *name) {
	autoUiField me = Thing_new (UiField);
	wchar shortName [101], *p;
	my type = type;
	my formLabel = Melder_wcsdup (name);
	wcscpy (shortName, name);
	/*
	 * Strip parentheses and colon off parameter name.
	 */
	//p = wcschr (shortName, ':');   /* ppgb 20101015: no idea why this used to be here */
	//if (p) *p = '\0';
	if ((p = (wchar *) wcschr (shortName, '(')) != NULL) {
		*p = '\0';
		if (p - shortName > 0 && p [-1] == ' ') p [-1] = '\0';
	}
	p = shortName;
	if (*p != '\0' && p [wcslen (p) - 1] == ':') p [wcslen (p) - 1] = '\0';
	Thing_setName (me.peek(), shortName);
	return me.transfer();
}

/***** class UiOption: radio buttons and menu options *****/

Thing_define (UiOption, Thing) {
	// new data:
	public:
		GuiObject toggle;
};

Thing_implement (UiOption, Thing, 0);

static Any UiOption_create (const wchar_t *label) {
	UiOption me = Thing_new (UiOption);
	Thing_setName (me, label);
	return me;
}

static void gui_radiobutton_cb_toggled (I, GuiRadioButtonEvent event) {
	#if !gtk
	iam (UiField);
	for (int i = 1; i <= my options -> size; i ++) {
		UiOption b = static_cast <UiOption> (my options -> item [i]);
		GuiRadioButton_setValue (b -> toggle, b -> toggle == event -> toggle);
	}
	#endif
}

Any UiRadio_addButton (I, const wchar_t *label) {
	iam (UiField);
	UiOption thee;
	if (me == NULL) return NULL;
	Melder_assert (my type == UI_RADIO || my type == UI_OPTIONMENU);
	thee = static_cast <UiOption> (UiOption_create (label));
	Collection_addItem (my options, thee);
	return thee;
}

#if motif
// TODO: Ik denk dat dit Native GTK gedrag is (als dit alleen het label update)
static void cb_optionChanged (GuiObject w, XtPointer void_me, XtPointer call) {
	iam (UiField);
	(void) call;
	for (int i = 1; i <= my options -> size; i ++) {
		UiOption b = static_cast <UiOption> (my options -> item [i]);
		#if motif
		if (b -> toggle == w) {
			XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
			XmToggleButtonSetState (b -> toggle, TRUE, FALSE);
			if (Melder_debug == 11) {
				Melder_warning (i, " \"", b -> name, "\"");
			}
		} else {
			XmToggleButtonSetState (b -> toggle, FALSE, FALSE);
		}
		#endif
	}
}
#endif

Any UiOptionMenu_addButton (I, const wchar_t *label) {
	iam (UiField);
	UiOption thee;
	if (me == NULL) return NULL;
	Melder_assert (my type == UI_RADIO || my type == UI_OPTIONMENU);
	thee = static_cast <UiOption> (UiOption_create (label));
	Collection_addItem (my options, thee);
	return thee;
}

/***** Things to do with UiField objects. *****/

static void UiField_setDefault (UiField me) {
	switch (my type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: case UI_INTEGER: case UI_NATURAL:
			case UI_WORD: case UI_SENTENCE: case UI_COLOUR: case UI_CHANNEL: case UI_TEXT:
		{
			GuiText_setString (my text, my stringDefaultValue);
		} break; case UI_BOOLEAN: {
			GuiCheckButton_setValue (my toggle, my integerDefaultValue);
		} break; case UI_RADIO: {
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = static_cast <UiOption> (my options -> item [i]);
				GuiRadioButton_setValue (b -> toggle, i == my integerDefaultValue);
			}
		} break; case UI_OPTIONMENU: {
			#if gtk
				gtk_combo_box_set_active (GTK_COMBO_BOX (my cascadeButton), (my integerDefaultValue - 1));
			#elif motif
				for (int i = 1; i <= my options -> size; i ++) {
					UiOption b = static_cast <UiOption> (my options -> item [i]);
					XmToggleButtonSetState (b -> toggle, i == my integerDefaultValue, False);
					if (i == my integerDefaultValue) {
						XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
					}
				}
			#endif
		} break; case UI_LIST: {
			GuiList_selectItem (my list, my integerDefaultValue);
		}
	}
}

static int colourToValue (UiField me, wchar_t *string) {
	wchar_t *p = string;
	while (*p == ' ' || *p == '\t') p ++;
	*p = tolower (*p);
	int first = *p;
	if (first == '{') {
		my colourValue. red = Melder_atof (++ p);
		p = (wchar *) wcschr (p, ',');
		if (p == NULL) return 0;
		my colourValue. green = Melder_atof (++ p);
		p = (wchar *) wcschr (p, ',');
		if (p == NULL) return 0;
		my colourValue. blue = Melder_atof (++ p);
	} else {
		*p = tolower (*p);
		if (wcsequ (p, L"black")) my colourValue = Graphics_BLACK;
		else if (wcsequ (p, L"white")) my colourValue = Graphics_WHITE;
		else if (wcsequ (p, L"red")) my colourValue = Graphics_RED;
		else if (wcsequ (p, L"green")) my colourValue = Graphics_GREEN;
		else if (wcsequ (p, L"blue")) my colourValue = Graphics_BLUE;
		else if (wcsequ (p, L"yellow")) my colourValue = Graphics_YELLOW;
		else if (wcsequ (p, L"cyan")) my colourValue = Graphics_CYAN;
		else if (wcsequ (p, L"magenta")) my colourValue = Graphics_MAGENTA;
		else if (wcsequ (p, L"maroon")) my colourValue = Graphics_MAROON;
		else if (wcsequ (p, L"lime")) my colourValue = Graphics_LIME;
		else if (wcsequ (p, L"navy")) my colourValue = Graphics_NAVY;
		else if (wcsequ (p, L"teal")) my colourValue = Graphics_TEAL;
		else if (wcsequ (p, L"purple")) my colourValue = Graphics_PURPLE;
		else if (wcsequ (p, L"olive")) my colourValue = Graphics_OLIVE;
		else if (wcsequ (p, L"pink")) my colourValue = Graphics_PINK;
		else if (wcsequ (p, L"silver")) my colourValue = Graphics_SILVER;
		else if (wcsequ (p, L"grey")) my colourValue = Graphics_GREY;
		else { *p = first; return 0; }
		*p = first;
	}
	return 1;
}

static void UiField_widgetToValue (UiField me) {
	switch (my type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
			autostring dirty = GuiText_getString (my text);   // the text as typed by the user
			Interpreter_numericExpression (NULL, dirty.peek(), & my realValue); therror
			/*
			 * Put a clean version of the new value in the form.
			 * If the value is equal to the default, make sure that any default comments are included.
			 */
			if (my realValue == Melder_atof (my stringDefaultValue)) {
				GuiText_setString (my text, my stringDefaultValue);
			} else {
				wchar_t clean [40];
				wcscpy (clean, Melder_double (my realValue));
				/*
				 * If the default is overtly real, the shown value must be as well.
				 */
				if ((wcschr (my stringDefaultValue, '.') || wcschr (my stringDefaultValue, 'e')) &&
					! (wcschr (clean, '.') || wcschr (clean, 'e')))
				{
					wcscat (clean, L".0");
				}
				GuiText_setString (my text, clean);
			}
			if (my realValue == NUMundefined && my type != UI_REAL_OR_UNDEFINED)
				Melder_throw (L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE L" has the value \"undefined\".");
			if (my type == UI_POSITIVE && my realValue <= 0.0)
				Melder_throw (L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE L" must be greater than 0.0.");
		} break; case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: {
			autostring dirty = GuiText_getString (my text);
			if (my type == UI_CHANNEL && (wcsequ (dirty.peek(), L"Left") || wcsequ (dirty.peek(), L"Mono"))) {
				my integerValue = 1;
			} else if (my type == UI_CHANNEL && (wcsequ (dirty.peek(), L"Right") || wcsequ (dirty.peek(), L"Stereo"))) {
				my integerValue = 2;
			} else {
				double realValue;
				Interpreter_numericExpression (NULL, dirty.peek(), & realValue); therror
				my integerValue = floor (realValue + 0.5);
			}
			if (my integerValue == wcstol (my stringDefaultValue, NULL, 10)) {
				GuiText_setString (my text, my stringDefaultValue);
			} else {
				GuiText_setString (my text, Melder_integer (my integerValue));
			}
			if (my type == UI_NATURAL && my integerValue < 1)
				Melder_throw (L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE L" must be a positive whole number.");
		} break; case UI_WORD: {
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
			wchar_t *p = my stringValue;
			while (*p != '\0') { if (*p == ' ' || *p == '\t') *p = '\0'; p ++; }
			GuiText_setString (my text, my stringValue);
		} break; case UI_SENTENCE: case UI_TEXT: {
			Melder_free (my stringValue);
			my stringValue = GuiText_getString (my text);
		} break; case UI_BOOLEAN: {
			my integerValue = GuiCheckButton_getValue (my toggle);
		} break; case UI_RADIO: {
			my integerValue = 0;
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = static_cast <UiOption> (my options -> item [i]);
				if (GuiRadioButton_getValue (b -> toggle))
					my integerValue = i;
			}
			if (my integerValue == 0)
				Melder_throw ("No option chosen for " L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE ".");
		} break; case UI_OPTIONMENU: {
			my integerValue = 0;
			#if gtk
				// TODO: Graag even een check :)
				my integerValue = gtk_combo_box_get_active (GTK_COMBO_BOX (my cascadeButton)) + 1;
			#elif motif
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = static_cast <UiOption> (my options -> item [i]);
				if (XmToggleButtonGetState (b -> toggle))
					my integerValue = i;
			}
			#endif
			if (my integerValue == 0)
				Melder_throw ("No option chosen for " L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE ".");
		} break; case UI_LIST: {
			long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);   // BUG memory
			if (selected == NULL) {
				Melder_warning (L"No items selected.");
				my integerValue = 1;
			} else {
				if (numberOfSelected > 1) Melder_warning (L"More than one item selected.");
				my integerValue = selected [1];
				NUMvector_free <long> (selected, 1);
			}
		} break; case UI_COLOUR: {
			autostring string = GuiText_getString (my text);
			if (colourToValue (me, string.peek())) {
				// do nothing
			} else {
				Interpreter_numericExpression (NULL, string.peek(), & my colourValue. red); therror
				my colourValue. green = my colourValue. blue = my colourValue. red;
			}
		}
	}
}

static void UiField_stringToValue (UiField me, const wchar *string, Interpreter interpreter) {
	switch (my type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
			if (wcsspn (string, L" \t") == wcslen (string))
				Melder_throw (L"Argument `", my name, L"' empty.");
			Interpreter_numericExpression (interpreter, string, & my realValue);
			if (my realValue == NUMundefined && my type != UI_REAL_OR_UNDEFINED)
				Melder_throw (L"\"", my name, "\" has the value \"undefined\".");
			if (my type == UI_POSITIVE && my realValue <= 0.0)
				Melder_throw ("\"", my name, "\" must be greater than 0.");
		} break; case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: {
			if (wcsspn (string, L" \t") == wcslen (string))
				Melder_throw ("Argument \"", my name, "\" empty.");
			if (my type == UI_CHANNEL && (wcsequ (string, L"All") || wcsequ (string, L"Average"))) {
				my integerValue = 0;
			} else if (my type == UI_CHANNEL && (wcsequ (string, L"Left") || wcsequ (string, L"Mono"))) {
				my integerValue = 1;
			} else if (my type == UI_CHANNEL && (wcsequ (string, L"Right") || wcsequ (string, L"Stereo"))) {
				my integerValue = 2;
			} else {
				double realValue;
				Interpreter_numericExpression (interpreter, string, & realValue);
				my integerValue = floor (realValue + 0.5);
			}
			if (my type == UI_NATURAL && my integerValue < 1)
				Melder_throw ("\"", my name, "\" must be a positive whole number.");
		} break; case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			Melder_free (my stringValue);
			my stringValue = Melder_wcsdup_f (string);
		} break; case UI_BOOLEAN: {
			if (! string [0])
				Melder_throw ("Empty argument for toggle button.");
			my integerValue = string [0] == '1' || string [0] == 'y' || string [0] == 'Y' ||
				string [0] == 't' || string [0] == 'T';
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			my integerValue = 0;
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = static_cast <UiOption> (my options -> item [i]);
				if (wcsequ (string, b -> name))
					my integerValue = i;
			}
			if (my integerValue == 0) {
				/*
				 * Retry with different case.
				 */
				for (int i = 1; i <= my options -> size; i ++) {
					UiOption b = static_cast <UiOption> (my options -> item [i]);
					wchar_t name2 [100];
					wcscpy (name2, b -> name);
					if (islower (name2 [0])) name2 [0] = toupper (name2 [0]);
					else if (isupper (name2 [0])) name2 [0] = tolower (name2 [0]);
					if (wcsequ (string, name2))
						my integerValue = i;
				}
			}
			if (my integerValue == 0) {
				Melder_throw ("Field \"", my name, "\" cannot have the value \"", string, "\".");
			}
		} break; case UI_LIST: {
			long i = 1;
			for (; i <= my numberOfStrings; i ++)
				if (wcsequ (string, my strings [i])) break;
			if (i > my numberOfStrings)
				Melder_throw ("Field \"", my name, "\" cannot have the value \"", string, "\".");
			my integerValue = i;
		} break; case UI_COLOUR: {
			autostring string2 = Melder_wcsdup_f (string);
			if (colourToValue (me, string2.peek())) {
				/* OK */
			} else {
				try {
					Interpreter_numericExpression (interpreter, string2.peek(), & my colourValue. red);
					my colourValue. green = my colourValue. blue = my colourValue. red;
				} catch (MelderError) {
					Melder_clearError ();
					Melder_throw ("Cannot compute a colour from \"", string2.peek(), "\".");
				}
			}
		} break; default: {
			Melder_throw ("Unknown field type ", my type, ".");
		}
	}
}

/***** History mechanism. *****/

static MelderString theHistory = { 0 };
void UiHistory_write (const wchar_t *string) { MelderString_append (& theHistory, string); }
wchar_t *UiHistory_get (void) { return theHistory.string; }
void UiHistory_clear (void) { MelderString_empty (& theHistory); }

/***** class UiForm: dialog windows *****/

Thing_implement (UiForm, Thing, 0);

bool (*theAllowExecutionHookHint) (void *closure) = NULL;
void *theAllowExecutionClosureHint = NULL;

void Ui_setAllowExecutionHook (bool (*allowExecutionHook) (void *closure), void *allowExecutionClosure) {
	theAllowExecutionHookHint = allowExecutionHook;
	theAllowExecutionClosureHint = allowExecutionClosure;
}

void structUiForm :: v_destroy () {
	for (int ifield = 1; ifield <= numberOfFields; ifield ++)
		forget (field [ifield]);
	if (d_dialogForm) {
		GuiObject_destroy (GuiObject_parent (d_dialogForm));
	}
	Melder_free (invokingButtonTitle);
	Melder_free (helpTitle);
	UiForm_Parent :: v_destroy ();
}

static void gui_button_cb_revert (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
}

static void gui_dialog_cb_close (I) {
	iam (UiForm);
	if (my cancelCallback) my cancelCallback (me, my buttonClosure);
	GuiObject_hide (my d_dialogForm);
	if (my destroyWhenUnmanaged) forget (me);
}
static void gui_button_cb_cancel (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	if (my cancelCallback) my cancelCallback (me, my buttonClosure);
	GuiObject_hide (my d_dialogForm);
	if (my destroyWhenUnmanaged) forget (me);
}

void UiForm_widgetsToValues (I) {
	iam (UiForm);
	try {
		for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
			UiField_widgetToValue (my field [ifield]);
	} catch (MelderError) {
		Melder_throw ("Please correct command window " L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE " or cancel.");
	}
}

static void UiForm_okOrApply (I, GuiObject button, int hide) {
	iam (UiForm);
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_error_ ("Cannot execute command window " L_LEFT_DOUBLE_QUOTE, my name, L_RIGHT_DOUBLE_QUOTE L".");
		Melder_flushError (NULL);
		return;
	}
	try {
		UiForm_widgetsToValues (me);
	} catch (MelderError) {
		Melder_flushError (NULL);
		return;
	}
	/* In the next, w must become my okButton? */
	/*XtRemoveCallback (w, XmNactivateCallback, UiForm_ok, void_me);   /* FIX */
	if (my okButton) GuiObject_setSensitive (my okButton, false);
	for (int i = 1; i <= my numberOfContinueButtons; i ++) if (my continueButtons [i]) GuiObject_setSensitive (my continueButtons [i], false);
	if (my applyButton) GuiObject_setSensitive (my applyButton, false);
	if (my cancelButton) GuiObject_setSensitive (my cancelButton, false);
	if (my revertButton) GuiObject_setSensitive (my revertButton, false);
	if (my helpButton) GuiObject_setSensitive (my helpButton, false);
	#if motif
	XmUpdateDisplay (my d_dialogForm);
	#endif
	if (my isPauseForm) {
		for (int i = 1; i <= my numberOfContinueButtons; i ++) {
			if (button == my continueButtons [i]) {
				my clickedContinueButton = i;
			}
		}
	}
	/*
	 * Keep the gate for error handling.
	 */
	try {
		my okCallback (me, NULL, NULL, NULL, false, my buttonClosure); therror
		/*
		 * Write everything to history. Before destruction!
		 */
		if (! my isPauseForm) {
			UiHistory_write (L"\n");
			UiHistory_write (my invokingButtonTitle);
			int size = my numberOfFields;
			while (size >= 1 && my field [size] -> type == UI_LABEL)
				size --;   // ignore trailing fields without a value
			for (int ifield = 1; ifield <= size; ifield ++) {
				UiField field = my field [ifield];
				switch (field -> type) {
					case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
						UiHistory_write (L" ");
						UiHistory_write (Melder_double (field -> realValue));
					} break; case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: {
						UiHistory_write (L" ");
						UiHistory_write (Melder_integer (field -> integerValue));
					} break; case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
						if (ifield < size && (field -> stringValue [0] == '\0' || wcschr (field -> stringValue, ' '))) {
							UiHistory_write (L" \"");
							UiHistory_write (field -> stringValue);   // BUG: should double any double quotes
							UiHistory_write (L"\"");
						} else {
							UiHistory_write (L" ");
							UiHistory_write (field -> stringValue);
						}
					} break; case UI_BOOLEAN: {
						UiHistory_write (field -> integerValue ? L" yes" : L" no");
					} break; case UI_RADIO: case UI_OPTIONMENU: {
						UiOption b = static_cast <UiOption> (field -> options -> item [field -> integerValue]);
						if (ifield < size && (b -> name [0] == '\0' || wcschr (b -> name, ' '))) {
							UiHistory_write (L" \"");
							UiHistory_write (b -> name);
							UiHistory_write (L"\"");
						} else {
							UiHistory_write (L" ");
							UiHistory_write (b -> name);
						}
					} break; case UI_LIST: {
						if (ifield < size && (field -> strings [field -> integerValue] [0] == '\0' || wcschr (field -> strings [field -> integerValue], ' '))) {
							UiHistory_write (L" \"");
							UiHistory_write (field -> strings [field -> integerValue]);
							UiHistory_write (L"\"");
						} else {
							UiHistory_write (L" ");
							UiHistory_write (field -> strings [field -> integerValue]);
						}
					} break; case UI_COLOUR: {
						UiHistory_write (L" ");
						UiHistory_write (Graphics_Colour_name (field -> colourValue));
					}
				}
			}
		}
		if (hide) {
			GuiObject_hide (my d_dialogForm);
			if (my destroyWhenUnmanaged) {
				forget (me);
				return;
			}
		}
	} catch (MelderError) {
		/*
		 * If a solution has already been suggested, or the "error" was actually a conscious user action, do not add anything more.
		 */
		if (! wcsstr (Melder_getError (), L"Please ") && ! wcsstr (Melder_getError (), L"You could ") &&
			! wcsstr (Melder_getError (), L"You interrupted ") && ! wcsstr (Melder_getError (), L"Interrupted!"))
		{
			/*
			 * Otherwise, show a generic message.
			 */
			if (wcsstr (Melder_getError (), L"Selection changed!")) {
				Melder_error_ ("Please change the selection in the object list, or click Cancel in the command window " L_LEFT_DOUBLE_QUOTE,
					my name, L_RIGHT_DOUBLE_QUOTE L".");
			} else {
				Melder_error_ ("Please change something in the command window " L_LEFT_DOUBLE_QUOTE,
					my name, L_RIGHT_DOUBLE_QUOTE L", or click Cancel in that window.");
			}
		}
		Melder_flushError (NULL);
	}
	if (my okButton) GuiObject_setSensitive (my okButton, true);
	for (int i = 1; i <= my numberOfContinueButtons; i ++) if (my continueButtons [i]) GuiObject_setSensitive (my continueButtons [i], true);
	if (my applyButton) GuiObject_setSensitive (my applyButton, true);
	if (my cancelButton) GuiObject_setSensitive (my cancelButton, true);
	if (my revertButton) GuiObject_setSensitive (my revertButton, true);
	if (my helpButton) GuiObject_setSensitive (my helpButton, true);
}

static void gui_button_cb_ok (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	UiForm_okOrApply (me, event -> button, true);
}

static void gui_button_cb_apply (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	UiForm_okOrApply (me, event -> button, false);
}

static void gui_button_cb_help (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	Melder_help (my helpTitle);
}

UiForm UiForm_create (GuiObject parent, const wchar *title,
	void (*okCallback) (UiForm sendingForm, const wchar *sendingString, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure), void *buttonClosure,
	const wchar *invokingButtonTitle, const wchar *helpTitle)
{
	autoUiForm me = Thing_new (UiForm);
	my d_dialogParent = parent;
	Thing_setName (me.peek(), title);
	my okCallback = okCallback;
	my buttonClosure = buttonClosure;
	my invokingButtonTitle = Melder_wcsdup (invokingButtonTitle);
	my helpTitle = Melder_wcsdup (helpTitle);
	return me.transfer();
}

void UiForm_setPauseForm (I,
	int numberOfContinueButtons, int defaultContinueButton, int cancelContinueButton,
	const wchar *continue1, const wchar *continue2, const wchar *continue3,
	const wchar *continue4, const wchar *continue5, const wchar *continue6,
	const wchar *continue7, const wchar *continue8, const wchar *continue9,
	const wchar *continue10,
	void (*cancelCallback) (Any dia, void *closure))
{
	iam (UiForm);
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

static void commonOkCallback (UiForm dia, const wchar *dummy, Interpreter interpreter, const wchar *invokingButtonTitle, bool modified, void *closure) {
	EditorCommand cmd = (EditorCommand) closure;
	(void) dia;
	(void) dummy;
	(void) invokingButtonTitle;
	(void) modified;
	cmd -> commandCallback (cmd -> d_editor, cmd, cmd -> d_uiform, NULL, interpreter); therror
}

UiForm UiForm_createE (EditorCommand cmd, const wchar *title, const wchar *invokingButtonTitle, const wchar *helpTitle) {
	Editor editor = (Editor) cmd -> d_editor;
	UiForm dia = UiForm_create (editor -> d_windowForm, title, commonOkCallback, cmd, invokingButtonTitle, helpTitle);
	dia -> command = cmd;
	return dia;
}

static UiField UiForm_addField (UiForm me, int type, const wchar *label) {
	if (my numberOfFields == MAXIMUM_NUMBER_OF_FIELDS)
		Melder_throw ("Cannot have more than ", MAXIMUM_NUMBER_OF_FIELDS, "in a form.");
	return my field [++ my numberOfFields] = UiField_create (type, label);
}

Any UiForm_addReal (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_REAL, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addRealOrUndefined (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_REAL_OR_UNDEFINED, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addPositive (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_POSITIVE, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addInteger (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_INTEGER, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addNatural (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_NATURAL, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addWord (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_WORD, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addSentence (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_SENTENCE, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addLabel (I, const wchar *name, const wchar *label) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_LABEL, name);
	thy stringValue = Melder_wcsdup (label);
	return thee.transfer();
}

Any UiForm_addBoolean (I, const wchar *label, int defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_BOOLEAN, label);
	thy integerDefaultValue = defaultValue;
	return thee.transfer();
}

Any UiForm_addText (I, const wchar *name, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_TEXT, name);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addRadio (I, const wchar *label, int defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_RADIO, label);
	thy integerDefaultValue = defaultValue;
	thy options = Ordered_create ();
	return thee.transfer();
}

Any UiForm_addOptionMenu (I, const wchar *label, int defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_OPTIONMENU, label);
	thy integerDefaultValue = defaultValue;
	thy options = Ordered_create ();
	return thee.transfer();
}

Any UiForm_addList (I, const wchar *label, long numberOfStrings, const wchar **strings, long defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_LIST, label);
	thy numberOfStrings = numberOfStrings;
	thy strings = strings;
	thy integerDefaultValue = defaultValue;
	return thee.transfer();
}

Any UiForm_addColour (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_COLOUR, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

Any UiForm_addChannel (I, const wchar *label, const wchar *defaultValue) {
	iam (UiForm);
	autoUiField thee = UiForm_addField (me, UI_CHANNEL, label);
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee.transfer();
}

#define DIALOG_X  150
#define DIALOG_Y  70
#define HELP_BUTTON_WIDTH  60
#define STANDARDS_BUTTON_WIDTH  100
#define REVERT_BUTTON_WIDTH  60
#define STOP_BUTTON_WIDTH  50
#define HELP_BUTTON_X  20
#define LIST_HEIGHT  192

static MelderString theFinishBuffer = { 0 };

static void appendColon (void) {
	long length = theFinishBuffer.length;
	if (length < 1) return;
	wchar_t lastCharacter = theFinishBuffer.string [length - 1];
	if (lastCharacter == ':' || lastCharacter == '?' || lastCharacter == '.') return;
	MelderString_appendCharacter (& theFinishBuffer, ':');
}

void UiForm_finish (I) {
	iam (UiForm);
	if (! my d_dialogParent && ! my isPauseForm) return;

	int size = my numberOfFields;
	int dialogHeight = 0, x = Gui_LEFT_DIALOG_SPACING, y;
	int textFieldHeight = Gui_TEXTFIELD_HEIGHT;
	int dialogWidth = 520, dialogCentre = dialogWidth / 2, fieldX = dialogCentre + Gui_LABEL_SPACING / 2;
	int labelWidth = fieldX - Gui_LABEL_SPACING - x, fieldWidth = labelWidth, halfFieldWidth = fieldWidth / 2 - 6;

	#if gtk
		GuiObject form, buttons;
		int numberOfRows = 0, row = 0;
	#else
		GuiObject form, buttons; // Define?
	#endif

	/*
		Compute height. Cannot leave this to the default geometry management system.
	*/
	for (long ifield = 1; ifield <= my numberOfFields; ifield ++ ) {
		UiField thee = my field [ifield], previous = my field [ifield - 1];
		dialogHeight +=
			ifield == 1 ? Gui_TOP_DIALOG_SPACING :
			thy type == UI_RADIO || previous -> type == UI_RADIO ? Gui_VERTICAL_DIALOG_SPACING_DIFFERENT :
			thy type >= UI_LABELLEDTEXT_MIN && thy type <= UI_LABELLEDTEXT_MAX && wcsnequ (thy name, L"right ", 6) &&
			previous -> type >= UI_LABELLEDTEXT_MIN && previous -> type <= UI_LABELLEDTEXT_MAX &&
			wcsnequ (previous -> name, L"left ", 5) ? - textFieldHeight : Gui_VERTICAL_DIALOG_SPACING_SAME;
		thy y = dialogHeight;
		dialogHeight +=
			thy type == UI_BOOLEAN ? Gui_CHECKBUTTON_HEIGHT :
			thy type == UI_RADIO ? thy options -> size * Gui_RADIOBUTTON_HEIGHT +
				(thy options -> size - 1) * Gui_RADIOBUTTON_SPACING :
			thy type == UI_OPTIONMENU ? Gui_OPTIONMENU_HEIGHT :
			thy type == UI_LIST ? LIST_HEIGHT :
			thy type == UI_LABEL && thy stringValue [0] != '\0' && thy stringValue [wcslen (thy stringValue) - 1] != '.' &&
				ifield != my numberOfFields ? textFieldHeight
				#ifdef _WIN32
					- 6 :
				#else
					- 10 :
				#endif
			textFieldHeight;
		#if gtk
			numberOfRows += wcsnequ (thy name, L"left ", 5);
		#endif
	}
	dialogHeight += 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT;
	my d_dialogForm = GuiDialog_create (my d_dialogParent, DIALOG_X, DIALOG_Y, dialogWidth, dialogHeight, my name, gui_dialog_cb_close, me, 0);

	#if gtk
		form = gtk_table_new (numberOfRows, 3, false);
		gtk_table_set_col_spacing (GTK_TABLE (form), 0, 5);
		gtk_container_add (GTK_CONTAINER (my d_dialogForm), GTK_WIDGET (form));
		gtk_widget_show (GTK_WIDGET (form));
		buttons = GTK_DIALOG (GuiObject_parent (my d_dialogForm)) -> action_area;
	#else
		form = my d_dialogForm;
		buttons = my d_dialogForm;
	#endif

	for (long ifield = 1; ifield <= size; ifield ++) {
		UiField field = my field [ifield];
		y = field -> y;
		switch (field -> type) {
			case UI_REAL:
			case UI_REAL_OR_UNDEFINED:
			case UI_POSITIVE:
			case UI_INTEGER:
			case UI_NATURAL:
			case UI_WORD:
			case UI_SENTENCE:
			case UI_COLOUR:
			case UI_CHANNEL:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 3;
				#endif
				if (wcsnequ (field -> name, L"left ", 5)) {
					MelderString_copy (& theFinishBuffer, field -> formLabel + 5);
					appendColon ();
					GuiObject label = GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					#if gtk
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (label), 0, 1, row, row + 1);
					#endif
					field -> text = GuiText_createShown (form, fieldX, fieldX + halfFieldWidth, y, Gui_AUTOMATIC, 0);
					#if gtk
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> text), 1, 2, row, row + 1);
					#endif
				} else if (wcsnequ (field -> name, L"right ", 6)) {
					field -> text = GuiText_createShown (form, fieldX + halfFieldWidth + 12, fieldX + fieldWidth,
						y, Gui_AUTOMATIC, 0);
					#if gtk
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> text), 2, 3, row, row + 1);
						row += 1;
					#endif
				} else {
					MelderString_copy (& theFinishBuffer, field -> formLabel);
					appendColon ();
					GuiObject label = GuiLabel_createShown (form, x, x + labelWidth,
						ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					#if gtk
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (label), 0, 1, row, row + 1);
					#endif
					field -> text = GuiText_createShown (form, fieldX, fieldX + fieldWidth, // or once the dialog is a Form: - Gui_RIGHT_DIALOG_SPACING,
						y, Gui_AUTOMATIC, 0);
					#if gtk
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> text), 1, 3, row, row + 1);
						row += 1;
					#endif
				}
			} break;
			case UI_TEXT:
			{
				field -> text = GuiText_createShown (form, x, x + dialogWidth - Gui_LEFT_DIALOG_SPACING - Gui_RIGHT_DIALOG_SPACING,
					y, Gui_AUTOMATIC, 0);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> text), 0, 3, row, row + 1);
					row += 1;
				#endif
			} break;
			case UI_LABEL:
			{
				MelderString_copy (& theFinishBuffer, field -> stringValue);
				field -> text = GuiLabel_createShown (form,
					x, dialogWidth /* allow to extend into the margin */, y + 5, y + 5 + textFieldHeight,
					theFinishBuffer.string, 0);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> text), 0, 3, row, row + 1);
					row += 1;
				#endif
			} break;
			case UI_RADIO:
			{
				int ylabel = y;
				#if gtk
					 void *group = NULL;
				#endif
				#if defined (macintosh)
					ylabel += 1;
				#endif
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiObject label = GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_RADIOBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (label), 0, 1, row, row + 1);
				#endif
				for (long ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = static_cast <UiOption> (field -> options -> item [ibutton]);
					MelderString_copy (& theFinishBuffer, button -> name);
					button -> toggle = GuiRadioButton_createShown (form,
						fieldX, dialogWidth /* allow to extend into the margin */,
						y + (ibutton - 1) * (Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING), Gui_AUTOMATIC,
						theFinishBuffer.string, gui_radiobutton_cb_toggled, field, 0);
					#if gtk
						if (group != NULL) {
							GuiRadioButton_setGroup (button -> toggle, group);
						} 
						group = GuiRadioButton_getGroup (button -> toggle);
						gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (button -> toggle), 1, 3, row, row + 1);
						row += 1;
					#endif
				}
			} break; 
			case UI_OPTIONMENU:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 2;
				#endif
				GuiObject bar, box;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiObject label = GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_OPTIONMENU_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (label), 0, 1, row, row + 1);
				#endif

				#if motif
					bar = XmCreateMenuBar (form, "UiOptionMenu", NULL, 0);
					XtVaSetValues (bar, XmNx, fieldX - 4, XmNy, y - 4
						#if defined (macintosh)
							- 1
						#endif
						, XmNwidth, fieldWidth + 8, XmNheight, Gui_OPTIONMENU_HEIGHT + 8, NULL);
				#endif
				// TODO: dit wil natuurlijk heel graag in GuiComboBox.c ;)
				#if gtk
					field -> cascadeButton = gtk_combo_box_new_text ();
					gtk_combo_box_set_focus_on_click (GTK_COMBO_BOX (field -> cascadeButton), false);
					GTK_WIDGET_UNSET_FLAGS (field -> cascadeButton, GTK_CAN_DEFAULT);
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> cascadeButton), 1, 3, row, row + 1);
					row += 1;
				#elif motif
					box = GuiMenuBar_addMenu2 (bar, L"choice", 0, & field -> cascadeButton);
					XtVaSetValues (bar, XmNwidth, fieldWidth + 8, NULL);
					XtVaSetValues (field -> cascadeButton, XmNx, 4, XmNy, 4, XmNwidth, fieldWidth, XmNheight, Gui_OPTIONMENU_HEIGHT, NULL);
				#endif
				for (long ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = static_cast <UiOption> (field -> options -> item [ibutton]);
					MelderString_copy (& theFinishBuffer, button -> name);
					#if gtk
						gtk_combo_box_append_text (GTK_COMBO_BOX (field -> cascadeButton), Melder_peekWcsToUtf8 (theFinishBuffer.string));
					#elif motif
						button -> toggle = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmToggleButtonWidgetClass, box, NULL);
						XtAddCallback (button -> toggle, XmNvalueChangedCallback, cb_optionChanged, (XtPointer) field);
					#endif
				}
				#if gtk
					GuiObject_show (field -> cascadeButton);
				#elif motif
					GuiObject_show (bar);
				#endif
			} break;
			case UI_BOOLEAN:
			{
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				/*GuiLabel_createShown (form, x, x + labelWidth, y, y + Gui_CHECKBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT); */
				field -> toggle = GuiCheckButton_createShown (form,
					fieldX, dialogWidth /* allow to extend into the margin */, y, Gui_AUTOMATIC,
					theFinishBuffer.string, NULL, NULL, 0);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (field -> toggle), 1, 3, row, row + 1);
					row += 1;
				#endif
			} break;
			case UI_LIST:
			{
				int listWidth = my numberOfFields == 1 ? dialogWidth - fieldX : fieldWidth;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiObject label = GuiLabel_createShown (form, x, x + labelWidth, y + 1, y + 21,
					theFinishBuffer.string, GuiLabel_RIGHT);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), GTK_WIDGET (label), 0, 1, row, row + 1);
				#endif
				field -> list = GuiList_create (form, fieldX, fieldX + listWidth, y, y + LIST_HEIGHT, false, theFinishBuffer.string);
				for (long i = 1; i <= field -> numberOfStrings; i ++) {
					GuiList_insertItem (field -> list, field -> strings [i], 0);
				}
				GuiObject_show (field -> list);
				#if gtk
					gtk_table_attach_defaults (GTK_TABLE (form), gtk_widget_get_parent (GTK_WIDGET (field -> list)), 1, 3, row, row + 1);
					row += 1;
				#endif
			} break;
		}
	}
	for (long ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
	/*separator = XmCreateSeparatorGadget (column, "separator", NULL, 0);*/
	y = dialogHeight - Gui_BOTTOM_DIALOG_SPACING - Gui_PUSHBUTTON_HEIGHT;
	if (my helpTitle) {
		my helpButton = GuiButton_createShown (buttons, HELP_BUTTON_X, HELP_BUTTON_X + HELP_BUTTON_WIDTH, y, Gui_AUTOMATIC,
			L"Help", gui_button_cb_help, me, 0);
		#if gtk
			gtk_button_box_set_child_secondary (GTK_BUTTON_BOX (buttons), GTK_WIDGET (my helpButton), TRUE);
		#endif
	}
	if (my numberOfFields > 1 || (my numberOfFields > 0 && my field [1] -> type != UI_LABEL)) {
		if (my isPauseForm) {
			my revertButton = GuiButton_createShown (buttons,
				HELP_BUTTON_X, HELP_BUTTON_X + REVERT_BUTTON_WIDTH,
				y, Gui_AUTOMATIC, L"Revert", gui_button_cb_revert, me, 0);
		} else {
			my revertButton = GuiButton_createShown (buttons,
				HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING,
				HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING + STANDARDS_BUTTON_WIDTH,
				y, Gui_AUTOMATIC, L"Standards", gui_button_cb_revert, me, 0);
		}
		#if gtk
			gtk_button_box_set_child_secondary (GTK_BUTTON_BOX (buttons), GTK_WIDGET (my revertButton), TRUE);
		#endif
	}
	if (my isPauseForm) {
		x = HELP_BUTTON_X + REVERT_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING;
		if (my cancelContinueButton == 0) {
			my cancelButton = GuiButton_createShown (buttons, x, x + STOP_BUTTON_WIDTH, y, Gui_AUTOMATIC,
				L"Stop", gui_button_cb_cancel, me, GuiButton_CANCEL);
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
			my continueButtons [i] = GuiButton_createShown (buttons, x, x + continueButtonWidth, y, Gui_AUTOMATIC,
				my continueTexts [i], gui_button_cb_ok, me, i == my defaultContinueButton ? GuiButton_DEFAULT : 0);
		}
	} else {
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - 2 * Gui_HORIZONTAL_DIALOG_SPACING
			 - Gui_APPLY_BUTTON_WIDTH - Gui_CANCEL_BUTTON_WIDTH;
		my cancelButton = GuiButton_createShown (buttons, x, x + Gui_CANCEL_BUTTON_WIDTH, y, Gui_AUTOMATIC,
			L"Cancel", gui_button_cb_cancel, me, GuiButton_CANCEL);
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - Gui_HORIZONTAL_DIALOG_SPACING - Gui_APPLY_BUTTON_WIDTH;
		if (my numberOfFields > 1 || my field [1] -> type != UI_LABEL) {
			my applyButton = GuiButton_createShown (buttons, x, x + Gui_APPLY_BUTTON_WIDTH, y, Gui_AUTOMATIC,
				L"Apply", gui_button_cb_apply, me, 0);
		}
		x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH;
		my okButton = GuiButton_createShown (buttons, x, x + Gui_OK_BUTTON_WIDTH, y, Gui_AUTOMATIC,
			my isPauseForm ? L"Continue" : L"OK", gui_button_cb_ok, me, GuiButton_DEFAULT);
	}
	/*GuiObject_show (separator);*/
}

void UiForm_destroyWhenUnmanaged (I) {
	iam (UiForm);
	my destroyWhenUnmanaged = true;
}

void UiForm_do (I, bool modified) {
	iam (UiForm);
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	/* Prevent double callbacks: */
	/*XtRemoveCallback (my okButton, XmNactivateCallback, UiForm_ok, (XtPointer) me);*/
	/* This is the only place where this callback is installed. Moved from UiForm_close ppgb950613. */
	/*XtAddCallback (my okButton, XmNactivateCallback, UiForm_ok, (XtPointer) me);*/
	GuiObject_show (my d_dialogForm);
	if (modified)
		UiForm_okOrApply (me, NULL, true);
}

void UiForm_parseString (I, const wchar *arguments, Interpreter interpreter) {
	iam (UiForm);
	int size = my numberOfFields;
	while (size >= 1 && my field [size] -> type == UI_LABEL)
		size --;   /* Ignore trailing fields without a value. */
	for (int i = 1; i < size; i ++) {
		static wchar_t stringValue [3000];
		int ichar = 0;
		if (my field [i] -> type == UI_LABEL)
			continue;   /* Ignore non-trailing fields without a value. */
		/*
		 * Skip spaces until next argument.
		 */
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		/*
		 * The argument is everything up to the next space, or, if that starts with a double quote,
		 * everything between this quote and the matching double quote;
		 * in this case, the argument can represent a double quote by a sequence of two double quotes.
		 * Example: the string
		 *     "I said ""hello"""
		 * will be passed to the dialog as a single argument containing the text
		 *     I said "hello"
		 */
		if (*arguments == '\"') {
			arguments ++;   /* Do not include leading double quote. */
			for (;;) {
				if (*arguments == '\0')
					Melder_throw ("Missing matching quote.");
				if (*arguments == '\"' && * ++ arguments != '\"') break;   // remember second quote
				stringValue [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != ' ' && *arguments != '\t' && *arguments != '\0')
				stringValue [ichar ++] = *arguments ++;
		}
		stringValue [ichar] = '\0';   // trailing null byte
		try {
			UiField_stringToValue (my field [i], stringValue, interpreter);
		} catch (MelderError) {
			Melder_throw ("Don't understand contents of field \"", my field [i] -> name, "\".");
		}
	}
	/*
	 * The last item is handled separately, because it consists of the rest of the line.
	 * Leading spaces are skipped, but trailing spaces are included.
	 */
	if (size > 0) {
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		try {
			UiField_stringToValue (my field [size], arguments, interpreter);
		} catch (MelderError) {
			Melder_throw ("Don't understand contents of field \"", my field [size] -> name, "\".");
		}
	}
	my okCallback (me, NULL, interpreter, NULL, false, my buttonClosure); therror
}

void UiForm_parseStringE (EditorCommand cmd, const wchar *arguments, Interpreter interpreter) {
	UiForm_parseString (cmd -> d_uiform, arguments, interpreter);
}

static UiField findField (UiForm me, const wchar *fieldName) {
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
		if (wcsequ (fieldName, my field [ifield] -> name)) return my field [ifield];
	return NULL;
}

static void fatalField (UiForm dia) {
	Melder_fatal ("Wrong field in command window \"%s\".", Melder_peekWcsToUtf8 (dia -> name));
}

void UiForm_setReal (I, const wchar_t *fieldName, double value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_setReal:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
			if (value == Melder_atof (field -> stringDefaultValue)) {
				GuiText_setString (field -> text, field -> stringDefaultValue);
			} else {
				wchar_t s [40];
				wcscpy (s, Melder_double (value));
				/*
				 * If the default is overtly real, the shown value must be as well.
				 */
				if ((wcschr (field -> stringDefaultValue, '.') || wcschr (field -> stringDefaultValue, 'e')) &&
					! (wcschr (s, '.') || wcschr (s, 'e')))
				{
					wcscat (s, L".0");
				}
				GuiText_setString (field -> text, s);
			}
		} break; case UI_COLOUR: {
			GuiText_setString (field -> text, Melder_double (value));   // some grey value
		} break; default: {
			Melder_fatal ("Wrong field in command window \"%s\".", Melder_peekWcsToUtf8 (my name));
		}
	}
}

void UiForm_setInteger (I, const wchar_t *fieldName, long value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_setInteger:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: {
			if (value == wcstol (field -> stringDefaultValue, NULL, 10)) {
				GuiText_setString (field -> text, field -> stringDefaultValue);
			} else {
				GuiText_setString (field -> text, Melder_integer (value));
			}
		} break; case UI_BOOLEAN: {
			GuiCheckButton_setValue (field -> toggle, value);
		} break; case UI_RADIO: {
			if (value < 1 || value > field -> options -> size) value = 1;   /* Guard against incorrect prefs file. */
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = static_cast <UiOption> (field -> options -> item [i]);
				GuiRadioButton_setValue (b -> toggle, i == value);
			}
		} break; case UI_OPTIONMENU: {
			if (value < 1 || value > field -> options -> size) value = 1;   /* Guard against incorrect prefs file. */
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = static_cast <UiOption> (field -> options -> item [i]);
				#if motif
				XmToggleButtonSetState (b -> toggle, i == value, False);
				if (i == value) {
					XtVaSetValues (field -> cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
				}
				#endif
			}
		} break; case UI_LIST: {
			if (value < 1 || value > field -> numberOfStrings) value = 1;   /* Guard against incorrect prefs file. */
			GuiList_selectItem (field -> list, value);
		} break; default: {
			fatalField (me);
		}
	}
}

void UiForm_setString (I, const wchar *fieldName, const wchar *value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_setString:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	if (value == NULL) value = L"";   /* Accept NULL strings. */
	switch (field -> type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: case UI_INTEGER: case UI_NATURAL:
			case UI_WORD: case UI_SENTENCE: case UI_COLOUR: case UI_CHANNEL: case UI_TEXT:
		{
			GuiText_setString (field -> text, value);
		} break; case UI_LABEL: {
			GuiLabel_setString (field -> text, value);
		} break; case UI_RADIO: {
			bool found = false;
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = static_cast <UiOption> (field -> options -> item [i]);
				if (wcsequ (value, b -> name)) {
					GuiRadioButton_setValue (b -> toggle, true);
					found = true;
				} else {
					GuiRadioButton_setValue (b -> toggle, false);
				}
			}
			/* If not found: do nothing (guard against incorrect prefs file). */
		} break; case UI_OPTIONMENU: {
			bool found = false;
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = static_cast <UiOption> (field -> options -> item [i]);
				if (wcsequ (value, b -> name)) {
					#if motif
					XmToggleButtonSetState (b -> toggle, True, False);
					#endif
					found = true;
					if (field -> type == UI_OPTIONMENU) {
						#if motif
						XtVaSetValues (field -> cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (value)), NULL);
						#endif
					}
				} else {
					#if motif
						XmToggleButtonSetState (b -> toggle, False, False);
					#endif
				}
			}
			/* If not found: do nothing (guard against incorrect prefs file). */
		} break; case UI_LIST: {
			long i;
			for (i = 1; i <= field -> numberOfStrings; i ++)
				if (wcsequ (value, field -> strings [i])) break;
			if (i > field -> numberOfStrings) i = 1;   /* Guard against incorrect prefs file. */
			GuiList_selectItem (field -> list, i);
		} break; default: {
			fatalField (me);
		}
	}
}

static UiField findField_check (UiForm me, const wchar *fieldName) {
	UiField result = findField (me, fieldName);
	if (result == NULL) {
		Melder_throw ("Cannot find field \"", fieldName, "\" in form.\n"
			"The script may have changed while the form was open.\n"
			"Please click Cancel in the form and try again.");
	}
	return result;
}

double UiForm_getReal (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_getReal:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
			return field -> realValue;
		} break; default: {
			fatalField (me);
		}
	}
	return 0.0;
}

double UiForm_getReal_check (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName);
	switch (field -> type) {
		case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
			return field -> realValue;
		} break; default: {
			Melder_throw ("Cannot find a real value in field \"", fieldName, "\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
	return 0.0;
}

long UiForm_getInteger (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_getInteger:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: case UI_BOOLEAN: case UI_RADIO:
			case UI_OPTIONMENU: case UI_LIST:
		{
			return field -> integerValue;
		} break; default: {
			fatalField (me);
		}
	}
	return 0L;
}

long UiForm_getInteger_check (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName);
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: case UI_BOOLEAN: case UI_RADIO:
			case UI_OPTIONMENU: case UI_LIST:
		{
			return field -> integerValue;
		} break; default: {
			Melder_throw ("Cannot find an integer value in field \"", fieldName, "\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
	return 0L;
}

wchar * UiForm_getString (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_getString:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			return field -> stringValue;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = static_cast <UiOption> (field -> options -> item [field -> integerValue]);
			return b -> name;
		} break; case UI_LIST: {
			return (wchar_t *) field -> strings [field -> integerValue];
		} break; default: {
			fatalField (me);
		}
	}
	return NULL;
}

wchar * UiForm_getString_check (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName);
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			return field -> stringValue;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = static_cast <UiOption> (field -> options -> item [field -> integerValue]);
			return b -> name;
		} break; case UI_LIST: {
			return (wchar_t *) field -> strings [field -> integerValue];
		} break; default: {
			Melder_throw ("Cannot find a string in field \"", fieldName, "\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
	return NULL;
}

Graphics_Colour UiForm_getColour (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (field == NULL) Melder_fatal ("(UiForm_getColour:) No field \"%s\" in command window \"%s\".",
		Melder_peekWcsToUtf8 (fieldName), Melder_peekWcsToUtf8 (my name));
	switch (field -> type) {
		case UI_COLOUR: {
			return field -> colourValue;
		} break; default: {
			fatalField (me);
		}
	}
	return Graphics_BLACK;
}

Graphics_Colour UiForm_getColour_check (I, const wchar *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName);
	switch (field -> type) {
		case UI_COLOUR: {
			return field -> colourValue;
		} break; default: {
			Melder_throw ("Cannot find a real value in field \"", fieldName, "\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
	return Graphics_BLACK;
}

void UiForm_Interpreter_addVariables (I, Interpreter interpreter) {
	iam (UiForm);
	static MelderString lowerCaseFieldName = { 0 };
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		UiField field = my field [ifield];
		MelderString_copy (& lowerCaseFieldName, field -> name);
		/*
		 * Change e.g. "Number of people" to "number_of_people".
		 */
		lowerCaseFieldName.string [0] = towlower (lowerCaseFieldName.string [0]);
		for (wchar_t *p = & lowerCaseFieldName.string [0]; *p != '\0'; p ++) {
			if (*p == ' ') *p = '_';
		}
		switch (field -> type) {
			case UI_INTEGER: case UI_NATURAL: case UI_CHANNEL: case UI_BOOLEAN: {
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				var -> numericValue = field -> integerValue;
			} break; case UI_REAL: case UI_REAL_OR_UNDEFINED: case UI_POSITIVE: {
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				var -> numericValue = field -> realValue;
			} break; case UI_RADIO: case UI_OPTIONMENU: {
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				var -> numericValue = field -> integerValue;
				MelderString_appendCharacter (& lowerCaseFieldName, '$');
				var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				Melder_free (var -> stringValue);
				UiOption b = static_cast <UiOption> (field -> options -> item [field -> integerValue]);
				var -> stringValue = Melder_wcsdup_f (b -> name);
			} break; case UI_LIST: {
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				var -> numericValue = field -> integerValue;
				MelderString_appendCharacter (& lowerCaseFieldName, '$');
				var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				Melder_free (var -> stringValue);
				var -> stringValue = Melder_wcsdup_f ((wchar *) field -> strings [field -> integerValue]);
			} break; case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
				MelderString_appendCharacter (& lowerCaseFieldName, '$');
				InterpreterVariable var = Interpreter_lookUpVariable (interpreter, lowerCaseFieldName.string); therror
				Melder_free (var -> stringValue);
				var -> stringValue = Melder_wcsdup_f (field -> stringValue);
			} break; case UI_COLOUR: {
				// to be implemented
			} break; default: {
			}
		}
	}
}

int UiForm_getClickedContinueButton (UiForm me) {
	return my clickedContinueButton;
}

/* End of file Ui.cpp */
