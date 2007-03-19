/* melder_quantity.c
 *
 * Copyright (C) 2007 Paul Boersma
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

/*
 * pb 2007/03/17
 */

#include "melder.h"

static const char * texts [1+MelderQuantity_NUMBER_OF_QUANTITIES] [4] = {
	{ "", "", "", "" },
	{ "Time", "Time (s)", "seconds", "s" },
	{ "Frequency", "Frequency (Hz)", "Hertz", "Hz" },
	{ "Frequency", "Frequency (Bark)", "Bark", "Bark" },
	{ "Distance from glottis", "Distance from glottis (m)", "metres", "m" },	
};

const char * MelderQuantity_getText (int quantity) { return texts [quantity] [0]; }
const char * MelderQuantity_getWithUnitText (int quantity) { return texts [quantity] [1]; }
const char * MelderQuantity_getLongUnitText (int quantity) { return texts [quantity] [2]; }
const char * MelderQuantity_getShortUnitText (int quantity) { return texts [quantity] [3]; }

/* End of file melder_quantity.c */
