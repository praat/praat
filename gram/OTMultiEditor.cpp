/* OTMultiEditor.cpp
 *
 * Copyright (C) 2005-2011,2012,2013 Paul Boersma
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
 * a selection of changes:
 * pb 2005/07/04 created
 * pb 2007/10/01 constraint plasticity
 * pb 2011/03/01 multiple update rules
 */

#include "OTMultiEditor.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (OTMultiEditor, HyperPage, 0);

static void menu_cb_evaluate (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Evaluate", 0)
		REAL (L"Evaluation noise", L"2.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Evaluate");
		OTMulti_newDisharmonies ((OTMulti) my data, GET_REAL (L"Evaluation noise"));
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_evaluate_noise_2_0 (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	Editor_save (me, L"Evaluate (noise 2.0)");
	OTMulti_newDisharmonies ((OTMulti) my data, 2.0);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_evaluate_tinyNoise (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	Editor_save (me, L"Evaluate (tiny noise)");
	OTMulti_newDisharmonies ((OTMulti) my data, 1e-9);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_editRanking (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Edit ranking", 0)
		LABEL (L"constraint", L"");
		REAL (L"Ranking value", L"100.0");
		REAL (L"Disharmony", L"100.0");
	EDITOR_OK
		OTMulti grammar = (OTMulti) my data;
		OTConstraint constraint;
		if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
			Melder_throw ("Select a constraint first.");
		constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		SET_STRING (L"constraint", constraint -> name)
		SET_REAL (L"Ranking value", constraint -> ranking)
		SET_REAL (L"Disharmony", constraint -> disharmony)
	EDITOR_DO
		OTMulti grammar = (OTMulti) my data;
		OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		Editor_save (me, L"Edit ranking");
		constraint -> ranking = GET_REAL (L"Ranking value");
		constraint -> disharmony = GET_REAL (L"Disharmony");
		OTMulti_sort (grammar);
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_learnOne (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Learn one", L"OTGrammar: Learn one...")
		OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, kOTGrammar_rerankingStrategy_SYMMETRIC_ALL)
		OPTIONMENU (L"Direction", 3)
			OPTION (L"forward")
			OPTION (L"backward")
			OPTION (L"bidirectionally")
		REAL (L"Plasticity", L"0.1")
		REAL (L"Rel. plasticity spreading", L"0.1")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Learn one");
		Melder_free (my form1);
		Melder_free (my form2);
		my form1 = my form1Text -> f_getString ();
		my form2 = my form2Text -> f_getString ();
		OTMulti_learnOne ((OTMulti) my data, my form1, my form2,
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"), GET_INTEGER (L"Direction"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"));
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_removeConstraint (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	OTMulti grammar = (OTMulti) my data;
	if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
		Melder_throw ("Select a constraint first.");
	OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, L"Remove constraint");
	OTMulti_removeConstraint (grammar, constraint -> name);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_resetAllRankings (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Reset all rankings", 0)
		REAL (L"Ranking", L"100.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Reset all rankings");
		OTMulti_reset ((OTMulti) my data, GET_REAL (L"Ranking"));
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_OTLearningTutorial (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	(void) me;
	Melder_help (L"OT learning");
}

static void do_limit (OTMultiEditor me) {
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = my form1Text -> f_getString ();
	my form2 = my form2Text -> f_getString ();
	Graphics_updateWs (my g);
}

static void gui_button_cb_limit (I, GuiButtonEvent event) {
	(void) event;
	iam (OTMultiEditor);
	do_limit (me);
}

static void gui_cb_limit (GUI_ARGS) {
	GUI_IAM (OTMultiEditor);
	do_limit (me);
}

void structOTMultiEditor :: v_createChildren () {
	OTMultiEditor_Parent :: v_createChildren ();
	#if defined (macintosh)
		#define STRING_SPACING 8
	#else
		#define STRING_SPACING 2
	#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	GuiButton_createShown (d_windowForm, 4, 124, y, y + height,
		L"Partial forms:", gui_button_cb_limit, this, GuiButton_DEFAULT);
	form1Text = GuiText_createShown (d_windowForm,
		124 + STRING_SPACING, 274 + STRING_SPACING, y, y + Gui_TEXTFIELD_HEIGHT, 0);
	form2Text = GuiText_createShown (d_windowForm,
		274 + 2 * STRING_SPACING, 424 + 2 * STRING_SPACING, y, y + Gui_TEXTFIELD_HEIGHT, 0);
}

void structOTMultiEditor :: v_createMenus () {
	OTMultiEditor_Parent :: v_createMenus ();
	Editor_addCommand (this, L"Edit", L"-- edit ot --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Evaluate...", 0, menu_cb_evaluate);
	Editor_addCommand (this, L"Edit", L"Evaluate (noise 2.0)", '2', menu_cb_evaluate_noise_2_0);
	Editor_addCommand (this, L"Edit", L"Evaluate (tiny noise)", '9', menu_cb_evaluate_tinyNoise);
	Editor_addCommand (this, L"Edit", L"Edit ranking...", 'E', menu_cb_editRanking);
	Editor_addCommand (this, L"Edit", L"Reset all rankings...", 'R', menu_cb_resetAllRankings);
	Editor_addCommand (this, L"Edit", L"Learn one...", '1', menu_cb_learnOne);
	Editor_addCommand (this, L"Edit", L"-- remove --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Remove constraint", 0, menu_cb_removeConstraint);
}

void structOTMultiEditor :: v_createHelpMenuItems (EditorMenu menu) {
	OTMultiEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"OT learning tutorial", 0, menu_cb_OTLearningTutorial);
}

static OTMulti drawTableau_grammar;
static const wchar_t *drawTableau_form1, *drawTableau_form2;
static bool drawTableau_constraintsAreDrawnVertically;
static void drawTableau (Graphics g) {
	OTMulti_drawTableau (drawTableau_grammar, g, drawTableau_form1, drawTableau_form2, drawTableau_constraintsAreDrawnVertically, true);
}

void structOTMultiEditor :: v_draw () {
	OTMulti grammar = (OTMulti) data;
	static MelderString buffer = { 0 };
	double rowHeight = 0.25, tableauHeight = 2 * rowHeight;
	Graphics_clearWs (g);
	HyperPage_listItem (this, L"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	for (long icons = 1; icons <= grammar -> numberOfConstraints; icons ++) {
		OTConstraint constraint = & grammar -> constraints [grammar -> index [icons]];
		MelderString_empty (& buffer);
		MelderString_append (& buffer, L"\t", icons == selectedConstraint ? L"\\sp " : L"   ", L"@@", Melder_integer (icons),
			L"|", constraint -> name, L"@\t      ", Melder_fixed (constraint -> ranking, 3));
		MelderString_append (& buffer, L"\t      ", Melder_fixed (constraint -> disharmony, 3));
		MelderString_append (& buffer, L"\t      ", Melder_fixed (constraint -> plasticity, 6));
		HyperPage_listItem (this, buffer.string);
	}
	Graphics_setAtSignIsLink (g, FALSE);
	drawTableau_grammar = grammar;
	for (long icand = 1; icand <= grammar -> numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (grammar, icand, form1, form2)) {
			tableauHeight += rowHeight;
		}
	}
	drawTableau_form1 = form1;
	drawTableau_form2 = form2;
	drawTableau_constraintsAreDrawnVertically = d_constraintsAreDrawnVertically;
	HyperPage_picture (this, 20, tableauHeight, drawTableau);
	Graphics_setAtSignIsLink (g, TRUE);
}

int structOTMultiEditor :: v_goToPage (const wchar_t *title) {
	if (title == NULL) return 1;
	selectedConstraint = wcstol (title, NULL, 10);
	return 1;
}

OTMultiEditor OTMultiEditor_create (const wchar_t *title, OTMulti grammar) {
	try {
		autoOTMultiEditor me = Thing_new (OTMultiEditor);
		my data = grammar;
		my form1 = Melder_wcsdup (L"");
		my form2 = Melder_wcsdup (L"");
		HyperPage_init (me.peek(), title, grammar);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("OTMulti window not created.");
	}
}

/* End of file OTMultiEditor.cpp */
