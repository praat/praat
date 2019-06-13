#ifndef _Net_h_
#define _Net_h_
/* Net.h
 *
 * Copyright (C) 2017-2019 Paul Boersma
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

#include "Layer.h"

#include "Table.h"
#include "PatternList.h"
#include "ActivationList.h"

#include "Net_def.h"

void Net_initAsDeepBeliefNet (Net me, constVECVU const& numbersOfNodes, bool inputsAreBinary);

void Net_initEmpty (Net me);
autoNet Net_createEmpty (integer numberOfInputNodes);

autoNet Net_createAsDeepBeliefNet (constVECVU const& numbersOfNodes, bool inputsAreBinary);

void Net_addInputLayer (Net me, integer numberOfNodes);
void Net_addRBMLayer (Net me, integer numberOfOutputNodes);
void Net_addFullyConnectedLayer (Net me, integer numberOfOutputNodes);

void Net_spreadUp (Net me, kLayer_activationType activationType);
void Net_spreadDown (Net me, kLayer_activationType activationType);
void Net_spreadDown_reconstruction (Net me);
void Net_spreadUp_reconstruction (Net me);
void Net_sampleInput (Net me);
void Net_sampleOutput (Net me);
void Net_update (Net me, double learningRate);

void Net_PatternList_applyToInput (Net me, PatternList thee, integer rowNumber);
void Net_PatternList_applyToOutput (Net me, PatternList thee, integer rowNumber);
void Net_PatternList_learn (Net me, PatternList thee, double learningRate);
void Net_PatternList_learnByLayer (Net me, PatternList thee, double learningRate);
void Net_PatternList_learn_twoPhases (Net me, PatternList thee, double learningRate);

autoActivationList Net_PatternList_to_ActivationList (Net me, PatternList thee, kLayer_activationType activationType);

autoMatrix Net_extractInputActivities (Net me);
autoMatrix Net_extractOutputActivities (Net me);
autoMatrix Net_extractInputReconstruction (Net me);
autoMatrix Net_extractOutputReconstruction (Net me);

autoMatrix Net_extractInputBiases (Net me, integer layerNumber);
autoMatrix Net_extractOutputBiases (Net me, integer layerNumber);
autoMatrix Net_extractWeights (Net me, integer layerNumber);

autoMAT Net_getWeights (Net me, integer layerNumber);

/* End of file Net.h */
#endif
