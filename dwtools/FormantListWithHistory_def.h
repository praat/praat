/* FormantListWithHistory_def.h
 *
 * Copyright (C) 2020 David Weenink
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

#define ooSTRUCT FormantAnalysisHistory
oo_DEFINE_STRUCT (FormantAnalysisHistory)
	oo_ENUM (kLPC_Analysis, lpcType)
	oo_DOUBLE (timeStep)
	oo_DOUBLE (maximumNumberOfFormants)
	oo_DOUBLE (windowLength)
	oo_DOUBLE (preemphasisFrequency)
	oo_DOUBLE (tol1) // marple
	oo_DOUBLE (tol2)
	oo_DOUBLE (huberNumberOfStdDev) // robust
	oo_INTEGER (maximumNumberOfIterations)
	oo_DOUBLE (tol)
	
oo_END_STRUCT(FormantAnalysisHistory)
#undef ooSTRUCT

#define ooSTRUCT FormantListWithHistory
oo_DEFINE_CLASS (FormantListWithHistory, Function)

	oo_INTEGER (numberOfElements)
	oo_STRUCT (FormantAnalysisHistory, formantAnalysisHistory)
	oo_VEC (ceilings, numberOfElements) // the 'Maximum formant's
	oo_COLLECTION_OF (OrderedOf, formants, Formant, 0)
	
oo_END_CLASS (FormantListWithHistory)	
#undef ooSTRUCT

/* End of file FormantList_def.h */	
