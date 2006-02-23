/* praat_Exp.c
 *
 * Copyright (C) 2001-2005 Paul Boersma
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
 * pb 2005/12/08
 */

#include "praat.h"

#include "ExperimentMFC.h"
#include "RunnerMFC.h"

/***** CATEGORIES *****/

DIRECT (Categories_getEnthropy)
	Melder_informationReal (Categories_getEnthropy (ONLY (classCategories)), "bits");
END

DIRECT (Categories_sort)
	WHERE (SELECTED) {
		Categories_sort (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

/***** EXPERIMENT_MFC *****/

DIRECT (ExperimentMFC_run)
	if (praat.batch) {
		return Melder_error ("Cannot run experiments from the command line.");
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
		runner = RunnerMFC_create (praat.topShell, "listening experiments", experiments);   /* Transfer ownership of experiments (ref). */
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
	Melder_information ("%ld", my numberOfTrials);
END

FORM (ResultsMFC_getResponse, "ResultsMFC: Get response", 0)
	NATURAL ("Trial", "1")
	OK
DO
	ResultsMFC me = ONLY (classResultsMFC);
	long trial = GET_INTEGER ("Trial");
	if (trial > my numberOfTrials) return Melder_error ("Trial %ld does not exist (maximum %ld).",
		trial, my numberOfTrials);
	Melder_information ("%s", my result [trial]. response);
END

FORM (ResultsMFC_getStimulus, "ResultsMFC: Get stimulus", 0)
	NATURAL ("Trial", "1")
	OK
DO
	ResultsMFC me = ONLY (classResultsMFC);
	long trial = GET_INTEGER ("Trial");
	if (trial > my numberOfTrials) return Melder_error ("Trial %ld does not exist (maximum %ld).",
		trial, my numberOfTrials);
	Melder_information ("%s", my result [trial]. stimulus);
END

DIRECT (ResultsMFC_removeUnsharedStimuli)
	ResultsMFC res1 = NULL, res2;
	WHERE (SELECTED) if (res1) res2 = OBJECT; else res1 = OBJECT;
	if (! praat_new (ResultsMFC_removeUnsharedStimuli (res1, res2), "%s_shared", res2 -> name)) return 0;
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
	if (! praat_new (ResultsMFCs_to_Table (me), "allResults")) {
		my size = 0; forget (me); return 0;
	}
	my size = 0; forget (me);
END

/***** buttons *****/

void praat_uvafon_Exp_init (void);
void praat_uvafon_Exp_init (void) {
	Thing_recognizeClassesByName (classExperimentMFC, classResultsMFC, NULL);

	praat_addAction1 (classCategories, 0, "Sort", 0, 0, DO_Categories_sort);
	praat_addAction1 (classCategories, 1, "Get enthropy", 0, 0, DO_Categories_getEnthropy);

	praat_addAction1 (classExperimentMFC, 0, "Run", 0, 0, DO_ExperimentMFC_run);
	praat_addAction1 (classExperimentMFC, 0, "Extract results", 0, 0, DO_ExperimentMFC_extractResults);

	praat_addAction1 (classResultsMFC, 0, "Query -          ", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 1, "Get number of trials", 0, 1, DO_ResultsMFC_getNumberOfTrials);
	praat_addAction1 (classResultsMFC, 1, "Get stimulus...", 0, 1, DO_ResultsMFC_getStimulus);
	praat_addAction1 (classResultsMFC, 1, "Get response...", 0, 1, DO_ResultsMFC_getResponse);
	praat_addAction1 (classResultsMFC, 0, "Modify", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 2, "Remove unshared stimuli", 0, 0, DO_ResultsMFC_removeUnsharedStimuli);
	praat_addAction1 (classResultsMFC, 0, "Convert", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 0, "To Categories (stimuli)", 0, 0, DO_ResultsMFC_to_Categories_stimuli);
	praat_addAction1 (classResultsMFC, 0, "To Categories (responses)", 0, 0, DO_ResultsMFC_to_Categories_responses);
	praat_addAction1 (classResultsMFC, 0, "Collect", 0, 0, 0);
	praat_addAction1 (classResultsMFC, 0, "Collect to Table", 0, 0, DO_ResultsMFCs_to_Table);
}

/* End of file praat_Exp.c */
