/* praat_FFNet_init.c
 *
 * Copyright (C) 1994-2004 David Weenink
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


static char *QUERY_BUTTON   = "Query -                ";
static char *DRAW_BUTTON     = "Draw -                 ";
static char *MODIFY_BUTTON  = "Modify -               ";
static char *EXTRACT_BUTTON = "Extract - ";

/**************** New FFNet ***************************/

static void FFNet_create_addCommonFields_inputOutput (void *dia)
{
    NATURAL ("Number of inputs", "4")
    NATURAL ("Number of outputs", "3")
}

static int FFNet_create_checkCommonFields_inputOutput (void *dia, long *numberOfInputs, long *numberOfOutputs)
{
	*numberOfInputs = GET_INTEGER ("Number of inputs");
	*numberOfOutputs = GET_INTEGER ("Number of outputs");
	return 1;
}

static void FFNet_create_addCommonFields_hidden (void *dia)
{
    INTEGER ("Number of units in hidden layer 1", "0")
    INTEGER ("Number of units in hidden layer 2", "0")
}

static int FFNet_create_checkCommonFields_hidden (void *dia, 	long *numberOfHidden1, long *numberOfHidden2)
{
	*numberOfHidden1 = GET_INTEGER ("Number of units in hidden layer 1");
	*numberOfHidden2 = GET_INTEGER ("Number of units in hidden layer 2");
	if (*numberOfHidden1 < 0 || *numberOfHidden2 < 0)
	{
		return Melder_error ("The number of units in a hidden layer must be greater than or equal to 0.");
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

FORM (FFNet_create, "Create FFNet", "Create FFNet...")
	WORD ("Name", "4-3")
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
		(void) Melder_error ("There was not enough memory to create the FFNet.\n");
		return Melder_error ("Please reduce some of the numbers.");
	}
    if (! praat_new (thee, GET_STRING ("Name"))) return 0;
END

FORM (FFNet_create_linearOutputs, "Create FFNet", "Create FFNet (linear outputs)...")
	WORD ("Name", "4-3L")
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
		(void) Melder_error ("There was not enough memory to create the FFNet.\n");
		return Melder_error ("Please reduce some of the numbers.");
	}
    if (! praat_new (thee, GET_STRING ("Name"))) return 0;
END

FORM (FFNet_createIrisExample, "Create iris example", "Create iris example...")
	LABEL ("", "For the feedforward neural net we need to know the:")
	FFNet_create_addCommonFields_hidden (dia);
	OK
DO
	long numberOfHidden1, numberOfHidden2;
	if (! FFNet_create_checkCommonFields_hidden (dia, &numberOfHidden1, &numberOfHidden2)) return 0;
	NEW (FFNet_createIrisExample (numberOfHidden1, numberOfHidden2))
END

DIRECT (FFNet_getNumberOfInputs)
	FFNet  me = ONLY(classFFNet);
	Melder_information ("%d units", my nUnitsInLayer[0]);
END

DIRECT (FFNet_getNumberOfOutputs)
	FFNet  me = ONLY(classFFNet);
	Melder_information ("%d units", my nUnitsInLayer[my nLayers]);
END

FORM (FFNet_getNumberOfHiddenUnits, "FFNet: Get number of hidden units", "FFNet: Get number of hidden units...")
	NATURAL ("Hidden layer number", "1")
	OK
DO
	FFNet  me = ONLY(classFFNet);
	long layerNumber = GET_INTEGER ("Hidden layer number");
	long numberOfUnits = 0;
	
	if (layerNumber > 0 && layerNumber <= my nLayers - 1) numberOfUnits = my nUnitsInLayer[layerNumber];
	Melder_information ("%d units", numberOfUnits);
END

FORM (FFNet_getNumberOfHiddenWeights, "FFNet: Get number of hidden weights", 
	"FFNet: Get number of hidden weights...")
	NATURAL ("Hidden layer number", "1")
	OK
DO
	FFNet  me = ONLY(classFFNet);
	long layerNumber = GET_INTEGER ("Hidden layer number");
	long numberOfWeights = 0;	
	if (layerNumber > 0 && layerNumber <= my nLayers - 1)
	{
		numberOfWeights = my nUnitsInLayer[layerNumber] * (my nUnitsInLayer[layerNumber - 1]+1);
	}
	Melder_information ("%d weights (inclusive biases)", numberOfWeights);
END

DIRECT (FFNet_getNumberOfOutputWeights)
	FFNet  me = ONLY(classFFNet);
	Melder_information ("%d weights", my nUnitsInLayer[my nLayers] * (my nUnitsInLayer[my nLayers - 1]+1));
END

/**************** New Pattern ***************************/

FORM (Pattern_create, "Create Pattern", 0)
	WORD ("Name", "1x1")
    NATURAL ("Dimension of a pattern", "1")
    NATURAL ("Number of patterns", "1")
    OK
DO
	if (! praat_new (Pattern_create (GET_INTEGER ("Number of patterns"), 
		GET_INTEGER ("Dimension of a pattern")), GET_STRING ("Name"))) return 0;
END

/**************** New Categories ***************************/

FORM (Categories_create, "Create Categories", "")
	WORD ("Name", "empty")
	OK
DO
	if (! praat_new (Categories_create (), GET_STRING ("Name"))) return 0;
END

DIRECT (FFNet_help)
	Melder_help ("Feedforward neural networks");
END

DIRECT (FFNet_getMinimum)
	Melder_information ("%s", Melder_double (FFNet_getMinimum (ONLY_OBJECT)));
END

FORM (FFNet_reset, "FFNet: Reset", "FFNet: Reset...")
	LABEL ("", "Warning: this command destroys all previous learning.")
	LABEL ("", "New weights will be randomly chosen from the interval [-range, +range].")
    POSITIVE ("Range", "0.1")
    OK
DO
    WHERE (SELECTED)
    {
    	FFNet_reset (OBJECT, GET_REAL ("Range"));
		praat_dataChanged (OBJECT);
	}
END

FORM (FFNet_selectBiasesInLayer, "FFNet: Select biases", "FFNet: Select biases...")
	LABEL ("", "WARNING: This command induces very specific behaviour ")
	LABEL ("", "during a following learning phase.")
	NATURAL ("Layer number", "1")
	OK
DO
	WHERE (SELECTED)
	{
		FFNet_selectBiasesInLayer (OBJECT, GET_INTEGER ("Layer number"));
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

FORM (FFNet_drawWeightsToLayer, "FFNet: Draw weights to layer", 0)
	LABEL ("", "Warning: Disapproved. Use \"Draw weights..\" instead.")
    NATURAL ("Layer number", "1")
	RADIO ("Scale", 1)
	RADIOBUTTON ("by maximum of all weights to layer")
	RADIOBUTTON ("by maximum weight from 'from-unit'")
	RADIOBUTTON ("by maximum weight to 'to-unit'")
    BOOLEAN ("Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawWeightsToLayer (OBJECT, GRAPHICS,
		GET_INTEGER ("Layer number"),
    	GET_INTEGER ("Scale"), GET_INTEGER ("Garnish")))
END

FORM (FFNet_drawWeights, "FFNet: Draw weights", "FFNet: Draw weights...")
    NATURAL ("Layer number", "1")
    BOOLEAN ("Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawWeights (OBJECT, GRAPHICS, 
		GET_INTEGER ("Layer number"), GET_INTEGER ("Garnish")))
END

FORM (FFNet_drawCostHistory, "FFNet: Draw cost history", "FFNet: Draw cost history...")
    INTEGER ("left Iteration_range", "0")
    INTEGER ("right Iteration_range", "0")
    REAL ("left Cost_range", "0.0")
    REAL ("right Cost_range", "0.0")
    BOOLEAN ("Garnish", 1)
    OK
DO
    EVERY_DRAW (FFNet_drawCostHistory (OBJECT, GRAPHICS,
	GET_INTEGER ("left Iteration_range"), GET_INTEGER ("right Iteration_range"),
	GET_REAL ("left Cost_range"), GET_REAL ("right Cost_range"), GET_INTEGER ("Garnish")))
END

FORM (FFNet_extractWeights, "FFNet: Extract weights", "FFNet: Extract weights...")
    NATURAL ("Layer number", "1")
    OK
DO
    EVERY_TO (FFNet_extractWeights (OBJECT, GET_INTEGER ("Layer number")))
END

FORM (FFNet_weightsToMatrix, "FFNet: Weights to Matrix ", 0)
	LABEL ("", "Warning: Use \"Extract weights..\" instead.")
    NATURAL ("Layer number", "1")
    OK
DO
    EVERY_TO (FFNet_weightsToMatrix (OBJECT, GET_INTEGER ("Layer number"), 0))
END

/******************* FFNet && Activation *************************************/

FORM (FFNet_Activation_to_Categories, "FFNet & Activation: To Categories", 0)
	RADIO ("Kind of labeling", 1)
	RADIOBUTTON ("Winner-takes-all")
	RADIOBUTTON ("Stochastic")
	OK
DO
	char name [200];
	praat_name2 (name, classFFNet, classActivation);
	if (! praat_new (FFNet_Activation_to_Categories (ONLY (classFFNet), ONLY (classActivation),
		GET_INTEGER ("Kind of labeling")), name)) return 0;
END

/******************* FFNet && Eigen ******************************************/

FORM (FFNet_Eigen_drawIntersection, "FFnet & Eigen: Draw intersection", 0)
    INTEGER ("X-component", "1")
    INTEGER ("Y-component", "2")
    REAL ("xmin", "0.0")
    REAL ("xmax", "0.0")
    REAL ("ymin", "0.0")
    REAL ("ymax", "0.0")
    OK
DO
    long pcx = GET_INTEGER ("X-component"), pcy = GET_INTEGER ("Y-component");
    REQUIRE (pcx != 0 && pcy != 0, "X and Y component must differ from 0.")
    praat_picture_open ();
    FFNet_Eigen_drawIntersection (ONLY (classFFNet), ONLY (classEigen),
    	GRAPHICS, pcx, pcy, GET_REAL ("xmin"), GET_REAL ("xmax"),
    	GET_REAL ("ymin"), GET_REAL ("ymax"));
    praat_picture_close ();
END

FORM (FFNet_PCA_drawDecisionPlaneInEigenspace, "FFNet & PCA: Draw decision plane", "")
	NATURAL ("Unit number", "1")
	NATURAL ("Layer number", "1")
    NATURAL ("Horizontal eigenvector number", "1")
    NATURAL ("Vertical eigenvector number", "2")
	REAL ("left Horizontal range", "0.0")
	REAL ("right Horizontal range", "0.0")
	REAL ("left Vertical range", "0.0")
	REAL ("right Vertical range", "0.0")
	OK
DO
    praat_picture_open ();
	FFNet_Eigen_drawDecisionPlaneInEigenspace(ONLY (classFFNet), ONLY (classPCA),
		GRAPHICS, GET_INTEGER ("Unit number"), GET_INTEGER ("Layer number"),
		GET_INTEGER ("Horizontal eigenvector number"), 
		GET_INTEGER ("Vertical eigenvector number"), GET_REAL ("left Horizontal range"),
		GET_REAL ("right Horizontal range"), GET_REAL ("left Vertical range"),
		GET_REAL ("right Vertical range"));
    praat_picture_close ();
	
END

/************************* FFNet && Categories **********************************/

DIRECT (FFNet_Categories_to_Activation)
	NEW (FFNet_Categories_to_Activation (ONLY (classFFNet), ONLY (classCategories)))
END

/************************* FFNet && Matrix **********************************/

FORM (FFNet_weightsFromMatrix, "Replace weights by values from Matrix", 0)
    NATURAL ("Layer", "1")
    OK
DO
    NEW (FFNet_weightsFromMatrix (ONLY (classFFNet), ONLY (classMatrix),
    	GET_INTEGER ("Layer")));
END

/************************* FFNet && Pattern **********************************/

FORM (FFNet_Pattern_drawActivation, "Draw an activation", 0)
	NATURAL ("Pattern (row) number", "1");
	OK
DO
	EVERY_DRAW (FFNet_Pattern_drawActivation (ONLY (classFFNet), ONLY (classPattern),
		GRAPHICS, GET_INTEGER ("Pattern")))
END

FORM (FFNet_Pattern_to_Activation, "To activations in layer", 0)
	NATURAL ("Layer", "1")
	OK
DO
	char name [200];
	praat_name2 (name, classFFNet, classPattern);
	NEW (FFNet_Pattern_to_Activation (ONLY (classFFNet),
		ONLY (classPattern), GET_INTEGER ("Layer")))
END

DIRECT (hint_FFNet_and_Pattern_classify)
	Melder_information ("You can use the FFNet as a classifier by selecting a\n"
		"FFNet and a Pattern together and choosing \"To Categories...\".");
END

DIRECT (hint_FFNet_and_Pattern_and_Categories_learn)
	Melder_information ("You can teach a FFNet to classify by selecting a\n"
		"FFNet, a Pattern and a Categories together and choosing \"Learn...\".");
END

FORM (FFNet_Pattern_to_Categories, "FFNet & Pattern: To Categories", 
	"FFNet & Pattern: To Categories...")
	RADIO ("Determine output category as", 1)
	RADIOBUTTON ("Winner-takes-all")
	RADIOBUTTON ("Stochastic")
	OK
DO
	char name [200];
	praat_name2 (name, classFFNet, classPattern);
	if (! praat_new (FFNet_Pattern_to_Categories (ONLY (classFFNet),
		ONLY (classPattern), GET_INTEGER ("Determine output category as")), name)) return 0;
END

/*********** FFNet Pattern Activation **********************************/

FORM (FFNet_Pattern_Activation_getCosts_total, "FFNet & Pattern & Activation: Get total costs", "FFNet & Pattern & Activation: Get total costs...")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
	OK
DO
	Melder_informationReal (FFNet_Pattern_Activation_getCosts_total (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classActivation), GET_INTEGER ("Cost function")), NULL);
END

FORM (FFNet_Pattern_Activation_getCosts_average, "FFNet & Pattern & Activation: Get average costs", "FFNet & Pattern & Activation: Get average costs...")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
	OK
DO
	Melder_informationReal (FFNet_Pattern_Activation_getCosts_average (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classActivation), GET_INTEGER ("Cost function")), NULL);
END

FORM (FFNet_Pattern_Activation_learnSD, "FFNet & Pattern & Activation: Learn slow", 0)
    NATURAL ("Maximum number of epochs", "100")
    POSITIVE ("Tolerance of minimizer", "1e-7")
    LABEL ("Specifics", "Specific for this minimization")
    POSITIVE ("Learning rate", "0.1")
    REAL ("Momentum", "0.9")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
    OK
DO
	struct structSteepestDescentMinimizer_parameters p;
    p.eta = GET_REAL ("Learning rate");
    p.momentum = GET_REAL ("Momentum");
    return FFNet_Pattern_Activation_learnSD (ONLY (classFFNet), ONLY (classPattern),
    	ONLY (classActivation), GET_INTEGER ("Maximum number of epochs"),
		GET_REAL ("Tolerance of minimizer"), & p, GET_INTEGER ("Cost function")); 
END

FORM (FFNet_Pattern_Activation_learnSM, "FFNet & Pattern & Activation: Learn", 0)
    NATURAL ("Maximum number of epochs", "100")
    POSITIVE ("Tolerance of minimizer", "1e-7")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
    OK
DO
    return FFNet_Pattern_Activation_learnSM (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classActivation), 
		GET_INTEGER ("Maximum number of epochs"),
		GET_REAL ("Tolerance of minimizer"), NULL, 
		GET_INTEGER ("Cost function")); 
END

/*********** FFNet Pattern Categories **********************************/

FORM (FFNet_Pattern_Categories_getCosts_total, "FFNet & Pattern & Categories: Get total costs", "FFNet & Pattern & Categories: Get total costs...")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
	OK
DO
	Melder_informationReal (FFNet_Pattern_Categories_getCosts_total (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classCategories), GET_INTEGER ("Cost function")), NULL);
END

FORM (FFNet_Pattern_Categories_getCosts_average, "FFNet & Pattern & Categories: Get average costs", "FFNet & Pattern & Categories: Get average costs...")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
	OK
DO
	Melder_informationReal (FFNet_Pattern_Categories_getCosts_average (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classCategories), GET_INTEGER ("Cost function")), NULL);
END

FORM (Pattern_Categories_to_FFNet, "Pattern & Categories: To FFNet", 
	"Pattern & Categories: To FFNet...")
    INTEGER ("Number of units in hidden layer 1", "0")
    INTEGER ("Number of units in hidden layer 2", "0")
    OK
DO
	Pattern p = ONLY (classPattern); 
	Categories uniq = NULL, l = ONLY (classCategories);
	long numberOfOutputs; 
	FFNet ffnet = NULL;
	long nHidden1 = GET_INTEGER ("Number of units in hidden layer 1");
	long nHidden2 = GET_INTEGER ("Number of units in hidden layer 2");
	char ffnetName[40];
	
	if (nHidden1 < 1) nHidden1 = 0;
 	if (nHidden2 < 1) nHidden2 = 0;
	uniq = Categories_selectUniqueItems (l, 1);
	if (uniq == NULL)
	{
		(void) Melder_error ("There is not enough memory to create %d output categories.\n");
		return Melder_error ("Please try again with less categories.");
	}
	numberOfOutputs = uniq -> size;
	if (numberOfOutputs < 1)
	{
		forget (uniq);
		(void) Melder_error ("There are not enough categories in the Categories.\n");
		return Melder_error ("Please try again with more categories in the Categories.");
	}
	
    ffnet = FFNet_create (p -> nx, nHidden1, nHidden2, numberOfOutputs, 0);
	if (ffnet == NULL)
	{
		forget (uniq);
		(void) Melder_error ("There was not enough memory to create the FFNet.\n");
		return Melder_error ("Please try again with less hidden nodes in the hidden layer(s).");
	}
	if (! FFNet_setOutputCategories (ffnet, uniq))
	{
		forget (uniq); forget (ffnet);
		return 0;
	}
	FFNet_createNameFromTopology (ffnet, ffnetName);
	if (! praat_new (ffnet, ffnetName)) return 0;
END

FORM (FFNet_Pattern_Categories_learnSM, "FFNet & Pattern & Categories: Learn", "FFNet & Pattern & Categories: Learn...")
    NATURAL ("Maximum number of epochs", "100")
    POSITIVE ("Tolerance of minimizer", "1e-7")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
    OK
DO
	if (! FFNet_Pattern_Categories_learnSM (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classCategories), 
		GET_INTEGER ("Maximum number of epochs"),
		GET_REAL ("Tolerance of minimizer"), NULL, 
		GET_INTEGER ("Cost function"))) return 0;
END

FORM (FFNet_Pattern_Categories_learnSD, "FFNet & Pattern & Categories: Learn slow",
	"FFNet & Pattern & Categories: Learn slow...")
    NATURAL ("Maximum number of epochs", "100")
    POSITIVE ("Tolerance of minimizer", "1e-7")
    LABEL ("Specifics", "Specific for this minimization")
    POSITIVE ("Learning rate", "0.1")
    REAL ("Momentum", "0.9")
	RADIO ("Cost function", 1)
	RADIOBUTTON ("Minimum-squared-error")
	RADIOBUTTON ("Minimum-cross-entropy")
    OK
DO
	struct structSteepestDescentMinimizer_parameters p;
	p.eta = GET_REAL ("Learning rate");
    p.momentum = GET_REAL ("Momentum");
    return FFNet_Pattern_Categories_learnSD (ONLY (classFFNet), 
		ONLY (classPattern), ONLY (classCategories), 
		GET_INTEGER ("Maximum number of epochs"),
		GET_REAL ("Tolerance of minimizer"), &p, 
		GET_INTEGER ("Cost function")); 
END

void praat_uvafon_FFNet_init (void);
void praat_uvafon_FFNet_init (void)
{
	Thing_recognizeClassesByName (classFFNet, NULL);
	
    praat_addMenuCommand ("Objects", "New", "Neural nets", 0, 0, 0);
	praat_addMenuCommand ("Objects", "New", "Feedforward neural networks", 0, 1,
		DO_FFNet_help);
    praat_addMenuCommand ("Objects", "New", "-- FFNet --", 0, 1, 0);		
    praat_addMenuCommand ("Objects", "New", "Create iris example...", 
		0, 1, DO_FFNet_createIrisExample);
    praat_addMenuCommand ("Objects", "New", "Create FFNet...", 
		0, 1, DO_FFNet_create);
	praat_addMenuCommand ("Objects", "New", "Advanced", 0, 1, 0);
    praat_addMenuCommand ("Objects", "New", "Create Pattern...",	
		0, 2, DO_Pattern_create);
    praat_addMenuCommand ("Objects", "New", "Create Categories...", 0, 2,
		DO_Categories_create);
	praat_addMenuCommand ("Objects", "New", "Create FFNet (linear outputs)...", 
		0, 2, DO_FFNet_create_linearOutputs);
        
	praat_addAction1 (classFFNet, 0, "FFNet help", 0, 0, DO_FFNet_help);
	praat_addAction1 (classFFNet, 0, DRAW_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, "Draw topology", 0, 1, DO_FFNet_drawTopology);
	praat_addAction1 (classFFNet, 0, "Draw weights...", 0, 1, DO_FFNet_drawWeights);
	praat_addAction1 (classFFNet, 0, "Draw weights to layer...", 0,  praat_DEPTH_1 | praat_HIDDEN,
		DO_FFNet_drawWeightsToLayer);
	praat_addAction1 (classFFNet, 0, "Draw cost history...", 0, 1, DO_FFNet_drawCostHistory);
	praat_addAction1 (classFFNet, 0, QUERY_BUTTON, 0, 0, 0);
		praat_addAction1 (classFFNet, 0, "Query structure", 0, 1, 0);
		praat_addAction1 (classFFNet, 1, "Get number of outputs", 0, 2, DO_FFNet_getNumberOfOutputs);
		praat_addAction1 (classFFNet, 1, "Get number of hidden units...", 0, 2, DO_FFNet_getNumberOfHiddenUnits);
		praat_addAction1 (classFFNet, 1, "Get number of inputs", 0, 2, DO_FFNet_getNumberOfInputs);
		praat_addAction1 (classFFNet, 0, "-- FFNet weights --", 0, 2, 0);
		praat_addAction1 (classFFNet, 1, "Get number of hidden weights...", 0, 2, DO_FFNet_getNumberOfHiddenWeights);
		praat_addAction1 (classFFNet, 1, "Get number of output weights", 0, 2, DO_FFNet_getNumberOfOutputWeights);
		praat_addAction1 (classFFNet, 1, "Get minimum", 0, 1, DO_FFNet_getMinimum);
	praat_addAction1 (classFFNet, 0, MODIFY_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 1, "Reset...", 0, 1, DO_FFNet_reset);
	praat_addAction1 (classFFNet, 0, "Select biases...", 0, 1, DO_FFNet_selectBiasesInLayer);
	praat_addAction1 (classFFNet, 0, "Select all weights", 0, 1, DO_FFNet_selectAllWeights);
	praat_addAction1 (classFFNet, 0, EXTRACT_BUTTON, 0, 0, 0);
	praat_addAction1 (classFFNet, 0, "Extract weights...", 0, 1, DO_FFNet_extractWeights);
	praat_addAction1 (classFFNet, 0, "Weights to Matrix...", 0, praat_DEPTH_1 | praat_HIDDEN, DO_FFNet_weightsToMatrix);
	praat_addAction1 (classFFNet, 0, "& Pattern: Classify?", 0, 0, DO_hint_FFNet_and_Pattern_classify);
	praat_addAction1 (classFFNet, 0, "& Pattern & Categories: Learn?", 0, 0, DO_hint_FFNet_and_Pattern_and_Categories_learn);
	
	praat_addAction2 (classFFNet, 1, classActivation, 1, "Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classActivation, 1, "To Categories...", 
		0, 0, DO_FFNet_Activation_to_Categories);
	
	praat_addAction2 (classFFNet, 1, classEigen, 1, "Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classEigen, 1, 
		"Draw hyperplane intersections", 0, 0, DO_FFNet_Eigen_drawIntersection);
	
	praat_addAction2 (classFFNet, 1, classCategories, 1, "Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classCategories, 1, "To Activation", 0, 0, 
		DO_FFNet_Categories_to_Activation);
	
	praat_addAction2 (classFFNet, 1, classMatrix, 1, "Modify", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classMatrix, 1, "Weights from Matrix...", 
		0, 0, DO_FFNet_weightsFromMatrix);
	
	praat_addAction2 (classFFNet, 1, classPattern, 1, "Draw", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, "Draw activation...", 
		0, 0, DO_FFNet_Pattern_drawActivation);
	praat_addAction2 (classFFNet, 1, classPattern, 1, "Analyse", 0, 0, 0);
	praat_addAction2 (classFFNet, 1, classPattern, 1, 
		"To Categories...", 0, 0, DO_FFNet_Pattern_to_Categories);
	praat_addAction2 (classFFNet, 1, classPattern, 1, "To Activation...", 0, 0, 
		DO_FFNet_Pattern_to_Activation);
		
	praat_addAction2 (classFFNet, 1, classPCA, 1, 
		"Draw decision plane...", 0, 0, DO_FFNet_PCA_drawDecisionPlaneInEigenspace);
	
	praat_addAction2 (classPattern, 1, classCategories, 1, 
		"To FFNet...", 0, 0, DO_Pattern_Categories_to_FFNet);
	
	praat_addAction (classFFNet, 1, classPattern, 1, classActivation, 1, 
		"Get total costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_total);
	praat_addAction (classFFNet, 1, classPattern, 1, classActivation, 1, 
		"Get average costs...", 0, 0, DO_FFNet_Pattern_Activation_getCosts_average);
	praat_addAction (classFFNet, 1, classPattern, 1, classActivation, 1, 
		"Learn", 0, 0, 0);
	praat_addAction (classFFNet, 1, classPattern, 1, classActivation, 1, 
		"Learn...", 0, 0, DO_FFNet_Pattern_Activation_learnSM);
	praat_addAction (classFFNet, 1, classPattern, 1, classActivation, 1, 
		"Learn slow...", 0, 0, DO_FFNet_Pattern_Activation_learnSD);
	
	praat_addAction (classFFNet, 1, classPattern, 1, classCategories, 1, 
		"Get total costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_total);
	praat_addAction (classFFNet, 1, classPattern, 1, classCategories, 1, 
		"Get average costs...", 0, 0, DO_FFNet_Pattern_Categories_getCosts_average);
	praat_addAction (classFFNet, 1, classPattern, 1, classCategories, 1, 
		"Learn", 0, 0, 0);
	praat_addAction (classFFNet, 1, classPattern, 1, classCategories, 1, 
		"Learn...", 0, 0, DO_FFNet_Pattern_Categories_learnSM);
	praat_addAction (classFFNet, 1, classPattern, 1, classCategories, 1, 
		"Learn slow...", 0, 0, DO_FFNet_Pattern_Categories_learnSD);
    
    INCLUDE_MANPAGES (manual_FFNet_init)
	
}
