/* OTMultiEditor.cpp
 *
 * Copyright (C) 2005-2020 Paul Boersma
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
 * pb 2005/07/04 created
 * pb 2007/10/01 constraint plasticity
 * pb 2011/03/01 multiple update rules
 */

#include "OTMultiEditor.h"
#include "EditorM.h"
#include "machine.h"

Thing_implement (OTMultiEditor, HyperPage, 0);

static void menu_cb_evaluate (OTMultiEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Evaluate", nullptr)
		REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Evaluate");
		OTMulti_newDisharmonies ((OTMulti) my data, evaluationNoise);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_evaluate_noise_2_0 (OTMultiEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Evaluate (noise 2.0)");
	OTMulti_newDisharmonies ((OTMulti) my data, 2.0);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_evaluate_tinyNoise (OTMultiEditor me, EDITOR_ARGS_DIRECT) {
	Editor_save (me, U"Evaluate (tiny noise)");
	OTMulti_newDisharmonies ((OTMulti) my data, 1e-9);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_editRanking (OTMultiEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Edit ranking", nullptr)
		MUTABLE_LABEL (constraintLabel, U"")
		REAL (rankingValue, U"Ranking value", U"100.0")
		REAL (disharmony, U"Disharmony", U"100.0")
	EDITOR_OK
		OTMulti grammar = (OTMulti) my data;
		OTConstraint constraint;
		if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
			Melder_throw (U"Select a constraint first.");
		constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		SET_STRING (constraintLabel, constraint -> name.get())
		SET_REAL (rankingValue, constraint -> ranking)
		SET_REAL (disharmony, constraint -> disharmony)
	EDITOR_DO
		OTMulti grammar = (OTMulti) my data;
		OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		Editor_save (me, U"Edit ranking");
		constraint -> ranking = rankingValue;
		constraint -> disharmony = disharmony;
		OTMulti_sort (grammar);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_learnOne (OTMultiEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Learn one", U"OTGrammar: Learn one...")
		OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
				U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
		OPTIONMENU (direction, U"Direction", 3)
			OPTION (U"forward")
			OPTION (U"backward")
			OPTION (U"bidirectionally")
		REAL (plasticity, U"Plasticity", U"0.1")
		REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Learn one");
		my form1 = GuiText_getString (my form1Text);
		my form2 = GuiText_getString (my form2Text);
		OTMulti_learnOne ((OTMulti) my data, my form1.get(), my form2.get(),
			updateRule, direction,
			plasticity, relativePlasticitySpreading);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_removeConstraint (OTMultiEditor me, EDITOR_ARGS_DIRECT) {
	OTMulti grammar = (OTMulti) my data;
	if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
		Melder_throw (U"Select a constraint first.");
	OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, U"Remove constraint");
	OTMulti_removeConstraint (grammar, constraint -> name.get());
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void menu_cb_resetAllRankings (OTMultiEditor me, EDITOR_ARGS_FORM) {
	EDITOR_FORM (U"Reset all rankings", nullptr)
		REAL (ranking, U"Ranking", U"100.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (me, U"Reset all rankings");
		OTMulti_reset ((OTMulti) my data, ranking);
		Graphics_updateWs (my graphics.get());
		Editor_broadcastDataChanged (me);
	EDITOR_END
}

static void menu_cb_OTLearningTutorial (OTMultiEditor, EDITOR_ARGS_DIRECT) {
	Melder_help (U"OT learning");
}

static void do_limit (OTMultiEditor me) {
	my form1 = GuiText_getString (my form1Text);
	my form2 = GuiText_getString (my form2Text);
	Graphics_updateWs (my graphics.get());
}

static void gui_button_cb_limit (OTMultiEditor me, GuiButtonEvent /* event */) {
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
	GuiButton_createShown (our windowForm, 4, 124, y, y + height,
		U"Partial forms:", gui_button_cb_limit, this, GuiButton_DEFAULT);
	form1Text = GuiText_createShown (our windowForm,
		124 + STRING_SPACING, 274 + STRING_SPACING, y, y + Gui_TEXTFIELD_HEIGHT, 0);
	form2Text = GuiText_createShown (our windowForm,
		274 + 2 * STRING_SPACING, 424 + 2 * STRING_SPACING, y, y + Gui_TEXTFIELD_HEIGHT, 0);
}

void structOTMultiEditor :: v_createMenus () {
	OTMultiEditor_Parent :: v_createMenus ();
	Editor_addCommand (this, U"Edit", U"-- edit ot --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Evaluate...", 0, menu_cb_evaluate);
	Editor_addCommand (this, U"Edit", U"Evaluate (noise 2.0)", '2', menu_cb_evaluate_noise_2_0);
	Editor_addCommand (this, U"Edit", U"Evaluate (tiny noise)", '9', menu_cb_evaluate_tinyNoise);
	Editor_addCommand (this, U"Edit", U"Edit ranking...", 'E', menu_cb_editRanking);
	Editor_addCommand (this, U"Edit", U"Reset all rankings...", 'R', menu_cb_resetAllRankings);
	Editor_addCommand (this, U"Edit", U"Learn one...", '1', menu_cb_learnOne);
	Editor_addCommand (this, U"Edit", U"-- remove --", 0, nullptr);
	Editor_addCommand (this, U"Edit", U"Remove constraint", 0, menu_cb_removeConstraint);
}

void structOTMultiEditor :: v_createHelpMenuItems (EditorMenu menu) {
	OTMultiEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"OT learning tutorial", 0, menu_cb_OTLearningTutorial);
}

static OTMulti drawTableau_grammar;
static conststring32 drawTableau_form1, drawTableau_form2;
static bool drawTableau_constraintsAreDrawnVertically;
static void drawTableau (Graphics g) {
	OTMulti_drawTableau (drawTableau_grammar, g, drawTableau_form1, drawTableau_form2, drawTableau_constraintsAreDrawnVertically, true);
}

void structOTMultiEditor :: v_draw () {
	const OTMulti grammar = (OTMulti) data;
	static MelderString buffer;
	const double rowHeight = 0.25;
	longdouble tableauHeight = 2 * rowHeight;
	HyperPage_listItem (this, U"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	for (integer icons = 1; icons <= grammar -> numberOfConstraints; icons ++) {
		const OTConstraint constraint = & grammar -> constraints [grammar -> index [icons]];
		MelderString_copy (& buffer, U"\t", ( icons == selectedConstraint ? U"♠︎ " : U"   " ), U"@@", icons,
			U"|", constraint -> name.get(), U"@\t      ", Melder_fixed (constraint -> ranking, 3),
			U"\t      ", Melder_fixed (constraint -> disharmony, 3),
			U"\t      ", Melder_fixed (constraint -> plasticity, 6)
		);
		HyperPage_listItem (this, buffer.string);
	}
	Graphics_setAtSignIsLink (graphics.get(), false);
	drawTableau_grammar = grammar;
	for (integer icand = 1; icand <= grammar -> numberOfCandidates; icand ++)
		if (OTMulti_candidateMatches (grammar, icand, our form1.get(), our form2.get()))
			tableauHeight += rowHeight;
	drawTableau_form1 = our form1.get();   // BUG: dangle
	drawTableau_form2 = our form2.get();
	drawTableau_constraintsAreDrawnVertically = d_constraintsAreDrawnVertically;
	HyperPage_picture (this, 20, tableauHeight, drawTableau);
	Graphics_setAtSignIsLink (graphics.get(), true);
}

int structOTMultiEditor :: v_goToPage (conststring32 title) {
	if (! title)
		return 1;
	selectedConstraint = Melder_atoi (title);
	return 1;
}

autoOTMultiEditor OTMultiEditor_create (conststring32 title, OTMulti grammar) {
	try {
		autoOTMultiEditor me = Thing_new (OTMultiEditor);
		my data = grammar;
		my form1 = Melder_dup (U"");
		my form2 = Melder_dup (U"");
		HyperPage_init (me.get(), title, grammar);
		return me;
	} catch (MelderError) {
		Melder_throw (U"OTMulti window not created.");
	}
}

/* End of file OTMultiEditor.cpp */
