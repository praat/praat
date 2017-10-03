/* FFNet_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
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

	oo_AUTO_COLLECTION (Categories, outputCategories, SimpleString, 0)

	oo_INTEGER (nWeights)	/* number of weights */

	oo_DOUBLE_VECTOR (w, nWeights)
	
	#if ! oo_READING && ! oo_WRITING && ! oo_COMPARING
		oo_INTEGER (nNodes)
		oo_INTEGER (nInputs)
		oo_INTEGER (nOutputs)
		oo_INTEGER (dimension)

		#if oo_DECLARING
			double (*nonLinearity) (FFNet /* me */, double /* x */, double * /* deriv */);
    		void *nlClosure;
    		double (*costFunction) (FFNet /* me */, const double * /* target */);
			void *cfClosure;
		#endif
		
		#if oo_DECLARING
			oo_DOUBLE (accumulatedCost)
			oo_INTEGER (nPatterns)
			oo_INTEGER (currentPattern)
			double **inputPattern, **targetActivation;
		#endif
		#if oo_DECLARING || oo_DESTROYING
			oo_AUTO_OBJECT (Minimizer, 0, minimizer)
		#endif

		oo_DOUBLE_VECTOR (activity, nNodes)
		oo_INTEGER_VECTOR (isbias, nNodes)
		oo_INTEGER_VECTOR (nodeFirst, nNodes)
		oo_INTEGER_VECTOR (nodeLast, nNodes)
		oo_INTEGER_VECTOR (wFirst, nNodes)
		oo_INTEGER_VECTOR (wLast, nNodes)
			
		oo_DOUBLE_VECTOR (deriv, nNodes)	
		oo_DOUBLE_VECTOR (error, nNodes)	
		oo_INTEGER_VECTOR (wSelected, nWeights)
		oo_DOUBLE_VECTOR (dw, nWeights)
		oo_DOUBLE_VECTOR (dwi, nWeights)
	#endif

	#if oo_READING
		bookkeeping (this);
		FFNet_setNonLinearity (this, nonLinearityType);
		FFNet_setCostFunction (this, costFunctionType);
	#endif

	#if oo_COPYING
		thy nonLinearity = nonLinearity;
    	thy nlClosure = nlClosure;
    	thy costFunction = costFunction;
		thy cfClosure = cfClosure;
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FFNet)
#undef ooSTRUCT

/* End of file FFNet_def.h */	
