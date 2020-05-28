#ifndef _TextGridView_h_
#define _TextGridView_h_
/* TextGridView.h
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

#include "TextGrid.h"
#include "melder.h"

/*
	A TextGridView offers a view on a TextGrid.
	With a TexGridView you can determine _which_ tiers of a TextGrid are exposed and in _what_ order.
	There is no limit on how often you can have a tier exposed. The array 'tierNumbers' logs which tiers
	and in what order they are exposed.
	Suppose you have a TextGrid with three tiers and 'tierNumbers' is an array with two elements,
	say 3 and 1, the TextGridView behaves like a TextGrid with only two tiers. Its first tier is the original
	TextGrid's tier 3 and its second tier is the original TextGrid's tier number 1.
	If 'tierNumbers' has five elements, for example 1, 2, 3, 2 and 1, then The TextGridView behaves as if it had
	5 tiers. Its tier number 1 and 5 are both equal to the original TextGrid's tier number 1, etc.
	The TextGridView can be destroyed at any time because it only contains links to the original TextGrid.
*/
Thing_define (TextGridView, TextGrid) {
	TextGrid origin;
	autoINTVEC tierNumbers;
	void v_info ()
		override;
};


void TextGridView_setDefaultView (TextGridView me);

integer TextGridView_getViewTierNumber (TextGridView me, integer originTierNumber);

static inline integer TextGridView_getOriginTierNumber (TextGridView me, integer viewTierNumber) {
	if (viewTierNumber < 1 || viewTierNumber > my tiers -> size)
		return 0;
	return my tierNumbers [viewTierNumber];
}

bool TextGridView_isDefaultView (TextGridView me);

static inline bool TextGridView_hasTierInView (TextGridView me, integer tierNumber) {
	return TextGridView_getViewTierNumber (me, tierNumber) != 0;
}

autoTextGridView TextGridView_create (TextGrid me);

autoTextGrid TextGridView_to_TextGrid (TextGridView me);

void TextGridView_checkNewView (TextGridView me, constINTVEC const& newTierNumbers);

void TextGridView_modifyView (TextGridView me, constINTVEC const& newTierNumbers);

void TextGridView_viewAllWithSelectedOnTop (TextGridView me, integer selected);


#endif /* _TextGridView_h_ */
