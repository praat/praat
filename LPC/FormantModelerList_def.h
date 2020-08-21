/*FormantModelerList_def.h
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

#define ooSTRUCT FormantModelerListDrawingSpecification
oo_DEFINE_CLASS (FormantModelerListDrawingSpecification, Daata)
	oo_INTEGER (numberOfModelers)
	oo_INTEGER (numberOfModelersToDraw)
	oo_INTVEC (drawingOrder, numberOfModelers)
	oo_INTEGER (selectedCandidate)
	oo_DOUBLE (boxLineWidth) // {3.0};
	oo_INTVEC (showOrder, numberOfModelers)
	oo_STRING_VECTOR (midTopText, numberOfModelers)
	
	#if oo_DECLARING
		MelderColour oddFormantColour; // {Melder_RED};
		MelderColour evenFormantColour; // {Melder_MAROON};
		MelderColour selectedCandidateColour; // {Melder_RED}
		MelderColour midTopText_colour; // {Melder_BLUE};
	#endif

	#if oo_DECLARING
		void v_info () override { }
	#endif
	
oo_END_CLASS (FormantModelerListDrawingSpecification)
#undef ooSTRUCT

#define ooSTRUCT FormantModelerList
oo_DEFINE_CLASS (FormantModelerList, Function)
	
	oo_INTEGER (numberOfModelers)
	oo_INTEGER (numberOfTracksPerModel)
	oo_DOUBLE (varianceExponent)
	oo_INTVEC (numberOfParametersPerTrack, numberOfTracksPerModel)
	oo_COLLECTION_OF (OrderedOf, formantModelers, FormantModeler, 0)
	
	#if oo_DECLARING || oo_DESTROYING
	
		oo_OBJECT (FormantModelerListDrawingSpecification, 0, drawingSpecification)
	
	#endif
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FormantModelerList)
#undef ooSTRUCT

/* End of file FormantModelerList_def.h */
