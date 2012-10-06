/* praat_Exp.cpp
 *
 * Copyright (C) 2001-2012 Paul Boersma
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

#include "ExperimentMFC.h"
#include "RunnerMFC.h"

/***** CATEGORIES *****/

DIRECT (Categories_getEntropy)
	iam_ONLY (Categories);
	double entropy = Categories_getEntropy (me);
	Melder_informationReal (entropy, L"bits");
END

DIRECT (Categories_sort)
	WHERE (SELECTED) {
		iam_LOOP (Categories);
		Categories_sort (me);
		praat_dataChanged (me);
	}
END

/***** EXPERIMENT_MFC *****/

DIRECT (ExperimentMFC_run)
	if (theCurrentPraatApplication -> batch) Melder_throw ("Cannot run experiments from the command line.");
	autoOrdered experiments = Ordered_create ();
	Collection_dontOwnItems (experiments.peek());
	WHERE (SELECTED) {
		iam_LOOP (ExperimentMFC);
		Melder_assert (my classInfo == classExperimentMFC);
		Collection_addItem (experiments.peek(), me);   // reference copy of me
	}
	Melder_assert (experiments-> size >= 1);
	Melder_assert (((Data) experiments -> item [1]) -> classInfo == classExperimentMFC);
	Melder_assert (((Data) experiments -> item [experiments -> size]) -> classInfo == classExperimentMFC);
	autoOrdered experimentsCopy = experiments.clone();   // we need a copy, because we do a transfer, then a peek
	Melder_assert (experimentsCopy -> size == experiments -> size);
	Melder_assert (experimentsCopy -> item [1] == experiments -> item [1]);
	Melder_assert (experimentsCopy -> item [experimentsCopy -> size] == experiments -> item [experiments -> size]);
	autoRunnerMFC runner = RunnerMFC_create (L"listening experiments", experimentsCopy.transfer());
	praat_installEditorN (runner.transfer(), experiments.peek());
END

DIRECT (ExperimentMFC_extractResults)
	WHERE (SELECTED) {
		iam_LOOP (ExperimentMFC);
		autoResultsMFC thee = ExperimentMFC_extractResults (me);
		praat_new (thee.transfer(), my name);
	}
END

/***** RESULTS_MFC *****/

DIRECT (ResultsMFC_getNumberOfTrials)
	iam_ONLY (ResultsMFC);
	Melder_information (Melder_integer (my numberOfTrials));
END

FORM (ResultsMFC_getResponse, L"ResultsMFC: Get response", 0)
	NATURAL (L"Trial", L"1")
	OK
DO
	iam_ONLY (ResultsMFC);
	long trial = GET_INTEGER (L"Trial");
	if (trial > my numberOfTrials)
		Melder_throw ("Trial ", trial, " does not exist (maximum ", my numberOfTrials, ").");
	Melder_information (my result [trial]. response);
END

FORM (ResultsMFC_getStimulus, L"ResultsMFC: Get stimulus", 0)
	NATURAL (L"Trial", L"1")
	OK
DO
	iam_ONLY (ResultsMFC);
	long trial = GET_INTEGER (L"Trial");
	if (trial > my numberOfTrials)
		Melder_throw ("Trial ", trial, " does not exist (maximum ", my numberOfTrials, ").");
	Melder_information (my result [trial]. stimulus);
END

DIRECT (ResultsMFC_removeUnsharedStimuli)
	ResultsMFC res1 = NULL, res2 = NULL;
	WHERE (SELECTED) { if (res1) res2 = (ResultsMFC) OBJECT; else res1 = (ResultsMFC) OBJECT; }
	Melder_assert (res1 && res2);
	praat_new (ResultsMFC_removeUnsharedStimuli (res1, res2), res2 -> name, L"_shared");
END

DIRECT (ResultsMFC_to_Categories_stimuli)
	WHERE (SELECTED) {
		iam_LOOP (ResultsMFC);
		autoCategories thee = ResultsMFC_to_Categories_stimuli (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (ResultsMFC_to_Categories_responses)
	WHERE (SELECTED) {
		iam_LOOP (ResultsMFC);
		autoCategories thee = ResultsMFC_to_Categories_responses (me);
		praat_new (thee.transfer(), my name);
	}
END

DIRECT (ResultsMFCs_to_Table)
	autoCollection collection = Collection_create (classResultsMFC, 100);
	Collection_dontOwnItems (collection.peek());
	WHERE (SELECTED) {
		iam_LOOP (ResultsMFC);
		Collection_addItem (collection.peek(), me);
	}
	autoTable thee = ResultsMFCs_to_Table (collection.peek());
	praat_new (thee.transfer(), L"allResults");
END

/***** buttons *****/

void praat_uvafon_Exp_init (void);
void praat_uvafon_Exp_init (void) {
	Thing_recognizeClassesByName (classExperimentMFC, classResultsMFC, NULL);

	praat_addAction1 (classCategories, 0, L"Sort", 0, 0, DO_Categories_sort);
	praat_addAction1 (classCategories, 1, L"Get entropy", 0, 0, DO_Categories_getEntropy);

	praat_addAction1 (classExperimentMFC, 0, L"Run", 0, 0, DO_ExperimentMFC_run);
	praat_addAction1 (classExperimentMFC, 0, L"Extract results", 0, 0, DO_ExperimentMFC_extractResults);

	praat_addAction1 (classResultsMFC, 0, L"Query -", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 1, L"Get number of trials", 0, 1, DO_ResultsMFC_getNumberOfTrials);
	praat_addAction1 (classResultsMFC, 1, L"Get stimulus...", 0, 1, DO_ResultsMFC_getStimulus);
	praat_addAction1 (classResultsMFC, 1, L"Get response...", 0, 1, DO_ResultsMFC_getResponse);
	praat_addAction1 (classResultsMFC, 0, L"Modify", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 2, L"Remove unshared stimuli", 0, 0, DO_ResultsMFC_removeUnsharedStimuli);
	praat_addAction1 (classResultsMFC, 0, L"Convert", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 0, L"To Categories (stimuli)", 0, 0, DO_ResultsMFC_to_Categories_stimuli);
	praat_addAction1 (classResultsMFC, 0, L"To Categories (responses)", 0, 0, DO_ResultsMFC_to_Categories_responses);
	praat_addAction1 (classResultsMFC, 0, L"Collect", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 0, L"Collect to Table", 0, 0, DO_ResultsMFCs_to_Table);
}

/* End of file praat_Exp.c */
