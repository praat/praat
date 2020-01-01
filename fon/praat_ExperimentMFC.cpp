/* praat_ExperimentMFC.cpp
 *
 * Copyright (C) 2001-2007,2009-2012,2015-2018 Paul Boersma
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

#include "praat_ExperimentMFC.h"

// MARK: - CATEGORIES

// MARK: Query

DIRECT (REAL_Categories_getEntropy) {
	NUMBER_ONE (Categories)
		double result = Categories_getEntropy (me);
	NUMBER_ONE_END (U" bits")
}

// MARK: Modify

DIRECT (MODIFY_Categories_sort) {
	MODIFY_EACH (Categories)
		Categories_sort (me);
	MODIFY_EACH_END
}

// MARK: - EXPERIMENT_MFC

DIRECT (WINDOW_ExperimentMFC_run) {
	if (theCurrentPraatApplication -> batch)
		Melder_throw (U"Cannot run experiments from the command line.");
	autoRunnerMFC runner;
	{// scope
		/*
			This `scope` comment refers to the idea that an autoThing (here, `list`)
			is created at the beginning of the scope and invalidated at the end of the scope (by `move`).
		*/
		FIND_TYPED_LIST (ExperimentMFC, ExperimentMFCList)
		Melder_assert (list->size >= 1);
		Melder_assert (list->at [1] -> classInfo == classExperimentMFC);
		Melder_assert (list->at [list->size] -> classInfo == classExperimentMFC);
		runner = RunnerMFC_create (U"listening experiments", list.move());
		/*
			Now that `list` has been moved, it has become invalid.
			We help the compiler notice this by ending the scope here:
		*/
	}
	/*
		As a result of the scope braces above, the compiler will now protest
		if we refer to `list` in the next line. So instead we refer to the `runner`-internal experiments,
		which are still in scope and haven't been invalidated:
	*/
	praat_installEditorN (runner.get(), runner -> experiments->asDaataList());   // refer to the moved version!
	runner.releaseToUser();
END }

DIRECT (NEW_ExperimentMFC_extractResults) {
	CONVERT_EACH (ExperimentMFC)
		autoResultsMFC result = ExperimentMFC_extractResults (me);
	CONVERT_EACH_END (my name.get())
}

// MARK: - RESULTS_MFC

DIRECT (INTEGER_ResultsMFC_getNumberOfTrials) {
	NUMBER_ONE (ResultsMFC)
		integer result = my numberOfTrials;
	NUMBER_ONE_END (U" trials")
}

FORM (STRING_ResultsMFC_getResponse, U"ResultsMFC: Get response", nullptr) {
	NATURAL (trial, U"Trial", U"1")
	OK
DO
	STRING_ONE (ResultsMFC)
		if (trial > my numberOfTrials)
			Melder_throw (U"Trial ", trial, U" does not exist (maximum ", my numberOfTrials, U").");
		conststring32 result = my result [trial]. response.get();
	STRING_ONE_END
}

FORM (STRING_ResultsMFC_getStimulus, U"ResultsMFC: Get stimulus", nullptr) {
	NATURAL (trial, U"Trial", U"1")
	OK
DO
	STRING_ONE (ResultsMFC)
		if (trial > my numberOfTrials)
			Melder_throw (U"Trial ", trial, U" does not exist (maximum ", my numberOfTrials, U").");
		conststring32 result = my result [trial]. stimulus.get();
	STRING_ONE_END
}

DIRECT (NEW1_ResultsMFC_removeUnsharedStimuli) {
	CONVERT_COUPLE (ResultsMFC)
		autoResultsMFC result = ResultsMFC_removeUnsharedStimuli (me, you);
	CONVERT_COUPLE_END (your name.get(), U"_shared")
}

DIRECT (NEW_ResultsMFC_to_Categories_stimuli) {
	CONVERT_EACH (ResultsMFC)
		autoCategories result = ResultsMFC_to_Categories_stimuli (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW_ResultsMFC_to_Categories_responses) {
	CONVERT_EACH (ResultsMFC)
		autoCategories result = ResultsMFC_to_Categories_responses (me);
	CONVERT_EACH_END (my name.get())
}

DIRECT (NEW1_ResultsMFCs_to_Table) {
	CONVERT_LIST (ResultsMFC)
		autoTable result = ResultsMFCs_to_Table (& list);
	CONVERT_LIST_END (U"allResults")
}

// MARK: - buttons

void praat_ExperimentMFC_init () {
	Thing_recognizeClassesByName (classExperimentMFC, classResultsMFC, nullptr);

	praat_addAction1 (classCategories, 0, U"Sort", nullptr, 0, MODIFY_Categories_sort);
	praat_addAction1 (classCategories, 1, U"Get entropy", nullptr, 0, REAL_Categories_getEntropy);

	praat_addAction1 (classExperimentMFC, 0, U"Run", nullptr, 0, WINDOW_ExperimentMFC_run);
	praat_addAction1 (classExperimentMFC, 0, U"Extract results", nullptr, 0, NEW_ExperimentMFC_extractResults);

	praat_addAction1 (classResultsMFC, 0, U"Query -", nullptr, 0, nullptr);
	praat_addAction1 (classResultsMFC, 1, U"Get number of trials", nullptr, 1, INTEGER_ResultsMFC_getNumberOfTrials);
	praat_addAction1 (classResultsMFC, 1, U"Get stimulus...", nullptr, 1, STRING_ResultsMFC_getStimulus);
	praat_addAction1 (classResultsMFC, 1, U"Get response...", nullptr, 1, STRING_ResultsMFC_getResponse);
	praat_addAction1 (classResultsMFC, 0, U"Modify", nullptr, 0, nullptr);
	praat_addAction1 (classResultsMFC, 2, U"Remove unshared stimuli", 0, 0, NEW1_ResultsMFC_removeUnsharedStimuli);
	praat_addAction1 (classResultsMFC, 0, U"Convert", nullptr, 0, nullptr);
	praat_addAction1 (classResultsMFC, 0, U"To Categories (stimuli)", nullptr, 0, NEW_ResultsMFC_to_Categories_stimuli);
	praat_addAction1 (classResultsMFC, 0, U"To Categories (responses)", nullptr, 0, NEW_ResultsMFC_to_Categories_responses);
	praat_addAction1 (classResultsMFC, 0, U"Collect", nullptr, 0, nullptr);
	praat_addAction1 (classResultsMFC, 0, U"Collect to Table", nullptr, 0, NEW1_ResultsMFCs_to_Table);
}

/* End of file praat_ExperimentMFC.cpp */
