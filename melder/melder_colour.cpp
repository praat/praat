/* melder_colour.cpp
 *
 * Copyright (C) 2019,2020 Paul Boersma
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

#include "melder.h"

MelderColour
	Melder_BLACK,
	Melder_WHITE = MelderColour (1.0),
	Melder_RED = MelderColour (0.865, 0.034, 0.026),
	Melder_GREEN = MelderColour (0.000, 0.500, 0.069),
	Melder_BLUE = MelderColour (0.000, 0.000, 0.828),
	Melder_CYAN = MelderColour (0.009, 0.669, 0.918),
	Melder_MAGENTA = MelderColour (0.949, 0.033, 0.519),
	Melder_YELLOW = MelderColour (0.984, 0.951, 0.020),
	Melder_MAROON = MelderColour (0.5, 0.0, 0.0),
	Melder_LIME = MelderColour (0.0, 1.0, 0.0),
	Melder_NAVY = MelderColour (0.0, 0.0, 0.5),
	Melder_TEAL = MelderColour (0.0, 0.5, 0.5),
	Melder_PURPLE = MelderColour (0.5, 0.0, 0.5),
	Melder_OLIVE = MelderColour (0.5, 0.5, 0.0),
	Melder_PINK = MelderColour (1.0, 0.75, 0.75),
	Melder_SILVER = MelderColour (0.75),
	Melder_GREY = MelderColour (0.5),
	Melder_WINDOW_BACKGROUND_COLOUR = MelderColour (0.90, 0.90, 0.85);

conststring32 MelderColour_nameRGB (MelderColour colour) {
	static MelderString buffer;
	MelderString_copy (& buffer,
		U"{", Melder_fixed (colour. red, 6),
		U",", Melder_fixed (colour. green, 6),
		U",", Melder_fixed (colour. blue, 6),
		U"}"
	);
	return buffer.string;
}
conststring32 MelderColour_namePrettyOrNull (MelderColour colour) {
	return
		MelderColour_equal (colour, Melder_BLACK) ? U"black" :
		MelderColour_equal (colour, Melder_WHITE) ? U"white" :
		MelderColour_equal (colour, Melder_RED) ? U"red" :
		MelderColour_equal (colour, Melder_GREEN) ? U"green" :
		MelderColour_equal (colour, Melder_BLUE) ? U"blue" :
		MelderColour_equal (colour, Melder_CYAN) ? U"cyan" :
		MelderColour_equal (colour, Melder_MAGENTA) ? U"magenta" :
		MelderColour_equal (colour, Melder_YELLOW) ? U"yellow" :
		MelderColour_equal (colour, Melder_MAROON) ? U"maroon" :
		MelderColour_equal (colour, Melder_LIME) ? U"lime" :
		MelderColour_equal (colour, Melder_NAVY) ? U"navy" :
		MelderColour_equal (colour, Melder_TEAL) ? U"teal" :
		MelderColour_equal (colour, Melder_PURPLE) ? U"purple" :
		MelderColour_equal (colour, Melder_OLIVE) ? U"olive" :
		MelderColour_equal (colour, Melder_SILVER) ? U"silver" :
		MelderColour_equal (colour, Melder_GREY) ? U"grey" :
		nullptr;
}
conststring32 MelderColour_name (MelderColour colour) {
	conststring32 result = MelderColour_namePrettyOrNull (colour);
	if (result)
		return result;
	return MelderColour_nameRGB (colour);
}

MelderColour MelderColour_fromColourName (conststring32 colourName) {
	const char32 *p = & colourName [0];
	Melder_skipHorizontalOrVerticalSpace (& p);
	if (Melder_equ_caseInsensitive (p, U"black"))   return Melder_BLACK;
	if (Melder_equ_caseInsensitive (p, U"white"))   return Melder_WHITE;
	if (Melder_equ_caseInsensitive (p, U"red"))     return Melder_RED;
	if (Melder_equ_caseInsensitive (p, U"green"))   return Melder_GREEN;
	if (Melder_equ_caseInsensitive (p, U"blue"))    return Melder_BLUE;
	if (Melder_equ_caseInsensitive (p, U"yellow"))  return Melder_YELLOW;
	if (Melder_equ_caseInsensitive (p, U"cyan"))    return Melder_CYAN;
	if (Melder_equ_caseInsensitive (p, U"magenta")) return Melder_MAGENTA;
	if (Melder_equ_caseInsensitive (p, U"maroon"))  return Melder_MAROON;
	if (Melder_equ_caseInsensitive (p, U"lime"))    return Melder_LIME;
	if (Melder_equ_caseInsensitive (p, U"navy"))    return Melder_NAVY;
	if (Melder_equ_caseInsensitive (p, U"teal"))    return Melder_TEAL;
	if (Melder_equ_caseInsensitive (p, U"purple"))  return Melder_PURPLE;
	if (Melder_equ_caseInsensitive (p, U"olive"))   return Melder_OLIVE;
	if (Melder_equ_caseInsensitive (p, U"pink"))    return Melder_PINK;
	if (Melder_equ_caseInsensitive (p, U"silver"))  return Melder_SILVER;
	if (Melder_equ_caseInsensitive (p, U"grey"))    return Melder_GREY;
	if (Melder_equ_caseInsensitive (p, U"gray"))    return Melder_GREY;
	MelderColour result;
	result. red = result. green = result. blue = undefined;   // invalid
	return result;
}

MelderColour MelderColour_fromNumberString (conststring32 numberString) {
	MelderColour result;
	result. red = result. green = result. blue =
			Melder_clipped (0.0, Melder_atof (numberString), 1.0);
	result. transparency = 0.0;
	return result;
}

MelderColour MelderColour_fromRGBString (conststring32 rgbString) {
	MelderColour result;
	const char32 *p = & rgbString [0];
	Melder_skipHorizontalOrVerticalSpace (& p);
	if (*p != U'{') {
		result. red = result. green = result. blue = undefined;   // invalid
		return result;
	}
	result. red = Melder_clipped (0.0, Melder_atof (++ p), 1.0);
	p = str32chr (p, U',');
	if (! p) {
		result. green = result. blue = result. red;   // construct a grey value
		result. transparency = 0.0;
		return result;
	}
	result. green = Melder_clipped (0.0, Melder_atof (++ p), 1.0);
	p = str32chr (p, U',');
	if (! p) {
		result. blue = 0.5;   // weird case with only red and green
		result. transparency = 0.0;
		return result;
	}
	result. blue = Melder_clipped (0.0, Melder_atof (++ p), 1.0);
	result. transparency = 0.0;
	return result;
}

MelderColour MelderColour_fromColourNameOrNumberStringOrRGBString (conststring32 string) {
	const char32 *p = & string [0];
	Melder_skipHorizontalOrVerticalSpace (& p);
	char32 first = *p;
	if (first == U'{')
		return MelderColour_fromRGBString (p);
	if (first == U'+' || first == U'-' || Melder_isAsciiDecimalNumber (first))
		return MelderColour_fromNumberString (p);
	return MelderColour_fromColourName (p);
}

MelderColour MelderColour_fromColourNameOrRGBString (conststring32 string) {
	MelderColour result;
	const char32 *p = & string [0];
	Melder_skipHorizontalOrVerticalSpace (& p);
	char32 first = *p;
	if (first == U'{')
		return MelderColour_fromRGBString (p);
	return MelderColour_fromColourName (p);
}

MelderColour::MelderColour (constVEC const& vec) {
	if (vec.size == 0) {
		*this = Melder_GREY;
		return;
	}
	if (vec.size == 1) {
		our red = our green = our blue = Melder_clipped (0.0, vec [1], 1.0);
		our transparency = 0.0;
		return;
	}
	if (vec.size == 2) {   // weird case with only red and green
		our red = Melder_clipped (0.0, vec [1], 1.0);
		our green = Melder_clipped (0.0, vec [2], 1.0);
		our blue = 0.0;
		our transparency = 0.0;
		return;
	}
	our red = Melder_clipped (0.0, vec [1], 1.0);
	our green = Melder_clipped (0.0, vec [2], 1.0);
	our blue = Melder_clipped (0.0, vec [3], 1.0);
	our transparency = ( vec.size >= 4 ? Melder_clipped (0.0, vec [4], 1.0) : 0.0 );
}

constexpr int theNumberOfCyclingColours = 10;
static MelderColour theCyclingBackgroundColours [theNumberOfCyclingColours] = {
	Melder_GREEN, Melder_SILVER, Melder_BLUE, Melder_YELLOW, Melder_RED,
	Melder_CYAN, Melder_MAROON, Melder_LIME, Melder_TEAL, Melder_MAGENTA
};
static MelderColour theCyclingTextColours [theNumberOfCyclingColours] = {
	Melder_WHITE, Melder_BLACK, Melder_WHITE, Melder_BLACK, Melder_WHITE,
	Melder_BLACK, Melder_WHITE, Melder_BLACK, Melder_WHITE, Melder_BLACK
};
MelderColour Melder_cyclingBackgroundColour (integer category) {
	return theCyclingBackgroundColours [(category - 1) % theNumberOfCyclingColours];
}
MelderColour Melder_cyclingTextColour (integer category) {
	return theCyclingTextColours [(category - 1) % theNumberOfCyclingColours];
}

/* End of file melder_colour.cpp */
