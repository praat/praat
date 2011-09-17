/* FormantGrid_extensions.c
 *
 * Copyright (C) 2009-2011 David Weenink
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
 djmw 20090310
*/

#include "FormantGrid_extensions.h"
#include "NUM2.h"

void FormantGrid_draw (FormantGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool bandwidths, bool garnish, const wchar_t *method) {
	Ordered tiers = bandwidths ? my bandwidths : my formants;

	if (xmax <= xmin) {
		xmin = my xmin; xmax = my xmax;
	}
	if (ymax <= ymin) {
		ymin = 0; ymax = bandwidths ? 1000 : 8000;
	}
	for (long iformant = 1; iformant <= my formants -> size; iformant++) {
		const wchar_t *quantity = 0;
		bool garnish2 = false;
		RealTier tier = (RealTier) tiers -> item[iformant];
		if (iformant == my formants -> size) {
			quantity = L"Frequency (Hz)";
			if (garnish) {
				garnish2 = true;
			}
		}
		RealTier_draw (tier, g, xmin, xmax, ymin, ymax, garnish2, method, quantity);
	}
}

static void FormantGrid_removeFormantTier (FormantGrid me, int position) {
	if (position < 1 || position > my formants -> size) {
		return;
	}
	Collection_removeItem (my formants, position);
}

static void FormantGrid_removeBandwidthTier (FormantGrid me, int position) {
	if (position < 1 || position > my bandwidths -> size) {
		return;
	}
	Collection_removeItem (my bandwidths, position);
}

void FormantGrid_removeFormantAndBandwidthTiers (FormantGrid me, int position) {
	FormantGrid_removeFormantTier (me, position);
	FormantGrid_removeBandwidthTier (me, position);
}

static void FormantGrid_addFormantTier (FormantGrid me, int position) {
	if (position > my formants -> size || position < 1) {
		position = my formants -> size + 1;
	}
	autoRealTier rt = RealTier_create (my xmin, my xmax);
	Ordered_addItemPos (my formants, rt.transfer(), position);
}

static void FormantGrid_addBandwidthTier (FormantGrid me, int position) {
	if (position > my bandwidths -> size || position < 1) {
		position = my bandwidths -> size + 1;
	}
	autoRealTier rt = RealTier_create (my xmin, my xmax);
	Ordered_addItemPos (my bandwidths, rt.transfer(), position);
}

void FormantGrid_addFormantAndBandwidthTiers (FormantGrid me, int position) {
	try {
		if (my formants -> size != my bandwidths -> size) {
			Melder_throw ("Number of formants and bandwidths must be equal.");
		}
		if (position > my formants -> size || position < 1) {
			position = my formants -> size + 1;
		}
		FormantGrid_addFormantTier (me, position);
		try {
			FormantGrid_addBandwidthTier (me, position);
		} catch (MelderError) {
			FormantGrid_removeFormantTier (me, position);
			throw MelderError ();
		}
	} catch (MelderError) {
		Melder_throw (me, ": no ties added.");
	}
}

/* End of file FormantGrid_extensions.cpp */
