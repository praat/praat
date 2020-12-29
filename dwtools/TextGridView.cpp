/* TextGridView.cpp
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

#include "TextGridView.h"
#include "NUM2.h"


Thing_implement (TextGridView, TextGrid, 0);

void structTextGridView :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:", xmin, U" to ", xmax, U" seconds");
}


void TextGridView_setDefaultView (TextGridView me) {
	my tierNumbers.resize (my origin -> tiers -> size);
	my tiers -> size = 0;
	for (integer itier = 1; itier <= my origin -> tiers -> size; itier ++) {
		const Function anyTier = my origin -> tiers -> at [itier];
		my tiers -> _insertItem_ref (anyTier, itier);
		my tierNumbers [itier] = itier;
	}
}

integer TextGridView_getViewTierNumber (TextGridView me, integer originTierNumber) {
	if (originTierNumber < 1 || originTierNumber > my origin -> tiers -> size)
		return 0;
	for (integer inum = 1; inum <= my tierNumbers.size; inum ++)
		if (my tierNumbers [inum] == originTierNumber)
			return inum;
	return 0;
}

bool TextGridView_isDefaultView (TextGridView me) {
	if (my tiers -> size != my origin -> tiers -> size)
		return false;
	for (integer itier = 1; itier <= my tiers -> size; itier ++)
		if (my tierNumbers [itier] != itier)
			return false;
	return true;
}

autoTextGridView TextGridView_create (TextGrid me) {
	try {
		autoTextGridView thee = Thing_new (TextGridView);
		thy tiers = FunctionList_create ();
		thy tiers -> _initializeOwnership (false);
		thy xmin = my xmin;
		thy xmax = my xmax;
		thy origin = me;
		TextGridView_setDefaultView (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"TextGridView not created.");
	}	
}

autoTextGrid TextGridView_to_TextGrid (TextGridView me) {
	autoTextGrid thee = TextGrid_createWithoutTiers (my xmin, my xmax);
	for (integer itier = 1; itier <= my tiers -> size; itier ++) {
		autoFunction tierCopy = Data_copy (my tiers -> at [itier]);
		thy tiers ->  addItem_move (tierCopy.move());
	}
	return thee;
}

void TextGridView_checkNewView (TextGridView me, constINTVEC const& newTierNumbers) {
	const integer size = my origin -> tiers -> size;
	const integer min = NUMmin (newTierNumbers);
	const integer max = NUMmax (newTierNumbers);
	Melder_require (min > 0,
		U"A tier number should be positive.");
	Melder_require (max <= my origin -> tiers -> size,
		U"A tier number should not exceed ", size, U" (=the number of tiers in the original TextGrid).");
}

void TextGridView_modifyView (TextGridView me, constINTVEC const& newTierNumbers) {
	TextGridView_checkNewView (me, newTierNumbers);
	my tierNumbers.resize (newTierNumbers.size);
	my tiers -> size = 0;
	for (integer itier = 1; itier <= newTierNumbers.size; itier ++) {
		const integer originNumber = newTierNumbers [itier];
		const Function anyTier = my origin -> tiers -> at [originNumber];
		my tiers -> _insertItem_ref (anyTier, itier);
		my tierNumbers [itier] = originNumber;
	}
}

void TextGridView_viewAllWithSelectedOnTop (TextGridView me, integer selected) {
	const integer originSize = my origin -> tiers -> size;
	Melder_require (selected >= 0 && selected <= originSize,
		U"The selected tier number should not exceed ", originSize, U".");
	autoINTVEC tierNumbers = to_INTVEC (originSize);
	if (selected > 0) {
		integer selectedPosition = 0;
		for (integer inum = 1; inum <= tierNumbers.size; inum ++)
			if (tierNumbers [inum] == selected) {
				selectedPosition = inum;
				break;
			}
		if (selectedPosition != 1) {
			for (integer inum = selectedPosition; inum > 1; inum --)
				tierNumbers [inum] = tierNumbers [inum - 1];
			tierNumbers [1] = selected;
		}
	}
	TextGridView_modifyView (me, tierNumbers.get());
}

/* End of file TextGridView.cpp */
