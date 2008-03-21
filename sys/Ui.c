/* Ui.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2002/03/30 less object-oriented
 * pb 2003/03/09 simplified calls of numeric expressions
 * pb 2003/05/19 Melder_atof
 * pb 2003/09/16 "standards" rather than "defaults"
 * pb 2004/05/09 no colons after ellipses
 * pb 2004/11/16 added Apply button
 * pb 2004/12/06 UiForm_getXXX_check
 * pb 2004/12/14 less space after LABEL unless its value is empty or ends in a period
 * pb 2005/03/06 guard against incorrect prefs files
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2007/03/23 new Editor API
 * pb 2007/04/23 moved allowExecutionHook installation from UiForm_finish to UiForm_do,
 *   so that scripts cannot prevent this hook from working
 * pb 2007/06/09 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/12/30 Gui
 */

#include <ctype.h>
#include "longchar.h"
#include "machine.h"
#include "Gui.h"
#include "Collection.h"
#include "UiP.h"
#include "Editor.h"
#include "Graphics.h"   /* colours. */

static Interpreter theCurrentInterpreter;
void UiInterpreter_set (Interpreter interpreter) { theCurrentInterpreter = interpreter; }
Interpreter UiInterpreter_get (void) { return theCurrentInterpreter; }

#define MAXIMUM_NUMBER_OF_FIELDS  30

/***** class UiField: the things that have values in an UiForm dialog *****/

/* Values for 'type'. */
#define UI_REAL  1
#define UI_POSITIVE  2
#define UI_INTEGER  3
#define UI_NATURAL  4
#define UI_WORD  5
#define UI_SENTENCE  6
#define UI_COLOUR  7
	#define UI_LABELLEDTEXT_MIN  UI_REAL
	#define UI_LABELLEDTEXT_MAX  UI_COLOUR
#define UI_LABEL  8
#define UI_TEXT  9
#define UI_BOOLEAN  10
#define UI_RADIO  11
#define UI_OPTIONMENU  12
#define UI_ENUM  13
#define UI_LIST  14

#define UiField_members Thing_members \
	int type; \
	const wchar_t *formLabel; \
	double realValue, realDefaultValue; \
	long integerValue, integerDefaultValue; \
	wchar_t *stringValue; const wchar_t *stringDefaultValue; \
	char *stringValueA; \
	Ordered options; \
	void *enumerated; \
	int includeZero; \
	long numberOfStrings; \
	const wchar_t **strings; \
	Widget text, toggle, list, cascadeButton; \
	int y;
#define UiField_methods Thing_methods
class_create (UiField, Thing);

static void classUiField_destroy (I) {
	iam (UiField);
	Melder_free (my formLabel);
	Melder_free (my stringValue);
	Melder_free (my stringValueA);
	Melder_free (my stringDefaultValue);
	forget (my options);
	inherited (UiField) destroy (me);
}

class_methods (UiField, Thing) {
	class_method_local (UiField, destroy)
class_methods_end }

static UiField UiField_create (int type, const wchar_t *name) {
	UiField me = new (UiField);
	wchar_t shortName [101], *p;
	my type = type;
	my formLabel = Melder_wcsdup (name);
	wcscpy (shortName, name);
	/*
	 * Strip parentheses and colon off parameter name.
	 */
	p = wcschr (shortName, ':');   /*  */
	if (p) *p = '\0';
	if ((p = wcschr (shortName, '(')) != NULL) {
		*p = '\0';
		if (p - shortName > 0 && p [-1] == ' ') p [-1] = '\0';
	}
	p = shortName;
	if (*p != '\0' && p [wcslen (p) - 1] == ':') p [wcslen (p) - 1] = '\0';
	Thing_setName (me, shortName);
	return me;
}

/***** class UiOption: radio buttons and menu options *****/

#define UiOption_members Thing_members \
	Widget toggle;
#define UiOption_methods Thing_methods
class_create (UiOption, Thing);

class_methods (UiOption, Thing) {
class_methods_end }

static Any UiOption_create (const wchar_t *label) {
	UiOption me = new (UiOption);
	Thing_setName (me, label);
	return me;
}

static void gui_radiobutton_cb_toggled (I, GuiRadioButtonEvent event) {
	#if !gtk
	iam (UiField);
	for (int i = 1; i <= my options -> size; i ++) {
		UiOption b = my options -> item [i];
		GuiRadioButton_setValue (b -> toggle, b -> toggle == event -> toggle);
	}
	#endif
}

Any UiRadio_addButton (I, const wchar_t *label) {
	iam (UiField);
	UiOption thee;
	if (me == NULL) return NULL;
	Melder_assert (my type == UI_RADIO || my type == UI_OPTIONMENU);
	thee = UiOption_create (label);
	Collection_addItem (my options, thee);
	return thee;
}

static void cb_optionChanged (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiField);
	int i;
	(void) call;
	for (i = 1; i <= my options -> size; i ++) {
		UiOption b = my options -> item [i];
		#if motif
			// TODO
		if (b -> toggle == w) {
			XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
			XmToggleButtonSetState (b -> toggle, TRUE, FALSE);
			if (Melder_debug == 11) {
				Melder_warning4 (Melder_integer (i), L" \"", b -> name, L"\"");
			}
		} else {
			XmToggleButtonSetState (b -> toggle, FALSE, FALSE);
		}
		#endif
	}
}

Any UiOptionMenu_addButton (I, const wchar_t *label) {
	iam (UiField);
	UiOption thee;
	if (me == NULL) return NULL;
	Melder_assert (my type == UI_RADIO || my type == UI_OPTIONMENU);
	thee = UiOption_create (label);
	Collection_addItem (my options, thee);
	return thee;
}

/***** Things to do with UiField objects. *****/

static void UiField_setDefault (UiField me) {
	switch (my type) {
		case UI_REAL: case UI_POSITIVE: case UI_INTEGER: case UI_NATURAL:
			case UI_WORD: case UI_SENTENCE: case UI_COLOUR: case UI_TEXT:
		{
			GuiText_setString (my text, my stringDefaultValue);
		} break; case UI_BOOLEAN: {
			GuiCheckButton_setValue (my toggle, my integerDefaultValue);
		} break; case UI_RADIO: {
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				GuiRadioButton_setValue (b -> toggle, i == my integerDefaultValue);
			}
		} break; case UI_OPTIONMENU: {
			for (int i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				#if motif
				// TODO
				XmToggleButtonSetState (b -> toggle, i == my integerDefaultValue, False);
				if (i == my integerDefaultValue) {
					XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
				}
				#endif
			}
		} break; case UI_ENUM: {
			GuiList_selectItem (my list, my integerDefaultValue + my includeZero);
		} break; case UI_LIST: {
			GuiList_selectItem (my list, my integerDefaultValue);
		}
	}
}

static int colourToValue (UiField me, wchar_t *string) {
	wchar_t *p = string;
	int first;
	while (*p == ' ' || *p == '\t') p ++;
	first = *p;
	*p = tolower (*p);
	if (wcsequ (p, L"black")) my realValue = Graphics_BLACK;
	else if (wcsequ (p, L"white")) my realValue = Graphics_WHITE;
	else if (wcsequ (p, L"red")) my realValue = Graphics_RED;
	else if (wcsequ (p, L"green")) my realValue = Graphics_GREEN;
	else if (wcsequ (p, L"blue")) my realValue = Graphics_BLUE;
	else if (wcsequ (p, L"yellow")) my realValue = Graphics_YELLOW;
	else if (wcsequ (p, L"cyan")) my realValue = Graphics_CYAN;
	else if (wcsequ (p, L"magenta")) my realValue = Graphics_MAGENTA;
	else if (wcsequ (p, L"maroon")) my realValue = Graphics_MAROON;
	else if (wcsequ (p, L"lime")) my realValue = Graphics_LIME;
	else if (wcsequ (p, L"navy")) my realValue = Graphics_NAVY;
	else if (wcsequ (p, L"teal")) my realValue = Graphics_TEAL;
	else if (wcsequ (p, L"purple")) my realValue = Graphics_PURPLE;
	else if (wcsequ (p, L"olive")) my realValue = Graphics_OLIVE;
	else if (wcsequ (p, L"silver")) my realValue = Graphics_SILVER;
	else if (wcsequ (p, L"grey")) my realValue = Graphics_GREY;
	else { *p = first; return 0; }
	*p = first;
	return 1;
}

static int UiField_widgetToValue (UiField me) {
	switch (my type) {
		case UI_REAL: case UI_POSITIVE: {
			wchar_t *dirty = GuiText_getString (my text);   /* The text as typed by the user. */
			if (! Interpreter_numericExpression (NULL, dirty, & my realValue)) { Melder_free (dirty); return 0; }
			Melder_free (dirty);
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
			if (my type == UI_POSITIVE && my realValue <= 0.0)
				return Melder_error3 (L"`", my name, L"' must be greater than 0.0.");
		} break; case UI_INTEGER: case UI_NATURAL: {
			wchar_t *dirty = GuiText_getString (my text);
			double realValue;
			if (! Interpreter_numericExpression (NULL, dirty, & realValue)) { Melder_free (dirty); return 0; }
			Melder_free (dirty);
			my integerValue = floor (realValue + 0.5);
			if (my integerValue == wcstol (my stringDefaultValue, NULL, 10)) {
				GuiText_setString (my text, my stringDefaultValue);
			} else {
				GuiText_setString (my text, Melder_integer (my integerValue));
			}
			if (my type == UI_NATURAL && my integerValue < 1)
				return Melder_error3 (L"`", my name, L"' must be a positive whole number.");
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
			int i;
			my integerValue = 0;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				if (GuiRadioButton_getValue (b -> toggle))
					my integerValue = i;
			}
			if (my integerValue == 0)
				return Melder_error3 (L"No option chosen for `", my name, L"'.");
		} break; case UI_OPTIONMENU: {
			int i;
			my integerValue = 0;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				#if motif
				// TODO
				if (XmToggleButtonGetState (b -> toggle))
					my integerValue = i;
				#endif
			}
			if (my integerValue == 0)
				return Melder_error3 (L"No option chosen for `", my name, L"'.");
		} break; case UI_ENUM: case UI_LIST: {
			long numberOfSelected, *selected = GuiList_getSelectedPositions (my list, & numberOfSelected);
			if (selected == NULL) {
				Melder_warning1 (L"No items selected.");
				my integerValue = 1;
			} else {
				if (numberOfSelected > 1) Melder_warning1 (L"More than one item selected.");
				my integerValue = selected [1];
				NUMlvector_free (selected, 1);
			}
			if (my type == UI_ENUM && my includeZero)
				my integerValue -= 1;
		} break; case UI_COLOUR: {
			wchar_t *string = GuiText_getString (my text);
			if (colourToValue (me, string))
				;
			else if (! Interpreter_numericExpression (NULL, string, & my realValue)) { Melder_free (string); return 0; }
			Melder_free (string);
		}
	}
	return 1;
}

static wchar_t *colourNames [] = { L"black", L"white", L"red", L"green", L"blue", L"cyan", L"magenta", L"yellow",
	L"maroon", L"lime", L"navy", L"teal", L"purple", L"olive", L"silver", L"grey" };

static int UiField_stringToValue (UiField me, const wchar_t *string) {
	switch (my type) {
		case UI_REAL: case UI_POSITIVE: {
			if (wcsspn (string, L" \t") == wcslen (string))
				return Melder_error3 (L"Argument `", my name, L"' empty.");
			if (! Interpreter_numericExpression (theCurrentInterpreter, string, & my realValue)) return 0;
			if (my type == UI_POSITIVE && my realValue <= 0.0)
				return Melder_error3 (L"`", my name, L"' must be greater than 0.");
		} break; case UI_INTEGER: case UI_NATURAL: {
			double realValue;
			if (wcsspn (string, L" \t") == wcslen (string))
				return Melder_error3 (L"Argument `", my name, L"' empty.");
			if (! Interpreter_numericExpression (theCurrentInterpreter, string, & realValue)) return 0;
			my integerValue = floor (realValue + 0.5);
			if (my type == UI_NATURAL && my integerValue < 1)
				return Melder_error3 (L"`", my name, L"' must be a positive whole number.");
		} break; case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			Melder_free (my stringValue);
			my stringValue = Melder_wcsdup (string);
		} break; case UI_BOOLEAN: {
			if (! string [0]) return Melder_error1 (L"Empty argument for toggle button.");
			my integerValue = string [0] == '1' || string [0] == 'y' || string [0] == 'Y' ||
				string [0] == 't' || string [0] == 'T';
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			int i;
			my integerValue = 0;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				if (wcsequ (string, b -> name))
					my integerValue = i;
			}
			if (my integerValue == 0) {
				/*
				 * Retry with different case.
				 */
				for (i = 1; i <= my options -> size; i ++) {
					UiOption b = my options -> item [i];
					wchar_t name2 [100];
					wcscpy (name2, b -> name);
					if (islower (name2 [0])) name2 [0] = toupper (name2 [0]);
					else if (isupper (name2 [0])) name2 [0] = tolower (name2 [0]);
					if (wcsequ (string, name2))
						my integerValue = i;
				}
			}
			if (my integerValue == 0) {
				return Melder_error5
					(L"Field `", my name, L"' cannot have the value \"", string, L"\".");
			}
		} break; case UI_ENUM: {
			my integerValue = enum_search (my enumerated, string);
			if (my integerValue < 0) return Melder_error5
				(L"Field `", my name, L"' cannot have the value \"", string, L"\".");
		} break; case UI_LIST: {
			long i;
			for (i = 1; i <= my numberOfStrings; i ++)
				if (wcsequ (string, my strings [i])) break;
			if (i > my numberOfStrings) return Melder_error5
				(L"Field `", my name, L"' cannot have the value \"", string, L"\".");
			my integerValue = i;
		} break; case UI_COLOUR: {
			wchar_t *string2 = Melder_wcsdup (string);
			if (colourToValue (me, string2)) {
				;
			} else if (! Interpreter_numericExpression (theCurrentInterpreter, string2, & my realValue)) {
				Melder_free (string2);
				return 0;
			}
			Melder_free (string2);
		} break; default: {
			return 0;
		}
	}
	return 1;
}

static void UiField_valueToHistory (UiField me, int isLast) {
	UiHistory_write (L" ");
	switch (my type) {
		case UI_REAL: case UI_POSITIVE: {
			UiHistory_write (Melder_double (my realValue));
		} break; case UI_INTEGER: case UI_NATURAL: {
			UiHistory_write (Melder_integer (my integerValue));
		} break; case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			if (isLast == FALSE && (my stringValue [0] == '\0' || wcschr (my stringValue, ' '))) {
				UiHistory_write (L"\"");
				UiHistory_write (my stringValue);
				UiHistory_write (L"\"");
			} else {
				UiHistory_write (my stringValue);
			}
		} break; case UI_BOOLEAN: {
			UiHistory_write (my integerValue ? L"yes" : L"no");
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = my options -> item [my integerValue];
			if (isLast == FALSE && (b -> name [0] == '\0' || wcschr (b -> name, ' '))) {
				UiHistory_write (L"\"");
				UiHistory_write (b -> name);
				UiHistory_write (L"\"");
			} else {
				UiHistory_write (b -> name);
			}
		} break; case UI_ENUM: {
			UiHistory_write (enum_string (my enumerated, my integerValue));
		} break; case UI_LIST: {
			if (isLast == FALSE && (my strings [my integerValue] [0] == '\0' || wcschr (my strings [my integerValue], ' '))) {
				UiHistory_write (L"\"");
				UiHistory_write (my strings [my integerValue]);
				UiHistory_write (L"\"");
			} else {
				UiHistory_write (my strings [my integerValue]);
			}
		} break; case UI_COLOUR: {
			int integerValue = floor (my realValue);
			if (integerValue != my realValue) {
				UiHistory_write (Melder_single (my realValue));
			} else if (integerValue >= 0 && integerValue <= 15) {
				UiHistory_write (colourNames [integerValue]);
			} else {
				UiHistory_write (L"black");
			}
		}
	}
}

/***** History mechanism. *****/

static MelderString theHistory = { 0 };
void UiHistory_write (const wchar_t *string) { MelderString_append (& theHistory, string); }
wchar_t *UiHistory_get (void) { return theHistory.string; }
void UiHistory_clear (void) { MelderString_empty (& theHistory); }

/***** class UiForm: dialog windows *****/

int (*theAllowExecutionHookHint) (void *closure) = NULL;
void *theAllowExecutionClosureHint = NULL;

void Ui_setAllowExecutionHook (int (*allowExecutionHook) (void *closure), void *allowExecutionClosure) {
	theAllowExecutionHookHint = allowExecutionHook;
	theAllowExecutionClosureHint = allowExecutionClosure;
}

#define UiForm_members Thing_members \
	EditorCommand command; \
	Widget parent, shell, dialog; \
	int (*okCallback) (Any dia, void *closure); \
	void *okClosure; \
	const wchar_t *helpTitle; \
	int numberOfFields; \
	UiField field [1 + MAXIMUM_NUMBER_OF_FIELDS]; \
	Widget okButton, cancelButton, useStandards, helpButton, applyButton; \
	int destroyWhenUnmanaged; \
	int (*allowExecutionHook) (void *closure); \
	void *allowExecutionClosure;
#define UiForm_methods Thing_methods
class_create (UiForm, Thing);

static void classUiForm_destroy (I) {
	iam (UiForm);
	int ifield;
	for (ifield = 1; ifield <= my numberOfFields; ifield ++)
		forget (my field [ifield]);
	if (my dialog) GuiObject_destroy (my dialog);
	Melder_free (my helpTitle);
	inherited (UiForm) destroy (me);
}

class_methods (UiForm, Thing) {
	class_method_local (UiForm, destroy)
class_methods_end }

static void gui_button_cb_useStandards (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
}

static void UiForm_hide (I) {
	iam (UiForm);
	GuiObject_hide (my dialog);
	if (my destroyWhenUnmanaged) forget (me);
}
static void gui_button_UiForm_hide (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	GuiObject_hide (my dialog);
	if (my destroyWhenUnmanaged) forget (me);
}

static void UiForm_okOrApply (I, int hide) {
	iam (UiForm);
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_error3 (L"Cannot execute dialog `", my name, L"'.");
		Melder_flushError (NULL);
		return;
	}
	for (int ifield = 1; ifield <= my numberOfFields; ifield ++) {
		if (! UiField_widgetToValue (my field [ifield])) {
			Melder_error3 (L"Please correct dialog " L_LEFT_SINGLE_QUOTE, my name, L_RIGHT_SINGLE_QUOTE L" or cancel.");
			Melder_flushError (NULL);
			return;
		}
	}
	/* In the next, w must become my okButton? */
	/*XtRemoveCallback (w, XmNactivateCallback, UiForm_ok, void_me);   /* FIX */
	GuiObject_setSensitive (my okButton, False);
	if (my applyButton) GuiObject_setSensitive (my applyButton, False);
	GuiObject_setSensitive (my cancelButton, False);
	if (my useStandards) GuiObject_setSensitive (my useStandards, False);
	if (my helpButton) GuiObject_setSensitive (my helpButton, False);
	#if motif
	XmUpdateDisplay (my dialog);
	#endif
	if (my okCallback (me, my okClosure)) {
		int destroyWhenUnmanaged = my destroyWhenUnmanaged;   /* Save before destruction. */
		/*
		 * Write everything to history. Before destruction!
		 */
		int size = my numberOfFields;
		while (size >= 1 && my field [size] -> type == UI_LABEL)
			size --;   /* Ignore trailing fields without a value. */
		for (int ifield = 1; ifield <= size; ifield ++)
			UiField_valueToHistory (my field [ifield], ifield == size);
		if (hide) {
			gui_button_UiForm_hide (me, 0);   // BUG: should not send 0
			if (destroyWhenUnmanaged) return;
		}
	} else {
		/*
		 * If a solution has already been suggested, do not add anything more.
		 */
		if (! wcsstr (Melder_getError (), L"Please ") && ! wcsstr (Melder_getError (), L"You could ")) {
			/*
			 * Otherwise, show a generic message.
			 */
			if (wcsstr (Melder_getError (), L"Selection changed!")) {
				Melder_error3 (L"Please change the selection in the object list, or click Cancel in the window `",
					my name, L"'.");
			} else {
				Melder_error3 (L"Please change something in the window `",
					my name, L"', or click Cancel in that window.");
			}
		}
		/*XtAddCallback (w, XmNactivateCallback, UiForm_ok, void_me);   /* FIX */
		Melder_flushError (NULL);
	}
	GuiObject_setSensitive (my okButton, True);
	if (my applyButton) GuiObject_setSensitive (my applyButton, True);
	GuiObject_setSensitive (my cancelButton, True);
	if (my useStandards) GuiObject_setSensitive (my useStandards, True);
	if (my helpButton) GuiObject_setSensitive (my helpButton, True);
}

static void gui_button_UiForm_ok (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	UiForm_okOrApply (me, true);
}

static void gui_button_UiForm_apply (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	UiForm_okOrApply (me, false);
}

static void gui_button_cb_help (I, GuiButtonEvent event) {
	(void) event;
	iam (UiForm);
	Melder_help (my helpTitle);
}

Any UiForm_create (Widget parent, const wchar_t *title,
	int (*okCallback) (Any dia, void *closure), void *okClosure,
	const wchar_t *helpTitle)
{
	UiForm me = new (UiForm);
	my parent = parent;
	Thing_setName (me, title);
	my helpTitle = Melder_wcsdup (helpTitle);
	my okCallback = okCallback;
	my okClosure = okClosure;
	return me;
}

static int commonOkCallback (Any dia, void *closure) {
	EditorCommand cmd = (EditorCommand) closure;
	(void) dia;
	return cmd -> commandCallback (cmd -> editor, cmd, cmd -> dialog);
}

Any UiForm_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *helpTitle) {
	Editor editor = (Editor) cmd -> editor;
	UiForm dia = UiForm_create (editor -> dialog, title, commonOkCallback, cmd, helpTitle);
	dia -> command = cmd;
	return dia;
}

static UiField UiForm_addField (UiForm me, int type, const wchar_t *label) {
	if (my numberOfFields == MAXIMUM_NUMBER_OF_FIELDS) return NULL;
	return my field [++ my numberOfFields] = UiField_create (type, label);
}

Any UiForm_addReal (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_REAL, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addPositive (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_POSITIVE, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addInteger (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_INTEGER, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addNatural (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_NATURAL, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addWord (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_WORD, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addSentence (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_SENTENCE, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addLabel (I, const wchar_t *name, const wchar_t *label) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_LABEL, name);
	if (thee == NULL) return NULL;
	thy stringValue = Melder_wcsdup (label);
	return thee;
}

Any UiForm_addBoolean (I, const wchar_t *label, int defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_BOOLEAN, label);
	if (thee == NULL) return NULL;
	thy integerDefaultValue = defaultValue;
	return thee;
}

Any UiForm_addText (I, const wchar_t *name, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_TEXT, name);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Any UiForm_addRadio (I, const wchar_t *label, int defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_RADIO, label);
	if (thee == NULL) return NULL;
	thy integerDefaultValue = defaultValue;
	thy options = Ordered_create ();
	return thee;
}

Any UiForm_addOptionMenu (I, const wchar_t *label, int defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_OPTIONMENU, label);
	if (thee == NULL) return NULL;
	thy integerDefaultValue = defaultValue;
	thy options = Ordered_create ();
	return thee;
}

Any UiForm_addEnum (I, const wchar_t *label, void *enumerated, int defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_ENUM, label);
	if (thee == NULL) return NULL;
	thy enumerated = enumerated;
	thy integerDefaultValue = defaultValue;
	thy includeZero = enum_string (enumerated, 0) [0] != '_';
	return thee;
}

Any UiForm_addList (I, const wchar_t *label, long numberOfStrings, const wchar_t **strings, long defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_LIST, label);
	if (thee == NULL) return NULL;
	thy numberOfStrings = numberOfStrings;
	thy strings = strings;
	thy integerDefaultValue = defaultValue;
	return thee;
}

Any UiForm_addColour (I, const wchar_t *label, const wchar_t *defaultValue) {
	iam (UiForm);
	UiField thee = UiForm_addField (me, UI_COLOUR, label);
	if (thee == NULL) return NULL;
	thy stringDefaultValue = Melder_wcsdup (defaultValue);
	return thee;
}

Widget Gui_addRadioButton (Widget parent, const char *title, int x1, int x2, int y1, int y2, unsigned long flags);

#define DIALOG_X  150
#define DIALOG_Y  70
#define HELP_BUTTON_WIDTH  60
#define DEF_BUTTON_WIDTH  100
#define HELP_BUTTON_X  20
#define DEF_BUTTON_X  100
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
	int size = my numberOfFields;
	int dialogHeight = 0, x = Gui_LEFT_DIALOG_SPACING, y;
	int textFieldHeight = Gui_TEXTFIELD_HEIGHT;
	int dialogWidth = 480, dialogCentre = dialogWidth / 2, fieldX = dialogCentre + Gui_LABEL_SPACING / 2;
	int labelWidth = fieldX - Gui_LABEL_SPACING - x, fieldWidth = labelWidth, halfFieldWidth = fieldWidth / 2 - 6;

	#if gtk
		Widget form, buttons;
	#else
		Widget form, buttons; // Define?
	#endif

	if (! my parent) return;
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
			thy type == UI_ENUM || thy type == UI_LIST ? LIST_HEIGHT :
			thy type == UI_LABEL && thy stringValue [0] != '\0' && thy stringValue [wcslen (thy stringValue) - 1] != '.' &&
				ifield != my numberOfFields ? textFieldHeight
				#ifdef _WIN32
					- 6 :
				#else
					- 10 :
				#endif
			textFieldHeight;
	}
	dialogHeight += 2 * Gui_BOTTOM_DIALOG_SPACING + Gui_PUSHBUTTON_HEIGHT;
	my dialog = GuiDialog_create (my parent, DIALOG_X, DIALOG_Y, dialogWidth, dialogHeight, my name, UiForm_hide, me, 0);
	
	#if gtk
		form = GTK_DIALOG(my dialog) -> vbox;
		buttons = GTK_DIALOG(my dialog) -> action_area;
	#else
		form = my dialog;
		buttons = my dialog;
	#endif

	for (long ifield = 1; ifield <= size; ifield ++) {
		UiField field = my field [ifield];
		y = field -> y;
		switch (field -> type) {
			case UI_REAL:
			case UI_POSITIVE:
			case UI_INTEGER:
			case UI_NATURAL:
			case UI_WORD:
			case UI_SENTENCE:
			case UI_COLOUR:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 3;
				#endif
				if (wcsnequ (field -> name, L"left ", 5)) {
					MelderString_copy (& theFinishBuffer, field -> formLabel + 5);
					appendColon ();
					GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					field -> text = GuiText_createShown (form, fieldX, fieldX + halfFieldWidth, y, Gui_AUTOMATIC, 0);
				} else if (wcsnequ (field -> name, L"right ", 6)) {
					field -> text = GuiText_createShown (form, fieldX + halfFieldWidth + 12, fieldX + fieldWidth,
						y, Gui_AUTOMATIC, 0);
				} else {
					MelderString_copy (& theFinishBuffer, field -> formLabel);
					appendColon ();
					GuiLabel_createShown (form, x, x + labelWidth,
						ylabel, ylabel + textFieldHeight,
						theFinishBuffer.string, GuiLabel_RIGHT);
					field -> text = GuiText_createShown (form, fieldX, fieldX + fieldWidth, // or once the dialog is a Form: - Gui_RIGHT_DIALOG_SPACING,
						y, Gui_AUTOMATIC, 0);
				}
			} break;
			case UI_TEXT:
			{
				field -> text = GuiText_createShown (form, x, x + dialogWidth - Gui_LEFT_DIALOG_SPACING - Gui_RIGHT_DIALOG_SPACING,
					y, Gui_AUTOMATIC, 0);
			} break;
			case UI_LABEL:
			{
				MelderString_copy (& theFinishBuffer, field -> stringValue);
				field -> text = GuiLabel_createShown (form,
					x, dialogWidth /* allow to extend into the margin */, y + 5, y + 5 + textFieldHeight,
					theFinishBuffer.string, 0);
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
				GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_RADIOBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);
				for (long ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = field -> options -> item [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					button -> toggle = GuiRadioButton_createShown (form,
						fieldX, dialogWidth /* allow to extend into the margin */,
						y + (ibutton - 1) * (Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING), Gui_AUTOMATIC,
						theFinishBuffer.string, gui_radiobutton_cb_toggled, field, 0);
					#if gtk
					if (group != NULL) {
						GuiRadioButton_setGroup(button -> toggle, group);
					} 
					group = GuiRadioButton_getGroup(button -> toggle);
					#endif
				}
			} break; 
			case UI_OPTIONMENU:
			{
				int ylabel = y;
				#if defined (macintosh)
					ylabel += 2;
				#endif
				Widget bar, box;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiLabel_createShown (form, x, x + labelWidth, ylabel, ylabel + Gui_OPTIONMENU_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT);

				// TODO
				#if motif
				bar = XmCreateMenuBar (form, "UiOptionMenu", NULL, 0);
				XtVaSetValues (bar, XmNx, fieldX - 4, XmNy, y - 4
					#if defined (macintosh)
						- 1
					#endif
					, XmNwidth, fieldWidth + 8, XmNheight, Gui_OPTIONMENU_HEIGHT + 8, NULL);
				#endif
				box = GuiMenuBar_addMenu2 (bar, L"choice", 0, & field -> cascadeButton);

				#if motif
				XtVaSetValues (bar, XmNwidth, fieldWidth + 8, NULL);
				XtVaSetValues (field -> cascadeButton, XmNx, 4, XmNy, 4, XmNwidth, fieldWidth, XmNheight, Gui_OPTIONMENU_HEIGHT, NULL);
				#endif
				for (long ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = field -> options -> item [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					#if motif
						button -> toggle = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmToggleButtonWidgetClass, box, NULL);
					XtAddCallback (button -> toggle, XmNvalueChangedCallback, cb_optionChanged, (XtPointer) field);
					#endif
				}
				GuiObject_show (bar);
			} break;
			case UI_BOOLEAN:
			{
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				/*GuiLabel_createShown (form, x, x + labelWidth, y, y + Gui_CHECKBUTTON_HEIGHT,
					theFinishBuffer.string, GuiLabel_RIGHT); */
				field -> toggle = GuiCheckButton_createShown (form,
					fieldX, dialogWidth /* allow to extend into the margin */, y, Gui_AUTOMATIC,
					theFinishBuffer.string, NULL, NULL, 0);
			} break;
			case UI_ENUM:
			{
				int max = enum_length (field -> enumerated);
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiLabel_createShown (form, x, x + labelWidth, y + 1, y + 21,
					theFinishBuffer.string, GuiLabel_RIGHT);
				field -> list = GuiList_create (form, fieldX, fieldX + fieldWidth, y, y + LIST_HEIGHT, false);
				for (int i = field -> includeZero ? 0 : 1; i <= max; i ++) {
					GuiList_insertItem (field -> list, enum_string (field -> enumerated, i), 0);
				}
				GuiObject_show (field -> list);
			} break;
			case UI_LIST:
			{
				int listWidth = my numberOfFields == 1 ? dialogWidth - fieldX : fieldWidth;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				GuiLabel_createShown (form, x, x + labelWidth, y + 1, y + 21,
					theFinishBuffer.string, GuiLabel_RIGHT);
				field -> list = GuiList_create (form, fieldX, fieldX + listWidth, y, y + LIST_HEIGHT, false);
				for (long i = 1; i <= field -> numberOfStrings; i ++) {
					GuiList_insertItem (field -> list, field -> strings [i], 0);
				}
				GuiObject_show (field -> list);
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
	}
	if (my numberOfFields > 1 || my field [1] -> type != UI_LABEL) {
		my useStandards = GuiButton_createShown (buttons,
			HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING,
			HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_HORIZONTAL_DIALOG_SPACING + DEF_BUTTON_WIDTH,
			y, Gui_AUTOMATIC,
			L"Standards", gui_button_cb_useStandards, me, 0);
	}
	x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - 2 * Gui_HORIZONTAL_DIALOG_SPACING
		 - Gui_APPLY_BUTTON_WIDTH - Gui_CANCEL_BUTTON_WIDTH;
	my cancelButton = GuiButton_createShown (buttons, x, x + Gui_CANCEL_BUTTON_WIDTH, y, Gui_AUTOMATIC,
		L"Cancel", gui_button_UiForm_hide, me, GuiButton_CANCEL);
	x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH - Gui_HORIZONTAL_DIALOG_SPACING - Gui_APPLY_BUTTON_WIDTH;
	if (my numberOfFields > 1 || my field [1] -> type != UI_LABEL) {
		my applyButton = GuiButton_createShown (buttons, x, x + Gui_APPLY_BUTTON_WIDTH, y, Gui_AUTOMATIC,
			L"Apply", gui_button_UiForm_apply, me, 0);
	}
	x = dialogWidth - Gui_RIGHT_DIALOG_SPACING - Gui_OK_BUTTON_WIDTH;
	my okButton = GuiButton_createShown (buttons, x, x + Gui_OK_BUTTON_WIDTH, y, Gui_AUTOMATIC,
		L"OK", gui_button_UiForm_ok, me, GuiButton_DEFAULT);
	/*GuiObject_show (separator);*/
}

void UiForm_destroyWhenUnmanaged (I) {
	iam (UiForm);
	my destroyWhenUnmanaged = 1;
}

void UiForm_do (I, int modified) {
	iam (UiForm);
	my allowExecutionHook = theAllowExecutionHookHint;
	my allowExecutionClosure = theAllowExecutionClosureHint;
	/* Prevent double callbacks: */
	/*XtRemoveCallback (my okButton, XmNactivateCallback, UiForm_ok, (XtPointer) me);*/
	/* This is the only place where this callback is installed. Moved from UiForm_close ppgb950613. */
	/*XtAddCallback (my okButton, XmNactivateCallback, UiForm_ok, (XtPointer) me);*/
	GuiDialog_show (my dialog);
	if (modified)
		gui_button_UiForm_ok (me, 0);   // BUG: should not send 0
}

int UiForm_parseString (I, const wchar_t *arguments) {
	iam (UiForm);
	int i, size = my numberOfFields;
	while (size >= 1 && my field [size] -> type == UI_LABEL)
		size --;   /* Ignore trailing fields without a value. */
	for (i = 1; i < size; i ++) {
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
				if (*arguments == '\0') return Melder_error1 (L"Missing matching quote.");
				if (*arguments == '\"' && * ++ arguments != '\"') break;   /* Remember second quote. */
				stringValue [ichar ++] = *arguments ++;
			}
		} else {
			while (*arguments != ' ' && *arguments != '\t' && *arguments != '\0')
				stringValue [ichar ++] = *arguments ++;
		}
		stringValue [ichar] = '\0';   /* Trailing null byte. */
		if (! UiField_stringToValue (my field [i], stringValue))
			return Melder_error3 (L"Don't understand contents of field \"", my field [i] -> name, L"\".");
	}
	/* The last item is handled separately, because it consists of the rest of the line.
	 * Leading spaces are skipped, but trailing spaces are included.
	 */
	if (size > 0) {
		while (*arguments == ' ' || *arguments == '\t') arguments ++;
		if (! UiField_stringToValue (my field [size], arguments))
			return Melder_error3 (L"Don't understand contents of field \"", my field [size] -> name, L"\".");
	}
	return my okCallback (me, my okClosure);
}

int UiForm_parseStringE (EditorCommand cmd, const wchar_t *arguments) {
	return UiForm_parseString (cmd -> dialog, arguments);
}

static UiField findField_lenient (UiForm me, const wchar_t *fieldName) {
	int ifield;
	for (ifield = 1; ifield <= my numberOfFields; ifield ++)
		if (wcsequ (fieldName, my field [ifield] -> name)) return my field [ifield];
	return NULL;
}

static void fatalField (UiForm dia) {
	Melder_fatal ("Wrong field in dialog \"%s\".", Melder_peekWcsToUtf8 (dia -> name));
}

static UiField findField (UiForm me, const wchar_t *fieldName) {
	UiField result = findField_lenient (me, fieldName);
	if (result == NULL) fatalField (me);
	return result;
}

void UiForm_setReal (I, const wchar_t *fieldName, double value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	switch (field -> type) {
		case UI_REAL: case UI_POSITIVE: {
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
			const wchar_t *s;
			int integerValue = floor (value);
			if (integerValue != value)
				s = Melder_single (value);
			else if (integerValue >= 0 && integerValue <= 15)
				s = colourNames [integerValue];
			else
				s = L"black";
			GuiText_setString (field -> text, s);
		} break; default: {
			fatalField (me);
		}
	}
}

void UiForm_setInteger (I, const wchar_t *fieldName, long value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: {
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
				UiOption b = field -> options -> item [i];
				GuiRadioButton_setValue (b -> toggle, i == value);
			}
		} break; case UI_OPTIONMENU: {
			if (value < 1 || value > field -> options -> size) value = 1;   /* Guard against incorrect prefs file. */
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = field -> options -> item [i];
				#if motif
				XmToggleButtonSetState (b -> toggle, i == value, False);
				if (i == value) {
					XtVaSetValues (field -> cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
				}
				#endif
			}
		} break; case UI_ENUM: {
			if (value < 0 || value > enum_length (field -> enumerated)) value = 0;   /* Guard against incorrect prefs file. */
			GuiList_selectItem (field -> list, value + field -> includeZero);
		} break; case UI_LIST: {
			if (value < 1 || value > field -> numberOfStrings) value = 1;   /* Guard against incorrect prefs file. */
			GuiList_selectItem (field -> list, value);
		} break; default: {
			fatalField (me);
		}
	}
}

void UiForm_setString (I, const wchar_t *fieldName, const wchar_t *value) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	if (value == NULL) value = L"";   /* Accept NULL strings. */
	switch (field -> type) {
		case UI_REAL: case UI_POSITIVE: case UI_INTEGER: case UI_NATURAL:
			case UI_WORD: case UI_SENTENCE: case UI_COLOUR: case UI_TEXT:
		{
			GuiText_setString (field -> text, value);
		} break; case UI_LABEL: {
			GuiLabel_setString (field -> text, value);
		} break; case UI_RADIO: {
			bool found = false;
			for (int i = 1; i <= field -> options -> size; i ++) {
				UiOption b = field -> options -> item [i];
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
				UiOption b = field -> options -> item [i];
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
		} break; case UI_ENUM: {
			long integerValue = enum_search (field -> enumerated, value);
			if (integerValue < 0) integerValue = 0;   /* Guard against incorrect prefs file. */
			GuiList_selectItem (field -> list, integerValue + field -> includeZero);
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

static UiField findField_check (UiForm me, const wchar_t *fieldName) {
	UiField result = findField_lenient (me, fieldName);
	if (result == NULL) {
		Melder_error3 (L"Cannot find field \"", fieldName, L"\" in form.\n"
			"The script may have changed while the form was open.\n"
			"Please click Cancel in the form and try again.");
	}
	return result;
}

double UiForm_getReal (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	switch (field -> type) {
		case UI_REAL: case UI_POSITIVE: case UI_COLOUR: {
			return field -> realValue;
		} break; default: {
			fatalField (me);
		}
	}
	return 0.0;
}

double UiForm_getReal_check (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName); cherror
	switch (field -> type) {
		case UI_REAL: case UI_POSITIVE: case UI_COLOUR: {
			return field -> realValue;
		} break; default: {
			Melder_error3 (L"Cannot find a real value in field \"", fieldName, L"\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
end:
	return 0.0;
}

long UiForm_getInteger (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: case UI_BOOLEAN: case UI_RADIO:
			case UI_OPTIONMENU: case UI_ENUM: case UI_LIST:
		{
			return field -> integerValue;
		} break; default: {
			fatalField (me);
		}
	}
	return 0L;
}

long UiForm_getInteger_check (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName); cherror
	switch (field -> type) {
		case UI_INTEGER: case UI_NATURAL: case UI_BOOLEAN: case UI_RADIO:
			case UI_OPTIONMENU: case UI_ENUM: case UI_LIST:
		{
			return field -> integerValue;
		} break; default: {
			Melder_error3 (L"Cannot find an integer value in field \"", fieldName, L"\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
end:
	return 0L;
}

wchar_t * UiForm_getString (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField (me, fieldName);
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			return field -> stringValue;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = field -> options -> item [field -> integerValue];
			return b -> name;
		} break; case UI_ENUM: {
			return enum_string (field -> enumerated, field -> integerValue);
		} break; case UI_LIST: {
			return (wchar_t *) field -> strings [field -> integerValue];
		} break; default: {
			fatalField (me);
		}
	}
	return NULL;
}

wchar_t * UiForm_getString_check (I, const wchar_t *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, fieldName); cherror
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			return field -> stringValue;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = field -> options -> item [field -> integerValue];
			return b -> name;
		} break; case UI_ENUM: {
			return enum_string (field -> enumerated, field -> integerValue);
		} break; case UI_LIST: {
			return (wchar_t *) field -> strings [field -> integerValue];
		} break; default: {
			Melder_error3 (L"Cannot find a string in field \"", fieldName, L"\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
end:
	return NULL;
}

/* End of file Ui.c */

