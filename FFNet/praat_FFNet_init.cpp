/* praat_FFNet_init.cpp
 *
 * Copyright (C) 1994-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020408 GPL
 djmw 20020408 added FFNet_help
 djmw 20030701 Removed non-GPL minimizations
 djmw 20040526 Removed bug in FFNet_drawCostHistory interface.
 djmw 20041123 Latest modification
 djmw 20061218 Changed to Melder_information<x> format.
 djmw 20080902 Melder_error<1...>
 djmw 20071011 REQUIRE requires U"".
 djmw 20071024 Use MelderString_append in FFNet_createNameFromTopology
 djmw 20100511 FFNet query outputs
*/

#include <math.h>
#include "praat.h"
#include "Discriminant.h"
#include "PCA.h"
#include "Minimizers.h"
#include "FFNet_Eigen.h"
#include "FFNet_Matrix.h"
#include "FFNet_Pattern.h"
#include "FFNet_Activation_Categories.h"
#include "FFNet_Pattern_Activation.h"
#include "FFNet_Pattern_Categories.h"

/* Routines to be removed sometime in the future:
20040422, 2.4.04: FFNet_drawWeightsToLayer  use FFNet_drawWeights
20040422, 2.4.04: FFNet_weightsToMatrix use FFNet_extractWeights
*/

#undef iam
#define iam iam_LOOP

static char32 const *QUERY_BUTTON   = U"Query -";
static char32 const *DRAW_BUTTON     = U"Draw -";
static char32 const *MODIFY_BUTTON  = U"Modify -";
static char32 const *EXTRACT_BUTTON = U"Extract -";

/**************** New FFNet ***************************/

static void FFNet_create_addCommonFields_inputOutput (UiForm dia) {
	NATURAL (U"Number of inputs", U"4")
	NATURAL (U"Number of outputs", U"3")
}

static void FFNet_create_checkCommonFields_inputOutput (UiForm dia, long *numberOfInputs, long *numberOfOutputs) {
	*numberOfInputs = GET_INTEGER (U"Number of inputs");
	*numberOfOutputs = GET_INTEGER (U"Number of outputs");
}

static void FFNet_create_addCommonFields_hidden (UiForm dia) {
	INTEGER (U"Number of units in hidden layer 1", U"0")
	INTEGER (U"Number of units in hidden layer 2", U"0")
}

static void FFNet_create_checkCommonFields_hidden (UiForm dia, 	long *numberOfHidden1, long *numberOfHidden2) {
	*numberOfHidden1 = GET_INTEGER (U"Number of units in hidden layer 1");
	*numberOfHidden2 = GET_INTEGER (U"Number of units in hidden layer 2");
	if (*numberOfHidden1 < 0 || *numberOfHidden2 < 0) {
		Melder_throw (U"The number of units in a hidden layer must be geater equal zero");
	}
}

static void FFNet_create_addCommonFields (UiForm dia) {
	FFNet_create_addCommonFields_inputOutput (dia);
	FFNet_create_addCommonFields_hidden (dia);
}

static void FFNet_create_checkCommonFields (UiForm dia, long *numberOfInputs, long *numberOfOutputs,
        long *numberOfHidden1, long *numberOfHidden2) {
	FFNet_create_checkCommonFields_inputOutput (dia, numberOfInputs, numberOfOutputs);
	FFNet_create_checkCommonFields_hidden (dia, numberOfHidden1, numberOfHidden2);
}

FORM (FFNet_create, U"Create FFNet", U"Create FFNet...")
	WORD (U"Name", U"4-3")
	FFNet_create_addCommonFields (dia);
	OK
DO
	long numberOfInputs, numberOfOutputs, numberOfHidden1, numberOfHidden2;
	FFNet_create_checkCommonFields (dia, &numberOfInputs, &numberOfOutputs, &numberOfHidden1, &numberOfHidden2);
	autoFFNet thee = FFNet_create (numberOfInputs, numberOfHidden1, numberOfHidden2, numberOfOutputs, 0);
	praat_new (thee.transfer(), GET_STRING (U"Name"));
END

FORM (FFNet_create_linearOutputs, U"Create FFNet", U"Create FFNet (linear outputs)...")
	WORD (U"Name", U"4-3L")
	FFNet_create_addCommonFields (dia);
	OK
DO
	long numberOfInputs, numberOfOutputs, numberOfHidden1, numberOfHidden2;
	FFNet_create_checkCommonFields (dia, &numberOfInputs, &numberOfOutputs, &numberOfHidden1, &numberOfHidden2);
	autoFFNet thee = FFNet_create (numberOfInputs, numberOfHidden1, numberOfHidden2, numberOfOutputs, 1);
	praat_new (thee.transfer(), GET_STRING (U"Name"));
END

FORM (FFNet_createIrisExample, U"Create iris example", U"Create iris example...")
	LABEL (U"", U"For the feedforward neural net we need to know the:")
	FFNet_create_addCommonFields_hidden (dia);
	OK
DO
	long numberOfHidden1, numberOfHidden2;
	FFNet_create_checkCommonFields_hidden (dia, &numberOfHidden1, &numberOfHidden2);
	autoCollection thee = FFNet_createIrisExample (numberOfHidden1, numberOfHidden2);
	praat_new (thee.transfer());
END

DIRECT (FFNet_getNumberOfInputs)
	LOOP {
		iam (FFNet);
		Melder_information (my nUnitsInLayer[0], U" units");
	}
END

DIRECT (FFNet_getNumberOfOutputs)
	LOOP {
		iam (FFNet);
		Melder_information (my nUnitsInLayer[my nLayers], U" units");
	}
END

FORM (FFNet_getNumberOfHiddenUnits, U"FFNet: Get number of hidden units", U"FFNet: Get number of hidden units...")
	NATURAL (U"Hidden layer number", U"1")
	OK
DO
	LOOP {
		iam (FFNet);
		long layerNumber = GET_INTEGER (U"Hidden layer number");
		long numberOfUnits = 0;
		if (layerNumber > 0 && layerNumber <= my nLayers - 1) {
			numberOfUnits = my nUnitsInLayer[layerNumber];
		}
		Melder_information (numberOfUnits, U" units");
	}
END

FORM (FFNet_getCategoryOfOutputUnit, U"FFNet: Get category of output unit", U"")
	NATURAL (U"Output unit", U"1")
	OK
	DO
	LOOP {
		iam (FFNet);
		long unit = GET_INTEGER (U"Output unit");
		if (unit > my outputCategories -> size) {
			Melder_throw (U"Output unit cannot be larger than ", my outputCategories -> size, U".");
		}
		SimpleString ss = (SimpleString) my outputCategories -> item[unit];
		Melder_information (ss -> string);
	}
END

FORM (FFNet_getOutputUnitOfCategory, U"FFNet: Get output unit of category", U"")
	SENTENCE (U"Category", U"u")
	OK
DO
	LOOP {
		iam (FFNet);
		char32 *category = GET_STRING (U"Category");
		long index = 0;
		for (long i = 1; i <= my outputCategories -> size; i++) {
			SimpleString s = (SimpleString) my outputCategories -> item[i];
			if (Melder_equ (s -> string, category)) {
				index = i;
				break;
			}
		}
		Melder_information (index);
	}
END

FORM (FFNet_getBias, U"FFNet: Get bias", 0)
	NATURAL (U"Layer", U"1")
	NATURAL (U"Unit", U"1")
	OK
DO
	LOOP {
		iam (FFNet);
		long layer = GET_INTEGER (U"Layer");
		long unit = GET_INTEGER (U"Unit");
		double bias = FFNet_getBias (me, layer, unit);
		Melder_information (bias, U"(bias for unit ", unit, U"in layer ", layer, U").");
	}
END

FORM (FFNet_setBias, U"FFNet: Set bias", 0)
	NATURAL (U"Layer", U"1")
	NATURAL (U"Unit", U"1")
	REAL (U"Value", U"0.0")
	OK
DO
	LOOP {
		iam (FFNet);
		FFNet_setBias (me, GET_INTEGER (U"Layer"), GET_INTEGER (U"Unit"), GET_REAL (U"Value"));
	}
END

FORM (FFNet_getWeight, U"FFNet: Get weight", 0)
	NATURAL (U"Layer", U"1")
	NATURAL (U"Unit", U"1")
	NATURAL (U"Unit from", U"1")
	OK
DO
	LOOP {
		iam (FFNet);
		long layer = GET_INTEGER (U"Layer");
		long unit = GET_INTEGER (U"Unit");
		long unitf = GET_INTEGER (U"Unit from");
		double w = FFNet_getWeight (me, layer, unit, unitf);
		Melder_information (w, U"(weight between unit ", unit,
			U" in layer ", layer, U", and unit ", unitf, U"in layer ",
			layer - 1);
	}
END

FORM (FFNet_setWeight, U"FFNet: Set weight", 0)
	NATURAL (U"Layer", U"1")
	NATURAL (U"Unit", U"1")
	NATURAL (U"Unit (from)", U"1")
	REAL (U"Value", U"0.0")
	OK
DO
	LOOP {
		iam (FFNet);
		long layer = GET_INTEGER (U"Layer");
		long unit = GET_INTEGER (U"Unit");
		long unitf = GET_INTEGER (U"Unit (from)");
		FFNet_setWeight (me, layer, unit, unitf, GET_REAL (U"Value"));
	}
END

FORM (FFNet_getNumberOfHiddenWeights, U"FFNet: Get number of hidden weights", U"FFNet: Get number of hidden weights...")
	NATURAL (U"Hidden layer number", U"1")
	OK
DO
	LOOP {
		iam (FFNet);
		long layerNumber = GET_INTEGER (U"Hidden layer number");
		long numberOfWeights = 0;
		if (layerNumber > 0 && layerNumber <= my nLayers - 1) {
			numberOfWeights = my nUnitsInLayer[layerNumber] * (my nUnitsInLayer[layerNumber - 1] + 1);
		}
		Melder_information (numberOfWeights, U" weights (including biases)");
	}
END

DIRECT (FFNet_getNumberOfOutputWeights)
	LOOP {
		iam (FFNet);
		Melder_information (my nUnitsInLayer[my nLayers] * (my nUnitsInLayer[my nLayers - 1] + 1),
		U" weights");
	}
END

/**************** New Pattern ***************************/

FORM (Pattern_create, U"Create Pattern", 0)
	WORD (U"Name", U"1x1")
	NATURAL (U"Dimension of a pattern", U"1")
	NATURAL (U"Number of patterns", U"1")
	OK
DO
	praat_new (Pattern_create (GET_INTEGER (U"Number of patterns"),
		GET_INTEGER (U"Dimension of a pattern")), GET_STRING (U"Name"));
END

/**************** New Categories ***************************/

FORM (Categories_create, U"Create Categories", U"")
	WORD (U"Name", U"empty")
	OK
DO
	autoCategories thee = Categories_create ();
	praat_new (thee.transfer(), GET_STRING (U"Name"));
END

DIRECT (FFNet_help) Melder_help (U"Feedforward neural networks"); END

DIRECT (FFNet_getMinimum)
	LOOP {
		iam (FFNet);
		Melder_information (FFNet_getMinimum (me));
	}
END

FORM (FFNet_reset, U"FFNet: Reset", U"FFNet: Reset...")
	LABEL (U"", U"Warning: this command destroys all previous learning.")
	LABEL (U"", U"New weights will be randomly chosen from the interval [-range, +range].")
	POSITIVE (U"Range", U"0.1")
	OK
DO
	LOOP {
		iam (FFNet);
		FFNet_reset (me, GET_REAL (U"Range"));
		praat_dataChanged (OBJECT);
	}
END

FORM (FFNet_selectBiasesInLayer, U"FFNet: Select biases", U"FFNet: Select biases...")
	LABEL (U"", U"WARNING: This command induces very specific behaviour ")
	LABEL (U"", U"during a following learning phase.")
	NATURAL (U"Layer number", U"1")
	OK
	DO
	LOOP {
		iam (FFNet);
		FFNet_selectBiasesInLayer (me, GET_INTEGER (U"Layer number"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (FFNet_selectAllWeights)
	LOOP {
		iam (FFNet);
		FFNet_selectAllWeights (me);
		praat_dataChanged (me);
	}
END

DIRECT (FFNet_drawTopology)
	autoPraatPicture picture;
	LOOP {
		iam (FFNet);
		FFNet_drawTopology (me, GRAPHICS);
	}
END

FORM (FFNet_drawWeightsToLayer, U"FFNet: Draw weights to layer", 0)
	LABEL (U"", U"Warning: Disapproved. Use \"Draw weights..\" instead.")
	NATURAL (U"Layer number", U"1")
	RADIO (U"Scale", 1)
	RADIOBUTTON (U"by maximum of all weights to layer")
	RADIOBUTTON (U"by maximum weight from 'from-unit'")
	RADIOBUTTON (U"by maximum weight to 'to-unit'")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FFNet);
		FFNet_drawWeightsToLayer (me, GRAPHICS, GET_INTEGER (U"Layer number"),
			GET_INTEGER (U"Scale"), GET_INTEGER (U"Garnish"));
	}
END

FORM (FFNet_drawWeights, U"FFNet: Draw weights", U"FFNet: Draw weights...")
	NATURAL (U"Layer number", U"1")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FFNet);
		FFNet_drawWeights (me, GRAPHICS, GET_INTEGER (U"Layer number"), GET_INTEGER (U"Garnish"));
	}
END

FORM (FFNet_drawCostHistory, U"FFNet: Draw cost history", U"FFNet: Draw cost history...")
	INTEGER (U"left Iteration_range", U"0")
	INTEGER (U"right Iteration_range", U"0")
	REAL (U"left Cost_range", U"0.0")
	REAL (U"right Cost_range", U"0.0")
	BOOLEAN (U"Garnish", 1)
	OK
DO
	autoPraatPicture picture;
	LOOP {
		iam (FFNet);
		FFNet_drawCostHistory (me, GRAPHICS, GET_INTEGER (U"left Iteration_range"),
			GET_INTEGER (U"right Iteration_range"), GET_REAL (U"left Cost_range"), GET_REAL (U"right Cost_range"),
			GET_INTEGER (U"Garnish"));
	}
END

FORM (FFNet_extractWeights, U"FFNet: Extract weights", U"FFNet: Extract weights...")
	NATURAL (U"Layer number", U"1")
	OK
	DO
	LOOP {
		iam (FFNet);
		autoTableOfReal thee = FFNet_extractWeights (me, GET_INTEGER (U"Layer number"));
		praat_new (thee.transfer());
	}
END

FORM (FFNet_weightsToMatrix, U"FFNet: Weights to Matrix ", 0)
	LABEL (U"", U"Warning: Use \"Extract weights..\" instead.")
	NATURAL (U"Layer number", U"1")
	OK
DO
	LOOP {
		iam (FFNet);
		autoMatrix thee = FFNet_weightsToMatrix (me, GET_INTEGER (U"Layer number"), 0);
		praat_new (thee.transfer(), my name);
	}
END

/******************* FFNet && Activation *************************************/

FORM (FFNet_Activation_to_Categories, U"FFNet & Activation: To Categories", 0)
	RADIO (U"Kind of labeling", 1)
	RADIOBUTTON (U"Winner-takes-all")
	RADIOBUTTON (U"Stochastic")
	OK
DO
	FFNet me = FIRST (FFNet);
	Activation thee = FIRST (Activation);
	autoCategories him = FFNet_Activation_to_Categories (me, thee, GET_INTEGER (U"Kind of labeling"));
	praat_new (him.transfer(), my name, U"_", thy name);
END

/******************* FFNet && Eigen ******************************************/

FORM (FFNet_Eigen_drawIntersection, U"FFnet & Eigen: Draw intersection", 0)
	INTEGER (U"X-component", U"1")
	INTEGER (U"Y-component", U"2")
	REAL (U"xmin", U"0.0")
	REAL (U"xmax", U"0.0")
	REAL (U"ymin", U"0.0")
	REAL (U"ymax", U"0.0")
	OK
DO
	FFNet me = FIRST (FFNet);
	Eigen thee = FIRST (Eigen);
	autoPraatPicture picture;
	long pcx = GET_INTEGER (U"X-component");
	long pcy = GET_INTEGER (U"Y-component");
	REQUIRE (pcx != 0 && pcy != 0, U"X and Y component must differ from 0.")
	FFNet_Eigen_drawIntersection (me, thee, GRAPHICS, pcx, pcy, GET_REAL (U"xmin"), GET_REAL (U"xmax"),
		GET_REAL (U"ymin"), GET_REAL (U"ymax"));
END

FORM (FFNet_PCA_drawDecisionPlaneInEigenspace, U"FFNet & PCA: Draw decision plane", U"")
	NATURAL (U"Unit number", U"1")
	NATURAL (U"Layer number", U"1")
	NATURAL (U"Horizontal eigenvector number", U"1")
	NATURAL (U"Vertical eigenvector number", U"2")
	REAL (U"left Horizontal range", U"0.0")
	REAL (U"right Horizontal range", U"0.0")
	REAL (U"left Vertical range", U"0.0")
	REAL (U"right Vertical range", U"0.0")
	OK
DO
	autoPraatPicture picture;
	FFNet me = FIRST (FFNet);
	PCA thee = FIRST (PCA);
	FFNet_Eigen_drawDecisionPlaneInEigenspace (me, thee,
		GRAPHICS, GET_INTEGER (U"Unit number"), GET_INTEGER (U"Layer number"),
		GET_INTEGER (U"Horizontal eigenvector number"),
		GET_INTEGER (U"Vertical eigenvector number"), GET_REAL (U"left Horizontal range"),
		GET_REAL (U"right Horizontal range"), GET_REAL (U"left Vertical range"),
		GET_REAL (U"right Vertical range"));
END

/************************* FFNet && Categories **********************************/

DIRECT (FFNet_Categories_to_Activation)
	FFNet me = FIRST (FFNet);
	Categories thee = FIRST (Categories);
	autoActivation him = FFNet_Categories_to_Activation (me, thee);
	praat_new (him.transfer(), my name);
END

/************************* FFNet && Matrix **********************************/

FORM (FFNet_weightsFromMatrix, U"Replace weights by values from Matrix", 0)
	NATURAL (U"Layer", U"1")
	OK
DO
	FFNet me = FIRST (FFNet);
	Matrix thee = FIRST (Matrix);
	autoFFNet him = FFNet_weightsFromMatrix (me, thee, GET_INTEGER (U"Layer"));
	praat_new (him.transfer(), my name);
END

/************************* FFNet && Pattern **********************************/

FORM (FFNet_Pattern_drawActivation, U"Draw an activation", 0)
	NATURAL (U"Pattern (row) number", U"1");
	OK
DO
	autoPraatPicture picture;
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	FFNet_Pattern_drawActivation (me, thee, GRAPHICS, GET_INTEGER (U"Pattern"));
END

FORM (FFNet_Pattern_to_Activation, U"To activations in layer", 0)
	NATURAL (U"Layer", U"1")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	autoActivation him = FFNet_Pattern_to_Activation (me, thee, GET_INTEGER (U"Layer"));
	praat_new (him.transfer(), my name, U"_", thy name);
END

DIRECT (hint_FFNet_and_Pattern_classify)
	Melder_information (U"You can use the FFNet as a classifier by selecting a\n"
		"FFNet and a Pattern together and choosing \"To Categories...\".");
END

DIRECT (hint_FFNet_and_Pattern_and_Categories_learn)
	Melder_information (U"You can teach a FFNet to classify by selecting a\n"
		"FFNet, a Pattern and a Categories together and choosing \"Learn...\".");
END

FORM (FFNet_Pattern_to_Categories, U"FFNet & Pattern: To Categories", U"FFNet & Pattern: To Categories...")
	RADIO (U"Determine output category as", 1)
	RADIOBUTTON (U"Winner-takes-all")
	RADIOBUTTON (U"Stochastic")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	autoCategories him = FFNet_Pattern_to_Categories (me, thee, GET_INTEGER (U"Determine output category as"));
	praat_new (him.transfer(), my name, U"_", thy name);
END

/*********** FFNet Pattern Activation **********************************/

FORM (FFNet_Pattern_Activation_getCosts_total, U"FFNet & Pattern & Activation: Get total costs", U"FFNet & Pattern & Activation: Get total costs...")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Activation him = FIRST (Activation);
	Melder_information (FFNet_Pattern_Activation_getCosts_total (me, thee, him, GET_INTEGER (U"Cost function")));
END

FORM (FFNet_Pattern_Activation_getCosts_average, U"FFNet & Pattern & Activation: Get average costs", U"FFNet & Pattern & Activation: Get average costs...")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Activation him = FIRST (Activation);
	Melder_information (FFNet_Pattern_Activation_getCosts_average (me, thee, him, GET_INTEGER (U"Cost function")));
END

FORM (FFNet_Pattern_Activation_learnSD, U"FFNet & Pattern & Activation: Learn slow", 0)
	NATURAL (U"Maximum number of epochs", U"100")
	POSITIVE (U"Tolerance of minimizer", U"1e-7")
	LABEL (U"Specifics", U"Specific for this minimization")
	POSITIVE (U"Learning rate", U"0.1")
	REAL (U"Momentum", U"0.9")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Activation him = FIRST (Activation);
	struct structSteepestDescentMinimizer_parameters p;
	p.eta = GET_REAL (U"Learning rate");
	p.momentum = GET_REAL (U"Momentum");
	return FFNet_Pattern_Activation_learnSD (me, thee, him, GET_INTEGER (U"Maximum number of epochs"),
			GET_REAL (U"Tolerance of minimizer"), & p, GET_INTEGER (U"Cost function"));
END

FORM (FFNet_Pattern_Activation_learnSM, U"FFNet & Pattern & Activation: Learn", 0)
	NATURAL (U"Maximum number of epochs", U"100")
	POSITIVE (U"Tolerance of minimizer", U"1e-7")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Activation him = FIRST (Activation);
	return FFNet_Pattern_Activation_learnSM (me, thee, him, GET_INTEGER (U"Maximum number of epochs"),
		GET_REAL (U"Tolerance of minimizer"), nullptr,
		GET_INTEGER (U"Cost function"));
END

/*********** FFNet Pattern Categories **********************************/

FORM (FFNet_Pattern_Categories_getCosts_total, U"FFNet & Pattern & Categories: Get total costs", U"FFNet & Pattern & Categories: Get total costs...")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Categories him = FIRST (Categories);
	Melder_information (FFNet_Pattern_Categories_getCosts_total (me, thee, him,
		GET_INTEGER (U"Cost function")));
END

FORM (FFNet_Pattern_Categories_getCosts_average, U"FFNet & Pattern & Categories: Get average costs", U"FFNet & Pattern & Categories: Get average costs...")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Categories him = FIRST (Categories);
	Melder_information (FFNet_Pattern_Categories_getCosts_average (me, thee, him,
		GET_INTEGER (U"Cost function")));
END

FORM (Pattern_Categories_to_FFNet, U"Pattern & Categories: To FFNet", U"Pattern & Categories: To FFNet...")
	INTEGER (U"Number of units in hidden layer 1", U"0")
	INTEGER (U"Number of units in hidden layer 2", U"0")
	OK
DO
	Pattern me = FIRST (Pattern);
	Categories thee = FIRST (Categories);
	long nHidden1 = GET_INTEGER (U"Number of units in hidden layer 1");
	long nHidden2 = GET_INTEGER (U"Number of units in hidden layer 2");

	if (nHidden1 < 1) {
		nHidden1 = 0;
	}
	if (nHidden2 < 1) {
		nHidden2 = 0;
	}
	autoCategories uniq = Categories_selectUniqueItems (thee, 1);
	long numberOfOutputs = uniq -> size;
	if (numberOfOutputs < 1) Melder_throw (U"There are not enough categories in the Categories.\n"
		U"Please try again with more categories in the Categories.");

	autoFFNet ffnet = FFNet_create (my nx, nHidden1, nHidden2, numberOfOutputs, 0);
	FFNet_setOutputCategories (ffnet.peek(), uniq.peek());
	autostring32 ffnetName = FFNet_createNameFromTopology (ffnet.peek());
	praat_new (ffnet.transfer(), ffnetName.peek());
END

FORM (FFNet_Pattern_Categories_learnSM, U"FFNet & Pattern & Categories: Learn", U"FFNet & Pattern & Categories: Learn...")
	NATURAL (U"Maximum number of epochs", U"100")
	POSITIVE (U"Tolerance of minimizer", U"1e-7")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Categories him = FIRST (Categories);
	FFNet_Pattern_Categories_learnSM (me, thee, him, GET_INTEGER (U"Maximum number of epochs"),
		GET_REAL (U"Tolerance of minimizer"), nullptr, GET_INTEGER (U"Cost function"));
END

FORM (FFNet_Pattern_Categories_learnSD, U"FFNet & Pattern & Categories: Learn slow", U"FFNet & Pattern & Categories: Learn slow...")
	NATURAL (U"Maximum number of epochs", U"100")
	POSITIVE (U"Tolerance of minimizer", U"1e-7")
	LABEL (U"Specifics", U"Specific for this minimization")
	POSITIVE (U"Learning rate", U"0.1")
	REAL (U"Momentum", U"0.9")
	RADIO (U"Cost function", 1)
	RADIOBUTTON (U"Minimum-squared-error")
	RADIOBUTTON (U"Minimum-cross-entropy")
	OK
DO
	FFNet me = FIRST (FFNet);
	Pattern thee = FIRST (Pattern);
	Categories him = FIRST (Categories);
	struct structSteepestDescentMinimizer_parameters p;
	p.eta = GET_REAL (U"Learning rate");
	p.momentum = GET_REAL (U"Momentum");
	FFNet_Pattern_Categories_learnSD (me, thee, him, GET_INTEGER (U"Maximum number of epochs"),
		GET_REAL (U"Tolerance of minimizer"), &p, GET_INTEGER (U"Cost function"));
END

void praat_uvafon_FFNet_init ();
void praat_uvafon_FFNet_init () {
	Thing_recognizeClassesByName (classFFNet, NULL);

	praat_addMenuCommand (U"Objects", U"New", U"Feedforward neural networks", 0, 0, 0);
	praat_addMenuCommand (U"Objects", U"New", U"Feedforward neural networks", 0, 1, DO_FFNet_help);
	praat_addMenuCommand (U"Objects", U"New", U"-- FFNet --", 0, 1, 0);
	praat_addMenuCommand (U"Objects", U"New", U"Create iris example...", 0, 1, DO_FFNet_createIrisExample);
	praat_addMenuCommand (U"Objects", U"New", U"Create FFNet...", 0, 1, DO_FFNet_create);
	praat_addMenuCommand (U"Objects", U"New", U"Advanced", 0, 1, 0);
	praat_addMenuCommand (U"Objects", U"New", U"Create Pattern...", 0, 2, DO_Pattern_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create Categories...", 0, 2, DO_Categories_create);
	praat_addMenuCommand (U"Objects", U"New", U"Create FFNet (linear outputs)...", 0, 2, DO_FFNet_create_linearOutputs);

	praat_addAction1 (classFFNet, 0, U"FFNet help", 0, 0, DO_FFNet_help);
	praat_addAction1 (classFFNet, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, U"Draw topology", 0, 1, DO_FFNet_drawTopology);
	praat_addAction1 (classFFNet, 0, U"Draw weights...", 0, 1, DO_FFNet_drawWeights);
	praat_addAction1 (classFFNet, 0, U"Draw weights to layer...", 0,  praat_DEPTH_1 | praat_HIDDEN, DO_FFNet_drawWeightsToLayer);
	praat_addAction1 (classFFNet, 0, U"Draw cost history...", 0, 1, DO_FFNet_drawCostHistory);
	praat_addAction1 (classFFNet, 0, QUERY_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, U"Query structure", 0, 1, 0);
	praat_addAction1 (classFFNet, 1, U"Get number of outputs", 0, 2, DO_FFNet_getNumberOfOutputs);
	praat_addAction1 (classFFNet, 1, U"Get number of hidden units...", 0, 2, DO_FFNet_getNumberOfHiddenUnits);
	praat_addAction1 (classFFNet, 1, U"Get number of inputs", 0, 2, DO_FFNet_getNumberOfInputs);
	praat_addAction1 (classFFNet, 1, U"Get number of hidden weights...", 0, 2, DO_FFNet_getNumberOfHiddenWeights);
	praat_addAction1 (classFFNet, 1, U"Get number of output weights", 0, 2, DO_FFNet_getNumberOfOutputWeights);
	praat_addAction1 (classFFNet, 1, U"Get category of output unit...", 0, 2, DO_FFNet_getCategoryOfOutputUnit);
	praat_addAction1 (classFFNet, 1, U"Get output unit of category...", 0, 2, DO_FFNet_getOutputUnitOfCategory);
	praat_addAction1 (classFFNet, 0, U"-- FFNet weights --", 0, 1, 0);
	praat_addAction1 (classFFNet, 1, U"Get bias...", 0, 1, DO_FFNet_getBias);
	praat_addAction1 (classFFNet, 1, U"Get weight...", 0, 1, DO_FFNet_getWeight);
	praat_addAction1 (classFFNet, 1, U"Get minimum", 0, 1, DO_FFNet_getMinimum);
	praat_addAction1 (classFFNet, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 1, U"Set bias...", 0, 1, DO_FFNet_setBias);
	praat_addAction1 (classFFNet, 1, U"Set weight...", 0, 1, DO_FFNet_setWeight);
	praat_addAction1 (classFFNet, 1, U"Reset...", 0, 1, DO_FFNet_reset);
	praat_addAction1 (classFFNet, 0, U"Select biases...", 0, 1, DO_FFNet_selectBiasesInLayer);
	praat_addAction1 (classFFNet, 0, U"Select all weights", 0, 1, DO_FFNet_selectAllWeights);
	praat_addAction1 (classFFNet, 0, EXTRACT_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, U"Extract weights...", 0, 1, DO_FFNet_extractWeights);
	praat_addAction1 (classFFNet, 0, U"Weights to Matrix...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_FFNet_weightsToMatrix);
	praat_addAction1 (classFFNet, 0, U"& Pattern: Classify?", 0, 0, DO_hint_FFNet_and_Pattern_classify);
	praat_addAction1 (classFFNet, 0, U"& Pattern & Categories: Learn?", 0, 0, DO_hint_FFNet_and_Pattern_and_Categories_learn);

	praat_addAction2 (classFFNet, 1, classActivation, 1, U"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classActivation, 1, U"To Categories...", 0, 0, DO_FFNet_Activation_to_Categories);

	praat_addAction2 (classFFNet, 1, classEigen, 1, U"Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classEigen, 1, U"Draw hyperplane intersections", 0, 0, DO_FFNet_Eigen_drawIntersection);

	praat_addAction2 (classFFNet, 1, classCategories, 1, U"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classCategories, 1, U"To Activation", 0, 0, DO_FFNet_Categories_to_Activation);

	praat_addAction2 (classFFNet, 1, classMatrix, 1, U"Modify", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classMatrix, 1, U"Weights from Matrix...", 0, 0, DO_FFNet_weightsFromMatrix);

	praat_addAction2 (classFFNet, 1, classPattern, 1, U"Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, U"Draw activation...", 0, 0, DO_FFNet_Pattern_drawActivation);
	praat_addAction2 (classFFNet, 1, classPattern, 1, U"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, U"To Categories...", 0, 0, DO_FFNet_Pattern_to_Categories);
	praat_addAction2 (classFFNet, 1, classPattern, 1, U"To Activation...", 0, 0, DO_FFNet_Pattern_to_Activation);

	praat_addAction2 (classFFNet, 1, classPCA, 1, U"Draw decision plane...", 0, 0, DO_FFNet_PCA_drawDecisionPlaneInEigenspace);

	praat_addAction2 (classPattern, 1, classCategories, 1, U"To FFNet...", 0, 0, DO_Pattern_Categories_to_FFNet);

	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, U"Get total costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_total);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, U"Get average costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_average);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, U"Learn", 0, 0, 0);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, U"Learn...", 0, 0, DO_FFNet_Pattern_Activation_learnSM);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, U"Learn slow...", 0, 0, DO_FFNet_Pattern_Activation_learnSD);

	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, U"Get total costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_total);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, U"Get average costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_average);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, U"Learn", 0, 0, 0);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, U"Learn...", 0, 0, DO_FFNet_Pattern_Categories_learnSM);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, U"Learn slow...", 0, 0, DO_FFNet_Pattern_Categories_learnSD);

	INCLUDE_MANPAGES (manual_FFNet_init)
}

/* End of file praat_FFnet_init.cpp */
