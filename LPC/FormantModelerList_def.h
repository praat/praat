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
	oo_INTEGER (numberOfModelsToDraw)
	oo_INTVEC (drawingOrder, numberOfModelers)
	oo_INTEGER (selected)
	oo_INTEGER (special)
	oo_DOUBLE (boxLineWidth) // {3.0};
	oo_BOOLEAN (markOutdated)
	oo_INTVEC (showOrder, numberOfModelers)
	oo_STRING_VECTOR (midTopText, numberOfModelers)
	
	#if oo_DECLARING
		MelderColour selected_boxColour; // {Melder_RED};
		MelderColour special_boxColour; // {Melder_BLUE};
		MelderColour default_boxColour; // {Melder_BLACK};
		MelderColour midTopText_colour; // {Melder_BLUE};
	#endif

	#if oo_DECLARING
		void v_info () { };
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
