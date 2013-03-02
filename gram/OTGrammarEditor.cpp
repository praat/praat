/* OTGrammarEditor.cpp
 *
 * Copyright (C) 1997-2011,2012,2013 Paul Boersma
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
 * pb 2003/05/27 learnOne and learnOneFromPartialOutput
 * pb 2008/05/31 ExponentialMaximumEntropy
 */

#include "OTGrammarEditor.h"
#include "EditorM.h"

Thing_implement (OTGrammarEditor, HyperPage, 0);

static void menu_cb_evaluate (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	EDITOR_FORM (L"Evaluate", 0)
		REAL (L"Noise", L"2.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Evaluate");
		OTGrammar_newDisharmonies ((OTGrammar) my data, GET_REAL (L"Noise"));
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_evaluate_noise_2_0 (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	Editor_save (me, L"Evaluate (noise 2.0)");
	OTGrammar_newDisharmonies ((OTGrammar) my data, 2.0);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_evaluate_tinyNoise (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	Editor_save (me, L"Evaluate (tiny noise)");
	OTGrammar_newDisharmonies ((OTGrammar) my data, 1e-9);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_evaluate_zeroNoise (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	Editor_save (me, L"Evaluate (zero noise)");
	OTGrammar_newDisharmonies ((OTGrammar) my data, 0.0);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_editConstraint (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	EDITOR_FORM (L"Edit constraint", 0)
		LABEL (L"constraint", L"");
		REAL (L"Ranking value", L"100.0");
		REAL (L"Disharmony", L"100.0");
		REAL (L"Plasticity", L"1.0");
	EDITOR_OK
		OTGrammar ot = (OTGrammar) my data;
		OTGrammarConstraint constraint;
		if (my selected < 1 || my selected > ot -> numberOfConstraints)
			Melder_throw ("Select a constraint first.");
		constraint = & ot -> constraints [ot -> index [my selected]];
		SET_STRING (L"constraint", constraint -> name)
		SET_REAL (L"Ranking value", constraint -> ranking)
		SET_REAL (L"Disharmony", constraint -> disharmony)
		SET_REAL (L"Plasticity", constraint -> plasticity)
	EDITOR_DO
		OTGrammar ot = (OTGrammar) my data;
		OTGrammarConstraint constraint = & ot -> constraints [ot -> index [my selected]];
		Editor_save (me, L"Edit constraint");
		constraint -> ranking = GET_REAL (L"Ranking value");
		constraint -> disharmony = GET_REAL (L"Disharmony");
		constraint -> plasticity = GET_REAL (L"Plasticity");
		OTGrammar_sort (ot);
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_learnOne (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	EDITOR_FORM (L"Learn one", L"OTGrammar: Learn one...")
		LABEL (L"", L"Underlying form:")
		SENTENCE (L"Input string", L"")
		LABEL (L"", L"Adult surface form:")
		SENTENCE (L"Output string", L"")
		REAL (L"Evaluation noise", L"2.0")
		OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, kOTGrammar_rerankingStrategy_SYMMETRIC_ALL)
		REAL (L"Plasticity", L"0.1")
		REAL (L"Rel. plasticity spreading", L"0.1")
		BOOLEAN (L"Honour local rankings", 1)
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Learn one");
		OTGrammar_learnOne ((OTGrammar) my data, GET_STRING (L"Input string"), GET_STRING (L"Output string"),
			GET_REAL (L"Evaluation noise"), GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"), GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), TRUE, TRUE, NULL);
		OTGrammar_sort ((OTGrammar) my data);
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_learnOneFromPartialOutput (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	EDITOR_FORM (L"Learn one from partial adult output", 0)
		LABEL (L"", L"Partial adult surface form (e.g. overt form):")
		SENTENCE (L"Partial output", L"")
		REAL (L"Evaluation noise", L"2.0")
		OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, kOTGrammar_rerankingStrategy_SYMMETRIC_ALL)
		REAL (L"Plasticity", L"0.1")
		REAL (L"Rel. plasticity spreading", L"0.1")
		BOOLEAN (L"Honour local rankings", 1)
		NATURAL (L"Number of chews", L"1")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Learn one from partial output");
		OTGrammar_learnOneFromPartialOutput ((OTGrammar) my data, GET_STRING (L"Partial output"),
			GET_REAL (L"Evaluation noise"), GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"), GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"), TRUE);
		OTGrammar_sort ((OTGrammar) my data);
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_removeConstraint (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	OTGrammar ot = (OTGrammar) my data;
	OTGrammarConstraint constraint;
	if (my selected < 1 || my selected > ot -> numberOfConstraints)
		Melder_throw ("Select a constraint first.");
	constraint = & ot -> constraints [ot -> index [my selected]];
	Editor_save (me, L"Remove constraint");
	OTGrammar_removeConstraint (ot, constraint -> name);
	Graphics_updateWs (my g);
	my broadcastDataChanged ();
}

static void menu_cb_resetAllRankings (EDITOR_ARGS) {
	EDITOR_IAM (OTGrammarEditor);
	EDITOR_FORM (L"Reset all rankings", 0)
		REAL (L"Ranking", L"100.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, L"Reset all rankings");
		OTGrammar_reset ((OTGrammar) my data, GET_REAL (L"Ranking"));
		Graphics_updateWs (my g);
		my broadcastDataChanged ();
	EDITOR_END
}

static void menu_cb_OTGrammarEditor_help (EDITOR_ARGS) { EDITOR_IAM (OTGrammarEditor); Melder_help (L"OTGrammarEditor"); }
static void menu_cb_OTGrammar_help (EDITOR_ARGS) { EDITOR_IAM (OTGrammarEditor); Melder_help (L"OTGrammar"); }
static void menu_cb_OTLearningTutorial (EDITOR_ARGS) { EDITOR_IAM (OTGrammarEditor); Melder_help (L"OT learning"); }

void structOTGrammarEditor :: v_createMenus () {
	OTGrammarEditor_Parent :: v_createMenus ();
	Editor_addCommand (this, L"Edit", L"-- edit ot --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Evaluate...", 0, menu_cb_evaluate);
	Editor_addCommand (this, L"Edit", L"Evaluate (noise 2.0)", '2', menu_cb_evaluate_noise_2_0);
	Editor_addCommand (this, L"Edit", L"Evaluate (zero noise)", '0', menu_cb_evaluate_zeroNoise);
	Editor_addCommand (this, L"Edit", L"Evaluate (tiny noise)", '9', menu_cb_evaluate_tinyNoise);
	Editor_addCommand (this, L"Edit", L"Edit constraint...", 'E', menu_cb_editConstraint);
	Editor_addCommand (this, L"Edit", L"Reset all rankings...", 'R', menu_cb_resetAllRankings);
	Editor_addCommand (this, L"Edit", L"Learn one...", 0, menu_cb_learnOne);
	Editor_addCommand (this, L"Edit", L"Learn one from partial output...", '1', menu_cb_learnOneFromPartialOutput);
	Editor_addCommand (this, L"Edit", L"-- remove ot --", 0, NULL);
	Editor_addCommand (this, L"Edit", L"Remove constraint", 0, menu_cb_removeConstraint);
}

void structOTGrammarEditor :: v_createHelpMenuItems (EditorMenu menu) {
	OTGrammarEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, L"OTGrammarEditor help", '?', menu_cb_OTGrammarEditor_help);
	EditorMenu_addCommand (menu, L"OTGrammar help", 0, menu_cb_OTGrammar_help);
	EditorMenu_addCommand (menu, L"OT learning tutorial", 0, menu_cb_OTLearningTutorial);
}

static OTGrammar drawTableau_ot;
static const wchar_t *drawTableau_input;
static bool drawTableau_constraintsAreDrawnVertically;
static void drawTableau (Graphics g) {
	OTGrammar_drawTableau (drawTableau_ot, g, drawTableau_constraintsAreDrawnVertically, drawTableau_input);
}

void structOTGrammarEditor :: v_draw () {
	OTGrammar ot = (OTGrammar) data;
	static wchar_t text [1000];
	Graphics_clearWs (g);
	if (ot -> decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
		ot -> decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		HyperPage_listItem (this, L"\t\t      %%ranking value\t      %disharmony\t      %plasticity\t   %%e^^disharmony");
	} else {
		HyperPage_listItem (this, L"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	}
	for (long icons = 1; icons <= ot -> numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & ot -> constraints [ot -> index [icons]];
		if (ot -> decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_HG ||
			ot -> decisionStrategy == kOTGrammar_decisionStrategy_EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			swprintf (text, 1000, L"\t%ls@@%ld|%ls@\t      %.3f\t      %.3f\t      %.6f\t %ls",
				icons == selected ? L"\\sp " : L"   ", icons, constraint -> name,
				constraint -> ranking, constraint -> disharmony, constraint -> plasticity,
				Melder_float (Melder_half (exp (constraint -> disharmony))));
		} else {
			swprintf (text, 1000, L"\t%ls@@%ld|%ls@\t      %.3f\t      %.3f\t      %.6f",
				icons == selected ? L"\\sp " : L"   ", icons, constraint -> name,
				constraint -> ranking, constraint -> disharmony, constraint -> plasticity);
		}
		HyperPage_listItem (this, text);
	}
	Graphics_setAtSignIsLink (g, FALSE);
	for (long itab = 1; itab <= ot -> numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & ot -> tableaus [itab];
		double rowHeight = 0.25;
		double tableauHeight = rowHeight * (tableau -> numberOfCandidates + 2);
		drawTableau_ot = ot;
		drawTableau_input = tableau -> input;
		drawTableau_constraintsAreDrawnVertically = d_constraintsAreDrawnVertically;
		HyperPage_picture (this, 20, tableauHeight, drawTableau);
	}
	Graphics_setAtSignIsLink (g, TRUE);
}

int structOTGrammarEditor :: v_goToPage (const wchar_t *title) {
	if (title == NULL) return 1;
	selected = wcstol (title, NULL, 10);
	return 1;
}

OTGrammarEditor OTGrammarEditor_create (const wchar_t *title, OTGrammar ot) {
	try {
		autoOTGrammarEditor me = Thing_new (OTGrammarEditor);
		my data = ot;
		HyperPage_init (me.peek(), title, ot);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("OTGrammar window not created.");
	}
}

/* End of file OTGrammarEditor.cpp */
