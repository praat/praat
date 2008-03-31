/* praat_OT.c
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
 * pb 2008/03/07
 */

#include "praat.h"

#include "OTGrammar.h"
#include "OTMulti.h"
#include "OTGrammarEditor.h"
#include "OTMultiEditor.h"

/***** HELP *****/

DIRECT (OT_learning_tutorial) Melder_help (L"OT learning"); END

DIRECT (OTGrammar_help) Melder_help (L"OTGrammar"); END

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
	if (! praat_new1 (OTGrammar_create_metrics (GET_INTEGER (L"Initial ranking"), GET_INTEGER (L"Trochaicity constraint"),
		GET_INTEGER (L"Include FootBimoraic"), GET_INTEGER (L"Include FootBisyllabic"),
		GET_INTEGER (L"Include Peripheral"), GET_INTEGER (L"Nonfinality constraint"),
		GET_INTEGER (L"Overt forms have secondary stress"), GET_INTEGER (L"Include *Clash and *Lapse"), GET_INTEGER (L"Include codas")),
		GET_STRING (L"Initial ranking"))) return 0;
END

DIRECT (Create_NoCoda_grammar)
	if (! praat_new1 (OTGrammar_create_NoCoda_grammar (), L"NoCoda")) return 0;
END

DIRECT (Create_NPA_grammar)
	if (! praat_new1 (OTGrammar_create_NPA_grammar (), L"assimilation")) return 0;
END

DIRECT (Create_NPA_distribution)
	if (! praat_new1 (OTGrammar_create_NPA_distribution (), L"assimilation")) return 0;
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
	if (! praat_new1 (OTGrammar_create_tongueRoot_grammar (GET_INTEGER (L"Constraint set"),
		GET_INTEGER (L"Ranking")), GET_STRING (L"Ranking"))) return 0;
END

FORM (OTGrammar_drawTableau, L"Draw tableau", L"OT learning")
	SENTENCE (L"Input string", L"")
	OK
DO
	EVERY_DRAW (OTGrammar_drawTableau (OBJECT, GRAPHICS, GET_STRING (L"Input string")))
END

DIRECT (OTGrammar_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit from batch.");
	} else {
		WHERE (SELECTED) {
			if (! praat_installEditor (OTGrammarEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT), IOBJECT)) return 0;
		}
	}
END

FORM (OTGrammar_evaluate, L"OTGrammar: Evaluate", 0)
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	OTGrammar_newDisharmonies (ONLY_OBJECT, GET_REAL (L"Evaluation noise"));
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_generateInputs, L"Generate inputs", L"OTGrammar: Generate inputs...")
	NATURAL (L"Number of trials", L"1000")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new2 (OTGrammar_generateInputs (OBJECT, GET_INTEGER (L"Number of trials")), NAMEW, L"_in");
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTGrammar_getCandidate, L"Get candidate", 0)
	NATURAL (L"Tableau", L"1")
	NATURAL (L"Candidate", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	OTGrammarTableau tableau;
	long itab = GET_INTEGER (L"Tableau"), icand = GET_INTEGER (L"Candidate");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	tableau = & my tableaus [itab];
	REQUIRE (icand <= tableau -> numberOfCandidates, L"'Candidate' should not exceed number of candidates.")
	Melder_information1 (tableau -> candidates [icand]. output);
END

FORM (OTGrammar_getConstraint, L"Get constraint name", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (my constraints [icons]. name);
END

FORM (OTGrammar_getDisharmony, L"Get disharmony", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. disharmony));
END

FORM (OTGrammar_getInput, L"Get input", 0)
	NATURAL (L"Tableau", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	Melder_information1 (my tableaus [itab]. input);
END

DIRECT (OTGrammar_getInputs)
	WHERE (SELECTED) {
		if (! praat_new2 (OTGrammar_getInputs (OBJECT), NAMEW, L"_in")) return 0;
	}
END

FORM (OTGrammar_getInterpretiveParse, L"OTGrammar: Interpretive parse", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long bestInput, bestOutput;
	if (! OTGrammar_getInterpretiveParse (me, GET_STRING (L"Partial output"), & bestInput, & bestOutput)) return 0;
	Melder_information8 (L"Best input = ", Melder_integer (bestInput), L": ", my tableaus [bestInput]. input,
		L"\nBest output = ", Melder_integer (bestOutput), L": ", my tableaus [bestInput]. candidates [bestOutput]. output);
END

FORM (OTGrammar_getNumberOfCandidates, L"Get number of candidates", 0)
	NATURAL (L"Tableau", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (my tableaus [itab]. numberOfCandidates));
END

DIRECT (OTGrammar_getNumberOfConstraints)
	OTGrammar me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfConstraints));
END

FORM (OTGrammar_getNumberOfOptimalCandidates, L"Get number of optimal candidates", 0)
	NATURAL (L"Tableau", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (OTGrammar_getNumberOfOptimalCandidates (me, itab)));
END

DIRECT (OTGrammar_getNumberOfTableaus)
	OTGrammar me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfTableaus));
END

FORM (OTGrammar_getNumberOfViolations, L"Get number of violations", 0)
	NATURAL (L"Tableau number", L"1")
	NATURAL (L"Candidate number", L"1")
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau number"), icand = GET_INTEGER (L"Candidate number"), icons = GET_INTEGER (L"Constraint number");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau number' should not exceed number of tableaus.")
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, L"'Candidate number' should not exceed number of candidates for this tableau.")
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_integer (my tableaus [itab]. candidates [icand]. marks [icons]));
END

FORM (OTGrammar_getRankingValue, L"Get ranking value", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. ranking));
END

FORM (OTGrammar_getWinner, L"Get winner", 0)
	NATURAL (L"Tableau", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau");
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (OTGrammar_getWinner (me, itab)));
END

FORM (OTGrammar_inputToOutput, L"OTGrammar: Input to output", L"OTGrammar: Input to output...")
	SENTENCE (L"Input form", L"")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	wchar_t output [100];
	if (! OTGrammar_inputToOutput (ONLY_OBJECT, GET_STRING (L"Input form"), output, GET_REAL (L"Evaluation noise"))) return 0;
	Melder_information1 (output);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_inputToOutputs, L"OTGrammar: Input to outputs", L"OTGrammar: Input to outputs...")
	NATURAL (L"Trials", L"1000")
	REAL (L"Evaluation noise", L"2.0")
	SENTENCE (L"Input form", L"")
	OK
DO
	OTGrammar ot = ONLY (classOTGrammar);
	if (! praat_new2 (OTGrammar_inputToOutputs (ot, GET_STRING (L"Input form"), GET_INTEGER (L"Trials"), GET_REAL (L"Evaluation noise")),
		ot -> name, L"_out")) return 0;
	praat_dataChanged (ot);
END

FORM (OTGrammar_inputsToOutputs, L"OTGrammar: Inputs to outputs", L"OTGrammar: Inputs to outputs...")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	OTGrammar ot = ONLY (classOTGrammar);
	if (! praat_new2 (OTGrammar_inputsToOutputs (ot, ONLY (classStrings), GET_REAL (L"Evaluation noise")),
		ot -> name, L"_out")) return 0;
	praat_dataChanged (ot);
END

FORM (OTGrammar_isCandidateGrammatical, L"Is candidate grammatical?", 0)
	NATURAL (L"Tableau", L"1")
	NATURAL (L"Candidate", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau"), icand;
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	icand = GET_INTEGER (L"Candidate");
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, L"'Candidate' should not exceed number of candidates.")
	Melder_information1 (Melder_integer (OTGrammar_isCandidateGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isCandidateSinglyGrammatical, L"Is candidate singly grammatical?", 0)
	NATURAL (L"Tableau", L"1")
	NATURAL (L"Candidate", L"1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER (L"Tableau"), icand;
	REQUIRE (itab <= my numberOfTableaus, L"'Tableau' should not exceed number of tableaus.")
	icand = GET_INTEGER (L"Candidate");
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, L"'Candidate' should not exceed number of candidates.")
	Melder_information1 (Melder_integer (OTGrammar_isCandidateSinglyGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isPartialOutputGrammatical, L"Is partial output grammatical?", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	Melder_information1 (Melder_integer (OTGrammar_isPartialOutputGrammatical (ONLY_OBJECT, GET_STRING (L"Partial output"))));
END

FORM (OTGrammar_isPartialOutputSinglyGrammatical, L"Is partial output singly grammatical?", 0)
	SENTENCE (L"Partial output", L"")
	OK
DO
	Melder_information1 (Melder_integer (OTGrammar_isPartialOutputSinglyGrammatical (ONLY_OBJECT, GET_STRING (L"Partial output"))));
END

FORM (OTGrammar_learn, L"OTGrammar: Learn", L"OTGrammar & 2 Strings: Learn...")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	Strings inputs = NULL, outputs = NULL;
	WHERE (SELECTED && CLASS == classStrings) { if (! inputs) inputs = OBJECT; else outputs = OBJECT; }
	OTGrammar_learn (grammar, inputs, outputs,
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"));
	praat_dataChanged (grammar);
	iferror return 0;
END

FORM (OTGrammar_learnFromPartialOutputs, L"OTGrammar: Learn from partial adult outputs", 0)
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	INTEGER (L"Store history every", L"0")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTHistory history = NULL;
	OTGrammar_learnFromPartialOutputs (grammar, ONLY (classStrings),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"),
		GET_INTEGER (L"Store history every"), & history);
	praat_dataChanged (grammar);
	if (history) praat_new1 (history, grammar -> name);
	iferror {
		if (history) praat_updateSelection ();
		return 0;
	}
END

FORM (OTGrammar_learnOne, L"OTGrammar: Learn one", L"OTGrammar: Learn one...")
	SENTENCE (L"Input string", L"")
	SENTENCE (L"Output string", L"")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_learnOne (OBJECT, GET_STRING (L"Input string"), GET_STRING (L"Output string"),
			GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), TRUE, TRUE, NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTGrammar_learnOneFromPartialOutput, L"OTGrammar: Learn one from partial adult output", 0)
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
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
	REAL (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_learnOneFromPartialOutput (OBJECT, GET_STRING (L"Partial output"),
			GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
			GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"), TRUE);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTGrammar_removeConstraint, L"OTGrammar: Remove constraint", 0)
	SENTENCE (L"Constraint name", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_removeConstraint (OBJECT, GET_STRING (L"Constraint name"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_removeHarmonicallyBoundedCandidates, L"OTGrammar: Remove harmonically bounded candidates", 0)
	BOOLEAN (L"Singly", 0)
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_removeHarmonicallyBoundedCandidates (OBJECT, GET_INTEGER (L"Singly"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_resetAllRankings, L"OTGrammar: Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_reset (OBJECT, GET_REAL (L"Ranking"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_resetToRandomTotalRanking, L"OTGrammar: Reset to random total ranking", 0)
	REAL (L"Maximum ranking", L"100.0")
	POSITIVE (L"Ranking distance", L"1.0")
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_resetToRandomTotalRanking (OBJECT, GET_REAL (L"Maximum ranking"), GET_REAL (L"Ranking distance"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_setConstraintPlasticity, L"OTGrammar: Set constraint plasticity", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Plasticity", L"1.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_setConstraintPlasticity (OBJECT, GET_INTEGER (L"Constraint"), GET_REAL (L"Plasticity"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_setDecisionStrategy, L"OTGrammar: Set decision strategy", 0)
	ENUM (L"Decision strategy", OTGrammar_DECISION_STRATEGY, 0)
	OK
OTGrammar me = ONLY_OBJECT;
SET_INTEGER (L"Decision strategy", my decisionStrategy);
DO
	OTGrammar me = ONLY_OBJECT;
	my decisionStrategy = GET_INTEGER (L"Decision strategy");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_setLeak, L"OTGrammar: Set leak", 0)
	REAL (L"Leak", L"0.0")
	OK
OTGrammar me = ONLY_OBJECT;
SET_REAL (L"Leak", my leak);
DO
	OTGrammar me = ONLY_OBJECT;
	my leak = GET_REAL (L"Leak");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_setRanking, L"OTGrammar: Set ranking", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Ranking", L"100.0")
	REAL (L"Disharmony", L"100.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_setRanking (OBJECT, GET_INTEGER (L"Constraint"), GET_REAL (L"Ranking"), GET_REAL (L"Disharmony"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_Distributions_getFractionCorrect, L"OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL (L"Column number", L"1")
	REAL (L"Evaluation noise", L"2.0")
	INTEGER (L"Replications", L"100000")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	double result;
	OTGrammar_Distributions_getFractionCorrect (grammar, ONLY (classDistributions), GET_INTEGER (L"Column number"),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Replications"), & result);
	praat_dataChanged (grammar);
	iferror return 0;
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_Distributions_learnFromPartialOutputs, L"OTGrammar & Distributions: Learn from partial outputs", L"OT learning 6. Shortcut to OT learning")
	NATURAL (L"Column number", L"1")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
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
	OTGrammar grammar = ONLY (classOTGrammar);
	OTHistory history = NULL;
	OTGrammar_Distributions_learnFromPartialOutputs (grammar, ONLY (classDistributions), GET_INTEGER (L"Column number"),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
		GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
		GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"),
		GET_INTEGER (L"Store history every"), & history);
	praat_dataChanged (grammar);
	if (history) praat_new1 (history, grammar -> name);
	iferror {
		if (history) praat_updateSelection ();
		return 0;
	}
END

FORM (OTGrammar_Distributions_listObligatoryRankings, L"OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL (L"Column number", L"1")
	OK
DO
	if (! OTGrammar_Distributions_listObligatoryRankings (ONLY (classOTGrammar), ONLY (classDistributions), GET_INTEGER (L"Column number"))) return 0;
END

FORM (OTGrammar_PairDistribution_findPositiveWeights, L"OTGrammar & PairDistribution: Find positive weights", L"OTGrammar & PairDistribution: Find positive weights...")
	POSITIVE (L"Weight floor", L"1.0")
	POSITIVE (L"Margin of separation", L"1.0")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTGrammar_PairDistribution_findPositiveWeights_e (grammar, ONLY (classPairDistribution),
		GET_REAL (L"Weight floor"), GET_REAL (L"Margin of separation"));
	iferror return 0;
	praat_dataChanged (grammar);
	iferror return 0;
END

FORM (OTGrammar_PairDistribution_getFractionCorrect, L"OTGrammar & PairDistribution: Get fraction correct...", 0)
	REAL (L"Evaluation noise", L"2.0")
	INTEGER (L"Replications", L"100000")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	double result;
	OTGrammar_PairDistribution_getFractionCorrect (grammar, ONLY (classPairDistribution),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Replications"), & result);
	praat_dataChanged (grammar);
	iferror return 0;
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_PairDistribution_learn, L"OTGrammar & PairDistribution: Learn", L"OT learning 6. Shortcut to OT learning")
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Reranking strategy", 3)
		OPTION (L"Demotion only")
		OPTION (L"Symmetric one")
		OPTION (L"Symmetric all")
		OPTION (L"Weighted uncancelled")
		OPTION (L"Weighted all")
		OPTION (L"EDCD")
		OPTION (L"EDCD with vacation")
		OPTION (L"Demote one with vacation")
	POSITIVE (L"Initial plasticity", L"1.0")
	NATURAL (L"Replications per plasticity", L"100000")
	REAL (L"Plasticity decrement", L"0.1")
	NATURAL (L"Number of plasticities", L"4")
	REAL (L"Rel. plasticity spreading", L"0.1")
	BOOLEAN (L"Honour local rankings", 1)
	NATURAL (L"Number of chews", L"1")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTGrammar_PairDistribution_learn (grammar, ONLY (classPairDistribution),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Reranking strategy") - 1, GET_INTEGER (L"Honour local rankings"),
		GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
		GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
		GET_REAL (L"Rel. plasticity spreading"), GET_INTEGER (L"Number of chews"));
	praat_dataChanged (grammar);
	iferror return 0;
END

DIRECT (OTGrammar_PairDistribution_listObligatoryRankings)
	if (! OTGrammar_PairDistribution_listObligatoryRankings (ONLY (classOTGrammar), ONLY (classPairDistribution))) return 0;
END

FORM (OTGrammar_to_Distributions, L"OTGrammar: Compute output distributions", L"OTGrammar: To output Distributions...")
	NATURAL (L"Trials per input", L"100000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new2 (OTGrammar_to_Distribution (OBJECT, GET_INTEGER (L"Trials per input"), GET_REAL (L"Evaluation noise")),
			NAMEW, L"_out");
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTGrammar_to_PairDistribution, L"OTGrammar: Compute output distributions", 0)
	NATURAL (L"Trials per input", L"100000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new2 (OTGrammar_to_PairDistribution (OBJECT, GET_INTEGER (L"Trials per input"), GET_REAL (L"Evaluation noise")),
			NAMEW, L"_out");
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

DIRECT (OTGrammar_measureTypology)
	WHERE (SELECTED) {
		int status = praat_new2 (OTGrammar_measureTypology (OBJECT), NAMEW, L"_out");
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM_WRITE (OTGrammar_writeToHeaderlessSpreadsheetFile, L"Write OTGrammar to spreadsheet", 0, L"txt")
	if (! OTGrammar_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

FORM (OTMulti_drawTableau, L"Draw tableau", L"OT learning")
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	BOOLEAN (L"Show disharmonies", 1)
	OK
DO
	EVERY_DRAW (OTMulti_drawTableau (OBJECT, GRAPHICS, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
		GET_INTEGER (L"Show disharmonies")))
END

DIRECT (OTMulti_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error1 (L"Cannot edit from batch.");
	} else {
		WHERE (SELECTED) {
			if (! praat_installEditor (OTMultiEditor_create (theCurrentPraat -> topShell, ID_AND_FULL_NAME,
				OBJECT), IOBJECT)) return 0;
		}
	}
END

FORM (OTMulti_evaluate, L"OTMulti: Evaluate", 0)
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	OTMulti_newDisharmonies (ONLY_OBJECT, GET_REAL (L"Evaluation noise"));
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_getCandidate, L"Get candidate", 0)
	NATURAL (L"Candidate", L"1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icand = GET_INTEGER (L"Candidate");
	REQUIRE (icand <= my numberOfCandidates, L"'Candidate' should not exceed number of candidates.")
	Melder_information1 (my candidates [icand]. string);
END

FORM (OTMulti_getConstraint, L"Get constraint name", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (my constraints [icons]. name);
END

FORM (OTMulti_getDisharmony, L"Get disharmony", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. disharmony));
END

DIRECT (OTMulti_getNumberOfCandidates)
	OTMulti me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfCandidates));
END

DIRECT (OTMulti_getNumberOfConstraints)
	OTMulti me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfConstraints));
END

FORM (OTMulti_getNumberOfViolations, L"Get number of violations", 0)
	NATURAL (L"Candidate number", L"1")
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icand = GET_INTEGER (L"Candidate number"), icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icand <= my numberOfCandidates, L"'Candidate number' should not exceed number of candidates.")
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_integer (my candidates [icand]. marks [icons]));
END

FORM (OTMulti_getRankingValue, L"Get ranking value", 0)
	NATURAL (L"Constraint number", L"1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER (L"Constraint number");
	REQUIRE (icons <= my numberOfConstraints, L"'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. ranking));
END

FORM (OTMulti_getWinner, L"OTMulti: Get winner", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (OTMulti_getWinner (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"))));
END

FORM (OTMulti_generateOptimalForm, L"OTMulti: Generate optimal form", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	wchar_t output [100];
	if (! OTMulti_generateOptimalForm (ONLY_OBJECT, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
		output, GET_REAL (L"Evaluation noise"))) return 0;
	Melder_information1 (output);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_generateOptimalForms, L"OTMulti: Generate optimal forms", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	NATURAL (L"Number of trials", L"1000")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	OTMulti me = ONLY (classOTMulti);
	if (! praat_new2 (OTMulti_generateOptimalForms (me, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
		GET_INTEGER (L"Number of trials"), GET_REAL (L"Evaluation noise")), my name, L"_out")) return 0;
	praat_dataChanged (me);
END

FORM (OTMulti_learnOne, L"OTMulti: Learn one", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	OPTIONMENU (L"Learn", 3)
		OPTION (L"forward")
		OPTION (L"backward")
		OPTION (L"bidirectionally")
	POSITIVE (L"Plasticity", L"0.1")
	REAL (L"Rel. plasticity spreading", L"0.1")
	OK
DO
	WHERE (SELECTED) {
		OTMulti_learnOne (OBJECT, GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
			GET_INTEGER (L"Learn"), GET_REAL (L"Plasticity"), GET_REAL (L"Rel. plasticity spreading"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTMulti_removeConstraint, L"OTMulti: Remove constraint", 0)
	SENTENCE (L"Constraint name", L"")
	OK
DO
	WHERE (SELECTED) {
		if (! OTMulti_removeConstraint (OBJECT, GET_STRING (L"Constraint name"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_resetAllRankings, L"OTMulti: Reset all rankings", 0)
	REAL (L"Ranking", L"100.0")
	OK
DO
	WHERE (SELECTED) {
		OTMulti_reset (OBJECT, GET_REAL (L"Ranking"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_setConstraintPlasticity, L"OTMulti: Set constraint plasticity", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Plasticity", L"1.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTMulti_setConstraintPlasticity (OBJECT, GET_INTEGER (L"Constraint"), GET_REAL (L"Plasticity"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_setDecisionStrategy, L"OTMulti: Set decision strategy", 0)
	ENUM (L"Decision strategy", OTGrammar_DECISION_STRATEGY, 0)
	OK
OTMulti me = ONLY_OBJECT;
SET_INTEGER (L"Decision strategy", my decisionStrategy);
DO
	OTMulti me = ONLY_OBJECT;
	my decisionStrategy = GET_INTEGER (L"Decision strategy");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_setLeak, L"OTGrammar: Set leak", 0)
	REAL (L"Leak", L"0.0")
	OK
OTMulti me = ONLY_OBJECT;
SET_REAL (L"Leak", my leak);
DO
	OTMulti me = ONLY_OBJECT;
	my leak = GET_REAL (L"Leak");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_setRanking, L"OTMulti: Set ranking", 0)
	NATURAL (L"Constraint", L"1")
	REAL (L"Ranking", L"100.0")
	REAL (L"Disharmony", L"100.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTMulti_setRanking (OBJECT, GET_INTEGER (L"Constraint"), GET_REAL (L"Ranking"), GET_REAL (L"Disharmony"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_to_Distribution, L"OTMulti: Compute output distribution", 0)
	SENTENCE (L"Partial form 1", L"")
	SENTENCE (L"Partial form 2", L"")
	NATURAL (L"Number of trials", L"100000")
	POSITIVE (L"Evaluation noise", L"2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new2 (OTMulti_to_Distribution (OBJECT,  GET_STRING (L"Partial form 1"), GET_STRING (L"Partial form 2"),
			GET_INTEGER (L"Number of trials"), GET_REAL (L"Evaluation noise")), NAMEW, L"_out");
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTMulti_PairDistribution_learn, L"OTMulti & PairDistribution: Learn", 0)
	REAL (L"Evaluation noise", L"2.0")
	OPTIONMENU (L"Learn", 3)
		OPTION (L"forward")
		OPTION (L"backward")
		OPTION (L"bidirectionally")
	POSITIVE (L"Initial plasticity", L"1.0")
	NATURAL (L"Replications per plasticity", L"100000")
	REAL (L"Plasticity decrement", L"0.1")
	NATURAL (L"Number of plasticities", L"4")
	REAL (L"Rel. plasticity spreading", L"0.1")
	OK
DO
	OTMulti grammar = ONLY (classOTMulti);
	OTMulti_PairDistribution_learn (grammar, ONLY (classPairDistribution),
		GET_REAL (L"Evaluation noise"), GET_INTEGER (L"Learn"),
		GET_REAL (L"Initial plasticity"), GET_INTEGER (L"Replications per plasticity"),
		GET_REAL (L"Plasticity decrement"), GET_INTEGER (L"Number of plasticities"),
		GET_REAL (L"Rel. plasticity spreading"));
	praat_dataChanged (grammar);
	iferror return 0;
END

FORM (OTMulti_Strings_generateOptimalForms, L"OTGrammar: Inputs to outputs", L"OTGrammar: Inputs to outputs...")
	REAL (L"Evaluation noise", L"2.0")
	OK
DO
	OTMulti me = ONLY (classOTMulti);
	if (! praat_new2 (OTMulti_Strings_generateOptimalForms (me,
		ONLY (classStrings), GET_REAL (L"Evaluation noise")), my name, L"_out")) return 0;
	praat_dataChanged (me);
END

/***** buttons *****/

void praat_uvafon_OT_init (void);
void praat_uvafon_OT_init (void) {
	Thing_recognizeClassesByName (classOTGrammar, classOTMulti, NULL);
	Thing_recognizeClassByOtherName (classOTGrammar, L"OTCase");

	praat_addMenuCommand (L"Objects", L"New", L"-- new optimality --", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Optimality Theory", 0, 0, 0);
		praat_addMenuCommand (L"Objects", L"New", L"OT learning tutorial", 0, 1, DO_OT_learning_tutorial);
		praat_addMenuCommand (L"Objects", L"New", L"-- tableau grammars --", 0, 1, 0);
		praat_addMenuCommand (L"Objects", L"New", L"Create NoCoda grammar", 0, 1, DO_Create_NoCoda_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create place assimilation grammar", 0, 1, DO_Create_NPA_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create place assimilation distribution", 0, 1, DO_Create_NPA_distribution);
		praat_addMenuCommand (L"Objects", L"New", L"Create tongue-root grammar...", 0, 1, DO_Create_tongue_root_grammar);
		praat_addMenuCommand (L"Objects", L"New", L"Create metrics grammar...", 0, 1, DO_Create_metrics_grammar);

	praat_addAction1 (classOTGrammar, 0, L"OTGrammar help", 0, 0, DO_OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, L"Edit", 0, 0, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, L"Draw tableau...", 0, 0, DO_OTGrammar_drawTableau);
	praat_addAction1 (classOTGrammar, 1, L"Write to headerless spreadsheet file...", 0, 0, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 0, L"Query -          ", 0, 0, 0);
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

	{ void praat_TableOfReal_init (void *klas); praat_TableOfReal_init (classOTHistory); }

	praat_addAction1 (classOTMulti, 0, L"Edit", 0, 0, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, L"Draw tableau...", 0, 0, DO_OTMulti_drawTableau);
	praat_addAction1 (classOTMulti, 0, L"Query -          ", 0, 0, 0);
	praat_addAction1 (classOTMulti, 1, L"Get number of constraints", 0, 1, DO_OTMulti_getNumberOfConstraints);
	praat_addAction1 (classOTMulti, 1, L"Get constraint...", 0, 1, DO_OTMulti_getConstraint);
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
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, L"List obligatory rankings", 0, 0, DO_OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, L"Inputs to outputs...", 0, 0, DO_OTGrammar_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, L"Learn from partial outputs...", 0, 0, DO_OTGrammar_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, L"Learn...", 0, 0, DO_OTGrammar_learn);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, L"Learn...", 0, 0, DO_OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, L"Get outputs...", 0, 0, DO_OTMulti_Strings_generateOptimalForms);
}

/* End of file praat_OT.c */
