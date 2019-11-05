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
	double red, green, blue;
};

extern MelderColour Melder_BLACK, Melder_WHITE, Melder_RED, Melder_GREEN, Melder_BLUE,
	Melder_CYAN, Melder_MAGENTA, Melder_YELLOW, Melder_MAROON, Melder_LIME, Melder_NAVY, Melder_TEAL,
	Melder_PURPLE, Melder_OLIVE, Melder_PINK, Melder_SILVER, Melder_GREY, Graphics_WINDOW_BACKGROUND_COLOUR;
conststring32 MelderColour_name (MelderColour colour);
static inline bool MelderColour_equal (MelderColour colour1, MelderColour colour2) {
	return colour1. red == colour2. red && colour1. green == colour2. green && colour1. blue == colour2. blue;
}
MelderColour Melder_cyclingBackgroundColour (integer category);
MelderColour Melder_cyclingTextColour (integer category);

/* End of file melder_colour.h */
#endif
