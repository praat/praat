/* DeepBeliefNetwork.cpp
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
#include "DeepBeliefNetwork.h"

#include "oo_DESTROY.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_COPY.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_EQUAL.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_WRITE_TEXT.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_READ_TEXT.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_WRITE_BINARY.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_READ_BINARY.h"
#include "DeepBeliefNetwork_def.h"
#include "oo_DESCRIPTION.h"
#include "DeepBeliefNetwork_def.h"

#include "enums_getText.h"
#include "DeepBeliefNetwork_enums.h"
#include "enums_getValue.h"
#include "DeepBeliefNetwork_enums.h"

Thing_implement (DeepBeliefNetwork, Daata, 0);

void DeepBeliefNetwork_init (DeepBeliefNetwork me, numvec numbersOfNodes, bool inputsAreBinary) {
	if (numbersOfNodes.size < 2)
		Melder_throw (U"A DeepBeliefNetwork should have at least two levels of nodes.");
	integer numberOfLayers = numbersOfNodes.size - 1;
	for (integer ilayer = 1; ilayer <= numberOfLayers; ilayer ++) {
		autoRBM layer = RBM_create (
			Melder_iround_tieUp (numbersOfNodes [ilayer]),
			Melder_iround_tieUp (numbersOfNodes [ilayer + 1]),
			ilayer == 1 ? inputsAreBinary : true
		);
		my layers. addItem_move (layer.move());
	}
}

autoDeepBeliefNetwork DeepBeliefNetwork_create (numvec numbersOfNodes, bool inputsAreBinary) {
	try {
		autoDeepBeliefNetwork me = Thing_new (DeepBeliefNetwork);
		DeepBeliefNetwork_init (me.get(), numbersOfNodes, inputsAreBinary);
		return me;
	} catch (MelderError) {
		Melder_throw (U"DeepBeliefNetwork not created.");
	}
}

static void copyOutputsToInputs (RBM me, RBM you) {
	Melder_assert (my numberOfOutputNodes == your numberOfInputNodes);
	for (integer inode = 1; inode <= my numberOfOutputNodes; inode ++) {
		your inputActivities [inode] = my outputActivities [inode];
	}
}

void DeepBeliefNetwork_spreadUp (DeepBeliefNetwork me, kDeepBeliefNetwork_activationType activationType) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		RBM layer = my layers.at [ilayer];
		if (ilayer > 1)
			copyOutputsToInputs (my layers.at [ilayer - 1], layer);
		RBM_spreadUp (layer);
		if (activationType == kDeepBeliefNetwork_activationType::STOCHASTIC)
			RBM_sampleOutput (layer);
	}
}

void DeepBeliefNetwork_sampleInput (DeepBeliefNetwork me) {
	RBM_sampleInput (my layers.at [1]);
}

void DeepBeliefNetwork_sampleOutput (DeepBeliefNetwork me) {
	RBM_sampleOutput (my layers.at [my layers.size]);
}

static void copyInputsToOutputs (RBM me, RBM you) {
	Melder_assert (my numberOfInputNodes == your numberOfOutputNodes);
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		your outputActivities [inode] = my inputActivities [inode];
	}
}

void DeepBeliefNetwork_spreadDown (DeepBeliefNetwork me, kDeepBeliefNetwork_activationType activationType) {
	for (integer ilayer = my layers.size; ilayer > 0; ilayer --) {
		RBM layer = my layers.at [ilayer];
		if (ilayer < my layers.size)
			copyInputsToOutputs (my layers.at [ilayer + 1], layer);
		RBM_spreadDown (layer);
		if (activationType == kDeepBeliefNetwork_activationType::STOCHASTIC)
			RBM_sampleInput (layer);
	}
}

void DeepBeliefNetwork_spreadDown_reconstruction (DeepBeliefNetwork me) {
	for (integer ilayer = my layers.size; ilayer > 0; ilayer --) {
		RBM_spreadDown_reconstruction (my layers.at [ilayer]);
	}
}

void DeepBeliefNetwork_spreadUp_reconstruction (DeepBeliefNetwork me) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		RBM_spreadUp_reconstruction (my layers.at [ilayer]);
	}
}

void DeepBeliefNetwork_update (DeepBeliefNetwork me, double learningRate) {
	for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
		RBM_update (my layers.at [ilayer], learningRate);
	}
}

void DeepBeliefNetwork_PatternList_applyToInput (DeepBeliefNetwork me, PatternList thee, integer rowNumber) {
	RBM_PatternList_applyToInput (my layers.at [1], thee, rowNumber);
}

void DeepBeliefNetwork_PatternList_applyToOutput (DeepBeliefNetwork me, PatternList thee, integer rowNumber) {
	RBM_PatternList_applyToOutput (my layers.at [my layers.size], thee, rowNumber);
}

void DeepBeliefNetwork_PatternList_learn (DeepBeliefNetwork me, PatternList thee, double learningRate) {
	for (integer ipattern = 1; ipattern <= thy ny; ipattern ++) {
		DeepBeliefNetwork_PatternList_applyToInput (me, thee, ipattern);
		DeepBeliefNetwork_spreadUp (me, kDeepBeliefNetwork_activationType::STOCHASTIC);
		for (integer ilayer = 1; ilayer <= my layers.size; ilayer ++) {
			RBM layer = my layers.at [ilayer];
			RBM_spreadDown_reconstruction (layer);
			RBM_spreadUp_reconstruction (layer);
			RBM_update (layer, learningRate);
		}
	}
}

autoMatrix DeepBeliefNetwork_extractInputActivities (DeepBeliefNetwork me) {
	return RBM_extractInputActivities (my layers.at [1]);
}

autoMatrix DeepBeliefNetwork_extractOutputActivities (DeepBeliefNetwork me) {
	return RBM_extractOutputActivities (my layers.at [my layers.size]);
}

autoMatrix DeepBeliefNetwork_extractInputReconstruction (DeepBeliefNetwork me) {
	return RBM_extractInputReconstruction (my layers.at [1]);
}

autoMatrix DeepBeliefNetwork_extractOutputReconstruction (DeepBeliefNetwork me) {
	return RBM_extractOutputReconstruction (my layers.at [my layers.size]);
}

autoMatrix DeepBeliefNetwork_extractInputBiases (DeepBeliefNetwork me, integer layerNumber) {
	return RBM_extractInputBiases (my layers.at [layerNumber]);
}

autoMatrix DeepBeliefNetwork_extractOutputBiases (DeepBeliefNetwork me, integer layerNumber) {
	return RBM_extractOutputBiases (my layers.at [layerNumber]);
}

autoMatrix DeepBeliefNetwork_extractWeights (DeepBeliefNetwork me, integer layerNumber) {
	return RBM_extractWeights (my layers.at [layerNumber]);
}

autonummat DeepBeliefNetwork_getWeights_nummat (DeepBeliefNetwork me, integer layerNumber) {
	RBM layer = my layers.at [layerNumber];
	return copy_nummat ({ layer -> weights, layer -> numberOfInputNodes, layer -> numberOfOutputNodes });
}

/* End of file DeepBeliefNetwork.cpp */
