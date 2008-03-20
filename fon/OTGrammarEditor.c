/* OTGrammarEditor.c
 *
 * Copyright (C) 1997-2008 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/03/31 removeConstraint
 * pb 2003/05/27 learnOne and learnOneFromPartialOutput
 * pb 2004/03/16 Evaluate (tiny noise)
 * pb 2007/06/10 wchar_t
 * pb 2008/03/20 split off Help menu
 */

#include "OTGrammarEditor.h"
#include "EditorM.h"

#define OTGrammarEditor_members HyperPage_members \
	long selected;
#define OTGrammarEditor_methods HyperPage_methods
class_create_opaque (OTGrammarEditor, HyperPage);

FORM (OTGrammarEditor, cb_evaluate, L"Evaluate", 0)
	REAL (L"Noise", L"2.0")
	OK
DO
	Editor_save (me, L"Evaluate");
	OTGrammar_newDisharmonies (my data, GET_REAL (L"Noise"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTGrammarEditor, cb_evaluate_noise_2_0)
	Editor_save (me, L"Evaluate (noise 2.0)");
	OTGrammar_newDisharmonies (my data, 2.0);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTGrammarEditor, cb_evaluate_tinyNoise)
	Editor_save (me, L"Evaluate (tiny noise)");
	OTGrammar_newDisharmonies (my data, 1e-9);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTGrammarEditor, cb_evaluate_zeroNoise)
	Editor_save (me, L"Evaluate (zero noise)");
	OTGrammar_newDisharmonies (my data, 0.0);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTGrammarEditor, cb_editConstraint, L"Edit constraint", 0)
	LABEL (L"constraint", L"");
	REAL (L"Ranking value", L"100.0");
	REAL (L"Disharmony", L"100.0");
	REAL (L"Plasticity", L"1.0");
	OK
OTGrammar ot = my data;
OTGrammarConstraint constraint;
if (my selected < 1 || my selected > ot -> numberOfConstraints) return Melder_error1 (L"Select a constraint first.");
constraint = & ot -> constraints [ot -> index [my selected]];
SET_STRING (L"constraint", constraint -> name)
SET_REAL (L"Ranking value", constraint -> ranking)
SET_REAL (L"Disharmony", constraint -> disharmony)
SET_REAL (L"Plasticity", constraint -> plasticity)
DO
	OTGrammar ot = my data;
	OTGrammarConstraint constraint = & ot -> constraints [ot -> index [my selected]];
	Editor_save (me, L"Edit constraint");
	constraint -> ranking = GET_REAL (L"Ranking value");
	constraint -> disharmony = GET_REAL (L"Disharmony");
	constraint -> plasticity = GET_REAL (L"Plasticity");
	OTGrammar_sort (ot);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTGrammarEditor, cb_learnOne, L"Learn one", L"OTGrammar: Learn one...")
	LABEL (L"", L"Underlying form:")
	SENTENCE (L"Input string", L"")
	LABEL (L"", L"Adult surface form:")
	SENTENCE (L"Output string", L"")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	OK
DO
	Editor_save (me, L"Learn one");
	OTGrammar_learnOne (my data, GET_STRING (L"Input string"), GET_STRING (L"Output string"),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), TRUE, TRUE, NULL);
	OTGrammar_sort (my data);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTGrammarEditor, cb_learnOneFromPartialOutput, L"Learn one from partial adult output", 0)
	LABEL (L"", L"Partial adult surface form (e.g. overt form):")
	SENTENCE (L"Partial output", L"")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	Editor_save (me, L"Learn one from partial output");
	OTGrammar_learnOneFromPartialOutput (my data, GET_STRING (L"Partial output"),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"), TRUE);
	OTGrammar_sort (my data);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTGrammarEditor, cb_removeConstraint)
	OTGrammar ot = my data;
	OTGrammarConstraint constraint;
	if (my selected < 1 || my selected > ot -> numberOfConstraints) return Melder_error ("Select a constraint first.");
	constraint = & ot -> constraints [ot -> index [my selected]];
	Editor_save (me, L"Remove constraint");
	OTGrammar_removeConstraint (ot, constraint -> name);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTGrammarEditor, cb_resetAllRankings, L"Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	Editor_save (me, L"Reset all rankings");
	OTGrammar_reset (my data, GET_REAL (L"Ranking"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTGrammarEditor, cb_OTGrammarEditor_help) Melder_help (L"OTGrammarEditor"); END

DIRECT (OTGrammarEditor, cb_OTGrammar_help) Melder_help (L"OTGrammar"); END

DIRECT (OTGrammarEditor, cb_OTLearningTutorial) Melder_help (L"OT learning"); END

static void createMenus (I) {
	iam (OTGrammarEditor);
	inherited (OTGrammarEditor) createMenus (me);
	Editor_addCommand (me, L"Edit", L"-- edit ot --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Evaluate...", 0, cb_evaluate);
	Editor_addCommand (me, L"Edit", L"Evaluate (noise 2.0)", '2', cb_evaluate_noise_2_0);
	Editor_addCommand (me, L"Edit", L"Evaluate (zero noise)", '0', cb_evaluate_zeroNoise);
	Editor_addCommand (me, L"Edit", L"Evaluate (tiny noise)", '9', cb_evaluate_tinyNoise);
	Editor_addCommand (me, L"Edit", L"Edit constraint...", 'E', cb_editConstraint);
	Editor_addCommand (me, L"Edit", L"Reset all rankings...", 'R', cb_resetAllRankings);
	Editor_addCommand (me, L"Edit", L"Learn one...", 0, cb_learnOne);
	Editor_addCommand (me, L"Edit", L"Learn one from partial output...", '1', cb_learnOneFromPartialOutput);
	Editor_addCommand (me, L"Edit", L"-- remove ot --", 0, NULL);
	Editor_addCommand (me, L"Edit", L"Remove constraint", 0, cb_removeConstraint);
}

static void createHelpMenuItems (I, EditorMenu menu) {
	iam (OTGrammarEditor);
	inherited (OTGrammarEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"OTGrammarEditor help", '?', cb_OTGrammarEditor_help);
	EditorMenu_addCommand (menu, L"OTGrammar help", 0, cb_OTGrammar_help);
	EditorMenu_addCommand (menu, L"OT learning tutorial", 0, cb_OTLearningTutorial);
}

static OTGrammar drawTableau_ot;
static const wchar_t *drawTableau_input;
static void drawTableau (Graphics g) {
	OTGrammar_drawTableau (drawTableau_ot, g, drawTableau_input);
}

static void draw (I) {
	iam (OTGrammarEditor);
	OTGrammar ot = my data;
	static wchar_t text [1000];
	long icons, itab;
	Graphics_clearWs (my g);
	HyperPage_listItem (me, L"\t\t      %%ranking value\t      %disharmony\t      %plasticity");
	for (icons = 1; icons <= ot -> numberOfConstraints; icons ++) {
		OTGrammarConstraint constraint = & ot -> constraints [ot -> index [icons]];
		swprintf (text, 1000, L"\t%ls@@%ld|%ls@\t      %.3f\t      %.3f\t      %.6f", icons == my selected ? L"\\sp " : L"   ", icons,
			constraint -> name, constraint -> ranking, constraint -> disharmony, constraint -> plasticity);
		HyperPage_listItem (me, text);
	}
	Graphics_setAtSignIsLink (my g, FALSE);
	for (itab = 1; itab <= ot -> numberOfTableaus; itab ++) {
		OTGrammarTableau tableau = & ot -> tableaus [itab];
		double rowHeight = 0.25;
		double tableauHeight = rowHeight * (tableau -> numberOfCandidates + 2);
		drawTableau_ot = ot;
		drawTableau_input = tableau -> input;
		HyperPage_picture (me, 20, tableauHeight, drawTableau);
	}
	Graphics_setAtSignIsLink (my g, TRUE);
}

static int goToPage (I, const wchar_t *title) {
	iam (OTGrammarEditor);
	if (! title) return 1;
	my selected = wcstol (title, NULL, 10);
	return 1;
}

class_methods (OTGrammarEditor, HyperPage) {
	class_method (createMenus)
	class_method (createHelpMenuItems)
	class_method (draw)
	us -> editable = TRUE;
	class_method (goToPage)
	class_methods_end
}

OTGrammarEditor OTGrammarEditor_create (Widget parent, const wchar_t *title, OTGrammar ot) {
	OTGrammarEditor me = new (OTGrammarEditor);
	my data = ot;
	if (! HyperPage_init (me, parent, title, ot))
		{ forget (me); return Melder_errorp ("OTGrammarEditor not created."); }
	return me;
}

/* End of file OTGrammarEditor.c */
