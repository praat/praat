/* praat_ExperimentMFC.cpp
 *
 * Copyright (C) 2001-2012,2015,2016 Paul Boersma
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

#undef iam
#define iam iam_LOOP

// MARK: - CATEGORIES

// MARK: Query

DIRECT (REAL_Categories_getEntropy) {
	iam_ONLY (Categories);
	double entropy = Categories_getEntropy (me);
	Melder_informationReal (entropy, U"bits");
END }

// MARK: Modify

DIRECT (MODIFY_Categories_sort) {
	LOOP {
		iam (Categories);
		Categories_sort (me);
		praat_dataChanged (me);
	}
END }

// MARK: - EXPERIMENT_MFC

DIRECT (WINDOW_ExperimentMFC_run) {
	if (theCurrentPraatApplication -> batch) Melder_throw (U"Cannot run experiments from the command line.");
	autoRunnerMFC runner;
	{// scope
		/*
			This `scope` comment refers to the idea that an autoThing (here, `experiments`)
			is created in the beginning of the scope and invalidated at the end of the scope (by `move`).
		*/
		autoExperimentMFCList experiments = ExperimentMFCList_create ();
		WHERE (SELECTED) {
			iam_LOOP (ExperimentMFC);
			Melder_assert (my classInfo == classExperimentMFC);
			experiments -> addItem_ref (me);
		}
		Melder_assert (experiments->size >= 1);
		Melder_assert (experiments->at [1] -> classInfo == classExperimentMFC);
		Melder_assert (experiments->at [experiments->size] -> classInfo == classExperimentMFC);
		runner = RunnerMFC_create (U"listening experiments", experiments.move());
		/*
			Now that `experiments` has been moved, it has become invalid.
			We help the compiler notice this by ending the scope here:
		*/
	}
	/*
		As a result of the scope braces above, the compiler will now protest
		if we refer to `experiments` in the next line. So instead we refer to the `runner`-internal experiments,
		which are still in scope and haven't been invalidated:
	*/
	praat_installEditorN (runner.get(), runner -> experiments->asDaataList());   // refer to the moved version!
	runner.releaseToUser();
END }

DIRECT (NEW_ExperimentMFC_extractResults) {
	LOOP {
		iam (ExperimentMFC);
		autoResultsMFC thee = ExperimentMFC_extractResults (me);
		praat_new (thee.move(), my name);
	}
END }

// MARK: - RESULTS_MFC

DIRECT (INTEGER_ResultsMFC_getNumberOfTrials) {
	iam_ONLY (ResultsMFC);
	Melder_information (my numberOfTrials);
END }

FORM (STRING_ResultsMFC_getResponse, U"ResultsMFC: Get response", nullptr) {
	NATURAL (U"Trial", U"1")
	OK
DO
	iam_ONLY (ResultsMFC);
	long trial = GET_INTEGER (U"Trial");
	if (trial > my numberOfTrials)
		Melder_throw (U"Trial ", trial, U" does not exist (maximum ", my numberOfTrials, U").");
	Melder_information (my result [trial]. response);
END }

FORM (STRING_ResultsMFC_getStimulus, U"ResultsMFC: Get stimulus", nullptr) {
	NATURAL (U"Trial", U"1")
	OK
DO
	iam_ONLY (ResultsMFC);
	long trial = GET_INTEGER (U"Trial");
	if (trial > my numberOfTrials)
		Melder_throw (U"Trial ", trial, U" does not exist (maximum ", my numberOfTrials, U").");
	Melder_information (my result [trial]. stimulus);
END }

DIRECT (NEW1_ResultsMFC_removeUnsharedStimuli) {
	ResultsMFC res1 = nullptr, res2 = nullptr;
	LOOP ( res1 ? res2 : res1 ) = (ResultsMFC) OBJECT;
	Melder_assert (res1 && res2);
	autoResultsMFC result = ResultsMFC_removeUnsharedStimuli (res1, res2);
	praat_new (result.move(), res2 -> name, U"_shared");
END }

DIRECT (NEW_ResultsMFC_to_Categories_stimuli) {
	LOOP {
		iam (ResultsMFC);
		autoCategories thee = ResultsMFC_to_Categories_stimuli (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW_ResultsMFC_to_Categories_responses) {
	LOOP {
		iam (ResultsMFC);
		autoCategories thee = ResultsMFC_to_Categories_responses (me);
		praat_new (thee.move(), my name);
	}
END }

DIRECT (NEW1_ResultsMFCs_to_Table) {
	OrderedOf<structResultsMFC> collection;
	LOOP {
		iam (ResultsMFC);
		collection. addItem_ref (me);
	}
	autoTable thee = ResultsMFCs_to_Table (& collection);
	praat_new (thee.move(), U"allResults");
END }

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
