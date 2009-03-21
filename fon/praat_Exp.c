/* praat_Exp.c
 *
 * Copyright (C) 2001-2006 Paul Boersma
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

#include "ExperimentMFC.h"
#include "RunnerMFC.h"

/***** CATEGORIES *****/

DIRECT (Categories_getEntropy)
	Melder_informationReal (Categories_getEntropy (ONLY (classCategories)), L"bits");
END

DIRECT (Categories_sort)
	WHERE (SELECTED) {
		Categories_sort (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

/***** EXPERIMENT_MFC *****/

DIRECT (ExperimentMFC_run)
	if (theCurrentPraatApplication -> batch) {
		return Melder_error1 (L"Cannot run experiments from the command line.");
	} else {
		RunnerMFC runner = NULL;
		Ordered experiments = Ordered_create ();
		if (experiments == NULL) return 0;
		WHERE (SELECTED) {
			if (! Collection_addItem (experiments, OBJECT)) {   /* This transfers ownership temporarily. */
				experiments -> size = 0;   /* Get ownership back. */
				forget (experiments);
				return 0;
			}
		}
		runner = RunnerMFC_create (theCurrentPraatApplication -> topShell, L"listening experiments", experiments);   /* Transfer ownership of experiments (ref). */
		if (! runner) return 0;
		if (! praat_installEditorN (runner, experiments)) return 0;
	}
END

DIRECT (ExperimentMFC_extractResults)
	EVERY_TO (ExperimentMFC_extractResults (OBJECT))
END

/***** RESULTS_MFC *****/

DIRECT (ResultsMFC_getNumberOfTrials)
	ResultsMFC me = ONLY (classResultsMFC);
	Melder_information1 (Melder_integer (my numberOfTrials));
END

FORM (ResultsMFC_getResponse, L"ResultsMFC: Get response", 0)
	NATURAL (L"Trial", L"1")
	OK
DO
	ResultsMFC me = ONLY (classResultsMFC);
	long trial = GET_INTEGER (L"Trial");
	if (trial > my numberOfTrials)
		return Melder_error5 (L"Trial ", Melder_integer (trial), L" does not exist (maximum ", Melder_integer (my numberOfTrials), L").");
	Melder_information1 (my result [trial]. response);
END

FORM (ResultsMFC_getStimulus, L"ResultsMFC: Get stimulus", 0)
	NATURAL (L"Trial", L"1")
	OK
DO
	ResultsMFC me = ONLY (classResultsMFC);
	long trial = GET_INTEGER (L"Trial");
	if (trial > my numberOfTrials)
		return Melder_error5 (L"Trial ", Melder_integer (trial), L" does not exist (maximum ", Melder_integer (my numberOfTrials), L").");
	Melder_information1 (my result [trial]. stimulus);
END

DIRECT (ResultsMFC_removeUnsharedStimuli)
	ResultsMFC res1 = NULL, res2 = NULL;
	WHERE (SELECTED) { if (res1) res2 = OBJECT; else res1 = OBJECT; }
	Melder_assert (res1 && res2);
	if (! praat_new2 (ResultsMFC_removeUnsharedStimuli (res1, res2), res2 -> name, L"_shared")) return 0;
END

DIRECT (ResultsMFC_to_Categories_stimuli)
	EVERY_TO (ResultsMFC_to_Categories_stimuli (OBJECT))
END

DIRECT (ResultsMFC_to_Categories_responses)
	EVERY_TO (ResultsMFC_to_Categories_responses (OBJECT))
END

DIRECT (ResultsMFCs_to_Table)
	Collection me = Collection_create (classResultsMFC, 100);
	if (! me) return 0;
	WHERE (SELECTED)
		if (! Collection_addItem (me, OBJECT)) { my size = 0; forget (me); return 0; }
	if (! praat_new1 (ResultsMFCs_to_Table (me), L"allResults")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

/***** buttons *****/

void praat_uvafon_Exp_init (void);
void praat_uvafon_Exp_init (void) {
	Thing_recognizeClassesByName (classExperimentMFC, classResultsMFC, NULL);

	praat_addAction1 (classCategories, 0, L"Sort", 0, 0, DO_Categories_sort);
	praat_addAction1 (classCategories, 1, L"Get entropy", 0, 0, DO_Categories_getEntropy);

	praat_addAction1 (classExperimentMFC, 0, L"Run", 0, 0, DO_ExperimentMFC_run);
	praat_addAction1 (classExperimentMFC, 0, L"Extract results", 0, 0, DO_ExperimentMFC_extractResults);

	praat_addAction1 (classResultsMFC, 0, L"Query -          ", 0, 0, 0);
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
