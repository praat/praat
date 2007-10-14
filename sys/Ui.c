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

static void cb_radioChanged (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiField);
	int i;
	(void) call;
	for (i = 1; i <= my options -> size; i ++) {
		UiOption b = my options -> item [i];
		XmToggleButtonSetState (b -> toggle, b -> toggle == w, FALSE);
	}
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
		if (b -> toggle == w) {
			XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
			XmToggleButtonSetState (b -> toggle, TRUE, FALSE);
			if (Melder_debug == 11) {
				Melder_warning ("%d \"%s\"", i, Melder_peekWcsToUtf8 (b -> name));
			}
		} else {
			XmToggleButtonSetState (b -> toggle, FALSE, FALSE);
		}
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
			GuiText_setStringW (my text, my stringDefaultValue);
		} break; case UI_BOOLEAN: {
			XmToggleButtonSetState (my toggle, my integerDefaultValue, False);
		} break; case UI_RADIO: {
			int i;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				XmToggleButtonSetState (b -> toggle, i == my integerDefaultValue, False);
			}
		} break; case UI_OPTIONMENU: {
			int i;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				XmToggleButtonSetState (b -> toggle, i == my integerDefaultValue, False);
				if (i == my integerDefaultValue) {
					XtVaSetValues (my cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
				}
			}
		} break; case UI_ENUM: {
			XmListSelectPos (my list, my integerDefaultValue + my includeZero, False);
		} break; case UI_LIST: {
			XmListSelectPos (my list, my integerDefaultValue, False);
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
			wchar_t *dirty = GuiText_getStringW (my text);   /* The text as typed by the user. */
			if (! Interpreter_numericExpression (NULL, dirty, & my realValue)) { Melder_free (dirty); return 0; }
			Melder_free (dirty);
			/*
			 * Put a clean version of the new value in the form.
			 * If the value is equal to the default, make sure that any default comments are included.
			 */
			if (my realValue == Melder_atofW (my stringDefaultValue)) {
				GuiText_setStringW (my text, my stringDefaultValue);
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
				GuiText_setStringW (my text, clean);
			}
			if (my type == UI_POSITIVE && my realValue <= 0.0)
				return Melder_error3 (L"`", my name, L"' must be greater than 0.0.");
		} break; case UI_INTEGER: case UI_NATURAL: {
			wchar_t *dirty = GuiText_getStringW (my text);
			double realValue;
			if (! Interpreter_numericExpression (NULL, dirty, & realValue)) { Melder_free (dirty); return 0; }
			Melder_free (dirty);
			my integerValue = floor (realValue + 0.5);
			if (my integerValue == wcstol (my stringDefaultValue, NULL, 10)) {
				GuiText_setStringW (my text, my stringDefaultValue);
			} else {
				GuiText_setStringW (my text, Melder_integer (my integerValue));
			}
			if (my type == UI_NATURAL && my integerValue < 1)
				return Melder_error3 (L"`", my name, L"' must be a positive whole number.");
		} break; case UI_WORD: {
			Melder_free (my stringValue);
			my stringValue = GuiText_getStringW (my text);
			wchar_t *p = my stringValue;
			while (*p != '\0') { if (*p == ' ' || *p == '\t') *p = '\0'; p ++; }
			GuiText_setStringW (my text, my stringValue);
		} break; case UI_SENTENCE: case UI_TEXT: {
			Melder_free (my stringValue);
			my stringValue = GuiText_getStringW (my text);
		} break; case UI_BOOLEAN: {
			my integerValue = XmToggleButtonGetState (my toggle);
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			int i;
			my integerValue = 0;
			for (i = 1; i <= my options -> size; i ++) {
				UiOption b = my options -> item [i];
				if (XmToggleButtonGetState (b -> toggle))
					my integerValue = i;
			}
			if (my integerValue == 0)
				return Melder_error3 (L"No option chosen for `", my name, L"'.");
		} break; case UI_ENUM: case UI_LIST: {
			int nSelected, *selected;
			if (! XmListGetSelectedPos (my list, & selected, & nSelected)) {
				Melder_warning ("No items selected.");
				my integerValue = 1;
			} else {
				if (nSelected > 1) Melder_warning ("More than one item selected.");
				my integerValue = selected [0];
				XtFree ((char *) selected);
			}
			if (my type == UI_ENUM && my includeZero)
				my integerValue -= 1;
		} break; case UI_COLOUR: {
			wchar_t *string = GuiText_getStringW (my text);
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
			if (! string [0]) return Melder_error ("Empty argument for toggle button.");
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
	if (my dialog) XtDestroyWidget (my dialog);
	Melder_free (my helpTitle);
	inherited (UiForm) destroy (me);
}

class_methods (UiForm, Thing) {
	class_method_local (UiForm, destroy)
class_methods_end }

static void cb_useStandards (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiForm);
	int ifield;
	(void) w;
	(void) call;
	for (ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
}

static void UiForm_hide (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiForm);
	(void) w;
	(void) call;
	XtUnmanageChild (my dialog);
	if (my destroyWhenUnmanaged) forget (me);
}

static void UiForm_okOrApply (Widget w, XtPointer void_me, XtPointer call, int hide) {
	iam (UiForm);
	int ifield;
	(void) w;
	(void) call;
	if (my allowExecutionHook && ! my allowExecutionHook (my allowExecutionClosure)) {
		Melder_error3 (L"Cannot execute dialog `", my name, L"'.");
		Melder_flushError (NULL);
		return;
	}
	for (ifield = 1; ifield <= my numberOfFields; ifield ++) {
		if (! UiField_widgetToValue (my field [ifield])) {
			Melder_error3 (L"Please correct dialog " L_LEFT_SINGLE_QUOTE, my name, L_RIGHT_SINGLE_QUOTE L" or cancel.");
			Melder_flushError (NULL);
			return;
		}
	}
	/* In the next, w must become my okButton? */
	/*XtRemoveCallback (w, XmNactivateCallback, UiForm_ok, void_me);   /* FIX */
	XtSetSensitive (my okButton, False);
	if (my applyButton) XtSetSensitive (my applyButton, False);
	XtSetSensitive (my cancelButton, False);
	if (my useStandards) XtSetSensitive (my useStandards, False);
	if (my helpButton) XtSetSensitive (my helpButton, False);
	XmUpdateDisplay (my dialog);
	if (my okCallback (me, my okClosure)) {
		int destroyWhenUnmanaged = my destroyWhenUnmanaged;   /* Save before destruction. */
		/*
		 * Write everything to history. Before destruction!
		 */
		int size = my numberOfFields;
		while (size >= 1 && my field [size] -> type == UI_LABEL)
			size --;   /* Ignore trailing fields without a value. */
		for (ifield = 1; ifield <= size; ifield ++)
			UiField_valueToHistory (my field [ifield], ifield == size);
		if (hide) {
			UiForm_hide (w, (XtPointer) me, 0);
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
	XtSetSensitive (my okButton, True);
	if (my applyButton) XtSetSensitive (my applyButton, True);
	XtSetSensitive (my cancelButton, True);
	if (my useStandards) XtSetSensitive (my useStandards, True);
	if (my helpButton) XtSetSensitive (my helpButton, True);
}

static void UiForm_ok (Widget w, XtPointer void_me, XtPointer call) {
	UiForm_okOrApply (w, void_me, call, TRUE);
}

static void UiForm_apply (Widget w, XtPointer void_me, XtPointer call) {
	UiForm_okOrApply (w, void_me, call, FALSE);
}

static void cb_help (Widget w, XtPointer void_me, XtPointer call) {
	iam (UiForm);
	(void) w;
	(void) call;
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

static int Gui_bottomDialogSpacing (void) { return 20; }
static int Gui_leftDialogSpacing (void) { return 20; }
static int Gui_rightDialogSpacing (void) { return 20; }
static int Gui_topDialogSpacing (void) { return 14; /* adapt to control type */ }
static int Gui_verticalDialogSpacing_same (void) { return 12; }
static int Gui_verticalDialogSpacing_different (void) { return 20; }
static int Gui_horizontalDialogSpacing (void) { return 12; }
static int Gui_textFieldHeight (void) { return Machine_getTextHeight (); }
static int Gui_radioButtonHeight (void) { return 18; }
static int Gui_radioButtonSpacing (void) { return 8; }
static int Gui_checkButtonHeight (void) { return 20; }
static int Gui_labelSpacing (void) { return 8; }
static int Gui_optionMenuHeight (void) { return 20; }
static int Gui_pushButtonHeight (void) { return 20; }
static int Gui_okButtonWidth (void) { return 69; }
static int Gui_cancelButtonWidth (void) { return 69; }
static int Gui_applyButtonWidth (void) { return 69; }
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
	int ifield, size = my numberOfFields, ibutton;
	int dialogHeight = 0, x = Gui_leftDialogSpacing (), y;
	int textFieldHeight = Gui_textFieldHeight ();
	int dialogWidth = 480, dialogCentre = dialogWidth / 2, fieldX = dialogCentre + Gui_labelSpacing () / 2;
	int labelWidth = fieldX - Gui_labelSpacing () - x, fieldWidth = labelWidth, halfFieldWidth = fieldWidth / 2 - 6;
	if (! my parent) return;
	/*
		Compute height. Cannot leave this to the default geometry management system.
	*/
	for (ifield = 1; ifield <= my numberOfFields; ifield ++ ) {
		UiField thee = my field [ifield], previous = my field [ifield - 1];
		dialogHeight +=
			ifield == 1 ? Gui_topDialogSpacing () :
			thy type == UI_RADIO || previous -> type == UI_RADIO ? Gui_verticalDialogSpacing_different () :
			thy type >= UI_LABELLEDTEXT_MIN && thy type <= UI_LABELLEDTEXT_MAX && wcsnequ (thy name, L"right ", 6) &&
			previous -> type >= UI_LABELLEDTEXT_MIN && previous -> type <= UI_LABELLEDTEXT_MAX &&
			wcsnequ (previous -> name, L"left ", 5) ? - textFieldHeight : Gui_verticalDialogSpacing_same ();
		thy y = dialogHeight;
		dialogHeight +=
			thy type == UI_BOOLEAN ? Gui_checkButtonHeight () :
			thy type == UI_RADIO ? thy options -> size * Gui_radioButtonHeight () +
				(thy options -> size - 1) * Gui_radioButtonSpacing () :
			thy type == UI_OPTIONMENU ? Gui_optionMenuHeight () :
			thy type == UI_ENUM || thy type == UI_LIST ? LIST_HEIGHT :
			thy type == UI_LABEL && thy stringValue [0] != '\0' && thy stringValue [wcslen (thy stringValue) - 1] != '.' &&
				ifield != my numberOfFields ? Gui_textFieldHeight ()
				#ifdef _WIN32
					- 6 :
				#else
					- 10 :
				#endif
			Gui_textFieldHeight ();
	}
	dialogHeight += 2 * Gui_bottomDialogSpacing () + Gui_pushButtonHeight ();
	my shell = XmCreateDialogShell (my parent, "UiForm", NULL, 0);
	XtVaSetValues (my shell, XmNx, DIALOG_X, XmNy, DIALOG_Y, XmNwidth, dialogWidth, XmNheight, dialogHeight, NULL);
	my dialog = XmCreateBulletinBoard (my shell, Melder_peekWcsToUtf8 (my name), NULL, 0);
	/* Catch Window Manager "Close". */
	Atom atom = XmInternAtom (XtDisplay (my shell), "WM_DELETE_WINDOW", True);
	XmAddWMProtocols (my shell, & atom, 1);
	XmAddWMProtocolCallback (my shell, atom, UiForm_hide, (void *) me);
	XtVaSetValues (XtParent (my dialog), XmNtitle, Melder_peekWcsToUtf8 (my name), XmNdeleteResponse, XmDO_NOTHING, NULL);
	XtVaSetValues (my dialog, XmNautoUnmanage, False,
		/*XmNdialogStyle, XmDIALOG_FULL_APPLICATION_MODAL,*/ NULL);
	for (ifield = 1; ifield <= size; ifield ++) {
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
				if (wcsnequ (field -> name, L"left ", 5)) {
					MelderString_copy (& theFinishBuffer, field -> formLabel + 5);
					appendColon ();
					XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
						XmNx, x, XmNy, y
						#if defined (macintosh)
							+ 3
						#endif
						, XmNwidth, labelWidth, XmNheight, textFieldHeight,
						XmNalignment, XmALIGNMENT_END, NULL);
					field -> text = XmCreateText (my dialog, "UiLabelledRange_textLeft", NULL, 0);
					XtVaSetValues (field -> text,
						XmNx, fieldX, XmNy, y,
						#if defined (UNIXppp)
							XmNcolumns, 11,
						#else
							XmNwidth, halfFieldWidth,
						#endif
						XmNheight, textFieldHeight, NULL);
					XtManageChild (field -> text);
				} else if (wcsnequ (field -> name, L"right ", 6)) {
					field -> text = XmCreateText (my dialog, "UiLabelledRange_textRight", NULL, 0);
					XtVaSetValues (field -> text,
						XmNx, fieldX + halfFieldWidth + 12, XmNy, y,
						#if defined (UNIXppp)
							XmNcolumns, 11,
						#else
							XmNwidth, halfFieldWidth,
						#endif
						XmNheight, textFieldHeight, NULL);
					XtManageChild (field -> text);
				} else {
					MelderString_copy (& theFinishBuffer, field -> formLabel);
					appendColon ();
					XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
						XmNx, x, XmNy, y
						#if defined (macintosh)
							+ 3
						#endif
						, XmNwidth, labelWidth, XmNheight, textFieldHeight,
						XmNalignment, XmALIGNMENT_END, NULL);
					field -> text = XmCreateTextField (my dialog, "UiLabelledText_text", NULL, 0);
					XtVaSetValues (field -> text,
						XmNx, fieldX, XmNy, y,
						#if defined (UNIXppp)
							XmNcolumns, 23,
						#else
							XmNwidth, fieldWidth,
						#endif
						XmNheight, textFieldHeight, NULL);
					XtManageChild (field -> text);
				}
			} break;
			case UI_TEXT:
			{
				field -> text = XtVaCreateManagedWidget ("UiText_text", xmTextFieldWidgetClass, my dialog,
					XmNx, x, XmNy, y, XmNwidth, dialogWidth - Gui_leftDialogSpacing () - Gui_rightDialogSpacing (),
					XmNheight, textFieldHeight, NULL);
			} break;
			case UI_LABEL:
			{
				MelderString_copy (& theFinishBuffer, field -> stringValue);
				field -> text = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y + 5, XmNwidth, dialogWidth - Gui_leftDialogSpacing () - Gui_rightDialogSpacing (),
					XmNheight, textFieldHeight, 0, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (theFinishBuffer.string)), NULL);
			} break;
			case UI_RADIO:
			{
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y
					#if defined (macintosh)
						+ 1
					#endif
					, XmNwidth, labelWidth, XmNheight, Gui_radioButtonHeight (),
					XmNalignment, XmALIGNMENT_END, NULL);
				for (ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = field -> options -> item [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					button -> toggle = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmToggleButtonWidgetClass, my dialog,
						XmNx, fieldX, XmNy, y + (ibutton - 1) * (Gui_radioButtonHeight () + Gui_radioButtonSpacing ()),
						XmNwidth, fieldWidth, XmNheight, Gui_radioButtonHeight (),
						XmNindicatorType, XmONE_OF_MANY, NULL);
					XtAddCallback (button -> toggle, XmNvalueChangedCallback, cb_radioChanged, (XtPointer) field);
				}
			} break; 
			case UI_OPTIONMENU:
			{
				Widget bar, box;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y
					#if defined (macintosh)
						+ 2
					#endif
					, XmNwidth, labelWidth, XmNheight, Gui_optionMenuHeight (),
					XmNalignment, XmALIGNMENT_END, NULL);
				bar = XmCreateMenuBar (my dialog, "UiOptionMenu", NULL, 0);
				XtVaSetValues (bar, XmNx, fieldX - 4, XmNy, y - 4
					#if defined (macintosh)
						- 1
					#endif
					, XmNwidth, fieldWidth + 8, XmNheight, Gui_optionMenuHeight () + 8, NULL);
				box = motif_addMenu2 (bar, L"choice", 0, & field -> cascadeButton);
				XtVaSetValues (bar, XmNwidth, fieldWidth + 8, NULL);
				XtVaSetValues (field -> cascadeButton, XmNx, 4, XmNy, 4, XmNwidth, fieldWidth, XmNheight, Gui_optionMenuHeight (), NULL);
				for (ibutton = 1; ibutton <= field -> options -> size; ibutton ++) {
					UiOption button = field -> options -> item [ibutton];
					MelderString_copy (& theFinishBuffer, button -> name);
					button -> toggle = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmToggleButtonWidgetClass, box, NULL);
					XtAddCallback (button -> toggle, XmNvalueChangedCallback, cb_optionChanged, (XtPointer) field);
				}
				XtManageChild (bar);
			} break;
			case UI_BOOLEAN:
			{
				/*XtVaCreateManagedWidget ("", xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y, XmNwidth, labelWidth, XmNheight, Gui_checkButtonHeight (),
					XmNalignment, XmALIGNMENT_END, NULL);*/
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				field -> toggle = XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmToggleButtonWidgetClass, my dialog,
					XmNx, fieldX, XmNy, y, XmNheight, Gui_checkButtonHeight (), NULL);
			} break;
			case UI_ENUM:
			{
				#ifndef _WIN32
					Widget scrolled;
				#endif
				int max = enum_length (field -> enumerated), n = max + field -> includeZero, i;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y + 1, XmNwidth, labelWidth, XmNheight, 20,
					XmNalignment, XmALIGNMENT_END, NULL);
				#ifdef _WIN32
					field -> list = XtVaCreateWidget ("UiEnum_list", xmListWidgetClass, my dialog,
						XmNselectionPolicy, XmBROWSE_SELECT, NULL);
					XtVaSetValues (field -> list, XmNx, fieldX, XmNy, y, XmNwidth, fieldWidth, XmNheight, LIST_HEIGHT, NULL);
				#else
					scrolled = XmCreateScrolledWindow (my dialog, "UiEnum_scrolled", NULL, 0);
					XtVaSetValues (scrolled, XmNx, fieldX, XmNy, y, XmNwidth, fieldWidth, XmNheight, LIST_HEIGHT, NULL);
					field -> list = XtVaCreateWidget ("UiEnum_list", xmListWidgetClass, scrolled,
						XmNselectionPolicy, XmBROWSE_SELECT, NULL);
				#endif
				XtVaSetValues (field -> list, XmNwidth, fieldWidth + 100,
					XmNvisibleItemCount, n < 10 ? n + 1 : 11, NULL);
				for (i = field -> includeZero ? 0 : 1; i <= max; i ++) {
					XmString s = XmStringCreateSimple (Melder_peekWcsToUtf8 (enum_string (field -> enumerated, i)));
					XmListAddItem (field -> list, s, 0);
					XmStringFree (s);
				}
				XtManageChild (field -> list);
				#ifndef _WIN32
					XtManageChild (scrolled);
				#endif
			} break;
			case UI_LIST:
			{
				#ifndef _WIN32
					Widget scrolled;
				#endif
				long i;
				int listWidth = my numberOfFields == 1 ? dialogWidth - fieldX : fieldWidth;
				MelderString_copy (& theFinishBuffer, field -> formLabel);
				appendColon ();
				XtVaCreateManagedWidget (Melder_peekWcsToUtf8 (theFinishBuffer.string), xmLabelWidgetClass, my dialog,
					XmNx, x, XmNy, y + 1, XmNwidth, labelWidth, XmNheight, 20,
					XmNalignment, XmALIGNMENT_END, NULL);
				#ifdef _WIN32
					field -> list = XtVaCreateWidget ("UiList_list", xmListWidgetClass, my dialog,
						XmNselectionPolicy, XmBROWSE_SELECT, NULL);
					XtVaSetValues (field -> list, XmNx, fieldX, XmNy, y, XmNwidth, listWidth, XmNheight, LIST_HEIGHT, NULL);
				#else
					scrolled = XmCreateScrolledWindow (my dialog, "UiList_scrolled", NULL, 0);
					XtVaSetValues (scrolled, XmNx, fieldX, XmNy, y, XmNwidth, listWidth, XmNheight, LIST_HEIGHT, NULL);
					field -> list = XtVaCreateWidget ("UiList_list", xmListWidgetClass, scrolled,
						XmNselectionPolicy, XmBROWSE_SELECT, NULL);
				#endif
				XtVaSetValues (field -> list, XmNwidth, listWidth + 100,
					XmNvisibleItemCount, (int) (field -> numberOfStrings < 10 ? field -> numberOfStrings + 1 : 11), NULL);
				for (i = 1; i <= field -> numberOfStrings; i ++) {
					XmString s = XmStringCreateSimple (Melder_peekWcsToUtf8 (field -> strings [i]));
					XmListAddItem (field -> list, s, 0);
					XmStringFree (s);
				}
				XtManageChild (field -> list);
				#ifndef _WIN32
					XtManageChild (scrolled);
				#endif
			} break;
		}
	}
	for (ifield = 1; ifield <= my numberOfFields; ifield ++)
		UiField_setDefault (my field [ifield]);
	/*separator = XmCreateSeparatorGadget (column, "separator", NULL, 0);*/
	y = dialogHeight - Gui_bottomDialogSpacing () - Gui_pushButtonHeight ();
	if (my helpTitle) {
		my helpButton = XmCreatePushButton (my dialog, "Help", NULL, 0);
		XtVaSetValues (my helpButton,
			XmNx, HELP_BUTTON_X, XmNy, y, XmNwidth, HELP_BUTTON_WIDTH, XmNheight, Gui_pushButtonHeight (), NULL);
		XtAddCallback (my helpButton, XmNactivateCallback, cb_help, (XtPointer) me);
		XtManageChild (my helpButton);
	}
	if (my numberOfFields > 1 || my field [1] -> type != UI_LABEL) {
		my useStandards = XmCreatePushButton (my dialog, "Standards", NULL, 0);
		XtVaSetValues (my useStandards,
			XmNx, HELP_BUTTON_X + HELP_BUTTON_WIDTH + Gui_horizontalDialogSpacing (), XmNy, y, XmNwidth, DEF_BUTTON_WIDTH, XmNheight, Gui_pushButtonHeight (), NULL);
		XtAddCallback (my useStandards, XmNactivateCallback, cb_useStandards, (XtPointer) me);
		XtManageChild (my useStandards);
	}
	my cancelButton = XmCreatePushButton (my dialog, "Cancel", NULL, 0);
	x = dialogWidth - Gui_rightDialogSpacing () - Gui_okButtonWidth () - 2 * Gui_horizontalDialogSpacing ()
		 - Gui_applyButtonWidth () - Gui_cancelButtonWidth ();
	XtVaSetValues (my cancelButton,
		XmNx, x, XmNy, y, XmNwidth, Gui_cancelButtonWidth (), XmNheight, Gui_pushButtonHeight (), NULL);
	XtVaSetValues (my dialog, XmNcancelButton, my cancelButton, NULL);
	XtAddCallback (my cancelButton, XmNactivateCallback, UiForm_hide, (XtPointer) me);
	x = dialogWidth - Gui_rightDialogSpacing () - Gui_okButtonWidth ();
	if (my numberOfFields > 1 || my field [1] -> type != UI_LABEL) {
		my applyButton = XmCreatePushButton (my dialog, "Apply", NULL, 0);
	}
	x = dialogWidth - Gui_rightDialogSpacing () - Gui_okButtonWidth () - Gui_horizontalDialogSpacing () - Gui_applyButtonWidth ();
	if (my applyButton) {
		XtVaSetValues (my applyButton,
			XmNx, x, XmNy, y, XmNwidth, Gui_applyButtonWidth (), XmNheight, Gui_pushButtonHeight (), NULL);
		XtAddCallback (my applyButton, XmNactivateCallback, UiForm_apply, (XtPointer) me);
		XtManageChild (my applyButton);
	}
	x = dialogWidth - Gui_rightDialogSpacing () - Gui_okButtonWidth ();
	my okButton = XmCreatePushButton (my dialog, "OK", NULL, 0);
	XtVaSetValues (my okButton, XmNx, x, XmNy, y, XmNwidth, Gui_okButtonWidth (), XmNheight, Gui_pushButtonHeight (), NULL);
	XtAddCallback (my okButton, XmNactivateCallback, UiForm_ok, (XtPointer) me);
	XtVaSetValues (my dialog, XmNdefaultButton, my okButton, NULL);
	/*XtManageChild (separator);*/
	XtManageChild (my okButton);
	XtManageChild (my cancelButton);
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
	XtManageChild (my dialog);
	XMapRaised (XtDisplay (my shell), XtWindow (my shell));
	if (modified)
		UiForm_ok (my okButton, (XtPointer) me, 0);
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
			if (value == Melder_atofW (field -> stringDefaultValue)) {
				GuiText_setStringW (field -> text, field -> stringDefaultValue);
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
				GuiText_setStringW (field -> text, s);
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
			GuiText_setStringW (field -> text, s);
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
				GuiText_setStringW (field -> text, field -> stringDefaultValue);
			} else {
				GuiText_setStringW (field -> text, Melder_integer (value));
			}
		} break; case UI_BOOLEAN: {
			XmToggleButtonSetState (field -> toggle, (int) value, False);
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			int i;
			if (value < 1 || value > field -> options -> size) value = 1;   /* Guard against incorrect prefs file. */
			for (i = 1; i <= field -> options -> size; i ++) {
				UiOption b = field -> options -> item [i];
				XmToggleButtonSetState (b -> toggle, i == value, False);
				if (field -> type == UI_OPTIONMENU && i == value) {
					XtVaSetValues (field -> cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (b -> name)), NULL);
				}
			}
		} break; case UI_ENUM: {
			if (value < 0 || value > enum_length (field -> enumerated)) value = 0;   /* Guard against incorrect prefs file. */
			XmListSelectPos (field -> list, value + field -> includeZero, False);
		} break; case UI_LIST: {
			if (value < 1 || value > field -> numberOfStrings) value = 1;   /* Guard against incorrect prefs file. */
			XmListSelectPos (field -> list, value, False);
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
			GuiText_setStringW (field -> text, value);
		} break; case UI_LABEL: {
			XtVaSetValues (field -> text, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (value)), NULL);
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			int i, found = FALSE;
			for (i = 1; i <= field -> options -> size; i ++) {
				UiOption b = field -> options -> item [i];
				if (wcsequ (value, b -> name)) {
					XmToggleButtonSetState (b -> toggle, True, False);
					found = TRUE;
					if (field -> type == UI_OPTIONMENU) {
						XtVaSetValues (field -> cascadeButton, motif_argXmString (XmNlabelString, Melder_peekWcsToUtf8 (value)), NULL);
					}
				} else {
					XmToggleButtonSetState (b -> toggle, False, False);
				}
			}
			/* If not found: do nothing (guard against incorrect prefs file). */
		} break; case UI_ENUM: {
			long integerValue = enum_search (field -> enumerated, value);
			if (integerValue < 0) integerValue = 0;   /* Guard against incorrect prefs file. */
			XmListSelectPos (field -> list, integerValue + field -> includeZero, False);
		} break; case UI_LIST: {
			long i;
			for (i = 1; i <= field -> numberOfStrings; i ++)
				if (wcsequ (value, field -> strings [i])) break;
			if (i > field -> numberOfStrings) i = 1;   /* Guard against incorrect prefs file. */
			XmListSelectPos (field -> list, i, False);
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

char * UiForm_getStringA (I, const char *fieldName) {
	iam (UiForm);
	UiField field = findField (me, Melder_peekUtf8ToWcs (fieldName));
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			Melder_free (field -> stringValueA);
			field -> stringValueA = Melder_wcsToUtf8 (field -> stringValue);
			return field -> stringValueA;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = field -> options -> item [field -> integerValue];
			return Melder_peekWcsToUtf8 (b -> name);
		} break; case UI_ENUM: {
			return Melder_peekWcsToUtf8 (enum_string (field -> enumerated, field -> integerValue));
		} break; case UI_LIST: {
			return Melder_peekWcsToUtf8 (field -> strings [field -> integerValue]);
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

char * UiForm_getStringA_check (I, const char *fieldName) {
	iam (UiForm);
	UiField field = findField_check (me, Melder_peekUtf8ToWcs (fieldName)); cherror
	switch (field -> type) {
		case UI_WORD: case UI_SENTENCE: case UI_TEXT: {
			if (field -> stringValueA == NULL) {
				field -> stringValueA = Melder_wcsToUtf8 (field -> stringValue);
			}
			return field -> stringValueA;
		} break; case UI_RADIO: case UI_OPTIONMENU: {
			UiOption b = field -> options -> item [field -> integerValue];
			return Melder_peekWcsToUtf8 (b -> name);
		} break; case UI_ENUM: {
			return Melder_peekWcsToUtf8 (enum_string (field -> enumerated, field -> integerValue));
		} break; case UI_LIST: {
			return Melder_peekWcsToUtf8 (field -> strings [field -> integerValue]);
		} break; default: {
			Melder_error3 (L"Cannot find a string in field \"", Melder_peekUtf8ToWcs (fieldName), L"\" in the form.\n"
				"The script may have changed while the form was open.\n"
				"Please click Cancel in the form and try again.");
		}
	}
end:
	return NULL;
}

/* End of file Ui.c */

