/* Net_def.h
 *
 * Copyright (C) 2017-2018 Paul Boersma
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


#define ooSTRUCT RBMLayer
oo_DEFINE_CLASS (RBMLayer, Layer)

	oo_QUESTION (inputsAreBinary)
	oo_MAT (weights, numberOfInputNodes, numberOfOutputNodes)
	oo_VEC (inputBiases, numberOfInputNodes)
	oo_VEC (outputBiases, numberOfOutputNodes)

	oo_VEC (inputReconstruction, numberOfInputNodes)
	oo_VEC (outputReconstruction, numberOfOutputNodes)

	#if oo_DECLARING
		void v_spreadUp (kLayer_activationType)
			override;
		void v_sampleInput ()
			override;
		void v_spreadDown (kLayer_activationType)
			override;
		void v_spreadDown_reconstruction ()
			override;
		void v_spreadUp_reconstruction ()
			override;
		void v_update (double learningRate)
			override;
		void v_updateFirstPhase (double learningRate)
			override;
		void v_updateSecondPhase (double learningRate)
			override;
		autoMatrix v_extractInputReconstruction ()
			override;
		autoMatrix v_extractOutputReconstruction ()
			override;
		autoMatrix v_extractInputBiases ()
			override;
		autoMatrix v_extractOutputBiases ()
			override;
		autoMatrix v_extractWeights ()
			override;
		autoMAT v_getWeights ()
			override;
	#endif

oo_END_CLASS (RBMLayer)
#undef ooSTRUCT



#define ooSTRUCT InputLayer
oo_DEFINE_CLASS (InputLayer, Layer)

oo_END_CLASS (InputLayer)
#undef ooSTRUCT



#define ooSTRUCT FullyConnectedLayer
oo_DEFINE_CLASS (FullyConnectedLayer, Layer)

	oo_MAT (weights, numberOfInputNodes, numberOfOutputNodes)
	oo_VEC (outputBiases, numberOfOutputNodes)

	#if oo_DECLARING &&0
		void v_spreadUp (kLayer_activationType)
			override;
		void v_update (double learningRate)
			override;
		autoMatrix v_extractOutputBiases ()
			override;
		autoMatrix v_extractWeights ()
			override;
		autoMAT v_getWeights ()
			override;
	#endif

oo_END_CLASS (FullyConnectedLayer)
#undef ooSTRUCT



#define ooSTRUCT Net
oo_DEFINE_CLASS (Net, Daata)

	oo_OBJECT (LayerList, 0, layers)

oo_END_CLASS (Net)
#undef ooSTRUCT


/* End of file Net_def.h */
