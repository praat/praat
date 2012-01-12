/* HMM_def.h
 *
 * Copyright (C) 2010-2011 David Weenink
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
 djmw 20100929 Initial definition.
 djmw 20110329 oo_STRINGW -> oo_STRING
 */


#define ooSTRUCT HMM_State
oo_DEFINE_CLASS (HMM_State, Data)

	oo_STRING(label)

oo_END_CLASS(HMM_State)
#undef ooSTRUCT


#define ooSTRUCT HMM_Observation
oo_DEFINE_CLASS (HMM_Observation, Data)

	oo_STRING(label)
	oo_OBJECT (GaussianMixture, 1, gm)

oo_END_CLASS(HMM_Observation)
#undef ooSTRUCT


#define ooSTRUCT HMM
oo_DEFINE_CLASS (HMM, Data)

	oo_INT (notHidden) // to model a Markov model instead of HMM
	oo_INT (leftToRight)
	oo_LONG (numberOfStates)
	oo_LONG (numberOfObservationSymbols)
	oo_LONG (numberOfMixtureComponents)
	oo_LONG (componentDimension)
	oo_INT (componentStorage)
	oo_DOUBLE_MATRIX_FROM (transitionProbs, 0, numberOfStates, 1, numberOfStates + 1) // row 0: initial probabilities
	oo_DOUBLE_MATRIX (emissionProbs, numberOfStates, numberOfObservationSymbols)
	oo_OBJECT (Ordered, 1, states)
	oo_OBJECT (Ordered, 1, observationSymbols)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS(HMM)
#undef ooSTRUCT


#define ooSTRUCT HMM_Viterbi
oo_DEFINE_CLASS (HMM_Viterbi, Data)
	oo_LONG (numberOfTimes)
	oo_LONG (numberOfStates)
	oo_DOUBLE (prob)
	oo_DOUBLE_MATRIX (viterbi, numberOfStates, numberOfTimes)
	oo_LONG_MATRIX (bp, numberOfStates, numberOfTimes)
	oo_LONG_VECTOR (path, numberOfTimes)
oo_END_CLASS(HMM_Viterbi)
#undef ooSTRUCT


/* End of file HMM_def.h */
