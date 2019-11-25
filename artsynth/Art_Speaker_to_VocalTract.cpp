/* Art_Speaker_to_VocalTract.cpp
 *
 * Copyright (C) 1992-2005,2008,2011,2015-2017,2019 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Delta.h"
#include "Speaker_to_Delta.h"
#include "Art_Speaker_Delta.h"
#include "Art_Speaker_to_VocalTract.h"

autoVocalTract Art_Speaker_to_VocalTract (Art art, Speaker speaker) {
	autoDelta delta = Speaker_to_Delta (speaker);
	Art_Speaker_intoDelta (art, speaker, delta.get());
	double area [300];
	constexpr double sectionLength = 0.001;   // one millimetre
	integer numberOfSections = 0;
	for (integer isection = 1; isection <= 27; isection ++) {
		Delta_Tube tube = & delta -> tubes [37 + isection];
		integer numberOfConstantSections = Melder_iround (tube -> Dxeq / sectionLength);
		double constantArea = tube -> Dyeq * tube -> Dzeq;
		for (integer jsection = 1; jsection <= numberOfConstantSections; jsection ++)
			area [++ numberOfSections] = constantArea;
	}
	autoVocalTract thee = VocalTract_create (numberOfSections, sectionLength);
	for (integer isection = 1; isection <= numberOfSections; isection ++)
		thy z [1] [isection] = area [isection];
	return thee;
}

/* End of file Art_Speaker_to_VocalTract.cpp */
