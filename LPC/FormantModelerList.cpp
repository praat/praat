/*FormantModelerList.cpp
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

#include "FormantModelerList.h"

autoINTVEC newINTVECfromString (conststring32 string) {
	autoVEC reals = newVECfromString (string);
	autoINTVEC result = newINTVECraw (reals.size);
	for (integer i = 1; i <= reals.size; i ++) {
		result [i]  = reals [i];
	}
	integer last = reals.size;
	while (result [last] == 0 && last > 0)
		last --;
	Melder_require (last > 0,
		U"There must be at least one integer in the list");
	result.resize (last);
	return result;
}

Thing_implement (FormantModelerList, Function, 0);

structFormantModelerList_drawingSpecification defaultSpecification = { 2.0, Graphics_DRAWN, Melder_BLACK, Melder_dup(U""), Melder_BLACK };

autovector<structFormantModelerList_drawingSpecification> FormantModelerList_getDefaultDrawingSpecification (FormantModelerList me) {
	autovector<structFormantModelerList_drawingSpecification> result = newvectorraw<structFormantModelerList_drawingSpecification> (my numberOfModelers);
	for (integer imodel = 1; imodel <= my numberOfModelers; imodel ++) {
		result [imodel].box_lineWidth = defaultSpecification.box_lineWidth;
		result [imodel].box_lineType = defaultSpecification.box_lineType;
		result [imodel].box_colour = defaultSpecification.box_colour;
		result [imodel].midTopText = Melder_dup (defaultSpecification.midTopText.get());
		result [imodel].midTopText_colour = defaultSpecification.midTopText_colour;
	}
	return result;
}

autoFormantModelerList FormantList_to_FormantModelerList (FormantList me, double startTime, double endTime, conststring32 numberOfParametersPerTrack_string) {
	try {
		autoFormantModelerList thee = Thing_new (FormantModelerList);
		thy xmin = startTime;
		thy xmax = endTime;
		autoINTVEC numberOfParametersPerTrack = newINTVECfromString (numberOfParametersPerTrack_string);
		Melder_require (numberOfParametersPerTrack.size > 0 ,
			U"The number of items in the parameter list should be larger than zero.");
		Formant formant = (Formant) my formants.at [1];
		integer maximumNumberOfFormants = formant -> maxnFormants;
		Melder_require (numberOfParametersPerTrack.size <= maximumNumberOfFormants,
			U"The number of items cannot exceed the maximum number of formants (", maximumNumberOfFormants, U").");
		thy numberOfTracksPerModel = thy numberOfParametersPerTrack.size;
		integer numberOfZeros = 0;
		for (integer ipar = 1; ipar <= thy numberOfParametersPerTrack.size; ipar ++) {
			const integer value = thy numberOfParametersPerTrack [ipar];
			Melder_require (value >= 0,
				U"Numbers in the 'Number of parameter list' should be positive.");
			if (value == 0)
				numberOfZeros += 1;
		}
		thy numberOfParametersPerTrack = numberOfParametersPerTrack.move();
		thy numberOfTracksPerModel = thy numberOfParametersPerTrack.size;
		thy numberOfModelers = my formants . size;
		for (integer imodel = 1; imodel <= thy numberOfModelers; imodel ++) {
			Formant formanti = (Formant) my formants.at [imodel];
			autoFormantModeler fm = Formant_to_FormantModeler (formanti, startTime, endTime,  thy numberOfParametersPerTrack.get());
			Thing_setName (fm.get(), my formantIdentifier [imodel].get());
			thy formantModelers. addItem_move (fm.move());
		}
		thy selected = newINTVEClinear (thy numberOfModelers, 1, 1);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantModelerList not created.");
	}
}

/* End of file FormantModelerList.cpp */
