/* Art_Speaker_to_VocalTract.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "Delta.h"
#include "Speaker_to_Delta.h"
#include "Art_Speaker_Delta.h"
#include "Art_Speaker_to_VocalTract.h"

VocalTract Art_Speaker_to_VocalTract (Art art, Speaker speaker)
{
	VocalTract thee = NULL;
	long isection;
	double area [300];
	int numberOfSections;
	double sectionLength = 0.001;   /* One millimetre. */
	Delta delta = NULL;

	delta = Speaker_to_Delta (speaker); if (! delta) goto end;
	Art_Speaker_intoDelta (art, speaker, delta);
	numberOfSections = 0;
	for (isection = 1; isection <= 27; isection ++)
	{
		Delta_Tube tube = delta -> tube + 37 + isection;
		int numberOfConstantSections = floor (tube -> Dxeq / sectionLength + 0.5);
		double constantArea = tube -> Dyeq * tube -> Dzeq;
		int jsection; for (jsection = 1; jsection <= numberOfConstantSections; jsection ++)
			area [++ numberOfSections] = constantArea;
	}
	thee = VocalTract_create (numberOfSections, sectionLength); if (! thee) goto end;
	for (isection = 1; isection <= numberOfSections; isection ++)
		thy z [1] [isection] = area [isection];

end:
	forget (delta);
	return thee;
}

/* End of file Art_Speaker_to_VocalTract.cpp */
