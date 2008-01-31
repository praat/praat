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
	
	oo_LONG_VECTOR_FROM (nUnitsInLayer, 0, my nLayers)

	oo_INT (outputsAreLinear)

	oo_INT (nonLinearityType)

	oo_INT (costFunctionType)

	oo_COLLECTION (Categories, outputCategories, SimpleString, 0)

	oo_LONG (nWeights)	/* number of weights */

	oo_DOUBLE_VECTOR (w, my nWeights)
	
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

		oo_DOUBLE_VECTOR (activity, my nNodes)
		oo_LONG_VECTOR (isbias, my nNodes)
		oo_LONG_VECTOR (nodeFirst, my nNodes)
		oo_LONG_VECTOR (nodeLast, my nNodes)
		oo_LONG_VECTOR (wFirst, my nNodes)
		oo_LONG_VECTOR (wLast, my nNodes)
			
		oo_DOUBLE_VECTOR (deriv, my nNodes)	
		oo_DOUBLE_VECTOR (error, my nNodes)	
		oo_LONG_VECTOR (wSelected, my nWeights)
		oo_DOUBLE_VECTOR (dw, my nWeights)
		oo_DOUBLE_VECTOR (dwi, my nWeights)
	#endif

	#if oo_READING
		if (! bookkeeping (me)) return 0;
		FFNet_setNonLinearity (me, my nonLinearityType);
		FFNet_setCostFunction (me, my costFunctionType);
	#endif

	#if oo_COPYING
		thy nonLinearity = my nonLinearity;
    	thy nlClosure = my nlClosure;
    	thy costFunction = my costFunction;
		thy cfClosure = my cfClosure;
	#endif
	
oo_END_CLASS (FFNet)
#undef ooSTRUCT

/* End of file FFNet_def.h */	
