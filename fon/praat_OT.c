/* praat_OT.c
 *
 * Copyright (C) 1997-2006 Paul Boersma
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
 * pb 2006/12/26
 */

#include "praat.h"

#include "OTGrammar.h"
#include "OTMulti.h"
#include "OTGrammarEditor.h"
#include "OTMultiEditor.h"

/***** HELP *****/

DIRECT (OT_learning_tutorial) Melder_help ("OT learning"); END

DIRECT (OTGrammar_help) Melder_help ("OTGrammar"); END

/***** OTGRAMMAR *****/

FORM (Create_metrics_grammar, "Create metrics grammar", 0)
	OPTIONMENU ("Initial ranking", 1)
		OPTION ("Equal")
		OPTION ("Foot form high")
		OPTION ("WSP high")
	OPTIONMENU ("Trochaicity constraint", 1)
		OPTION ("FtNonfinal")
		OPTION ("Trochaic")
	BOOLEAN ("Include FootBimoraic", 0)
	BOOLEAN ("Include FootBisyllabic", 0)
	BOOLEAN ("Include Peripheral", 0)
	OPTIONMENU ("Nonfinality constraint", 1)
		OPTION ("Nonfinal")
		OPTION ("MainNonfinal")
		OPTION ("HeadNonfinal")
	BOOLEAN ("Overt forms have secondary stress", 1)
	BOOLEAN ("Include *Clash and *Lapse", 0)
	BOOLEAN ("Include codas", 0)
	OK
DO
	if (! praat_new (OTGrammar_create_metrics (GET_INTEGER ("Initial ranking"), GET_INTEGER ("Trochaicity constraint"),
		GET_INTEGER ("Include FootBimoraic"), GET_INTEGER ("Include FootBisyllabic"),
		GET_INTEGER ("Include Peripheral"), GET_INTEGER ("Nonfinality constraint"),
		GET_INTEGER ("Overt forms have secondary stress"), GET_INTEGER ("Include *Clash and *Lapse"), GET_INTEGER ("Include codas")),
		GET_STRING ("Initial ranking"))) return 0;
END

DIRECT (Create_NoCoda_grammar)
	if (! praat_new (OTGrammar_create_NoCoda_grammar (), "NoCoda")) return 0;
END

DIRECT (Create_NPA_grammar)
	if (! praat_new (OTGrammar_create_NPA_grammar (), "assimilation")) return 0;
END

DIRECT (Create_NPA_distribution)
	if (! praat_new (OTGrammar_create_NPA_distribution (), "assimilation")) return 0;
END

FORM (Create_tongue_root_grammar, "Create tongue-root grammar", "Create tongue-root grammar...")
	RADIO ("Constraint set", 1)
		RADIOBUTTON ("Five")
		RADIOBUTTON ("Nine")
	RADIO ("Ranking", 3)
		RADIOBUTTON ("Equal")
		RADIOBUTTON ("Random")
		RADIOBUTTON ("Infant")
		RADIOBUTTON ("Wolof")
	OK
DO
	if (! praat_new (OTGrammar_create_tongueRoot_grammar (GET_INTEGER ("Constraint set"),
		GET_INTEGER ("Ranking")), GET_STRING ("Ranking"))) return 0;
END

FORM (OTGrammar_drawTableau, "Draw tableau", "OT learning")
	SENTENCE ("Input string", "")
	OK
DO
	EVERY_DRAW (OTGrammar_drawTableau (OBJECT, GRAPHICS, GET_STRING ("Input string")))
END

DIRECT (OTGrammar_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error ("Cannot edit from batch.");
	} else {
		WHERE (SELECTED) {
			if (! praat_installEditor (OTGrammarEditor_create (theCurrentPraat -> topShell, FULL_NAME,
				OBJECT), IOBJECT)) return 0;
		}
	}
END

FORM (OTGrammar_evaluate, "OTGrammar: Evaluate", 0)
	REAL ("Evaluation noise", "2.0")
	OK
DO
	OTGrammar_newDisharmonies (ONLY_OBJECT, GET_REAL ("Evaluation noise"));
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_generateInputs, "Generate inputs", "OTGrammar: Generate inputs...")
	NATURAL ("Number of trials", "1000")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new (OTGrammar_generateInputs (OBJECT, GET_INTEGER ("Number of trials")),
			"%s_in", NAME);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTGrammar_getCandidate, "Get candidate", 0)
	NATURAL ("Tableau", "1")
	NATURAL ("Candidate", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	OTGrammarTableau tableau;
	long itab = GET_INTEGER ("Tableau"), icand = GET_INTEGER ("Candidate");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	tableau = & my tableaus [itab];
	REQUIRE (icand <= tableau -> numberOfCandidates, "'Candidate' should not exceed number of candidates.")
	Melder_information1 (tableau -> candidates [icand]. output);
END

FORM (OTGrammar_getConstraint, "Get constraint name", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (my constraints [icons]. name);
END

FORM (OTGrammar_getDisharmony, "Get disharmony", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. disharmony));
END

FORM (OTGrammar_getInput, "Get input", 0)
	NATURAL ("Tableau", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	Melder_information1 (my tableaus [itab]. input);
END

DIRECT (OTGrammar_getInputs)
	WHERE (SELECTED) {
		if (! praat_new (OTGrammar_getInputs (OBJECT), "%s_in", NAME)) return 0;
	}
END

FORM (OTGrammar_getInterpretiveParse, "OTGrammar: Interpretive parse", 0)
	SENTENCE ("Partial output", "")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long bestInput, bestOutput;
	if (! OTGrammar_getInterpretiveParse (me, GET_STRING ("Partial output"), & bestInput, & bestOutput)) return 0;
	Melder_information9 ("Best input = ", Melder_integer (bestInput), ": ", my tableaus [bestInput]. input,
		"\nBest output = ", Melder_integer (bestOutput), ": ", my tableaus [bestInput]. candidates [bestOutput]. output, 0);
END

FORM (OTGrammar_getNumberOfCandidates, "Get number of candidates", 0)
	NATURAL ("Tableau", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (my tableaus [itab]. numberOfCandidates));
END

DIRECT (OTGrammar_getNumberOfConstraints)
	OTGrammar me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfConstraints));
END

FORM (OTGrammar_getNumberOfOptimalCandidates, "Get number of optimal candidates", 0)
	NATURAL ("Tableau", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (OTGrammar_getNumberOfOptimalCandidates (me, itab)));
END

DIRECT (OTGrammar_getNumberOfTableaus)
	OTGrammar me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (my numberOfTableaus));
END

FORM (OTGrammar_getNumberOfViolations, "Get number of violations", 0)
	NATURAL ("Tableau number", "1")
	NATURAL ("Candidate number", "1")
	NATURAL ("Constraint number", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau number"), icand = GET_INTEGER ("Candidate number"), icons = GET_INTEGER ("Constraint number");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau number' should not exceed number of tableaus.")
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, "'Candidate number' should not exceed number of candidates for this tableau.")
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_integer (my tableaus [itab]. candidates [icand]. marks [icons]));
END

FORM (OTGrammar_getRankingValue, "Get ranking value", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. ranking));
END

FORM (OTGrammar_getWinner, "Get winner", 0)
	NATURAL ("Tableau", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau");
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	Melder_information1 (Melder_integer (OTGrammar_getWinner (me, itab)));
END

FORM (OTGrammar_inputToOutput, "OTGrammar: Input to output", "OTGrammar: Input to output...")
	SENTENCE ("Input form", "")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	char output [100];
	if (! OTGrammar_inputToOutput (ONLY_OBJECT, GET_STRING ("Input form"), output, GET_REAL ("Evaluation noise"))) return 0;
	Melder_information1 (output);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_inputToOutputs, "OTGrammar: Input to outputs", "OTGrammar: Input to outputs...")
	NATURAL ("Trials", "1000")
	REAL ("Evaluation noise", "2.0")
	SENTENCE ("Input form", "")
	OK
DO
	OTGrammar ot = ONLY (classOTGrammar);
	if (! praat_new (OTGrammar_inputToOutputs (ot,
		GET_STRING ("Input form"), GET_INTEGER ("Trials"), GET_REAL ("Evaluation noise")), "%s_out", ot -> name)) return 0;
	praat_dataChanged (ot);
END

FORM (OTGrammar_inputsToOutputs, "OTGrammar: Inputs to outputs", "OTGrammar: Inputs to outputs...")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	OTGrammar ot = ONLY (classOTGrammar);
	if (! praat_new (OTGrammar_inputsToOutputs (ot,
		ONLY (classStrings), GET_REAL ("Evaluation noise")), "%s_out", ot -> name)) return 0;
	praat_dataChanged (ot);
END

FORM (OTGrammar_isCandidateGrammatical, "Is candidate grammatical?", 0)
	NATURAL ("Tableau", "1")
	NATURAL ("Candidate", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau"), icand;
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	icand = GET_INTEGER ("Candidate");
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, "'Candidate' should not exceed number of candidates.")
	Melder_information1 (Melder_integer (OTGrammar_isCandidateGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isCandidateSinglyGrammatical, "Is candidate singly grammatical?", 0)
	NATURAL ("Tableau", "1")
	NATURAL ("Candidate", "1")
	OK
DO
	OTGrammar me = ONLY_OBJECT;
	long itab = GET_INTEGER ("Tableau"), icand;
	REQUIRE (itab <= my numberOfTableaus, "'Tableau' should not exceed number of tableaus.")
	icand = GET_INTEGER ("Candidate");
	REQUIRE (icand <= my tableaus [itab]. numberOfCandidates, "'Candidate' should not exceed number of candidates.")
	Melder_information1 (Melder_integer (OTGrammar_isCandidateSinglyGrammatical (me, itab, icand)));
END

FORM (OTGrammar_isPartialOutputGrammatical, "Is partial output grammatical?", 0)
	SENTENCE ("Partial output", "")
	OK
DO
	Melder_information1 (Melder_integer (OTGrammar_isPartialOutputGrammatical (ONLY_OBJECT, GET_STRING ("Partial output"))));
END

FORM (OTGrammar_isPartialOutputSinglyGrammatical, "Is partial output singly grammatical?", 0)
	SENTENCE ("Partial output", "")
	OK
DO
	Melder_information1 (Melder_integer (OTGrammar_isPartialOutputSinglyGrammatical (ONLY_OBJECT, GET_STRING ("Partial output"))));
END

FORM (OTGrammar_learn, "OTGrammar: Learn", "OTGrammar & 2 Strings: Learn...")
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	REAL ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	NATURAL ("Number of chews", "1")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	Strings inputs = NULL, outputs = NULL;
	WHERE (SELECTED && CLASS == classStrings) { if (! inputs) inputs = OBJECT; else outputs = OBJECT; }
	OTGrammar_learn (grammar, inputs, outputs,
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
		GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"), GET_INTEGER ("Number of chews"));
	praat_dataChanged (grammar);
	iferror return 0;
END

FORM (OTGrammar_learnFromPartialOutputs, "OTGrammar: Learn from partial adult outputs", 0)
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	REAL ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	NATURAL ("Number of chews", "1")
	INTEGER ("Store history every", "0")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTHistory history = NULL;
	OTGrammar_learnFromPartialOutputs (grammar, ONLY (classStrings),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
		GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"), GET_INTEGER ("Number of chews"),
		GET_INTEGER ("Store history every"), & history);
	praat_dataChanged (grammar);
	if (history) praat_new (history, "%s", grammar -> name);
	iferror {
		if (history) praat_updateSelection ();
		return 0;
	}
END

FORM (OTGrammar_learnOne, "OTGrammar: Learn one", "OTGrammar: Learn one...")
	LABEL ("", "Underlying form:")
	SENTENCE ("Input string", "")
	LABEL ("", "Adult surface form:")
	SENTENCE ("Output string", "")
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	REAL ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_learnOne (OBJECT, GET_STRING ("Input string"), GET_STRING ("Output string"),
			GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
			GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"), TRUE, TRUE, NULL);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTGrammar_learnOneFromPartialOutput, "OTGrammar: Learn one from partial adult output", 0)
	LABEL ("", "Partial adult surface form (e.g. overt form):")
	SENTENCE ("Partial output", "")
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	REAL ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	NATURAL ("Number of chews", "1")
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_learnOneFromPartialOutput (OBJECT, GET_STRING ("Partial output"),
			GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
			GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"), GET_INTEGER ("Number of chews"), TRUE);
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTGrammar_removeConstraint, "OTGrammar: Remove constraint", 0)
	SENTENCE ("Constraint name", "")
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_removeConstraint (OBJECT, GET_STRING ("Constraint name"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_removeHarmonicallyBoundedCandidates, "OTGrammar: Remove harmonically bounded candidates", 0)
	BOOLEAN ("Singly", 0)
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_removeHarmonicallyBoundedCandidates (OBJECT, GET_INTEGER ("Singly"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_resetAllRankings, "OTGrammar: Reset all rankings", 0)
	REAL ("Ranking", "100.0")
	OK
DO
	WHERE (SELECTED) {
		OTGrammar_reset (OBJECT, GET_REAL ("Ranking"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_setDecisionStrategy, "OTGrammar: Set decision strategy", 0)
	ENUM ("Decision strategy", OTGrammar_DECISION_STRATEGY, 0)
	OK
OTGrammar me = ONLY_OBJECT;
SET_INTEGER ("Decision strategy", my decisionStrategy);
DO
	OTGrammar me = ONLY_OBJECT;
	my decisionStrategy = GET_INTEGER ("Decision strategy");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTGrammar_setRanking, "OTGrammar: Set ranking", 0)
	NATURAL ("Constraint", "1")
	REAL ("Ranking", "100.0")
	REAL ("Disharmony", "100.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTGrammar_setRanking (OBJECT, GET_INTEGER ("Constraint"), GET_REAL ("Ranking"), GET_REAL ("Disharmony"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTGrammar_Distributions_getFractionCorrect, "OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL ("Column number", "1")
	REAL ("Evaluation noise", "2.0")
	INTEGER ("Replications", "100000")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	double result;
	OTGrammar_Distributions_getFractionCorrect (grammar, ONLY (classDistributions), GET_INTEGER ("Column number"),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Replications"), & result);
	praat_dataChanged (grammar);
	iferror return 0;
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_Distributions_learnFromPartialOutputs, "OTGrammar & Distributions: Learn from partial outputs", "OT learning 6. Shortcut to OT learning")
	NATURAL ("Column number", "1")
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	REAL ("Initial plasticity", "1.0")
	NATURAL ("Replications per plasticity", "100000")
	REAL ("Plasticity decrement", "0.1")
	NATURAL ("Number of plasticities", "4")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	NATURAL ("Number of chews", "1")
	INTEGER ("Store history every", "0")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTHistory history = NULL;
	OTGrammar_Distributions_learnFromPartialOutputs (grammar, ONLY (classDistributions), GET_INTEGER ("Column number"),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
		GET_REAL ("Initial plasticity"), GET_INTEGER ("Replications per plasticity"),
		GET_REAL ("Plasticity decrement"), GET_INTEGER ("Number of plasticities"),
		GET_REAL ("Rel. plasticity spreading"), GET_INTEGER ("Number of chews"),
		GET_INTEGER ("Store history every"), & history);
	praat_dataChanged (grammar);
	if (history) praat_new (history, "%s", grammar -> name);
	iferror {
		if (history) praat_updateSelection ();
		return 0;
	}
END

FORM (OTGrammar_Distributions_listObligatoryRankings, "OTGrammar & Distributions: Get fraction correct...", 0)
	NATURAL ("Column number", "1")
	OK
DO
	if (! OTGrammar_Distributions_listObligatoryRankings (ONLY (classOTGrammar), ONLY (classDistributions), GET_INTEGER ("Column number"))) return 0;
END

FORM (OTGrammar_PairDistribution_getFractionCorrect, "OTGrammar & PairDistribution: Get fraction correct...", 0)
	REAL ("Evaluation noise", "2.0")
	INTEGER ("Replications", "100000")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	double result;
	OTGrammar_PairDistribution_getFractionCorrect (grammar, ONLY (classPairDistribution),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Replications"), & result);
	praat_dataChanged (grammar);
	iferror return 0;
	Melder_informationReal (result, NULL);
END

FORM (OTGrammar_PairDistribution_learn, "OTGrammar & PairDistribution: Learn", "OT learning 6. Shortcut to OT learning")
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Reranking strategy", 3)
		OPTION ("Demotion only")
		OPTION ("Symmetric one")
		OPTION ("Symmetric all")
		OPTION ("Weighted uncancelled")
		OPTION ("Weighted all")
		OPTION ("EDCD")
	POSITIVE ("Initial plasticity", "1.0")
	NATURAL ("Replications per plasticity", "100000")
	REAL ("Plasticity decrement", "0.1")
	NATURAL ("Number of plasticities", "4")
	REAL ("Rel. plasticity spreading", "0.1")
	BOOLEAN ("Honour local rankings", 1)
	NATURAL ("Number of chews", "1")
	OK
DO
	OTGrammar grammar = ONLY (classOTGrammar);
	OTGrammar_PairDistribution_learn (grammar, ONLY (classPairDistribution),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Reranking strategy") - 1, GET_INTEGER ("Honour local rankings"),
		GET_REAL ("Initial plasticity"), GET_INTEGER ("Replications per plasticity"),
		GET_REAL ("Plasticity decrement"), GET_INTEGER ("Number of plasticities"),
		GET_REAL ("Rel. plasticity spreading"), GET_INTEGER ("Number of chews"));
	praat_dataChanged (grammar);
	iferror return 0;
END

DIRECT (OTGrammar_PairDistribution_listObligatoryRankings)
	if (! OTGrammar_PairDistribution_listObligatoryRankings (ONLY (classOTGrammar), ONLY (classPairDistribution))) return 0;
END

FORM (OTGrammar_to_Distributions, "OTGrammar: Compute output distributions", "OTGrammar: To output Distributions...")
	NATURAL ("Trials per input", "100000")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new (OTGrammar_to_Distribution (OBJECT, GET_INTEGER ("Trials per input"), GET_REAL ("Evaluation noise")), "%s_out", NAME);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTGrammar_to_PairDistribution, "OTGrammar: Compute output distributions", 0)
	NATURAL ("Trials per input", "100000")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new (OTGrammar_to_PairDistribution (OBJECT, GET_INTEGER ("Trials per input"), GET_REAL ("Evaluation noise")), "%s_out", NAME);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

DIRECT (OTGrammar_measureTypology)
	WHERE (SELECTED) {
		int status = praat_new (OTGrammar_measureTypology (OBJECT), "%s_out", NAME);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM_WRITE (OTGrammar_writeToHeaderlessSpreadsheetFile, "Write OTGrammar to spreadsheet", 0, "txt")
	if (! OTGrammar_writeToHeaderlessSpreadsheetFile (ONLY_OBJECT, file)) return 0;
END

FORM (OTMulti_drawTableau, "Draw tableau", "OT learning")
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	BOOLEAN ("Show disharmonies", 1)
	OK
DO
	EVERY_DRAW (OTMulti_drawTableau (OBJECT, GRAPHICS, GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"),
		GET_INTEGER ("Show disharmonies")))
END

DIRECT (OTMulti_edit)
	if (theCurrentPraat -> batch) {
		return Melder_error ("Cannot edit from batch.");
	} else {
		WHERE (SELECTED) {
			if (! praat_installEditor (OTMultiEditor_create (theCurrentPraat -> topShell, FULL_NAME,
				OBJECT), IOBJECT)) return 0;
		}
	}
END

FORM (OTMulti_getCandidate, "Get candidate", 0)
	NATURAL ("Candidate", "1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icand = GET_INTEGER ("Candidate");
	REQUIRE (icand <= my numberOfCandidates, "'Candidate' should not exceed number of candidates.")
	Melder_information1 (my candidates [icand]. string);
END

FORM (OTMulti_getConstraint, "Get constraint name", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (my constraints [icons]. name);
END

FORM (OTMulti_getDisharmony, "Get disharmony", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
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

FORM (OTMulti_getNumberOfViolations, "Get number of violations", 0)
	NATURAL ("Candidate number", "1")
	NATURAL ("Constraint number", "1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icand = GET_INTEGER ("Candidate number"), icons = GET_INTEGER ("Constraint number");
	REQUIRE (icand <= my numberOfCandidates, "'Candidate number' should not exceed number of candidates.")
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_integer (my candidates [icand]. marks [icons]));
END

FORM (OTMulti_getRankingValue, "Get ranking value", 0)
	NATURAL ("Constraint number", "1")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	long icons = GET_INTEGER ("Constraint number");
	REQUIRE (icons <= my numberOfConstraints, "'Constraint number' should not exceed number of constraints.")
	Melder_information1 (Melder_double (my constraints [icons]. ranking));
END

FORM (OTMulti_getWinner, "OTMulti: Get winner", 0)
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	OK
DO
	OTMulti me = ONLY_OBJECT;
	Melder_information1 (Melder_integer (OTMulti_getWinner (me, GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"))));
END

FORM (OTMulti_generateOptimalForm, "OTMulti: Generate optimal form", 0)
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	char output [100];
	if (! OTMulti_generateOptimalForm (ONLY_OBJECT, GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"),
		output, GET_REAL ("Evaluation noise"))) return 0;
	Melder_information1 (output);
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_generateOptimalForms, "OTMulti: Generate optimal forms", 0)
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	REAL ("Evaluation noise", "2.0")
	NATURAL ("Trials", "1000")
	OK
DO
	OTMulti me = ONLY (classOTMulti);
	if (! praat_new (OTMulti_generateOptimalForms (me, GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Trials")), "%s_out", my name)) return 0;
	praat_dataChanged (me);
END

FORM (OTMulti_learnOne, "OTMulti: Learn one", 0)
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	OPTIONMENU ("Learn", 3)
		OPTION ("forward")
		OPTION ("backward")
		OPTION ("bidirectionally")
	POSITIVE ("Plasticity", "0.1")
	REAL ("Rel. plasticity spreading", "0.1")
	OK
DO
	WHERE (SELECTED) {
		OTMulti_learnOne (OBJECT, GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"),
			GET_INTEGER ("Learn"), GET_REAL ("Plasticity"), GET_REAL ("Rel. plasticity spreading"));
		praat_dataChanged (OBJECT);
		iferror return 0;
	}
END

FORM (OTMulti_removeConstraint, "OTMulti: Remove constraint", 0)
	SENTENCE ("Constraint name", "")
	OK
DO
	WHERE (SELECTED) {
		if (! OTMulti_removeConstraint (OBJECT, GET_STRING ("Constraint name"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_resetAllRankings, "OTMulti: Reset all rankings", 0)
	REAL ("Ranking", "100.0")
	OK
DO
	WHERE (SELECTED) {
		OTMulti_reset (OBJECT, GET_REAL ("Ranking"));
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_setDecisionStrategy, "OTMulti: Set decision strategy", 0)
	ENUM ("Decision strategy", OTGrammar_DECISION_STRATEGY, 0)
	OK
OTMulti me = ONLY_OBJECT;
SET_INTEGER ("Decision strategy", my decisionStrategy);
DO
	OTMulti me = ONLY_OBJECT;
	my decisionStrategy = GET_INTEGER ("Decision strategy");
	praat_dataChanged (ONLY_OBJECT);
END

FORM (OTMulti_setRanking, "OTMulti: Set ranking", 0)
	NATURAL ("Constraint", "1")
	REAL ("Ranking", "100.0")
	REAL ("Disharmony", "100.0")
	OK
DO
	WHERE (SELECTED) {
		if (! OTMulti_setRanking (OBJECT, GET_INTEGER ("Constraint"), GET_REAL ("Ranking"), GET_REAL ("Disharmony"))) return 0;
		praat_dataChanged (OBJECT);
	}
END

FORM (OTMulti_to_Distribution, "OTMulti: Compute output distribution", 0)
	SENTENCE ("Partial form 1", "")
	SENTENCE ("Partial form 2", "")
	NATURAL ("Number of trials", "100000")
	POSITIVE ("Evaluation noise", "2.0")
	OK
DO
	WHERE (SELECTED) {
		int status = praat_new (OTMulti_to_Distribution (OBJECT,  GET_STRING ("Partial form 1"), GET_STRING ("Partial form 2"),
		GET_INTEGER ("Number of trials"), GET_REAL ("Evaluation noise")), "%s_out", NAME);
		praat_dataChanged (OBJECT);
		if (! status) return 0;
	}
END

FORM (OTMulti_PairDistribution_learn, "OTMulti & PairDistribution: Learn", 0)
	REAL ("Evaluation noise", "2.0")
	OPTIONMENU ("Learn", 3)
		OPTION ("forward")
		OPTION ("backward")
		OPTION ("bidirectionally")
	POSITIVE ("Initial plasticity", "1.0")
	NATURAL ("Replications per plasticity", "100000")
	REAL ("Plasticity decrement", "0.1")
	NATURAL ("Number of plasticities", "4")
	REAL ("Rel. plasticity spreading", "0.1")
	OK
DO
	OTMulti grammar = ONLY (classOTMulti);
	OTMulti_PairDistribution_learn (grammar, ONLY (classPairDistribution),
		GET_REAL ("Evaluation noise"), GET_INTEGER ("Learn"),
		GET_REAL ("Initial plasticity"), GET_INTEGER ("Replications per plasticity"),
		GET_REAL ("Plasticity decrement"), GET_INTEGER ("Number of plasticities"),
		GET_REAL ("Rel. plasticity spreading"));
	praat_dataChanged (grammar);
	iferror return 0;
END

FORM (OTMulti_Strings_generateOptimalForms, "OTGrammar: Inputs to outputs", "OTGrammar: Inputs to outputs...")
	REAL ("Evaluation noise", "2.0")
	OK
DO
	OTMulti me = ONLY (classOTMulti);
	if (! praat_new (OTMulti_Strings_generateOptimalForms (me,
		ONLY (classStrings), GET_REAL ("Evaluation noise")), "%s_out", my name)) return 0;
	praat_dataChanged (me);
END

/***** buttons *****/

void praat_uvafon_OT_init (void);
void praat_uvafon_OT_init (void) {
	Thing_recognizeClassesByName (classOTGrammar, classOTMulti, NULL);
	Thing_recognizeClassByOtherName (classOTGrammar, "OTCase");

	praat_addMenuCommand ("Objects", "New", "-- new optimality --", 0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "Optimality Theory", 0, 0, 0);
		praat_addMenuCommand ("Objects", "New", "OT learning tutorial", 0, 1, DO_OT_learning_tutorial);
		praat_addMenuCommand ("Objects", "New", "-- tableau grammars --", 0, 1, 0);
		praat_addMenuCommand ("Objects", "New", "Create NoCoda grammar", 0, 1, DO_Create_NoCoda_grammar);
		praat_addMenuCommand ("Objects", "New", "Create place assimilation grammar", 0, 1, DO_Create_NPA_grammar);
		praat_addMenuCommand ("Objects", "New", "Create place assimilation distribution", 0, 1, DO_Create_NPA_distribution);
		praat_addMenuCommand ("Objects", "New", "Create tongue-root grammar...", 0, 1, DO_Create_tongue_root_grammar);
		praat_addMenuCommand ("Objects", "New", "Create metrics grammar...", 0, 1, DO_Create_metrics_grammar);

	praat_addAction1 (classOTGrammar, 0, "OTGrammar help", 0, 0, DO_OTGrammar_help);
	praat_addAction1 (classOTGrammar, 0, "Edit", 0, 0, DO_OTGrammar_edit);
	praat_addAction1 (classOTGrammar, 0, "Draw tableau...", 0, 0, DO_OTGrammar_drawTableau);
	praat_addAction1 (classOTGrammar, 1, "Write to headerless spreadsheet file...", 0, 0, DO_OTGrammar_writeToHeaderlessSpreadsheetFile);
	praat_addAction1 (classOTGrammar, 0, "Query -          ", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 1, "Get number of constraints", 0, 1, DO_OTGrammar_getNumberOfConstraints);
	praat_addAction1 (classOTGrammar, 1, "Get constraint...", 0, 1, DO_OTGrammar_getConstraint);
	praat_addAction1 (classOTGrammar, 1, "Get ranking value...", 0, 1, DO_OTGrammar_getRankingValue);
	praat_addAction1 (classOTGrammar, 1, "Get disharmony...", 0, 1, DO_OTGrammar_getDisharmony);
	praat_addAction1 (classOTGrammar, 1, "Get number of tableaus", 0, 1, DO_OTGrammar_getNumberOfTableaus);
	praat_addAction1 (classOTGrammar, 1, "Get input...", 0, 1, DO_OTGrammar_getInput);
	praat_addAction1 (classOTGrammar, 1, "Get number of candidates...", 0, 1, DO_OTGrammar_getNumberOfCandidates);
	praat_addAction1 (classOTGrammar, 1, "Get candidate...", 0, 1, DO_OTGrammar_getCandidate);
	praat_addAction1 (classOTGrammar, 1, "Get number of violations...", 0, 1, DO_OTGrammar_getNumberOfViolations);
	praat_addAction1 (classOTGrammar, 1, "-- parse --", 0, 1, 0);
	praat_addAction1 (classOTGrammar, 1, "Get winner...", 0, 1, DO_OTGrammar_getWinner);
	praat_addAction1 (classOTGrammar, 1, "Get number of optimal candidates...", 0, 1, DO_OTGrammar_getNumberOfOptimalCandidates);
	praat_addAction1 (classOTGrammar, 1, "Is candidate grammatical...", 0, 1, DO_OTGrammar_isCandidateGrammatical);
	praat_addAction1 (classOTGrammar, 1, "Is candidate singly grammatical...", 0, 1, DO_OTGrammar_isCandidateSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 1, "Get interpretive parse...", 0, 1, DO_OTGrammar_getInterpretiveParse);
	praat_addAction1 (classOTGrammar, 1, "Is partial output grammatical...", 0, 1, DO_OTGrammar_isPartialOutputGrammatical);
	praat_addAction1 (classOTGrammar, 1, "Is partial output singly grammatical...", 0, 1, DO_OTGrammar_isPartialOutputSinglyGrammatical);
	praat_addAction1 (classOTGrammar, 0, "Generate inputs...", 0, 0, DO_OTGrammar_generateInputs);
	praat_addAction1 (classOTGrammar, 0, "Get inputs", 0, 0, DO_OTGrammar_getInputs);
	praat_addAction1 (classOTGrammar, 0, "Measure typology", 0, 0, DO_OTGrammar_measureTypology);
	praat_addAction1 (classOTGrammar, 0, "Evaluate", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, "Evaluate...", 0, 0, DO_OTGrammar_evaluate);
	praat_addAction1 (classOTGrammar, 0, "Input to output...", 0, 0, DO_OTGrammar_inputToOutput);
	praat_addAction1 (classOTGrammar, 0, "Input to outputs...", 0, 0, DO_OTGrammar_inputToOutputs);
	praat_addAction1 (classOTGrammar, 0, "To output Distributions...", 0, 0, DO_OTGrammar_to_Distributions);
	praat_addAction1 (classOTGrammar, 0, "To PairDistribution...", 0, 0, DO_OTGrammar_to_PairDistribution);
	praat_addAction1 (classOTGrammar, 0, "Modify ranking", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, "Set ranking...", 0, 0, DO_OTGrammar_setRanking);
	praat_addAction1 (classOTGrammar, 0, "Reset all rankings...", 0, 0, DO_OTGrammar_resetAllRankings);
	praat_addAction1 (classOTGrammar, 0, "Learn one...", 0, 0, DO_OTGrammar_learnOne);
	praat_addAction1 (classOTGrammar, 0, "Learn one from partial output...", 0, 0, DO_OTGrammar_learnOneFromPartialOutput);
	praat_addAction1 (classOTGrammar, 0, "Modify behaviour -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 1, "Set harmony computation method...", 0, praat_DEPTH_1 + praat_HIDDEN, DO_OTGrammar_setDecisionStrategy);
	praat_addAction1 (classOTGrammar, 1, "Set decision strategy...", 0, 1, DO_OTGrammar_setDecisionStrategy);
	praat_addAction1 (classOTGrammar, 0, "Modify structure -", 0, 0, 0);
	praat_addAction1 (classOTGrammar, 0, "Remove constraint...", 0, 1, DO_OTGrammar_removeConstraint);
	praat_addAction1 (classOTGrammar, 0, "Remove harmonically bounded candidates...", 0, 1, DO_OTGrammar_removeHarmonicallyBoundedCandidates);

	{ void praat_TableOfReal_init (void *klas); praat_TableOfReal_init (classOTHistory); }

	praat_addAction1 (classOTMulti, 0, "Edit", 0, 0, DO_OTMulti_edit);
	praat_addAction1 (classOTMulti, 0, "Draw tableau...", 0, 0, DO_OTMulti_drawTableau);
	praat_addAction1 (classOTMulti, 0, "Query -          ", 0, 0, 0);
	praat_addAction1 (classOTMulti, 1, "Get number of constraints", 0, 1, DO_OTMulti_getNumberOfConstraints);
	praat_addAction1 (classOTMulti, 1, "Get constraint...", 0, 1, DO_OTMulti_getConstraint);
	praat_addAction1 (classOTMulti, 1, "Get ranking value...", 0, 1, DO_OTMulti_getRankingValue);
	praat_addAction1 (classOTMulti, 1, "Get disharmony...", 0, 1, DO_OTMulti_getDisharmony);
	praat_addAction1 (classOTMulti, 1, "Get number of candidates", 0, 1, DO_OTMulti_getNumberOfCandidates);
	praat_addAction1 (classOTMulti, 1, "Get candidate...", 0, 1, DO_OTMulti_getCandidate);
	praat_addAction1 (classOTMulti, 1, "Get number of violations...", 0, 1, DO_OTMulti_getNumberOfViolations);
	praat_addAction1 (classOTMulti, 1, "-- parse --", 0, 1, 0);
	praat_addAction1 (classOTMulti, 1, "Get winner...", 0, 1, DO_OTMulti_getWinner);
	praat_addAction1 (classOTMulti, 0, "Evaluate", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, "Get output...", 0, 0, DO_OTMulti_generateOptimalForm);
	praat_addAction1 (classOTMulti, 0, "Get outputs...", 0, 0, DO_OTMulti_generateOptimalForms);
	praat_addAction1 (classOTMulti, 0, "To output Distribution...", 0, 0, DO_OTMulti_to_Distribution);
	praat_addAction1 (classOTMulti, 0, "Modify ranking", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, "Set ranking...", 0, 0, DO_OTMulti_setRanking);
	praat_addAction1 (classOTMulti, 0, "Reset all rankings...", 0, 0, DO_OTMulti_resetAllRankings);
	praat_addAction1 (classOTMulti, 0, "Learn one...", 0, 0, DO_OTMulti_learnOne);
	praat_addAction1 (classOTMulti, 0, "Modify behaviour -", 0, 0, 0);
	praat_addAction1 (classOTMulti, 1, "Set decision strategy...", 0, 1, DO_OTMulti_setDecisionStrategy);
	praat_addAction1 (classOTMulti, 0, "Modify structure -", 0, 0, 0);
	praat_addAction1 (classOTMulti, 0, "Remove constraint...", 0, 1, DO_OTMulti_removeConstraint);

	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, "Learn from partial outputs...", 0, 0, DO_OTGrammar_Distributions_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, "Get fraction correct...", 0, 0, DO_OTGrammar_Distributions_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classDistributions, 1, "List obligatory rankings...", 0, praat_HIDDEN, DO_OTGrammar_Distributions_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, "Learn...", 0, 0, DO_OTGrammar_PairDistribution_learn);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, "Get fraction correct...", 0, 0, DO_OTGrammar_PairDistribution_getFractionCorrect);
	praat_addAction2 (classOTGrammar, 1, classPairDistribution, 1, "List obligatory rankings", 0, 0, DO_OTGrammar_PairDistribution_listObligatoryRankings);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, "Inputs to outputs...", 0, 0, DO_OTGrammar_inputsToOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 1, "Learn from partial outputs...", 0, 0, DO_OTGrammar_learnFromPartialOutputs);
	praat_addAction2 (classOTGrammar, 1, classStrings, 2, "Learn...", 0, 0, DO_OTGrammar_learn);
	praat_addAction2 (classOTMulti, 1, classPairDistribution, 1, "Learn...", 0, 0, DO_OTMulti_PairDistribution_learn);
	praat_addAction2 (classOTMulti, 1, classStrings, 1, "Get outputs...", 0, 0, DO_OTMulti_Strings_generateOptimalForms);
}

/* End of file praat_OT.c */
