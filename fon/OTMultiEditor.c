/* OTGrammarEditor.c
 *
 * Copyright (C) 2005 Paul Boersma
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
 * pb 2005/07/04
 */

#include "OTMultiEditor.h"
#include "EditorM.h"
#include "machine.h"

#define OTMultiEditor_members HyperPage_members \
	const char *form1, *form2; \
	Widget form1Text, form2Text; \
	long selectedConstraint;
#define OTMultiEditor_methods HyperPage_methods
class_create_opaque (OTMultiEditor, HyperPage)

FORM (OTMultiEditor, cb_evaluate, "Evaluate", 0)
	REAL ("Evaluation noise", "2.0")
	OK
DO
	Editor_save (me, "Evaluate");
	OTMulti_newDisharmonies (my data, GET_REAL ("Evaluation noise"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_evaluate_noise_2_0)
	Editor_save (me, "Evaluate (noise 2.0)");
	OTMulti_newDisharmonies (my data, 2.0);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_evaluate_tinyNoise)
	Editor_save (me, "Evaluate (tiny noise)");
	OTMulti_newDisharmonies (my data, 1e-9);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_editRanking, "Edit ranking", 0)
	LABEL ("constraint", "");
	REAL ("Ranking value", "100.0");
	REAL ("Disharmony", "100.0");
	OK
OTMulti grammar = my data;
OTConstraint constraint;
if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints) return Melder_error ("Select a constraint first.");
constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
SET_STRING ("constraint", constraint -> name)
SET_REAL ("Ranking value", constraint -> ranking)
SET_REAL ("Disharmony", constraint -> disharmony)
DO
	OTMulti grammar = my data;
	OTConstraint constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, "Edit ranking");
	constraint -> ranking = GET_REAL ("Ranking value");
	constraint -> disharmony = GET_REAL ("Disharmony");
	OTMulti_sort (grammar);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_learnOne, "Learn one", "OTGrammar: Learn one...")
	OPTIONMENU ("Learn", 3)
		OPTION ("forward")
		OPTION ("backward")
		OPTION ("bidirectionally")
	REAL ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	OK
DO
	char *form1 = XmTextFieldGetString (my form1Text), *form2 = XmTextFieldGetString (my form2Text);
	Editor_save (me, "Learn one");
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = Melder_strdup (form1);
	my form2 = Melder_strdup (form2);
	XtFree (form1);
	XtFree (form2);
	OTMulti_learnOne (my data, my form1, my form2,
		GET_INTEGER ("Learn"), GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"));
	iferror return 0;
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_removeConstraint)
	OTMulti grammar = my data;
	OTConstraint constraint;
	if (my selectedConstraint < 1 || my selectedConstraint > grammar -> numberOfConstraints)
		return Melder_error ("Select a constraint first.");
	constraint = & grammar -> constraints [grammar -> index [my selectedConstraint]];
	Editor_save (me, "Remove constraint");
	OTMulti_removeConstraint (grammar, constraint -> name);
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

FORM (OTMultiEditor, cb_resetAllRankings, "Reset all rankings", 0)
	REAL ("Ranking", "100.0")
	OK
DO
	Editor_save (me, "Reset all rankings");
	OTMulti_reset (my data, GET_REAL ("Ranking"));
	Graphics_updateWs (my g);
	Editor_broadcastChange (me);
END

DIRECT (OTMultiEditor, cb_OTLearningTutorial) Melder_help ("OT learning"); END

MOTIF_CALLBACK (cb_limit)
	iam (OTMultiEditor);
	char *form1 = XmTextFieldGetString (my form1Text), *form2 = XmTextFieldGetString (my form2Text);
	Melder_free (my form1);
	Melder_free (my form2);
	my form1 = Melder_strdup (form1);
	my form2 = Melder_strdup (form2);
	XtFree (form1);
	XtFree (form2);
	Graphics_updateWs (my g);
MOTIF_CALLBACK_END

static void createChildren (I) {
	iam (OTMultiEditor);
	Widget button;
#if defined (macintosh)
	#define STRING_SPACING 8
#else
	#define STRING_SPACING 2
#endif
	int height = Machine_getTextHeight (), y = Machine_getMenuBarHeight () + 4;
	inherited (OTMultiEditor) createChildren (me);
	button = XtVaCreateManagedWidget ("Partial forms:", xmPushButtonWidgetClass, my dialog,
		XmNx, 4, XmNy, y, XmNheight, height, XmNwidth, 120, 0);
	XtAddCallback (button, XmNactivateCallback, cb_limit, (XtPointer) me);
	#ifdef _WIN32
	/* BUG: activateCallback should work for texts. */
	XtVaSetValues (my dialog, XmNdefaultButton, button, NULL);
	#endif
	my form1Text = XtVaCreateManagedWidget ("form1Text", xmTextFieldWidgetClass, my dialog,
		XmNx, 124 + STRING_SPACING, XmNy, y, XmNwidth, 150, 0);
	XtAddCallback (my form1Text, XmNactivateCallback, cb_limit, (XtPointer) me);
	my form2Text = XtVaCreateManagedWidget ("form2Text", xmTextFieldWidgetClass, my dialog,
		XmNx, 274 + 2 * STRING_SPACING, XmNy, y, XmNwidth, 150, 0);
	XtAddCallback (my form2Text, XmNactivateCallback, cb_limit, (XtPointer) me);
}

static void createMenus (I) {
	iam (OTMultiEditor);
	inherited (OTMultiEditor) createMenus (me);
	Editor_addCommand (me, "Edit", "-- edit ot --", 0, NULL);
	Editor_addCommand (me, "Edit", "Evaluate...", 0, cb_evaluate);
	Editor_addCommand (me, "Edit", "Evaluate (noise 2.0)", '2', cb_evaluate_noise_2_0);
	Editor_addCommand (me, "Edit", "Evaluate (tiny noise)", '9', cb_evaluate_tinyNoise);
	Editor_addCommand (me, "Edit", "Edit ranking...", 'E', cb_editRanking);
	Editor_addCommand (me, "Edit", "Reset all rankings...", 'R', cb_resetAllRankings);
	Editor_addCommand (me, "Edit", "Learn one...", '1', cb_learnOne);
	Editor_addCommand (me, "Edit", "-- remove --", 0, NULL);
	Editor_addCommand (me, "Edit", "Remove constraint", 0, cb_removeConstraint);
	Editor_addCommand (me, "Help", "OT learning tutorial", 0, cb_OTLearningTutorial);
}

static OTMulti drawTableau_grammar;
static const char *drawTableau_form1, *drawTableau_form2;
static void drawTableau (Graphics g) {
	OTMulti_drawTableau (drawTableau_grammar, g, drawTableau_form1, drawTableau_form2);
}

static void draw (I) {
	iam (OTMultiEditor);
	OTMulti grammar = my data;
	static char text [1000];
	long icons, icand;
	double rowHeight = 0.25, tableauHeight = 2 * rowHeight;
	Graphics_clearWs (my g);
	HyperPage_listItem (me, "\t\t      %%ranking value\t      %disharmony");
	for (icons = 1; icons <= grammar -> numberOfConstraints; icons ++) {
		OTConstraint constraint = & grammar -> constraints [grammar -> index [icons]];
		sprintf (text, "\t%s@@%ld|%s@\t      %.3f\t      %.3f", icons == my selectedConstraint ? "\\sp " : "   ", icons,
			constraint -> name, constraint -> ranking, constraint -> disharmony);
		HyperPage_listItem (me, text);
	}
	Graphics_setAtSignIsLink (my g, FALSE);
	drawTableau_grammar = grammar;
	for (icand = 1; icand <= grammar -> numberOfCandidates; icand ++) {
		if (OTMulti_candidateMatches (grammar, icand, my form1, my form2)) {
			tableauHeight += rowHeight;
		}
	}
	drawTableau_form1 = my form1;
	drawTableau_form2 = my form2;
	HyperPage_picture (me, 20, tableauHeight, drawTableau);
	Graphics_setAtSignIsLink (my g, TRUE);
}

static int goToPage (I, const char *title) {
	iam (OTMultiEditor);
	if (! title) return 1;
	my selectedConstraint = atoi (title);
	return 1;
}

class_methods (OTMultiEditor, HyperPage)
	class_method (createChildren)
	class_method (createMenus)
	class_method (draw)
	us -> editable = TRUE;
	class_method (goToPage)
class_methods_end

OTMultiEditor OTMultiEditor_create (Widget parent, const char *title, OTMulti grammar) {
	OTMultiEditor me = new (OTMultiEditor);
	my data = grammar;
	my form1 = Melder_strdup ("");
	my form2 = Melder_strdup ("");
	if (! HyperPage_init (me, parent, title, grammar))
		{ forget (me); return Melder_errorp ("OTMultiEditor not created."); }
	return me;
}

/* End of file OTGrammarEditor.c */
