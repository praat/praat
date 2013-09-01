/* praat_gram.cpp
 *
 * Copyright (C) 1997-2012,2013 Paul Boersma
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

#include "praat.h"

#include "Network.h"
#include "OTGrammar.h"
#include "OTMulti.h"
#include "OTGrammarEditor.h"
#include "OTMultiEditor.h"

#undef iam
#define iam iam_LOOP

/***** HELP *****/

DIRECT (OT_learning_tutorial) Melder_help (L"OT learning"); END

DIRECT (OTGrammar_help) Melder_help (L"OTGrammar"); END

/***** NETWORK *****/

static void UiForm_addNetworkFields (UiForm dia) {
	Any radio;
	LABEL (L"", L"Activity spreading settings:")
	REAL (L"Spreading rate", L"0.01")
	OPTIONMENU_ENUM (L"Activity clipping rule", kNetwork_activityClippingRule, DEFAULT)
	REAL (L"left Activity range", L"0.0")
	REAL (L"right Activity range", L"1.0")
	REAL (L"Activity leak", L"1.0")
	LABEL (L"", L"Weight update settings:")
	REAL (L"Learning rate", L"0.1")
	REAL (L"left Weight range", L"-1.0")
	REAL (L"right Weight range", L"1.0")
	REAL (L"Weight leak", L"0.0")
}

FORM (Create_empty_Network, L"Create empty Network", 0)
	WORD (L"Name", L"network")
	UiForm_addNetworkFields (dia);
	LABEL (L"", L"World coordinates:")
	REAL (L"left x range", L"0.0")
	REAL (L"right x range", L"10.0")
	REAL (L"left y range", L"0.0")
	REAL (L"right y range", L"10.0")
	OK
DO
	autoNetwork me = Network_create (GET_REAL (L"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, L"Activity clipping rule"),
		GET_REAL (L"left Activity range"), GET_REAL (L"right Activity range"), GET_REAL (L"Activity leak"),
		GET_REAL (L"Learning rate"), GET_REAL (L"left Weight range"), GET_REAL (L"right Weight range"), GET_REAL (L"Weight leak"),
		GET_REAL (L"left x range"), GET_REAL (L"right x range"), GET_REAL (L"left y range"), GET_REAL (L"right y range"),
		0, 0);
	praat_new (me.transfer(), GET_STRING (L"Name"));
END

FORM (Create_rectangular_Network, L"Create rectangular Network", 0)
	UiForm_addNetworkFields (dia);
	LABEL (L"", L"Structure settings:")
	NATURAL (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"10")
	BOOLEAN (L"Bottom row clamped", 1)
	LABEL (L"", L"Initial state settings:")
	REAL (L"left Initial weight range", L"-0.1")
	REAL (L"right Initial weight range", L"0.1")
	OK
DO
	autoNetwork me = Network_create_rectangle (GET_REAL (L"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, L"Activity clipping rule"),
		GET_REAL (L"left Activity range"), GET_REAL (L"right Activity range"), GET_REAL (L"Activity leak"),
		GET_REAL (L"Learning rate"), GET_REAL (L"left Weight range"), GET_REAL (L"right Weight range"), GET_REAL (L"Weight leak"),
		GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns"),
		GET_INTEGER (L"Bottom row clamped"),
		GET_REAL (L"left Initial weight range"), GET_REAL (L"right Initial weight range"));
	praat_new (me.transfer(),
			L"rectangle_", Melder_integer (GET_INTEGER (L"Number of rows")),
			L"_", Melder_integer (GET_INTEGER (L"Number of columns")));
END

FORM (Create_rectangular_Network_vertical, L"Create rectangular Network (vertical)", 0)
	UiForm_addNetworkFields (dia);
	LABEL (L"", L"Structure settings:")
	NATURAL (L"Number of rows", L"10")
	NATURAL (L"Number of columns", L"10")
	BOOLEAN (L"Bottom row clamped", 1)
	LABEL (L"", L"Initial state settings:")
	REAL (L"left Initial weight range", L"-0.1")
	REAL (L"right Initial weight range", L"0.1")
	OK
DO
	autoNetwork me = Network_create_rectangle_vertical (GET_REAL (L"Spreading rate"), GET_ENUM (kNetwork_activityClippingRule, L"Activity clipping rule"),
		GET_REAL (L"left Activity range"), GET_REAL (L"right Activity range"), GET_REAL (L"Activity leak"),
		GET_REAL (L"Learning rate"), GET_REAL (L"left Weight range"), GET_REAL (L"right Weight range"), GET_REAL (L"Weight leak"),
		GET_INTEGER (L"Number of rows"), GET_INTEGER (L"Number of columns"),
		GET_INTEGER (L"Bottom row clamped"),
		GET_REAL (L"left Initial weight range"), GET_REAL (L"right Initial weight range"));
	praat_new (me.transfer(),
			L"rectangle_", Melder_integer (GET_INTEGER (L"Number of rows")),
			L"_", Melder_integer (GET_INTEGER (L"Number of columns")));
END

FORM (Network_addConnection, L"Network: Add connection", 0)
	NATURAL (L"From node", L"1")
	NATURAL (L"To node", L"2")
	REAL (L"Weight", L"0.0")
	REAL (L"Plasticity", L"1.0")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_addConnection (GET_INTEGER (L"From node"), GET_INTEGER (L"To node"), GET_REAL (L"Weight"), GET_REAL (L"Plasticity"));
		praat_dataChanged (me);
	}
END

FORM (Network_addNode, L"Network: Add node", 0)
	REAL (L"x", L"5.0")
	REAL (L"y", L"5.0")
	REAL (L"Activity", L"0.0")
	BOOLEAN (L"Clamping", 0)
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_addNode (GET_REAL (L"x"), GET_REAL (L"y"), GET_REAL (L"Activity"), GET_INTEGER (L"Clamping"));
		praat_dataChanged (me);
	}
END

FORM (Network_nodes_downto_Table, L"Network: Nodes down to Table", 0)
	INTEGER (L"From node number", L"1")
	INTEGER (L"To node number", L"1000")
	BOOLEAN (L"Include node numbers", true)
	BOOLEAN (L"Include x", false)
	BOOLEAN (L"Include y", false)
	INTEGER (L"Position decimals", L"6")
	BOOLEAN (L"Include clamped", false)
	BOOLEAN (L"Include activity", true)
	BOOLEAN (L"Include excitation", false)
	INTEGER (L"Activity decimals", L"6")
	OK
DO
	LOOP {
		iam (Network);
		autoTable thee = my f_nodes_downto_Table (GET_INTEGER (L"From node number"), GET_INTEGER (L"To node number"),
			GET_INTEGER (L"Include node numbers"),
			GET_INTEGER (L"Include x"), GET_INTEGER (L"Include y"), GET_INTEGER (L"Position decimals"),
			GET_INTEGER (L"Include clamped"),
			GET_INTEGER (L"Include activity"), GET_INTEGER (L"Include excitation"), GET_INTEGER (L"Activity decimals"));
		praat_new (thee.transfer(), my name);
	}
END

FORM (Network_draw, L"Draw Network", 0)
	BOOLEAN (L"Colour", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam_LOOP (Network);
		me -> f_draw (GRAPHICS, GET_INTEGER (L"Colour"));
	}
END

FORM (Network_getActivity, L"Network: Get activity", 0)
	NATURAL (L"Node", L"1")
	OK
DO
	iam_ONLY (Network);
	double activity = me -> f_getActivity (GET_INTEGER (L"Node"));
	Melder_information (Melder_double (activity));
END

FORM (Network_getWeight, L"Network: Get weight", 0)
	NATURAL (L"Connection", L"1")
	OK
DO
	iam_ONLY (Network);
	double weight = me -> f_getWeight (GET_INTEGER (L"Connection"));
	Melder_information (Melder_double (weight));
END

FORM (Network_listNodes, L"Network: List nodes", 0)
	INTEGER (L"From node number", L"1")
	INTEGER (L"To node number", L"1000")
	BOOLEAN (L"Include node numbers", true)
	BOOLEAN (L"Include x", false)
	BOOLEAN (L"Include y", false)
	INTEGER (L"Position decimals", L"6")
	BOOLEAN (L"Include clamped", false)
	BOOLEAN (L"Include activity", true)
	BOOLEAN (L"Include excitation", false)
	INTEGER (L"Activity decimals", L"6")
	OK
DO
	LOOP {
		iam (Network);
		my f_listNodes (GET_INTEGER (L"From node number"), GET_INTEGER (L"To node number"),
			GET_INTEGER (L"Include node numbers"),
			GET_INTEGER (L"Include x"), GET_INTEGER (L"Include y"), GET_INTEGER (L"Position decimals"),
			GET_INTEGER (L"Include clamped"),
			GET_INTEGER (L"Include activity"), GET_INTEGER (L"Include excitation"), GET_INTEGER (L"Activity decimals"));
	}
END

FORM (Network_normalizeActivities, L"Network: Normalize activities", 0)
	INTEGER (L"From node", L"1")
	INTEGER (L"To node", L"0 (= all)")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_normalizeActivities (GET_INTEGER (L"From node"), GET_INTEGER (L"To node"));
		praat_dataChanged (me);
	}
END

FORM (Network_normalizeWeights, L"Network: Normalize weights", 0)
	INTEGER (L"From node", L"1")
	INTEGER (L"To node", L"0 (= all)")
	INTEGER (L"From incoming node", L"1")
	INTEGER (L"To incoming node", L"10")
	REAL (L"New sum", L"1.0")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_normalizeWeights (GET_INTEGER (L"From node"), GET_INTEGER (L"To node"),
			GET_INTEGER (L"From incoming node"), GET_INTEGER (L"To incoming node"), GET_REAL (L"New sum"));
		praat_dataChanged (me);
	}
END

FORM (Network_setActivity, L"Network: Set activity", 0)
	NATURAL (L"Node", L"1")
	REAL (L"Activity", L"1.0")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_setActivity (GET_INTEGER (L"Node"), GET_REAL (L"Activity"));
		praat_dataChanged (me);
	}
END

FORM (Network_setActivityClippingRule, L"Network: Set activity clipping rule", 0)
	RADIO_ENUM (L"Activity clipping rule", kNetwork_activityClippingRule, DEFAULT)
	OK
iam_ONLY (Network);
SET_ENUM (L"Activity clipping rule", kNetwork_activityClippingRule, my d_activityClippingRule);
DO
	iam_ONLY (Network);
	me -> f_setActivityClippingRule (GET_ENUM (kNetwork_activityClippingRule, L"Activity clipping rule"));
	praat_dataChanged (me);
END

FORM (Network_setActivityLeak, L"Network: Set activity leak", 0)
	REAL (L"Activity leak", L"1.0")
	OK
iam_ONLY (Network);
SET_REAL (L"Activity leak", my d_activityLeak);
DO
	iam_ONLY (Network);
	me -> f_setActivityLeak (GET_REAL (L"Activity leak"));
	praat_dataChanged (me);
END

FORM (Network_setClamping, L"Network: Set clamping", 0)
	NATURAL (L"Node", L"1")
	BOOLEAN (L"Clamping", 1)
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_setClamping (GET_INTEGER (L"Node"), GET_INTEGER (L"Clamping"));
		praat_dataChanged (me);
	}
END

FORM (Network_setInstar, L"Network: Set instar", 0)
	REAL (L"Instar", L"0.0")
	OK
iam_ONLY (Network);
SET_REAL (L"Instar", my d_instar);
DO
	iam_ONLY (Network);
	me -> f_setInstar (GET_REAL (L"Instar"));
	praat_dataChanged (me);
END

FORM (Network_setWeightLeak, L"Network: Set weight leak", 0)
	REAL (L"Weight leak", L"0.0")
	OK
iam_ONLY (Network);
SET_REAL (L"Weight leak", my d_weightLeak);
DO
	iam_ONLY (Network);
	me -> f_setWeightLeak (GET_REAL (L"Weight leak"));
	praat_dataChanged (me);
END

FORM (Network_setOutstar, L"Network: Set outstar", 0)
	REAL (L"Outstar", L"0.0")
	OK
iam_ONLY (Network);
SET_REAL (L"Outstar", my d_outstar);
DO
	iam_ONLY (Network);
	me -> f_setOutstar (GET_REAL (L"Outstar"));
	praat_dataChanged (me);
END

FORM (Network_setShunting, L"Network: Set shunting", 0)
	REAL (L"Shunting", L"1.0")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_setShunting (GET_REAL (L"Shunting"));
		praat_dataChanged (me);
	}
END

FORM (Network_setWeight, L"Network: Set weight", 0)
	NATURAL (L"Connection", L"1")
	REAL (L"Weight", L"1.0")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_setWeight (GET_INTEGER (L"Connection"), GET_REAL (L"Weight"));
		praat_dataChanged (me);
	}
END

FORM (Network_spreadActivities, L"Network: Spread activities", 0)
	NATURAL (L"Number of steps", L"20")
	OK
DO
	LOOP {
		iam_LOOP (Network);
		me -> f_spreadActivities (GET_INTEGER (L"Number of steps"));
		praat_dataChanged (me);
	}
END

DIRECT (Network_updateWeights)
	LOOP {
		iam_LOOP (Network);
		me -> f_updateWeights ();
		praat_dataChanged (me);
	}
END

FORM (Network_zeroActivities, L"Network: Zero activities", 0)
	INTEGER (L"From node", L"1")
	INTEGER (L"To node", L"0 (= all)")
	OK
DO
	LOOP {
		iam (Network);
		me -> f_zeroActivities (GET_INTEGER (L"From node"), GET_INTEGER (L"To node"));
		praat_dataChanged (me);
	}
END


/***** OTGRAMMAR *****/

FORM (Create_metrics_grammar, L"Create metrics grammar", 0)
	OPTIONMENU (L"Initial ranking", 1)
		OPTION (L"Equal")
		OPTION (L"Foot form high")
		OPTION (L"WSP high")
	OPTIONMENU (L"Trochaicity constraint", 1)
		OPTION (L"FtNonfinal")
		OPTION (L"Trochaic")
	BOOLEAN (L"Include FootBimoraic", 0)
	BOOLEAN (L"Include FootBisyllabic", 0)
	BOOLEAN (L"Include Peripheral", 0)
	OPTIONMENU (L"Nonfinality constraint", 1)
		OPTION (L"Nonfinal")
		OPTION (L"MainNonfinal")
		OPTION (L"HeadNonfinal")
	BOOLEAN (L"Overt forms have secondary stress", 1)
	BOOLEAN (L"Include *Clash and *Lapse", 0)
	BOOLEAN (L"Include codas", 0)
	OK
DO
	praat_new (OTGrammar_create_metrics (GET_INTEGER (L"Initial ranking"), GET_INTEGER (L"Trochaicity constraint"),
		GET_INTEGER (L"Include FootBimoraic"), GET_INTEGER (L"Include FootBisyllabic"),
		GET_INTEGER (L"Include Peripheral"), GET_INTEGER (L"Nonfinality constraint"),
		GET_INTEGER (L"Overt forms have secondary stress"), GET_INTEGER (L"Include *Clash and *Lapse"), GET_INTEGER (L"Include codas")),
		GET_STRING (L"Initial ranking"));
END

DIRECT (Create_NoCoda_grammar)
	autoOTGrammar me = OTGrammar_create_NoCoda_grammar ();
	praat_new (me.transfer(), L"NoCoda");
END

DIRECT (Create_NPA_grammar)
	autoOTGrammar me = OTGrammar_create_NPA_grammar ();
	praat_new (me.transfer(), L"assimilation");
END

DIRECT (Create_NPA_distribution)
	autoPairDistribution me = OTGrammar_create_NPA_distribution ();
	praat_new (me.transfer(), L"assimilation");
END

FORM (Create_tongue_root_grammar, L"Create tongue-root grammar", L"Create tongue-root grammar...")
	RADIO (L"Constraint set", 1)
		RADIOBUTTON (L"Five")
		RADIOBUTTON (L"Nine")
	RADIO (L"Ranking", 3)
		RADIOBUTTON (L"Equal")
		RADIOBUTTON (L"Random")
		RADIOBUTTON (L"Infant")
		RADIOBUTTON (L"Wolof")
	OK
DO
	autoOTGrammar me = OTGrammar_create_tongueRoot_grammar (GET_INTEGER (L"Constraint set"), GET_INTEGER (L"Ranking"));
	praat_new (me.transfer(), GET_STRING (L"Ranking"));
END

FORM (OTGrammar_drawTableau, L"Draw tableau", L"OT learning")
	SENTENCE (L"Input string", L"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTGrammar);
		OTGrammar_drawTableau (me, GRAPHICS, false, GET_STRING (L"Input string"));
	}
END

FORM (OTGrammar_drawTableau_narrowly, L"Draw tableau (narrowly)", L"OT learning")
	SENTENCE (L"Input string", L"")
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTGrammar);
		OTGrammar_drawTableau (me, GRAPHICS, true, GET_STRING (L"Input string"));
	}
END

DIRECT (OTGrammar_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot edit from batch.");
	LOOP {
		iam (OTGrammar);
		autoOTGrammarEditor editor = OTGrammarEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (OTGrammar_evaluate, L"OTGrammar: Evaluate", 0)
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_newDisharmonies (me, GET_REAL (L"Evaluation noise"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_generateInputs, L"Generate inputs", L"OTGrammar: Generate inputs...")
	NATURAL (L"Number of trials", L"1000")
	OK
DO
	LOOP {
		iam (OTGrammar);
		autoStrings thee = OTGrammar_generateInputs (me, GET_INTEGER (L"Number of trials"));
		praat_new (thee.transfer(), my name, L"_in");
	}
END

FORM (OTGrammar_getCandidate, L"Get candidate", 0)
	NATURAL (L"Tableau number", L"1")
	NATURAL (L"Candidate number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	OTGrammarTableau tableau;
	long itab = GET_INTEGER (L"Tableau number"), icand = GET_INTEGER (L"Candidate number");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	tableau = & my tableaus [itab];
	if (icand > tableau -> numberOfCandidates)
		Melder_throw ("The specified candidate should not exceed the number of candidates.");
	Melder_information (tableau -> candidates [icand]. output);
END

FORM (OTGrammar_getConstraint, L"Get constraint name", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. name);
END

FORM (OTGrammar_getDisharmony, L"Get disharmony", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_double (my constraints [icons]. disharmony));
END

FORM (OTGrammar_getInput, L"Get input", 0)
	NATURAL (L"Tableau number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	Melder_information (my tableaus [itab]. input);
END

DIRECT (OTGrammar_getInputs)
	LOOP {
		iam (OTGrammar);
		autoStrings thee = OTGrammar_getInputs (me);
		praat_new (thee.transfer(), my name, L"_in");
	}
END

FORM (OTGrammar_getInterpretiveParse, L"OTGrammar: Interpretive parse", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	iam_ONLY (OTGrammar);
	long bestInput, bestOutput;
	OTGrammar_getInterpretiveParse (me, GET_STRING (L"Partial output"), & bestInput, & bestOutput);
	Melder_information (L"Best input = ", Melder_integer (bestInput), L": ", my tableaus [bestInput]. input,
		L"\nBest output = ", Melder_integer (bestOutput), L": ", my tableaus [bestInput]. candidates [bestOutput]. output);
END

FORM (OTGrammar_getNumberOfCandidates, L"Get number of candidates", 0)
	NATURAL (L"Tableau number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	Melder_information (Melder_integer (my tableaus [itab]. numberOfCandidates));
END

DIRECT (OTGrammar_getNumberOfConstraints)
	iam_ONLY (OTGrammar);
	Melder_information (Melder_integer (my numberOfConstraints));
END

FORM (OTGrammar_getNumberOfOptimalCandidates, L"Get number of optimal candidates", 0)
	NATURAL (L"Tableau number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau number");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	Melder_information (Melder_integer (OTGrammar_getNumberOfOptimalCandidates (me, itab)));
END

DIRECT (OTGrammar_getNumberOfTableaus)
	iam_ONLY (OTGrammar);
	Melder_information (Melder_integer (my numberOfTableaus));
END

FORM (OTGrammar_getNumberOfViolations, L"Get number of violations", 0)
	NATURAL (L"Tableau number", L"1")
	NATURAL (L"Candidate number", L"1")
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau number"), icand = GET_INTEGER (L"Candidate number"), icons = GET_INTEGER (L"Constraint number");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw ("The specified candidate should not exceed the number of candidates.");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_integer (my tableaus [itab]. candidates [icand]. marks [icons]));
END

FORM (OTGrammar_compareCandidates, L"Compare candidates", 0)
	NATURAL (L"Tableau number 1", L"1")
	NATURAL (L"Candidate number 1", L"1")
	NATURAL (L"Tableau number 2", L"1")
	NATURAL (L"Candidate number 2", L"2")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab1 = GET_INTEGER (L"Tableau number 1"), icand1 = GET_INTEGER (L"Candidate number 1");
	long itab2 = GET_INTEGER (L"Tableau number 2"), icand2 = GET_INTEGER (L"Candidate number 2");
	if (itab1 > my numberOfTableaus)
		Melder_throw ("The specified tableau (number 1) should not exceed the number of tableaus.");
	if (itab2 > my numberOfTableaus)
		Melder_throw ("The specified tableau (number 2) should not exceed the number of tableaus.");
	if (icand1 > my tableaus [itab1]. numberOfCandidates)
		Melder_throw ("The specified candidate (number 1) should not exceed the number of candidates for this tableau.");
	if (icand2 > my tableaus [itab1]. numberOfCandidates)
		Melder_throw ("The specified candidate (number 2) should not exceed the number of candidates for this tableau.");
	Melder_information (Melder_integer (OTGrammar_compareCandidates (me, itab1, icand1, itab2, icand2)));
END

FORM (OTGrammar_getRankingValue, L"Get ranking value", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_double (my constraints [icons]. ranking));
END

FORM (OTGrammar_getWinner, L"Get winner", 0)
	NATURAL (L"Tableau", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	Melder_information (Melder_integer (OTGrammar_getWinner (me, itab)));
END

FORM (OTGrammar_inputToOutput, L"OTGrammar: Input to output", L"OTGrammar: Input to output...")
	SENTENCE (L"Input form", L"")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTGrammar);
	wchar_t output [100];
	OTGrammar_inputToOutput (me, GET_STRING (L"Input form"), output, GET_REAL (L"Evaluation noise"));
	Melder_information (output);
	praat_dataChanged (me);
END

FORM (OTGrammar_inputToOutputs, L"OTGrammar: Input to outputs", L"OTGrammar: Input to outputs...")
	NATURAL (L"Trials", L"1000")
	REAL (L"Evaluation noise", L"2.0")
	SENTENCE (L"Input form", L"")
	OK
DO
	iam_ONLY (OTGrammar);
	autoStrings thee = OTGrammar_inputToOutputs (me, GET_STRING (L"Input form"), GET_INTEGER (L"Trials"), GET_REAL (L"Evaluation noise"));
	praat_new (thee.transfer(), my name, L"_out");
	praat_dataChanged (me);
END

FORM (OTGrammar_inputsToOutputs, L"OTGrammar: Inputs to outputs", L"OTGrammar: Inputs to outputs...")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	autoStrings him = OTGrammar_inputsToOutputs (me, thee, GET_REAL (L"Evaluation noise"));
	praat_new (him.transfer(), my name, L"_out");
	praat_dataChanged (me);
END

FORM (OTGrammar_isCandidateGrammatical, L"Is candidate grammatical?", 0)
	NATURAL (L"Tableau", L"1")
	NATURAL (L"Candidate", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	long icand = GET_INTEGER (L"Candidate");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw ("The specified candidate should not exceed the number of candidates.");
	Melder_information (Melder_integer (OTGrammar_isCandidateGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isCandidateSinglyGrammatical, L"Is candidate singly grammatical?", 0)
	NATURAL (L"Tableau", L"1")
	NATURAL (L"Candidate", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	long itab = GET_INTEGER (L"Tableau");
	if (itab > my numberOfTableaus)
		Melder_throw ("The specified tableau number should not exceed the number of tableaus.");
	long icand = GET_INTEGER (L"Candidate");
	if (icand > my tableaus [itab]. numberOfCandidates)
		Melder_throw ("The specified candidate should not exceed the number of candidates.");
	Melder_information (Melder_integer (OTGrammar_isCandidateSinglyGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isPartialOutputGrammatical, L"Is partial output grammatical?", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	iam_ONLY (OTGrammar);
	Melder_information (Melder_integer (OTGrammar_isPartialOutputGrammatical (me, GET_STRING (L"Partial output"))));
END

FORM (OTGrammar_isPartialOutputSinglyGrammatical, L"Is partial output singly grammatical?", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	iam_ONLY (OTGrammar);
	Melder_information (Melder_integer (OTGrammar_isPartialOutputSinglyGrammatical (me, GET_STRING (L"Partial output"))));
END

FORM (OTGrammar_learn, L"OTGrammar: Learn", L"OTGrammar & 2 Strings: Learn...")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	Strings inputs = NULL, outputs = NULL;
	WHERE (SELECTED && CLASS == classStrings) { if (! inputs) inputs = (Strings) OBJECT; else outputs = (Strings) OBJECT; }
	try {
		OTGrammar_learn (me, inputs, outputs,
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // partial change
		throw;
	}
END

FORM (OTGrammar_learnFromPartialOutputs, L"OTGrammar: Learn from partial adult outputs", 0)
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	INTEGER (L"Store history every", L"0")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Strings);
	OTHistory history = NULL;
	try {
		OTGrammar_learnFromPartialOutputs (me, thee,
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"),
			GET_INTEGER (L"Store history every"), & history);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // e.g. in case of partial learning
		Melder_flushError (NULL);
		// trickle down to save history
	}
	if (history) praat_new (history, my name);
END

FORM (OTGrammar_learnOne, L"OTGrammar: Learn one", L"OTGrammar: Learn one...")
	SENTENCE (L"Input string", L"")
	SENTENCE (L"Output string", L"")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	OK
DO
	LOOP try {
		iam (OTGrammar);
		OTGrammar_learnOne (me, GET_STRING (L"Input string"), GET_STRING (L"Output string"),
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), TRUE, TRUE, NULL);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END

FORM (OTGrammar_learnOneFromPartialOutput, L"OTGrammar: Learn one from partial adult output", 0)
	LABEL (L"", L"Partial adult surface form (e.g. overt form):")
	SENTENCE (L"Partial output", L"")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	LOOP try {
		iam (OTGrammar);
		OTGrammar_learnOneFromPartialOutput (me, GET_STRING (L"Partial output"),
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"), TRUE);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END

FORM (OTGrammar_removeConstraint, L"OTGrammar: Remove constraint", 0)
	SENTENCE (L"Constraint name", L"")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_removeConstraint (me, GET_STRING (L"Constraint name"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_removeHarmonicallyBoundedCandidates, L"OTGrammar: Remove harmonically bounded candidates", 0)
	BOOLEAN (L"Singly", 0)
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_removeHarmonicallyBoundedCandidates (me, GET_INTEGER (L"Singly"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_resetAllRankings, L"OTGrammar: Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_reset (me, GET_REAL (L"Ranking"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_resetToRandomTotalRanking, L"OTGrammar: Reset to random total ranking", 0)
	REAL (L"Maximum ranking", L"100.0")
	POSITIVE (L"Ranking distance", L"1.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_resetToRandomTotalRanking (me, GET_REAL (L"Maximum ranking"), GET_REAL (L"Ranking distance"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_setConstraintPlasticity, L"OTGrammar: Set constraint plasticity", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Plasticity", L"1.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_setConstraintPlasticity (me, GET_INTEGER (L"Constraint"), GET_REAL (L"Plasticity"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_setDecisionStrategy, L"OTGrammar: Set decision strategy", 0)
	RADIO_ENUM (L"Decision strategy", kOTGrammar_decisionStrategy, DEFAULT)
	OK
iam_ONLY (OTGrammar);
SET_ENUM (L"Decision strategy", kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	iam_ONLY (OTGrammar);
	my decisionStrategy = GET_ENUM (kOTGrammar_decisionStrategy, L"Decision strategy");
	praat_dataChanged (me);
END

FORM (OTGrammar_setLeak, L"OTGrammar: Set leak", 0)
	REAL (L"Leak", L"0.0")
	OK
iam_ONLY (OTGrammar);
SET_REAL (L"Leak", my leak);
DO
	iam_ONLY (OTGrammar);
	my leak = GET_REAL (L"Leak");
	praat_dataChanged (me);
END

FORM (OTGrammar_setRanking, L"OTGrammar: Set ranking", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Ranking", L"100.0")
	REAL (L"Disharmony", L"100.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		OTGrammar_setRanking (me, GET_INTEGER (L"Constraint"), GET_REAL (L"Ranking"), GET_REAL (L"Disharmony"));
		praat_dataChanged (me);
	}
END

FORM (OTGrammar_Distributions_getFractionCorrect, L"OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL (L"Column number", L"1")
	REAL (L"Evaluation noise", L"2.0")
	INTEGER (L"Replications", L"100000")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	double result = OTGrammar_Distributions_getFractionCorrect (me, thee, GET_INTEGER (L"Column number"),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Replications"));
	praat_dataChanged (me);
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_Distributions_learnFromPartialOutputs, L"OTGrammar & Distributions: Learn from partial outputs", L"OT learning 6. Shortcut to grammar learning")
	NATURAL (L"Column number", L"1")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	REAL (L"Initial plasticity", L"1.0")
	NATURAL (L"Replications per plasticity", L"100000")
	REAL (L"Plasticity decrement", L"0.1")
	NATURAL (L"Number of plasticities", L"4")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	INTEGER (L"Store history every", L"0")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	OTHistory history = NULL;
	try {
		OTGrammar_Distributions_learnFromPartialOutputs (me, thee, GET_INTEGER (L"Column number"),
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
			GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
			GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"),
			GET_INTEGER (L"Store history every"), & history);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		Melder_flushError (NULL);
	}
	if (history) praat_new (history, my name);
END

FORM (OTGrammar_Distributions_listObligatoryRankings, L"OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (Distributions);
	OTGrammar_Distributions_listObligatoryRankings (me, thee, GET_INTEGER (L"Column number"));
END

FORM (OTGrammar_PairDistribution_findPositiveWeights, L"OTGrammar & PairDistribution: Find positive weights", L"OTGrammar & PairDistribution: Find positive weights...")
	POSITIVE (L"Weight floor", L"1.0")
	POSITIVE (L"Margin of separation", L"1.0")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	OTGrammar_PairDistribution_findPositiveWeights_e (me, thee,
		GET_REAL (L"Weight floor"), GET_REAL (L"Margin of separation"));
	praat_dataChanged (me);
END

FORM (OTGrammar_PairDistribution_getFractionCorrect, L"OTGrammar & PairDistribution: Get fraction correct...", 0)
	REAL (L"Evaluation noise", L"2.0")
	INTEGER (L"Replications", L"100000")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	double result;
	try {
		result = OTGrammar_PairDistribution_getFractionCorrect (me, thee,
			GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Replications"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_PairDistribution_getMinimumNumberCorrect, L"OTGrammar & PairDistribution: Get minimum number correct...", 0)
	REAL (L"Evaluation noise", L"2.0")
	INTEGER (L"Replications per input", L"1000")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	long result;
	try {
		result = OTGrammar_PairDistribution_getMinimumNumberCorrect (me, thee,
			GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Replications per input"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
	Melder_information (Melder_integer (result));
END

FORM (OTGrammar_PairDistribution_learn, L"OTGrammar & PairDistribution: Learn", L"OT learning 6. Shortcut to grammar learning")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	POSITIVE (L"Initial plasticity", L"1.0")
	NATURAL (L"Replications per plasticity", L"100000")
	REAL (L"Plasticity decrement", L"0.1")
	NATURAL (L"Number of plasticities", L"4")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	try {
		OTGrammar_PairDistribution_learn (me, thee,
			GET_REAL (L"Evaluation noise"), GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"), GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
			GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
			GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"));
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);
		throw;
	}
END

DIRECT (OTGrammar_PairDistribution_listObligatoryRankings)
	iam_ONLY (OTGrammar);
	thouart_ONLY (PairDistribution);
	OTGrammar_PairDistribution_listObligatoryRankings (me, thee);
END

FORM (OTGrammar_to_Distributions, L"OTGrammar: Compute output distributions", L"OTGrammar: To output Distributions...")
	NATURAL (L"Trials per input", L"100000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		try {
			autoDistributions thee = OTGrammar_to_Distribution (me, GET_INTEGER (L"Trials per input"), GET_REAL (L"Evaluation noise"));
			praat_new (thee.transfer(), my name, L"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (OTGrammar_to_PairDistribution, L"OTGrammar: Compute output distributions", 0)
	NATURAL (L"Trials per input", L"100000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	LOOP try {
		iam (OTGrammar);
		autoPairDistribution thee = OTGrammar_to_PairDistribution (me, GET_INTEGER (L"Trials per input"), GET_REAL (L"Evaluation noise"));
		praat_new (thee.transfer(), my name, L"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END

DIRECT (OTGrammar_measureTypology)
	LOOP try {
		iam (OTGrammar);
		autoDistributions thee = OTGrammar_measureTypology (me);
		praat_new (thee.transfer(), my name, L"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END

FORM_WRITE (OTGrammar_writeToHeaderlessSpreadsheetFile, L"Write OTGrammar to spreadsheet", 0, L"txt")
	iam_ONLY (OTGrammar);
	OTGrammar_writeToHeaderlessSpreadsheetFile (me, file);
END

FORM (OTMulti_drawTableau, L"Draw tableau", L"OT learning")
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	BOOLEAN (L"Show disharmonies", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTMulti);
		OTMulti_drawTableau (me, GRAPHICS, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
			false, GET_INTEGER (L"Show disharmonies"));
	}
END

FORM (OTMulti_drawTableau_narrowly, L"Draw tableau (narrowly)", L"OT learning")
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	BOOLEAN (L"Show disharmonies", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (OTMulti);
		OTMulti_drawTableau (me, GRAPHICS, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
			true, GET_INTEGER (L"Show disharmonies"));
	}
END

DIRECT (OTMulti_edit)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot edit an OTMulti from batch.");
	LOOP {
		iam (OTMulti);
		autoOTMultiEditor editor = OTMultiEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.transfer(), IOBJECT);
	}
END

FORM (OTMulti_evaluate, L"OTMulti: Evaluate", 0)
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTMulti);
	OTMulti_newDisharmonies (me, GET_REAL (L"Evaluation noise"));
	praat_dataChanged (me);
END

FORM (OTMulti_generateOptimalForms, L"OTMulti: Generate optimal forms", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	NATURAL (L"Number of trials", L"1000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTMulti);
	autoStrings thee = OTMulti_generateOptimalForms (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
		GET_INTEGER (L"Number of trials"), GET_REAL (L"Evaluation noise"));
	praat_new (thee.transfer(), my name, L"_out");
	praat_dataChanged (me);
END

FORM (OTMulti_getCandidate, L"Get candidate", 0)
	NATURAL (L"Candidate", L"1")
	OK
DO
	iam_ONLY (OTMulti);
	long icand = GET_INTEGER (L"Candidate");
	if (icand > my numberOfCandidates)
		Melder_throw ("The specified candidate number should not exceed the number of candidates.");
	Melder_information (my candidates [icand]. string);
END

FORM (OTMulti_getConstraint, L"Get constraint name", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (my constraints [icons]. name);
END

FORM (OTMulti_getConstraintIndexFromName, L"OTMulti: Get constraint number", 0)
	SENTENCE (L"Constraint name", L"")
	OK
DO
	iam_ONLY (OTMulti);
	Melder_information (Melder_integer (OTMulti_getConstraintIndexFromName (me, GET_STRING (L"Constraint name"))));
END

FORM (OTMulti_getDisharmony, L"Get disharmony", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_double (my constraints [icons]. disharmony));
END

DIRECT (OTMulti_getNumberOfCandidates)
	iam_ONLY (OTMulti);
	Melder_information (Melder_integer (my numberOfCandidates));
END

DIRECT (OTMulti_getNumberOfConstraints)
	iam_ONLY (OTMulti);
	Melder_information (Melder_integer (my numberOfConstraints));
END

FORM (OTMulti_getNumberOfViolations, L"Get number of violations", 0)
	NATURAL (L"Candidate number", L"1")
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTMulti);
	long icand = GET_INTEGER (L"Candidate number");
	if (icand > my numberOfCandidates)
		Melder_throw ("The specified candidate number should not exceed the number of candidates.");
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_integer (my candidates [icand]. marks [icons]));
END

FORM (OTMulti_getRankingValue, L"Get ranking value", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	iam_ONLY (OTMulti);
	long icons = GET_INTEGER (L"Constraint number");
	if (icons > my numberOfConstraints)
		Melder_throw ("The specified constraint number should not exceed the number of constraints.");
	Melder_information (Melder_double (my constraints [icons]. ranking));
END

FORM (OTMulti_getWinner, L"OTMulti: Get winner", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	OK
DO
	iam_ONLY (OTMulti);
	Melder_information (Melder_integer (OTMulti_getWinner (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"))));
END

FORM (OTMulti_generateOptimalForm, L"OTMulti: Generate optimal form", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTMulti);
	wchar_t output [100];
	OTMulti_generateOptimalForm (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
		output, GET_REAL (L"Evaluation noise"));
	Melder_information (output);
	praat_dataChanged (me);
END

FORM (OTMulti_learnOne, L"OTMulti: Learn one", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	OPTIONMENU (L"Direction", 3)
		OPTION (L"forward")
		OPTION (L"backward")
		OPTION (L"bidirectionally")
	POSITIVE (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	OK
DO
	LOOP {
		iam (OTMulti);
		try {
			OTMulti_learnOne (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
				GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
				GET_INTEGER (L"Direction"), GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"));
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (OTMulti_removeConstraint, L"OTMulti: Remove constraint", 0)
	SENTENCE (L"Constraint name", L"")
	OK
DO
	LOOP {
		iam (OTMulti);
		OTMulti_removeConstraint (me, GET_STRING (L"Constraint name"));
		praat_dataChanged (me);
	}
END

FORM (OTMulti_resetAllRankings, L"OTMulti: Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	LOOP {
		iam (OTMulti);
		OTMulti_reset (me, GET_REAL (L"Ranking"));
		praat_dataChanged (me);
	}
END

FORM (OTMulti_setConstraintPlasticity, L"OTMulti: Set constraint plasticity", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Plasticity", L"1.0")
	OK
DO
	LOOP {
		iam (OTMulti);
		OTMulti_setConstraintPlasticity (me, GET_INTEGER (L"Constraint"), GET_REAL (L"Plasticity"));
		praat_dataChanged (me);
	}
END

FORM (OTMulti_setDecisionStrategy, L"OTMulti: Set decision strategy", 0)
	RADIO_ENUM (L"Decision strategy", kOTGrammar_decisionStrategy, DEFAULT)
	OK
iam_ONLY (OTMulti);
SET_ENUM (L"Decision strategy", kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	iam_ONLY (OTMulti);
	my decisionStrategy = GET_ENUM (kOTGrammar_decisionStrategy, L"Decision strategy");
	praat_dataChanged (me);
END

FORM (OTMulti_setLeak, L"OTGrammar: Set leak", 0)
	REAL (L"Leak", L"0.0")
	OK
iam_ONLY (OTMulti);
SET_REAL (L"Leak", my leak);
DO
	iam_ONLY (OTMulti);
	my leak = GET_REAL (L"Leak");
	praat_dataChanged (me);
END

FORM (OTMulti_setRanking, L"OTMulti: Set ranking", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Ranking", L"100.0")
	REAL (L"Disharmony", L"100.0")
	OK
DO
	LOOP {
		iam (OTMulti);
		OTMulti_setRanking (me, GET_INTEGER (L"Constraint"), GET_REAL (L"Ranking"), GET_REAL (L"Disharmony"));
		praat_dataChanged (me);
	}
END

FORM (OTMulti_to_Distribution, L"OTMulti: Compute output distribution", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	NATURAL (L"Number of trials", L"100000")
	POSITIVE (L"Evaluation noise", L"2.0")
	OK
DO
	LOOP {
		iam (OTMulti);
		try {
			autoDistributions thee = OTMulti_to_Distribution (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
			GET_INTEGER (L"Number of trials"), GET_REAL (L"Evaluation noise"));
			praat_new (thee.transfer(), my name, L"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END

FORM (OTMulti_PairDistribution_learn, L"OTMulti & PairDistribution: Learn", 0)
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU_ENUM (L"Update rule", kOTGrammar_rerankingStrategy, SYMMETRIC_ALL)
	OPTIONMENU (L"Direction", 3)
		OPTION (L"forward")
		OPTION (L"backward")
		OPTION (L"bidirectionally")
	POSITIVE (L"Initial plasticity", L"1.0")
	NATURAL (L"Replications per plasticity", L"100000")
	REAL (L"Plasticity decrement", L"0.1")
	NATURAL (L"Number of plasticities", L"4")
	REAL (L"Rel. plasticity spreading", L"0.1")
	INTEGER (L"Store history every", L"0")
	OK
DO
	iam_ONLY (OTMulti);
	thouart_ONLY (PairDistribution);
	Table history = NULL;
	try {
		OTMulti_PairDistribution_learn (me, thee,
			GET_REAL (L"Evaluation noise"),
			GET_ENUM (kOTGrammar_rerankingStrategy, L"Update rule"),
			GET_INTEGER (L"Direction"),
			GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
			GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
			GET_REAL (L"Rel. plasticity spreading"),
			GET_INTEGER (L"Store history every"), & history);
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (me);   // e.g. in case of partial learning
		Melder_flushError (NULL);
		// trickle down to save history
	}
	if (history) praat_new (history, my name);
END

FORM (OTMulti_Strings_generateOptimalForms, L"OTGrammar: Inputs to outputs", L"OTGrammar: Inputs to outputs...")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	iam_ONLY (OTMulti);
	thouart_ONLY (Strings);
	autoStrings him = OTMulti_Strings_generateOptimalForms (me, thee, GET_REAL (L"Evaluation noise"));
	praat_new (him.transfer(), my name, L"_out");
	praat_dataChanged (me);
END

/***** buttons *****/

void praat_uvafon_gram_init (void);
void praat_uvafon_gram_init (void) {
	Thing_recognizeClassesByName (classNetwork, classOTGrammar, classOTHistory, classOTMulti, NULL);
	Thing_recognizeClassByOtherName (classOTGrammar, L"OTCase");

	praat_addMenuCommand (L"Objects", L"New", L"Constraint grammars", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"OT learning tutorial", 0, 1, DO_OT_learning_tutorial);
		praat_addMenuCommand (L"Objects", L"New", L"-- tableau grammars --", 0, 1, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create NoCoda grammar", 0, 1, DO_Create_NoCoda_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create place assimilation grammar", 0, 1, DO_Create_NPA_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create place assimilation distribution", 0, 1, DO_Create_NPA_distribution);
		praat_addMenuCommand (L"Objects", L"New", L"Create tongue-root grammar...", 0, 1, DO_Create_tongue_root_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create metrics grammar...", 0, 1, DO_Create_metrics_grammar);

	praat_addAction1 (classOTGrammar, 0, L"OTGrammar help", 0, 0, DO_OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, L"View & Edit", 0, praat_ATTRACTIVE, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, L"Edit", 0, praat_HIDDEN, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, L"Draw tableau...", 0, 0, DO_OTGrammar_drawTableau);
	praat_addAction1 (classOTGrammar, 0, L"Draw tableau (narrowly)...", 0, 0, DO_OTGrammar_drawTableau_narrowly);
	praat_addAction1 (classOTGrammar, 1, L"Save as headerless spreadsheet file...", 0, 0, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 1, L"Write to headerless spreadsheet file...", 0, praat_HIDDEN, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 0, L"Query -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 1, L"Get number of constraints", 0, 1, DO_OTGrammar_getNumberOfConstraints);
	praat_addAction1 (classOTGrammar, 1, L"Get constraint...", 0, 1, DO_OTGrammar_getConstraint);
	praat_addAction1 (classOTGrammar, 1, L"Get ranking value...", 0, 1, DO_OTGrammar_getRankingValue);
	praat_addAction1 (classOTGrammar, 1, L"Get disharmony...", 0, 1, DO_OTGrammar_getDisharmony);
	praat_addAction1 (classOTGrammar, 1, L"Get number of tableaus", 0, 1, DO_OTGrammar_getNumberOfTableaus);
	praat_addAction1 (classOTGrammar, 1, L"Get input...", 0, 1, DO_OTGrammar_getInput);
	praat_addAction1 (classOTGrammar, 1, L"Get number of candidates...", 0, 1, DO_OTGrammar_getNumberOfCandidates);
	praat_addAction1 (classOTGrammar, 1, L"Get candidate...", 0, 1, DO_OTGrammar_getCandidate);
	praat_addAction1 (classOTGrammar, 1, L"Get number of violations...", 0, 1, DO_OTGrammar_getNumberOfViolations);
	praat_addAction1 (classOTGrammar, 1, L"-- parse --", 0, 1, 0);
	praat_addAction1 (classOTGrammar, 1, L"Get winner...", 0, 1, DO_OTGrammar_getWinner);
	praat_addAction1 (classOTGrammar, 1, L"Compare candidates...", 0, 1, DO_OTGrammar_compareCandidates);
	praat_addAction1 (classOTGrammar, 1, L"Get number of optimal candidates...", 0, 1, DO_OTGrammar_getNumberOfOptimalCandidates);
	praat_addAction1 (classOTGrammar, 1, L"Is candidate grammatical...", 0, 1, DO_OTGrammar_isCandidateGrammatical);
	praat_addAction1 (classOTGrammar, 1, L"Is candidate singly grammatical...", 0, 1, DO_OTGrammar_isCandidateSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 1, L"Get interpretive parse...", 0, 1, DO_OTGrammar_getInterpretiveParse);
	praat_addAction1 (classOTGrammar, 1, L"Is partial output grammatical...", 0, 1, DO_OTGrammar_isPartialOutputGrammatical);
	praat_addAction1 (classOTGrammar, 1, L"Is partial output singly grammatical...", 0, 1, DO_OTGrammar_isPartialOutputSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 0, L"Generate inputs...", 0, 0, DO_OTGrammar_generateInputs);
	praat_addAction1 (classOTGrammar, 0, L"Get inputs", 0, 0, DO_OTGrammar_getInputs);
	praat_addAction1 (classOTGrammar, 0, L"Measure typology", 0, 0, DO_OTGrammar_measureTypology);
	praat_addAction1 (classOTGrammar, 0, L"Evaluate", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, L"Evaluate...", 0, 0, DO_OTGrammar_evaluate);
	praat_addAction1 (classOTGrammar, 0, L"Input to output...", 0, 0, DO_OTGrammar_inputToOutput);
	praat_addAction1 (classOTGrammar, 0, L"Input to outputs...", 0, 0, DO_OTGrammar_inputToOutputs);
	praat_addAction1 (classOTGrammar, 0, L"To output Distributions...", 0, 0, DO_OTGrammar_to_Distributions);
	praat_addAction1 (classOTGrammar, 0, L"To PairDistribution...", 0, 0, DO_OTGrammar_to_PairDistribution);
	praat_addAction1 (classOTGrammar, 0, L"Modify ranking -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, L"Set ranking...", 0, 1, DO_OTGrammar_setRanking);
	praat_addAction1 (classOTGrammar, 0, L"Reset all rankings...", 0, 1, DO_OTGrammar_resetAllRankings);
	praat_addAction1 (classOTGrammar, 0, L"Reset to random total ranking...", 0, 1, DO_OTGrammar_resetToRandomTotalRanking);
	praat_addAction1 (classOTGrammar, 0, L"Learn one...", 0, 1, DO_OTGrammar_learnOne);
	praat_addAction1 (classOTGrammar, 0, L"Learn one from partial output...", 0, 1, DO_OTGrammar_learnOneFromPartialOutput);
	praat_addAction1 (classOTGrammar, 0, L"Modify behaviour -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 1, L"Set harmony computation method...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_OTGrammar_setDecisionStrategy);
	praat_addAction1 (classOTGrammar, 1, L"Set decision strategy...", 0, 1, DO_OTGrammar_setDecisionStrategy);
	praat_addAction1 (classOTGrammar, 1, L"Set leak...", 0, 1, DO_OTGrammar_setLeak);
	praat_addAction1 (classOTGrammar, 1, L"Set constraint plasticity...", 0, 1, DO_OTGrammar_setConstraintPlasticity);
	praat_addAction1 (classOTGrammar, 0, L"Modify structure -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, L"Remove constraint...", 0, 1, DO_OTGrammar_removeConstraint);
	praat_addAction1 (classOTGrammar, 0, L"Remove harmonically bounded candidates...", 0, 1, DO_OTGrammar_removeHarmonicallyBoundedCandidates);

	{ void praat_TableOfReal_init (ClassInfo klas); praat_TableOfReal_init (classOTHistory); }

	praat_addAction1 (classOTMulti, 0, L"View & Edit", 0, praat_ATTRACTIVE, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, L"Edit", 0, praat_HIDDEN, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, L"Draw tableau...", 0, 0, DO_OTMulti_drawTableau);
	praat_addAction1 (classOTMulti, 0, L"Draw tableau (narrowly)...", 0, 0, DO_OTMulti_drawTableau_narrowly);
	praat_addAction1 (classOTMulti, 0, L"Query -", 0, 0, 0);
	praat_addAction1 (classOTMulti, 1, L"Get number of constraints", 0, 1, DO_OTMulti_getNumberOfConstraints);
	praat_addAction1 (classOTMulti, 1, L"Get constraint...", 0, 1, DO_OTMulti_getConstraint);
	praat_addAction1 (classOTMulti, 1, L"Get constraint number...", 0, 1, DO_OTMulti_getConstraintIndexFromName);
	praat_addAction1 (classOTMulti, 1, L"Get ranking value...", 0, 1, DO_OTMulti_getRankingValue);
	praat_addAction1 (classOTMulti, 1, L"Get disharmony...", 0, 1, DO_OTMulti_getDisharmony);
	praat_addAction1 (classOTMulti, 1, L"Get number of candidates", 0, 1, DO_OTMulti_getNumberOfCandidates);
	praat_addAction1 (classOTMulti, 1, L"Get candidate...", 0, 1, DO_OTMulti_getCandidate);
	praat_addAction1 (classOTMulti, 1, L"Get number of violations...", 0, 1, DO_OTMulti_getNumberOfViolations);
	praat_addAction1 (classOTMulti, 1, L"-- parse --", 0, 1, 0);
	praat_addAction1 (classOTMulti, 1, L"Get winner...", 0, 1, DO_OTMulti_getWinner);
	praat_addAction1 (classOTMulti, 0, L"Evaluate", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, L"Evaluate...", 0, 0, DO_OTMulti_evaluate);
	praat_addAction1 (classOTMulti, 0, L"Get output...", 0, 0, DO_OTMulti_generateOptimalForm);
	praat_addAction1 (classOTMulti, 0, L"Get outputs...", 0, 0, DO_OTMulti_generateOptimalForms);
	praat_addAction1 (classOTMulti, 0, L"To output Distribution...", 0, 0, DO_OTMulti_to_Distribution);
	praat_addAction1 (classOTMulti, 0, L"Modify ranking", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, L"Set ranking...", 0, 0, DO_OTMulti_setRanking);
	praat_addAction1 (classOTMulti, 0, L"Reset all rankings...", 0, 0, DO_OTMulti_resetAllRankings);
	praat_addAction1 (classOTMulti, 0, L"Learn one...", 0, 0, DO_OTMulti_learnOne);
	praat_addAction1 (classOTMulti, 0, L"Modify behaviour -", 0, 0, 0);
	praat_addAction1 (classOTMulti, 1, L"Set decision strategy...", 0, 1, DO_OTMulti_setDecisionStrategy);
	praat_addAction1 (classOTMulti, 1, L"Set leak...", 0, 1, DO_OTMulti_setLeak);
	praat_addAction1 (classOTMulti, 1, L"Set constraint plasticity...", 0, 1, DO_OTMulti_setConstraintPlasticity);
	praat_addAction1 (classOTMulti, 0, L"Modify structure -", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, L"Remove constraint...", 0, 1, DO_OTMulti_removeConstraint);

	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, L"Learn from partial outputs...", 0, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, L"Get fraction correct...", 0, 0, DO_OTGrammar_Distributions_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, L"List obligatory rankings...", 0, praat_HIDDEN, DO_OTGrammar_Distributions_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"Learn...", 0, 0, DO_OTGrammar_PairDistribution_learn);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"Find positive weights...", 0, 0, DO_OTGrammar_PairDistribution_findPositiveWeights);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"Get fraction correct...", 0, 0, DO_OTGrammar_PairDistribution_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"Get minimum number correct...", 0, 0, DO_OTGrammar_PairDistribution_getMinimumNumberCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"List obligatory rankings", 0, 0, DO_OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, L"Inputs to outputs...", 0, 0, DO_OTGrammar_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, L"Learn from partial outputs...", 0, 0, DO_OTGrammar_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, L"Learn...", 0, 0, DO_OTGrammar_learn);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, L"Learn...", 0, 0, DO_OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, L"Get outputs...", 0, 0, DO_OTMulti_Strings_generateOptimalForms);

	praat_addMenuCommand (L"Objects", L"New", L"Symmetric networks", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create empty Network...", 0, 1, DO_Create_empty_Network);
		praat_addMenuCommand (L"Objects", L"New", L"Create rectangular Network...", 0, 1, DO_Create_rectangular_Network);
		praat_addMenuCommand (L"Objects", L"New", L"Create rectangular Network (vertical)...", 0, 1, DO_Create_rectangular_Network_vertical);

	praat_addAction1 (classNetwork, 0, L"Draw...", 0, 0, DO_Network_draw);
	praat_addAction1 (classNetwork, 1, L"Tabulate -", 0, 0, 0);
		praat_addAction1 (classNetwork, 1, L"List nodes...", 0, 1, DO_Network_listNodes);
		praat_addAction1 (classNetwork, 1, L"Nodes down to table...", 0, 1, DO_Network_nodes_downto_Table);
	praat_addAction1 (classNetwork, 0, L"Query -", 0, 0, 0);
		praat_addAction1 (classNetwork, 1, L"Get activity...", 0, 1, DO_Network_getActivity);
		praat_addAction1 (classNetwork, 1, L"Get weight...", 0, 1, DO_Network_getWeight);
	praat_addAction1 (classNetwork, 0, L"Modify -", 0, 0, 0);
		praat_addAction1 (classNetwork, 0, L"Add node...", 0, 1, DO_Network_addNode);
		praat_addAction1 (classNetwork, 0, L"Add connection...", 0, 1, DO_Network_addConnection);
		praat_addAction1 (classNetwork, 1, L"-- activity --", 0, 1, 0);
		praat_addAction1 (classNetwork, 0, L"Set activity...", 0, 1, DO_Network_setActivity);
		praat_addAction1 (classNetwork, 0, L"Set clamping...", 0, 1, DO_Network_setClamping);
		praat_addAction1 (classNetwork, 0, L"Zero activities...", 0, 1, DO_Network_zeroActivities);
		praat_addAction1 (classNetwork, 0, L"Normalize activities...", 0, 1, DO_Network_normalizeActivities);
		praat_addAction1 (classNetwork, 0, L"Spread activities...", 0, 1, DO_Network_spreadActivities);
		praat_addAction1 (classNetwork, 1, L"Set activity clipping rule...", 0, 1, DO_Network_setActivityClippingRule);
		praat_addAction1 (classNetwork, 1, L"Set activity leak...", 0, 1, DO_Network_setActivityLeak);
		praat_addAction1 (classNetwork, 1, L"Set shunting...", 0, 1, DO_Network_setShunting);
		praat_addAction1 (classNetwork, 1, L"-- weight --", 0, 1, 0);
		praat_addAction1 (classNetwork, 0, L"Set weight...", 0, 1, DO_Network_setWeight);
		praat_addAction1 (classNetwork, 0, L"Update weights", 0, 1, DO_Network_updateWeights);
		praat_addAction1 (classNetwork, 0, L"Normalize weights...", 0, 1, DO_Network_normalizeWeights);
		praat_addAction1 (classNetwork, 1, L"Set instar...", 0, 1, DO_Network_setInstar);
		praat_addAction1 (classNetwork, 1, L"Set outstar...", 0, 1, DO_Network_setOutstar);
		praat_addAction1 (classNetwork, 1, L"Set weight leak...", 0, 1, DO_Network_setWeightLeak);
}

/* End of file praat_gram.cpp */
