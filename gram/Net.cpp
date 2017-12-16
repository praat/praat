/* Net.cpp
 *
 * Copyright (C) 2017 Paul Boersma
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

//#include <OpenCL/OpenCL.h>
#include "Net.h"
#include "tensor.h"
#include "PAIRWISE_SUM.h"

#include "oo_DESTROY.h"
#include "Net_def.h"
#include "oo_COPY.h"
#include "Net_def.h"
#include "oo_EQUAL.h"
#include "Net_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Net_def.h"
#include "oo_WRITE_TEXT.h"
#include "Net_def.h"
#include "oo_READ_TEXT.h"
#include "Net_def.h"
#include "oo_WRITE_BINARY.h"
#include "Net_def.h"
#include "oo_READ_BINARY.h"
#include "Net_def.h"
#include "oo_DESCRIPTION.h"
#include "Net_def.h"

#include "enums_getText.h"
#include "Net_enums.h"
#include "enums_getValue.h"
#include "Net_enums.h"

Thing_implement (Layer, Daata, 0);

Thing_implement (Net, Daata, 0);

static autoLayer Layer_createAsRBM (integer numberOfInputNodes, integer numberOfOutputNodes, bool inputsAreBinary) {
	try {
		autoLayer me = Thing_new (Layer);
		my numberOfInputNodes = numberOfInputNodes;
		my inputBiases = NUMvector <double> (1, numberOfInputNodes);
		my inputActivities = NUMvector <double> (1, numberOfInputNodes);
		my inputReconstruction = NUMvector <double> (1, numberOfInputNodes);
		my numberOfOutputNodes = numberOfOutputNodes;
		my outputBiases = NUMvector <double> (1, numberOfOutputNodes);
		my outputActivities = NUMvector <double> (1, numberOfOutputNodes);
		my outputReconstruction = NUMvector <double> (1, numberOfOutputNodes);
		my weights = NUMmatrix <double> (1, numberOfInputNodes, 1, numberOfOutputNodes);
		my inputsAreBinary = inputsAreBinary;
		return me;
	} catch (MelderError) {
		Melder_throw (U"RBM layer with ", numberOfInputNodes, U" input nodes and ",
			numberOfOutputNodes, U" output nodes not created.");
	}
}

void Net_initAsDeepBeliefNet (Net me, numvec numbersOfNodes, bool inputsAreBinary) {
	if (numbersOfNodes.size < 2)
		Melder_throw (U"A deep belief net should have at least two levels of nodes.");
	integer numberOfLayers = numbersOfNodes.size - 1;
	for (integer ilayer = 1; ilayer <= numberOfLayers; ilayer ++) {
		autoLayer layer = Layer_createAsRBM (
			Melder_iround (numbersOfNodes [ilayer]),
			Melder_iround (numbersOfNodes [ilayer + 1]),
			ilayer == 1 ? inputsAreBinary : true
		);
		my layers. addItem_move (layer.move());
	}
}

autoNet Net_createAsDeepBeliefNet (numvec numbersOfNodes, bool inputsAreBinary) {
	try {
		autoNet me = Thing_new (Net);
		Net_initAsDeepBeliefNet (me.get(), numbersOfNodes, inputsAreBinary);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Net not created.");
	}
}

static void copyOutputsToInputs (Layer me, Layer you) {
	Melder_assert (my numberOfOutputNodes == your numberOfInputNodes);
	for (integer inode = 1; inode <= my numberOfOutputNodes; inode ++) {
		your inputActivities [inode] = my outputActivities [inode];
	}
}

inline static double logistic (double excitation) {
	return 1.0 / (1.0 + exp (- excitation));
}

static void Layer_spreadUp (Layer me) {
	integer numberOfOutputNodes = my numberOfOutputNodes;
	for (integer jnode = 1; jnode <= numberOfOutputNodes; jnode ++) {
		PAIRWISE_SUM (real80, excitation, integer, my numberOfInputNodes,
 			double *p_inputActivity = & my inputActivities [0];
 			double *p_weight = & my weights [1] [jnode] - numberOfOutputNodes,
 			( p_inputActivity += 1, p_weight += numberOfOutputNodes ),
 			(real80) *p_inputActivity * (real80) *p_weight
		);
		excitation += my outputBiases [jnode];
		my outputActivities [jnode] = logistic ((real) excitation);
	}
}

static void Layer_sampleOutput (Layer me) {
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		double probability = my outputActivities [jnode];
		my outputActivities [jnode] = (double) NUMrandomBernoulli (probability);
	}
}

void Net_spreadUp (Net me, kNet_activationType activationType) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		Layer layer = my layers.at [ilayer];
		if (ilayer > 1)
			copyOutputsToInputs (my layers.at [ilayer - 1], layer);
		Layer_spreadUp (layer);
		if (activationType == kNet_activationType::STOCHASTIC)
			Layer_sampleOutput (layer);
	}
}

static void Layer_sampleInput (Layer me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		if (my inputsAreBinary) {
			double probability = my inputActivities [inode];
			my inputActivities [inode] = (double) NUMrandomBernoulli (probability);
		} else {   // Gaussian
			double excitation = my inputActivities [inode];
			my inputActivities [inode] = NUMrandomGauss (excitation, 1.0);
		}
	}
}

void Net_sampleInput (Net me) {
	Layer_sampleInput (my layers.at [1]);
}

void Net_sampleOutput (Net me) {
	Layer_sampleOutput (my layers.at [my layers.size]);
}

static void copyInputsToOutputs (Layer me, Layer you) {
	Melder_assert (my numberOfInputNodes == your numberOfOutputNodes);
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		your outputActivities [inode] = my inputActivities [inode];
	}
}

static void Layer_spreadDown (Layer me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		PAIRWISE_SUM (real80, excitation, integer, my numberOfOutputNodes,
 			double *p_weight = & my weights [inode] [0];
 			double *p_outputActivity = & my outputActivities [0],
 			( p_weight += 1, p_outputActivity += 1 ),
 			(real80) *p_weight * (real80) *p_outputActivity
		);
		excitation += my inputBiases [inode];
		if (my inputsAreBinary) {
			my inputActivities [inode] = logistic ((real) excitation);
		} else {   // linear
			my inputActivities [inode] = (real) excitation;
		}
	}
}

void Net_spreadDown (Net me, kNet_activationType activationType) {
	for (integer ilayer = my layers.size; ilayer > 0; ilayer --) {
		Layer layer = my layers.at [ilayer];
		if (ilayer < my layers.size)
			copyInputsToOutputs (my layers.at [ilayer + 1], layer);
		Layer_spreadDown (layer);
		if (activationType == kNet_activationType::STOCHASTIC)
			Layer_sampleInput (layer);
	}
}

static void Layer_spreadDown_reconstruction (Layer me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		PAIRWISE_SUM (real80, excitation, integer, my numberOfOutputNodes,
 			double *p_weight = & my weights [inode] [0];
 			double *p_outputActivity = & my outputActivities [0],
 			( p_weight += 1, p_outputActivity += 1 ),
 			(real80) *p_weight * (real80) *p_outputActivity
		);
		excitation += my inputBiases [inode];
		if (my inputsAreBinary) {
			my inputReconstruction [inode] = logistic ((real) excitation);
		} else {   // linear
			my inputReconstruction [inode] = (real) excitation;
		}
	}
}

void Net_spreadDown_reconstruction (Net me) {
	for (integer ilayer = my layers.size; ilayer > 0; ilayer --) {
		Layer_spreadDown_reconstruction (my layers.at [ilayer]);
	}
}

static void Layer_spreadUp_reconstruction (Layer me) {
	integer numberOfOutputNodes = my numberOfOutputNodes;
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		PAIRWISE_SUM (real80, excitation, integer, my numberOfInputNodes,
 			double *p_inputActivity = & my inputReconstruction [0];
 			double *p_weight = & my weights [1] [jnode] - numberOfOutputNodes,
 			( p_inputActivity += 1, p_weight += numberOfOutputNodes ),
 			(real80) *p_inputActivity * (real80) *p_weight
		);
		excitation += my outputBiases [jnode];
		my outputReconstruction [jnode] = logistic ((real) excitation);
	}
}

void Net_spreadUp_reconstruction (Net me) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		Layer_spreadUp_reconstruction (my layers.at [ilayer]);
	}
}

static void Layer_update (Layer me, double learningRate) {
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		my outputBiases [jnode] += learningRate * (my outputActivities [jnode] - my outputReconstruction [jnode]);
	}
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		my inputBiases [inode] += learningRate * (my inputActivities [inode] - my inputReconstruction [inode]);
		for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			my weights [inode] [jnode] += learningRate *
				(my inputActivities [inode] * my outputActivities [jnode] -
				 my inputReconstruction [inode] * my outputReconstruction [jnode]);
		}
	}
}

void Net_update (Net me, double learningRate) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		Layer_update (my layers.at [ilayer], learningRate);
	}
}

static void Layer_PatternList_applyToInput (Layer me, PatternList thee, integer rowNumber) {
	Melder_assert (my numberOfInputNodes == thy nx);
	for (integer ifeature = 1; ifeature <= my numberOfInputNodes; ifeature ++) {
		my inputActivities [ifeature] = thy z [rowNumber] [ifeature];
	}
}

void Net_PatternList_applyToInput (Net me, PatternList thee, integer rowNumber) {
	Layer_PatternList_applyToInput (my layers.at [1], thee, rowNumber);
}

static void Layer_PatternList_applyToOutput (Layer me, PatternList thee, integer rowNumber) {
	Melder_assert (my numberOfOutputNodes == thy nx);
	for (integer icat = 1; icat <= my numberOfOutputNodes; icat ++) {
		my outputActivities [icat] = thy z [rowNumber] [icat];
	}
}

void Net_PatternList_applyToOutput (Net me, PatternList thee, integer rowNumber) {
	Layer_PatternList_applyToOutput (my layers.at [my layers.size], thee, rowNumber);
}

void Net_PatternList_learn (Net me, PatternList thee, double learningRate) {
	for (integer ipattern = 1; ipattern <= thy ny; ipattern ++) {
		Net_PatternList_applyToInput (me, thee, ipattern);
		Net_spreadUp (me, kNet_activationType::STOCHASTIC);
		for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
			Layer layer = my layers.at [ilayer];
			Layer_spreadDown_reconstruction (layer);
			Layer_spreadUp_reconstruction (layer);
			Layer_update (layer, learningRate);
		}
	}
}

void Net_PatternList_learnByLayer (Net me, PatternList thee, double learningRate) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		Layer layer = my layers.at [ilayer];
		for (integer ipattern = 1; ipattern <= thy ny; ipattern ++) {
			Layer_PatternList_applyToInput (my layers.at [1], thee, ipattern);
			Layer_spreadUp (my layers.at [1]);
			Layer_sampleOutput (my layers.at [1]);
			for (integer jlayer = 2; jlayer <= ilayer; jlayer ++) {
				copyOutputsToInputs (my layers.at [jlayer - 1], my layers.at [jlayer]);
				Layer_spreadUp (my layers.at [jlayer]);
				Layer_sampleOutput (my layers.at [jlayer]);
			}
			Layer_spreadDown_reconstruction (layer);
			Layer_spreadUp_reconstruction (layer);
			Layer_update (layer, learningRate);
		}
	}
}

autoActivationList Net_PatternList_to_ActivationList (Net me, PatternList thee, kNet_activationType activationType) {
	try {
		Layer outputLayer = my layers.at [my layers.size];
		autoActivationList activations = ActivationList_create (thy ny, outputLayer -> numberOfOutputNodes);
		for (long ipattern = 1; ipattern <= thy ny; ipattern ++) {
			Net_PatternList_applyToInput (me, thee, ipattern);
			Net_spreadUp (me, activationType);
			NUMvector_copyElements <double> (outputLayer -> outputActivities, activations -> z [ipattern], 1, outputLayer -> numberOfOutputNodes);
		}
		return activations;
	} catch (MelderError) {
		Melder_throw (me, thee, U"No ActivationList created.");
	}
}

static autoMatrix Layer_extractInputActivities (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputActivities, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input activities not extracted.");
	}
}

autoMatrix Net_extractInputActivities (Net me) {
	return Layer_extractInputActivities (my layers.at [1]);
}

static autoMatrix Layer_extractOutputActivities (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputActivities, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output activities not extracted.");
	}
}

autoMatrix Net_extractOutputActivities (Net me) {
	return Layer_extractOutputActivities (my layers.at [my layers.size]);
}

static autoMatrix Layer_extractInputReconstruction (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputReconstruction, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input reconstruction not extracted.");
	}
}

autoMatrix Net_extractInputReconstruction (Net me) {
	return Layer_extractInputReconstruction (my layers.at [1]);
}

static autoMatrix Layer_extractOutputReconstruction (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputReconstruction, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output reconstruction not extracted.");
	}
}

autoMatrix Net_extractOutputReconstruction (Net me) {
	return Layer_extractOutputReconstruction (my layers.at [my layers.size]);
}

static autoMatrix Layer_extractInputBiases (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputBiases, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input biases not extracted.");
	}
}

autoMatrix Net_extractInputBiases (Net me, integer layerNumber) {
	return Layer_extractInputBiases (my layers.at [layerNumber]);
}

static autoMatrix Layer_extractOutputBiases (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputBiases, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input biases not extracted.");
	}
}

autoMatrix Net_extractOutputBiases (Net me, integer layerNumber) {
	return Layer_extractOutputBiases (my layers.at [layerNumber]);
}

static autoMatrix Layer_extractWeights (Layer me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfInputNodes, my numberOfOutputNodes);
		NUMmatrix_copyElements <double> (my weights, thy z, 1, my numberOfInputNodes, 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": weights not extracted.");
	}
}

autoMatrix Net_extractWeights (Net me, integer layerNumber) {
	return Layer_extractWeights (my layers.at [layerNumber]);
}

autonummat Net_getWeights_nummat (Net me, integer layerNumber) {
	Layer layer = my layers.at [layerNumber];
	return copy_nummat ({ layer -> weights, layer -> numberOfInputNodes, layer -> numberOfOutputNodes });
}

/* End of file Net.cpp */
