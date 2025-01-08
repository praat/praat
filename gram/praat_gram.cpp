/* praat_gram.cpp
 *
 * Copyright (C) 1997-2025 Paul Boersma
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
#include "CubeGridEditor.h"

#include "praat_TableOfReal.h"
#include "praat_TimeFunction.h"

// MARK: - NETWORK

// MARK: New

#define UiForm_addNetworkFields  \
	COMMENT (U"Activity spreading settings:") \
	REAL (spreadingRate, U"Spreading rate", U"0.01") \
	OPTIONMENU_ENUM (kNetwork_activityClippingRule, activityClippingRule, \
			U"Activity clipping rule", kNetwork_activityClippingRule::DEFAULT) \
	REAL (minimumActivity, U"left Activity range", U"0.0") \
	REAL (maximumActivity, U"right Activity range", U"1.0") \
	REAL (activityLeak, U"Activity leak", U"1.0") \
	COMMENT (U"Weight update settings:") \
	REAL (learningRate, U"Learning rate", U"0.1") \
	REAL (minimumWeight, U"left Weight range", U"-1.0") \
	REAL (maximumWeight, U"right Weight range", U"1.0") \
	REAL (weightLeak, U"Weight leak", U"0.0")

FORM (CREATE_ONE__Create_empty_Network, U"Create empty Network", nullptr) {
	WORD (name, U"Name", U"network")
	UiForm_addNetworkFields
	COMMENT (U"World coordinates:")
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

FORM (CREATE_ONE__Create_rectangular_Network, U"Create rectangular Network", nullptr) {
	UiForm_addNetworkFields
	COMMENT (U"Structure settings:")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	BOOLEAN (bottomRowClamped, U"Bottom row clamped", 1)
	COMMENT (U"Initial state settings:")
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

FORM (CREATE_ONE__Create_rectangular_Network_vertical, U"Create rectangular Network (vertical)", nullptr) {
	UiForm_addNetworkFields
	COMMENT (U"Structure settings:")
	NATURAL (numberOfRows, U"Number of rows", U"10")
	NATURAL (numberOfColumns, U"Number of columns", U"10")
	BOOLEAN (bottomRowClamped, U"Bottom row clamped", 1)
	COMMENT (U"Initial state settings:")
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

FORM (GRAPHICS_EACH__Network_draw, U"Draw Network", nullptr) {
	BOOLEAN (useColour, U"Use colour", true)
	OK
DO
	GRAPHICS_EACH (Network)
		Network_draw (me, GRAPHICS, useColour);
	GRAPHICS_EACH_END
}

// MARK: Tabulate

FORM (INFO_ONE__Network_listNodes, U"Network: List nodes", nullptr) {
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

FORM (CONVERT_EACH_TO_ONE__Network_nodes_downto_Table, U"Network: Nodes down to Table", nullptr) {
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
	CONVERT_EACH_TO_ONE (Network)
		autoTable result = Network_nodes_downto_Table (me, fromNodeNumber, toNodeNumber,
			includeNodeNumbers, includeX, includeY, positionDecimals,
			includeClamped, includeActivity, includeExcitation, activityDecimals
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Query

FORM (QUERY_ONE_FOR_REAL__Network_getActivity, U"Network: Get activity", nullptr) {
	NATURAL (node, U"Node", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Network)
		const double result = Network_getActivity (me, node);
	QUERY_ONE_FOR_REAL_END (U" (activity of node ", node, U")")
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__Network_getActivities, U"Network: Get activities", nullptr) {
	NATURAL (fromNode, U"From node", U"1")
	NATURAL (toNode, U"To node", U"0 (= all)")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (Network)
		autoVEC result = Network_getActivities (me, fromNode, toNode);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (QUERY_ONE_FOR_REAL__Network_getWeight, U"Network: Get weight", nullptr) {
	NATURAL (connection, U"Connection", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (Network)
		const double result = Network_getWeight (me, connection);
	QUERY_ONE_FOR_REAL_END (U" (weight of connection ", connection, U")")
}

// MARK: Modify

FORM (MODIFY_EACH__Network_addConnection, U"Network: Add connection", nullptr) {
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

FORM (MODIFY_EACH__Network_addNode, U"Network: Add node", nullptr) {
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

FORM (MODIFY_EACH__Network_normalizeActivities, U"Network: Normalize activities", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	OK
DO
	MODIFY_EACH (Network)
		Network_normalizeActivities (me, fromNode, toNode);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_normalizeWeights, U"Network: Normalize weights", nullptr) {
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

FORM (MODIFY_EACH__Network_setActivity, U"Network: Set activity", nullptr) {
	NATURAL (node, U"Node", U"1")
	REAL (activity, U"Activity", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivity (me, node, activity);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH_WEAK__Network_formula_activities, U"Network: Formula (activities)", nullptr) {
	INTEGER (fromNode, U"From node", U"1")
	INTEGER (toNode, U"To node", U"0 (= all)")
	COMMENT (U"`col` is the node number, `self` is the current activity")
	COMMENT (U"for col := 1 to ncol do { self [col] := `formula' }")
	FORMULA (formula, U"Formula", U"0")
	OK
DO
	MODIFY_EACH_WEAK (Network)
		Network_formula_activities (me, fromNode, toNode, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_EACH__Network_setActivityClippingRule, U"Network: Set activity clipping rule", nullptr) {
	CHOICE_ENUM (kNetwork_activityClippingRule, activityClippingRule,
			U"Activity clipping rule", kNetwork_activityClippingRule::DEFAULT)
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivityClippingRule (me, activityClippingRule);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setActivityLeak, U"Network: Set activity leak", nullptr) {
	REAL (activityLeak, U"Activity leak", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setActivityLeak (me, activityLeak);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setClamping, U"Network: Set clamping", nullptr) {
	NATURAL (node, U"Node", U"1")
	BOOLEAN (clamping, U"Clamping", true)
	OK
DO
	MODIFY_EACH (Network)
		Network_setClamping (me, node, clamping);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setInstar, U"Network: Set instar", nullptr) {
	REAL (instar, U"Instar", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setInstar (me, instar);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setWeightLeak, U"Network: Set weight leak", nullptr) {
	REAL (weightLeak, U"Weight leak", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setWeightLeak (me, weightLeak);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setOutstar, U"Network: Set outstar", nullptr) {
	REAL (outstar, U"Outstar", U"0.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setOutstar (me, outstar);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setShunting, U"Network: Set shunting", nullptr) {
	REAL (shunting, U"Shunting", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setShunting (me, shunting);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_setWeight, U"Network: Set weight", nullptr) {
	NATURAL (connection, U"Connection", U"1")
	REAL (weight, U"Weight", U"1.0")
	OK
DO
	MODIFY_EACH (Network)
		Network_setWeight (me, connection, weight);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_spreadActivities, U"Network: Spread activities", nullptr) {
	NATURAL (numberOfSteps, U"Number of steps", U"20")
	OK
DO
	MODIFY_EACH (Network)
		Network_spreadActivities (me, numberOfSteps);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Network_updateWeights) {
	MODIFY_EACH (Network)
		Network_updateWeights (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Network_zeroActivities, U"Network: Zero activities", nullptr) {
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

DIRECT (HELP__OT_learning_tutorial) {
	HELP (U"OT learning")
}

DIRECT (CREATE_ONE__Create_NoCoda_grammar) {
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_NoCoda_grammar ();
	CREATE_ONE_END (U"NoCoda")
}

DIRECT (CREATE_ONE__Create_NPA_grammar) {
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_NPA_grammar ();
	CREATE_ONE_END (U"assimilation")
}

DIRECT (CREATE_ONE__Create_NPA_distribution) {
	CREATE_ONE
		autoPairDistribution result = OTGrammar_create_NPA_distribution ();
	CREATE_ONE_END (U"assimilation")
}

FORM (CREATE_ONE__Create_tongue_root_grammar, U"Create tongue-root grammar", U"Create tongue-root grammar...") {
	CHOICE_ENUM (kOTGrammar_createTongueRootGrammar_constraintSet, constraintSet,
			U"Constraint set", kOTGrammar_createTongueRootGrammar_constraintSet::DEFAULT)
	CHOICE_ENUM (kOTGrammar_createTongueRootGrammar_ranking, ranking,
			U"Ranking", kOTGrammar_createTongueRootGrammar_ranking::DEFAULT)
	OK
DO
	CREATE_ONE
		autoOTGrammar result = OTGrammar_create_tongueRoot_grammar (constraintSet, ranking);
	CREATE_ONE_END (kOTGrammar_createTongueRootGrammar_ranking_getText (ranking))
}

FORM (CREATE_ONE__Create_metrics_grammar, U"Create metrics grammar", nullptr) {
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
			overtFormsHaveSecondaryStress, includeClashAndLapse, includeCodas
		);
	CREATE_ONE_END (kOTGrammar_createMetricsGrammar_initialRanking_getText (initialRanking))
}

// MARK: Save

FORM_SAVE (SAVE_ONE__OTGrammar_writeToHeaderlessSpreadsheetFile, U"Write OTGrammar to spreadsheet", 0, U"txt") {
	SAVE_ONE (OTGrammar)
		OTGrammar_writeToHeaderlessSpreadsheetFile (me, file);
	SAVE_ONE_END
}

// MARK: Help

DIRECT (HELP__OTGrammar_help) {
	HELP (U"OTGrammar")
}

// MARK: View & Edit

DIRECT (EDITOR_ONE__OTGrammar_viewAndEdit) {
	EDITOR_ONE (an,OTGrammar)
		autoOTGrammarEditor editor = OTGrammarEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: Draw

FORM (GRAPHICS_EACH__OTGrammar_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (inputString, U"Input string", U"")
	OK
DO
	GRAPHICS_EACH (OTGrammar)
		OTGrammar_drawTableau (me, GRAPHICS, false, inputString);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__OTGrammar_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
	SENTENCE (inputString, U"Input string", U"")
	OK
DO
	GRAPHICS_EACH (OTGrammar)
		OTGrammar_drawTableau (me, GRAPHICS, true, inputString);
	GRAPHICS_EACH_END
}

// MARK: Query

DIRECT (QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfConstraints) {
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		const integer result = my numberOfConstraints;
	QUERY_ONE_FOR_INTEGER_END (U" constraints")
}

FORM (QUERY_ONE_FOR_STRING__OTGrammar_getConstraint, U"Get constraint name", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const conststring32 result = my constraints [constraintNumber]. name.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_REAL__OTGrammar_getRankingValue, U"Get ranking value", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const double result = my constraints [constraintNumber]. ranking;
	QUERY_ONE_FOR_REAL_END (U" (ranking of constraint ", constraintNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__OTGrammar_getDisharmony, U"Get disharmony", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (OTGrammar)
		my checkConstraintNumber (constraintNumber);
		const double result = my constraints [constraintNumber]. disharmony;
	QUERY_ONE_FOR_REAL_END (U" (disharmony of constraint ", constraintNumber, U")")
}

DIRECT (QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfTableaus) {
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		const integer result = my numberOfTableaus;
	QUERY_ONE_FOR_INTEGER_END (U" tableaus")
}

FORM (QUERY_ONE_FOR_STRING__OTGrammar_getInput, U"Get input", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const conststring32 result = my tableaus [tableauNumber]. input.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfCandidates, U"Get number of candidates", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = my tableaus [tableauNumber]. numberOfCandidates;
	QUERY_ONE_FOR_INTEGER_END (U" candidates in tableau ", tableauNumber)
}

FORM (QUERY_ONE_FOR_STRING__OTGrammar_getCandidate, U"Get candidate", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const conststring32 result = my tableaus [tableauNumber]. candidates [candidateNumber]. output.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfViolations, U"Get number of violations", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		my checkConstraintNumber (constraintNumber);
		const integer result = my tableaus [tableauNumber]. candidates [candidateNumber]. marks [constraintNumber];
	QUERY_ONE_FOR_INTEGER_END (U" violations")
}

DIRECT (QUERY_ONE_FOR_BOOLEAN__OTGrammar_areAllOutputsDistinguishable) {
	QUERY_ONE_FOR_BOOLEAN (OTGrammar)
		const integer result = OTGrammar_areAllOutputsDistinguishable (me);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (all pairs are distinguishable)" : U" (at least one equivalent pair)")
}

DIRECT (CONVERT_EACH_TO_ONE_OTGrammar_tabulateEquivalentPairs) {
	CONVERT_EACH_TO_ONE (OTGrammar)
		autoTable result = OTGrammar_tabulateEquivalentPairs (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

// MARK: Query (parse)

FORM (QUERY_ONE_FOR_INTEGER__OTGrammar_getWinner, U"Get winner", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = OTGrammar_getWinner (me, tableauNumber);
	QUERY_ONE_FOR_INTEGER_END (U" (winner in tableau ", tableauNumber, U")")
}

FORM (QUERY_ONE_FOR_INTEGER__OTGrammar_compareCandidates, U"Compare candidates", nullptr) {
	NATURAL (tableauNumber1, U"Tableau number 1", U"1")
	NATURAL (candidateNumber1, U"Candidate number 1", U"1")
	NATURAL (tableauNumber2, U"Tableau number 2", U"1")
	NATURAL (candidateNumber2, U"Candidate number 2", U"2")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber1, candidateNumber1);
		my checkTableauAndCandidateNumber (tableauNumber2, candidateNumber2);
		const integer result = OTGrammar_compareCandidates (me, tableauNumber1, candidateNumber1, tableauNumber2, candidateNumber2);
	QUERY_ONE_FOR_INTEGER_END (result == -1 ? U" (candidate 1 is better)" :
					result == +1 ? U" (candidate 2 is better)" : U" (candidates are equally good)")
}

FORM (QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfOptimalCandidates, U"Get number of optimal candidates", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTGrammar)
		my checkTableauNumber (tableauNumber);
		const integer result = OTGrammar_getNumberOfOptimalCandidates (me, tableauNumber);
	QUERY_ONE_FOR_INTEGER_END (U" optimal candidates in tableau ", tableauNumber)
}

FORM (QUERY_ONE_FOR_BOOLEAN__OTGrammar_isCandidateGrammatical, U"Is candidate grammatical?", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	QUERY_ONE_FOR_BOOLEAN (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const integer result = OTGrammar_isCandidateGrammatical (me, tableauNumber, candidateNumber);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (grammatical)" : U" (ungrammatical)")
}

FORM (QUERY_ONE_FOR_BOOLEAN__OTGrammar_isCandidateSinglyGrammatical, U"Is candidate singly grammatical?", nullptr) {
	NATURAL (tableauNumber, U"Tableau number", U"1")
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	QUERY_ONE_FOR_BOOLEAN (OTGrammar)
		my checkTableauAndCandidateNumber (tableauNumber, candidateNumber);
		const integer result = OTGrammar_isCandidateSinglyGrammatical (me, tableauNumber, candidateNumber);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (singly grammatical)" : U" (not singly grammatical)")
}

FORM (INFO_ONE__OTGrammar_getInterpretiveParse, U"OTGrammar: Interpretive parse", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	INFO_ONE (OTGrammar)
		integer bestInput, bestOutput;
		OTGrammar_getInterpretiveParse (me, partialOutput, & bestInput, & bestOutput);
		Melder_information (U"Best input = ", bestInput, U": ", my tableaus [bestInput]. input.get(),
			U"\nBest output = ", bestOutput, U": ", my tableaus [bestInput]. candidates [bestOutput]. output.get());
	INFO_ONE_END
}

FORM (QUERY_ONE_FOR_BOOLEAN__OTGrammar_isPartialOutputGrammatical, U"Is partial output grammatical?", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	QUERY_ONE_FOR_BOOLEAN (OTGrammar)
		const integer result = OTGrammar_isPartialOutputGrammatical (me, partialOutput);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (grammatical)" : U" (ungrammatical)")
}

FORM (QUERY_ONE_FOR_BOOLEAN__OTGrammar_isPartialOutputSinglyGrammatical, U"Is partial output singly grammatical?", nullptr) {
	SENTENCE (partialOutput, U"Partial output", U"")
	OK
DO
	QUERY_ONE_FOR_BOOLEAN (OTGrammar)
		const integer result = OTGrammar_isPartialOutputSinglyGrammatical (me, partialOutput);
	QUERY_ONE_FOR_BOOLEAN_END (result ? U" (singly grammatical)" : U" (not singly grammatical)")
}

// MARK: -

FORM (CONVERT_EACH_TO_ONE__OTGrammar_generateInputs, U"Generate inputs", U"OTGrammar: Generate inputs...") {
	NATURAL (numberOfTrials, U"Number of trials", U"1000")
	OK
DO
	CONVERT_EACH_TO_ONE (OTGrammar)
		autoStrings result = OTGrammar_generateInputs (me, numberOfTrials);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_in")
}

DIRECT (CONVERT_EACH_TO_ONE__OTGrammar_getInputs) {
	CONVERT_EACH_TO_ONE (OTGrammar)
		autoStrings result = OTGrammar_getInputs (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_in")
}

DIRECT (CONVERT_EACH_WEAK_TO_ONE__OTGrammar_measureTypology) {
	CONVERT_EACH_WEAK_TO_ONE (OTGrammar)
		autoDistributions result = OTGrammar_measureTypology_WEAK (me);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

// MARK: Evaluate

FORM (MODIFY_EACH__OTGrammar_evaluate, U"OTGrammar: Evaluate", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_newDisharmonies (me, evaluationNoise);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_WEAK_FOR_STRING__OTGrammar_inputToOutput, U"OTGrammar: Input to output", U"OTGrammar: Input to output...") {
	SENTENCE (inputForm, U"Input form", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	QUERY_ONE_WEAK_FOR_STRING (OTGrammar)
		autostring32 output = OTGrammar_inputToOutput (me, inputForm, evaluationNoise);
		conststring32 result = output.get();
	QUERY_ONE_WEAK_FOR_STRING_END
}

FORM (CONVERT_EACH_WEAK_TO_ONE__OTGrammar_inputToOutputs, U"OTGrammar: Input to outputs", U"OTGrammar: Input to outputs...") {
	NATURAL (trials, U"Trials", U"1000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	SENTENCE (inputForm, U"Input form", U"")
	OK
DO
	CONVERT_EACH_WEAK_TO_ONE (OTGrammar)
		autoStrings result = OTGrammar_inputToOutputs (me, inputForm, trials, evaluationNoise);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

FORM (CONVERT_EACH_WEAK_TO_ONE__OTGrammar_to_Distributions, U"OTGrammar: Compute output distributions", U"OTGrammar: To output Distributions...") {
	NATURAL (trialsPerInput, U"Trials per input", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_EACH_WEAK_TO_ONE (OTGrammar)
		autoDistributions result = OTGrammar_to_Distribution (me, trialsPerInput, evaluationNoise);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

FORM (CONVERT_EACH_WEAK_TO_ONE__OTGrammar_to_PairDistribution, U"OTGrammar: Compute output distributions", nullptr) {
	NATURAL (trialsPerInput, U"Trials per input", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_EACH_WEAK_TO_ONE (OTGrammar)
		autoPairDistribution result = OTGrammar_to_PairDistribution (me, trialsPerInput, evaluationNoise);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

// MARK: Modify ranking

FORM (MODIFY_EACH__OTGrammar_setRanking, U"OTGrammar: Set ranking", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	REAL (ranking, U"Ranking", U"100.0")
	REAL (disharmony, U"Disharmony", U"100.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_setRanking (me, constraintNumber, ranking, disharmony);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_resetAllRankings, U"OTGrammar: Reset all rankings", nullptr) {
	REAL (ranking, U"Ranking", U"100.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_reset (me, ranking);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_resetToRandomRanking, U"OTGrammar: Reset to random ranking", nullptr) {
	REAL (mean, U"Mean", U"10.0")
	POSITIVE (standardDeviation, U"Standard deviation", U"1e-4")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_resetToRandomRanking (me, mean, standardDeviation);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_resetToRandomTotalRanking, U"OTGrammar: Reset to random total ranking", nullptr) {
	REAL (maximumRanking, U"Maximum ranking", U"100.0")
	POSITIVE (rankingDistance, U"Ranking distance", U"1.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_resetToRandomTotalRanking (me, maximumRanking, rankingDistance);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH_WEAK__OTGrammar_learnOne, U"OTGrammar: Learn one", U"OTGrammar: Learn one...") {
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

FORM (MODIFY_EACH_WEAK__OTGrammar_learnOneFromPartialOutput, U"OTGrammar: Learn one from partial adult output", nullptr) {
	COMMENT (U"Partial adult surface form (e.g. overt form):")
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

FORM (MODIFY_EACH__OTGrammar_setDecisionStrategy, U"OTGrammar: Set decision strategy", nullptr) {
	CHOICE_ENUM (kOTGrammar_decisionStrategy, decisionStrategy,
			U"Decision strategy", kOTGrammar_decisionStrategy::DEFAULT)
OK
	FIND_ONE (OTGrammar)
		SET_ENUM (decisionStrategy, kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	MODIFY_EACH (OTGrammar)
		my decisionStrategy = decisionStrategy;
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_setLeak, U"OTGrammar: Set leak", nullptr) {
	REAL (leak, U"Leak", U"0.0")
OK
	FIND_ONE (OTGrammar)
		SET_REAL (leak, my leak)
DO
	MODIFY_EACH (OTGrammar)
		my leak = leak;
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_setConstraintPlasticity, U"OTGrammar: Set constraint plasticity", nullptr) {
	NATURAL (constraint, U"Constraint", U"1")
	REAL (plasticity, U"Plasticity", U"1.0")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_setConstraintPlasticity (me, constraint, plasticity);
	MODIFY_EACH_END
}

// MARK: Modify structure

FORM (MODIFY_EACH__OTGrammar_removeConstraint, U"OTGrammar: Remove constraint", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_removeConstraint (me, constraintName);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTGrammar_removeHarmonicallyBoundedCandidates, U"OTGrammar: Remove harmonically bounded candidates", nullptr) {
	BOOLEAN (singly, U"Singly", false)
	OK
DO
	MODIFY_EACH (OTGrammar)
		OTGrammar_removeHarmonicallyBoundedCandidates (me, singly);   // strong exception guarantee (2021-05-05)
	MODIFY_EACH_END
}

// MARK: OTGRAMMAR & STRINGS

FORM (CONVERT_ONE_WEAK_AND_ONE_TO_ONE__OTGrammar_Strings_inputsToOutputs, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_ONE_WEAK_AND_ONE_TO_ONE (OTGrammar, Strings)
		autoStrings result = OTGrammar_inputsToOutputs (me, you, evaluationNoise);
	CONVERT_ONE_WEAK_AND_ONE_TO_ONE_END (my name.get(), U"_out")
}

DIRECT (QUERY_ONE_AND_ONE_FOR_BOOLEAN__OTGrammar_Strings_areAllPartialOutputsGrammatical) {
	QUERY_ONE_AND_ONE_FOR_BOOLEAN (OTGrammar, Strings)
		const bool result = OTGrammar_areAllPartialOutputsGrammatical (me, you);
	QUERY_ONE_AND_ONE_FOR_BOOLEAN_END (result ? U" (all grammatical)" : U" (not all grammatical)")
}

DIRECT (QUERY_ONE_AND_ONE_FOR_BOOLEAN__OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical) {
	QUERY_ONE_AND_ONE_FOR_BOOLEAN (OTGrammar, Strings)
		const bool result = OTGrammar_areAllPartialOutputsSinglyGrammatical (me, you);
	QUERY_ONE_AND_ONE_FOR_BOOLEAN_END (result ? U" (all singly grammatical)" : U" (not all singly grammatical)")
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_TWO__OTGrammar_Stringses_learn, U"OTGrammar: Learn", U"OTGrammar & 2 Strings: Learn...") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OPTIONMENU_ENUM (kOTGrammar_rerankingStrategy, updateRule,
			U"Update rule", kOTGrammar_rerankingStrategy::SYMMETRIC_ALL)
	REAL (plasticity, U"Plasticity", U"0.1")
	REAL (relativePlasticitySpreading, U"Rel. plasticity spreading", U"0.1")
	BOOLEAN (honourLocalRankings, U"Honour local rankings", 1)
	NATURAL (numberOfChews, U"Number of chews", U"1")
	OK
DO
	MODIFY_FIRST_OF_ONE_WEAK_AND_TWO (OTGrammar, Strings)
		OTGrammar_learn (me, you, him, evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticitySpreading, numberOfChews);
	MODIFY_FIRST_OF_ONE_WEAK_AND_TWO_END
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Strings_learnFromPartialOutputs, U"OTGrammar: Learn from partial adult outputs", nullptr) {
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTGrammar, Strings, OTHistory)
		OTGrammar_learnFromPartialOutputs (me, you, evaluationNoise, updateRule, honourLocalRankings,
				plasticity, relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

DIRECT (CONVERT_ONE_AND_TWO_TO_ONE__OTGrammar_Stringses_tabulateAllCorrectRankings) {
	CONVERT_ONE_AND_TWO_TO_ONE (OTGrammar, Strings)
		autoTable result = OTGrammar_tabulateAllCorrectRankings (me, you, him);
	CONVERT_ONE_AND_TWO_TO_ONE_END (my name.get())
}

// MARK: OTGRAMMAR & DISTRIBUTIONS

FORM (QUERY_ONE_WEAK_AND_ONE_FOR_REAL__OTGrammar_Distributions_getFractionCorrect, U"OTGrammar & Distributions: Get fraction correct...", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replications, U"Replications", U"100000")
	OK
DO
	QUERY_ONE_WEAK_AND_ONE_FOR_REAL (OTGrammar, Distributions)
		const double result = OTGrammar_Distributions_getFractionCorrect (me, you, columnNumber,
				evaluationNoise, replications);
	QUERY_ONE_WEAK_AND_ONE_FOR_REAL_END (U" (fraction correct)")
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs,
		U"OTGrammar & Distributions: Learn from partial outputs", U"OT learning 6. Shortcut to grammar learning")
{
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTGrammar, Distributions, OTHistory)
		OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
			updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
			relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, false, false, 0
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_rrip,
		U"OTGrammar & Distributions: Learn from partial outputs (rrip)", U"OT learning 6. Shortcut to grammar learning")
{
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTGrammar, Distributions, OTHistory)
		OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
			updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
			relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 0
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_eip,
		U"OTGrammar & Distributions: Learn from partial outputs (eip)", U"OT learning 6. Shortcut to grammar learning")
{
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTGrammar, Distributions, OTHistory)
		OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
			updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
			relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 1000
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_wrip,
		U"OTGrammar & Distributions: Learn from partial outputs (wrip)", U"OT learning 6. Shortcut to grammar learning")
{
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTGrammar, Distributions, OTHistory)
		OTGrammar_Distributions_learnFromPartialOutputs (me, you, columnNumber, evaluationNoise,
			updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
			relativePlasticitySpreading, numberOfChews, storeHistoryEvery, & history, true, true, 1
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

FORM (INFO_ONE_AND_ONE__OTGrammar_Distributions_listObligatoryRankings, U"OTGrammar & Distributions: Get fraction correct...", nullptr) {
	NATURAL (columnNumber, U"Column number", U"1")
	OK
DO
	INFO_ONE_AND_ONE (OTGrammar, Distributions)
		OTGrammar_Distributions_listObligatoryRankings (me, you, columnNumber);
	INFO_ONE_AND_ONE_END
}

// MARK: OTGRAMMAR & PAIRDISTRIBUTION

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__OTGrammar_PairDistribution_findPositiveWeights,
		U"OTGrammar & PairDistribution: Find positive weights", U"OTGrammar & PairDistribution: Find positive weights...")
{
	POSITIVE (weightFloor, U"Weight floor", U"1.0")
	POSITIVE (marginOfSeparation, U"Margin of separation", U"1.0")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_findPositiveWeights (me, you, weightFloor, marginOfSeparation);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (QUERY_ONE_WEAK_AND_ONE_FOR_REAL__OTGrammar_PairDistribution_getFractionCorrect, U"OTGrammar & PairDistribution: Get fraction correct...", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replications, U"Replications", U"100000")
	OK
DO
	QUERY_ONE_WEAK_AND_ONE_FOR_REAL (OTGrammar, PairDistribution)
		const double result = OTGrammar_PairDistribution_getFractionCorrect (me, you, evaluationNoise, replications);
	QUERY_ONE_WEAK_AND_ONE_FOR_REAL_END (U" (fraction correct)")
}

FORM (QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER__OTGrammar_PairDistribution_getMinimumNumberCorrect, U"OTGrammar & PairDistribution: Get minimum number correct...", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	INTEGER (replicationsPerInput, U"Replications per input", U"1000")
	OK
DO
	QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER (OTGrammar, PairDistribution)
		const integer result = OTGrammar_PairDistribution_getMinimumNumberCorrect (me, you,
				evaluationNoise, replicationsPerInput);
	QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER_END (U" (minimally correct)")
}

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE__OTGrammar_PairDistribution_learn, U"OTGrammar & PairDistribution: Learn", U"OT learning 6. Shortcut to grammar learning") {
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_learn (me, you,
			evaluationNoise, updateRule, honourLocalRankings,
			initialPlasticity, replicationsPerPlasticity,
			plasticityDecrement, numberOfPlasticities, relativePlasticitySpreading, numberOfChews
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_END
}

DIRECT (INFO_ONE_AND_ONE__OTGrammar_PairDistribution_listObligatoryRankings) {
	INFO_ONE_AND_ONE (OTGrammar, PairDistribution)
		OTGrammar_PairDistribution_listObligatoryRankings (me, you);
	INFO_ONE_AND_ONE_END
}

// MARK: - OTMULTI

// MARK: New

FORM (CREATE_ONE__Create_multi_level_metrics_grammar, U"Create multi-level metrics grammar", nullptr) {
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
			overtFormsHaveSecondaryStress, includeClashAndLapse, includeCodas
		);
	CREATE_ONE_END (kOTGrammar_createMetricsGrammar_initialRanking_getText (initialRanking))
}

// MARK: Draw

FORM (GRAPHICS_EACH__OTMulti_drawTableau, U"Draw tableau", U"OT learning") {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	BOOLEAN (showDisharmonies, U"Show disharmonies", true)
	OK
DO
	GRAPHICS_EACH (OTMulti)
		OTMulti_drawTableau (me, GRAPHICS, partialForm1, partialForm2, false, showDisharmonies);
	GRAPHICS_EACH_END
}

FORM (GRAPHICS_EACH__OTMulti_drawTableau_narrowly, U"Draw tableau (narrowly)", U"OT learning") {
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

DIRECT (EDITOR_ONE__OTMulti_viewAndEdit) {
	EDITOR_ONE (an,OTMulti)
		autoOTMultiEditor editor = OTMultiEditor_create (ID_AND_FULL_NAME, me);
	EDITOR_ONE_END
}

// MARK: Query

DIRECT (QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfConstraints) {
	QUERY_ONE_FOR_INTEGER (OTMulti)
		const integer result = my numberOfConstraints;
	QUERY_ONE_FOR_INTEGER_END (U" constraints")
}

FORM (QUERY_ONE_FOR_STRING__OTMulti_getConstraint, U"Get constraint name", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const conststring32 result = my constraints [constraintNumber]. name.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__OTMulti_getConstraintIndexFromName, U"OTMulti: Get constraint number", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTMulti)
		const integer result = OTMulti_getConstraintIndexFromName (me, constraintName);
	QUERY_ONE_FOR_INTEGER_END (U" (index of constraint ", constraintName, U")")
}

FORM (QUERY_ONE_FOR_REAL__OTMulti_getRankingValue, U"Get ranking value", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const double result = my constraints [constraintNumber]. ranking;
	QUERY_ONE_FOR_REAL_END (U" (ranking of constraint ", constraintNumber, U")")
}

FORM (QUERY_ONE_FOR_REAL__OTMulti_getDisharmony, U"Get disharmony", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_REAL (OTMulti)
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const double result = my constraints [constraintNumber]. disharmony;
	QUERY_ONE_FOR_REAL_END (U" (disharmony of constraint ", constraintNumber, U")")
}

DIRECT (QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfCandidates) {
	QUERY_ONE_FOR_INTEGER (OTMulti)
		integer result = my numberOfCandidates;
	QUERY_ONE_FOR_INTEGER_END (U" candidates")
}

FORM (QUERY_ONE_FOR_STRING__OTMulti_getCandidate, U"Get candidate", nullptr) {
	NATURAL (candidateNumber, U"Candidate number", U"1")
	OK
DO
	QUERY_ONE_FOR_STRING (OTMulti)
		if (candidateNumber > my numberOfCandidates)
			Melder_throw (U"Your candidate number should not exceed the number of candidates.");
		const conststring32 result = my candidates [candidateNumber]. string.get();
	QUERY_ONE_FOR_STRING_END
}

FORM (QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfViolations, U"Get number of violations", nullptr) {
	NATURAL (candidateNumber, U"Candidate number", U"1")
	NATURAL (constraintNumber, U"Constraint number", U"1")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTMulti)
		if (candidateNumber > my numberOfCandidates)
			Melder_throw (U"Your candidate number should not exceed the number of candidates.");
		if (constraintNumber > my numberOfConstraints)
			Melder_throw (U"Your constraint number should not exceed the number of constraints.");
		const integer result = my candidates [candidateNumber]. marks [constraintNumber];
	QUERY_ONE_FOR_INTEGER_END (U" violations")
}

FORM (QUERY_ONE_FOR_INTEGER__OTMulti_getWinner, U"OTMulti: Get winner", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	OK
DO
	QUERY_ONE_FOR_INTEGER (OTMulti)
		const integer result = OTMulti_getWinner (me, partialForm1, partialForm2);
	QUERY_ONE_FOR_INTEGER_END (U" (winner)")
}

// MARK: Evaluate

FORM (MODIFY_EACH__OTMulti_evaluate, U"OTMulti: Evaluate", nullptr) {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_newDisharmonies (me, evaluationNoise);
	MODIFY_EACH_END
}

FORM (QUERY_ONE_WEAK_FOR_STRING__OTMulti_generateOptimalForm, U"OTMulti: Generate optimal form", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	QUERY_ONE_WEAK_FOR_STRING (OTMulti)
		autostring32 output = OTMulti_generateOptimalForm (me, partialForm1, partialForm2, evaluationNoise);
		conststring32 result = output.get();
	QUERY_ONE_WEAK_FOR_STRING_END
}

FORM (CONVERT_EACH_WEAK_TO_ONE__OTMulti_generateOptimalForms, U"OTMulti: Generate optimal forms", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	NATURAL (numberOfTrials, U"Number of trials", U"1000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_EACH_WEAK_TO_ONE (OTMulti)
		autoStrings result = OTMulti_generateOptimalForms (me, partialForm1, partialForm2,
				numberOfTrials, evaluationNoise);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

FORM (CONVERT_EACH_WEAK_TO_ONE__OTMulti_to_Distribution, U"OTMulti: Compute output distribution", nullptr) {
	SENTENCE (partialForm1, U"Partial form 1", U"")
	SENTENCE (partialForm2, U"Partial form 2", U"")
	NATURAL (numberOfTrials, U"Number of trials", U"100000")
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_EACH_WEAK_TO_ONE (OTMulti)
		autoDistributions result = OTMulti_to_Distribution (me, partialForm1, partialForm2,
				numberOfTrials, evaluationNoise);
	CONVERT_EACH_WEAK_TO_ONE_END (my name.get(), U"_out")
}

// MARK: Modify ranking

FORM (MODIFY_EACH__OTMulti_setRanking, U"OTMulti: Set ranking", nullptr) {
	NATURAL (constraint, U"Constraint", U"1")
	REAL (ranking, U"Ranking", U"100.0")
	REAL (disharmony, U"Disharmony", U"100.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_setRanking (me, constraint, ranking, disharmony);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTMulti_resetAllRankings, U"OTMulti: Reset all rankings", nullptr) {
	REAL (ranking, U"Ranking", U"100.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_reset (me, ranking);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH_WEAK__OTMulti_learnOne, U"OTMulti: Learn one", nullptr) {
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

FORM (MODIFY_EACH__OTMulti_setDecisionStrategy, U"OTMulti: Set decision strategy", nullptr) {
	CHOICE_ENUM (kOTGrammar_decisionStrategy, decisionStrategy,
			U"Decision strategy", kOTGrammar_decisionStrategy::DEFAULT)
OK
	FIND_ONE (OTMulti)
		SET_ENUM (decisionStrategy, kOTGrammar_decisionStrategy, my decisionStrategy);
DO
	MODIFY_EACH (OTMulti)
		my decisionStrategy = decisionStrategy;
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTMulti_setLeak, U"OTGrammar: Set leak", nullptr) {
	REAL (leak, U"Leak", U"0.0")
OK
	FIND_ONE (OTMulti)
		SET_REAL (leak, my leak)
DO
	MODIFY_EACH (OTMulti)
		my leak = leak;
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__OTMulti_setConstraintPlasticity, U"OTMulti: Set constraint plasticity", nullptr) {
	NATURAL (constraintNumber, U"Constraint number", U"1")
	REAL (plasticity, U"Plasticity", U"1.0")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_setConstraintPlasticity (me, constraintNumber, plasticity);
	MODIFY_EACH_END
}

// MARK: Modify structure

FORM (MODIFY_EACH__OTMulti_removeConstraint, U"OTMulti: Remove constraint", nullptr) {
	SENTENCE (constraintName, U"Constraint name", U"")
	OK
DO
	MODIFY_EACH (OTMulti)
		OTMulti_removeConstraint (me, constraintName);
	MODIFY_EACH_END
}

// MARK: OTMULTI & PAIRDISTRIBUTION

FORM (MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTMulti_PairDistribution_learn, U"OTMulti & PairDistribution: Learn", nullptr) {
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
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY (OTMulti, PairDistribution, Table)
		OTMulti_PairDistribution_learn (me, you, evaluationNoise,
			updateRule, direction,
			initialPlasticity, replicationsPerPlasticity, plasticityDecrement, numberOfPlasticities,
			relativePlasticitySpreading, storeHistoryEvery, & history
		);
	MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY_END
}

// MARK: OTMULTI & STRINGS

FORM (CONVERT_ONE_WEAK_AND_ONE_TO_ONE__OTMulti_Strings_generateOptimalForms, U"OTGrammar: Inputs to outputs", U"OTGrammar: Inputs to outputs...") {
	REAL (evaluationNoise, U"Evaluation noise", U"2.0")
	OK
DO
	CONVERT_ONE_WEAK_AND_ONE_TO_ONE (OTMulti, Strings)
		autoStrings result = OTMulti_Strings_generateOptimalForms (me, you, evaluationNoise);
	CONVERT_ONE_WEAK_AND_ONE_TO_ONE_END (my name.get(), U"_out")
}

// MARK: - NET

// MARK: New

FORM (CREATE_ONE__CreateNetAsDeepBeliefNetwork, U"Create Net as DeepBeliefNetwork", nullptr) {
	WORD (name, U"Name", U"network")
	NATURALVECTOR (numbersOfNodes, U"Numbers of nodes", WHITESPACE_SEPARATED_, U"30 50 20")
	BOOLEAN (inputsAreBinary, U"Inputs are binary", false)
	OK
DO
	CREATE_ONE
		autoNet result = Net_createAsDeepBeliefNet (numbersOfNodes, inputsAreBinary);
	CREATE_ONE_END (name)
}

// MARK: Modify

FORM (MODIFY_EACH__Net_spreadUp, U"Net: Spread up", nullptr) {
	CHOICE_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::STOCHASTIC)
	OK
DO
	MODIFY_EACH (Net)
		Net_spreadUp (me, activationType);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Net_spreadDown, U"Net: Spread down", nullptr) {
	CHOICE_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::DETERMINISTIC)
	OK
DO
	MODIFY_EACH (Net)
		Net_spreadDown (me, activationType);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Net_spreadUp_reconstruction) {
	MODIFY_EACH (Net)
		Net_spreadUp_reconstruction (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Net_spreadDown_reconstruction) {
	MODIFY_EACH (Net)
		Net_spreadDown_reconstruction (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Net_sampleInput) {
	MODIFY_EACH (Net)
		Net_sampleInput (me);
	MODIFY_EACH_END
}

DIRECT (MODIFY_EACH__Net_sampleOutput) {
	MODIFY_EACH (Net)
		Net_sampleOutput (me);
	MODIFY_EACH_END
}

FORM (MODIFY_EACH__Net_update, U"Net: Update", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_EACH (Net)
		Net_update (me, learningRate);
	MODIFY_EACH_END
}

// MARK: Extract

DIRECT (CONVERT_EACH_TO_ONE__Net_extractInputActivities) {
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractInputActivities (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_inputActivities")
}

DIRECT (CONVERT_EACH_TO_ONE__Net_extractOutputActivities) {
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractOutputActivities (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_outputActivities")
}

DIRECT (CONVERT_EACH_TO_ONE__Net_extractInputReconstruction) {
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractInputReconstruction (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_inputReconstruction")
}

DIRECT (CONVERT_EACH_TO_ONE__Net_extractOutputReconstruction) {
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractOutputReconstruction (me);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_outputReconstruction")
}

FORM (CONVERT_EACH_TO_ONE__Net_extractInputBiases, U"Net: Extract input biases", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractInputBiases (me, layerNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_inputBiases")
}

FORM (CONVERT_EACH_TO_ONE__Net_extractOutputBiases, U"Net: Extract output biases", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractOutputBiases (me, layerNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_outputBiases")
}

FORM (CONVERT_EACH_TO_ONE__Net_extractWeights, U"Net: Extract weights", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (Net)
		autoMatrix result = Net_extractWeights (me, layerNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_weights")
}

FORM (QUERY_ONE_FOR_MATRIX__Net_getWeights, U"Net: Get weigths", nullptr) {
	NATURAL (layerNumber, U"Layer number", U"1")
	OK
DO
	QUERY_ONE_FOR_MATRIX (Net)
		autoMAT result = Net_getWeights (me, layerNumber);
	QUERY_ONE_FOR_MATRIX_END
}

// MARK: - NET & PATTERN

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_applyToInput, U"Net & PatternList: Apply to input", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (Net, PatternList)
		Net_PatternList_applyToInput (me, you, rowNumber);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_applyToOutput, U"Net & PatternList: Apply to output", nullptr) {
	NATURAL (rowNumber, U"Row number", U"1")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (Net, PatternList)
		Net_PatternList_applyToOutput (me, you, rowNumber);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learn, U"Net & PatternList: Learn", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (Net, PatternList)
		Net_PatternList_learn (me, you, learningRate);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learnByLayer, U"Net & PatternList: Learn by layer", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (Net, PatternList)
		Net_PatternList_learnByLayer (me, you, learningRate);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learn_twoPhases, U"Net & PatternList: Learn (two phases)", nullptr) {
	POSITIVE (learningRate, U"Learning rate", U"0.001")
	OK
DO
	MODIFY_FIRST_OF_ONE_AND_ONE (Net, PatternList)
		Net_PatternList_learn_twoPhases (me, you, learningRate);
	MODIFY_FIRST_OF_ONE_AND_ONE_END
}

FORM (CONVERT_ONE_AND_ONE_TO_ONE__Net_PatternList_to_ActivationList, U"Net & PatternList: To ActivationList", nullptr) {
	CHOICE_ENUM (kLayer_activationType, activationType,
			U"Activation type", kLayer_activationType::DETERMINISTIC)
	OK
DO
	CONVERT_ONE_AND_ONE_TO_ONE (Net, PatternList)
		autoActivationList result = Net_PatternList_to_ActivationList (me, you, activationType);
	CONVERT_ONE_AND_ONE_TO_ONE_END (my name.get(), U"_", your name.get())
}

// MARK: - NOULLIGRID

// MARK: View & Edit

DIRECT (EDITOR_ONE_WITH_ONE__NoulliGrid_viewAndEdit) {
	EDITOR_ONE_WITH_ONE (a,NoulliGrid, Sound)   // Sound may be null
		autoNoulliGridEditor editor = NoulliGridEditor_create (ID_AND_FULL_NAME, me, you);
	EDITOR_ONE_WITH_ONE_END
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__NoulliGrid_getAverageProbabilities, U"NoulliGrid: Get average probabilities", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	REAL (fromTime, U"From time (s)", U"0")
	REAL (toTime, U"To time (s)", U"10.0")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (NoulliGrid)
		autoVEC result = NoulliGrid_getAverageProbabilities (me, tierNumber, fromTime, toTime);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (GRAPHICS_NoulliGrid_paint, U"NoulliGrid: Paint", U"NoulliGrid: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (NoulliGrid)
		NoulliGrid_paint (me, GRAPHICS, fromTime, toTime, garnish);
	GRAPHICS_EACH_END
}

// MARK: - CUBEGRID

// MARK: View & Edit

DIRECT (EDITOR_ONE_WITH_ONE__CubeGrid_viewAndEdit) {
	EDITOR_ONE_WITH_ONE (a,CubeGrid, Sound)   // Sound may be null
		autoCubeGridEditor editor = CubeGridEditor_create (ID_AND_FULL_NAME, me, you);
	EDITOR_ONE_WITH_ONE_END
}

FORM (QUERY_ONE_FOR_REAL_VECTOR__CubeGrid_getAverage, U"CubeGrid: Get average", nullptr) {
	NATURAL (tierNumber, U"Tier number", U"1")
	REAL (fromTime, U"From time (s)", U"0")
	REAL (toTime, U"To time (s)", U"10.0")
	OK
DO
	QUERY_ONE_FOR_REAL_VECTOR (CubeGrid)
		autoVEC result = CubeGrid_getAverages (me, tierNumber, fromTime, toTime);
	QUERY_ONE_FOR_REAL_VECTOR_END
}

FORM (GRAPHICS_CubeGrid_paint, U"CubeGrid: Paint", U"CubeGrid: Paint...") {
	praat_TimeFunction_RANGE (fromTime, toTime)
	BOOLEAN (garnish, U"Garnish", 1)
	OK
DO
	GRAPHICS_EACH (CubeGrid)
		CubeGrid_paint (me, GRAPHICS, fromTime, toTime, garnish);
	GRAPHICS_EACH_END
}

// MARK: - buttons

void praat_uvafon_gram_init ();
void praat_uvafon_gram_init () {
	Thing_recognizeClassesByName (classNetwork,
		classOTGrammar, classOTHistory, classOTMulti,
		classRBMLayer, classFullyConnectedLayer, classNet,
		classNoulliTier, classNoulliGrid, classCubeGrid,
		nullptr
	);
	Thing_recognizeClassByOtherName (classOTGrammar, U"OTCase");

	structNoulliGridEditor :: f_preferences ();
	structCubeGridEditor :: f_preferences ();

	praat_addMenuCommand (U"Objects", U"New", U"Constraint grammars", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"OT learning tutorial", nullptr, GuiMenu_DEPTH_1 | GuiMenu_NO_API,
				HELP__OT_learning_tutorial);
		praat_addMenuCommand (U"Objects", U"New", U"-- tableau grammars --", nullptr, 1, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create NoCoda grammar", nullptr, 1,
				CREATE_ONE__Create_NoCoda_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation grammar", nullptr, 1,
				CREATE_ONE__Create_NPA_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create place assimilation distribution", nullptr, 1,
				CREATE_ONE__Create_NPA_distribution);
		praat_addMenuCommand (U"Objects", U"New", U"Create tongue-root grammar...", nullptr, 1,
				CREATE_ONE__Create_tongue_root_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create metrics grammar...", nullptr, 1,
				CREATE_ONE__Create_metrics_grammar);
		praat_addMenuCommand (U"Objects", U"New", U"Create multi-level metrics grammar...", nullptr, 1,
				CREATE_ONE__Create_multi_level_metrics_grammar);
	praat_addAction1 (classOTGrammar, 1, U"Save as headerless spreadsheet file... || Write to headerless spreadsheet file...", nullptr, 0,
			SAVE_ONE__OTGrammar_writeToHeaderlessSpreadsheetFile);   // alternative COMPATIBILITY <= 2011

	praat_addAction1 (classOTGrammar, 0, U"OTGrammar help", nullptr, 0,
			HELP__OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, U"View & Edit || Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE__OTGrammar_viewAndEdit);
	praat_addAction1 (classOTGrammar, 0, U"Draw", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau...", nullptr, 0,
				GRAPHICS_EACH__OTGrammar_drawTableau);
		praat_addAction1 (classOTGrammar, 0, U"Draw tableau (narrowly)...", nullptr, 0,
				GRAPHICS_EACH__OTGrammar_drawTableau_narrowly);
	praat_addAction1 (classOTGrammar, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get number of constraints", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfConstraints);
		praat_addAction1 (classOTGrammar, 1, U"Get constraint...", nullptr, 1,
				QUERY_ONE_FOR_STRING__OTGrammar_getConstraint);
		praat_addAction1 (classOTGrammar, 1, U"Get ranking value...", nullptr, 1,
				QUERY_ONE_FOR_REAL__OTGrammar_getRankingValue);
		praat_addAction1 (classOTGrammar, 1, U"Get disharmony...", nullptr, 1,
				QUERY_ONE_FOR_REAL__OTGrammar_getDisharmony);
		praat_addAction1 (classOTGrammar, 1, U"Get number of tableaus", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfTableaus);
		praat_addAction1 (classOTGrammar, 1, U"Get input...", nullptr, 1,
				QUERY_ONE_FOR_STRING__OTGrammar_getInput);
		praat_addAction1 (classOTGrammar, 1, U"Get number of candidates...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get candidate...", nullptr, 1,
				QUERY_ONE_FOR_STRING__OTGrammar_getCandidate);
		praat_addAction1 (classOTGrammar, 1, U"Get number of violations...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfViolations);
		praat_addAction1 (classOTGrammar, 1, U"Are all outputs distinguishable?", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__OTGrammar_areAllOutputsDistinguishable);
		praat_addAction1 (classOTGrammar, 1, U"Tabulate equivalent pairs", nullptr, 1,
				CONVERT_EACH_TO_ONE_OTGrammar_tabulateEquivalentPairs);
		praat_addAction1 (classOTGrammar, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Get winner...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getWinner);
		praat_addAction1 (classOTGrammar, 1, U"Compare candidates...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_compareCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Get number of optimal candidates...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTGrammar_getNumberOfOptimalCandidates);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate grammatical...", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__OTGrammar_isCandidateGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is candidate singly grammatical...", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__OTGrammar_isCandidateSinglyGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Get interpretive parse...", nullptr, 1,
				INFO_ONE__OTGrammar_getInterpretiveParse);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output grammatical...", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__OTGrammar_isPartialOutputGrammatical);
		praat_addAction1 (classOTGrammar, 1, U"Is partial output singly grammatical...", nullptr, 1,
				QUERY_ONE_FOR_BOOLEAN__OTGrammar_isPartialOutputSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 0, U"Generate inputs...", nullptr, 0,
			CONVERT_EACH_TO_ONE__OTGrammar_generateInputs);
	praat_addAction1 (classOTGrammar, 0, U"Get inputs", nullptr, 0,
			CONVERT_EACH_TO_ONE__OTGrammar_getInputs);
	praat_addAction1 (classOTGrammar, 0, U"Measure typology", nullptr, 0,
			CONVERT_EACH_WEAK_TO_ONE__OTGrammar_measureTypology);
	praat_addAction1 (classOTGrammar, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Evaluate...", nullptr, 0,
				MODIFY_EACH__OTGrammar_evaluate);
		praat_addAction1 (classOTGrammar, 0, U"Input to output...", nullptr, 0,
				QUERY_ONE_WEAK_FOR_STRING__OTGrammar_inputToOutput);
		praat_addAction1 (classOTGrammar, 0, U"Input to outputs...", nullptr, 0,
				CONVERT_EACH_WEAK_TO_ONE__OTGrammar_inputToOutputs);
		praat_addAction1 (classOTGrammar, 0, U"To output Distributions...", nullptr, 0,
				CONVERT_EACH_WEAK_TO_ONE__OTGrammar_to_Distributions);
		praat_addAction1 (classOTGrammar, 0, U"To PairDistribution...", nullptr, 0,
				CONVERT_EACH_WEAK_TO_ONE__OTGrammar_to_PairDistribution);
	praat_addAction1 (classOTGrammar, 0, U"Modify ranking -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Set ranking...", nullptr, 1,
				MODIFY_EACH__OTGrammar_setRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset all rankings...", nullptr, 1,
				MODIFY_EACH__OTGrammar_resetAllRankings);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random ranking...", nullptr, 1,
				MODIFY_EACH__OTGrammar_resetToRandomRanking);
		praat_addAction1 (classOTGrammar, 0, U"Reset to random total ranking...", nullptr, 1,
				MODIFY_EACH__OTGrammar_resetToRandomTotalRanking);
		praat_addAction1 (classOTGrammar, 0, U"Learn one...", nullptr, 1,
				MODIFY_EACH_WEAK__OTGrammar_learnOne);
		praat_addAction1 (classOTGrammar, 0, U"Learn one from partial output...", nullptr, 1,
				MODIFY_EACH_WEAK__OTGrammar_learnOneFromPartialOutput);
	praat_addAction1 (classOTGrammar, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 1, U"Set decision strategy... || Set harmony computation method...",
				nullptr, 1, MODIFY_EACH__OTGrammar_setDecisionStrategy);
		praat_addAction1 (classOTGrammar, 1, U"Set leak...", nullptr, 1,
				MODIFY_EACH__OTGrammar_setLeak);
		praat_addAction1 (classOTGrammar, 1, U"Set constraint plasticity...", nullptr, 1,
				MODIFY_EACH__OTGrammar_setConstraintPlasticity);
	praat_addAction1 (classOTGrammar, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTGrammar, 0, U"Remove constraint...", nullptr, 1,
				MODIFY_EACH__OTGrammar_removeConstraint);
		praat_addAction1 (classOTGrammar, 0, U"Remove harmonically bounded candidates...", nullptr, 1,
				MODIFY_EACH__OTGrammar_removeHarmonicallyBoundedCandidates);

	praat_TableOfReal_init (classOTHistory);

	praat_addAction1 (classOTMulti, 0, U"View & Edit || Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE__OTMulti_viewAndEdit);
	praat_addAction1 (classOTMulti, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau...", nullptr, 1,
				GRAPHICS_EACH__OTMulti_drawTableau);
		praat_addAction1 (classOTMulti, 0, U"Draw tableau (narrowly)...", nullptr, 1,
				GRAPHICS_EACH__OTMulti_drawTableau_narrowly);
	praat_addAction1 (classOTMulti, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get number of constraints", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfConstraints);
		praat_addAction1 (classOTMulti, 1, U"Get constraint...", nullptr, 1,
				QUERY_ONE_FOR_STRING__OTMulti_getConstraint);
		praat_addAction1 (classOTMulti, 1, U"Get constraint number...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTMulti_getConstraintIndexFromName);
		praat_addAction1 (classOTMulti, 1, U"Get ranking value...", nullptr, 1,
				QUERY_ONE_FOR_REAL__OTMulti_getRankingValue);
		praat_addAction1 (classOTMulti, 1, U"Get disharmony...", nullptr, 1,
				QUERY_ONE_FOR_REAL__OTMulti_getDisharmony);
		praat_addAction1 (classOTMulti, 1, U"Get number of candidates", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfCandidates);
		praat_addAction1 (classOTMulti, 1, U"Get candidate...", nullptr, 1,
				QUERY_ONE_FOR_STRING__OTMulti_getCandidate);
		praat_addAction1 (classOTMulti, 1, U"Get number of violations...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTMulti_getNumberOfViolations);
		praat_addAction1 (classOTMulti, 1, U"-- parse --", nullptr, 1, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Get winner...", nullptr, 1,
				QUERY_ONE_FOR_INTEGER__OTMulti_getWinner);
	praat_addAction1 (classOTMulti, 0, U"Evaluate", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Evaluate...", nullptr, 1,
				MODIFY_EACH__OTMulti_evaluate);
		praat_addAction1 (classOTMulti, 0, U"Get output...", nullptr, 1,
				QUERY_ONE_WEAK_FOR_STRING__OTMulti_generateOptimalForm);
		praat_addAction1 (classOTMulti, 0, U"Get outputs...", nullptr, 1,
				CONVERT_EACH_WEAK_TO_ONE__OTMulti_generateOptimalForms);
		praat_addAction1 (classOTMulti, 0, U"To output Distribution...", nullptr, 1,
				CONVERT_EACH_WEAK_TO_ONE__OTMulti_to_Distribution);
	praat_addAction1 (classOTMulti, 0, U"Modify ranking", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Set ranking...", nullptr, 0,
				MODIFY_EACH__OTMulti_setRanking);
		praat_addAction1 (classOTMulti, 0, U"Reset all rankings...", nullptr, 0,
				MODIFY_EACH__OTMulti_resetAllRankings);
		praat_addAction1 (classOTMulti, 0, U"Learn one...", nullptr, 0,
				MODIFY_EACH_WEAK__OTMulti_learnOne);
	praat_addAction1 (classOTMulti, 0, U"Modify behaviour -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 1, U"Set decision strategy...", nullptr, 1,
				MODIFY_EACH__OTMulti_setDecisionStrategy);
		praat_addAction1 (classOTMulti, 1, U"Set leak...", nullptr, 1,
				MODIFY_EACH__OTMulti_setLeak);
		praat_addAction1 (classOTMulti, 1, U"Set constraint plasticity...", nullptr, 1,
				MODIFY_EACH__OTMulti_setConstraintPlasticity);
	praat_addAction1 (classOTMulti, 0, U"Modify structure -", nullptr, 0, nullptr);
		praat_addAction1 (classOTMulti, 0, U"Remove constraint...", nullptr, 1,
				MODIFY_EACH__OTMulti_removeConstraint);

	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Query -", nullptr, 0, nullptr);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs grammatical?", nullptr, 1,
				QUERY_ONE_AND_ONE_FOR_BOOLEAN__OTGrammar_Strings_areAllPartialOutputsGrammatical);
		praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Are all partial outputs singly grammatical?", nullptr, 1,
				QUERY_ONE_AND_ONE_FOR_BOOLEAN__OTGrammar_Strings_areAllPartialOutputsSinglyGrammatical);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Inputs to outputs...", nullptr, 0,
			CONVERT_ONE_WEAK_AND_ONE_TO_ONE__OTGrammar_Strings_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, U"Learn from partial outputs...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Strings_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_TWO__OTGrammar_Stringses_learn);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, U"Tabulate all correct rankings", nullptr, 0,
			CONVERT_ONE_AND_TWO_TO_ONE__OTGrammar_Stringses_tabulateAllCorrectRankings);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (rrip)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_rrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (eip)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_eip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Learn from partial outputs (wrip)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTGrammar_Distributions_learnFromPartialOutputs_wrip);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"Get fraction correct...", nullptr, 0,
			QUERY_ONE_WEAK_AND_ONE_FOR_REAL__OTGrammar_Distributions_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, U"List obligatory rankings...", nullptr, GuiMenu_HIDDEN,
			INFO_ONE_AND_ONE__OTGrammar_Distributions_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE__OTGrammar_PairDistribution_learn);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Find positive weights...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__OTGrammar_PairDistribution_findPositiveWeights);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get fraction correct...", nullptr, 0,
			QUERY_ONE_WEAK_AND_ONE_FOR_REAL__OTGrammar_PairDistribution_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"Get minimum number correct...", nullptr, 0,
			QUERY_ONE_WEAK_AND_ONE_FOR_INTEGER__OTGrammar_PairDistribution_getMinimumNumberCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, U"List obligatory rankings", nullptr, 0,
			INFO_ONE_AND_ONE__OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_WEAK_AND_ONE_WITH_HISTORY__OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, U"Get outputs...", nullptr, 0,
			CONVERT_ONE_WEAK_AND_ONE_TO_ONE__OTMulti_Strings_generateOptimalForms);

	praat_addMenuCommand (U"Objects", U"New", U"Symmetric neural networks", nullptr, 0, nullptr);
		praat_addMenuCommand (U"Objects", U"New", U"Create empty Network...", nullptr, 1,
				CREATE_ONE__Create_empty_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network...", nullptr, 1,
				CREATE_ONE__Create_rectangular_Network);
		praat_addMenuCommand (U"Objects", U"New", U"Create rectangular Network (vertical)...", nullptr, 1,
				CREATE_ONE__Create_rectangular_Network_vertical);
		praat_addMenuCommand (U"Objects", U"New", U"Create Net as deep belief network...", nullptr, 1,
				CREATE_ONE__CreateNetAsDeepBeliefNetwork);

	praat_addAction1 (classNetwork, 0, U"Draw...", nullptr, 0,
			GRAPHICS_EACH__Network_draw);
	praat_addAction1 (classNetwork, 1, U"Tabulate -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"List nodes...", nullptr, 1,
				INFO_ONE__Network_listNodes);
		praat_addAction1 (classNetwork, 1, U"Nodes down to table...", nullptr, 1,
				CONVERT_EACH_TO_ONE__Network_nodes_downto_Table);
	praat_addAction1 (classNetwork, 0, U"Query -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 1, U"Get activity...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Network_getActivity);
		praat_addAction1 (classNetwork, 1, U"Get activities...", nullptr, 1,
				QUERY_ONE_FOR_REAL_VECTOR__Network_getActivities);
		praat_addAction1 (classNetwork, 1, U"Get weight...", nullptr, 1,
				QUERY_ONE_FOR_REAL__Network_getWeight);
	praat_addAction1 (classNetwork, 0, U"Modify -", nullptr, 0, nullptr);
		praat_addAction1 (classNetwork, 0, U"Add node...", nullptr, 1,
				MODIFY_EACH__Network_addNode);
		praat_addAction1 (classNetwork, 0, U"Add connection...", nullptr, 1,
				MODIFY_EACH__Network_addConnection);
		praat_addAction1 (classNetwork, 0, U"-- activity --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set activity...", nullptr, 1,
				MODIFY_EACH__Network_setActivity);
		praat_addAction1 (classNetwork, 0, U"Set clamping...", nullptr, 1,
				MODIFY_EACH__Network_setClamping);
		praat_addAction1 (classNetwork, 0, U"Zero activities...", nullptr, 1,
				MODIFY_EACH__Network_zeroActivities);
		praat_addAction1 (classNetwork, 0, U"Normalize activities...", nullptr, 1,
				MODIFY_EACH__Network_normalizeActivities);
		praat_addAction1 (classNetwork, 0, U"Formula (activities)...", nullptr, 1,
				MODIFY_EACH_WEAK__Network_formula_activities);
		praat_addAction1 (classNetwork, 0, U"Spread activities...", nullptr, 1,
				MODIFY_EACH__Network_spreadActivities);
		praat_addAction1 (classNetwork, 0, U"Set activity clipping rule...", nullptr, 1,
				MODIFY_EACH__Network_setActivityClippingRule);
		praat_addAction1 (classNetwork, 0, U"Set activity leak...", nullptr, 1,
				MODIFY_EACH__Network_setActivityLeak);
		praat_addAction1 (classNetwork, 0, U"Set shunting...", nullptr, 1,
				MODIFY_EACH__Network_setShunting);
		praat_addAction1 (classNetwork, 0, U"-- weight --", nullptr, 1, nullptr);
		praat_addAction1 (classNetwork, 0, U"Set weight...", nullptr, 1,
				MODIFY_EACH__Network_setWeight);
		praat_addAction1 (classNetwork, 0, U"Update weights", nullptr, 1,
				MODIFY_EACH__Network_updateWeights);
		praat_addAction1 (classNetwork, 0, U"Normalize weights...", nullptr, 1,
				MODIFY_EACH__Network_normalizeWeights);
		praat_addAction1 (classNetwork, 0, U"Set instar...", nullptr, 1,
				MODIFY_EACH__Network_setInstar);
		praat_addAction1 (classNetwork, 0, U"Set outstar...", nullptr, 1,
				MODIFY_EACH__Network_setOutstar);
		praat_addAction1 (classNetwork, 0, U"Set weight leak...", nullptr, 1,
				MODIFY_EACH__Network_setWeightLeak);

	praat_addAction1 (classNet, 0, U"Query", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Get weights...", nullptr, 0,
				QUERY_ONE_FOR_MATRIX__Net_getWeights);
	praat_addAction1 (classNet, 0, U"Modify", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Spread up...", nullptr, 0,
				MODIFY_EACH__Net_spreadUp);
		praat_addAction1 (classNet, 0, U"Spread down...", nullptr, 0,
				MODIFY_EACH__Net_spreadDown);
		praat_addAction1 (classNet, 0, U"Spread up (reconstruction)", nullptr, 0,
				MODIFY_EACH__Net_spreadUp_reconstruction);
		praat_addAction1 (classNet, 0, U"Spread down (reconstruction)", nullptr, 0,
				MODIFY_EACH__Net_spreadDown_reconstruction);
		praat_addAction1 (classNet, 0, U"Sample input", nullptr, 0,
				MODIFY_EACH__Net_sampleInput);
		praat_addAction1 (classNet, 0, U"Sample output", nullptr, 0,
				MODIFY_EACH__Net_sampleOutput);
		praat_addAction1 (classNet, 0, U"Update...", nullptr, 0,
				MODIFY_EACH__Net_update);
	praat_addAction1 (classNet, 0, U"Extract", nullptr, 0, nullptr);
		praat_addAction1 (classNet, 0, U"Extract input activities", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractInputActivities);
		praat_addAction1 (classNet, 0, U"Extract output activities", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractOutputActivities);
		praat_addAction1 (classNet, 0, U"Extract input reconstruction", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractInputReconstruction);
		praat_addAction1 (classNet, 0, U"Extract output reconstruction", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractOutputReconstruction);
		praat_addAction1 (classNet, 0, U"Extract input biases...", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractInputBiases);
		praat_addAction1 (classNet, 0, U"Extract output biases...", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractOutputBiases);
		praat_addAction1 (classNet, 0, U"Extract weights...", nullptr, 0,
				CONVERT_EACH_TO_ONE__Net_extractWeights);

	praat_addAction2 (classNet, 1, classPatternList, 1, U"Apply to input...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_applyToInput);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Apply to output...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_applyToOutput);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learn);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn by layer...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learnByLayer);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"Learn (two phases)...", nullptr, 0,
			MODIFY_FIRST_OF_ONE_AND_ONE__Net_PatternList_learn_twoPhases);
	praat_addAction2 (classNet, 1, classPatternList, 1, U"To ActivationList", nullptr, 0,
			CONVERT_ONE_AND_ONE_TO_ONE__Net_PatternList_to_ActivationList);

	praat_addAction1 (classNoulliGrid, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE_WITH_ONE__NoulliGrid_viewAndEdit);
	praat_addAction1 (classNoulliGrid, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classNoulliGrid);
		praat_addAction1 (classNoulliGrid, 1, U"Get average probabilities...", nullptr, 1,
			QUERY_ONE_FOR_REAL_VECTOR__NoulliGrid_getAverageProbabilities);
	praat_addAction1 (classNoulliGrid, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classNoulliGrid, 0, U"Paint...", nullptr, 1, GRAPHICS_NoulliGrid_paint);
	praat_addAction2 (classNoulliGrid, 1, classSound, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE_WITH_ONE__NoulliGrid_viewAndEdit);

	praat_addAction1 (classCubeGrid, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE_WITH_ONE__CubeGrid_viewAndEdit);
	praat_addAction1 (classCubeGrid, 0, U"Query -", nullptr, 0, nullptr);
		praat_TimeFunction_query_init (classCubeGrid);
		praat_addAction1 (classCubeGrid, 1, U"Get average...", nullptr, 1,
			QUERY_ONE_FOR_REAL_VECTOR__CubeGrid_getAverage);
	praat_addAction1 (classCubeGrid, 0, U"Draw -", nullptr, 0, nullptr);
		praat_addAction1 (classCubeGrid, 0, U"Paint...", nullptr, 1, GRAPHICS_CubeGrid_paint);
	praat_addAction2 (classCubeGrid, 1, classSound, 1, U"View & Edit", nullptr, GuiMenu_ATTRACTIVE,
			EDITOR_ONE_WITH_ONE__CubeGrid_viewAndEdit);
}

/* End of file praat_gram.cpp */
