/* FFNet_def.h
 *
 * Copyright (C) 1994-2018 David Weenink
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

/*
 djmw 19961104
 djmw 20020712 GPL header
 djmw 20060529 Added version number to oo_Collection
 djmw 20080122 float -> double
*/

#define ooSTRUCT FFNet
oo_DEFINE_CLASS (FFNet, Daata)

	oo_INTEGER (nLayers)	/* number of layers */
	
	oo_INTEGER_VECTOR_FROM (nUnitsInLayer, 0, nLayers)

	oo_INT (outputsAreLinear)

	oo_INT (nonLinearityType)

	oo_INT (costFunctionType)

	oo_COLLECTION (Categories, outputCategories, SimpleString, 0)

	oo_INTEGER (nWeights)	/* number of weights */

	oo_VEC (w, nWeights)
	
	#if ! oo_READING && ! oo_WRITING && ! oo_COMPARING
		oo_INTEGER (nNodes)
		oo_INTEGER (nInputs)
		oo_INTEGER (nOutputs)
		oo_INTEGER (dimension)

		#if oo_DECLARING
			double (*nonLinearity) (FFNet /* me */, double /* x */, double * /* deriv */);
    		void *nlClosure;
    		double (*costFunction) (FFNet /* me */, constVEC& /* target */);
			void *cfClosure;
		#endif
		
		#if oo_DECLARING
			oo_DOUBLE (accumulatedCost)
			oo_INTEGER (nPatterns)
			oo_INTEGER (currentPattern)
			MAT inputPattern, targetActivation;
		#endif
		#if oo_DECLARING || oo_DESTROYING
			oo_OBJECT (Minimizer, 0, minimizer)
		#endif

		oo_VEC (activity, nNodes)
		oo_INTVEC (isbias, nNodes)
		oo_INTVEC (nodeFirst, nNodes)
		oo_INTVEC (nodeLast, nNodes)
		oo_INTVEC (wFirst, nNodes)
		oo_INTVEC (wLast, nNodes)
			
		oo_VEC (deriv, nNodes)	
		oo_VEC (error, nNodes)	
		oo_INTVEC (wSelected, nWeights)
		oo_VEC (dw, nWeights)
		oo_VEC (dwi, nWeights)
	#endif

	#if oo_READING
		bookkeeping (this);
		FFNet_setNonLinearity (this, nonLinearityType);
		FFNet_setCostFunction (this, costFunctionType);
	#endif

	#if oo_COPYING
		thy nonLinearity = our nonLinearity;
    	thy nlClosure = our nlClosure;
    	thy costFunction = our costFunction;
		thy cfClosure = our cfClosure;
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FFNet)
#undef ooSTRUCT

/* End of file FFNet_def.h */	
