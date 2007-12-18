/* OTMultiEditor.c
 *
 * Copyright (C) 2005-2007 Paul Boersma
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
 * pb 2005/07/04 created
 * pb 2006/05/17 draw disharmonies on top of tableau
 * pb 2007/06/10 wchar_t
 * pb 2007/08/12 wchar_t
 * pb 2007/10/01 constraint plasticity
 * pb 2007/12/14 Gui
 */

#include "OTMultiEditor.h"
#include "EditorM.h"
#include "machine.h"

#define OTMultiEditor_members HyperPage_members \
	const wchar_t *form1, *form2; \
	Widget form1Text, form2Text; \
	long selectedConstraint;
#define OTMultiEditor_methods HyperPage_methods
class_create_opaque (OTMultiEditor, HyperPage);

FORM (OTMultiEditor, cb_evaluate, L"Evaluate", 0)
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	Editor_save (me, L"Evaluate");
	OTMulti_newDisharmonies (my data, GET_REAL (L"Evaluation noise"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_evaluate_noise_2_0)
	Editor_save (me, L"Evaluate (noise 2.0)");
	OTMulti_newDisharmonies (my data, 2.0);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_evaluate_tinyNoise)
	Editor_save (me, L"Evaluate (tiny noise)");
	OTMulti_newDisharmonies (my data, 1e-9);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_editRanking, L"Edit ranking", 0)
	LABEL (L"constraint", L"");
	REAL (L"Ranking value", L"100.0");
	REAL (L"Disharmony", L"100.0");
	OK
OTMulti grammar = my data;
OTConstraint constraint;
if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints) return Melder_error1 (L"Select a constraint first.");
constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
SET_STRING (L"constraint", constraint -> name)
SET_REAL (L"Ranking value", constraint -> ranking)
SET_REAL (L"Disharmony", constraint -> disharmony)
DO
	OTMulti grammar = my data;
	OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, L"Edit ranking");
	constraint -> ranking = GET_REAL (L"Ranking value");
	constraint -> disharmony = GET_REAL (L"Disharmony");
	OTMulti_sort (grammar);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_learnOne, L"Learn one", L"OTGrammar: Learn one...")
	OPTIONMENU (L"Learn", 3)
		OPTION (L"forward")
		OPTION (L"backward")
		OPTION (L"bidirectionally")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	OK
DO
	Editor_save (me, L"Learn one");
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = GuiText_getString (my form1Text);
	my form2 = GuiText_getString (my form2Text);
	OTMulti_learnOne (my data, my form1, my form2,
		GET_INTEGER (L"Learn"), GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"));
	iferror return 0;
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_removeConstraint)
	OTMulti grammar = my data;
	OTConstraint constraint;
	if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
		return Melder_error1 (L"Select a constraint first.");
	constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, L"Remove constraint");
	OTMulti_removeConstraint (grammar, constraint -> name);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_resetAllRankings, L"Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	Editor_save (me, L"Reset all rankings");
	OTMulti_reset (my data, GET_REAL (L"Ranking"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_OTLearningTutorial) Melder_help (L"OT learning"); END

static void do_limit (OTMultiEditor me) {
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = GuiText_getString (my form1Text);
	my form2 = GuiText_getString (my form2Text);
	Graphics_updateWs (my g);
}

static void gui_button_cb_limit (Widget widget, I) {
	(void) widget;
	iam (OTMultiEditor);
	do_limit (me);
}

static void gui_cb_limit (GUI_ARGS) {
	GUI_IAM (OTMultiEditor);
	do_limit (me);
}

static void createChildren (I) {
	iam (OTMultiEditor);
#if defined (macintosh)
	#define STRING_SPACING 8
#else
	#define STRING_SPACING 2
#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	inherited (OTMultiEditor) createChildren (me);
	GuiButton_createShown (my dialog, 4, 124, y, y + height,
		L"Partial forms:", gui_button_cb_limit, me,
		#ifdef _WIN32
			GuiButton_DEFAULT   // BUG: clickedCallback should work for texts
		#else
			0
		#endif
		);
	my form1Text = XtVaCreateManagedWidget ("form1Text", xmTextFieldWidgetClass, my dialog,
		XmNx, 124 + STRING_SPACING, XmNy, y, XmNwidth, 150, NULL);
	XtAddCallback (my form1Text, XmNactivateCallback, gui_cb_limit, (XtPointer) me);
	my form2Text = XtVaCreateManagedWidget ("form2Text", xmTextFieldWidgetClass, my dialog,
		XmNx, 274 + 2 * STRING_SPACING, XmNy, y, XmNwidth, 150, NULL);
	XtAddCallback (my form2Text, XmNactivateCallback, gui_cb_limit, (XtPointer) me);
}

static void createMenus (I) {
	iam (OTMultiEditor);
	inherited (OTMultiEditor) createMenus (me);
	Editor_addCommand (me, L"Edit", L"-- edit ot --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Evaluate...", 0, cb_evaluate);
	Editor_addCommand (me, L"Edit", L"Evaluate (noise 2.0)", '2', cb_evaluate_noise_2_0);
	Editor_addCommand (me, L"Edit", L"Evaluate (tiny noise)", '9', cb_evaluate_tinyNoise);
	Editor_addCommand (me, L"Edit", L"Edit ranking...", 'E', cb_editRanking);
	Editor_addCommand (me, L"Edit", L"Reset all rankings...", 'R', cb_resetAllRankings);
	Editor_addCommand (me, L"Edit", L"Learn one...", '1', cb_learnOne);
	Editor_addCommand (me, L"Edit", L"-- remove --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Remove constraint", 0, cb_removeConstraint);
	Editor_addCommand (me, L"Help", L"OT learning tutorial", 0, cb_OTLearningTutorial);
}

static OTMulti drawTableau_grammar;
static const wchar_t *drawTableau_form1, *drawTableau_form2;
static void drawTableau (Graphics g) {
	OTMulti_drawTableau (drawTableau_grammar, g, drawTableau_form1, drawTableau_form2, TRUE);
}

static void draw (I) {
	iam (OTMultiEditor);
	OTMulti grammar = my data;
	static MelderString buffer = { 0 };
	double rowHeight = 0.25, tableauHeight = 2 * rowHeight;
	Graphics_clearWs (my g);
	HyperPage_listItem (me, L"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	for (long icons = 1; icons <= grammar -> numberOfConstraints; icons ++) {
		OTConstraint constraint = & grammar -> constraints [grammar -> index [icons]];
		MelderString_empty (& buffer);
		MelderString_append8 (& buffer, L"\t", icons == my selectedConstraint ? L"\\sp " : L"   ", L"@@", Melder_integer (icons),
			L"|", constraint -> name, L"@\t      ", Melder_fixed (constraint -> ranking, 3));
		MelderString_append2 (& buffer, L"\t      ", Melder_fixed (constraint -> disharmony, 3));
		MelderString_append2 (& buffer, L"\t      ", Melder_fixed (constraint -> plasticity, 6));
		HyperPage_listItem (me, buffer.string);
	}
	Graphics_setAtSignIsLink (my g, FALSE);
	drawTableau_grammar = grammar;
	for (long icand = 1; icand <= grammar -> numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (grammar, icand, my form1, my form2)) {
			tableauHeight += rowHeight;
		}
	}
	drawTableau_form1 = my form1;
	drawTableau_form2 = my form2;
	HyperPage_picture (me, 20, tableauHeight, drawTableau);
	Graphics_setAtSignIsLink (my g, TRUE);
}

static int goToPage (I, const wchar_t *title) {
	iam (OTMultiEditor);
	if (! title) return 1;
	my selectedConstraint = wcstol (title, NULL, 10);
	return 1;
}

class_methods (OTMultiEditor, HyperPage)
	class_method (createChildren)
	class_method (createMenus)
	class_method (draw)
	us -> editable = TRUE;
	class_method (goToPage)
class_methods_end

OTMultiEditor OTMultiEditor_create (Widget parent, const wchar_t *title, OTMulti grammar) {
	OTMultiEditor me = new (OTMultiEditor);
	my data = grammar;
	my form1 = Melder_wcsdup (L"");
	my form2 = Melder_wcsdup (L"");
	if (! HyperPage_init (me, parent, title, grammar))
		{ forget (me); return Melder_errorp ("OTMultiEditor not created."); }
	return me;
}

/* End of file OTGrammarEditor.c */
