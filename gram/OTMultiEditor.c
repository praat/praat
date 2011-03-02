/* OTMultiEditor.c
 *
 * Copyright (C) 2005-2011 Paul Boersma
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
 * pb 2008/03/20 split off Help menu
 * pb 2011/03/01 multiple update rules
 */

#include "OTMultiEditor.h"
#include "EditorM.h"
#include "machine.h"

#define OTMultiEditor__members(Klas) HyperPage__members(Klas) \
	const wchar_t *form1, *form2; \
	GuiObject form1Text, form2Text; \
	long selectedConstraint;
#define OTMultiEditor__methods(Klas) HyperPage__methods(Klas)
Thing_declare2 (OTMultiEditor, HyperPage);

static int menu_cb_evaluate (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Evaluate", 0)
		REAL (L"Evaluation noise", L"2.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (OTMultiEditor_as_Editor (me), L"Evaluate");
		OTMulti_newDisharmonies (my data, GET_REAL (L"Evaluation noise"));
		Graphics_updateWs (my g);
		Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_evaluate_noise_2_0 (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	Editor_save (OTMultiEditor_as_Editor (me), L"Evaluate (noise 2.0)");
	OTMulti_newDisharmonies (my data, 2.0);
	Graphics_updateWs (my g);
	Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	return 1;
}

static int menu_cb_evaluate_tinyNoise (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	Editor_save (OTMultiEditor_as_Editor (me), L"Evaluate (tiny noise)");
	OTMulti_newDisharmonies (my data, 1e-9);
	Graphics_updateWs (my g);
	Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	return 1;
}

static int menu_cb_editRanking (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Edit ranking", 0)
		LABEL (L"constraint", L"");
		REAL (L"Ranking value", L"100.0");
		REAL (L"Disharmony", L"100.0");
	EDITOR_OK
		OTMulti grammar = my data;
		OTConstraint constraint;
		if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints) return Melder_error1 (L"Select a constraint first.");
		constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		SET_STRING (L"constraint", constraint -> name)
		SET_REAL (L"Ranking value", constraint -> ranking)
		SET_REAL (L"Disharmony", constraint -> disharmony)
	EDITOR_DO
		OTMulti grammar = my data;
		OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
		Editor_save (OTMultiEditor_as_Editor (me), L"Edit ranking");
		constraint -> ranking = GET_REAL (L"Ranking value");
		constraint -> disharmony = GET_REAL (L"Disharmony");
		OTMulti_sort (grammar);
		Graphics_updateWs (my g);
		Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_learnOne (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Learn one", L"OTGrammar: Learn one...")
		OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
		OPTIONMENU (L"Direction", 3)
			OPTION (L"forward")
			OPTION (L"backward")
			OPTION (L"bidirectionally")
		REAL (L"Plasticity", L"0.1")
		REAL (L"Rel. plasticity spreading", L"0.1")
	EDITOR_OK
	EDITOR_DO
		Editor_save (OTMultiEditor_as_Editor (me), L"Learn one");
		Melder_free (my form1);
		Melder_free (my form2);
		my form1 = GuiText_getString (my form1Text);
		my form2 = GuiText_getString (my form2Text);
		OTMulti_learnOne (my data, my form1, my form2,
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"), GET_INTEGER (L"Direction"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"));
		iferror return 0;
		Graphics_updateWs (my g);
		Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_removeConstraint (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	OTMulti grammar = my data;
	OTConstraint constraint;
	if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
		return Melder_error1 (L"Select a constraint first.");
	constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (OTMultiEditor_as_Editor (me), L"Remove constraint");
	OTMulti_removeConstraint (grammar, constraint -> name);
	Graphics_updateWs (my g);
	Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	return 1;
}

static int menu_cb_resetAllRankings (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	EDITOR_FORM (L"Reset all rankings", 0)
		REAL (L"Ranking", L"100.0")
	EDITOR_OK
	EDITOR_DO
		Editor_save (OTMultiEditor_as_Editor (me), L"Reset all rankings");
		OTMulti_reset (my data, GET_REAL (L"Ranking"));
		Graphics_updateWs (my g);
		Editor_broadcastChange (OTMultiEditor_as_Editor (me));
	EDITOR_END
}

static int menu_cb_OTLearningTutorial (EDITOR_ARGS) {
	EDITOR_IAM (OTMultiEditor);
	(void) me;
	Melder_help (L"OT learning");
	return 1;
}

static void do_limit (OTMultiEditor me) {
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = GuiText_getString (my form1Text);
	my form2 = GuiText_getString (my form2Text);
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

static void createChildren (OTMultiEditor me) {
#if defined (macintosh)
	#define STRING_SPACING 8
#else
	#define STRING_SPACING 2
#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	inherited (OTMultiEditor) createChildren (OTMultiEditor_as_parent (me));
	GuiButton_createShown (my dialog, 4, 124, y, y + height,
		L"Partial forms:", gui_button_cb_limit, me,
		#ifdef _WIN32
			GuiButton_DEFAULT   // BUG: clickedCallback should work for texts
		#else
			0
		#endif
		);
	my form1Text = GuiText_createShown (my dialog, 124 + STRING_SPACING, 274 + STRING_SPACING, y, Gui_AUTOMATIC, 0);
	#if motif
	/* TODO */
	XtAddCallback (my form1Text, XmNactivateCallback, gui_cb_limit, (XtPointer) me);
	#endif
	my form2Text = GuiText_createShown (my dialog, 274 + 2 * STRING_SPACING, 424 + 2 * STRING_SPACING, y, Gui_AUTOMATIC, 0);
	#if motif
	/* TODO */
	XtAddCallback (my form2Text, XmNactivateCallback, gui_cb_limit, (XtPointer) me);
	#endif
}

static void createMenus (OTMultiEditor me) {
	inherited (OTMultiEditor) createMenus (OTMultiEditor_as_parent (me));
	Editor_addCommand (me, L"Edit", L"-- edit ot --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Evaluate...", 0, menu_cb_evaluate);
	Editor_addCommand (me, L"Edit", L"Evaluate (noise 2.0)", '2', menu_cb_evaluate_noise_2_0);
	Editor_addCommand (me, L"Edit", L"Evaluate (tiny noise)", '9', menu_cb_evaluate_tinyNoise);
	Editor_addCommand (me, L"Edit", L"Edit ranking...", 'E', menu_cb_editRanking);
	Editor_addCommand (me, L"Edit", L"Reset all rankings...", 'R', menu_cb_resetAllRankings);
	Editor_addCommand (me, L"Edit", L"Learn one...", '1', menu_cb_learnOne);
	Editor_addCommand (me, L"Edit", L"-- remove --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Remove constraint", 0, menu_cb_removeConstraint);
}

static void createHelpMenuItems (OTMultiEditor me, EditorMenu menu) {
	inherited (OTMultiEditor) createHelpMenuItems (OTMultiEditor_as_parent (me), menu);
	EditorMenu_addCommand (menu, L"OT learning tutorial", 0, menu_cb_OTLearningTutorial);
}

static OTMulti drawTableau_grammar;
static const wchar_t *drawTableau_form1, *drawTableau_form2;
static void drawTableau (Graphics g) {
	OTMulti_drawTableau (drawTableau_grammar, g, drawTableau_form1, drawTableau_form2, TRUE);
}

static void draw (OTMultiEditor me) {
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

static int goToPage (OTMultiEditor me, const wchar_t *title) {
	if (title == NULL) return 1;
	my selectedConstraint = wcstol (title, NULL, 10);
	return 1;
}

class_methods (OTMultiEditor, HyperPage) {
	class_method (createChildren)
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (draw)
	us -> editable = true;
	class_method (goToPage)
	class_methods_end
}

OTMultiEditor OTMultiEditor_create (GuiObject parent, const wchar_t *title, OTMulti grammar) {
	OTMultiEditor me = new (OTMultiEditor); cherror
	my data = grammar;
	my form1 = Melder_wcsdup_e (L""); cherror
	my form2 = Melder_wcsdup_e (L""); cherror
	HyperPage_init (OTMultiEditor_as_parent (me), parent, title, grammar); cherror
end:
	iferror forget (me);
	return me;
}

/* End of file OTGrammarEditor.c */
