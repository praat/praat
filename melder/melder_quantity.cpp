/* melder_quantity.cpp
 *
 * Copyright (C) 2007-2011,2015 Paul Boersma
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

static const conststring32 texts [1+MelderQuantity_NUMBER_OF_QUANTITIES] [4] = {
	{ U"", U"", U"", U"" },
	{ U"Time", U"Time (s)", U"seconds", U"s" },
	{ U"Frequency", U"Frequency (Hz)", U"Hertz", U"Hz" },
	{ U"Frequency", U"Frequency (Bark)", U"Bark", U"Bark" },
	{ U"Distance from glottis", U"Distance from glottis (m)", U"metres", U"m" },	
};

conststring32 MelderQuantity_getText (int quantity) { return texts [quantity] [0]; }
conststring32 MelderQuantity_getWithUnitText (int quantity) { return texts [quantity] [1]; }
conststring32 MelderQuantity_getLongUnitText (int quantity) { return texts [quantity] [2]; }
conststring32 MelderQuantity_getShortUnitText (int quantity) { return texts [quantity] [3]; }

/* End of file melder_quantity.cpp */
