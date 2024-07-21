/* OTGrammarEditor.cpp
 *
 * Copyright (C) 1997-2005,2007-2020,2022,2024 Paul Boersma
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

/*
 * a selection of changes:
 * pb 2003/05/27 learnOne and learnOneFromPartialOutput
 * pb 2008/05/31 ExponentialMaximumEntropy
 */

#include "OTGrammarEditor.h"
#include "EditorM.h"

Thing_implement (OTGrammarEditor, HyperPage, 0);

static void menu_cb_evaluate (OTGrammarEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Evaluate", nullptr)
		REAL (noise, U"Noise", U"2.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Evaluate");
		OTGrammar_newDisharmonies (my otGrammar(), noise);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_evaluate_noise_2_0 (OTGrammarEditor me, EDITOR_ARGS) {
	Editor_save (me, U"Evaluate (noise 2.0)");
	OTGrammar_newDisharmonies (my otGrammar(), 2.0);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_evaluate_tinyNoise (OTGrammarEditor me, EDITOR_ARGS) {
	Editor_save (me, U"Evaluate (tiny noise)");
	OTGrammar_newDisharmonies (my otGrammar(), 1e-9);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_evaluate_zeroNoise (OTGrammarEditor me, EDITOR_ARGS) {
	Editor_save (me, U"Evaluate (zero noise)");
	OTGrammar_newDisharmonies (my otGrammar(), 0.0);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_editConstraint (OTGrammarEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Edit constraint", nullptr)
		MUTABLE_COMMENT (constraintLabel, U"");
		REAL (rankingValue, U"Ranking value", U"100.0");
		REAL (disharmony, U"Disharmony", U"100.0");
		REAL (plasticity, U"Plasticity", U"1.0");
	EDITOR_OK
		if (my selected < 1 || my selected > my otGrammar() -> numberOfConstraints)
			Melder_throw (U"Select a constraint first.");
		OTGrammarConstraint constraint = & my otGrammar() -> constraints [my otGrammar() -> index [my selected]];
		SET_STRING (constraintLabel, constraint -> name.get())
		SET_REAL (rankingValue, constraint -> ranking)
		SET_REAL (disharmony, constraint -> disharmony)
		SET_REAL (plasticity, constraint -> plasticity)
	EDITOR_DO
		OTGrammarConstraint constraint = & my otGrammar() -> constraints [my otGrammar() -> index [my selected]];
		Editor_save (me, U"Edit constraint");
		constraint -> ranking = rankingValue;
		constraint -> disharmony = disharmony;
		constraint -> plasticity = plasticity;
		OTGrammar_sort (my otGrammar());
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_learnOne (OTGrammarEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Learn one", U"OTGrammar: Learn one...")
		COMMENT (U"Underlying form:")
		SENTENCE (inputString, U"Input string", U"")
		COMMENT (U"Adult surface form:")
		SENTENCE (outputString, U"Output string", U"")
		REAL (evaluationNoise, U"Evaluation noise", U"2.0")
		OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
				U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
		REAL (plasticity, U"Plasticity", U"0.1")
		REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
		BOOLEAN (honourLocalRankings, U"Honour local rankings", true)
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Learn one");
		OTGrammar_learnOne (my otGrammar(), inputString, outputString,
			evaluationNoise, updateRule, honourLocalRankings,
			plasticity, relativePlasticitySpreading, true, true, nullptr);
		OTGrammar_sort (my otGrammar());
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_learnOneFromPartialOutput (OTGrammarEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Learn one from partial adult output", nullptr)
		COMMENT (U"Partial adult surface form (e.g. overt form):")
		SENTENCE (partialOutput, U"Partial output", U"")
		REAL (evaluationNoise, U"Evaluation noise", U"2.0")
		OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
				U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
		REAL (plasticity, U"Plasticity", U"0.1")
		REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
		BOOLEAN (honourLocalRankings, U"Honour local rankings", true)
		NATURAL (numberOfChews, U"Number of chews", U"1")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Learn one from partial output");
		OTGrammar_learnOneFromPartialOutput (my otGrammar(), partialOutput,
			evaluationNoise, updateRule, honourLocalRankings,
			plasticity, relativePlasticitySpreading, numberOfChews, true);
		OTGrammar_sort (my otGrammar());
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_removeConstraint (OTGrammarEditor me, EDITOR_ARGS) {
	OTGrammarConstraint constraint;
	if (my selected < 1 || my selected > my otGrammar() -> numberOfConstraints)
		Melder_throw (U"Select a constraint first.");
	constraint = & my otGrammar() -> constraints [my otGrammar() -> index [my selected]];
	Editor_save (me, U"Remove constraint");
	OTGrammar_removeConstraint (my otGrammar(), constraint -> name.get());
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_resetAllRankings (OTGrammarEditor me, EDITOR_ARGS) {
	EDITOR_FORM (U"Reset all rankings", nullptr)
		REAL (ranking, U"Ranking", U"100.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Reset all rankings");
		OTGrammar_reset (my otGrammar(), ranking);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_OTGrammarEditor_help (OTGrammarEditor, EDITOR_ARGS) { Melder_help (U"OTGrammarEditor"); }
static void menu_cb_OTGrammar_help (OTGrammarEditor, EDITOR_ARGS) { Melder_help (U"OTGrammar"); }
static void menu_cb_OTLearningTutorial (OTGrammarEditor, EDITOR_ARGS) { Melder_help (U"OT learning"); }

void structOTGrammarEditor :: v_createMenus () {
	OTGrammarEditor_Parent :: v_createMenus ();
	Editor_addCommand (this, U"Edit", U"-- edit ot --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Evaluate...", 0, menu_cb_evaluate);
	Editor_addCommand (this, U"Edit", U"Evaluate (noise 2.0)", '2', menu_cb_evaluate_noise_2_0);
	Editor_addCommand (this, U"Edit", U"Evaluate (zero noise)", '0', menu_cb_evaluate_zeroNoise);
	Editor_addCommand (this, U"Edit", U"Evaluate (tiny noise)", '9', menu_cb_evaluate_tinyNoise);
	Editor_addCommand (this, U"Edit", U"Edit constraint...", 'E', menu_cb_editConstraint);
	Editor_addCommand (this, U"Edit", U"Reset all rankings...", 'R', menu_cb_resetAllRankings);
	Editor_addCommand (this, U"Edit", U"Learn one...", 0, menu_cb_learnOne);
	Editor_addCommand (this, U"Edit", U"Learn one from partial output...", '1', menu_cb_learnOneFromPartialOutput);
	Editor_addCommand (this, U"Edit", U"-- remove ot --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Remove constraint", 0, menu_cb_removeConstraint);
}

void structOTGrammarEditor :: v_createMenuItems_help (EditorMenu menu) {
	OTGrammarEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"OTGrammarEditor help", '?', menu_cb_OTGrammarEditor_help);
	EditorMenu_addCommand (menu, U"OTGrammar help", 0, menu_cb_OTGrammar_help);
	EditorMenu_addCommand (menu, U"OT learning tutorial", 0, menu_cb_OTLearningTutorial);
}

static OTGrammar drawTableau_ot;
static conststring32 drawTableau_input;
static bool drawTableau_constraintsAreDrawnVertically;
static void drawTableau (Graphics g) {
	OTGrammar_drawTableau (drawTableau_ot, g, drawTableau_constraintsAreDrawnVertically, drawTableau_input);
}

void structOTGrammarEditor :: v_draw () {
	static char32 text [1000];
	if (our otGrammar() -> decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
		our otGrammar() -> decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
	{
		HyperPage_listItem (this, U"\t\t      %%ranking value\t      %disharmony\t      %plasticity\t   %%e^^disharmony");
	} else {
		HyperPage_listItem (this, U"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	}
	for (integer icons = 1; icons <= our otGrammar() -> numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & our otGrammar() -> constraints [our otGrammar() -> index [icons]];
		if (our otGrammar() -> decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_HG ||
			our otGrammar() -> decisionStrategy == kOTGrammar_decisionStrategy::EXPONENTIAL_MAXIMUM_ENTROPY)
		{
			Melder_sprint (text,1000,
				U"\t", icons == selected ? U"♠︎ " : U"   ",
				U"@@", icons,
				U"|", constraint -> name.get(),
				U"@\t      ", Melder_fixed (constraint -> ranking, 3),
				U"\t      ", Melder_fixed (constraint -> disharmony, 3),
				U"\t      ", Melder_fixed (constraint -> plasticity, 6),
				U"\t ", Melder_float (Melder_half (exp (constraint -> disharmony))));
		} else {
			Melder_sprint (text,1000,
				U"\t", icons == selected ? U"♠︎ " : U"   ",
				U"@@", icons,
				U"|", constraint -> name.get(),
				U"@\t      ", Melder_fixed (constraint -> ranking, 3),
				U"\t      ", Melder_fixed (constraint -> disharmony, 3),
				U"\t      ", Melder_fixed (constraint -> plasticity, 6));
		}
		HyperPage_listItem (this, text);
	}
	Graphics_setAtSignIsLink (graphics.get(), false);
	for (integer itab = 1; itab <= our otGrammar() -> numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & our otGrammar() -> tableaus [itab];
		double rowHeight = 0.25;
		double tableauHeight = rowHeight * (tableau -> numberOfCandidates + 2);
		drawTableau_ot = our otGrammar();
		drawTableau_input = tableau -> input.get();
		drawTableau_constraintsAreDrawnVertically = d_constraintsAreDrawnVertically;
		HyperPage_picture (this, 20, tableauHeight, drawTableau);
	}
	Graphics_setAtSignIsLink (graphics.get(), true);
}

int structOTGrammarEditor :: v_goToPage (conststring32 title) {
	if (! title)
		return 1;
	selected = Melder_atoi (title);
	return 1;
}

autoOTGrammarEditor OTGrammarEditor_create (conststring32 title, OTGrammar otGrammar) {
	try {
		autoOTGrammarEditor me = Thing_new (OTGrammarEditor);
		HyperPage_init (me.get(), title, otGrammar);
		return me;
	} catch (MelderError) {
		Melder_throw (U"OTGrammar window not created.");
	}
}

/* End of file OTGrammarEditor.cpp */
