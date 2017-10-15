#ifndef _DeepBeliefNetwork_h_
#define _DeepBeliefNetwork_h_
/* DeepBeliefNetwork.h
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

#include "RBM.h"
#include "Table.h"
#include "PatternList.h"

#include "DeepBeliefNetwork_enums.h"
#include "DeepBeliefNetwork_def.h"

void DeepBeliefNetwork_init (DeepBeliefNetwork me, numvec numbersOfNodes, bool inputsAreBinary);

autoDeepBeliefNetwork DeepBeliefNetwork_create (numvec numbersOfNodes, bool inputsAreBinary);

void DeepBeliefNetwork_addLayer (DeepBeliefNetwork me, integer numberOfOutputNodes);

void DeepBeliefNetwork_spreadUp (DeepBeliefNetwork me, kDeepBeliefNetwork_activationType activationType);
void DeepBeliefNetwork_spreadDown (DeepBeliefNetwork me, kDeepBeliefNetwork_activationType activationType);
void DeepBeliefNetwork_spreadDown_reconstruction (DeepBeliefNetwork me);
void DeepBeliefNetwork_spreadUp_reconstruction (DeepBeliefNetwork me);
void DeepBeliefNetwork_sampleInput (DeepBeliefNetwork me);
void DeepBeliefNetwork_sampleOutput (DeepBeliefNetwork me);
void DeepBeliefNetwork_update (DeepBeliefNetwork me, double learningRate);

void DeepBeliefNetwork_PatternList_applyToInput (DeepBeliefNetwork me, PatternList thee, integer rowNumber);
void DeepBeliefNetwork_PatternList_applyToOutput (DeepBeliefNetwork me, PatternList thee, integer rowNumber);
void DeepBeliefNetwork_PatternList_learn (DeepBeliefNetwork me, PatternList thee, double learningRate);
void DeepBeliefNetwork_PatternList_learnByLayer (DeepBeliefNetwork me, PatternList thee, double learningRate);

autoMatrix DeepBeliefNetwork_extractInputActivities (DeepBeliefNetwork me);
autoMatrix DeepBeliefNetwork_extractOutputActivities (DeepBeliefNetwork me);
autoMatrix DeepBeliefNetwork_extractInputReconstruction (DeepBeliefNetwork me);
autoMatrix DeepBeliefNetwork_extractOutputReconstruction (DeepBeliefNetwork me);

autoMatrix DeepBeliefNetwork_extractInputBiases (DeepBeliefNetwork me, integer layerNumber);
autoMatrix DeepBeliefNetwork_extractOutputBiases (DeepBeliefNetwork me, integer layerNumber);
autoMatrix DeepBeliefNetwork_extractWeights (DeepBeliefNetwork me, integer layerNumber);

autonummat DeepBeliefNetwork_getWeights_nummat (DeepBeliefNetwork me, integer layerNumber);

/* End of file DeepBeliefNetwork.h */
#endif
