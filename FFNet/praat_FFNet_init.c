/* praat_FFNet_init.c
 *
 * Copyright (C) 1994-2010 David Weenink
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
 djmw 20071011 REQUIRE requires L"".
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


static wchar_t *QUERY_BUTTON   = L"Query -";
static wchar_t *DRAW_BUTTON     = L"Draw -";
static wchar_t *MODIFY_BUTTON  = L"Modify -";
static wchar_t *EXTRACT_BUTTON = L"Extract -";

/**************** New FFNet ***************************/

static void FFNet_create_addCommonFields_inputOutput (void *dia)
{
    NATURAL (L"Number of inputs", L"4")
    NATURAL (L"Number of outputs", L"3")
}

static int FFNet_create_checkCommonFields_inputOutput (void *dia, long *numberOfInputs, long *numberOfOutputs)
{
	*numberOfInputs = GET_INTEGER (L"Number of inputs");
	*numberOfOutputs = GET_INTEGER (L"Number of outputs");
	return 1;
}

static void FFNet_create_addCommonFields_hidden (void *dia)
{
    INTEGER (L"Number of units in hidden layer 1", L"0")
    INTEGER (L"Number of units in hidden layer 2", L"0")
}

static int FFNet_create_checkCommonFields_hidden (void *dia, 	long *numberOfHidden1, long *numberOfHidden2)
{
	*numberOfHidden1 = GET_INTEGER (L"Number of units in hidden layer 1");
	*numberOfHidden2 = GET_INTEGER (L"Number of units in hidden layer 2");
	if (*numberOfHidden1 < 0 || *numberOfHidden2 < 0)
	{
		return Melder_error1 (L"The number of units in a hidden layer must be greater than or equal to 0.");
	}
	return 1;
}

static void FFNet_create_addCommonFields (void *dia)
{
	FFNet_create_addCommonFields_inputOutput (dia);
    FFNet_create_addCommonFields_hidden (dia);
}

static int FFNet_create_checkCommonFields (void *dia, long *numberOfInputs, long *numberOfOutputs,
	long *numberOfHidden1, long *numberOfHidden2)
{
	return FFNet_create_checkCommonFields_inputOutput (dia, numberOfInputs, numberOfOutputs) &&
		FFNet_create_checkCommonFields_hidden (dia, numberOfHidden1, numberOfHidden2);
}

FORM (FFNet_create, L"Create FFNet", L"Create FFNet...")
	WORD (L"Name", L"4-3")
	FFNet_create_addCommonFields (dia);
    OK
DO
	FFNet thee;
	long numberOfInputs, numberOfOutputs, numberOfHidden1, numberOfHidden2;
	if (! FFNet_create_checkCommonFields (dia, &numberOfInputs, &numberOfOutputs,
		&numberOfHidden1, &numberOfHidden2)) return 0;
	thee = FFNet_create (numberOfInputs, numberOfHidden1, numberOfHidden2, numberOfOutputs, 0);
	if (thee == NULL)
	{
		return Melder_error1 (L"There was not enough memory to create the FFNet.\n"
			" Please reduce some of the numbers.");
	}
    if (! praat_new1 (thee, GET_STRING (L"Name"))) return 0;
END

FORM (FFNet_create_linearOutputs, L"Create FFNet", L"Create FFNet (linear outputs)...")
	WORD (L"Name", L"4-3L")
	FFNet_create_addCommonFields (dia);
    OK
DO
	FFNet thee;
	long numberOfInputs, numberOfOutputs, numberOfHidden1, numberOfHidden2;
	if (! FFNet_create_checkCommonFields (dia, &numberOfInputs, &numberOfOutputs,
		&numberOfHidden1, &numberOfHidden2)) return 0;
	thee = FFNet_create (numberOfInputs, numberOfHidden1, numberOfHidden2, numberOfOutputs, 1);
	if (thee == NULL)
	{
		return Melder_error1 (L"There was not enough memory to create the FFNet.\nPlease reduce some of the numbers.");
	}
    if (! praat_new1 (thee, GET_STRING (L"Name"))) return 0;
END

FORM (FFNet_createIrisExample, L"Create iris example", L"Create iris example...")
	LABEL (L"", L"For the feedforward neural net we need to know the:")
	FFNet_create_addCommonFields_hidden (dia);
	OK
DO
	long numberOfHidden1, numberOfHidden2;
	if (! FFNet_create_checkCommonFields_hidden (dia, &numberOfHidden1, &numberOfHidden2)) return 0;
	NEW (FFNet_createIrisExample (numberOfHidden1, numberOfHidden2))
END

DIRECT (FFNet_getNumberOfInputs)
	FFNet  me = ONLY(classFFNet);
	Melder_information2 (Melder_integer (my nUnitsInLayer[0]), L" units");
END

DIRECT (FFNet_getNumberOfOutputs)
	FFNet  me = ONLY(classFFNet);
	Melder_information2 (Melder_integer (my nUnitsInLayer[my nLayers]), L" units");
END

FORM (FFNet_getNumberOfHiddenUnits, L"FFNet: Get number of hidden units", L"FFNet: Get number of hidden units...")
	NATURAL (L"Hidden layer number", L"1")
	OK
DO
	FFNet  me = ONLY(classFFNet);
	long layerNumber = GET_INTEGER (L"Hidden layer number");
	long numberOfUnits = 0;

	if (layerNumber > 0 && layerNumber <= my nLayers - 1) numberOfUnits = my nUnitsInLayer[layerNumber];
	Melder_information2 (Melder_integer (numberOfUnits), L" units");
END

FORM (FFNet_getCategoryOfOutputUnit, L"FFNet: Get category of output unit", L"")
	NATURAL (L"Output unit", L"1")
	OK
DO
	FFNet me = ONLY_OBJECT;
	Categories c = my outputCategories;
	long unit = GET_INTEGER (L"Output unit");
	if (unit > c -> size) return Melder_error3 (L"Output unit cannot be larger than ", Melder_integer (c -> size), L".");
	SimpleString ss = c -> item[unit];
	Melder_information1 (ss -> string);
END

FORM (FFNet_getOutputUnitOfCategory, L"FFNet: Get output unit of category", L"")
	SENTENCE (L"Category", L"u")
	OK
DO
	FFNet me = ONLY_OBJECT;
	Categories c = my outputCategories;
	wchar_t *category = GET_STRING (L"Category");
	long index = 0;
	for (long i = 1; i <= c -> size; i++)
	{
		SimpleString s = c -> item[i];
		if (Melder_wcsequ (s -> string, category)) { index = i; break;};
	}
	Melder_information1 (Melder_integer (index));
END

FORM (FFNet_getNumberOfHiddenWeights, L"FFNet: Get number of hidden weights", L"FFNet: Get number of hidden weights...")
	NATURAL (L"Hidden layer number", L"1")
	OK
DO
	FFNet  me = ONLY(classFFNet);
	long layerNumber = GET_INTEGER (L"Hidden layer number");
	long numberOfWeights = 0;
	if (layerNumber > 0 && layerNumber <= my nLayers - 1)
	{
		numberOfWeights = my nUnitsInLayer[layerNumber] * (my nUnitsInLayer[layerNumber - 1]+1);
	}
	Melder_information2 (Melder_integer (numberOfWeights), L" weights (including biases)");
END

DIRECT (FFNet_getNumberOfOutputWeights)
	FFNet  me = ONLY(classFFNet);
	Melder_information2 (Melder_integer (my nUnitsInLayer[my nLayers] * (my nUnitsInLayer[my nLayers - 1]+1)), L" weights");
END

/**************** New Pattern ***************************/

FORM (Pattern_create, L"Create Pattern", 0)
	WORD (L"Name", L"1x1")
    NATURAL (L"Dimension of a pattern", L"1")
    NATURAL (L"Number of patterns", L"1")
    OK
DO
	if (! praat_new1 (Pattern_create (GET_INTEGER (L"Number of patterns"),
		GET_INTEGER (L"Dimension of a pattern")), GET_STRING (L"Name"))) return 0;
END

/**************** New Categories ***************************/

FORM (Categories_create, L"Create Categories", L"")
	WORD (L"Name", L"empty")
	OK
DO
	if (! praat_new1 (Categories_create (), GET_STRING (L"Name"))) return 0;
END

DIRECT (FFNet_help)
	Melder_help (L"Feedforward neural networks");
END

DIRECT (FFNet_getMinimum)
	Melder_information1 (Melder_double (FFNet_getMinimum (ONLY_OBJECT)));
END

FORM (FFNet_reset, L"FFNet: Reset", L"FFNet: Reset...")
	LABEL (L"", L"Warning: this command destroys all previous learning.")
	LABEL (L"", L"New weights will be randomly chosen from the interval [-range, +range].")
    POSITIVE (L"Range", L"0.1")
    OK
DO
    WHERE (SELECTED)
    {
    	FFNet_reset (OBJECT, GET_REAL (L"Range"));
		praat_dataChanged (OBJECT);
	}
END

FORM (FFNet_selectBiasesInLayer, L"FFNet: Select biases", L"FFNet: Select biases...")
	LABEL (L"", L"WARNING: This command induces very specific behaviour ")
	LABEL (L"", L"during a following learning phase.")
	NATURAL (L"Layer number", L"1")
	OK
DO
	WHERE (SELECTED)
	{
		FFNet_selectBiasesInLayer (OBJECT, GET_INTEGER (L"Layer number"));
		praat_dataChanged (OBJECT);
	}
END

DIRECT (FFNet_selectAllWeights)
	WHERE (SELECTED)
	{
    	FFNet_selectAllWeights (OBJECT);
		praat_dataChanged (OBJECT);
	}
END

DIRECT (FFNet_drawTopology)
    EVERY_DRAW (FFNet_drawTopology (OBJECT, GRAPHICS))
END

FORM (FFNet_drawWeightsToLayer, L"FFNet: Draw weights to layer", 0)
	LABEL (L"", L"Warning: Disapproved. Use \"Draw weights..\" instead.")
    NATURAL (L"Layer number", L"1")
	RADIO (L"Scale", 1)
	RADIOBUTTON (L"by maximum of all weights to layer")
	RADIOBUTTON (L"by maximum weight from 'from-unit'")
	RADIOBUTTON (L"by maximum weight to 'to-unit'")
    BOOLEAN (L"Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawWeightsToLayer (OBJECT, GRAPHICS,
		GET_INTEGER (L"Layer number"),
    	GET_INTEGER (L"Scale"), GET_INTEGER (L"Garnish")))
END

FORM (FFNet_drawWeights, L"FFNet: Draw weights", L"FFNet: Draw weights...")
    NATURAL (L"Layer number", L"1")
    BOOLEAN (L"Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawWeights (OBJECT, GRAPHICS,
		GET_INTEGER (L"Layer number"), GET_INTEGER (L"Garnish")))
END

FORM (FFNet_drawCostHistory, L"FFNet: Draw cost history", L"FFNet: Draw cost history...")
    INTEGER (L"left Iteration_range", L"0")
    INTEGER (L"right Iteration_range", L"0")
    REAL (L"left Cost_range", L"0.0")
    REAL (L"right Cost_range", L"0.0")
    BOOLEAN (L"Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawCostHistory (OBJECT, GRAPHICS,
	GET_INTEGER (L"left Iteration_range"), GET_INTEGER (L"right Iteration_range"),
	GET_REAL (L"left Cost_range"), GET_REAL (L"right Cost_range"), GET_INTEGER (L"Garnish")))
END

FORM (FFNet_extractWeights, L"FFNet: Extract weights", L"FFNet: Extract weights...")
    NATURAL (L"Layer number", L"1")
    OK
DO
    EVERY_TO (FFNet_extractWeights (OBJECT, GET_INTEGER (L"Layer number")))
END

FORM (FFNet_weightsToMatrix, L"FFNet: Weights to Matrix ", 0)
	LABEL (L"", L"Warning: Use \"Extract weights..\" instead.")
    NATURAL (L"Layer number", L"1")
    OK
DO
    EVERY_TO (FFNet_weightsToMatrix (OBJECT, GET_INTEGER (L"Layer number"), 0))
END

/******************* FFNet && Activation *************************************/

FORM (FFNet_Activation_to_Categories, L"FFNet & Activation: To Categories", 0)
	RADIO (L"Kind of labeling", 1)
	RADIOBUTTON (L"Winner-takes-all")
	RADIOBUTTON (L"Stochastic")
	OK
DO
	wchar_t name [200];
	praat_name2 (name, classFFNet, classActivation);
	if (! praat_new1 (FFNet_Activation_to_Categories (ONLY (classFFNet), ONLY (classActivation),
		GET_INTEGER (L"Kind of labeling")), name)) return 0;
END

/******************* FFNet && Eigen ******************************************/

FORM (FFNet_Eigen_drawIntersection, L"FFnet & Eigen: Draw intersection", 0)
    INTEGER (L"X-component", L"1")
    INTEGER (L"Y-component", L"2")
    REAL (L"xmin", L"0.0")
    REAL (L"xmax", L"0.0")
    REAL (L"ymin", L"0.0")
    REAL (L"ymax", L"0.0")
    OK
DO
    long pcx = GET_INTEGER (L"X-component"), pcy = GET_INTEGER (L"Y-component");
    REQUIRE (pcx != 0 && pcy != 0, L"X and Y component must differ from 0.")
    praat_picture_open ();
    FFNet_Eigen_drawIntersection (ONLY (classFFNet), ONLY (classEigen),
    	GRAPHICS, pcx, pcy, GET_REAL (L"xmin"), GET_REAL (L"xmax"),
    	GET_REAL (L"ymin"), GET_REAL (L"ymax"));
    praat_picture_close ();
END

FORM (FFNet_PCA_drawDecisionPlaneInEigenspace, L"FFNet & PCA: Draw decision plane", L"")
	NATURAL (L"Unit number", L"1")
	NATURAL (L"Layer number", L"1")
    NATURAL (L"Horizontal eigenvector number", L"1")
    NATURAL (L"Vertical eigenvector number", L"2")
	REAL (L"left Horizontal range", L"0.0")
	REAL (L"right Horizontal range", L"0.0")
	REAL (L"left Vertical range", L"0.0")
	REAL (L"right Vertical range", L"0.0")
	OK
DO
    praat_picture_open ();
	FFNet_Eigen_drawDecisionPlaneInEigenspace(ONLY (classFFNet), ONLY (classPCA),
		GRAPHICS, GET_INTEGER (L"Unit number"), GET_INTEGER (L"Layer number"),
		GET_INTEGER (L"Horizontal eigenvector number"),
		GET_INTEGER (L"Vertical eigenvector number"), GET_REAL (L"left Horizontal range"),
		GET_REAL (L"right Horizontal range"), GET_REAL (L"left Vertical range"),
		GET_REAL (L"right Vertical range"));
    praat_picture_close ();

END

/************************* FFNet && Categories **********************************/

DIRECT (FFNet_Categories_to_Activation)
	NEW (FFNet_Categories_to_Activation (ONLY (classFFNet), ONLY (classCategories)))
END

/************************* FFNet && Matrix **********************************/

FORM (FFNet_weightsFromMatrix, L"Replace weights by values from Matrix", 0)
    NATURAL (L"Layer", L"1")
    OK
DO
    NEW (FFNet_weightsFromMatrix (ONLY (classFFNet), ONLY (classMatrix),
    	GET_INTEGER (L"Layer")));
END

/************************* FFNet && Pattern **********************************/

FORM (FFNet_Pattern_drawActivation, L"Draw an activation", 0)
	NATURAL (L"Pattern (row) number", L"1");
	OK
DO
	EVERY_DRAW (FFNet_Pattern_drawActivation (ONLY (classFFNet), ONLY (classPattern),
		GRAPHICS, GET_INTEGER (L"Pattern")))
END

FORM (FFNet_Pattern_to_Activation, L"To activations in layer", 0)
	NATURAL (L"Layer", L"1")
	OK
DO
	wchar_t name [200];
	praat_name2 (name, classFFNet, classPattern);
	if (! praat_new1 (FFNet_Pattern_to_Activation (ONLY (classFFNet),
		ONLY (classPattern), GET_INTEGER (L"Layer")), name)) return 0;
END

DIRECT (hint_FFNet_and_Pattern_classify)
	Melder_information1 (L"You can use the FFNet as a classifier by selecting a\n"
		"FFNet and a Pattern together and choosing \"To Categories...\".");
END

DIRECT (hint_FFNet_and_Pattern_and_Categories_learn)
	Melder_information1 (L"You can teach a FFNet to classify by selecting a\n"
		"FFNet, a Pattern and a Categories together and choosing \"Learn...\".");
END

FORM (FFNet_Pattern_to_Categories, L"FFNet & Pattern: To Categories", L"FFNet & Pattern: To Categories...")
	RADIO (L"Determine output category as", 1)
	RADIOBUTTON (L"Winner-takes-all")
	RADIOBUTTON (L"Stochastic")
	OK
DO
	wchar_t name [200];
	praat_name2 (name, classFFNet, classPattern);
	if (! praat_new1 (FFNet_Pattern_to_Categories (ONLY (classFFNet),
		ONLY (classPattern), GET_INTEGER (L"Determine output category as")), name)) return 0;
END

/*********** FFNet Pattern Activation **********************************/

FORM (FFNet_Pattern_Activation_getCosts_total, L"FFNet & Pattern & Activation: Get total costs", L"FFNet & Pattern & Activation: Get total costs...")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
	OK
DO
	Melder_information1 (Melder_double (FFNet_Pattern_Activation_getCosts_total (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classActivation), GET_INTEGER (L"Cost function"))));
END

FORM (FFNet_Pattern_Activation_getCosts_average, L"FFNet & Pattern & Activation: Get average costs", L"FFNet & Pattern & Activation: Get average costs...")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
	OK
DO
	Melder_information1 (Melder_double (FFNet_Pattern_Activation_getCosts_average (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classActivation), GET_INTEGER (L"Cost function"))));
END

FORM (FFNet_Pattern_Activation_learnSD, L"FFNet & Pattern & Activation: Learn slow", 0)
    NATURAL (L"Maximum number of epochs", L"100")
    POSITIVE (L"Tolerance of minimizer", L"1e-7")
    LABEL (L"Specifics", L"Specific for this minimization")
    POSITIVE (L"Learning rate", L"0.1")
    REAL (L"Momentum", L"0.9")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
    OK
DO
	struct structSteepestDescentMinimizer_parameters p;
    p.eta = GET_REAL (L"Learning rate");
    p.momentum = GET_REAL (L"Momentum");
    return FFNet_Pattern_Activation_learnSD (ONLY (classFFNet), ONLY (classPattern),
    	ONLY (classActivation), GET_INTEGER (L"Maximum number of epochs"),
		GET_REAL (L"Tolerance of minimizer"), & p, GET_INTEGER (L"Cost function"));
END

FORM (FFNet_Pattern_Activation_learnSM, L"FFNet & Pattern & Activation: Learn", 0)
    NATURAL (L"Maximum number of epochs", L"100")
    POSITIVE (L"Tolerance of minimizer", L"1e-7")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
    OK
DO
    return FFNet_Pattern_Activation_learnSM (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classActivation),
		GET_INTEGER (L"Maximum number of epochs"),
		GET_REAL (L"Tolerance of minimizer"), NULL,
		GET_INTEGER (L"Cost function"));
END

/*********** FFNet Pattern Categories **********************************/

FORM (FFNet_Pattern_Categories_getCosts_total, L"FFNet & Pattern & Categories: Get total costs", L"FFNet & Pattern & Categories: Get total costs...")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
	OK
DO
	Melder_information1 (Melder_double (FFNet_Pattern_Categories_getCosts_total (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classCategories), GET_INTEGER (L"Cost function"))));
END

FORM (FFNet_Pattern_Categories_getCosts_average, L"FFNet & Pattern & Categories: Get average costs", L"FFNet & Pattern & Categories: Get average costs...")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
	OK
DO
	Melder_information1 (Melder_double (FFNet_Pattern_Categories_getCosts_average (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classCategories), GET_INTEGER (L"Cost function"))));
END

FORM (Pattern_Categories_to_FFNet, L"Pattern & Categories: To FFNet", L"Pattern & Categories: To FFNet...")
    INTEGER (L"Number of units in hidden layer 1", L"0")
    INTEGER (L"Number of units in hidden layer 2", L"0")
    OK
DO
	Pattern p = ONLY (classPattern);
	Categories uniq = NULL, l = ONLY (classCategories);
	long numberOfOutputs;
	FFNet ffnet = NULL;
	long nHidden1 = GET_INTEGER (L"Number of units in hidden layer 1");
	long nHidden2 = GET_INTEGER (L"Number of units in hidden layer 2");
	MelderString ffnetName = { 0 };

	if (nHidden1 < 1) nHidden1 = 0;
 	if (nHidden2 < 1) nHidden2 = 0;
	uniq = Categories_selectUniqueItems (l, 1);
	if (uniq == NULL) return Melder_error1 (L"There is not enough memory to create the output categories.\n"
			"Please try again with less categories.");
	numberOfOutputs = uniq -> size;
	if (numberOfOutputs < 1)
	{
		forget (uniq);
		return Melder_error1 (L"There are not enough categories in the Categories.\n"
			"Please try again with more categories in the Categories.");
	}

    ffnet = FFNet_create (p -> nx, nHidden1, nHidden2, numberOfOutputs, 0);
	if (ffnet == NULL)
	{
		forget (uniq);
		return Melder_error1 (L"There was not enough memory to create the FFNet.\n"
			"Please try again with less hidden nodes in the hidden layer(s).");
	}
	if (! FFNet_setOutputCategories (ffnet, uniq))
	{
		forget (uniq); forget (ffnet);
		return 0;
	}
	FFNet_createNameFromTopology (ffnet, &ffnetName);
	int status = praat_new1 (ffnet, ffnetName.string);
	MelderString_free (&ffnetName);
	return status;
END

FORM (FFNet_Pattern_Categories_learnSM, L"FFNet & Pattern & Categories: Learn", L"FFNet & Pattern & Categories: Learn...")
    NATURAL (L"Maximum number of epochs", L"100")
    POSITIVE (L"Tolerance of minimizer", L"1e-7")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
    OK
DO
	if (! FFNet_Pattern_Categories_learnSM (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classCategories),
		GET_INTEGER (L"Maximum number of epochs"),
		GET_REAL (L"Tolerance of minimizer"), NULL,
		GET_INTEGER (L"Cost function"))) return 0;
END

FORM (FFNet_Pattern_Categories_learnSD, L"FFNet & Pattern & Categories: Learn slow", L"FFNet & Pattern & Categories: Learn slow...")
    NATURAL (L"Maximum number of epochs", L"100")
    POSITIVE (L"Tolerance of minimizer", L"1e-7")
    LABEL (L"Specifics", L"Specific for this minimization")
    POSITIVE (L"Learning rate", L"0.1")
    REAL (L"Momentum", L"0.9")
	RADIO (L"Cost function", 1)
	RADIOBUTTON (L"Minimum-squared-error")
	RADIOBUTTON (L"Minimum-cross-entropy")
    OK
DO
	struct structSteepestDescentMinimizer_parameters p;
	p.eta = GET_REAL (L"Learning rate");
    p.momentum = GET_REAL (L"Momentum");
    return FFNet_Pattern_Categories_learnSD (ONLY (classFFNet),
		ONLY (classPattern), ONLY (classCategories),
		GET_INTEGER (L"Maximum number of epochs"),
		GET_REAL (L"Tolerance of minimizer"), &p,
		GET_INTEGER (L"Cost function"));
END

void praat_uvafon_FFNet_init (void);
void praat_uvafon_FFNet_init (void)
{
	Thing_recognizeClassesByName (classFFNet, NULL);

    praat_addMenuCommand (L"Objects", L"New", L"Neural nets", 0, 0, 0);
	praat_addMenuCommand (L"Objects", L"New", L"Feedforward neural networks", 0, 1, DO_FFNet_help);
    praat_addMenuCommand (L"Objects", L"New", L"-- FFNet --", 0, 1, 0);
    praat_addMenuCommand (L"Objects", L"New", L"Create iris example...", 0, 1, DO_FFNet_createIrisExample);
    praat_addMenuCommand (L"Objects", L"New", L"Create FFNet...", 0, 1, DO_FFNet_create);
	praat_addMenuCommand (L"Objects", L"New", L"Advanced", 0, 1, 0);
    praat_addMenuCommand (L"Objects", L"New", L"Create Pattern...", 0, 2, DO_Pattern_create);
    praat_addMenuCommand (L"Objects", L"New", L"Create Categories...", 0, 2, DO_Categories_create);
	praat_addMenuCommand (L"Objects", L"New", L"Create FFNet (linear outputs)...", 0, 2, DO_FFNet_create_linearOutputs);

	praat_addAction1 (classFFNet, 0, L"FFNet help", 0, 0, DO_FFNet_help);
	praat_addAction1 (classFFNet, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, L"Draw topology", 0, 1, DO_FFNet_drawTopology);
	praat_addAction1 (classFFNet, 0, L"Draw weights...", 0, 1, DO_FFNet_drawWeights);
	praat_addAction1 (classFFNet, 0, L"Draw weights to layer...", 0,  praat_DEPTH_1 | praat_HIDDEN, DO_FFNet_drawWeightsToLayer);
	praat_addAction1 (classFFNet, 0, L"Draw cost history...", 0, 1, DO_FFNet_drawCostHistory);
	praat_addAction1 (classFFNet, 0, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (classFFNet, 0, L"Query structure", 0, 1, 0);
		praat_addAction1 (classFFNet, 1, L"Get number of outputs", 0, 2, DO_FFNet_getNumberOfOutputs);
		praat_addAction1 (classFFNet, 1, L"Get number of hidden units...", 0, 2, DO_FFNet_getNumberOfHiddenUnits);
		praat_addAction1 (classFFNet, 1, L"Get number of inputs", 0, 2, DO_FFNet_getNumberOfInputs);
		praat_addAction1 (classFFNet, 1, L"Get number of hidden weights...", 0, 2, DO_FFNet_getNumberOfHiddenWeights);
		praat_addAction1 (classFFNet, 1, L"Get number of output weights", 0, 2, DO_FFNet_getNumberOfOutputWeights);
		praat_addAction1 (classFFNet, 1, L"Get category of output unit...", 0, 2, DO_FFNet_getCategoryOfOutputUnit);
		praat_addAction1 (classFFNet, 1, L"Get output unit of category...", 0, 2, DO_FFNet_getOutputUnitOfCategory);
		praat_addAction1 (classFFNet, 0, L"-- FFNet weights --", 0, 1, 0);
		praat_addAction1 (classFFNet, 1, L"Get minimum", 0, 1, DO_FFNet_getMinimum);
	praat_addAction1 (classFFNet, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 1, L"Reset...", 0, 1, DO_FFNet_reset);
	praat_addAction1 (classFFNet, 0, L"Select biases...", 0, 1, DO_FFNet_selectBiasesInLayer);
	praat_addAction1 (classFFNet, 0, L"Select all weights", 0, 1, DO_FFNet_selectAllWeights);
	praat_addAction1 (classFFNet, 0, EXTRACT_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, L"Extract weights...", 0, 1, DO_FFNet_extractWeights);
	praat_addAction1 (classFFNet, 0, L"Weights to Matrix...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_FFNet_weightsToMatrix);
	praat_addAction1 (classFFNet, 0, L"& Pattern: Classify?", 0, 0, DO_hint_FFNet_and_Pattern_classify);
	praat_addAction1 (classFFNet, 0, L"& Pattern & Categories: Learn?", 0, 0, DO_hint_FFNet_and_Pattern_and_Categories_learn);

	praat_addAction2 (classFFNet, 1, classActivation, 1, L"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classActivation, 1, L"To Categories...", 0, 0, DO_FFNet_Activation_to_Categories);

	praat_addAction2 (classFFNet, 1, classEigen, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classEigen, 1, L"Draw hyperplane intersections", 0, 0, DO_FFNet_Eigen_drawIntersection);

	praat_addAction2 (classFFNet, 1, classCategories, 1, L"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classCategories, 1, L"To Activation", 0, 0, DO_FFNet_Categories_to_Activation);

	praat_addAction2 (classFFNet, 1, classMatrix, 1, L"Modify", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classMatrix, 1, L"Weights from Matrix...", 0, 0, DO_FFNet_weightsFromMatrix);

	praat_addAction2 (classFFNet, 1, classPattern, 1, L"Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, L"Draw activation...", 0, 0, DO_FFNet_Pattern_drawActivation);
	praat_addAction2 (classFFNet, 1, classPattern, 1, L"Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, L"To Categories...", 0, 0, DO_FFNet_Pattern_to_Categories);
	praat_addAction2 (classFFNet, 1, classPattern, 1, L"To Activation...", 0, 0, DO_FFNet_Pattern_to_Activation);

	praat_addAction2 (classFFNet, 1, classPCA, 1, L"Draw decision plane...", 0, 0, DO_FFNet_PCA_drawDecisionPlaneInEigenspace);

	praat_addAction2 (classPattern, 1, classCategories, 1, L"To FFNet...", 0, 0, DO_Pattern_Categories_to_FFNet);

	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, L"Get total costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_total);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, L"Get average costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_average);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, L"Learn", 0, 0, 0);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, L"Learn...", 0, 0, DO_FFNet_Pattern_Activation_learnSM);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classActivation, 1, L"Learn slow...", 0, 0, DO_FFNet_Pattern_Activation_learnSD);

	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, L"Get total costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_total);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, L"Get average costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_average);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, L"Learn", 0, 0, 0);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, L"Learn...", 0, 0, DO_FFNet_Pattern_Categories_learnSM);
	praat_addAction3 (classFFNet, 1, classPattern, 1, classCategories, 1, L"Learn slow...", 0, 0, DO_FFNet_Pattern_Categories_learnSD);

    INCLUDE_MANPAGES (manual_FFNet_init)

}
