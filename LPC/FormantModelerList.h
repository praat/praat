#ifndef _FormantModelerList_h_
#define _FormantModelerList_h_
/*FormantModelerList.h
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

#include "Collection.h"
#include "Function.h"
#include "FormantModeler.h"
#include "FormantList.h"
#include "Graphics.h"

Thing_define (FormantModelerList, Function) {
	integer numberOfModelers, numberOfTracksPerModel;
	double varianceExponent;
	autoINTVEC numberOfParametersPerTrack; // .size == (numberOfFormantTracks)
	OrderedOf<structFormantModeler> formantModelers;
	autoINTVEC selected; // models are displayed in this order
};

typedef struct FormantModelerList_drawingSpecification {
	double box_lineWidth;
	int box_lineType;
	MelderColour box_colour;
	autostring32 midTopText;
	MelderColour midTopText_colour;
} structFormantModelerList_drawingSpecification;

autovector<structFormantModelerList_drawingSpecification> FormantModelerList_getDefaultDrawingSpecification (FormantModelerList me);

autoFormantModelerList FormantList_to_FormantModelerList (FormantList me, double startTime, double endTime, conststring32 numberOfParametersPerTrack_string);


#endif /* _FormantModelerList_h_ */
