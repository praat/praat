/* FFNet_def.h
 *
 * Copyright (C) 1994-2008 David Weenink
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
 djmw 19961104
 djmw 20020712 GPL header
 djmw 20060529 Added version number to oo_Collection
 djmw 20080122 float -> double
*/

#define ooSTRUCT FFNet
oo_DEFINE_CLASS (FFNet, Data)

	oo_LONG (nLayers)	/* number of layers */
	
	oo_LONG_VECTOR_FROM (nUnitsInLayer, 0, nLayers)

	oo_INT (outputsAreLinear)

	oo_INT (nonLinearityType)

	oo_INT (costFunctionType)

	oo_COLLECTION (Categories, outputCategories, SimpleString, 0)

	oo_LONG (nWeights)	/* number of weights */

	oo_DOUBLE_VECTOR (w, nWeights)
	
	#if ! oo_READING && ! oo_WRITING && ! oo_COMPARING
		oo_LONG (nNodes)
		oo_LONG (nInputs)
		oo_LONG (nOutputs)
		oo_LONG (dimension)

		#if oo_DECLARING
			double (*nonLinearity) (I, double /* x */, double * /* deriv */);
    		void *nlClosure;
    		double (*costFunction) (I, const double * /* target */);
			void *cfClosure;
		#endif
		
		#if oo_DECLARING
			oo_DOUBLE (accumulatedCost)
			oo_LONG (nPatterns)
			oo_LONG (currentPattern)
			double **inputPattern, **targetActivation;
		#endif
		#if oo_DECLARING || oo_DESTROYING
			oo_OBJECT (Minimizer, 0, minimizer)
		#endif

		oo_DOUBLE_VECTOR (activity, nNodes)
		oo_LONG_VECTOR (isbias, nNodes)
		oo_LONG_VECTOR (nodeFirst, nNodes)
		oo_LONG_VECTOR (nodeLast, nNodes)
		oo_LONG_VECTOR (wFirst, nNodes)
		oo_LONG_VECTOR (wLast, nNodes)
			
		oo_DOUBLE_VECTOR (deriv, nNodes)	
		oo_DOUBLE_VECTOR (error, nNodes)	
		oo_LONG_VECTOR (wSelected, nWeights)
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
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (FFNet)
#undef ooSTRUCT

/* End of file FFNet_def.h */	
