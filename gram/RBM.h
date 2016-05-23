#ifndef _RBM_h_
#define _RBM_h_
/* RBM
 *
 * Copyright (C) 2016 Paul Boersma
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

#include "Table.h"
#include "Pattern.h"

#include "RBM_def.h"

void RBM_init (RBM me, long numberOfInputNodes, long numberOfOutputNodes, bool inputsAreBinary);

autoRBM RBM_create (long numberOfInputNodes, long numberOfOutputNodes, bool inputsAreBinary);

void RBM_spreadUp (RBM me);
void RBM_spreadDown (RBM me);
void RBM_spreadDown_reconstruction (RBM me);
void RBM_spreadUp_reconstruction (RBM me);
void RBM_sampleInput (RBM me);
void RBM_sampleOutput (RBM me);
void RBM_update (RBM me, double learningRate);

void RBM_Pattern_applyToInput (RBM me, Pattern thee, long rowNumber);
void RBM_Pattern_applyToOutput (RBM me, Pattern thee, long rowNumber);
void RBM_Pattern_learn (RBM me, Pattern thee, double learningRate);

autoMatrix RBM_extractInputActivities (RBM me);
autoMatrix RBM_extractOutputActivities (RBM me);
autoMatrix RBM_extractInputReconstruction (RBM me);
autoMatrix RBM_extractOutputReconstruction (RBM me);

autoMatrix RBM_extractInputBiases (RBM me);
autoMatrix RBM_extractOutputBiases (RBM me);
autoMatrix RBM_extractWeights (RBM me);

/* End of file RBM.h */
#endif
