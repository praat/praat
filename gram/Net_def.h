/* Net_def.h
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


#define ooSTRUCT RBMLayer
oo_DEFINE_CLASS (RBMLayer, Layer)

	oo_QUESTION (inputsAreBinary)
	oo_DOUBLE_MATRIX (weights, numberOfInputNodes, numberOfOutputNodes)
	oo_DOUBLE_VECTOR (inputBiases, numberOfInputNodes)
	oo_DOUBLE_VECTOR (outputBiases, numberOfOutputNodes)

	oo_DOUBLE_VECTOR (inputReconstruction, numberOfInputNodes)
	oo_DOUBLE_VECTOR (outputReconstruction, numberOfOutputNodes)

	#if oo_DECLARING
		virtual void v_spreadUp (kLayer_activationType)
			override;
		virtual void v_sampleInput ()
			override;
		virtual void v_spreadDown (kLayer_activationType)
			override;
		virtual void v_spreadDown_reconstruction ()
			override;
		virtual void v_spreadUp_reconstruction ()
			override;
		virtual void v_update (double learningRate)
			override;
		virtual autoMatrix v_extractInputReconstruction ()
			override;
		virtual autoMatrix v_extractOutputReconstruction ()
			override;
		virtual autoMatrix v_extractInputBiases ()
			override;
		virtual autoMatrix v_extractOutputBiases ()
			override;
		virtual autoMatrix v_extractWeights ()
			override;
		virtual autonummat v_getWeights_nummat ()
			override;
	#endif

oo_END_CLASS (RBMLayer)
#undef ooSTRUCT



#define ooSTRUCT FullyConnectedLayer
oo_DEFINE_CLASS (FullyConnectedLayer, Layer)

	oo_DOUBLE_MATRIX (weights, numberOfInputNodes, numberOfOutputNodes)
	oo_DOUBLE_VECTOR (outputBiases, numberOfOutputNodes)

	#if oo_DECLARING &&0
		virtual void v_spreadUp (kLayer_activationType)
			override;
		virtual void v_update (double learningRate)
			override;
		virtual autoMatrix v_extractOutputBiases ()
			override;
		virtual autoMatrix v_extractWeights ()
			override;
		virtual autonummat v_getWeights_nummat ()
			override;
	#endif

oo_END_CLASS (FullyConnectedLayer)
#undef ooSTRUCT



#define ooSTRUCT Net
oo_DEFINE_CLASS (Net, Daata)

	oo_AUTO_OBJECT (LayerList, 0, layers)

oo_END_CLASS (Net)
#undef ooSTRUCT


/* End of file Net_def.h */
