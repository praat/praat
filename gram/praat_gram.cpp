/* praat_gram.cpp
 *
 * Copyright (C) 1997-2012,2013,2014,2015,2016 Paul Boersma
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

#include "praat.h"

#include "Network.h"
#include "OTGrammar.h"
#include "OTMulti.h"
#include "OTGrammarEditor.h"
#include "OTMultiEditor.h"
#include "RBM.h"

#undef iam
#define iam iam_LOOP

#pragma mark -
#pragma mark NETWORK

#pragma mark New

static void UiForm_addNetworkFields (UiForm dia) {
	UiField radio;
	LABEL (U"", U"Activity spreading settings:")
	REAL (U"Spreading rate", U"0.01")
	OPTIONMENU_ENUM (U"Activity clipping rule", kNetwork_activityClippingRule, DEFAULT)
	REAL (U"left Activity range", U"0.0")
	REAL (U"right Activity range", U"1.0")
	REAL (U"Activity leak", U"1.0")
	LABEL (U"", U"Weight update settings:")
	REAL (U"Learning rate", U"0.1")
	REAL (U"left Weight range", U"-1.0")
	REAL (U"right Weight range", U"1.0")
	REAL (U"Weight leak", U"0.0")
}

FORM (Create_empty_Network, U"Create empty Network", 0) {
	WORD (U"Name", U"network")
	UiForm_addNetworkFields (dia);
	LABEL (U"", U"World coordinates:")
	REAL (U"left x range", U"0.0")
	REAL (U"right x range", U"10.0")
	REAL (U"left y range", U"0.0")
	REAL (U"right y range", U"10.0")
	OK2
DO
	autoNetwork me = Network_create (GET_REAL (U"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, U"Activity clipping rule"),
		GET_REAL (U"left Activity range"), GET_REAL (U"right Activity range"), GET_REAL (U"Activity leak"),
		GET_REAL (U"Learning rate"), GET_REAL (U"left Weight range"), GET_REAL (U"right Weight range"), GET_REAL (U"Weight leak"),
		GET_REAL (U"left x range"), GET_REAL (U"right x range"), GET_REAL (U"left y range"), GET_REAL (U"right y range"),
		0, 0);
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

FORM (Create_rectangular_Network, U"Create rectangular Network", 0) {
	UiForm_addNetworkFields (dia);
	LABEL (U"", U"Structure settings:")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"10")
	BOOLEAN (U"Bottom row clamped", 1)
	LABEL (U"", U"Initial state settings:")
	REAL (U"left Initial weight range", U"-0.1")
	REAL (U"right Initial weight range", U"0.1")
	OK2
DO
	autoNetwork me = Network_create_rectangle (GET_REAL (U"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, U"Activity clipping rule"),
		GET_REAL (U"left Activity range"), GET_REAL (U"right Activity range"), GET_REAL (U"Activity leak"),
		GET_REAL (U"Learning rate"), GET_REAL (U"left Weight range"), GET_REAL (U"right Weight range"), GET_REAL (U"Weight leak"),
		GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"),
		GET_INTEGER (U"Bottom row clamped"),
		GET_REAL (U"left Initial weight range"), GET_REAL (U"right Initial weight range"));
	praat_new (me.move(),
			U"rectangle_", GET_INTEGER (U"Number of rows"),
			U"_", GET_INTEGER (U"Number of columns"));
END2 }

FORM (Create_rectangular_Network_vertical, U"Create rectangular Network (vertical)", 0) {
	UiForm_addNetworkFields (dia);
	LABEL (U"", U"Structure settings:")
	NATURAL (U"Number of rows", U"10")
	NATURAL (U"Number of columns", U"10")
	BOOLEAN (U"Bottom row clamped", 1)
	LABEL (U"", U"Initial state settings:")
	REAL (U"left Initial weight range", U"-0.1")
	REAL (U"right Initial weight range", U"0.1")
	OK2
DO
	autoNetwork me = Network_create_rectangle_vertical (GET_REAL (U"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, U"Activity clipping rule"),
		GET_REAL (U"left Activity range"), GET_REAL (U"right Activity range"), GET_REAL (U"Activity leak"),
		GET_REAL (U"Learning rate"), GET_REAL (U"left Weight range"), GET_REAL (U"right Weight range"), GET_REAL (U"Weight leak"),
		GET_INTEGER (U"Number of rows"), GET_INTEGER (U"Number of columns"),
		GET_INTEGER (U"Bottom row clamped"),
		GET_REAL (U"left Initial weight range"), GET_REAL (U"right Initial weight range"));
	praat_new (me.move(),
			U"rectangle_", GET_INTEGER (U"Number of rows"),
			U"_", GET_INTEGER (U"Number of columns"));
END2 }

#pragma mark Draw

FORM (Network_draw, U"Draw Network", 0) {
	BOOLEAN (U"Colour", 1)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam_LOOP (Network);
		Network_draw (me, GRAPHICS, GET_INTEGER (U"Colour"));
	}
END2 }

#pragma mark Tabulate

FORM (Network_listNodes, U"Network: List nodes", 0) {
	INTEGER (U"From node number", U"1")
	INTEGER (U"To node number", U"1000")
	BOOLEAN (U"Include node numbers", true)
	BOOLEAN (U"Include x", false)
	BOOLEAN (U"Include y", false)
	INTEGER (U"Position decimals", U"6")
	BOOLEAN (U"Include clamped", false)
	BOOLEAN (U"Include activity", true)
	BOOLEAN (U"Include excitation", false)
	INTEGER (U"Activity decimals", U"6")
	OK2
DO
	LOOP {
		iam (Network);
		Network_listNodes (me, GET_INTEGER (U"From node number"), GET_INTEGER (U"To node number"),
			GET_INTEGER (U"Include node numbers"),
			GET_INTEGER (U"Include x"), GET_INTEGER (U"Include y"), GET_INTEGER (U"Position decimals"),
			GET_INTEGER (U"Include clamped"),
			GET_INTEGER (U"Include activity"), GET_INTEGER (U"Include excitation"), GET_INTEGER (U"Activity decimals"));
	}
END2 }

FORM (Network_nodes_downto_Table, U"Network: Nodes down to Table", 0) {
	INTEGER (U"From node number", U"1")
	INTEGER (U"To node number", U"1000")
	BOOLEAN (U"Include node numbers", true)
	BOOLEAN (U"Include x", false)
	BOOLEAN (U"Include y", false)
	INTEGER (U"Position decimals", U"6")
	BOOLEAN (U"Include clamped", false)
	BOOLEAN (U"Include activity", true)
	BOOLEAN (U"Include excitation", false)
	INTEGER (U"Activity decimals", U"6")
	OK2
DO
	LOOP {
		iam (Network);
		autoTable thee = Network_nodes_downto_Table (me, GET_INTEGER (U"From node number"), GET_INTEGER (U"To node number"),
			GET_INTEGER (U"Include node numbers"),
			GET_INTEGER (U"Include x"), GET_INTEGER (U"Include y"), GET_INTEGER (U"Position decimals"),
			GET_INTEGER (U"Include clamped"),
			GET_INTEGER (U"Include activity"), GET_INTEGER (U"Include excitation"), GET_INTEGER (U"Activity decimals"));
		praat_new (thee.move(), my name);
	}
END2 }

#pragma mark Query

FORM (Network_getActivity, U"Network: Get activity", 0) {
	NATURAL (U"Node", U"1")
	OK2
DO
	iam_ONLY (Network);
	double activity = Network_getActivity (me, GET_INTEGER (U"Node"));
	Melder_information (activity);
END2 }

FORM (Network_getWeight, U"Network: Get weight", 0) {
	NATURAL (U"Connection", U"1")
	OK2
DO
	iam_ONLY (Network);
	double weight = Network_getWeight (me, GET_INTEGER (U"Connection"));
	Melder_information (weight);
END2 }

#pragma mark Modify

FORM (Network_addConnection, U"Network: Add connection", 0) {
	NATURAL (U"From node", U"1")
	NATURAL (U"To node", U"2")
	REAL (U"Weight", U"0.0")
	REAL (U"Plasticity", U"1.0")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_addConnection (me, GET_INTEGER (U"From node"), GET_INTEGER (U"To node"), GET_REAL (U"Weight"), GET_REAL (U"Plasticity"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_addNode, U"Network: Add node", 0) {
	REAL (U"x", U"5.0")
	REAL (U"y", U"5.0")
	REAL (U"Activity", U"0.0")
	BOOLEAN (U"Clamping", 0)
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_addNode (me, GET_REAL (U"x"), GET_REAL (U"y"), GET_REAL (U"Activity"), GET_INTEGER (U"Clamping"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_normalizeActivities, U"Network: Normalize activities", 0) {
	INTEGER (U"From node", U"1")
	INTEGER (U"To node", U"0 (= all)")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_normalizeActivities (me, GET_INTEGER (U"From node"), GET_INTEGER (U"To node"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_normalizeWeights, U"Network: Normalize weights", 0) {
	INTEGER (U"From node", U"1")
	INTEGER (U"To node", U"0 (= all)")
	INTEGER (U"From incoming node", U"1")
	INTEGER (U"To incoming node", U"10")
	REAL (U"New sum", U"1.0")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_normalizeWeights (me, GET_INTEGER (U"From node"), GET_INTEGER (U"To node"),
			GET_INTEGER (U"From incoming node"), GET_INTEGER (U"To incoming node"), GET_REAL (U"New sum"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_setActivity, U"Network: Set activity", 0) {
	NATURAL (U"Node", U"1")
	REAL (U"Activity", U"1.0")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_setActivity (me, GET_INTEGER (U"Node"), GET_REAL (U"Activity"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_setActivityClippingRule, U"Network: Set activity clipping rule", 0) {
	RADIO_ENUM (U"Activity clipping rule", kNetwork_activityClippingRule, DEFAULT)
	OK2
iam_ONLY (Network);
SET_ENUM (U"Activity clipping rule", kNetwork_activityClippingRule, my activityClippingRule);
DO
	iam_ONLY (Network);
	Network_setActivityClippingRule (me, GET_ENUM (kNetwork_activityClippingRule, U"Activity clipping rule"));
	praat_dataChanged (me);
END2 }

FORM (Network_setActivityLeak, U"Network: Set activity leak", 0) {
	REAL (U"Activity leak", U"1.0")
	OK2
iam_ONLY (Network);
SET_REAL (U"Activity leak", my activityLeak);
DO
	iam_ONLY (Network);
	Network_setActivityLeak (me, GET_REAL (U"Activity leak"));
	praat_dataChanged (me);
END2 }

FORM (Network_setClamping, U"Network: Set clamping", 0) {
	NATURAL (U"Node", U"1")
	BOOLEAN (U"Clamping", 1)
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_setClamping (me, GET_INTEGER (U"Node"), GET_INTEGER (U"Clamping"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_setInstar, U"Network: Set instar", 0) {
	REAL (U"Instar", U"0.0")
	OK2
iam_ONLY (Network);
SET_REAL (U"Instar", my instar);
DO
	iam_ONLY (Network);
	Network_setInstar (me, GET_REAL (U"Instar"));
	praat_dataChanged (me);
END2 }

FORM (Network_setWeightLeak, U"Network: Set weight leak", 0) {
	REAL (U"Weight leak", U"0.0")
	OK2
iam_ONLY (Network);
SET_REAL (U"Weight leak", my weightLeak);
DO
	iam_ONLY (Network);
	Network_setWeightLeak (me, GET_REAL (U"Weight leak"));
	praat_dataChanged (me);
END2 }

FORM (Network_setOutstar, U"Network: Set outstar", 0) {
	REAL (U"Outstar", U"0.0")
	OK2
iam_ONLY (Network);
SET_REAL (U"Outstar", my outstar);
DO
	iam_ONLY (Network);
	Network_setOutstar (me, GET_REAL (U"Outstar"));
	praat_dataChanged (me);
END2 }

FORM (Network_setShunting, U"Network: Set shunting", 0) {
	REAL (U"Shunting", U"1.0")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_setShunting (me, GET_REAL (U"Shunting"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_setWeight, U"Network: Set weight", 0) {
	NATURAL (U"Connection", U"1")
	REAL (U"Weight", U"1.0")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_setWeight (me, GET_INTEGER (U"Connection"), GET_REAL (U"Weight"));
		praat_dataChanged (me);
	}
END2 }

FORM (Network_spreadActivities, U"Network: Spread activities", 0) {
	NATURAL (U"Number of steps", U"20")
	OK2
DO
	LOOP {
		iam_LOOP (Network);
		Network_spreadActivities (me, GET_INTEGER (U"Number of steps"));
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (Network_updateWeights) {
	LOOP {
		iam_LOOP (Network);
		Network_updateWeights (me);
		praat_dataChanged (me);
	}
END2 }

FORM (Network_zeroActivities, U"Network: Zero activities", 0) {
	INTEGER (U"From node", U"1")
	INTEGER (U"To node", U"0 (= all)")
	OK2
DO
	LOOP {
		iam (Network);
		Network_zeroActivities (me, GET_INTEGER (U"From node"), GET_INTEGER (U"To node"));
		praat_dataChanged (me);
	}
END2 }

#pragma mark -
#pragma mark OTGRAMMAR

#pragma mark New

DIRECT2 (OT_learning_tutorial) {
	Melder_help (U"OT learning");
END2 }

DIRECT2 (Create_NoCoda_grammar) {
	autoOTGrammar me = OTGrammar_create_NoCoda_grammar ();
	praat_new (me.move(), U"NoCoda");
END2 }

DIRECT2 (Create_NPA_grammar) {
	autoOTGrammar me = OTGrammar_create_NPA_grammar ();
	praat_new (me.move(), U"assimilation");
END2 }

DIRECT2 (Create_NPA_distribution) {
	autoPairDistribution me = OTGrammar_create_NPA_distribution ();
	praat_new (me.move(), U"assimilation");
END2 }

FORM (Create_tongue_root_grammar, U"Create tongue-root grammar", U"Create tongue-root grammar...") {
	RADIO (U"Constraint set", 1)
		RADIOBUTTON (U"Five")
		RADIOBUTTON (U"Nine")
	RADIO (U"Ranking", 3)
		RADIOBUTTON (U"Equal")
		RADIOBUTTON (U"Random")
		RADIOBUTTON (U"Infant")
		RADIOBUTTON (U"Wolof")
	OK2
DO
	autoOTGrammar me = OTGrammar_create_tongueRoot_grammar (GET_INTEGER (U"Constraint set"), GET_INTEGER (U"Ranking"));
	praat_new (me.move(), GET_STRING (U"Ranking"));
END2 }

FORM (Create_metrics_grammar, U"Create metrics grammar", 0) {
	OPTIONMENU (U"Initial ranking", 1)
		OPTION (U"Equal")
		OPTION (U"Foot form high")
		OPTION (U"WSP high")
	OPTIONMENU (U"Trochaicity constraint", 1)
		OPTION (U"FtNonfinal")
		OPTION (U"Trochaic")
	BOOLEAN (U"Include FootBimoraic", 0)
	BOOLEAN (U"Include FootBisyllabic", 0)
	BOOLEAN (U"Include Peripheral", 0)
	OPTIONMENU (U"Nonfinality constraint", 1)
		OPTION (U"Nonfinal")
		OPTION (U"MainNonfinal")
		OPTION (U"HeadNonfinal")
	BOOLEAN (U"Overt forms have secondary stress", 1)
	BOOLEAN (U"Include *Clash and *Lapse", 0)
	BOOLEAN (U"Include codas", 0)
	OK2
DO
	autoOTGrammar me = OTGrammar_create_metrics (GET_INTEGER (U"Initial ranking"), GET_INTEGER (U"Trochaicity constraint"),
		GET_INTEGER (U"Include FootBimoraic"), GET_INTEGER (U"Include FootBisyllabic"),
		GET_INTEGER (U"Include Peripheral"), GET_INTEGER (U"Nonfinality constraint"),
		GET_INTEGER (U"Overt forms have secondary stress"), GET_INTEGER (U"Include *Clash and *Lapse"), GET_INTEGER (U"Include codas"));
	praat_new (me.move(), GET_STRING (U"Initial ranking"));
END2 }

#pragma mark Save
FORM_WRITE2 (OTGrammar_writeToHeaderlessSpreadsheetFile, U"Write OTGrammar to spreadsheet", 0, U"txt") {
	iam_ONLY (OTGrammar);
	OTGrammar_writeToHeaderlessSpreadsheetFile (me, file);
END2 }

#pragma mark Help

DIRECT2 (OTGrammar_help) {
	Melder_help (U"OTGrammar");
END2 }

#pragma mark Edit

DIRECT2 (OTGrammar_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit from batch.");
	LOOP {
		iam (OTGrammar);
		autoOTGrammarEditor editor = OTGrammarEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

#pragma mark Draw

FORM (OTGrammar_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (U"Input string", U"")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTGrammar);
		OTGrammar_drawTableau (me, GRAPHICS, false, GET_STRING (U"Input string"));
	}
END2 }

FORM (OTGrammar_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
	SENTENCE (U"Input string", U"")
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTGrammar);
		OTGrammar_drawTableau (me, GRAPHICS, true, GET_STRING (U"Input string"));
	}
END2 }

#pragma mark Query

DIRECT2 (OTGrammar_getNumberOfConstraints) {
	iam_ONLY (OTGrammar);
	Melder_information (my numberOfConstraints);
END2 }

FORM (OTGrammar_getConstraint, U"Get constraint name", 0) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. name);
END2 }

FORM (OTGrammar_getRankingValue, U"Get ranking value", 0) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. ranking);
END2 }

FORM (OTGrammar_getDisharmony, U"Get disharmony", 0) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. disharmony);
END2 }

DIRECT2 (OTGrammar_getNumberOfTableaus) {
	iam_ONLY (OTGrammar);
	Melder_information (my numberOfTableaus);
END2 }

FORM (OTGrammar_getInput, U"Get input", 0) {
	NATURAL (U"Tableau number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	Melder_information (my tableaus [itab]. input);
END2 }

FORM (OTGrammar_getNumberOfCandidates, U"Get number of candidates", 0) {
	NATURAL (U"Tableau number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	Melder_information (my tableaus [itab]. numberOfCandidates);
END2 }

FORM (OTGrammar_getCandidate, U"Get candidate", 0) {
	NATURAL (U"Tableau number", U"1")
	NATURAL (U"Candidate number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	OTGrammarTableau tableau;
	long itab = GET_INTEGER (U"Tableau number"), icand = GET_INTEGER (U"Candidate number");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	tableau = & my tableaus [itab];
	if (icand > tableau -> numberOfCandidates)
		Melder_throw (U"The specified candidate should not exceed the number of candidates.");
	Melder_information (tableau -> candidates [icand]. output);
END2 }

FORM (OTGrammar_getNumberOfViolations, U"Get number of violations", 0) {
	NATURAL (U"Tableau number", U"1")
	NATURAL (U"Candidate number", U"1")
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau number"), icand = GET_INTEGER (U"Candidate number"), icons = GET_INTEGER (U"Constraint number");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw (U"The specified candidate should not exceed the number of candidates.");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my tableaus [itab]. candidates [icand]. marks [icons]);
END2 }

#pragma mark Query (parse)

FORM (OTGrammar_getWinner, U"Get winner", 0) {
	NATURAL (U"Tableau", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	Melder_information (OTGrammar_getWinner (me, itab));
END2 }

FORM (OTGrammar_compareCandidates, U"Compare candidates", 0) {
	NATURAL (U"Tableau number 1", U"1")
	NATURAL (U"Candidate number 1", U"1")
	NATURAL (U"Tableau number 2", U"1")
	NATURAL (U"Candidate number 2", U"2")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab1 = GET_INTEGER (U"Tableau number 1"), icand1 = GET_INTEGER (U"Candidate number 1");
	long itab2 = GET_INTEGER (U"Tableau number 2"), icand2 = GET_INTEGER (U"Candidate number 2");
	if (itab1 > my numberOfTableaus)
		Melder_throw (U"The specified tableau (number 1) should not exceed the number of tableaus.");
	if (itab2 > my numberOfTableaus)
		Melder_throw (U"The specified tableau (number 2) should not exceed the number of tableaus.");
	if (icand1 > my tableaus [itab1]. numberOfCandidates)
		Melder_throw (U"The specified candidate (number 1) should not exceed the number of candidates for this tableau.");
	if (icand2 > my tableaus [itab1]. numberOfCandidates)
		Melder_throw (U"The specified candidate (number 2) should not exceed the number of candidates for this tableau.");
	Melder_information (OTGrammar_compareCandidates (me, itab1, icand1, itab2, icand2));
END2 }

FORM (OTGrammar_getNumberOfOptimalCandidates, U"Get number of optimal candidates", 0) {
	NATURAL (U"Tableau number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	Melder_information (OTGrammar_getNumberOfOptimalCandidates (me, itab));
END2 }

FORM (OTGrammar_isCandidateGrammatical, U"Is candidate grammatical?", 0) {
	NATURAL (U"Tableau", U"1")
	NATURAL (U"Candidate", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	long icand = GET_INTEGER (U"Candidate");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw (U"The specified candidate should not exceed the number of candidates.");
	Melder_information ((int) OTGrammar_isCandidateGrammatical (me, itab, icand));   // 0 or 1
END2 }

FORM (OTGrammar_isCandidateSinglyGrammatical, U"Is candidate singly grammatical?", 0) {
	NATURAL (U"Tableau", U"1")
	NATURAL (U"Candidate", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (U"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw (U"The specified tableau number should not exceed the number of tableaus.");
	long icand = GET_INTEGER (U"Candidate");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw (U"The specified candidate should not exceed the number of candidates.");
	Melder_information ((int) OTGrammar_isCandidateSinglyGrammatical (me, itab, icand));   // 0 or 1
END2 }

FORM (OTGrammar_getInterpretiveParse, U"OTGrammar: Interpretive parse", 0) {
	SENTENCE (U"Partial output", U"")
	OK2
DO
	iam_ONLY (OTGrammar);
	long bestInput, bestOutput;
	OTGrammar_getInterpretiveParse (me, GET_STRING (U"Partial output"), & bestInput, & bestOutput);
	Melder_information (U"Best input = ", bestInput, U": ", my tableaus [bestInput]. input,
		U"\nBest output = ", bestOutput, U": ", my tableaus [bestInput]. candidates [bestOutput]. output);
END2 }

FORM (OTGrammar_isPartialOutputGrammatical, U"Is partial output grammatical?", 0) {
	SENTENCE (U"Partial output", U"")
	OK2
DO
	iam_ONLY (OTGrammar);
	Melder_information ((int) OTGrammar_isPartialOutputGrammatical (me, GET_STRING (U"Partial output")));   // "0" or "1"
END2 }

FORM (OTGrammar_isPartialOutputSinglyGrammatical, U"Is partial output singly grammatical?", 0) {
	SENTENCE (U"Partial output", U"")
	OK2
DO
	iam_ONLY (OTGrammar);
	Melder_information ((int) OTGrammar_isPartialOutputSinglyGrammatical (me, GET_STRING (U"Partial output")));   // "0" or "1"
END2 }

#pragma mark -

FORM (OTGrammar_generateInputs, U"Generate inputs", U"OTGrammar: Generate inputs...") {
	NATURAL (U"Number of trials", U"1000")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		autoStrings thee = OTGrammar_generateInputs (me, GET_INTEGER (U"Number of trials"));
		praat_new (thee.move(), my name, U"_in");
	}
END2 }

DIRECT2 (OTGrammar_getInputs) {
	LOOP {
		iam (OTGrammar);
		autoStrings thee = OTGrammar_getInputs (me);
		praat_new (thee.move(), my name, U"_in");
	}
END2 }

DIRECT2 (OTGrammar_measureTypology) {
	LOOP try {
		iam (OTGrammar);
		autoDistributions thee = OTGrammar_measureTypology (me);
		praat_new (thee.move(), my name, U"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END2 }

#pragma mark Evaluate

FORM (OTGrammar_evaluate, U"OTGrammar: Evaluate", 0) {
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_newDisharmonies (me, GET_REAL (U"Evaluation noise"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_inputToOutput, U"OTGrammar: Input to output", U"OTGrammar: Input to output...") {
	SENTENCE (U"Input form", U"")
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTGrammar);
	char32 output [100];
	OTGrammar_inputToOutput (me, GET_STRING (U"Input form"), output, GET_REAL (U"Evaluation noise"));
	Melder_information (output);
	praat_dataChanged (me);
END2 }

FORM (OTGrammar_inputToOutputs, U"OTGrammar: Input to outputs", U"OTGrammar: Input to outputs...") {
	NATURAL (U"Trials", U"1000")
	REAL (U"Evaluation noise", U"2.0")
	SENTENCE (U"Input form", U"")
	OK2
DO
	iam_ONLY (OTGrammar);
	autoStrings thee = OTGrammar_inputToOutputs (me, GET_STRING (U"Input form"), GET_INTEGER (U"Trials"), GET_REAL (U"Evaluation noise"));
	praat_new (thee.move(), my name, U"_out");
	praat_dataChanged (me);
END2 }

FORM (OTGrammar_to_Distributions, U"OTGrammar: Compute output distributions", U"OTGrammar: To output Distributions...") {
	NATURAL (U"Trials per input", U"100000")
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		try {
			autoDistributions thee = OTGrammar_to_Distribution (me, GET_INTEGER (U"Trials per input"), GET_REAL (U"Evaluation noise"));
			praat_new (thee.move(), my name, U"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END2 }

FORM (OTGrammar_to_PairDistribution, U"OTGrammar: Compute output distributions", 0) {
	NATURAL (U"Trials per input", U"100000")
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	LOOP try {
		iam (OTGrammar);
		autoPairDistribution thee = OTGrammar_to_PairDistribution (me, GET_INTEGER (U"Trials per input"), GET_REAL (U"Evaluation noise"));
		praat_new (thee.move(), my name, U"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END2 }

#pragma mark Modify ranking

FORM (OTGrammar_setRanking, U"OTGrammar: Set ranking", 0) {
	NATURAL (U"Constraint", U"1")
	REAL (U"Ranking", U"100.0")
	REAL (U"Disharmony", U"100.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_setRanking (me, GET_INTEGER (U"Constraint"), GET_REAL (U"Ranking"), GET_REAL (U"Disharmony"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_resetAllRankings, U"OTGrammar: Reset all rankings", 0) {
	REAL (U"Ranking", U"100.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_reset (me, GET_REAL (U"Ranking"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_resetToRandomRanking, U"OTGrammar: Reset to random ranking", 0) {
	REAL (U"Mean", U"10.0")
	POSITIVE (U"Standard deviation", U"0.0001")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_resetToRandomRanking (me, GET_REAL (U"Mean"), GET_REAL (U"Standard deviation"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_resetToRandomTotalRanking, U"OTGrammar: Reset to random total ranking", 0) {
	REAL (U"Maximum ranking", U"100.0")
	POSITIVE (U"Ranking distance", U"1.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_resetToRandomTotalRanking (me, GET_REAL (U"Maximum ranking"), GET_REAL (U"Ranking distance"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_learnOne, U"OTGrammar: Learn one", U"OTGrammar: Learn one...") {
	SENTENCE (U"Input string", U"")
	SENTENCE (U"Output string", U"")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Plasticity", U"0.1")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	OK2
DO
	LOOP try {
		iam (OTGrammar);
		OTGrammar_learnOne (me, GET_STRING (U"Input string"), GET_STRING (U"Output string"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Plasticity"), GET_REAL (U"Rel. plasticity spreading"), true, true, nullptr);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END2 }

FORM (OTGrammar_learnOneFromPartialOutput, U"OTGrammar: Learn one from partial adult output", 0) {
	LABEL (U"", U"Partial adult surface form (e.g. overt form):")
	SENTENCE (U"Partial output", U"")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Plasticity", U"0.1")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	OK2
DO
	LOOP try {
		iam (OTGrammar);
		OTGrammar_learnOneFromPartialOutput (me, GET_STRING (U"Partial output"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Plasticity"), GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"), true);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END2 }

#pragma mark Modify behaviour

FORM (OTGrammar_setDecisionStrategy, U"OTGrammar: Set decision strategy", 0) {
	RADIO_ENUM (U"Decision strategy", kOTGrammar_decisionStrategy, DEFAULT)
	OK2
iam_ONLY (OTGrammar);
SET_ENUM (U"Decision strategy", kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	iam_ONLY (OTGrammar);
	my decisionStrategy = GET_ENUM (kOTGrammar_decisionStrategy, U"Decision strategy");
	praat_dataChanged (me);
END2 }

FORM (OTGrammar_setLeak, U"OTGrammar: Set leak", 0) {
	REAL (U"Leak", U"0.0")
	OK2
iam_ONLY (OTGrammar);
SET_REAL (U"Leak", my leak);
DO
	iam_ONLY (OTGrammar);
	my leak = GET_REAL (U"Leak");
	praat_dataChanged (me);
END2 }

FORM (OTGrammar_setConstraintPlasticity, U"OTGrammar: Set constraint plasticity", 0) {
	NATURAL (U"Constraint", U"1")
	REAL (U"Plasticity", U"1.0")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_setConstraintPlasticity (me, GET_INTEGER (U"Constraint"), GET_REAL (U"Plasticity"));
		praat_dataChanged (OBJECT);
	}
END2 }

#pragma mark Modify structure

FORM (OTGrammar_removeConstraint, U"OTGrammar: Remove constraint", 0) {
	SENTENCE (U"Constraint name", U"")
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_removeConstraint (me, GET_STRING (U"Constraint name"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTGrammar_removeHarmonicallyBoundedCandidates, U"OTGrammar: Remove harmonically bounded candidates", 0) {
	BOOLEAN (U"Singly", 0)
	OK2
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_removeHarmonicallyBoundedCandidates (me, GET_INTEGER (U"Singly"));
		praat_dataChanged (me);
	}
END2 }

#pragma mark OTGRAMMAR & STRINGS

FORM (OTGrammar_Strings_inputsToOutputs, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	autoStrings him = OTGrammar_inputsToOutputs (me, thee, GET_REAL (U"Evaluation noise"));
	praat_new (him.move(), my name, U"_out");
	praat_dataChanged (me);
END2 }

DIRECT2 (OTGrammar_Strings_areAllPartialOutputsGrammatical) {
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	Melder_information ((int) OTGrammar_areAllPartialOutputsGrammatical (me, thee));   // "0" or "1"
END2 }

DIRECT2 (OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical) {
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	Melder_information ((int) OTGrammar_areAllPartialOutputsSinglyGrammatical (me, thee));   // "0" or "1"
END2 }

FORM (OTGrammar_Stringses_learn, U"OTGrammar: Learn", U"OTGrammar & 2 Strings: Learn...") {
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Plasticity", U"0.1")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	Strings inputs = nullptr, outputs = nullptr;
	WHERE (SELECTED && CLASS == classStrings) { if (! inputs) inputs = (Strings) OBJECT; else outputs = (Strings) OBJECT; }
	try {
		OTGrammar_learn (me, inputs, outputs,
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Plasticity"), GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // partial change
		throw;
	}
END2 }

FORM (OTGrammar_Strings_learnFromPartialOutputs, U"OTGrammar: Learn from partial adult outputs", 0) {
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Plasticity", U"0.1")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	autoOTHistory history;
	try {
		OTGrammar_learnFromPartialOutputs (me, thee,
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Plasticity"), GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"),
			GET_INTEGER (U"Store history every"), & history);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // e.g. in case of partial learning
		Melder_flushError ();
		// trickle down to save history
	}
	if (history) praat_new (history.move(), my name);
END2 }

#pragma mark OTGRAMMAR & DISTRIBUTIONS

FORM (OTGrammar_Distributions_getFractionCorrect, U"OTGrammar & Distributions: Get fraction correct...", 0) {
	NATURAL (U"Column number", U"1")
	REAL (U"Evaluation noise", U"2.0")
	INTEGER (U"Replications", U"100000")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	double result = OTGrammar_Distributions_getFractionCorrect (me, thee, GET_INTEGER (U"Column number"),
		GET_REAL (U"Evaluation noise"), GET_INTEGER (U"Replications"));
	praat_dataChanged (me);
	Melder_informationReal (result, nullptr);
END2 }

FORM (OTGrammar_Distributions_learnFromPartialOutputs, U"OTGrammar & Distributions: Learn from partial outputs", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (U"Column number", U"1")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	autoOTHistory history;
	try {
		OTGrammar_Distributions_learnFromPartialOutputs (me, thee, GET_INTEGER (U"Column number"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"),
			GET_INTEGER (U"Store history every"), & history, false, false, 0);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		Melder_flushError ();
	}
	if (history) praat_new (history.move(), my name);
END2 }

FORM (OTGrammar_Distributions_learnFromPartialOutputs_rrip, U"OTGrammar & Distributions: Learn from partial outputs (rrip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (U"Column number", U"1")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	autoOTHistory history;
	try {
		OTGrammar_Distributions_learnFromPartialOutputs (me, thee, GET_INTEGER (U"Column number"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"),
			GET_INTEGER (U"Store history every"), & history, true, true, 0);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		Melder_flushError ();
	}
	if (history) praat_new (history.move(), my name);
END2 }

FORM (OTGrammar_Distributions_learnFromPartialOutputs_eip, U"OTGrammar & Distributions: Learn from partial outputs (eip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (U"Column number", U"1")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	autoOTHistory history;
	try {
		OTGrammar_Distributions_learnFromPartialOutputs (me, thee, GET_INTEGER (U"Column number"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"),
			GET_INTEGER (U"Store history every"), & history, true, true, 1000);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		Melder_flushError ();
	}
	if (history) praat_new (history.move(), my name);
END2 }

FORM (OTGrammar_Distributions_learnFromPartialOutputs_wrip, U"OTGrammar & Distributions: Learn from partial outputs (wrip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (U"Column number", U"1")
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", 1)
	NATURAL (U"Number of chews", U"1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	autoOTHistory history;
	try {
		OTGrammar_Distributions_learnFromPartialOutputs (me, thee, GET_INTEGER (U"Column number"),
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"),
			GET_INTEGER (U"Store history every"), & history, true, true, 1);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		Melder_flushError ();
	}
	if (history) praat_new (history.move(), my name);
END2 }

FORM (OTGrammar_Distributions_listObligatoryRankings, U"OTGrammar & Distributions: Get fraction correct...", 0) {
	NATURAL (U"Column number", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	OTGrammar_Distributions_listObligatoryRankings (me, thee, GET_INTEGER (U"Column number"));
END2 }

#pragma mark OTGRAMMAR & PAIRDISTRIBUTION

FORM (OTGrammar_PairDistribution_findPositiveWeights, U"OTGrammar & PairDistribution: Find positive weights", U"OTGrammar & PairDistribution: Find positive weights...") {
	POSITIVE (U"Weight floor", U"1.0")
	POSITIVE (U"Margin of separation", U"1.0")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	OTGrammar_PairDistribution_findPositiveWeights_e (me, thee,
		GET_REAL (U"Weight floor"), GET_REAL (U"Margin of separation"));
	praat_dataChanged (me);
END2 }

FORM (OTGrammar_PairDistribution_getFractionCorrect, U"OTGrammar & PairDistribution: Get fraction correct...", 0) {
	REAL (U"Evaluation noise", U"2.0")
	INTEGER (U"Replications", U"100000")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	double result;
	try {
		result = OTGrammar_PairDistribution_getFractionCorrect (me, thee,
			GET_REAL (U"Evaluation noise"), GET_INTEGER (U"Replications"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
	Melder_informationReal (result, nullptr);
END2 }

FORM (OTGrammar_PairDistribution_getMinimumNumberCorrect, U"OTGrammar & PairDistribution: Get minimum number correct...", 0) {
	REAL (U"Evaluation noise", U"2.0")
	INTEGER (U"Replications per input", U"1000")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	long result;
	try {
		result = OTGrammar_PairDistribution_getMinimumNumberCorrect (me, thee,
			GET_REAL (U"Evaluation noise"), GET_INTEGER (U"Replications per input"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
	Melder_information (result);
END2 }

FORM (OTGrammar_PairDistribution_learn, U"OTGrammar & PairDistribution: Learn", U"OT learning 6. Shortcut to grammar learning") {
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	POSITIVE (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (U"Honour local rankings", true)
	NATURAL (U"Number of chews", U"1")
	OK2
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	try {
		OTGrammar_PairDistribution_learn (me, thee,
			GET_REAL (U"Evaluation noise"), GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"), GET_INTEGER (U"Honour local rankings"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"), GET_INTEGER (U"Number of chews"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
END2 }

DIRECT2 (OTGrammar_PairDistribution_listObligatoryRankings) {
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	OTGrammar_PairDistribution_listObligatoryRankings (me, thee);
END2 }

#pragma mark -
#pragma mark OTMULTI

#pragma mark New

FORM (Create_multi_level_metrics_grammar, U"Create multi-level metrics grammar", nullptr) {
	OPTIONMENU (U"Initial ranking", 1)
		OPTION (U"Equal")
		OPTION (U"Foot form high")
		OPTION (U"WSP high")
	OPTIONMENU (U"Trochaicity constraint", 1)
		OPTION (U"FtNonfinal")
		OPTION (U"Trochaic")
	BOOLEAN (U"Include FootBimoraic", false)
	BOOLEAN (U"Include FootBisyllabic", false)
	BOOLEAN (U"Include Peripheral", false)
	OPTIONMENU (U"Nonfinality constraint", 1)
		OPTION (U"Nonfinal")
		OPTION (U"MainNonfinal")
		OPTION (U"HeadNonfinal")
	BOOLEAN (U"Overt forms have secondary stress", true)
	BOOLEAN (U"Include *Clash and *Lapse", false)
	BOOLEAN (U"Include codas", false)
	OK2
DO
	autoOTMulti me = OTMulti_create_metrics (GET_INTEGER (U"Initial ranking"), GET_INTEGER (U"Trochaicity constraint"),
		GET_INTEGER (U"Include FootBimoraic"), GET_INTEGER (U"Include FootBisyllabic"),
		GET_INTEGER (U"Include Peripheral"), GET_INTEGER (U"Nonfinality constraint"),
		GET_INTEGER (U"Overt forms have secondary stress"), GET_INTEGER (U"Include *Clash and *Lapse"), GET_INTEGER (U"Include codas"));
	praat_new (me.move(), GET_STRING (U"Initial ranking"));
END2 }

#pragma mark Draw

FORM (OTMulti_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	BOOLEAN (U"Show disharmonies", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTMulti);
		OTMulti_drawTableau (me, GRAPHICS, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
			false, GET_INTEGER (U"Show disharmonies"));
	}
END2 }

FORM (OTMulti_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	BOOLEAN (U"Show disharmonies", true)
	OK2
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTMulti);
		OTMulti_drawTableau (me, GRAPHICS, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
			true, GET_INTEGER (U"Show disharmonies"));
	}
END2 }

#pragma mark Edit

DIRECT2 (OTMulti_edit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit an OTMulti from batch.");
	LOOP {
		iam (OTMulti);
		autoOTMultiEditor editor = OTMultiEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	}
END2 }

#pragma mark Query

DIRECT2 (OTMulti_getNumberOfConstraints) {
	iam_ONLY (OTMulti);
	Melder_information (my numberOfConstraints);
END2 }

FORM (OTMulti_getConstraint, U"Get constraint name", nullptr) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. name);
END2 }

FORM (OTMulti_getConstraintIndexFromName, U"OTMulti: Get constraint number", nullptr) {
	SENTENCE (U"Constraint name", U"")
	OK2
DO
	iam_ONLY (OTMulti);
	Melder_information (OTMulti_getConstraintIndexFromName (me, GET_STRING (U"Constraint name")));
END2 }

FORM (OTMulti_getRankingValue, U"Get ranking value", nullptr) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. ranking);
END2 }

FORM (OTMulti_getDisharmony, U"Get disharmony", nullptr) {
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. disharmony);
END2 }

DIRECT2 (OTMulti_getNumberOfCandidates) {
	iam_ONLY (OTMulti);
	Melder_information (my numberOfCandidates);
END2 }

FORM (OTMulti_getCandidate, U"Get candidate", nullptr) {
	NATURAL (U"Candidate", U"1")
	OK2
DO
	iam_ONLY (OTMulti);
	long icand = GET_INTEGER (U"Candidate");
	if (icand > my numberOfCandidates)
		Melder_throw (U"The specified candidate number should not exceed the number of candidates.");
	Melder_information (my candidates [icand]. string);
END2 }

FORM (OTMulti_getNumberOfViolations, U"Get number of violations", nullptr) {
	NATURAL (U"Candidate number", U"1")
	NATURAL (U"Constraint number", U"1")
	OK2
DO
	iam_ONLY (OTMulti);
	long icand = GET_INTEGER (U"Candidate number");
	if (icand > my numberOfCandidates)
		Melder_throw (U"The specified candidate number should not exceed the number of candidates.");
	long icons = GET_INTEGER (U"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw (U"The specified constraint number should not exceed the number of constraints.");
	Melder_information (my candidates [icand]. marks [icons]);
END2 }

FORM (OTMulti_getWinner, U"OTMulti: Get winner", nullptr) {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	OK2
DO
	iam_ONLY (OTMulti);
	Melder_information (OTMulti_getWinner (me, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2")));
END2 }

#pragma mark Evaluate

FORM (OTMulti_evaluate, U"OTMulti: Evaluate", nullptr) {
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTMulti);
	OTMulti_newDisharmonies (me, GET_REAL (U"Evaluation noise"));
	praat_dataChanged (me);
END2 }

FORM (OTMulti_generateOptimalForm, U"OTMulti: Generate optimal form", nullptr) {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTMulti);
	char32 output [100];
	OTMulti_generateOptimalForm (me, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
		output, GET_REAL (U"Evaluation noise"));
	Melder_information (output);
	praat_dataChanged (me);
END2 }

FORM (OTMulti_generateOptimalForms, U"OTMulti: Generate optimal forms", nullptr) {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	NATURAL (U"Number of trials", U"1000")
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTMulti);
	autoStrings thee = OTMulti_generateOptimalForms (me, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
		GET_INTEGER (U"Number of trials"), GET_REAL (U"Evaluation noise"));
	praat_new (thee.move(), my name, U"_out");
	praat_dataChanged (me);
END2 }

FORM (OTMulti_to_Distribution, U"OTMulti: Compute output distribution", nullptr) {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	NATURAL (U"Number of trials", U"100000")
	POSITIVE (U"Evaluation noise", U"2.0")
	OK2
DO
	LOOP {
		iam (OTMulti);
		try {
			autoDistributions thee = OTMulti_to_Distribution (me, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
				GET_INTEGER (U"Number of trials"), GET_REAL (U"Evaluation noise"));
			praat_new (thee.move(), my name, U"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END2 }

#pragma mark Modify ranking

FORM (OTMulti_setRanking, U"OTMulti: Set ranking", nullptr) {
	NATURAL (U"Constraint", U"1")
	REAL (U"Ranking", U"100.0")
	REAL (U"Disharmony", U"100.0")
	OK2
DO
	LOOP {
		iam (OTMulti);
		OTMulti_setRanking (me, GET_INTEGER (U"Constraint"), GET_REAL (U"Ranking"), GET_REAL (U"Disharmony"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTMulti_resetAllRankings, U"OTMulti: Reset all rankings", nullptr) {
	REAL (U"Ranking", U"100.0")
	OK2
DO
	LOOP {
		iam (OTMulti);
		OTMulti_reset (me, GET_REAL (U"Ranking"));
		praat_dataChanged (me);
	}
END2 }

FORM (OTMulti_learnOne, U"OTMulti: Learn one", nullptr) {
	SENTENCE (U"Partial form 1", U"")
	SENTENCE (U"Partial form 2", U"")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	OPTIONMENU (U"Direction", 3)
		OPTION (U"forward")
		OPTION (U"backward")
		OPTION (U"bidirectionally")
	POSITIVE (U"Plasticity", U"0.1")
	REAL (U"Rel. plasticity spreading", U"0.1")
	OK2
DO
	LOOP {
		iam (OTMulti);
		try {
			OTMulti_learnOne (me, GET_STRING (U"Partial form 1"), GET_STRING (U"Partial form 2"),
				GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
				GET_INTEGER (U"Direction"), GET_REAL (U"Plasticity"), GET_REAL (U"Rel. plasticity spreading"));
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END2 }

#pragma mark Modify behaviour

FORM (OTMulti_setDecisionStrategy, U"OTMulti: Set decision strategy", nullptr) {
	RADIO_ENUM (U"Decision strategy", kOTGrammar_decisionStrategy, DEFAULT)
	OK2
iam_ONLY (OTMulti);
SET_ENUM (U"Decision strategy", kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	iam_ONLY (OTMulti);
	my decisionStrategy = GET_ENUM (kOTGrammar_decisionStrategy, U"Decision strategy");
	praat_dataChanged (me);
END2 }

FORM (OTMulti_setLeak, U"OTGrammar: Set leak", nullptr) {
	REAL (U"Leak", U"0.0")
	OK2
iam_ONLY (OTMulti);
SET_REAL (U"Leak", my leak);
DO
	iam_ONLY (OTMulti);
	my leak = GET_REAL (U"Leak");
	praat_dataChanged (me);
END2 }

FORM (OTMulti_setConstraintPlasticity, U"OTMulti: Set constraint plasticity", nullptr) {
	NATURAL (U"Constraint", U"1")
	REAL (U"Plasticity", U"1.0")
	OK2
DO
	LOOP {
		iam (OTMulti);
		OTMulti_setConstraintPlasticity (me, GET_INTEGER (U"Constraint"), GET_REAL (U"Plasticity"));
		praat_dataChanged (me);
	}
END2 }

#pragma mark Modify structure

FORM (OTMulti_removeConstraint, U"OTMulti: Remove constraint", nullptr) {
	SENTENCE (U"Constraint name", U"")
	OK2
DO
	LOOP {
		iam (OTMulti);
		OTMulti_removeConstraint (me, GET_STRING (U"Constraint name"));
		praat_dataChanged (me);
	}
END2 }

#pragma mark OTMULTI & PAIRDISTRIBUTION

FORM (OTMulti_PairDistribution_learn, U"OTMulti & PairDistribution: Learn", nullptr) {
	REAL (U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (U"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	OPTIONMENU (U"Direction", 3)
		OPTION (U"forward")
		OPTION (U"backward")
		OPTION (U"bidirectionally")
	POSITIVE (U"Initial plasticity", U"1.0")
	NATURAL (U"Replications per plasticity", U"100000")
	REAL (U"Plasticity decrement", U"0.1")
	NATURAL (U"Number of plasticities", U"4")
	REAL (U"Rel. plasticity spreading", U"0.1")
	INTEGER (U"Store history every", U"0")
	OK2
DO
	iam_ONLY (OTMulti);
	thouart_ONLY (PairDistribution);
	autoTable history;
	try {
		OTMulti_PairDistribution_learn (me, thee,
			GET_REAL (U"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, U"Update rule"),
			GET_INTEGER (U"Direction"),
			GET_REAL (U"Initial plasticity"), GET_INTEGER (U"Replications per plasticity"),
			GET_REAL (U"Plasticity decrement"), GET_INTEGER (U"Number of plasticities"),
			GET_REAL (U"Rel. plasticity spreading"),
			GET_INTEGER (U"Store history every"), & history);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // e.g. in case of partial learning
		Melder_flushError ();
		// trickle down to save history
	}
	if (history) praat_new (history.move(), my name);
END2 }

#pragma mark OTMULTI & STRINGS

FORM (OTMulti_Strings_generateOptimalForms, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (U"Evaluation noise", U"2.0")
	OK2
DO
	iam_ONLY (OTMulti);
	thouart_ONLY (Strings);
	autoStrings him = OTMulti_Strings_generateOptimalForms (me, thee, GET_REAL (U"Evaluation noise"));
	praat_new (him.move(), my name, U"_out");
	praat_dataChanged (me);
END2 }

#pragma mark -
#pragma mark RBM

#pragma mark New

FORM (Create_RBM, U"Create RBM (Restricted Boltzmann Machine)", nullptr) {
	WORD (U"Name", U"network")
	NATURAL (U"Number of input nodes", U"50")
	NATURAL (U"Number of output nodes", U"20")
	BOOLEAN (U"Inputs are binary", true)
	OK2
DO
	autoRBM me = RBM_create (
		GET_INTEGER (U"Number of input nodes"),
		GET_INTEGER (U"Number of output nodes"),
		GET_INTEGER (U"Inputs are binary"));
	praat_new (me.move(), GET_STRING (U"Name"));
END2 }

#pragma mark Modify

DIRECT2 (RBM_spreadUp) {
	LOOP {
		iam_LOOP (RBM);
		RBM_spreadUp (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (RBM_spreadDown) {
	LOOP {
		iam_LOOP (RBM);
		RBM_spreadDown (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (RBM_spreadDown_reconstruction) {
	LOOP {
		iam_LOOP (RBM);
		RBM_spreadDown_reconstruction (me);
		praat_dataChanged (me);
	}
END2 }

DIRECT2 (RBM_spreadUp_reconstruction) {
	LOOP {
		iam_LOOP (RBM);
		RBM_spreadUp_reconstruction (me);
		praat_dataChanged (me);
	}
END2 }

FORM (RBM_update, U"RBM: Update", nullptr) {
	POSITIVE (U"Learning rate", U"0.001")
	OK2
DO
	LOOP {
		iam_LOOP (RBM);
		RBM_update (me, GET_REAL (U"Learning rate"));
		praat_dataChanged (me);
	}
END2 }

#pragma mark Extract

DIRECT2 (RBM_extractInputActivities) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractInputActivities (me);
		praat_new (thee.move(), my name, U"_inputActivities");
	}
END2 }

DIRECT2 (RBM_extractOutputActivities) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractOutputActivities (me);
		praat_new (thee.move(), my name, U"_outputActivities");
	}
END2 }

DIRECT2 (RBM_extractInputReconstruction) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractInputReconstruction (me);
		praat_new (thee.move(), my name, U"_inputReconstruction");
	}
END2 }

DIRECT2 (RBM_extractOutputReconstruction) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractOutputReconstruction (me);
		praat_new (thee.move(), my name, U"_outputReconstruction");
	}
END2 }

DIRECT2 (RBM_extractInputBiases) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractInputBiases (me);
		praat_new (thee.move(), my name, U"_inputBiases");
	}
END2 }

DIRECT2 (RBM_extractOutputBiases) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractOutputBiases (me);
		praat_new (thee.move(), my name, U"_outputBiases");
	}
END2 }

DIRECT2 (RBM_extractWeights) {
	LOOP {
		iam_LOOP (RBM);
		autoMatrix thee = RBM_extractWeights (me);
		praat_new (thee.move(), my name, U"_weights");
	}
END2 }

#pragma mark RBM & PATTERN

FORM (RBM_Pattern_applyToInput, U"RBM & Pattern: Apply to input", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	iam_ONLY (RBM);
	thouart_ONLY (Pattern);
	RBM_Pattern_applyToInput (me, thee, GET_INTEGER (U"Row number"));
	praat_dataChanged (me);
END2 }

FORM (RBM_Pattern_applyToOutput, U"RBM & Pattern: Apply to output", nullptr) {
	NATURAL (U"Row number", U"1")
	OK2
DO
	iam_ONLY (RBM);
	thouart_ONLY (Pattern);
	RBM_Pattern_applyToOutput (me, thee, GET_INTEGER (U"Row number"));
	praat_dataChanged (me);
END2 }

FORM (RBM_Pattern_learn, U"RBM & Pattern: Learn", nullptr) {
	POSITIVE (U"Learning rate", U"0.001")
	OK2
DO
	iam_ONLY (RBM);
	thouart_ONLY (Pattern);
	RBM_Pattern_learn (me, thee, GET_REAL (U"Learning rate"));
	praat_dataChanged (me);
END2 }

#pragma mark -
#pragma mark buttons

void praat_uvafon_gram_init ();
void praat_uvafon_gram_init () {
	Thing_recognizeClassesByName (classNetwork,
		classOTGrammar, classOTHistory, classOTMulti,
		classRBM,
		nullptr);
	Thing_recognizeClassByOtherName (classOTGrammar, U"OTCase");

	praat_addMenuCommand (U"Objects", U"New", U"Constraint grammars", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"OT learning tutorial", nullptr, 1, DO_OT_learning_tutorial);
		praat_addMenuCommand (U"Objects", U"New", U"-- tableau grammars --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create NoCoda grammar", nullptr, 1, DO_Create_NoCoda_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation grammar", nullptr, 1, DO_Create_NPA_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation distribution", nullptr, 1, DO_Create_NPA_distribution);
		praat_addMenuCommand (U"Objects", U"New", U"Create tongue-root grammar...", nullptr, 1, DO_Create_tongue_root_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create metrics grammar...", nullptr, 1, DO_Create_metrics_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create multi-level metrics grammar...", nullptr, 1, DO_Create_multi_level_metrics_grammar);
	praat_addAction1 (classOTGrammar, 1, U"Save as headerless spreadsheet file...", nullptr, 0, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 1, U"Write to headerless spreadsheet file...", nullptr, praat_HIDDEN, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);

	praat_addAction1 (classOTGrammar, 0, U"OTGrammar help", nullptr, 0, DO_OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, U"Edit", nullptr, praat_HIDDEN, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau...", nullptr, 0, DO_OTGrammar_drawTableau);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau (narrowly)...", nullptr, 0, DO_OTGrammar_drawTableau_narrowly);
	praat_addAction1 (classOTGrammar, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get number of constraints", nullptr, 1, DO_OTGrammar_getNumberOfConstraints);
		praat_addAction1 (classOTGrammar, 1, U"Get constraint...", nullptr, 1, DO_OTGrammar_getConstraint);
		praat_addAction1 (classOTGrammar, 1, U"Get ranking value...", nullptr, 1, DO_OTGrammar_getRankingValue);
		praat_addAction1 (classOTGrammar, 1, U"Get disharmony...", nullptr, 1, DO_OTGrammar_getDisharmony);
		praat_addAction1 (classOTGrammar, 1, U"Get number of tableaus", nullptr, 1, DO_OTGrammar_getNumberOfTableaus);
		praat_addAction1 (classOTGrammar, 1, U"Get input...", nullptr, 1, DO_OTGrammar_getInput);
		praat_addAction1 (classOTGrammar, 1, U"Get number of candidates...", nullptr, 1, DO_OTGrammar_getNumberOfCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get candidate...", nullptr, 1, DO_OTGrammar_getCandidate);
		praat_addAction1 (classOTGrammar, 1, U"Get number of violations...", nullptr, 1, DO_OTGrammar_getNumberOfViolations);
		praat_addAction1 (classOTGrammar, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get winner...", nullptr, 1, DO_OTGrammar_getWinner);
		praat_addAction1 (classOTGrammar, 1, U"Compare candidates...", nullptr, 1, DO_OTGrammar_compareCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get number of optimal candidates...", nullptr, 1, DO_OTGrammar_getNumberOfOptimalCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate grammatical...", nullptr, 1, DO_OTGrammar_isCandidateGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate singly grammatical...", nullptr, 1, DO_OTGrammar_isCandidateSinglyGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Get interpretive parse...", nullptr, 1, DO_OTGrammar_getInterpretiveParse);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output grammatical...", nullptr, 1, DO_OTGrammar_isPartialOutputGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output singly grammatical...", nullptr, 1, DO_OTGrammar_isPartialOutputSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 0, U"Generate inputs...", nullptr, 0, DO_OTGrammar_generateInputs);
	praat_addAction1 (classOTGrammar, 0, U"Get inputs", nullptr, 0, DO_OTGrammar_getInputs);
	praat_addAction1 (classOTGrammar, 0, U"Measure typology", nullptr, 0, DO_OTGrammar_measureTypology);
	praat_addAction1 (classOTGrammar, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Evaluate...", nullptr, 0, DO_OTGrammar_evaluate);
		praat_addAction1 (classOTGrammar, 0, U"Input to output...", nullptr, 0, DO_OTGrammar_inputToOutput);
		praat_addAction1 (classOTGrammar, 0, U"Input to outputs...", nullptr, 0, DO_OTGrammar_inputToOutputs);
		praat_addAction1 (classOTGrammar, 0, U"To output Distributions...", nullptr, 0, DO_OTGrammar_to_Distributions);
		praat_addAction1 (classOTGrammar, 0, U"To PairDistribution...", nullptr, 0, DO_OTGrammar_to_PairDistribution);
	praat_addAction1 (classOTGrammar, 0, U"Modify ranking -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Set ranking...", nullptr, 1, DO_OTGrammar_setRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset all rankings...", nullptr, 1, DO_OTGrammar_resetAllRankings);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random ranking...", nullptr, 1, DO_OTGrammar_resetToRandomRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random total ranking...", nullptr, 1, DO_OTGrammar_resetToRandomTotalRanking);
		praat_addAction1 (classOTGrammar, 0, U"Learn one...", nullptr, 1, DO_OTGrammar_learnOne);
		praat_addAction1 (classOTGrammar, 0, U"Learn one from partial output...", nullptr, 1, DO_OTGrammar_learnOneFromPartialOutput);
	praat_addAction1 (classOTGrammar, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Set harmony computation method...", nullptr, praat_DEPTH_1 + praat_HIDDEN, DO_OTGrammar_setDecisionStrategy);
		praat_addAction1 (classOTGrammar, 1, U"Set decision strategy...", nullptr, 1, DO_OTGrammar_setDecisionStrategy);
		praat_addAction1 (classOTGrammar, 1, U"Set leak...", nullptr, 1, DO_OTGrammar_setLeak);
		praat_addAction1 (classOTGrammar, 1, U"Set constraint plasticity...", nullptr, 1, DO_OTGrammar_setConstraintPlasticity);
	praat_addAction1 (classOTGrammar, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Remove constraint...", nullptr, 1, DO_OTGrammar_removeConstraint);
		praat_addAction1 (classOTGrammar, 0, U"Remove harmonically bounded candidates...", nullptr, 1, DO_OTGrammar_removeHarmonicallyBoundedCandidates);

	{ void praat_TableOfReal_init (ClassInfo klas); praat_TableOfReal_init (classOTHistory); }

	praat_addAction1 (classOTMulti, 0, U"View & Edit", nullptr, praat_ATTRACTIVE, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, U"Edit", nullptr, praat_HIDDEN, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau...", nullptr, 1, DO_OTMulti_drawTableau);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau (narrowly)...", nullptr, 1, DO_OTMulti_drawTableau_narrowly);
	praat_addAction1 (classOTMulti, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get number of constraints", nullptr, 1, DO_OTMulti_getNumberOfConstraints);
		praat_addAction1 (classOTMulti, 1, U"Get constraint...", nullptr, 1, DO_OTMulti_getConstraint);
		praat_addAction1 (classOTMulti, 1, U"Get constraint number...", nullptr, 1, DO_OTMulti_getConstraintIndexFromName);
		praat_addAction1 (classOTMulti, 1, U"Get ranking value...", nullptr, 1, DO_OTMulti_getRankingValue);
		praat_addAction1 (classOTMulti, 1, U"Get disharmony...", nullptr, 1, DO_OTMulti_getDisharmony);
		praat_addAction1 (classOTMulti, 1, U"Get number of candidates", nullptr, 1, DO_OTMulti_getNumberOfCandidates);
		praat_addAction1 (classOTMulti, 1, U"Get candidate...", nullptr, 1, DO_OTMulti_getCandidate);
		praat_addAction1 (classOTMulti, 1, U"Get number of violations...", nullptr, 1, DO_OTMulti_getNumberOfViolations);
		praat_addAction1 (classOTMulti, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get winner...", nullptr, 1, DO_OTMulti_getWinner);
	praat_addAction1 (classOTMulti, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Evaluate...", nullptr, 1, DO_OTMulti_evaluate);
		praat_addAction1 (classOTMulti, 0, U"Get output...", nullptr, 1, DO_OTMulti_generateOptimalForm);
		praat_addAction1 (classOTMulti, 0, U"Get outputs...", nullptr, 1, DO_OTMulti_generateOptimalForms);
		praat_addAction1 (classOTMulti, 0, U"To output Distribution...", nullptr, 1, DO_OTMulti_to_Distribution);
	praat_addAction1 (classOTMulti, 0, U"Modify ranking", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Set ranking...", nullptr, 0, DO_OTMulti_setRanking);
		praat_addAction1 (classOTMulti, 0, U"Reset all rankings...", nullptr, 0, DO_OTMulti_resetAllRankings);
		praat_addAction1 (classOTMulti, 0, U"Learn one...", nullptr, 0, DO_OTMulti_learnOne);
	praat_addAction1 (classOTMulti, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Set decision strategy...", nullptr, 1, DO_OTMulti_setDecisionStrategy);
		praat_addAction1 (classOTMulti, 1, U"Set leak...", nullptr, 1, DO_OTMulti_setLeak);
		praat_addAction1 (classOTMulti, 1, U"Set constraint plasticity...", nullptr, 1, DO_OTMulti_setConstraintPlasticity);
	praat_addAction1 (classOTMulti, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Remove constraint...", nullptr, 1, DO_OTMulti_removeConstraint);

	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs grammatical?", nullptr, 1, DO_OTGrammar_Strings_areAllPartialOutputsGrammatical);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs singly grammatical?", nullptr, 1, DO_OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Inputs to outputs...", nullptr, 0, DO_OTGrammar_Strings_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Learn from partial outputs...", nullptr, 0, DO_OTGrammar_Strings_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, U"Learn...", nullptr, 0, DO_OTGrammar_Stringses_learn);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs...", nullptr, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (rrip)...", nullptr, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs_rrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (eip)...", nullptr, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs_eip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (wrip)...", nullptr, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs_wrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0, DO_OTGrammar_Distributions_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"List obligatory rankings...", nullptr, praat_HIDDEN, DO_OTGrammar_Distributions_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Learn...", nullptr, 0, DO_OTGrammar_PairDistribution_learn);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Find positive weights...", nullptr, 0, DO_OTGrammar_PairDistribution_findPositiveWeights);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get fraction correct...", nullptr, 0, DO_OTGrammar_PairDistribution_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get minimum number correct...", nullptr, 0, DO_OTGrammar_PairDistribution_getMinimumNumberCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"List obligatory rankings", nullptr, 0, DO_OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, U"Learn...", nullptr, 0, DO_OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, U"Get outputs...", nullptr, 0, DO_OTMulti_Strings_generateOptimalForms);

	praat_addMenuCommand (U"Objects", U"New", U"Symmetric neural networks", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty Network...", nullptr, 1, DO_Create_empty_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network...", nullptr, 1, DO_Create_rectangular_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network (vertical)...", nullptr, 1, DO_Create_rectangular_Network_vertical);
		praat_addMenuCommand (U"Objects", U"New", U"Create RBM...", nullptr, 1, DO_Create_RBM);

	praat_addAction1 (classNetwork, 0, U"Draw...", nullptr, 0, DO_Network_draw);
	praat_addAction1 (classNetwork, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"List nodes...", nullptr, 1, DO_Network_listNodes);
		praat_addAction1 (classNetwork, 1, U"Nodes down to table...", nullptr, 1, DO_Network_nodes_downto_Table);
	praat_addAction1 (classNetwork, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"Get activity...", nullptr, 1, DO_Network_getActivity);
		praat_addAction1 (classNetwork, 1, U"Get weight...", nullptr, 1, DO_Network_getWeight);
	praat_addAction1 (classNetwork, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 0, U"Add node...", nullptr, 1, DO_Network_addNode);
		praat_addAction1 (classNetwork, 0, U"Add connection...", nullptr, 1, DO_Network_addConnection);
		praat_addAction1 (classNetwork, 1, U"-- activity --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set activity...", nullptr, 1, DO_Network_setActivity);
		praat_addAction1 (classNetwork, 0, U"Set clamping...", nullptr, 1, DO_Network_setClamping);
		praat_addAction1 (classNetwork, 0, U"Zero activities...", nullptr, 1, DO_Network_zeroActivities);
		praat_addAction1 (classNetwork, 0, U"Normalize activities...", nullptr, 1, DO_Network_normalizeActivities);
		praat_addAction1 (classNetwork, 0, U"Spread activities...", nullptr, 1, DO_Network_spreadActivities);
		praat_addAction1 (classNetwork, 1, U"Set activity clipping rule...", nullptr, 1, DO_Network_setActivityClippingRule);
		praat_addAction1 (classNetwork, 1, U"Set activity leak...", nullptr, 1, DO_Network_setActivityLeak);
		praat_addAction1 (classNetwork, 1, U"Set shunting...", nullptr, 1, DO_Network_setShunting);
		praat_addAction1 (classNetwork, 1, U"-- weight --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set weight...", nullptr, 1, DO_Network_setWeight);
		praat_addAction1 (classNetwork, 0, U"Update weights", nullptr, 1, DO_Network_updateWeights);
		praat_addAction1 (classNetwork, 0, U"Normalize weights...", nullptr, 1, DO_Network_normalizeWeights);
		praat_addAction1 (classNetwork, 1, U"Set instar...", nullptr, 1, DO_Network_setInstar);
		praat_addAction1 (classNetwork, 1, U"Set outstar...", nullptr, 1, DO_Network_setOutstar);
		praat_addAction1 (classNetwork, 1, U"Set weight leak...", nullptr, 1, DO_Network_setWeightLeak);

	praat_addAction1 (classRBM, 0, U"Modify", nullptr, 0, nullptr);
		praat_addAction1 (classRBM, 0, U"Spread up", nullptr, 0, DO_RBM_spreadUp);
		praat_addAction1 (classRBM, 0, U"Spread down", nullptr, 0, DO_RBM_spreadDown);
		praat_addAction1 (classRBM, 0, U"Spread up (reconstruction)", nullptr, 0, DO_RBM_spreadUp_reconstruction);
		praat_addAction1 (classRBM, 0, U"Spread down (reconstruction)", nullptr, 0, DO_RBM_spreadDown_reconstruction);
		praat_addAction1 (classRBM, 0, U"Update...", nullptr, 0, DO_RBM_update);
	praat_addAction1 (classRBM, 0, U"Extract", nullptr, 0, nullptr);
		praat_addAction1 (classRBM, 0, U"Extract input activities", nullptr, 0, DO_RBM_extractInputActivities);
		praat_addAction1 (classRBM, 0, U"Extract output activities", nullptr, 0, DO_RBM_extractOutputActivities);
		praat_addAction1 (classRBM, 0, U"Extract input reconstruction", nullptr, 0, DO_RBM_extractInputReconstruction);
		praat_addAction1 (classRBM, 0, U"Extract output reconstruction", nullptr, 0, DO_RBM_extractOutputReconstruction);
		praat_addAction1 (classRBM, 0, U"Extract input biases", nullptr, 0, DO_RBM_extractInputBiases);
		praat_addAction1 (classRBM, 0, U"Extract output biases", nullptr, 0, DO_RBM_extractOutputBiases);
		praat_addAction1 (classRBM, 0, U"Extract weights", nullptr, 0, DO_RBM_extractWeights);

	praat_addAction2 (classRBM, 1, classPattern, 1, U"Apply to input...", nullptr, 0, DO_RBM_Pattern_applyToInput);
	praat_addAction2 (classRBM, 1, classPattern, 1, U"Apply to output...", nullptr, 0, DO_RBM_Pattern_applyToOutput);
	praat_addAction2 (classRBM, 1, classPattern, 1, U"Learn...", nullptr, 0, DO_RBM_Pattern_learn);
}

/* End of file praat_gram.cpp */
