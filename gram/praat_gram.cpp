/* praat_gram.cpp
 *
 * Copyright (C) 1997-2020 Paul Boersma
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

#include "Network.h"
#include "OTGrammar.h"
#include "OTMulti.h"
#include "OTGrammarEditor.h"
#include "OTMultiEditor.h"
#include "Net.h"
#include "NoulliGridEditor.h"

#include "praat_TableOfReal.h"
#include "praat_TimeFunction.h"

#undef iam
#define iam iam_LOOP

// MARK: - NETWORK

// MARK: New

#define UiForm_addNetworkFields  \
	LABEL (U"Activity spreading settings:") \
	REAL (spreadingRate, U"Spreading rate", U"0.01") \
	OPTIONMENU_ENUM (kNetwork_activityClippingRule, activityClippingRule, \
			U"Activity clipping rule", kNetwork_activityClippingRule::DEFAULT) \
	REAL (minimumActivity, U"left Activity range", U"0.0") \
	REAL (maximumActivity, U"right Activity range", U"1.0") \
	REAL (activityLeak, U"Activity leak", U"1.0") \
	LABEL (U"Weight update settings:") \
	REAL (learningRate, U"Learning rate", U"0.1") \
	REAL (minimumWeight, U"left Weight range", U"-1.0") \
	REAL (maximumWeight, U"right Weight range", U"1.0") \
	REAL (weightLeak, U"Weight leak", U"0.0")

FORM (NEW1_Create_empty_Network, U"Create empty Network", nullptr) {
	WORD (name, U"Name", U"network")
	UiForm_addNetworkFields
	LABEL (U"World coordinates:")
	REAL (fromX, U"left x range", U"0.0")
	REAL (toX, U"right x range", U"10.0")
	REAL (fromY, U"left y range", U"0.0")
	REAL (toY, U"right y range", U"10.0")
	OK
DO
	CREATE_ONE
		autoNetwork result = Network_create (spreadingRate, activityClippingRule,
			minimumActivity, maximumActivity, activityLeak, learningRate, minimumWeight, maximumWeight, weightLeak,
			fromX, toX, fromY, toY, 0, 0
		);
	CREATE_ONE_END (name)
}

FORM (NEW1_Create_rectangular_Network, U"Create rectangular Network", nullptr) {
	UiForm_addNetworkFields
	LABEL (U"Structure settings:")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	BOOLEAN (bottomRowClamped, U"Bottom row clamped", 1)
	LABEL (U"Initial state settings:")
	REAL (minimumInitialWeight, U"left Initial weight range", U"-0.1")
	REAL (maximumInitialWeight, U"right Initial weight range", U"0.1")
	OK
DO
	CREATE_ONE
		autoNetwork result = Network_create_rectangle (spreadingRate, activityClippingRule,
			minimumActivity, maximumActivity, activityLeak, learningRate, minimumWeight, maximumWeight, weightLeak,
			numberOfRows, numberOfColumns, bottomRowClamped, minimumInitialWeight, maximumInitialWeight
		);
	CREATE_ONE_END (U"rectangle_", numberOfRows, U"_", numberOfColumns)
}

FORM (NEW1_Create_rectangular_Network_vertical, U"Create rectangular Network (vertical)", nullptr) {
	UiForm_addNetworkFields
	LABEL (U"Structure settings:")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	BOOLEAN (bottomRowClamped, U"Bottom row clamped", 1)
	LABEL (U"Initial state settings:")
	REAL (minimumInitialWeight, U"left Initial weight range", U"-0.1")
	REAL (maximumInitialWeight, U"right Initial weight range", U"0.1")
	OK
DO
	CREATE_ONE
		autoNetwork result = Network_create_rectangle_vertical (spreadingRate,
			(kNetwork_activityClippingRule) activityClippingRule,
			minimumActivity, maximumActivity, activityLeak, learningRate, minimumWeight, maximumWeight, weightLeak,
			numberOfRows, numberOfColumns, bottomRowClamped, minimumInitialWeight, maximumInitialWeight
		);
	CREATE_ONE_END (U"rectangle_", numberOfRows, U"_", numberOfColumns)
}

// MARK: Draw

FORM (GRAPHICS_Network_draw, U"Draw Network", nullptr) {
	BOOLEAN (useColour, U"Use colour", true)
	OK
DO
	GRAPHICS_EACH (Network)
		Network_draw (me, GRAPHICS, useColour);
	GRAPHICS_EACH_END
}

// MARK: Tabulate

FORM (LIST_Network_listNodes, U"Network: List nodes", nullptr) {
	INTEGER (fromNodeNumber, U"From node number", U"1")
	INTEGER (toNodeNumber, U"To node number", U"1000")
	BOOLEAN (includeNodeNumbers, U"Include node numbers", true)
	BOOLEAN (includeX, U"Include x", false)
	BOOLEAN (includeY, U"Include y", false)
	INTEGER (positionDecimals, U"Position decimals", U"6")
	BOOLEAN (includeClamped, U"Include clamped", false)
	BOOLEAN (includeActivity, U"Include activity", true)
	BOOLEAN (includeExcitation, U"Include excitation", false)
	INTEGER (activityDecimals, U"Activity decimals", U"6")
	OK
DO
	INFO_ONE (Network)
		Network_listNodes (me, fromNodeNumber, toNodeNumber,
			includeNodeNumbers, includeX, includeY, positionDecimals,
			includeClamped, includeActivity, includeExcitation, activityDecimals
		);
	INFO_ONE_END
}

FORM (NEW_Network_nodes_downto_Table, U"Network: Nodes down to Table", nullptr) {
	INTEGER (fromNodeNumber, U"From node number", U"1")
	INTEGER (toNodeNumber, U"To node number", U"1000")
	BOOLEAN (includeNodeNumbers, U"Include node numbers", true)
	BOOLEAN (includeX, U"Include x", false)
	BOOLEAN (includeY, U"Include y", false)
	INTEGER (positionDecimals, U"Position decimals", U"6")
	BOOLEAN (includeClamped, U"Include clamped", false)
	BOOLEAN (includeActivity, U"Include activity", true)
	BOOLEAN (includeExcitation, U"Include excitation", false)
	INTEGER (activityDecimals, U"Activity decimals", U"6")
	OK
DO
	CONVERT_EACH (Network)
		autoTable result = Network_nodes_downto_Table (me, fromNodeNumber, toNodeNumber,
			includeNodeNumbers, includeX, includeY, positionDecimals,
			includeClamped, includeActivity, includeExcitation, activityDecimals
		);
	CONVERT_EACH_END (my name.get())
}

// MARK: Query

FORM (REAL_Network_getActivity, U"Network: Get activity", nullptr) {
	NATURAL (node, U"Node", U"1")
	OK
DO
	NUMBER_ONE (Network)
		const double result = Network_getActivity (me, node);
	NUMBER_ONE_END (U" (activity of node ", node, U")")
}

FORM (NUMVEC_Network_getActivities, U"Network: Get activities", nullptr) {
	NATURAL (fromNode, U"From node", U"1")
	NATURAL (toNode, U"To node", U"0 (= all)")
	OK
DO
	NUMVEC_ONE (Network)
		autoVEC result = Network_getActivities (me, fromNode, toNode);
	NUMVEC_ONE_END
}

FORM (REAL_Network_getWeight, U"Network: Get weight", nullptr) {
	NATURAL (connection, U"Connection", U"1")
	OK
DO
	NUMBER_ONE (Network)
		const double result = Network_getWeight (me, connection);
	NUMBER_ONE_END (U" (weight of connection ", connection, U")")
}

// MARK: Modify

FORM (MODIFY_Network_addConnection, U"Network: Add connection", nullptr) {
	NATURAL (fromNode, U"From node", U"1")
	NATURAL (toNode, U"To node", U"2")
	REAL (weight, U"Weight", U"0.0")
	REAL (plasticity, U"Plasticity", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_addConnection (me, fromNode, toNode, weight, plasticity);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_addNode, U"Network: Add node", nullptr) {
	REAL (x, U"x", U"5.0")
	REAL (y, U"y", U"5.0")
	REAL (activity, U"Activity", U"0.0")
	BOOLEAN (clamping, U"Clamping", false)
	OK
DO
	MODIFY_EACH (Network)
		Network_addNode (me, x, y, activity, clamping);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_normalizeActivities, U"Network: Normalize activities", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	OK
DO
	MODIFY_EACH (Network)
		Network_normalizeActivities (me, fromNode, toNode);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_normalizeWeights, U"Network: Normalize weights", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	INTEGER (fromIncomingNode, U"From incoming node", U"1")
	INTEGER (toIncomingNode, U"To incoming node", U"10")
	REAL (newSum, U"New sum", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_normalizeWeights (me, fromNode, toNode, fromIncomingNode, toIncomingNode, newSum);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setActivity, U"Network: Set activity", nullptr) {
	NATURAL (node, U"Node", U"1")
	REAL (activity, U"Activity", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivity (me, node, activity);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_formula_activities, U"Network: Formula (activities)", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	LABEL (U"`col` is the node number, `self` is the current activity")
	LABEL (U"for col := 1 to ncol do { self [col] := `formula' }")
	TEXTFIELD (formula, U"Formula:", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Network)
		Network_formula_activities (me, fromNode, toNode, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_Network_setActivityClippingRule, U"Network: Set activity clipping rule", nullptr) {
	RADIO_ENUM (kNetwork_activityClippingRule, activityClippingRule,
			U"Activity clipping rule", kNetwork_activityClippingRule::DEFAULT)
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivityClippingRule (me, activityClippingRule);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setActivityLeak, U"Network: Set activity leak", nullptr) {
	REAL (activityLeak, U"Activity leak", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivityLeak (me, activityLeak);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setClamping, U"Network: Set clamping", nullptr) {
	NATURAL (node, U"Node", U"1")
	BOOLEAN (clamping, U"Clamping", true)
	OK
DO
	MODIFY_EACH (Network)
		Network_setClamping (me, node, clamping);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setInstar, U"Network: Set instar", nullptr) {
	REAL (instar, U"Instar", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setInstar (me, instar);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setWeightLeak, U"Network: Set weight leak", nullptr) {
	REAL (weightLeak, U"Weight leak", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setWeightLeak (me, weightLeak);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setOutstar, U"Network: Set outstar", nullptr) {
	REAL (outstar, U"Outstar", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setOutstar (me, outstar);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setShunting, U"Network: Set shunting", nullptr) {
	REAL (shunting, U"Shunting", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setShunting (me, shunting);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_setWeight, U"Network: Set weight", nullptr) {
	NATURAL (connection, U"Connection", U"1")
	REAL (weight, U"Weight", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setWeight (me, connection, weight);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_spreadActivities, U"Network: Spread activities", nullptr) {
	NATURAL (numberOfSteps, U"Number of steps", U"20")
	OK
DO
	MODIFY_EACH (Network)
		Network_spreadActivities (me, numberOfSteps);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Network_updateWeights) {
	MODIFY_EACH (Network)
		Network_updateWeights (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Network_zeroActivities, U"Network: Zero activities", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	OK
DO
	MODIFY_EACH (Network)
		Network_zeroActivities (me, fromNode, toNode);
	MODIFY_EACH_END
}

// MARK: - OTGRAMMAR

// MARK: New

DIRECT (HELP_OT_learning_tutorial) {
	HELP (U"OT learning")
}

DIRECT (NEW1_Create_NoCoda_grammar) {
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_NoCoda_grammar ();
	CREATE_ONE_END (U"NoCoda")
}

DIRECT (NEW1_Create_NPA_grammar) {
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_NPA_grammar ();
	CREATE_ONE_END (U"assimilation")
}

DIRECT (NEW1_Create_NPA_distribution) {
	CREATE_ONE
		autoPairDistribution result = OTGrammar_create_NPA_distribution ();
	CREATE_ONE_END (U"assimilation")
}

FORM (NEW1_Create_tongue_root_grammar, U"Create tongue-root grammar", U"Create tongue-root grammar...") {
	RADIO_ENUM (kOTGrammar_createTongueRootGrammar_constraintSet, constraintSet,
			U"Constraint set", kOTGrammar_createTongueRootGrammar_constraintSet::DEFAULT)
	RADIO_ENUM (kOTGrammar_createTongueRootGrammar_ranking, ranking,
			U"Ranking", kOTGrammar_createTongueRootGrammar_ranking::DEFAULT)
	OK
DO
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_tongueRoot_grammar (constraintSet, ranking);
	CREATE_ONE_END (kOTGrammar_createTongueRootGrammar_ranking_getText (ranking))
}

FORM (NEW1_Create_metrics_grammar, U"Create metrics grammar", nullptr) {
	OPTIONMENU_ENUM (kOTGrammar_createMetricsGrammar_initialRanking, initialRanking,
			U"Initial ranking", kOTGrammar_createMetricsGrammar_initialRanking::DEFAULT)
	OPTIONMENU (trochaicityConstraint, U"Trochaicity constraint", 1)
		OPTION (U"FtNonfinal")
		OPTION (U"Trochaic")
	BOOLEAN (includeFootBimoraic, U"Include FootBimoraic", false)
	BOOLEAN (includeFootBisyllabic, U"Include FootBisyllabic", false)
	BOOLEAN (includePeripheral, U"Include Peripheral", false)
	OPTIONMENU (nonfinalityConstraint, U"Nonfinality constraint", 1)
		OPTION (U"Nonfinal")
		OPTION (U"MainNonfinal")
		OPTION (U"HeadNonfinal")
	BOOLEAN (overtFormsHaveSecondaryStress, U"Overt forms have secondary stress", true)
	BOOLEAN (includeClashAndLapse, U"Include *Clash and *Lapse", false)
	BOOLEAN (includeCodas, U"Include codas", false)
	OK
DO
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_metrics (initialRanking, trochaicityConstraint,
			includeFootBimoraic, includeFootBisyllabic, includePeripheral, nonfinalityConstraint,
			overtFormsHaveSecondaryStress, includeClashAndLapse, includeCodas);
	CREATE_ONE_END (kOTGrammar_createMetricsGrammar_initialRanking_getText (initialRanking))
}

// MARK: Save

FORM_SAVE (SAVE_OTGrammar_writeToHeaderlessSpreadsheetFile, U"Write OTGrammar to spreadsheet", 0, U"txt") {
	SAVE_ONE (OTGrammar)
		OTGrammar_writeToHeaderlessSpreadsheetFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP_OTGrammar_help) {
	HELP (U"OTGrammar")
}

// MARK: View & Edit

DIRECT (WINDOW_OTGrammar_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit from batch.");
	FIND_ONE_WITH_IOBJECT (OTGrammar)
		autoOTGrammarEditor editor = OTGrammarEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Draw

FORM (GRAPHICS_OTGrammar_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (inputString, U"Input string", U"")
	OK
DO
	GRAPHICS_EACH (OTGrammar)
		OTGrammar_drawTableau (me, GRAPHICS, false, inputString);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_OTGrammar_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
	SENTENCE (inputString, U"Input string", U"")
	OK
DO
	GRAPHICS_EACH (OTGrammar)
		OTGrammar_drawTableau (me, GRAPHICS, true, inputString);
	GRAPHICS_EACH_END
}

// MARK: Query

DIRECT (INTEGER_OTGrammar_getNumberOfConstraints) {
	INTEGER_ONE (OTGrammar)
		integer result = my numberOfConstraints;
	INTEGER_ONE_END (U" constraints")
}

FORM (STRING_OTGrammar_getConstraint, U"Get constraint name", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	STRING_ONE (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const conststring32 result = my constraints [constraintNumber]. name.get();
	STRING_ONE_END
}

FORM (REAL_OTGrammar_getRankingValue, U"Get ranking value", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const double result = my constraints [constraintNumber]. ranking;
	NUMBER_ONE_END (U" (ranking of constraint ", constraintNumber, U")")
}

FORM (REAL_OTGrammar_getDisharmony, U"Get disharmony", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const double result = my constraints [constraintNumber]. disharmony;
	NUMBER_ONE_END (U" (disharmony of constraint ", constraintNumber, U")")
}

DIRECT (INTEGER_OTGrammar_getNumberOfTableaus) {
	INTEGER_ONE (OTGrammar)
		const integer result = my numberOfTableaus;
	INTEGER_ONE_END (U" tableaus")
}

FORM (STRING_OTGrammar_getInput, U"Get input", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	STRING_ONE (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const conststring32 result = my tableaus [tableauNumber]. input.get();
	STRING_ONE_END
}

FORM (INTEGER_OTGrammar_getNumberOfCandidates, U"Get number of candidates", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = my tableaus [tableauNumber]. numberOfCandidates;
	NUMBER_ONE_END (U" candidates in tableau ", tableauNumber)
}

FORM (STRING_OTGrammar_getCandidate, U"Get candidate", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	STRING_ONE (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const conststring32 result = my tableaus [tableauNumber]. candidates [candidateNumber]. output.get();
	STRING_ONE_END
}

FORM (INTEGER_OTGrammar_getNumberOfViolations, U"Get number of violations", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		my checkConstraintNumber (constraintNumber);
		const integer result = my tableaus [tableauNumber]. candidates [candidateNumber]. marks [constraintNumber];
	NUMBER_ONE_END (U" violations")
}

// MARK: Query (parse)

FORM (INTEGER_OTGrammar_getWinner, U"Get winner", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = OTGrammar_getWinner (me, tableauNumber);
	NUMBER_ONE_END (U" (winner in tableau ", tableauNumber, U")")
}

FORM (INTEGER_OTGrammar_compareCandidates, U"Compare candidates", nullptr) {
	NATURAL (tableauNumber1, U"Tableau number 1", U"1")
	NATURAL (candidateNumber1, U"Candidate number 1", U"1")
	NATURAL (tableauNumber2, U"Tableau number 2", U"1")
	NATURAL (candidateNumber2, U"Candidate number 2", U"2")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber1, candidateNumber1);
		my checkTableauAndCandidateNumber (tableauNumber2, candidateNumber2);
		const integer result = OTGrammar_compareCandidates (me, tableauNumber1, candidateNumber1, tableauNumber2, candidateNumber2);
	NUMBER_ONE_END (result == -1 ? U" (candidate 1 is better)" :
					result == +1 ? U" (candidate 2 is better)" : U" (candidates are equally good)")
}

FORM (INTEGER_OTGrammar_getNumberOfOptimalCandidates, U"Get number of optimal candidates", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = OTGrammar_getNumberOfOptimalCandidates (me, tableauNumber);
	NUMBER_ONE_END (U" optimal candidates in tableau ", tableauNumber)
}

FORM (BOOLEAN_OTGrammar_isCandidateGrammatical, U"Is candidate grammatical?", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const integer result = OTGrammar_isCandidateGrammatical (me, tableauNumber, candidateNumber);
	NUMBER_ONE_END (result ? U" (grammatical)" : U" (ungrammatical)")
}

FORM (BOOLEAN_OTGrammar_isCandidateSinglyGrammatical, U"Is candidate singly grammatical?", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	NUMBER_ONE (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const integer result = OTGrammar_isCandidateSinglyGrammatical (me, tableauNumber, candidateNumber);
	NUMBER_ONE_END (result ? U" (singly grammatical)" : U" (not singly grammatical)")
}

FORM (STRING_OTGrammar_getInterpretiveParse, U"OTGrammar: Interpretive parse", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	FIND_ONE (OTGrammar)
		integer bestInput, bestOutput;
		OTGrammar_getInterpretiveParse (me, partialOutput, & bestInput, & bestOutput);
		Melder_information (U"Best input = ", bestInput, U": ", my tableaus [bestInput]. input.get(),
			U"\nBest output = ", bestOutput, U": ", my tableaus [bestInput]. candidates [bestOutput]. output.get());
	END
}

FORM (BOOLEAN_OTGrammar_isPartialOutputGrammatical, U"Is partial output grammatical?", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	NUMBER_ONE (OTGrammar)
		const integer result = OTGrammar_isPartialOutputGrammatical (me, partialOutput);
	NUMBER_ONE_END (result ? U" (grammatical)" : U" (ungrammatical)")
}

FORM (BOOLEAN_OTGrammar_isPartialOutputSinglyGrammatical, U"Is partial output singly grammatical?", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	NUMBER_ONE (OTGrammar)
		const integer result = OTGrammar_isPartialOutputSinglyGrammatical (me, partialOutput);
	NUMBER_ONE_END (result ? U" (singly grammatical)" : U" (not singly grammatical)")
}

// MARK: -

FORM (NEW_OTGrammar_generateInputs, U"Generate inputs", U"OTGrammar: Generate inputs...") {
	NATURAL (numberOfTrials, U"Number of trials", U"1000")
	OK
DO
	CONVERT_EACH (OTGrammar)
		autoStrings result = OTGrammar_generateInputs (me, numberOfTrials);
	CONVERT_EACH_END (my name.get(), U"_in")
}

DIRECT (NEW_OTGrammar_getInputs) {
	CONVERT_EACH (OTGrammar)
		autoStrings result = OTGrammar_getInputs (me);
	CONVERT_EACH_END (my name.get(), U"_in")
}

DIRECT (NEW_MODIFY_OTGrammar_measureTypology) {
	LOOP try {
		iam (OTGrammar);
		autoDistributions thee = OTGrammar_measureTypology_WEAK (me);
		praat_new (std::move (thee), my name.get(), U"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END }

// MARK: Evaluate

FORM (MODIFY_OTGrammar_evaluate, U"OTGrammar: Evaluate", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_newDisharmonies (me, evaluationNoise);
	MODIFY_EACH_END
}

FORM (STRING_MODIFY_OTGrammar_inputToOutput, U"OTGrammar: Input to output", U"OTGrammar: Input to output...") {
	SENTENCE (inputForm, U"Input form", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	FIND_ONE (OTGrammar)
		autostring32 output = OTGrammar_inputToOutput (me, inputForm, evaluationNoise);
		Melder_information (output.get());
		praat_dataChanged (me);
	END
}

FORM (NEW1_MODIFY_OTGrammar_inputToOutputs, U"OTGrammar: Input to outputs", U"OTGrammar: Input to outputs...") {
	NATURAL (trials, U"Trials", U"1000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	SENTENCE (inputForm, U"Input form", U"")
	OK
DO
	FIND_ONE (OTGrammar)
		autoStrings thee = OTGrammar_inputToOutputs (me, inputForm, trials, evaluationNoise);
		praat_new (thee.move(), my name.get(), U"_out");
		praat_dataChanged (me);
	END
}

FORM (NEW_MODIFY_OTGrammar_to_Distributions, U"OTGrammar: Compute output distributions", U"OTGrammar: To output Distributions...") {
	NATURAL (trialsPerInput, U"Trials per input", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	LOOP {
		iam (OTGrammar);
		try {
			autoDistributions thee = OTGrammar_to_Distribution (me, trialsPerInput, evaluationNoise);
			praat_new (thee.move(), my name.get(), U"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END }

FORM (NEW_MODIFY_OTGrammar_to_PairDistribution, U"OTGrammar: Compute output distributions", nullptr) {
	NATURAL (trialsPerInput, U"Trials per input", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	LOOP try {
		iam (OTGrammar);
		autoPairDistribution thee = OTGrammar_to_PairDistribution (me, trialsPerInput, evaluationNoise);
		praat_new (thee.move(), my name.get(), U"_out");
		praat_dataChanged (me);
	} catch (MelderError) {
		praat_dataChanged (OBJECT);
		throw;
	}
END }

// MARK: Modify ranking

FORM (MODIFY_OTGrammar_setRanking, U"OTGrammar: Set ranking", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	REAL (ranking, U"Ranking", U"100.0")
	REAL (disharmony, U"Disharmony", U"100.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_setRanking (me, constraintNumber, ranking, disharmony);
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_resetAllRankings, U"OTGrammar: Reset all rankings", nullptr) {
	REAL (ranking, U"Ranking", U"100.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_reset (me, ranking);
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_resetToRandomRanking, U"OTGrammar: Reset to random ranking", nullptr) {
	REAL (mean, U"Mean", U"10.0")
	POSITIVE (standardDeviation, U"Standard deviation", U"1e-4")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_resetToRandomRanking (me, mean, standardDeviation);
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_resetToRandomTotalRanking, U"OTGrammar: Reset to random total ranking", nullptr) {
	REAL (maximumRanking, U"Maximum ranking", U"100.0")
	POSITIVE (rankingDistance, U"Ranking distance", U"1.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_resetToRandomTotalRanking (me, maximumRanking, rankingDistance);
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_learnOne, U"OTGrammar: Learn one", U"OTGrammar: Learn one...") {
	SENTENCE (inputString, U"Input string", U"")
	SENTENCE (outputString, U"Output string", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	OK
DO
	MODIFY_EACH_WEAK (OTGrammar)
		OTGrammar_learnOne (me, inputString, outputString, evaluationNoise, updateRule, honourLocalRankings,
			plasticity, relativePlasticitySpreading, true, true, nullptr);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_OTGrammar_learnOneFromPartialOutput, U"OTGrammar: Learn one from partial adult output", nullptr) {
	LABEL (U"Partial adult surface form (e.g. overt form):")
	SENTENCE (partialOutput, U"Partial output", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	OK
DO
	MODIFY_EACH_WEAK (OTGrammar)
		OTGrammar_learnOneFromPartialOutput (me, partialOutput, evaluationNoise, updateRule, honourLocalRankings,
			plasticity, relativePlasticitySpreading, numberOfChews, true);
	MODIFY_EACH_WEAK_END
}

// MARK: Modify behaviour

FORM (MODIFY_OTGrammar_setDecisionStrategy, U"OTGrammar: Set decision strategy", nullptr) {
	RADIO_ENUM (kOTGrammar_decisionStrategy, decisionStrategy,
			U"Decision strategy", kOTGrammar_decisionStrategy::DEFAULT)
OK
	FIND_ONE (OTGrammar)
		SET_ENUM (decisionStrategy, kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	MODIFY_EACH (OTGrammar)
		my decisionStrategy = decisionStrategy;
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_setLeak, U"OTGrammar: Set leak", nullptr) {
	REAL (leak, U"Leak", U"0.0")
OK
	FIND_ONE (OTGrammar)
		SET_REAL (leak, my leak)
DO
	MODIFY_EACH (OTGrammar)
		my leak = leak;
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_setConstraintPlasticity, U"OTGrammar: Set constraint plasticity", nullptr) {
	NATURAL (constraint, U"Constraint", U"1")
	REAL (plasticity, U"Plasticity", U"1.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_setConstraintPlasticity (me, constraint, plasticity);
	MODIFY_EACH_END
}

// MARK: Modify structure

FORM (MODIFY_OTGrammar_removeConstraint, U"OTGrammar: Remove constraint", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_removeConstraint (me, constraintName);
	MODIFY_EACH_END
}

FORM (MODIFY_OTGrammar_removeHarmonicallyBoundedCandidates, U"OTGrammar: Remove harmonically bounded candidates", nullptr) {
	BOOLEAN (singly, U"Singly", false)
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_removeHarmonicallyBoundedCandidates (me, singly);
	MODIFY_EACH_END
}

// MARK: OTGRAMMAR & STRINGS

FORM (NEW1_MODIFY_OTGrammar_Strings_inputsToOutputs, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	FIND_TWO (OTGrammar, Strings)
		autoStrings result = OTGrammar_inputsToOutputs (me, you, evaluationNoise);
		praat_new (result.move(), my name.get(), U"_out");
		praat_dataChanged (me);
	END
}

DIRECT (BOOLEAN_OTGrammar_Strings_areAllPartialOutputsGrammatical) {
	NUMBER_TWO (OTGrammar, Strings)
		integer result = OTGrammar_areAllPartialOutputsGrammatical (me, you);
	NUMBER_TWO_END (result ? U" (all grammatical)" : U" (not all grammatical)")
}

DIRECT (BOOLEAN_OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical) {
	NUMBER_TWO (OTGrammar, Strings)
		integer result = OTGrammar_areAllPartialOutputsSinglyGrammatical (me, you);
	NUMBER_TWO_END (result ? U" (all singly grammatical)" : U" (not all singly grammatical)")
}

FORM (MODIFY_OTGrammar_Stringses_learn, U"OTGrammar: Learn", U"OTGrammar & 2 Strings: Learn...") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_COUPLE_WEAK (OTGrammar, Strings)
		OTGrammar_learn (me, you, him, evaluationNoise, updateRule, honourLocalRankings,
			plasticity, relativePlasticitySpreading, numberOfChews);
	MODIFY_FIRST_OF_ONE_AND_COUPLE_WEAK_END
}

FORM (MODIFY_OTGrammar_Strings_learnFromPartialOutputs, U"OTGrammar: Learn from partial adult outputs", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTGrammar, Strings)
		autoOTHistory history;
		try {
			OTGrammar_learnFromPartialOutputs (me, you, evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // e.g. in case of partial learning
			Melder_flushError ();
			// trickle down to save history
		}
		if (history) praat_new (history.move(), my name.get());
	END
}

// MARK: OTGRAMMAR & DISTRIBUTIONS

FORM (REAL_MODIFY_OTGrammar_Distributions_getFractionCorrect, U"OTGrammar & Distributions: Get fraction correct...", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replications, U"Replications", U"100000")
	OK
DO
	FIND_TWO (OTGrammar, Distributions)
		const double result = OTGrammar_Distributions_getFractionCorrect (me, you, columnNumber,
			evaluationNoise, replications);
		praat_dataChanged (me);
		Melder_informationReal (result, nullptr);
	END
}

FORM (MODIFY_OTGrammar_Distributions_learnFromPartialOutputs, U"OTGrammar & Distributions: Learn from partial outputs", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTGrammar, Distributions)
		autoOTHistory history;
		try {
			OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
				updateRule, honourLocalRankings,
				initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
				relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, false, false, 0
			);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			Melder_flushError ();
		}
		if (history)
			praat_new (history.move(), my name.get());
	END
}

FORM (MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_rrip, U"OTGrammar & Distributions: Learn from partial outputs (rrip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTGrammar, Distributions)
		autoOTHistory history;
		try {
			OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
				updateRule, honourLocalRankings,
				initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
				relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 0
			);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			Melder_flushError ();
		}
		if (history)
			praat_new (history.move(), my name.get());
	END
}

FORM (MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_eip, U"OTGrammar & Distributions: Learn from partial outputs (eip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTGrammar, Distributions)
		autoOTHistory history;
		try {
			OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
				updateRule, honourLocalRankings,
				initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
				relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 1000
			);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			Melder_flushError ();
		}
		if (history)
			praat_new (history.move(), my name.get());
	END
}

FORM (MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_wrip, U"OTGrammar & Distributions: Learn from partial outputs (wrip)", U"OT learning 6. Shortcut to grammar learning") {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTGrammar, Distributions)
		autoOTHistory history;
		try {
			OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
				updateRule, honourLocalRankings,
				initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
				relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 1
			);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			Melder_flushError ();
		}
		if (history)
			praat_new (history.move(), my name.get());
	END
}

FORM (LIST_OTGrammar_Distributions_listObligatoryRankings, U"OTGrammar & Distributions: Get fraction correct...", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	INFO_TWO (OTGrammar, Distributions)
		OTGrammar_Distributions_listObligatoryRankings (me, you, columnNumber);
	INFO_TWO_END
}

// MARK: OTGRAMMAR & PAIRDISTRIBUTION

FORM (MODIFY_OTGrammar_PairDistribution_findPositiveWeights, U"OTGrammar & PairDistribution: Find positive weights", U"OTGrammar & PairDistribution: Find positive weights...") {
	POSITIVE (weightFloor, U"Weight floor", U"1.0")
	POSITIVE (marginOfSeparation, U"Margin of separation", U"1.0")
	OK
DO
	MODIFY_FIRST_OF_TWO (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_findPositiveWeights (me, you, weightFloor, marginOfSeparation);
	MODIFY_FIRST_OF_TWO_END
}

FORM (REAL_MODIFY_OTGrammar_PairDistribution_getFractionCorrect, U"OTGrammar & PairDistribution: Get fraction correct...", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replications, U"Replications", U"100000")
	OK
DO
	FIND_TWO (OTGrammar, PairDistribution)
		double result;
		try {
			result = OTGrammar_PairDistribution_getFractionCorrect (me, you, evaluationNoise, replications);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
		Melder_information (result, U" correct");
	END
}

FORM (INTEGER_MODIFY_OTGrammar_PairDistribution_getMinimumNumberCorrect, U"OTGrammar & PairDistribution: Get minimum number correct...", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replicationsPerInput, U"Replications per input", U"1000")
	OK
DO
	FIND_TWO (OTGrammar, PairDistribution)
		integer result;
		try {
			result = OTGrammar_PairDistribution_getMinimumNumberCorrect (me, you,
				evaluationNoise, replicationsPerInput);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
		Melder_information (result, U" (minimally correct)");
	END
}

FORM (MODIFY_OTGrammar_PairDistribution_learn, U"OTGrammar & PairDistribution: Learn", U"OT learning 6. Shortcut to grammar learning") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	POSITIVE (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", true)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	OK
DO
	MODIFY_FIRST_OF_TWO_WEAK (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_learn (me, you,
			evaluationNoise, updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity,
			plasticityDecrement, numberOfPlasticities, relativePlasticitySpreading, numberOfChews);
	MODIFY_FIRST_OF_TWO_WEAK_END
}

DIRECT (LIST_OTGrammar_PairDistribution_listObligatoryRankings) {
	FIND_TWO (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_listObligatoryRankings (me, you);
	END
}

// MARK: - OTMULTI

// MARK: New

FORM (NEW1_Create_multi_level_metrics_grammar, U"Create multi-level metrics grammar", nullptr) {
	OPTIONMENU_ENUM (kOTGrammar_createMetricsGrammar_initialRanking, initialRanking,
			U"Initial ranking", kOTGrammar_createMetricsGrammar_initialRanking::DEFAULT)
	OPTIONMENU (trochaicityConstraint, U"Trochaicity constraint", 1)
		OPTION (U"FtNonfinal")
		OPTION (U"Trochaic")
	BOOLEAN (includeFootBimoraic, U"Include FootBimoraic", false)
	BOOLEAN (includeFootBisyllabic, U"Include FootBisyllabic", false)
	BOOLEAN (includePeripheral, U"Include Peripheral", false)
	OPTIONMENU (nonfinalityConstraint, U"Nonfinality constraint", 1)
		OPTION (U"Nonfinal")
		OPTION (U"MainNonfinal")
		OPTION (U"HeadNonfinal")
	BOOLEAN (overtFormsHaveSecondaryStress, U"Overt forms have secondary stress", true)
	BOOLEAN (includeClashAndLapse, U"Include *Clash and *Lapse", false)
	BOOLEAN (includeCodas, U"Include codas", false)
	OK
DO
	CREATE_ONE
		autoOTMulti result = OTMulti_create_metrics (initialRanking, trochaicityConstraint,
			includeFootBimoraic, includeFootBisyllabic, includePeripheral, nonfinalityConstraint,
			overtFormsHaveSecondaryStress, includeClashAndLapse, includeCodas);
	CREATE_ONE_END (kOTGrammar_createMetricsGrammar_initialRanking_getText (initialRanking))
}

// MARK: Draw

FORM (GRAPHICS_OTMulti_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	BOOLEAN (showDisharmonies, U"Show disharmonies", true)
	OK
DO
	GRAPHICS_EACH (OTMulti)
		OTMulti_drawTableau (me, GRAPHICS, partialForm1, partialForm2, false, showDisharmonies);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_OTMulti_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	BOOLEAN (showDisharmonies, U"Show disharmonies", true)
	OK
DO
	GRAPHICS_EACH (OTMulti)
		OTMulti_drawTableau (me, GRAPHICS, partialForm1, partialForm2, true, showDisharmonies);
	GRAPHICS_EACH_END
}

// MARK: View & Edit

DIRECT (WINDOW_OTMulti_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit an OTMulti from batch.");
	FIND_ONE_WITH_IOBJECT (OTMulti)
		autoOTMultiEditor editor = OTMultiEditor_create (ID_AND_FULL_NAME, me);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

// MARK: Query

DIRECT (INTEGER_OTMulti_getNumberOfConstraints) {
	NUMBER_ONE (OTMulti)
		const integer result = my numberOfConstraints;
	NUMBER_ONE_END (U" constraints")
}

FORM (STRING_OTMulti_getConstraint, U"Get constraint name", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	STRING_ONE (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const conststring32 result = my constraints [constraintNumber]. name.get();
	STRING_ONE_END
}

FORM (INTEGER_OTMulti_getConstraintIndexFromName, U"OTMulti: Get constraint number", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	NUMBER_ONE (OTMulti)
		const integer result = OTMulti_getConstraintIndexFromName (me, constraintName);
	NUMBER_ONE_END (U" (index of constraint ", constraintName, U")")
}

FORM (REAL_OTMulti_getRankingValue, U"Get ranking value", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const double result = my constraints [constraintNumber]. ranking;
	NUMBER_ONE_END (U" (ranking of constraint ", constraintNumber, U")")
}

FORM (REAL_OTMulti_getDisharmony, U"Get disharmony", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const double result = my constraints [constraintNumber]. disharmony;
	NUMBER_ONE_END (U" (disharmony of constraint ", constraintNumber, U")")
}

DIRECT (INTEGER_OTMulti_getNumberOfCandidates) {
	NUMBER_ONE (OTMulti)
		integer result = my numberOfCandidates;
	NUMBER_ONE_END (U" candidates")
}

FORM (STRING_OTMulti_getCandidate, U"Get candidate", nullptr) {
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	STRING_ONE (OTMulti)
		if (candidateNumber > my numberOfCandidates)
			Melder_throw (U"Your candidate number should not exceed the number of candidates.");
		const conststring32 result = my candidates [candidateNumber]. string.get();
	STRING_ONE_END
}

FORM (INTEGER_OTMulti_getNumberOfViolations, U"Get number of violations", nullptr) {
	NATURAL (candidateNumber, U"Candidate number", U"1")
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	NUMBER_ONE (OTMulti)
		if (candidateNumber > my numberOfCandidates)
			Melder_throw (U"Your candidate number should not exceed the number of candidates.");
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		integer result = my candidates [candidateNumber]. marks [constraintNumber];
	NUMBER_ONE_END (U" violations")
}

FORM (INTEGER_OTMulti_getWinner, U"OTMulti: Get winner", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	OK
DO
	NUMBER_ONE (OTMulti)
		integer result = OTMulti_getWinner (me, partialForm1, partialForm2);
	NUMBER_ONE_END (U" (winner)")
}

// MARK: Evaluate

FORM (MODIFY_OTMulti_evaluate, U"OTMulti: Evaluate", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_newDisharmonies (me, evaluationNoise);
	MODIFY_EACH_END
}

FORM (STRING_MODIFY_OTMulti_generateOptimalForm, U"OTMulti: Generate optimal form", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	FIND_ONE (OTMulti)
		autostring32 output = OTMulti_generateOptimalForm (me, partialForm1, partialForm2, evaluationNoise);
		Melder_information (output.get());
		praat_dataChanged (me);
	END
}

FORM (NEW1_MODIFY_OTMulti_generateOptimalForms, U"OTMulti: Generate optimal forms", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	NATURAL (numberOfTrials, U"Number of trials", U"1000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	FIND_ONE (OTMulti)
		autoStrings thee = OTMulti_generateOptimalForms (me, partialForm1, partialForm2,
			numberOfTrials, evaluationNoise);
		praat_new (thee.move(), my name.get(), U"_out");
		praat_dataChanged (me);
	END
}

FORM (NEW_MODIFY_OTMulti_to_Distribution, U"OTMulti: Compute output distribution", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	NATURAL (numberOfTrials, U"Number of trials", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	LOOP {
		iam (OTMulti);
		try {
			autoDistributions result = OTMulti_to_Distribution (me, partialForm1, partialForm2,
				numberOfTrials, evaluationNoise);
			praat_new (result.move(), my name.get(), U"_out");
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);
			throw;
		}
	}
END }

// MARK: Modify ranking

FORM (MODIFY_OTMulti_setRanking, U"OTMulti: Set ranking", nullptr) {
	NATURAL (constraint, U"Constraint", U"1")
	REAL (ranking, U"Ranking", U"100.0")
	REAL (disharmony, U"Disharmony", U"100.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_setRanking (me, constraint, ranking, disharmony);
	MODIFY_EACH_END
}

FORM (MODIFY_OTMulti_resetAllRankings, U"OTMulti: Reset all rankings", nullptr) {
	REAL (ranking, U"Ranking", U"100.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_reset (me, ranking);
	MODIFY_EACH_END
}

FORM (MODIFY_OTMulti_learnOne, U"OTMulti: Learn one", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	OPTIONMENU (direction, U"Direction", 3)
		OPTION (U"forward")
		OPTION (U"backward")
		OPTION (U"bidirectionally")
	POSITIVE (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	OK
DO
	MODIFY_EACH_WEAK (OTMulti)
		OTMulti_learnOne (me, partialForm1, partialForm2, updateRule,
			direction, plasticity, relativePlasticitySpreading);
	MODIFY_EACH_WEAK_END
}

// MARK: Modify behaviour

FORM (MODIFY_OTMulti_setDecisionStrategy, U"OTMulti: Set decision strategy", nullptr) {
	RADIO_ENUM (kOTGrammar_decisionStrategy, decisionStrategy,
			U"Decision strategy", kOTGrammar_decisionStrategy::DEFAULT)
OK
	FIND_ONE (OTMulti)
		SET_ENUM (decisionStrategy, kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	MODIFY_EACH (OTMulti)
		my decisionStrategy = decisionStrategy;
	MODIFY_EACH_END
}

FORM (MODIFY_OTMulti_setLeak, U"OTGrammar: Set leak", nullptr) {
	REAL (leak, U"Leak", U"0.0")
OK
	FIND_ONE (OTMulti)
		SET_REAL (leak, my leak)
DO
	MODIFY_EACH (OTMulti)
		my leak = leak;
	MODIFY_EACH_END
}

FORM (MODIFY_OTMulti_setConstraintPlasticity, U"OTMulti: Set constraint plasticity", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	REAL (plasticity, U"Plasticity", U"1.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_setConstraintPlasticity (me, constraintNumber, plasticity);
	MODIFY_EACH_END
}

// MARK: Modify structure

FORM (MODIFY_OTMulti_removeConstraint, U"OTMulti: Remove constraint", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_removeConstraint (me, constraintName);
	MODIFY_EACH_END
}

// MARK: OTMULTI & PAIRDISTRIBUTION

FORM (DANGEROUS_MODIFY_OTMulti_PairDistribution_learn, U"OTMulti & PairDistribution: Learn", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	OPTIONMENU (direction, U"Direction", 3)
		OPTION (U"forward")
		OPTION (U"backward")
		OPTION (U"bidirectionally")
	POSITIVE (initialPlasticity, U"Initial plasticity", U"1.0")
	NATURAL (replicationsPerPlasticity, U"Replications per plasticity", U"100000")
	REAL (plasticityDecrement, U"Plasticity decrement", U"0.1")
	NATURAL (numberOfPlasticities, U"Number of plasticities", U"4")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	INTEGER (storeHistoryEvery, U"Store history every", U"0")
	OK
DO
	FIND_TWO (OTMulti, PairDistribution)
		autoTable history;
		try {
			OTMulti_PairDistribution_learn (me, you, evaluationNoise,
				updateRule, direction,
				initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
				relativePlasticitySpreading, storeHistoryEvery, & history);
			praat_dataChanged (me);
		} catch (MelderError) {
			praat_dataChanged (me);   // e.g. in case of partial learning
			Melder_flushError ();
			// trickle down to save history
		}
		if (history) praat_new (history.move(), my name.get());
	END
}

// MARK: OTMULTI & STRINGS

FORM (NEW1_MODIFY_OTMulti_Strings_generateOptimalForms, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (evaluationNoide, U"Evaluation noise", U"2.0")
	OK
DO
	FIND_TWO (OTMulti, Strings)
		autoStrings result = OTMulti_Strings_generateOptimalForms (me, you, evaluationNoide);
		praat_new (result.move(), my name.get(), U"_out");
		praat_dataChanged (me);
	END
}

// MARK: - NET

// MARK: New

FORM (NEW1_CreateNetAsDeepBeliefNetwork, U"Create Net as DeepBeliefNetwork", nullptr) {
	WORD (name, U"Name", U"network")
	NUMVEC (numbersOfNodes, U"Numbers of nodes", U"{ 30, 50, 20 }")
	BOOLEAN (inputsAreBinary, U"Inputs are binary", false)
	OK
DO
	CREATE_ONE
		autoNet result = Net_createAsDeepBeliefNet (numbersOfNodes, inputsAreBinary);
	CREATE_ONE_END (name)
}

// MARK: Modify

FORM (MODIFY_Net_spreadUp, U"Net: Spread up", nullptr) {
	RADIO_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::STOCHASTIC)
	OK
DO
	MODIFY_EACH (Net)
		Net_spreadUp (me, activationType);
	MODIFY_EACH_END
}

FORM (MODIFY_Net_spreadDown, U"Net: Spread down", nullptr) {
	RADIO_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::DETERMINISTIC)
	OK
DO
	MODIFY_EACH (Net)
		Net_spreadDown (me, activationType);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Net_spreadUp_reconstruction) {
	MODIFY_EACH (Net)
		Net_spreadUp_reconstruction (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Net_spreadDown_reconstruction) {
	MODIFY_EACH (Net)
		Net_spreadDown_reconstruction (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Net_sampleInput) {
	MODIFY_EACH (Net)
		Net_sampleInput (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_Net_sampleOutput) {
	MODIFY_EACH (Net)
		Net_sampleOutput (me);
	MODIFY_EACH_END
}

FORM (MODIFY_Net_update, U"Net: Update", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_EACH (Net)
		Net_update (me, learningRate);
	MODIFY_EACH_END
}

// MARK: Extract

DIRECT (NEW_Net_extractInputActivities) {
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractInputActivities (me);
	CONVERT_EACH_END (my name.get(), U"_inputActivities")
}

DIRECT (NEW_Net_extractOutputActivities) {
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractOutputActivities (me);
	CONVERT_EACH_END (my name.get(), U"_outputActivities")
}

DIRECT (NEW_Net_extractInputReconstruction) {
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractInputReconstruction (me);
	CONVERT_EACH_END (my name.get(), U"_inputReconstruction")
}

DIRECT (NEW_Net_extractOutputReconstruction) {
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractOutputReconstruction (me);
	CONVERT_EACH_END (my name.get(), U"_outputReconstruction")
}

FORM (NEW_Net_extractInputBiases, U"Net: Extract input biases", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractInputBiases (me, layerNumber);
	CONVERT_EACH_END (my name.get(), U"_inputBiases")
}

FORM (NEW_Net_extractOutputBiases, U"Net: Extract output biases", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractOutputBiases (me, layerNumber);
	CONVERT_EACH_END (my name.get(), U"_outputBiases")
}

FORM (NEW_Net_extractWeights, U"Net: Extract weights", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH (Net)
		autoMatrix result = Net_extractWeights (me, layerNumber);
	CONVERT_EACH_END (my name.get(), U"_weights")
}

FORM (NUMMAT_Net_getWeights, U"Net: Get weigths", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	NUMMAT_ONE (Net)
		autoMAT result = Net_getWeights (me, layerNumber);
	NUMMAT_ONE_END
}

// MARK: - NET & PATTERN

FORM (MODIFY_Net_PatternList_applyToInput, U"Net & PatternList: Apply to input", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_FIRST_OF_TWO (Net, PatternList)
		Net_PatternList_applyToInput (me, you, rowNumber);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_Net_PatternList_applyToOutput, U"Net & PatternList: Apply to output", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_FIRST_OF_TWO (Net, PatternList)
		Net_PatternList_applyToOutput (me, you, rowNumber);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_Net_PatternList_learn, U"Net & PatternList: Learn", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_TWO (Net, PatternList)
		Net_PatternList_learn (me, you, learningRate);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_Net_PatternList_learnByLayer, U"Net & PatternList: Learn by layer", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_TWO (Net, PatternList)
		Net_PatternList_learnByLayer (me, you, learningRate);
	MODIFY_FIRST_OF_TWO_END
}

FORM (MODIFY_Net_PatternList_learn_twoPhases, U"Net & PatternList: Learn (two phases)", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_TWO (Net, PatternList)
		Net_PatternList_learn_twoPhases (me, you, learningRate);
	MODIFY_FIRST_OF_TWO_END
}

FORM (NEW1_Net_PatternList_to_ActivationList, U"Net & PatternList: To ActivationList", nullptr) {
	RADIO_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::DETERMINISTIC)
	OK
DO
	CONVERT_TWO (Net, PatternList)
		autoActivationList result = Net_PatternList_to_ActivationList (me, you, activationType);
	CONVERT_TWO_END (my name.get(), U"_", your name.get())
}

// MARK: - NOULLIGRID

// MARK: View & Edit

DIRECT (WINDOW_NoulliGrid_viewAndEdit) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot edit a NoulliGrid from batch.");
	FIND_TWO_WITH_IOBJECT (NoulliGrid, Sound)   // Sound may be null
		autoNoulliGridEditor editor = NoulliGridEditor_create (ID_AND_FULL_NAME, me, you, true);
		praat_installEditor (editor.get(), IOBJECT);
		editor.releaseToUser();
	END
}

FORM (NUMVEC_NoulliGrid_getAverageProbabilities, U"NoulliGrid: Get average probabilities", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	REAL (fromTime, U"From time (s)", U"0")
	REAL (toTime, U"To time (s)", U"0 (= all)")
	OK
DO
	NUMVEC_ONE (NoulliGrid)
		autoVEC result = NoulliGrid_getAverageProbabilities (me, tierNumber, fromTime, toTime);
	NUMVEC_ONE_END
}

// MARK: - buttons

void praat_uvafon_gram_init ();
void praat_uvafon_gram_init () {
	Thing_recognizeClassesByName (classNetwork,
		classOTGrammar, classOTHistory, classOTMulti,
		classRBMLayer, classFullyConnectedLayer, classNet,
		classNoulliTier, classNoulliGrid,
		nullptr);
	Thing_recognizeClassByOtherName (classOTGrammar, U"OTCase");

	structNoulliGridEditor :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"New", U"Constraint grammars", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"OT learning tutorial", nullptr, praat_DEPTH_1 | praat_NO_API, HELP_OT_learning_tutorial);
		praat_addMenuCommand (U"Objects", U"New", U"-- tableau grammars --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create NoCoda grammar", nullptr, 1, NEW1_Create_NoCoda_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation grammar", nullptr, 1, NEW1_Create_NPA_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation distribution", nullptr, 1, NEW1_Create_NPA_distribution);
		praat_addMenuCommand (U"Objects", U"New", U"Create tongue-root grammar...", nullptr, 1, NEW1_Create_tongue_root_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create metrics grammar...", nullptr, 1, NEW1_Create_metrics_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create multi-level metrics grammar...", nullptr, 1, NEW1_Create_multi_level_metrics_grammar);
	praat_addAction1 (classOTGrammar, 1, U"Save as headerless spreadsheet file...", nullptr, 0, SAVE_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 1,   U"Write to headerless spreadsheet file...", U"*Save as headerless spreadsheet file...", praat_DEPRECATED_2011, SAVE_OTGrammar_writeToHeaderlessSpreadsheetFile);

	praat_addAction1 (classOTGrammar, 0, U"OTGrammar help", nullptr, 0, HELP_OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_OTGrammar_viewAndEdit);
	praat_addAction1 (classOTGrammar, 0,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_OTGrammar_viewAndEdit);
	praat_addAction1 (classOTGrammar, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau...", nullptr, 0, GRAPHICS_OTGrammar_drawTableau);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau (narrowly)...", nullptr, 0, GRAPHICS_OTGrammar_drawTableau_narrowly);
	praat_addAction1 (classOTGrammar, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get number of constraints", nullptr, 1, INTEGER_OTGrammar_getNumberOfConstraints);
		praat_addAction1 (classOTGrammar, 1, U"Get constraint...", nullptr, 1, STRING_OTGrammar_getConstraint);
		praat_addAction1 (classOTGrammar, 1, U"Get ranking value...", nullptr, 1, REAL_OTGrammar_getRankingValue);
		praat_addAction1 (classOTGrammar, 1, U"Get disharmony...", nullptr, 1, REAL_OTGrammar_getDisharmony);
		praat_addAction1 (classOTGrammar, 1, U"Get number of tableaus", nullptr, 1, INTEGER_OTGrammar_getNumberOfTableaus);
		praat_addAction1 (classOTGrammar, 1, U"Get input...", nullptr, 1, STRING_OTGrammar_getInput);
		praat_addAction1 (classOTGrammar, 1, U"Get number of candidates...", nullptr, 1, INTEGER_OTGrammar_getNumberOfCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get candidate...", nullptr, 1, STRING_OTGrammar_getCandidate);
		praat_addAction1 (classOTGrammar, 1, U"Get number of violations...", nullptr, 1, INTEGER_OTGrammar_getNumberOfViolations);
		praat_addAction1 (classOTGrammar, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get winner...", nullptr, 1, INTEGER_OTGrammar_getWinner);
		praat_addAction1 (classOTGrammar, 1, U"Compare candidates...", nullptr, 1, INTEGER_OTGrammar_compareCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get number of optimal candidates...", nullptr, 1, INTEGER_OTGrammar_getNumberOfOptimalCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate grammatical...", nullptr, 1, BOOLEAN_OTGrammar_isCandidateGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate singly grammatical...", nullptr, 1, BOOLEAN_OTGrammar_isCandidateSinglyGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Get interpretive parse...", nullptr, 1, STRING_OTGrammar_getInterpretiveParse);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output grammatical...", nullptr, 1, BOOLEAN_OTGrammar_isPartialOutputGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output singly grammatical...", nullptr, 1, BOOLEAN_OTGrammar_isPartialOutputSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 0, U"Generate inputs...", nullptr, 0, NEW_OTGrammar_generateInputs);
	praat_addAction1 (classOTGrammar, 0, U"Get inputs", nullptr, 0, NEW_OTGrammar_getInputs);
	praat_addAction1 (classOTGrammar, 0, U"Measure typology", nullptr, 0, NEW_MODIFY_OTGrammar_measureTypology);
	praat_addAction1 (classOTGrammar, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Evaluate...", nullptr, 0, MODIFY_OTGrammar_evaluate);
		praat_addAction1 (classOTGrammar, 0, U"Input to output...", nullptr, 0, STRING_MODIFY_OTGrammar_inputToOutput);
		praat_addAction1 (classOTGrammar, 0, U"Input to outputs...", nullptr, 0, NEW1_MODIFY_OTGrammar_inputToOutputs);
		praat_addAction1 (classOTGrammar, 0, U"To output Distributions...", nullptr, 0, NEW_MODIFY_OTGrammar_to_Distributions);
		praat_addAction1 (classOTGrammar, 0, U"To PairDistribution...", nullptr, 0, NEW_MODIFY_OTGrammar_to_PairDistribution);
	praat_addAction1 (classOTGrammar, 0, U"Modify ranking -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Set ranking...", nullptr, 1, MODIFY_OTGrammar_setRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset all rankings...", nullptr, 1, MODIFY_OTGrammar_resetAllRankings);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random ranking...", nullptr, 1, MODIFY_OTGrammar_resetToRandomRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random total ranking...", nullptr, 1, MODIFY_OTGrammar_resetToRandomTotalRanking);
		praat_addAction1 (classOTGrammar, 0, U"Learn one...", nullptr, 1, MODIFY_OTGrammar_learnOne);
		praat_addAction1 (classOTGrammar, 0, U"Learn one from partial output...", nullptr, 1, MODIFY_OTGrammar_learnOneFromPartialOutput);
	praat_addAction1 (classOTGrammar, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Set decision strategy...", nullptr, 1, MODIFY_OTGrammar_setDecisionStrategy);
		praat_addAction1 (classOTGrammar, 1,   U"Set harmony computation method...", U"*Set decision strategy...", praat_DEPTH_1 | praat_DEPRECATED_2006, MODIFY_OTGrammar_setDecisionStrategy);
		praat_addAction1 (classOTGrammar, 1, U"Set leak...", nullptr, 1, MODIFY_OTGrammar_setLeak);
		praat_addAction1 (classOTGrammar, 1, U"Set constraint plasticity...", nullptr, 1, MODIFY_OTGrammar_setConstraintPlasticity);
	praat_addAction1 (classOTGrammar, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Remove constraint...", nullptr, 1, MODIFY_OTGrammar_removeConstraint);
		praat_addAction1 (classOTGrammar, 0, U"Remove harmonically bounded candidates...", nullptr, 1, MODIFY_OTGrammar_removeHarmonicallyBoundedCandidates);

	praat_TableOfReal_init (classOTHistory);

	praat_addAction1 (classOTMulti, 0, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_OTMulti_viewAndEdit);
	praat_addAction1 (classOTMulti, 0,   U"Edit", U"*View & Edit", praat_DEPRECATED_2011, WINDOW_OTMulti_viewAndEdit);
	praat_addAction1 (classOTMulti, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau...", nullptr, 1, GRAPHICS_OTMulti_drawTableau);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau (narrowly)...", nullptr, 1, GRAPHICS_OTMulti_drawTableau_narrowly);
	praat_addAction1 (classOTMulti, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get number of constraints", nullptr, 1, INTEGER_OTMulti_getNumberOfConstraints);
		praat_addAction1 (classOTMulti, 1, U"Get constraint...", nullptr, 1, STRING_OTMulti_getConstraint);
		praat_addAction1 (classOTMulti, 1, U"Get constraint number...", nullptr, 1, INTEGER_OTMulti_getConstraintIndexFromName);
		praat_addAction1 (classOTMulti, 1, U"Get ranking value...", nullptr, 1, REAL_OTMulti_getRankingValue);
		praat_addAction1 (classOTMulti, 1, U"Get disharmony...", nullptr, 1, REAL_OTMulti_getDisharmony);
		praat_addAction1 (classOTMulti, 1, U"Get number of candidates", nullptr, 1, INTEGER_OTMulti_getNumberOfCandidates);
		praat_addAction1 (classOTMulti, 1, U"Get candidate...", nullptr, 1, STRING_OTMulti_getCandidate);
		praat_addAction1 (classOTMulti, 1, U"Get number of violations...", nullptr, 1, INTEGER_OTMulti_getNumberOfViolations);
		praat_addAction1 (classOTMulti, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get winner...", nullptr, 1, INTEGER_OTMulti_getWinner);
	praat_addAction1 (classOTMulti, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Evaluate...", nullptr, 1, MODIFY_OTMulti_evaluate);
		praat_addAction1 (classOTMulti, 0, U"Get output...", nullptr, 1, STRING_MODIFY_OTMulti_generateOptimalForm);
		praat_addAction1 (classOTMulti, 0, U"Get outputs...", nullptr, 1, NEW1_MODIFY_OTMulti_generateOptimalForms);
		praat_addAction1 (classOTMulti, 0, U"To output Distribution...", nullptr, 1, NEW_MODIFY_OTMulti_to_Distribution);
	praat_addAction1 (classOTMulti, 0, U"Modify ranking", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Set ranking...", nullptr, 0, MODIFY_OTMulti_setRanking);
		praat_addAction1 (classOTMulti, 0, U"Reset all rankings...", nullptr, 0, MODIFY_OTMulti_resetAllRankings);
		praat_addAction1 (classOTMulti, 0, U"Learn one...", nullptr, 0, MODIFY_OTMulti_learnOne);
	praat_addAction1 (classOTMulti, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Set decision strategy...", nullptr, 1, MODIFY_OTMulti_setDecisionStrategy);
		praat_addAction1 (classOTMulti, 1, U"Set leak...", nullptr, 1, MODIFY_OTMulti_setLeak);
		praat_addAction1 (classOTMulti, 1, U"Set constraint plasticity...", nullptr, 1, MODIFY_OTMulti_setConstraintPlasticity);
	praat_addAction1 (classOTMulti, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Remove constraint...", nullptr, 1, MODIFY_OTMulti_removeConstraint);

	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs grammatical?", nullptr, 1, BOOLEAN_OTGrammar_Strings_areAllPartialOutputsGrammatical);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs singly grammatical?", nullptr, 1, BOOLEAN_OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Inputs to outputs...", nullptr, 0, NEW1_MODIFY_OTGrammar_Strings_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Learn from partial outputs...", nullptr, 0, MODIFY_OTGrammar_Strings_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, U"Learn...", nullptr, 0, MODIFY_OTGrammar_Stringses_learn);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs...", nullptr, 0, MODIFY_OTGrammar_Distributions_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (rrip)...", nullptr, 0, MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_rrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (eip)...", nullptr, 0, MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_eip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (wrip)...", nullptr, 0, MODIFY_OTGrammar_Distributions_learnFromPartialOutputs_wrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0, REAL_MODIFY_OTGrammar_Distributions_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"List obligatory rankings...", nullptr, praat_HIDDEN, LIST_OTGrammar_Distributions_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Learn...", nullptr, 0, MODIFY_OTGrammar_PairDistribution_learn);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Find positive weights...", nullptr, 0, MODIFY_OTGrammar_PairDistribution_findPositiveWeights);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get fraction correct...", nullptr, 0, REAL_MODIFY_OTGrammar_PairDistribution_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get minimum number correct...", nullptr, 0, INTEGER_MODIFY_OTGrammar_PairDistribution_getMinimumNumberCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"List obligatory rankings", nullptr, 0, LIST_OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, U"Learn...", nullptr, 0, DANGEROUS_MODIFY_OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, U"Get outputs...", nullptr, 0, NEW1_MODIFY_OTMulti_Strings_generateOptimalForms);

	praat_addMenuCommand (U"Objects", U"New", U"Symmetric neural networks", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty Network...", nullptr, 1, NEW1_Create_empty_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network...", nullptr, 1, NEW1_Create_rectangular_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network (vertical)...", nullptr, 1, NEW1_Create_rectangular_Network_vertical);
		praat_addMenuCommand (U"Objects", U"New", U"Create Net as deep belief network...", nullptr, 1, NEW1_CreateNetAsDeepBeliefNetwork);

	praat_addAction1 (classNetwork, 0, U"Draw...", nullptr, 0, GRAPHICS_Network_draw);
	praat_addAction1 (classNetwork, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"List nodes...", nullptr, 1, LIST_Network_listNodes);
		praat_addAction1 (classNetwork, 1, U"Nodes down to table...", nullptr, 1, NEW_Network_nodes_downto_Table);
	praat_addAction1 (classNetwork, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"Get activity...", nullptr, 1, REAL_Network_getActivity);
		praat_addAction1 (classNetwork, 1, U"Get activities...", nullptr, 1, NUMVEC_Network_getActivities);
		praat_addAction1 (classNetwork, 1, U"Get weight...", nullptr, 1, REAL_Network_getWeight);
	praat_addAction1 (classNetwork, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 0, U"Add node...", nullptr, 1, MODIFY_Network_addNode);
		praat_addAction1 (classNetwork, 0, U"Add connection...", nullptr, 1, MODIFY_Network_addConnection);
		praat_addAction1 (classNetwork, 0, U"-- activity --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set activity...", nullptr, 1, MODIFY_Network_setActivity);
		praat_addAction1 (classNetwork, 0, U"Set clamping...", nullptr, 1, MODIFY_Network_setClamping);
		praat_addAction1 (classNetwork, 0, U"Zero activities...", nullptr, 1, MODIFY_Network_zeroActivities);
		praat_addAction1 (classNetwork, 0, U"Normalize activities...", nullptr, 1, MODIFY_Network_normalizeActivities);
		praat_addAction1 (classNetwork, 0, U"Formula (activities)...", nullptr, 1, MODIFY_Network_formula_activities);
		praat_addAction1 (classNetwork, 0, U"Spread activities...", nullptr, 1, MODIFY_Network_spreadActivities);
		praat_addAction1 (classNetwork, 0, U"Set activity clipping rule...", nullptr, 1, MODIFY_Network_setActivityClippingRule);
		praat_addAction1 (classNetwork, 0, U"Set activity leak...", nullptr, 1, MODIFY_Network_setActivityLeak);
		praat_addAction1 (classNetwork, 0, U"Set shunting...", nullptr, 1, MODIFY_Network_setShunting);
		praat_addAction1 (classNetwork, 0, U"-- weight --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set weight...", nullptr, 1, MODIFY_Network_setWeight);
		praat_addAction1 (classNetwork, 0, U"Update weights", nullptr, 1, MODIFY_Network_updateWeights);
		praat_addAction1 (classNetwork, 0, U"Normalize weights...", nullptr, 1, MODIFY_Network_normalizeWeights);
		praat_addAction1 (classNetwork, 0, U"Set instar...", nullptr, 1, MODIFY_Network_setInstar);
		praat_addAction1 (classNetwork, 0, U"Set outstar...", nullptr, 1, MODIFY_Network_setOutstar);
		praat_addAction1 (classNetwork, 0, U"Set weight leak...", nullptr, 1, MODIFY_Network_setWeightLeak);

	praat_addAction1 (classNet, 0, U"Query", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Get weights...", nullptr, 0, NUMMAT_Net_getWeights);
	praat_addAction1 (classNet, 0, U"Modify", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Spread up...", nullptr, 0, MODIFY_Net_spreadUp);
		praat_addAction1 (classNet, 0, U"Spread down...", nullptr, 0, MODIFY_Net_spreadDown);
		praat_addAction1 (classNet, 0, U"Spread up (reconstruction)", nullptr, 0, MODIFY_Net_spreadUp_reconstruction);
		praat_addAction1 (classNet, 0, U"Spread down (reconstruction)", nullptr, 0, MODIFY_Net_spreadDown_reconstruction);
		praat_addAction1 (classNet, 0, U"Sample input", nullptr, 0, MODIFY_Net_sampleInput);
		praat_addAction1 (classNet, 0, U"Sample output", nullptr, 0, MODIFY_Net_sampleOutput);
		praat_addAction1 (classNet, 0, U"Update...", nullptr, 0, MODIFY_Net_update);
	praat_addAction1 (classNet, 0, U"Extract", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Extract input activities", nullptr, 0, NEW_Net_extractInputActivities);
		praat_addAction1 (classNet, 0, U"Extract output activities", nullptr, 0, NEW_Net_extractOutputActivities);
		praat_addAction1 (classNet, 0, U"Extract input reconstruction", nullptr, 0, NEW_Net_extractInputReconstruction);
		praat_addAction1 (classNet, 0, U"Extract output reconstruction", nullptr, 0, NEW_Net_extractOutputReconstruction);
		praat_addAction1 (classNet, 0, U"Extract input biases...", nullptr, 0, NEW_Net_extractInputBiases);
		praat_addAction1 (classNet, 0, U"Extract output biases...", nullptr, 0, NEW_Net_extractOutputBiases);
		praat_addAction1 (classNet, 0, U"Extract weights...", nullptr, 0, NEW_Net_extractWeights);

	praat_addAction2 (classNet, 1, classPatternList, 1, U"Apply to input...", nullptr, 0, MODIFY_Net_PatternList_applyToInput);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Apply to output...", nullptr, 0, MODIFY_Net_PatternList_applyToOutput);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn...", nullptr, 0, MODIFY_Net_PatternList_learn);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn by layer...", nullptr, 0, MODIFY_Net_PatternList_learnByLayer);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn (two phases)...", nullptr, 0, MODIFY_Net_PatternList_learn_twoPhases);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"To ActivationList", nullptr, 0, NEW1_Net_PatternList_to_ActivationList);

	praat_addAction1 (classNoulliGrid, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_NoulliGrid_viewAndEdit);
	praat_addAction1 (classNoulliGrid, 0, U"Query -", nullptr, 0, nullptr);
	praat_TimeFunction_query_init (classNoulliGrid);
	praat_addAction1 (classNoulliGrid, 1, U"Get average probabilities...", nullptr, 1, NUMVEC_NoulliGrid_getAverageProbabilities);
	praat_addAction2 (classNoulliGrid, 1, classSound, 1, U"View & Edit", nullptr, praat_ATTRACTIVE, WINDOW_NoulliGrid_viewAndEdit);
}

/* End of file praat_gram.cpp */
