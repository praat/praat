/* melder_colour.cpp
 *
 * Copyright (C) 2019 Paul Boersma
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
	Melder_BLACK = { 0.0, 0.0, 0.0 },
	Melder_WHITE = { 1.0, 1.0, 1.0 },
	Melder_RED = { 0.865, 0.034, 0.026 },
	Melder_GREEN = { 0.000, 0.500, 0.069 },
	Melder_BLUE = { 0.000, 0.000, 0.828 },
	Melder_CYAN = { 0.009, 0.669, 0.918 },
	Melder_MAGENTA = { 0.949, 0.033, 0.519 },
	Melder_YELLOW = { 0.984, 0.951, 0.020 },
	Melder_MAROON = { 0.5, 0.0, 0.0 },
	Melder_LIME = { 0.0, 1.0, 0.0 },
	Melder_NAVY = { 0.0, 0.0, 0.5 },
	Melder_TEAL = { 0.0, 0.5, 0.5 },
	Melder_PURPLE = { 0.5, 0.0, 0.5 },
	Melder_OLIVE = { 0.5, 0.5, 0.0 },
	Melder_PINK = { 1.0, 0.75, 0.75 },
	Melder_SILVER = { 0.75, 0.75, 0.75 },
	Melder_GREY = { 0.5, 0.5, 0.5 },
	Graphics_WINDOW_BACKGROUND_COLOUR = { 0.90, 0.90, 0.85 };

inline static conststring32 rgbColourName (MelderColour colour) {
	static MelderString buffer { };
	MelderString_copy (& buffer,
		U"{", Melder_fixed (colour. red, 6),
		U",", Melder_fixed (colour. green, 6),
		U",", Melder_fixed (colour. blue, 6),
		U"}"
	);
	return buffer.string;
}
conststring32 MelderColour_name (MelderColour colour) {
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
		rgbColourName (colour);
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
