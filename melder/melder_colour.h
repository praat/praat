#ifndef _melder_colour_h_
#define _melder_colour_h_
/* melder_colour.h
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

struct MelderColour {
	double red, green, blue, transparency;
	explicit MelderColour () :
		red (0.0), green (0.0), blue (0.0), transparency (0.0) {}
	explicit MelderColour (double greyValue) :
		red (Melder_clipped (0.0, greyValue, 1.0)), green (red), blue (red), transparency (0.0) {}
	explicit MelderColour (double initialRed, double initialGreen, double initialBlue) :
		red (initialRed), green (initialGreen), blue (initialBlue), transparency (0.0) {}
	explicit MelderColour (double initialRed, double initialGreen, double initialBlue, double initialTransparency) :
		red (initialRed), green (initialGreen), blue (initialBlue), transparency (initialTransparency) {}
	explicit MelderColour (constVEC const& vec);
	bool valid () const {
		return isdefined (our red);
	}
	bool isGrey () const {
		return our red == our green && our green == our blue;
	}
};

extern MelderColour Melder_BLACK, Melder_WHITE, Melder_RED, Melder_GREEN, Melder_BLUE,
	Melder_CYAN, Melder_MAGENTA, Melder_YELLOW, Melder_MAROON, Melder_LIME, Melder_NAVY, Melder_TEAL,
	Melder_PURPLE, Melder_OLIVE, Melder_PINK, Melder_SILVER, Melder_GREY, Melder_WINDOW_BACKGROUND_COLOUR;

MelderColour MelderColour_fromColourName (conststring32 colourName);
/*
	If `colourName` is one of the 16 standard names (case-insensitively),
	then return that colour. If not, then result.valid() will return false.
*/

MelderColour MelderColour_fromNumberString (conststring32 numberString);

MelderColour MelderColour_fromRGBString (conststring32 rgbString);

MelderColour MelderColour_fromColourNameOrNumberStringOrRGBString (conststring32 string);

MelderColour MelderColour_fromColourNameOrRGBString (conststring32 string);

conststring32 MelderColour_nameRGB (MelderColour colour);
conststring32 MelderColour_namePrettyOrNull (MelderColour colour);
conststring32 MelderColour_name (MelderColour colour);

static inline bool MelderColour_equal (MelderColour colour1, MelderColour colour2) {
	return
		colour1. red == colour2. red &&
		colour1. green == colour2. green &&
		colour1. blue == colour2. blue &&
		colour1. transparency == colour2. transparency;
}
MelderColour Melder_cyclingBackgroundColour (integer category);
MelderColour Melder_cyclingTextColour (integer category);

/* End of file melder_colour.h */
#endif
