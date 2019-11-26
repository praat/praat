/* FormantGrid_extensions.cpp
 *
 * Copyright (C) 2009-2019 David Weenink, 2015 Paul Boersma
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

#include "FormantGrid_extensions.h"
#include "NUM2.h"

void FormantGrid_draw (FormantGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax,
	bool bandwidths, bool garnish, conststring32 method)
{
	OrderedOf<structRealTier>* tiers = ( bandwidths ? & my bandwidths : & my formants );
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	if (ymax <= ymin) {
		ymin = 0.0;
		ymax = ( bandwidths ? 1000.0 : 8000.0 );
	}
	for (integer iformant = 1; iformant <= tiers->size; iformant ++) {
		conststring32 quantity = nullptr;
		bool garnish2 = false;
		const RealTier tier = tiers->at [iformant];
		if (iformant == my formants.size) {
			quantity = U"Frequency (Hz)";
			if (garnish)
				garnish2 = true;
		}
		RealTier_draw (tier, g, xmin, xmax, ymin, ymax, garnish2, method, quantity);
	}
}

static void FormantGrid_removeFormantTier (FormantGrid me, integer position) {
	if (position < 1 || position > my formants.size)
		return;
	my formants. removeItem (position);
}

static void FormantGrid_removeBandwidthTier (FormantGrid me, integer position) {
	if (position < 1 || position > my bandwidths.size)
		return;
	my bandwidths. removeItem (position);
}

void FormantGrid_removeFormantAndBandwidthTiers (FormantGrid me, integer position) {
	FormantGrid_removeFormantTier (me, position);
	FormantGrid_removeBandwidthTier (me, position);
}

static void FormantGrid_addFormantTier (FormantGrid me, integer position) {
	if (position > my formants.size || position < 1)
		position = my formants.size + 1;
	autoRealTier rt = RealTier_create (my xmin, my xmax);
	my formants. addItemAtPosition_move (rt.move(), position);
}

static void FormantGrid_addBandwidthTier (FormantGrid me, integer position) {
	if (position > my bandwidths.size || position < 1)
		position = my bandwidths.size + 1;
	autoRealTier rt = RealTier_create (my xmin, my xmax);
	my bandwidths. addItemAtPosition_move (rt.move(), position);
}

void FormantGrid_addFormantAndBandwidthTiers (FormantGrid me, integer position) {
	try {
		Melder_require (my formants.size == my bandwidths.size,
			U"Number of formants and bandwidths should be equal.");
		
		if (position > my formants.size || position < 1)
			position = my formants.size + 1;
		FormantGrid_addFormantTier (me, position);
		try {
			FormantGrid_addBandwidthTier (me, position);
		} catch (MelderError) {
			FormantGrid_removeFormantTier (me, position);
			throw;
		}
	} catch (MelderError) {
		Melder_throw (me, U": no ties added.");
	}
}

/* End of file FormantGrid_extensions.cpp */
