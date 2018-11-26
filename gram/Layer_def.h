/* Layer_def.h
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


#define ooSTRUCT Layer
oo_DEFINE_CLASS (Layer, Daata)

	oo_INTEGER (numberOfInputNodes)
	oo_VEC (inputActivities, numberOfInputNodes)
	oo_INTEGER (numberOfOutputNodes)
	oo_VEC (outputActivities, numberOfOutputNodes)

	#if oo_DECLARING
		virtual void v_spreadUp (kLayer_activationType) { }
		virtual void v_sampleInput () { }
		virtual void v_spreadDown (kLayer_activationType) { }
		virtual void v_spreadDown_reconstruction () { }
		virtual void v_spreadUp_reconstruction () { }
		virtual void v_update (double /* learningRate */) { }
		virtual void v_updateFirstPhase (double /* learningRate */) { }
		virtual void v_updateSecondPhase (double /* learningRate */) { }
		virtual autoMatrix v_extractInputReconstruction () { return autoMatrix (); }
		virtual autoMatrix v_extractOutputReconstruction () { return autoMatrix (); }
		virtual autoMatrix v_extractInputBiases () { return autoMatrix (); }
		virtual autoMatrix v_extractOutputBiases () { return autoMatrix (); }
		virtual autoMatrix v_extractWeights () { return autoMatrix (); }
		virtual autoMAT v_getWeights () { return autoMAT (); }
	#endif

oo_END_CLASS (Layer)
#undef ooSTRUCT


/* End of file Layer_def.h */
