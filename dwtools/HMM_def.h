/* HMM_def.h
 *
 * Copyright (C) 2010-2018 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */


#define ooSTRUCT HMMState
oo_DEFINE_CLASS (HMMState, Daata)

	oo_STRING (label)

oo_END_CLASS (HMMState)
#undef ooSTRUCT


#define ooSTRUCT HMMObservation
oo_DEFINE_CLASS (HMMObservation, Daata)

	oo_STRING (label)
	oo_OBJECT (GaussianMixture, 0, gm)

oo_END_CLASS (HMMObservation)
#undef ooSTRUCT


#define ooSTRUCT HMM
oo_DEFINE_CLASS (HMM, Daata)

	oo_INT (notHidden) // to model a Markov model instead of HMM
	oo_INT (leftToRight)
	oo_INTEGER (numberOfStates)
	oo_INTEGER (numberOfObservationSymbols)
	oo_INTEGER (numberOfMixtureComponents)
	oo_INTEGER (componentDimension)
	oo_ENUM (kHMMstorage, componentStorage)
	oo_FROM (1)
		oo_VEC (initialStateProbs, numberOfStates)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			oo_MAT (transitionProbs, numberOfStates + 1, numberOfStates + 1)
			our initialStateProbs = copy_VEC (our transitionProbs.row (1).part (1, our numberOfStates));
			our transitionProbs = copy_MAT (our transitionProbs.horizontalBand (2, our numberOfStates + 1));
		oo_VERSION_ELSE
			oo_MAT (transitionProbs, numberOfStates, numberOfStates + 1)
		oo_VERSION_END
	#else
		oo_MAT (transitionProbs, numberOfStates, numberOfStates + 1)
	#endif
	oo_MAT (emissionProbs, numberOfStates, numberOfObservationSymbols)
	oo_OBJECT (HMMStateList, 0, states)
	oo_OBJECT (HMMObservationList, 0, observationSymbols)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (HMM)
#undef ooSTRUCT


#define ooSTRUCT HMMViterbi
oo_DEFINE_CLASS (HMMViterbi, Daata)

	oo_INTEGER (numberOfTimes)
	oo_INTEGER (numberOfStates)
	oo_DOUBLE (prob)
	oo_MAT (viterbi, numberOfStates, numberOfTimes)
	oo_INTMAT (bp, numberOfStates, numberOfTimes)
	oo_INTVEC (path, numberOfTimes)

oo_END_CLASS (HMMViterbi)
#undef ooSTRUCT


/* End of file HMM_def.h */
